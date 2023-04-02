/**
 * @file Bridge_Freedom-metal_FreeRTOS.h
 * @author Pierre-Henry Moussay (pierre-henry.moussay@sifive.com)
 * @brief Bridge_Freedom-metal_FreeRTOS.h.in template for FreeRTOS
 * @version 0.1
 * @date 2020-02-26
 * 
 * @copyright Copyright (c) 2020 SiFive, Inc
 * @copyright SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef BRIDGE_FREEDOM_METAL_FREERTOS_H
#define BRIDGE_FREEDOM_METAL_FREERTOS_H

#include <metal-platform.h>

#if defined(METAL_RISCV_CLINT0)
#define MTIME_CTRL_ADDR               METAL_RISCV_CLINT0_0_BASE_ADDRESS
#elif defined(METAL_SIFIVE_CLIC0)
#define MTIME_CTRL_ADDR               METAL_SIFIVE_CLIC0_0_BASE_ADDRESS
#endif

#define MTIME_RATE_HZ               32768 

#define portHANDLE_INTERRUPT        FreedomMetal_InterruptHandler
#define portHANDLE_EXCEPTION        FreedomMetal_ExceptionHandler

#define portUSING_MPU_WRAPPERS      1

#endif /* BRIDGE_FREEDOM_METAL_FREERTOS_H */