# mTower functionality description

This file provides an overview of methods comprising mTower API available for use by apps. It contains 3 primary parts: mTower H/W API description, GP TEE API description, and mTower configuration options description.

## 1. mTower H/W API
mTower consists of Secure World and Normal World parts, which interact in a client-server pattern, with Normal World acting as a client and Secure World as a server. Normal World is an application that runs under FreeRTOS operating system. Below is a link to description of FreeRTOS API:
[The FreeRTOS Reference Manual](https://www.freertos.org/Documentation/FreeRTOS_Reference_Manual_V10.0.0.pdf)
SecureWorld is basically just a set of event handlers, and its API is completely defined by developers themselves. Example of API functions is given in GP TEE Internal API table.

## 2. mTower and GP TEE API descriptions
Table below gives a listing of API functions for Normal World. For a detailed description of each function, please see [TEE Client API](specs/gp/TEE_Client_API_Specification-V1.0.pdf).

| **GP TEE Client API**           |**Implemented**| **Comments**              |
|---------------------------------|-----|-------------------------------------|
| TEEC_InitializeContext          | Yes | Covered in test suite               |
| TEEC_FinalizeContext            | Yes | Covered in test suite               |
| TEEC_RegisterSharedMemory       | No  |                                     |
| TEEC_AllocateSharedMemory       | Yes | Covered in test suite               |
| TEEC_ReleaseSharedMemory        | Yes | Covered in test suite               |
| TEEC_OpenSession                | Yes | Covered in test suite               |
| TEEC_CloseSession               | Yes | Covered in test suite               |
| TEEC_InvokeCommand              | Yes | Covered in test suite               |
| TEEC_RequestCancellation        | No  |                                     |
| TEEC_PARAM_TYPES                | Yes | Not tested                          |

A list of Secure World API functions is given in a table below. For a detailed description of each function, please see [TEE Internal API](specs/gp/GPD_TEE_Internal_API_Specification_v1.0.pdf).

| **GP TEE Internal API**         |**Implemented**| **Comments**              |
|---------------------------------|-----|-------------------------------------|
| **Trusted Core Framework**      |     |                                     |
| **TA Interface**                |     |                                     |
| TA_CreateEntryPoint             | Yes | Every TA includes this function replacing <TA> with <TA_name> prefix |
| TA_DestroyEntryPoint            | Yes | Every TA includes this function replacing <TA> with <TA_name> prefix |
| TA_OpenSessionEntryPoint        | Yes | Every TA includes this function replacing <TA> with <TA_name> prefix. Covered in test suite |
| TA_CloseSessionEntryPoint       | Yes | Every TA includes this function replacing <TA> with <TA_name> prefix. Covered in test suite |
| TA_InvokeCommandEntryPoint      | Yes | Every TA includes this function replacing <TA> with <TA_name> prefix. Covered in test suite |
| **Property Access Functions**   |     |                                     |
| TEE_GetPropertyAsString         | No  |                                     |
| TEE_GetPropertyAsBool           | No  |                                     |
| TEE_GetPropertyAsU32            | No  |                                     |
| TEE_GetPropertyAsBinaryBlock    | No  |                                     |
| TEE_GetPropertyAsUUID           | No  |                                     |
| TEE_GetPropertyAsIdentity       | No  |                                     |
| TEE_AllocatePropertyEnumerator  | No  |                                     |
| TEE_FreePropertyEnumerator      | No  |                                     |
| TEE_StartPropertyEnumerator     | No  |                                     |
| TEE_ResetPropertyEnumerator     | No  |                                     |
| TEE_GetPropertyName             | No  |                                     |
| TEE_GetNextProperty             | No  |                                     |
| **Panics**                      |     |                                     |
| TEE_Panic                       | Yes | Implemented as stub                 |
| **Internal Client API**         |     |                                     |
| TEE_OpenTASession               | Yes | Not tested                          |
| TEE_CloseTASession              | Yes | Not tested                          |
| TEE_InvokeTACommand             | Yes | Not tested                          |
| **Cancellation Functions**      |     |                                     |
| TEE_GetCancellationFlag         | No  |                                     |
| TEE_UnmaskCancellation          | No  |                                     |
| TEE_MaskCancellation            | No  |                                     |
| **Memory Management Functions** |     |                                     |
| TEE_CheckMemoryAccessRights     | No  |                                     |
| TEE_SetInstanceData             | No  |                                     |
| TEE_GetInstanceData             | No  |                                     |
| TEE_Malloc                      | Yes | Covered in test suite               |
| TEE_Realloc                     | Yes | Not tested                          |
| TEE_Free                        | Yes | Covered in test suite               |
| TEE_MemMove                     | Yes | Not tested                          |
| TEE_MemCompare                  | Yes | Not tested                          |
| TEE_MemFill                     | Yes | Not tested                          |
| **Trusted Storage API for Data and Keys**||                                 |
| **Generic Object Functions**    |     |                                     |
| TEE_GetObjectInfo               | Yes | Partially covered in test suite     |
| TEE_RestrictObjectUsage         | Yes | Not tested                          |
| TEE_GetObjectBufferAttribute    | Yes | Not tested                          |
| TEE_GetObjectValueAttribute     | Yes | Not tested                          |
| TEE_CloseObject                 | Yes | Not tested                          |
| **Transient Object Functions**  |     |                                     |
| TEE_AllocateTransientObject     | Yes | Partially covered in test suite     |
| TEE_FreeTransientObject         | Yes | Partially covered in test suite     |
| TEE_ResetTransientObject        | Yes | Partially covered in test suite     |
| TEE_PopulateTransientObject     | Yes | Partially covered in test suite     |
| TEE_InitRefAttribute            | Yes | Partially covered in test suite     |
| TEE_InitValueAttribute          | Yes | Not tested                          |
| TEE_CopyObjectAttributes        | Yes | Not tested                          |
| TEE_GenerateKey                 | No  |                                     |
| **Persistent Object Functions** |     |                                     |
| TEE_OpenPersistentObject        | Yes | Not tested                          |
| TEE_CreatePersistentObject      | Yes | Not tested                          |
| TEE_CloseAndDeletePersistentObject | Yes | Not tested                       |
| TEE_RenamePersistentObject      | Yes | Not tested                          |
| **Persistent Object Enumeration Functions**||                               |
| TEE_AllocatePersistentObjectEnumerator | Yes | Not tested                   |
| TEE_FreePersistentObjectEnumerator  | Yes | Not tested                      |
| TEE_ResetPersistentObjectEnumerator | Yes | Not tested                      |
| TEE_StartPersistentObjectEnumerator | Yes | Not tested                      |
| TEE_GetNextPersistentObject     | Yes | Not tested                          |
| **Data Stream Access Functions**|     |                                     |
| TEE_ReadObjectData              | Yes | Not tested                          |
| TEE_WriteObjectData             | Yes | Not tested                          |
| TEE_TruncateObjectData          | Yes | Not tested                          |
| TEE_SeekObjectData              | Yes | Not tested                          |
| **Cryptographic Operations API**|     |                                     |
| **Generic Operation Functions** |     |                                     |
| TEE_AllocateOperation           | Yes | Partially covered in test suite     |
| TEE_FreeOperation               | Yes | Partially covered in test suite     |
| TEE_GetOperationInfo            | Yes | Partially covered in test suite     |
| TEE_ResetOperation              | Yes | Partially covered in test suite     |
| TEE_SetOperationKey             | Yes | Partially covered in test suite     |
| TEE_SetOperationKey2            | Yes | Not tested                          |
| TEE_CopyOperation               | Yes | Not tested                          |
| **Message Digest Functions**    |     |                                     |
| TEE_DigestUpdate                | Yes | Supports SHA only. Not tested       |
| TEE_DigestDoFinal               | Yes | Supports SHA only. Not tested       |
| **Symmetric Cipher Functions**  |     |                                     |
| TEE_CipherInit                  | Yes | Supports AES only. Partially covered in test suite |
| TEE_CipherUpdate                | Yes | Supports AES only. Partially covered in test suite |
| TEE_CipherDoFinal               | Yes | Supports AES only. Not tested       |
| **MAC Functions**               |     |                                     |
| TEE_MACInit                     | Yes | Supports SHA only. Not tested       |
| TEE_MACUpdate                   | Yes | Supports SHA only. Not tested       |
| TEE_MACComputeFinal             | Yes | Supports SHA only. Not tested       |
| TEE_MACCompareFinal             | Yes | Not tested                          |
| **Authenticated Encryption Functions** ||                                   |
| TEE_AEInit                      | No  |                                     |
| TEE_AEUpdateAAD                 | No  |                                     |
| TEE_AEUpdate                    | No  |                                     |
| TEE_AEEncryptFinal              | No  |                                     |
| TEE_AEDecryptFinal              | No  |                                     |
| **Asymmetric Functions**        | No  |                                     |
| TEE_AsymmetricEncrypt           | No  |                                     |
| TEE_AsymmetricDecrypt           | No  |                                     |
| TEE_AsymmetricSignDigest        | No  |                                     |
| TEE_AsymmetricVerifyDigest      | No  |                                     |
| **Key Derivation Functions**    |     |                                     |
| TEE_DeriveKey                   | No  |                                     |
| **Random Data Generation Function**    ||                                   |
| TEE_GenerateRandom              | No  |                                     |
| **Time API**                    |     |                                     |
| **Time Functions**              |     |                                     |
| TEE_GetSystemTime               | No  |                                     |
| TEE_Wait                        | No  |                                     |
| TEE_GetTAPersistentTime         | No  |                                     |
| TEE_SetTAPersistentTime         | No  |                                     |
| TEE_GetREETime                  | No  |                                     |
| **TEE Arithmetical API**        | No  |                                     |
| **Initialization Functions**    | No  |                                     |
| TEE_BigIntInit                  | No  |                                     |
| TEE_BigIntInitFMMContext        | No  |                                     |
| TEE_BigIntInitFMM               | No  |                                     |
| **Converter Functions**         |     |                                     |
| TEE_BigIntConvertFromOctetString| No  |                                     |
| TEE_BigIntConvertToOctetString  | No  |                                     |
| TEE_BigIntConvertFromS32        | No  |                                     |
| TEE_BigIntConvertToS32          | No  |                                     |
| **Logical Operations**          |     |                                     |
| TEE_BigIntCmp                   | No  |                                     |
| TEE_BigIntCmpS32                | No  |                                     |
| TEE_BigIntShiftRight            | No  |                                     |
| TEE_BigIntGetBit                | No  |                                     |
| TEE_BigIntGetBitCount           | No  |                                     |
| **Basic Arithmetic Operations** |     |                                     |
| TEE_BigIntAdd                   | No  |                                     |
| TEE_BigIntSub                   | No  |                                     |
| TEE_BigIntNeg                   | No  |                                     |
| TEE_BigIntMul                   | No  |                                     |
| TEE_BigIntSquare                | No  |                                     |
| TEE_BigIntDiv                   | No  |                                     |
| **Modular Arithmetic Operations** |   |                                     |
| TEE_BigIntMod                   | No  |                                     |
| TEE_BigIntAddMod                | No  |                                     |
| TEE_BigIntSubMod                | No  |                                     |
| TEE_BigIntMulMod                | No  |                                     |
| TEE_BigIntSquareMod             | No  |                                     |
| TEE_BigIntInvMod                | No  |                                     |
| **Other Arithmetic Operations** |     |                                     |
| TEE_BigIntRelativePrime         | No  |                                     |
| TEE_BigIntComputeExtendedGcd    | No  |                                     |
| TEE_BigIntIsProbablePrime       | No  |                                     |
| **Fast Modular Multiplication Operations**||                                |
| TEE_BigIntConvertToFMM          | No  |                                     |
| TEE_BigIntConvertFromFMM        | No  |                                     |
| TEE_BigIntComputeFMM            | No  |                                     |

## 3. mTower configuration options

Build parameters of mTower project, including target platform, optimization settings, debug options and startup/boot sequence, are governed by values that are set in Kconfig-based build configuration system. Below is an overview of parameters that can be set or changed by using this system.

*Build setup* menu
This menu contains settings that determine optimization level of generated binary images, debug support, and trace messages generated by binary code.
*Control optimization* submenu lets you select optimization level for generated binary images. Note, though, that most of theoretically avaliable optimization levels do not work in practice (they cause compilation errors). Default is optimization level 1 that is guaranteed to work.

*Debug options* submenu contains a variety of debug-related options.
*Enable Debug Features* option, if set, opens a submenu of debug features:
*Enable Error Output* option, if set, will include error messages into output log. It will also allow you to select
*Enable Warnings Output* option, that, if set, will include warning messages into output log. It will also allow you to select
*Enable Informational Debug Output* option, which will include info messages into output log.  It will also allow you to select
*Enable Debug Informational Output* option, which will include general messages into output log.
*Enable Debug Assertions* option enables debug assertions that will halt the system when fail.
*Enable include File and Function name to output* option includes info on file and function names which emit messages into logs.
*Enable colorized output* option enables color control sequences in logging messages that allow you to mark various messages with differing colors.
*Generate Debug Symbols* option which must be set to *y* if you want the binary images to include symbols required by debuggers.
*TEE Internal API trace level* submenu lets you select which messages from TEE Internal API function code will be included into log: general messages, errors, warning, information, debug and/or flow.
*TEE Client API trace level* submenu lets you select which messages from TEE Client API function code will be included into log: general messages, errors, warning, information, debug and/or flow.

*Trusted boot* menu
This menu lets you enable or disable trusted boot support for mTower, and to specify exact boot sequence (which loaders are used by boot sequence and which are disabled). There are also options for explicitly specifying start addresses of boot loaders.

*System type* menu
This menu lets you specify target platform for which the build is to be performed. Note that contents of this menu are directly imported from arch/Kconfig file.
*MCU Architecture* submenu lets you select type of MCU for which the binary images are to be built. As of now, Cortex-M23 is the only supported type.
*MCU Family* submenu lets you select the family of MCU - however, as of now M2351 is the only choice.
*M2351 Platform Selection* submenu lets you select specific board for which images are to be built. As of now, there are 2 choices: NuMaker-PFM-M2351 board and M2351-Badge board.
*Non-secure debug UART configuration* submenu lets you select which UART is to be used for debug output from non-secure world (UART0 to UART5 are available choices).
*Non-secure debug font color configuration* submenu lets you select color for log messages sent via non-secure debug UART.
*Secure debug UART configuration* submenu lets you select which UART is to be used for debug output from secure world (UART0 to UART5 are available choices).
*Secure debug font color configuration* submenu lets you select color for log messages sent via secure debug UART.
*Secure attribution configuration* submenu contains submenus and options for configuring security-related parameters of Cortex-M23 MCUs:
*GPIO Secure Attribution Configuration* submenu allows you to specify which ports are to be considered secure (ports A to H are available for configuration).
*Secure SRAM size* submenu allows you to select Secure SRAM Size (0 to 96 kB in 8 kB increments)
*Secure Flash ROM Size* option lets you specify Secure Flash ROM size (hexadecimal size value is accepted)
*Peripheral Secure Attribution Configuration* submenu lets you specify if individual peripheral devices are considered secure or non-secure.
*Assign Interrupt to Secure or Non-secure Vector* submenu lets you assign specific interrupts to secure or non-secure handler addresses.
*Enable secure violation interrupts* submenu lets you specify which secure violation interrupts are enabled.
*Secure Attribute Unit (SAU) Control* option, if enabled, opens up a subset of options that allow you to configure SAU.
*Enable SAU* option enables Secure Attribute Unit if set.
*All Memory Attribute When SAU is disabled* option lets you specify if all memory is to be considered secure or non-secure if SAU is disabled.
*Enable and Set Secure/Non-Secure region* submenu allows you to specify border addresses of up to 8 memory regions and specify if each of these regions is to be considered secure or non-secure.
*Toolchain Selection* submenu lets you select toolchain used for building binary images. Options are GCC 6.1q1, GCC 6.1q2 and GCC 8q4.


*Application configuration* menu
This menu contains options for including specific applications into generated binary images. Note that contents of this menu are directly imported from apps/Kconfig file.

