# mTower build

## 1. Platforms supported
Below is a table showing the platforms supported. In order to manage slight 
differences between platforms, a `PLATFORM` flag has been introduced.

| Platform            | Composite PLATFORM flag     | Maintained |  Quick start guide doc |
|---------------------|-----------------------------|------------|------------------------| 
| [NuMaker-PFM-M2351] |`PLATFORM=numaker_pfm_m2351` | v0.4       | [numaker_pfm_m2351.md] |
| [M2351-Badge]       |`PLATFORM=m2351_badge`       | v0.4       | [m2351_badge.md]       |
| [V2M-MPS2-Qemu]     |`PLATFORM=mps2_an505_qemu`   | v0.4       | [v2m-mps2-qemu.md]     |

---

## 2. Get and build the solution
Below we will describe the general way of getting the source, building the
solution and how to run test on the device. For device specific instructions,
see the respective `[platform].md` file in the [docs] folder.

### 2.1 Prerequisites
We believe that you can use any Linux distribution to build mTower, but as
maintainers of mTower we are mainly using Ubuntu-based distributions and to be
able to build and run mTower there are a few packages that must be installed
to start with. Therefore install the following packages regardless of what
target you will use in the end.

```sh
sudo apt-get install git gcc subversion make gperf flex bison libncurses5-dev texinfo g++ curl pkg-config autoconf libtool libtool-bin libc6:i386 libc6-dev:i386 gcc-multilib doxygen doxygen-gui libssl-dev libcurl4-openssl-dev wget
```

### 2.2 Building kconfig-frontends in Linux (optional)

Preparing kconfig-frontends for Linux is a fast and simple process, similar
(if not identical) to preparation of any software using autotools. Typically
the whole process will require you to only download the source tarball, extract
it somewhere and run `./configure && make && sudo make install` in extracted
folder.

#### 2.2.1 Download and extract the source tarball

