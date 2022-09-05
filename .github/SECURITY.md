# Security policy
1. [Supported Versions](#1-supported-versions)  
2. [Vulnerability Report](#2-vulnerability-report)  
3. [Security Disclosure](#3-security-disclosure)  

---

## 1. Supported Versions

We are releasing patches to eliminate vulnerabilities, you can see below:

| Version     | Supported by | mTower             | 3-rd party component                           |
| ----------- | ------------ | ------------------ | ---------------------------------------------- |
| 0.1.x-0.3.1 | N/A          |                    |                                                |
| 0.4.0       | Fixed        |                    | CVE-2019-1010296                               |
| 0.4.1       | Fixed        |                    | CVE-2022-36621, CVE-2022-36622                 |

---

## 2. Vulnerability Report

The mTower command assigns the highest priority to all security bugs in mTower. We appreciate your efforts and
responsible disclosure of information to eliminate vulnerabilities.

Please report security bugs by emailing the Lead Maintenance Specialist at t.drozdovsky@samsung.com. marked "SECURITY" or create an issue in the repository.
Our team will confirm your request and within 2 weeks will try to prepare recommendations for elimination. Our team will keep you updated on the progress towards the fix until the full announcement of the patch release. During this process, the security team may request additional information or guidance.

---

## 3. Security Disclosure

When a security group receives a security error report as previously mentioned, it is assigned the highest priority and the person in charge. This person will coordinate the patch and release process,
including the following steps:

  * Confirm the problem and identify the affected versions.
  * Check the code to find any similar problems.
  * Prepare fixes for all releases still in maintenance. These fixes will
    released as quickly as possible.

We suggest the following format when disclosing vulnerabilities:

  * Your name and email.
  * Include scope of vulnerability. Let us know who could use this exploit.
  * Document steps to identify the vulnerability. It is important that we can reproduce your findings. 
  * How to exploit vulnerability, give us an attack scenario.
