# mTower source code structure

The general source code layout for mTower is typical for multiplatform projects.
At the top level is the main makefile and a series of sub-directories that are
described in detail below:

```sh
. 
├── arch
│   ├── <arch-name>
│   │   ├── <arch-family-name>
│   │   │   ├── include
│   │   │   ├── src
│   │   │   │   ├── <platform-name>
│   │   │   │   │   ├── main.c
│   │   │   │   │   ├── Make.defs
│   │   │   │   │   ├── Makefile
│   │   │   │   │   └── Kconfig
│   │   │   │   └──
│   │   │   └── Kconfig
│   ├── ...
│   └── Kconfig
├── configs (platform configuration-specific files)
│   ├── <platform-name>
│   │   ├── defconfig
│   │   └── Make.defs
│   ├── ...
│   └── Makefile
├── docs (documentation files)
│   ├── images
│   │   └── ...
│   ├── ...
│   └── numaker_pfm_m2351.md
├── include (common header files)
│   └── mtower
│       └── config.h
├── lib (temporary store for *.a files)
│   └── Makefile
├── libc (standard libc sources)
│   ├── Kconfig
│   ├── Makefile
│   ├── ...
│   └── strstr.c
├── toolchain (specific toolchains)
│   ├── gcc-arm-none-eabi-6-2017-q1-update
├── tools (miscellaneous scripts and programs for build system)
│   ├── Makefile.host
│   ├── mkconfig.c
│   └── ...
├── AUTHORS (Authors and maintainers list)
├── COPYING (Extra license for usage of source code by third parties)
├── Doxyfile (doxygen configuration file)
├── Kconfig (Main kconfig-frontends file)
├── LICENSE (mTower license)
├── Makefile (Main Makefile)
├── README.md (Main doc file)
└── ReleaseNotes (Release description)

```

## 1. mTower/arch
This directory contains several sub-directories, each containing
architecture-specific logic. The task of porting mTower to a new processor
consists of adding a new subdirectory under arch/ containing logic specific to
the new architecture.


## 2. mTower/configs
The configs/ subdirectory contains configuration data for each board. These
board-specific configurations plus the architecture-specific configurations in
the arch/ subdirectory completely define a customized port of mTower.


## 3. mTower/docs
General documentation for the mTower resides in this directory.

## 4. mTower/include
This directory holds mTower header files.

```sh
/* Included Files */
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "mtower.h"
```

Contains partial implementation of standard C language library.

## 5. mTower/toolchain
Toolchains necessary for building mTower for specific H/W platforms are
downloaded into this directory.

## 6. mTower/tools
Source code of tools used for building mTower itself for specific architectures.

[README.md]: ../README.md