```sh
curl -O http://ymorin.is-a-geek.org/download/kconfig-frontends/kconfig-frontends-3.12.0.0.tar.xz
tar -xf kconfig-frontends-3.12.0.0.tar.xz
cd kconfig-frontends-3.12.0.0
```
> Sometimes the http://ymorin.is-a-geek.org site does not work, in this case
you can download it [here](https://autobuilder.yocto.io/pub/sources/) or find
it yourself

```sh
curl -O https://autobuilder.yocto.io/pub/sources/kconfig-frontends-3.12.0.0.tar.xz
```

#### 2.2.2 Patch

If your system has gperf 3.0.4 or earlier, you may safely skip this chapter.

gperf 3.1 (released on 5th January of 2017) changed the type used as length
argument in generated functions from unsigned int to size_t. This will cause
your build to fail with following error message:

```sh
      CC     libkconfig_parser_la-yconf.lo
    In file included from yconf.c:234:0:
    hconf.gperf:141:1: error: conflicting types for 'kconf_id_lookup'
    hconf.gperf:12:31: note: previous declaration of 'kconf_id_lookup' was here
     static const struct kconf_id *kconf_id_lookup(register const char *str, register unsigned int len);
                                   ^~~~~~~~~~~~~~~
    make[3]: *** [Makefile:456: libkconfig_parser_la-yconf.lo] Error 1
    make[2]: *** [Makefile:350: all] Error 2
    make[1]: *** [Makefile:334: all-recursive] Error 1
    make: *** [Makefile:385: all-recursive] Error 1
```
The procedure to fix is below:

```sh
curl -O https://gist.githubusercontent.com/KamilSzczygiel/d16a5d88075939578f7bd8fadd0907aa/raw/1928495cfb6a6141365d545a23d66203222d28c0/kconfig-frontends.patch
patch -p1 -i kconfig-frontends.patch
autoreconf -fi
```
#### 2.2.4 Configure

The recommended set of options to configure kconfig-frontends for use with
distortos would be:

```sh
./configure --enable-conf --enable-mconf --disable-shared --enable-static
```
#### 2.2.5 Compile and install

```sh
make
sudo make install
# sudo strip /usr/local/bin/kconfig-*
cd <mTower_PATH>
```

> Full description [How to install kconfig-frontends](http://distortos.org/documentation/building-kconfig-frontends-linux/)

### 2.3 Get the source code

```sh
git clone https://github.com/Samsung/mTower.git
```
Choose the PLATFORM corresponding to the platform you intend to use. For
example, if you intend to use NuMaker-PFM-M2351, then `PLATFORM` should be
`numaker_pfm_m2351`. After that, generate context using default configuration
options.

```sh
cd mTower/
```
```sh
make PLATFORM=numaker_pfm_m2351 create_context
```

To use BootLoader2 for M2351 Nuvoton chips, you need to download NuBL2 sources and 
syscall file. This can be done by executing following shell commands:

```sh
wget -L https://raw.githubusercontent.com/OpenNuvoton/M2351BSP/master/Library/Device/Nuvoton/M2351/Source/GCC/_syscalls.c -O ./arch/cortex-m23/m2351/src/Device/Nuvoton/M2351/Source/GCC/_syscalls.c
```
```sh
wget -L https://raw.githubusercontent.com/OpenNuvoton/M2351BSP/master/SampleCode/MKROM/SecureBootDemo/NuBL2/main.c -O ./arch/cortex-m23/m2351/src/NuBL2/main.c
```
```sh
wget -L https://raw.githubusercontent.com/OpenNuvoton/M2351BSP/master/SampleCode/MKROM/SecureBootDemo/NuBL2/VerifyNuBL3x.c -O ./arch/cortex-m23/m2351/src/NuBL2/VerifyNuBL3x.c
```
```sh
wget -L https://raw.githubusercontent.com/OpenNuvoton/M2351BSP/master/SampleCode/MKROM/SecureBootDemo/NuBL2/NuBL2.h -O ./arch/cortex-m23/m2351/src/NuBL2/NuBL2.h
```
```sh
wget -L https://raw.githubusercontent.com/OpenNuvoton/M2351BSP/master/SampleCode/MKROM/SecureBootDemo/NuBL2/FwInfo/FwInfo.c -O ./arch/cortex-m23/m2351/src/NuBL2/FwInfo.c
```

> Please note that files from these downloads contain the following copyright
notice: `Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.`
Licensing terms specified [here](https://github.com/OpenNuvoton/M2351BSP/blob/master/NUVOTON%20SOFTWARE%20LICENSE%20AGREEMENT.pdf).  
`arch/cortex-m23/m2351/src/Device/Nuvoton/M2351/Source/GCC/_syscalls.c` under:  
// This file is part of the uOS++ III distribution  
// Parts of this file are from the newlib sources, issued under GPL.  
// Copyright (c) 2014 Liviu Ionescu  

### 2.4 Get the toolchains
In mTower different toolchains are used for different targets (depends on
architecture 32-bit solutions). In any case start by downloading the
toolchains by:

```sh
make toolchain
```
### 2.5 Build the solution
We've configured default .config and Make.defs for each platforms, so that 
configuration will always automatically to the correct device specific make, 
that means that you simply start the build by running:

```sh
make
```

### 2.6 Flash the device
Please see instructions for specific devices (e.g., for NuMaker-PFM-M2351, see [numaker_pfm_m2351.md]).

### 2.7 Boot up the device
Please see instructions for specific devices (e.g., for NuMaker-PFM-M2351, see [numaker_pfm_m2351.md]).

[docs]: ./
[NuMaker-PFM-M2351]: http://www.nuvoton.com/resource-files/UM_NuMaker-PFM-M2351_EN_Rev1.00.pdf
[numaker_pfm_m2351.md]: ./numaker_pfm_m2351.md
[M2351-Badge]: ./m2351_badge.md
[m2351_badge.md]: ./m2351_badge.md
[V2M-MPS2-Qemu]: https://developer.arm.com/documentation/100964/1114/Microcontroller-Prototyping-System-2?lang=en
[v2m-mps2-qemu.md]: ./v2m-mps2-qemu.md