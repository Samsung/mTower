# How to port a new platform

# Contents
1. [Introduction](#1-introduction)
2. [Add a new platform](#2-add-a-new-platform)
    2.1 [arch/<newdev>](#2-1-arch-newdev)
    2.2 [config/<newdev>](#2-2-config-newdev)
3. [Code inclusion to mTower upstream](#3-code-inclusion-to-mtower-upstream)

## 1. Introduction
This document provides and overview of the mTower build and configuration logic
and provides hints for the incorporation of new processor/board architectures
into the build.

## 2. Add a new platform
The first thing you need to do after you have decided to port mTower to another
device is to add a new platform device. Typically initial setup involve
configuring UART, memory allocator etc. For simplicity let us call our new
platform for "newdev" just so we have something to refer to when writing
examples further down.

### 2.1 arch/newdev

In `arch/` you will find all the currently supported architectures. That is where you
are supposed to add a new platform or modify an existing one. Typically you will
find this set of files in a specific platform folder. Clone from an existing
platform.

#### 2.1.1 Adding new architecture
If a new platform uses an architecture not included into build before (as of now, only cortex-m23 architecture is included), you need to add a new block into `arch/Kconfig` file that specifies the name of new architecture. For example:

```

config ARCH_CORTEX_M33
	bool "cortex-m33"
	---help---
		The ARM architecture

```

Also, under line

```
default "cortex-m23"    if ARCH_CORTEX_M23`
```

 you need to add a new line for architecture that is added, e. g.:
 
```
default "cortex-m33"    if ARCH_CORTEX_M33`
```

Under

```
if ARCH_CORTEX_M23
source arch/cortex-m23/Kconfig
endif
```
block you need to add a similar block for new architecture, e.g.,

```
if ARCH_CORTEX_M33
source arch/cortex-m33/Kconfig
endif
```


#### 2.1.2 Adding chip family
If a new platform uses chip family not included into build before, add a new folder into architecture folder, e.g.,
`arch/cortex-m23/new_chip_family`.
After that, add a new chip family into `Kconfig` file, e.g., `arch/cortex-m23/Kconfig`:

```
config ARCH_FAMILY_NEW_CHIP_FAMILY
	bool "NEW_CHIP_FAMILY"
	---help---
		The new chip family

endchoice

config ARCH_FAMILY
	string
	default "m2351"            if ARCH_FAMILY_M2351
	default "NEW_CHIP_FAMILY" if ARCH_FAMILY_NEW_CHIP_FAMILY

if ARCH_FAMILY_M2351
comment "M2351 Configuration Options"
source arch/cortex-m23/m2351/Kconfig
endif

if ARCH_FAMILY_NEW_CHIP_FAMILY
comment "NEW_CHIP_FAMILY Configuration Options"
source arch/cortex-m23/new_chip_family/Kconfig
endif

```

#### 2.1.3 Adding platform
Last step (and the only one that is always needed) is adding new platform folder and updating corresponding `KConfig` file. Add a folder named after new platform into chip family folder, e.g., `arch/cortex-m23/new_chip_family/new_platform`. Then open `Kconfig` file in this new folder, and add the following lines into it:

```
config NEW_PLATFORM
	bool "new_platform board"
	---help---
		new platform board.

endchoice

config PLATFORM
	string
	default "numaker_pfm_m2351"  if PLATFORM_NUMAKER_PFM_M2351
	default "new_platform"  if NEW_PLATFORM
```

#### 2.1.4 Adding source files for new platform
All the source files that are specific for new platform should be located in subfolders of platform `src` folder. Usually, `src` folder contains following subfolders: `CMSIS` with Cortex Microcontroller Software Interface Standard files
`Device` with device-specific low-level startup routines (these routines are usually executed before main() function starts in C-language programs).
`StdDriver` with standard low-level hardware interaction routines provided by device manufacturer.
`<platform_name>` for user-developed applications. We will store our application sources (both Secure and Non-secure) here, as well as main makefile containing build instructions for these applications. Note that `Makefile` and `Make.defs` files contained in these folders must use file paths specific for this platform. It is impossible to provide detailed instructions for writing these files here, so we will provide just some guidelines. `Makefile` must include low-level libraries necessary for applications into build sequence; all the drivers that are necessary for hardware used by program also must be included in build.

> Pay special attention to file which describes memory regions and peripheral device access rules. This file is platform specific, e.g., for `m2351` platform it is named `partition_M2351.h`.

### 2.2 config/newdev
Clone from an existing platform.
For example: config/numaker_pfm_m2351 -> arch/newdev

```sh
configs/numaker_pfm_m2351  ->  configs/<newdev>
├── defconfig                  ├── defconfig
└── Make.defs                  └── Make.defs
```

Make all the necessary changes such as toolchain, compilation options, etc.,
and save this `Make.defs` file. If you modified the root `mTower/Make.defs`
file save it using the command:

```sh
$ make savedefmakedefs
```

Create the default build configuration using the command:

```sh
$ make menuconfig
```
and save it as the default configuration for this platform

```sh
$ make savedefconfig

```

## 3. Code inclusion to mTower upstream
We do encourage everyone to submit their board support to the mTower project
itself, so it becomes part of the official releases and will be maintained by
the mTower community itself. If you intend to do so, then there are a few more
things that you are supposed to do.


### 3.1 Update README.md build.md

There is a section (3. Platforms Supported) that lists all devices officially
supported in mTower, that is where you also shall list your device. It should
contain the name of the platform, then composite `PLATFORM` flag and whether the
device is publicly available or not. Also need to add `<newdev>.md` file with
description

### 3.2 Maintainer
If you are submitting the board support upstream and cannot give mTower
maintainers a device, then we are going to ask you to become the maintainer for
the device you have added. This means that you should also update the AUTHORS
file accordingly. By being a maintainer for a device you are responsible to
keep it up to date and you will be asked every quarter as part of the mTower
release schedule to test your device running the latest mTower software.

[README.md]: ../README.md
[build.md]: build.md
