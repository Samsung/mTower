# How to add a new app

# Contents
1. [Introduction](#1-introduction)
2. [Add a new application](#2-add-a-new-application-non-secure-and-secure-part)
3. [Changing configuration to include new application and build](#3-changing-configuration-to-include-new-application-into-build)

## 1. Introduction
Note that so far, all the applications for mTower are not dynamically
loadable - they are compiled as a part of same Secure and Non-Secure
images and loaded when the system starts. Every application is considered
to be a separate task in FreeRTOS that controls the Non-Secure parts of
applications. Secure parts are not strictly necessary - it is possible
to write an application that will run completely in Non-Secure mode. However,
most applications will make calls to Secure API (Trusted Applications - TAs).
Furthermore, some applications (so called **Pseudo-TAs**) do not make use of TEE
Internal API, while **User-TAs** make calls to functions that (internally) call
functions belonging to TEE Internal API.
Below is a sequence of operations required to add new User TA to mTower build
system.

## 2. Add a new application: Non-secure and Secure part

### 2.1. Add new subfolders to apps folder
To simplify the beginning, the `apps/hello_world` folder contains necessary subfolderse
and files for basic application. Copy this folder and its contents into new subfolder
(e.g., app1).

### 2.1. Add new application into build configuration
Edit `apps/Kconfig` file, adding into it the following lines:

```
config APPS_APP1
	bool "App1 example"
	default y
	---help---
		"App1 example."
```

### 2.2. Generate unique application ID
Each TA must have an unique 128-bit identifier defined in header file located
in application root folder (e.g., `apps/app1/app1_ta.h`). To generate this ID,
use [UUID online generator](http://www.itu.int/ITU-T/asn1/uuid.html) or other.

### 2.3. Write the application implementation
Implementation of Non-Secure part of application must be located in `ca` subfolder
of application root folder, and implementation of Secure part must be located
in `ta` subfolder. There are no other strict requirements.

### 2.4. Add application into init sequence.
#### 2.4.1. Edit arch/your_platform/nonsecure/main_ns.c file 
For example, to add a task into init sequence for m2351 board, add a following code
into `arch/cortex-m23/m2351/src/numaker_pfm_m2351/main_ns.c`:

```
#ifdef CONFIG_APPS_APP1
  xTaskCreate( app1_test,       /* The function that implements the task. */
      "app1_test",             /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );
#endif
```

#### 2.4.2. Edit arch/your_platform/nonsecure/Make.defs file 
Add new application Non-secure part into build sequence by adding a following
fragment into `Make.defs` file in the same folder:
 
```
ifeq ($(CONFIG_APPS_APP1),y)
CHIP_CSRCS_NS += $(TOPDIR)/apps/app1/ca/app1_ns.c
endif
```

#### 2.4.3. Edit arch/your_platform/nonsecure/Makefile file 
Add new application Non-secure part folder into build sequence by adding a following
fragment into `Makefile` file in the same folder:

```ifeq ($(CONFIG_APPS_APP1),y)
CFLAGS += -I$(TOPDIR)/apps/app1
endif
```

#### 2.4.4. Edit arch/your_platform/secure/Make.defs file 
Add new application Secure part into build sequence by adding a following
fragment into `Make.defs` file in the `secure` subfolder:
 
```
ifeq ($(CONFIG_APPS_APP1),y)
CHIP_CSRCS_S += $(TOPDIR)/apps/app1/ta/app1_s.c
endif
```

#### 2.4.5. Edit arch/your_platform/secure/Makefile file 
Add new application Non-secure part folder into build sequence by adding a following
fragment into `Makefile` file in the `secure` folder:

```
ifeq ($(CONFIG_APPS_APP1),y)
CFLAGS += -I$(TOPDIR)/apps/app1
endif
```

## 3. Changing configuration to include new application into build
Execute following shell commands:

```
$ make clean
$ make menuconfig
```

In the Kconfig menu that will open, select new application for build and close the menu, saving changes. After that, build new application by calling

```
$ make
```
