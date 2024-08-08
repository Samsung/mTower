    .syntax unified
    .arch   armv8 - m.base

    .section .rodata
    .align  3

    .global  g_NuBL32KeyStart
    .global  g_NuBL32KeyEnd
    .global  g_NuBL32KeyHashStart
    .global  g_NuBL32KeyHashEnd

    .global  g_NuBL33KeyStart
    .global  g_NuBL33KeyEnd
    .global  g_NuBL33KeyHashStart
    .global  g_NuBL33KeyHashEnd

g_NuBL32KeyStart:
    .incbin "KeyInfo/NuBL32PubKeyEncrypted.bin"
g_NuBL32KeyEnd:

g_NuBL32KeyHashStart:
    .incbin "KeyInfo/NuBL32PubKeyEncryptedHash.bin"
g_NuBL32KeyHashEnd:


g_NuBL33KeyStart:
    .incbin "KeyInfo/NuBL33PubKeyEncrypted.bin"
g_NuBL33KeyEnd:

g_NuBL33KeyHashStart:
    .incbin "KeyInfo/NuBL33PubKeyEncryptedHash.bin"
g_NuBL33KeyHashEnd:

