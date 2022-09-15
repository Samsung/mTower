# mTower on V2M-MPS2-Qemu

[![V2M-MPS2-Qemu](images/platforms/v2m-mps2/v2m-mps2.png)](https://developer.arm.com/documentation/100964/1114/Microcontroller-Prototyping-System-2?lang=en)

# Contents
1. [Introduction](#1-introduction)
2. [Regular build](#2-regular-build)
3. [How to run](#3-how-to-run)
4. [How to debug](#4-how-to-debug)
5. [References](#5-references)

## 1. Introduction
The instructions here will tell how to run mTower on the [V2M-MPS2] board.

## 2. Regular build
Start out by following the "Get and build the solution" in the [build.md] file.
> Warning: Need to download the toolchain (exec. `make toolchain`).

## 3. How to run

> TBD

## 4. How to debug
```
make debug
```
We use nc consoles for debugging: one will connect for Secure and one for Non-Secure input/output.
```
nc 127.0.0.1 1235
```
```
nc 127.0.0.1 1236
```

## 5. References

[build.md]: build.md
[V2M-MPS2]: https://developer.arm.com/documentation/100964/1114/Microcontroller-Prototyping-System-2?lang=en
