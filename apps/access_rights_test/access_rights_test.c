/*
 * Temporary negative test for the TA->secure-kernel pointer-access guard.
 *
 * tee_svc_copy_to_user/copy_from_user and the crypto syscalls
 * (utee_hash_update/final, utee_cipher_init, cipher update, copy_in_attrs)
 * validate TA-supplied pointer ranges through tee_mmu_check_access_rights()
 * before dereferencing them. This test drives that guard directly with
 * malformed ranges and asserts they are rejected with TEE_ERROR_ACCESS_DENIED,
 * and that a normal in-range buffer is still accepted.
 *
 * The guard is exercised directly rather than through a syscall such as
 * utee_hash_update(). At boot there is no active TA session, and in this port
 * tee_ta_get_current_session() does not fail in that case - it returns
 * TEE_SUCCESS with a NULL current_session - so a session-dependent syscall
 * would fault on the NULL session rather than reach the range check. Calling
 * the guard directly tests exactly the logic under test with no such
 * dependency.
 *
 * This app is not part of the product; remove it once the fix is verified.
 * Gated behind CONFIG_APPS_ACCESS_RIGHTS_TEST.
 */

#include <stdint.h>
#include <tee_api_types.h>
#include <tee/tee_svc.h>
#include "printf.h"

static int check(const char *name, TEE_Result got, TEE_Result expected)
{
	if (got == expected) {
		printf("  [PASS] %s -> 0x%08x\n", name, (unsigned int)got);
		return 0;
	}
	printf("  [FAIL] %s -> 0x%08x (expected 0x%08x)\n", name,
	       (unsigned int)got, (unsigned int)expected);
	return 1;
}

void tee_access_rights_test(void)
{
	int fails = 0;
	uint32_t rd = TEE_MEMORY_ACCESS_READ | TEE_MEMORY_ACCESS_ANY_OWNER;
	uint32_t wr = TEE_MEMORY_ACCESS_WRITE | TEE_MEMORY_ACCESS_ANY_OWNER;
	volatile uint32_t valid = 0;

	printf("== access-rights guard negative test ==\n");

	/* NULL buffer with non-zero length must be rejected. */
	fails += check("null+len (read)",
		       tee_mmu_check_access_rights(NULL, rd, 0, 8),
		       TEE_ERROR_ACCESS_DENIED);

	/* A range that wraps the address space must be rejected. */
	fails += check("wrapping range (write)",
		       tee_mmu_check_access_rights(NULL, wr, (uaddr_t)~0UL, 2),
		       TEE_ERROR_ACCESS_DENIED);

	/* Zero-length range is trivially accepted. */
	fails += check("zero length",
		       tee_mmu_check_access_rights(NULL, rd, 0, 0),
		       TEE_SUCCESS);

	/* A normal in-range buffer is still accepted. */
	fails += check("valid buffer",
		       tee_mmu_check_access_rights(NULL, wr, (uaddr_t)&valid,
						   sizeof(valid)),
		       TEE_SUCCESS);

	if (fails == 0)
		printf("== RESULT: all cases handled correctly ==\n");
	else
		printf("== RESULT: %d case(s) FAILED ==\n", fails);
}
