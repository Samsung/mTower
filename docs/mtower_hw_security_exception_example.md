# mTower hardware security exception example

# Contents
1. [Overview](#1-overview)
2. [How to run](#2-how-to-run)

## 1. Overview
mTower code includes a variant of firmware for MCU board that demonstrates operation of TrustZone security that protects specified hardware and memory areas against unauthorized access. This variant can be built by selecting corresponding option from menuconfig, building firmware from source code and flashing it onto board. After flashing is complete, board will startup and provide a text menu via connected console that allows user to check standard cases where security mechanism allows and/or blocks operations depending on addresses and system configuration. Access to unprotected memory areas and hardware will be demonstrated as successful, while attempts to access protected memory areas and hardware without necessary privileges will result in security exceptions or hard faults, depending on specific circumstances. Operations can be called from normal world or secure world depending on user's choice, and will have different results depending on world chosen.

## 2. How to run
Start out by following the "Get and build the solution" in the [build.md] file.
> Warning: Need to download the toolchain (exec. `make toolchain`).

To run test, we need to build a special firmware version that includes test cases and their shell program. All the other applications are excluded from build. To build this firmware version, we run

```
make menuconfig
```

shell command, and select Application configuration -> H/W security exception example in on-display menu. After saving the configuration and exiting configuration menu, we need to build the firmware by issuing

```
make clean; make
```

command. After firmware is built, procedure of flashing it onto board and running is the same as usual.
Below is an example of menu displayed by example in Secure World console after startup:

```
        -=mTower v0.1=-  Aug 12 2019  13:17:56

+---------------------------------------------+
|              Secure is running ...          |
+---------------------------------------------+
Non-secure Hard Fault handled by secure code.
+---------------------------------------------------------------------+
| Type  | Range addresses         | Size      | Remarks               |
+---------------------------------------------------------------------+
| FLASH | 0x00000000 - 0x0003F000 | 0x3F000   | 252k                  |
| RAM   | 0x20000000 - 0x20007FFF | 0x8000    | 32k                   |
| NCS   | 0x0003F000 - 0x0003FFFF | 0x1000    | 4k                    |
| GPIO  | PC                      |           | Port C is non-secure  |
| GPIO  | PA,PB                   |           | Port A,B are secure   |
+---------------------------------------------------------------------+

+---------------------------------------------------------------------+
| Key | Action                                   | Expected response  |
+---------------------------------------------------------------------+
| [1] | Read SRAM non-secure address 0x30017000  | Access successful  |
|     | Read SRAM non-secure address 0x20017000  | RAZWI              |
| [2] | Read SRAM secure address 0x20001D4C      | Access successful  |
|     | Read SRAM secure address 0x30001D4C      | RAZWI              |
| [3] | Read FLASH non-secure address 0x00000000 | Access successful  |
|     | Read FLASH non-secure address 0x10000000 | RAZWI              |
| [4] | Read FLASH secure address 0x10040000     | Access successful  |
|     | Read FLASH secure address 0x00040000     | RAZWI              |
| [5] | Read GPIO non-secure port PC1_NS         | Access successful  |
|     | Write 0 GPIO non-secure port by PC1_NS   | Access successful  |
|     | Write 1 GPIO non-secure port by PC1      | RAZWI              |
| [6] | Read GPIO secure port PA10               | Access successful  |
|     | Write 0 GPIO secure port by PA10         | Access successful  |
|     | Write 1 GPIO secure port by PA10_NS      | RAZWI, sec. violat.|
| [7] | Write 0 FLASH to address 0x0 (directly)        | Hard fault         |
| [8] | Read 0x20018000 address (nonexistent)    | Hard fault         |
|[any]| Go non-secure code                       | Exec. non-sec code |
+---------------------------------------------------------------------+
```

User can cause different operations to be performed by typing in corresponding
numbers (1 to 8) or switch to Normal World mode by typing any other character.
Normal World mode operations can be selected from similar menu, but results
of these operations will differ from results of similar operations called from
Secure World (mostly they will result in Hard Faults).

```
        -=mTower v0.1=-  Aug 12 2019  13:18:06

+---------------------------------------------+
|     Nonsecure FreeRTOS is running ...       |
+---------------------------------------------+
+---------------------------------------------------------------------+
| Type  | Range addresses         | Size      | Remarks               |
+---------------------------------------------------------------------+
| FLASH | 0x10040000 - 0x0007FFFF | 0x40000   | 256k                  |
| RAM   | 0x30008000 - 0x30017FFF | 0x10000   | 64k                   |
| GPIO  | PC                      |           | Port C is non-secure  |
| GPIO  | PA,PB                   |           | Port A,B are secure   |
+---------------------------------------------------------------------+

+---------------------------------------------------------------------+
| Key | Action                                   | Expected response  |
+---------------------------------------------------------------------+
| [1] | Read SRAM secure address 0x30000000      | Hard fault         |
| [2] | Read SRAM secure address 0x20000000      | Hard fault         |
| [3] | Read SRAM non-secure address 0x30009C2C  | Access successful  |
|     | Read SRAM non-secure address 0x20009C2C  | Hard fault         |
| [4] | Read FLASH non-secure address 0x00000000 | Hard fault         |
| [5] | Read FLASH non-secure address 0x10000000 | Hard fault         |
| [6] | Read FLASH secure address 0x10040000     | Access successful  |
|     | Read FLASH secure address 0x00040000     | Hard fault         |
| [7] | Read GPIO non-secure port PC1_NS         | Access successful  |
|     | Write 0 GPIO non-secure port by PC1_NS   | Access successful  |
|     | Write 1 GPIO non-secure port by PC1      | Hard fault         |
| [8] | Read GPIO secure port PA10_NS            | RAZWI, sec. violat.|
|     | Write 0 GPIO secure port by PA10_NS      | RAZWI, sec. violat.|
|     | Write 1 GPIO secure port by PA10         | Hard fault         |
| [9] | Write 0 to address 0x0 (directly)        | Hard fault         |
| [a] | Read 0x30018000 address (nonexistent)    | Hard fault         |
+---------------------------------------------------------------------+
```
> Note that correct operation of this example depends on addresses and sizes of
memory areas allocated to Secure and Normal World, as well as on correct
configuration of GPIO pins.
