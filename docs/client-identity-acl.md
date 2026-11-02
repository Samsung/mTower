# Client identity and per-TA access control (ACL) in mTower

This document describes mTower's client-identity model and the hardcoded
per-TA Access Control List (ACL) enforced on the TA session-open path. It is
the authoritative reference for the design decision, its threat-model
limitations, and the operational procedure for changing access rights.

Related external finding: S-0003 (CWE-862 / CWE-306 / CWE-284) — "missing
client-identity / ACL enforcement".

## 1. Threat model

**Design premise (official):** mTower treats the **entire Normal World as a
single trust domain**. The TEE does not provide any inter-client separation
between non-secure applications:

* The Non-Secure ioctl entry (`tee/kernel/entry_std.c`) is a single veneer
  callable by any Normal-World task; there is no per-caller credential
  that the secure world can verify.
* All Normal-World RTOS targets run a flat privileged image (FreeRTOS
  without an NS-MPU configuration), so a "client identity" claim
  (`TEEC_LoginMethod` / `TEEC_UUID` in the open-session request) is
  **self-asserted, attacker-controlled input** — never an authentication
  boundary.
* A session is looked up by its raw id (`find_session()`), which the
  Normal World also knows; ownership of a session therefore cannot be
  proven to the TEE either.

What the secure world *can* enforce is which **login methods** may open a
session to which **TA UUID**. That is exactly what the ACL below does.

### Platform applicability

The single-trust-domain premise holds for all currently supported platforms
(Cortex-M23/M33 and the RISC-V BL808/Pine64 Ox64 in their FreeRTOS
configurations, where the whole Normal World is one image). If a platform
ever runs a Normal-World OS with **process separation** (e.g. Linux with
per-process credentials), the premise breaks and the identity plumbing must
be re-assessed before the login-method ACL can be considered meaningful.

## 2. The hardcoded per-TA ACL

Location: `tee/kernel/tee_ta_manager.c`, table `ta_acl[]`, helper
`ta_acl_allowed()`.

```
struct ta_acl_entry {
        const TEE_UUID ta_uuid;             /* target TA */
        const uint32_t allowed_login_methods; /* bitmask: 1 << login */
};
```

The ACL is evaluated in `tee_ta_open_session()` **before** the TA context
is initialised, and applies to every non-secure client. Decision rules:

1. The client's claimed login method (from `TEEC_OpenSession()`'s
   `connectionMethod`, forwarded by libteec as `arg->clnt_login`) selects
   the bit `1 << login` in the entry's `allowed_login_methods` mask.
2. A TA that has **no entry** in the table is **denied for all non-secure
   clients** (default-deny).
3. Login values outside the GP-defined set
   (`TEEC_LOGIN_PUBLIC/USER/GROUP/APPLICATION/USER_APPLICATION/
   GROUP_APPLICATION`) are denied.
4. Teecore-internal callers (`KERN_IDENTITY`) and TA-to-TA session opens
   (a real `TEE_Identity` with `TEE_LOGIN_TRUSTED_APP`) are **not** subject
   to the table.
5. A denied open returns `TEE_ERROR_ACCESS_DENIED` with origin
   `TEE_ORIGIN_TEE` — no TA code runs.

Current table contents (keep in sync with the source):

| TA | UUID | Allowed logins |
|----|------|----------------|
| hello_world | `8aaaf200-2450-11e4-abe2-0002a5d5c51b` | APPLICATION |
| AES | `5dbac793-f574-4871-8ad3-0433-1ec17f24` | APPLICATION |
| HOTP | `484d4143-2d53-4841-3120-4a6f636b6542` | APPLICATION |
| test | `5dbac744-f574-4871-8ad3-0433-1ec17f24` | PUBLIC |

Bit definitions (`TEE_ACL_LOGIN_*_BIT`) live next to the table. Because
`TEE_LOGIN_PUBLIC == 0`, its bit is `1u << 0`.

## 3. Why the login method is a restriction, not authentication

`TEEC_LOGIN_APPLICATION` is *not* proof that the caller is the intended
application. Any Normal-World code can pass any `connectionMethod` it
wants. The ACL therefore does **not** defend against a malicious Normal
World that is allowed to run arbitrary code — nothing in the TEE trust
model can. What it does provide:

* **Failure-mode containment / hardening:** the generic, zero-effort
  `TEEC_LOGIN_PUBLIC` pattern used by the repository's own demo clients
  and by any casual 3rd-party Normal-World app can no longer reach the
  demo TAs (HOTP secret registration, AES key handling). A caller now has
  to *know* which login method the deployment allows, which is a
  deliberate deployment decision recorded in the secure-world binary.
* **A hook for real deployments:** a product that needs stronger policy
  (e.g. HMAC over `connection_data`, or a per-client UUID allow-list)
  can extend `ta_acl_allowed()` — the plumbing from the NS request up to
  the session manager already exists (`arg->clnt_login` /
  `arg->clnt_uuid` → `TEE_Identity`).

## 4. How to grant or revoke access (hardcoded by design)

To change which clients may open a session to a TA:

1. Edit `ta_acl[]` in `tee/kernel/tee_ta_manager.c`:
   * add a new `{ { UUID }, allowed_login_methods }` entry, or
   * modify the bitmask of an existing entry
     (e.g. `TEE_ACL_LOGIN_APPLICATION_BIT | TEE_ACL_LOGIN_PUBLIC_BIT`),
   * or delete the entry to deny all non-secure clients.
2. Rebuild the **secure world** image and reflash. The table is compiled
   in; the Normal World cannot alter it at runtime.
3. Update the table in section 2 of this document and the corresponding
   CA (client application) source if the login method changed.

Client applications call `TEEC_OpenSession(..., TEEC_LOGIN_APPLICATION,
NULL, ...)` — see `apps/*/ca/*_ns.c` for the updated examples. The demo
`test` TA keeps `TEEC_LOGIN_PUBLIC` for CI convenience.

## 5. Session-ownership checks (check_client)

Independently of the ACL, `check_client()` (restored from upstream
OP-TEE) is enforced on **invoke** and **close**:

* Sessions opened via `NSAPP_IDENTITY` are owned by "the Normal World"
  as a whole; any NS caller may use them (single trust domain).
* Sessions opened by teecore (`KERN_IDENTITY`) are tagged
  `TEE_LOGIN_TRUSTED_APP`; a Normal-World caller attempting to invoke or
  close such a session is rejected
  ("nsec tries to hijack TA session").

## 6. Response to finding S-0003

The factual observations in the report were confirmed: at commit
`102d3dc` the client-identity verification logic in
`tee/kernel/tee_ta_manager.c` was commented out and no ACL mechanism
existed. The consequences were as described: any Normal-World application
could open a session to any TA and invoke its commands. As of this
change:

* client identity (`login` + `uuid`) is carried from the client request
  through the secure-world entry into the session manager,
* session ownership is checked on invoke/close,
* a compile-time, default-deny per-TA ACL gates session open,
* this document records the single-trust-domain premise and the
  hardcoded-ACL policy as an official design decision.
