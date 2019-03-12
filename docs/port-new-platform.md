# How to port a new platform

# Contents
1. [Introduction](#1-introduction)
2. [Add a new platform](#2-add-a-new-platform)
    1. [arch/<newdev>](#2-1-arch-newdev)
    2. [config/<newdev>](#2-2-config-newdev)
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

### 2.1 arch/<newdev>

In `arch/` you will find all the currently supported architecture. That is where you
are supposed to add a new platform or modify an existing one. Typically you will
find this set of files in a specific platform folder. Clone from an existing
platform.

> TBD

### 2.2 config/<newdev>
Clone from an existing platform.
For example: config/numaker_pfm_m2351 -> arch/<newdev>

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
