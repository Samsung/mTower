---
title: 'mTower: Trusted Execution Environment for MCU-based devices'
tags:
  - ARM
  - TrustZone
  - Trusted Execution Environment
  - embedded systems
  - microcontrollers
authors:
  - name: Taras A. Drozdovskyi
    orcid: 0000-0002-4651-1696
    affiliation: 1
  - name: Oleksandr S. Moliavko
    orcid: 0000-0003-3167-983X
    affiliation: 1
affiliations:
  - name: Samsung R&D Institute Ukraine
  - index: 1
date: 6 May 2019
bibliography: paper.bib
---

# Background
Embedded computing systems have already become omnipresent in day-to-day life, and in many cases these systems deal with confidential data and control access to valuable resources. Therefore, an ever-increasing effort is put into protecting these systems against malicious access. However, in many cases this effort lacks solid ground - the software and data being processed are being protected by outdated mechanisms, contain easy to exploit vulnerabilities and do not pose any real obstacles for determined and technically savvy adversary. Yet developing safe and efficient data protection mechanism from scratch is time-consuming and costly endeavor, and without independent assessment there's always risk of some undiscovered vulnerability that will render the mechanism useless. However, current generations of ARM-based microcontrollers offer a solid hardware foundation for building security mechanisms without having to design and implement basic security primitives - ARM TrustZone technology. Initially developed for ARM family of CPUs, TrustZone technology was later adopted into microcontroller (MCU) implementations of ARM architecture. Software libraries that implement security-related operations based on ARM TrustZone are readily available for Linux-based OSes and widely used, e.g., in smartphones running Android based on Linux kernel. However, these libraries are too cumbersome and resource-consuming to be directly adopted to embedded systems and MCU-based devices where CPU speeds are order of magnitude slower and RAM available for use is severely limited. mTower is an experimental standard-compliant implementation of GlobalPlatform Trusted Execution Environment (GP TEE) APIs based on ARM TrustZone technology. From the outset, it is designed to have minimal RAM footprint and avoid use of time-consuming operations that are undesirable in embedded systems. So far, the results seem promising, even though as of May 2019 only a partial implementation of GP TEE APIs is available in mTower.

# Implementation overview
In general, secure applications using TrustZone hardware as hardware security basis consist of two interacting components: Normal World (NW) and Secure World (SW). Normal World part is, in general, a standard Linux application that uses TEE library containing API functions to interact with Secure World part. Corresponding Secure World part of application is essentially a set of event handlers that are executed in hardware-protected area of RAM under control of specially designed operating system. Secure World part processes calls received from Normal World and deals with sensitive data that must be protected from unauthorized access (cryptographic keys, passwords, user identification data, etc.). Common operations performed by Secure World part of application include data encryption/decryption, user authentication, key generation, digital signing and signature verification. Overall architecture that applications should adhere to is described in TEE System Architecture v1.2 document [@TEESystemArch].

mTower implementation follows this layout: it provides implementation of APIs described in GP TEE specification documents. Two most relevant TEE specifications are

* TEE Client API Specification v1.0[@ClientAPISpec] describing the communication between NW applications and trusted applications residing in the SW.
* TEE Internal Core API Specification v1.2[@CoreAPISpec] describing internal operations of SW parts of applications.

Resource management for mTower is based on FreeRTOS real-time operating system; however, it can be replaced by other operating systems designed for embedded devices if necessary.

It should be noted that mTower, being developed for resource-restricted environments, does not implement either of these specifications fully; furthermore, it is planned that implementations of specific features will be modular so unneeded features can be excluded from executable code at build time to minimize resource consumption.

As of May 2019, mTower runs on Nuvoton M2351 board based on ARM Cortex-M23 MCU; there are plans to include support for other boards based on ARM Cortex-M23/33/33p family MCUs.

# Potential future development and applications
After completing implementation of relevant APIs, the next stage will be providing support for dynamic loading/updating of secure applications, including necessary security mechanism to prevent malicious use of these features. Also, resource manager that will provide secure access to H/W for applicatios will be highly desirable for practical uses of mTower. Another planned development is set of instrumentation hooks in mTower code that will simplify evaluation, performance measurements, assessment and debugging of secure applications.

# Expected audience for mTower project
mTower project is expected to produce proof-of-concepts and evaluation models for ARM TrustZone-based secure applications in MCU-based systems. As such, it can be of interest to:

* Embedded system developers in general
* Internet-of-things and smart home appliance developers
* Computer security specialists

# Research applications
mTower is being developed as a part of search for efficient hardware/software security solution for embedded systems. It is expected to demonstrate feasibility of porting GP TEE-based standard compliant trusted applications from full-feature CPU-based to MCU-based systems. Other intended research application for mTower is using it as platform for developing and evaluating secure applications and frameworks suitable for resource-limited environments, including Internet-of-things and EDGE devices. So far, security issues were rarely addressed during development of applications for embedded systems, and there very little info on performance and resource consumption penalties incurred by adding security mechanisms into applications for such systems. The prevailing opinion seems to be that security through obscurity is sufficient for embedded systems, and any attempt to introduce sophisticated security features will incur unacceptable overhead. So one of the objectives of mTower development is to provide a reference platform which would allow the researchers to actually measure these penalties (in the simplest case, by comparing results obtained from running profiling tool over two versions of same application, first with security mechanism based on mTower and second without security mechanism). We hope that when specific, measurable performance data will become available, ARM TrustZone-based security solutions for embedded system applications will be seen as technically feasible. While we cannot foresee all potential use cases for mTower, and, therefore, cannot provide performance estimates for them, we plan to equip mTower with software hooks and built-in measurement mechanisms, so the potential users can easily measure performance of cryptographic and data security related mechanisms in any application they will base on mTower.

# References

