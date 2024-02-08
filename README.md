# ![](docs/images/icon/mTower-logo-81_128.png) mTower

[![Build](https://github.com/samsung/mtower/workflows/Build/badge.svg)](https://github.com/samsung/mtower/actions?query=workflow%3ABuild)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/6108/badge)](https://bestpractices.coreinfrastructure.org/projects/6108)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/samsung/mtower/badge)](https://api.securityscorecards.dev/projects/github.com/samsung/mtower)
[![RepoSize](https://img.shields.io/github/repo-size/samsung/mtower.svg)](https://github.com/samsung/mtower)
[![Release](https://img.shields.io/github/v/release/samsung/mtower.svg)](https://github.com/samsung/mtower/releases)
[![LICENSE](https://img.shields.io/github/license/samsung/mtower.svg)](https://github.com/samsung/mtower/blob/master/LICENSE)

## Contents
1. [Introduction](#1-introduction)
2. [License](#2-license)
3. [Platforms supported](#3-platforms-supported)
4. [Get and build mTower software](#4-get-and-build-mtower-software)
5. [Source code structure](#5-source-code-structure)
6. [Coding standards](#6-coding-standards)
7. [Documentation](#7-documentation)
8. [Contributing](#8-contributing)

## 1. Introduction
The `mTower` is a new Trusted Execution Environment (TEE) specially designed
to protect size-constrained IoT devices based on Cortex-m23 MCU. Usage mTower
pre-embedded into the microcontroller, a module developer can use a simple SDK
that based on Global Platform API standards to add security to their solution.

---
## 2. License
mTower software consists of multiple components that are individually available
under different licensing terms. Terms for each individual file are listed at
the beginnings of corresponding files; also, all licenses are listed in
[COPYING] file.

--- 
## 3. Platforms supported

| **NuMaker-PFM-M2351** | **M2351-Badge** | **V2M-MPS2-Qemu** | **How to add a platform** |
|-----------------------|---------------------------|-------------|-----------|
|[![](docs/images/platforms/numaker_pfm_m2351/numaker_pfm_m2351.png)](docs/numaker_pfm_m2351.md)|[![](docs/images/platforms/m2351_badge/m2351_badge.png)](docs/m2351_badge.md) |[![V2M-MPS2-Qemu](docs/images/platforms/v2m-mps2/v2m-mps2.png)](docs/v2m-mps2-qemu.md)|[![](docs/images/platforms/add_new_board.jpg)](docs/port-new-platform.md)|


Several platforms are supported. In order to manage slight differences
between platforms, a `PLATFORM` flag has been introduced.

| Platform              | Composite PLATFORM flag            | Maintained |
|-----------------------|------------------------------------|------------|
| [NuMaker-PFM-M2351]   |`PLATFORM=numaker_pfm_m2351`        | v0.4       |
| [M2351-Badge]         |`PLATFORM=m2351_badge`              | v0.4       |
| [V2M-MPS2-Qemu]       |`PLATFORM=mps2_an505_qemu`          | v0.4       |

For information on adding a new platform see the [how to add a platform].

---
## 4. Get and build mTower software
Please see [build] for instructions how to run mTower on various devices.

---
## 5. Source code structure
The general [source code structure] for mTower is similar to the structure of the
multy platforms source code.

---
## 6. Coding standards
In this project we are trying to adhere to the mTower coding convention 
(see [CodingStyle]). However there are a few exceptions that we had to make since
the code also uses other open source components.

---
## 7. Documentation
There is a brief overall [functionality description](docs/mtower_functionality_description.md) of mTower. Other mTower documentation for the project is located in the [docs] folder. The latest version of the specification that describes the mTower source code can be generated using [doxygen] tool from command line. To generate documentation, use

```sh
make docs_gen
```
command, and to view generated docs use

```sh
make docs_show
```
> Note that documentation on mTower is work in progress, and right now doxygen does not provide much documentation.

---
## 8. Contributing
If you want to contribute to the mTower project and make it better, your help is
very welcome. Contributing is also a great way to learn more about social
coding on Github, new technologies and and their ecosystems. [How to contribute
you can find here](.github/CONTRIBUTING.md).

---

[docs]: ./docs
[COPYING]: COPYING
[build]: docs/build.md
[how to add a platform]: docs/port-new-platform.md
[CodingStyle]: docs/mtower-coding-standard.md
[source code structure]: docs/source-code-structure.md
[doxygen]: http://www.doxygen.nl
[NuMaker-PFM-M2351]: http://www.nuvoton.com.cn/hq/products/iot-solution/iot-platform/numaker-maker-platform/numaker-pfm-m2351?__locale=en
[M2351-Badge]: docs/schemes/m2351_badge
[V2M-MPS2-Qemu]: https://developer.arm.com/documentation/100964/1114/Microcontroller-Prototyping-System-2?lang=en
