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

> TBD

