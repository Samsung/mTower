# mTower GP API test suite

# Contents
1. [Overview](#1-overview)
2. [How to run](#2-how-to-run)

## 1. Overview
mTower GP API test suite demonstrates the valid execution of relevant GP TEE API function implementations. As many of the functions depend on each other for successful operation, it is not reasonable to create a separate test for every possible combination of invalid parameters for each function in API, so, while the tests demonstrate behavior of API functions under some error conditions, most of the tests are concerned with correct use of functions, where returned results signal successful execution. Also note that many of the functions in Trusted part of TEE do not have full-fledged error handling mechanisms, as these functions are considered critical - that is, they must succeed, because it is impossible for system to recover if these functions fail. In such functions, error condition typically results in calling TEE_Panic() function, which writes an error message into log and drops the system into infinite loop, essentially hanging it.

**When adding or changing functionality, it is highly advisable to add new tests as part of your contribution.**

## 2. How to run
Start out by following the "Get and build the solution" in the [build.md] file.
> Warning: Need to download the toolchain (exec. `make toolchain`).

To run test, we need to build a special firmware version that includes test cases and their shell program. All the other applications are excluded from build. To build this firmware version, we run

```
make menuconfig
```

shell command, and select Application configuration -> GP API test suite in on-display menu. After saving the configuration and exiting configuration menu, we need to build the firmware by issuing

```
make clean; make
```

command. After firmware is built, procedure of flashing it onto board and running is the same as usual.
Below is an example of test case run results in Normal World console:

```
        -=mTower v0.1=-  Jul 17 2019  12:07:37

+---------------------------------------------+
|     Nonsecure FreeRTOS is running ...       |
+---------------------------------------------+

TEE test: test suite
TEEC_InitializeContext failed with code 0xffff0006
Check TEEC_InitializeContext [OK]
TEEC_Opensession failed with code 0xffff0006 origin 0x1
TEEC_Opensession failed with code 0xffff0006 origin 0x1
TEEC_Opensession failed with code 0xffff0008 origin 0x3
Check TEEC_OpenSession [OK]
Check TEE_Malloc [OK]
Check TA_OpenSessionEntryPoint [OK]
TEEC_InvokeCommand failed 0xffff000a origin 0x2
TEEC_InvokeCommand failed 0xffff0006 origin 0x1
TEEC_InvokeCommand failed 0xffff0006 origin 0x2
TEEC_InvokeCommand failed 0xffff0006 origin 0x2
Check TEEC_InvokeCommand [OK]
Check TA_InvokeCommandEntryPoint [OK]
TEEC_AllocateSharedMemory failed 0xffff0006
TEEC_AllocateSharedMemory failed 0xffff0006
TEEC_AllocateSharedMemory failed 0xffff0006
Check TEEC_AllocateSharedMemory [OK]
Check TEEC_ReleaseSharedMemory [OK]
Failed to close session 0x20000f8c
Session successfully closed 0x20000f88
Check TEEC_CloseSession [OK]
Check TA_CloseSessionEntryPoint [OK]
Check TEE_Free [OK]
Check TEEC_FinalizeContext [OK]
Prepare session with the TA
Prepare encode operation
Check TEE_AllocateOperation [OK]
Check TEE_Malloc [OK]
Check TEE_InitRefAttribute [OK]
Check TEE_AllocateTransientObject [OK]
Check TEE_PopulateTransientObject [OK]
Check TEE_SetOperationKey [OK]
Load key in TA
Check TEE_ResetTransientObject [OK]
Check TEE_ResetOperation [OK]
Reset ciphering operation in TA (provides the initial vector)
Check TEE_CipherInit [OK]
Encode buffer from TA
Check TEE_CipherUpdate [OK]
Session successfully closed 0x20000f88
34 tests successfully completed from 34 total tests
```

