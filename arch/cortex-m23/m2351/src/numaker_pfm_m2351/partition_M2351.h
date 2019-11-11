/**
 * @file        arch/cortex-m23/m2351/m2351/src/numaker_pfm_m2351/partition_M2351.h
 * @brief       SAU configuration for secure/nonsecure region settings.
 *
 * @copyright   Copyright (c) 2019 Samsung Electronics Co., Ltd. All Rights Reserved.
 * @author      Taras Drozdovskyi t.drozdovsky@samsung.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ARCH_CORTEX_M23_M2351_SRC_NUMAKER_PFM_M2351_PARTITION_M2351_H_
#define __ARCH_CORTEX_M23_M2351_SRC_NUMAKER_PFM_M2351_PARTITION_M2351_H_

/* Included Files */
#include "M2351.h"
#include "config.h"

/* Pre-processor Definitions */
/* All C pre-processor macros are defined here. */

#define SCU_SECURE_SRAM_SIZE    CONFIG_SCU_SECURE_SRAM_SIZE
#define NON_SECURE_SRAM_BASE    (0x30000000 + CONFIG_SCU_SECURE_SRAM_SIZE)

#define FMC_INIT_NSBA          1

#define FMC_SECURE_ROM_SIZE      0x40000
#define FMC_NON_SECURE_BASE     (0x10000000 + FMC_SECURE_ROM_SIZE)

/* Peripheral Secure Attribution Configuration */
#ifdef CONFIG_USBH_NONSECURE
# define SCU_PNSSET0_USBH     SCU_PNSSET0_USBH_Msk
#else
# define SCU_PNSSET0_USBH     0x0ul
#endif

#ifdef CONFIG_SD0_NONSECURE
# define SCU_PNSSET0_SDH0     SCU_PNSSET0_SDH0_Msk
#else
# define SCU_PNSSET0_SDH0     0x0ul
#endif

#ifdef CONFIG_EBI_NONSECURE
# define SCU_PNSSET0_EBI      SCU_PNSSET0_EBI_Msk
#else
# define SCU_PNSSET0_EBI      0x0ul
#endif

#ifdef CONFIG_PDMA1_NONSECURE
# define SCU_PNSSET0_PDMA1    SCU_PNSSET0_PDMA1_Msk
#else
# define SCU_PNSSET0_PDMA1    0x0ul
#endif

#define SCU_INIT_PNSSET0_VAL (SCU_PNSSET0_USBH | SCU_PNSSET0_SDH0 | SCU_PNSSET0_EBI | SCU_PNSSET0_PDMA1)

#ifdef CONFIG_CRC_NONSECURE
# define SCU_PNSSET1_CRC      SCU_PNSSET1_CRC_Msk
#else
# define SCU_PNSSET1_CRC      0x0ul
#endif

#ifdef CONFIG_CRPT_NONSECURE
# define SCU_PNSSET1_CRPT     SCU_PNSSET1_CRPT_Msk
#else
# define SCU_PNSSET1_CRPT     0x0ul
#endif

#define SCU_INIT_PNSSET1_VAL (SCU_PNSSET1_CRC | SCU_PNSSET1_CRPT)

#ifdef CONFIG_RTC_NONSECURE
# define SCU_PNSSET2_RTC      SCU_PNSSET2_RTC_Msk
#else
# define SCU_PNSSET2_RTC      0x0ul
#endif

#ifdef CONFIG_EADC_NONSECURE
# define SCU_PNSSET2_EADC     SCU_PNSSET2_EADC_Msk
#else
# define SCU_PNSSET2_EADC     0x0ul
#endif

#ifdef CONFIG_ACMP01_NONSECURE
# define SCU_PNSSET2_ACMP01   SCU_PNSSET2_ACMP01_Msk
#else
# define SCU_PNSSET2_ACMP01   0x0ul
#endif

#ifdef CONFIG_DAC_NONSECURE
# define SCU_PNSSET2_DAC      SCU_PNSSET2_DAC_Msk
#else
# define SCU_PNSSET2_DAC      0x0ul
#endif

#ifdef CONFIG_I2S0_NONSECURE
# define SCU_PNSSET2_I2S0     SCU_PNSSET2_I2S0_Msk
#else
# define SCU_PNSSET2_I2S0     0x0ul
#endif

#ifdef CONFIG_OTG_NONSECURE
# define SCU_PNSSET2_OTG      SCU_PNSSET2_OTG_Msk
#else
# define SCU_PNSSET2_OTG      0x0ul
#endif

#ifdef CONFIG_TMR23_NONSECURE
# define SCU_PNSSET2_TMR23    SCU_PNSSET2_TMR23_Msk
#else
# define SCU_PNSSET2_TMR23    0x0ul
#endif

#ifdef CONFIG_EPWM0_NONSECURE
# define SCU_PNSSET2_EPWM0    SCU_PNSSET2_EPWM0_Msk
#else
# define SCU_PNSSET2_EPWM0    0x0ul
#endif

#ifdef CONFIG_EPWM1_NONSECURE
# define SCU_PNSSET2_EPWM1    SCU_PNSSET2_EPWM1_Msk
#else
# define SCU_PNSSET2_EPWM1    0x0ul
#endif

#ifdef CONFIG_BPWM0_NONSECURE
# define SCU_PNSSET2_BPWM0    SCU_PNSSET2_BPWM0_Msk
#else
# define SCU_PNSSET2_BPWM0    0x0ul
#endif

#ifdef CONFIG_BPWM1_NONSECURE
# define SCU_PNSSET2_BPWM1    SCU_PNSSET2_BPWM1_Msk
#else
# define SCU_PNSSET2_BPWM1    0x0ul
#endif

#define SCU_INIT_PNSSET2_VAL (SCU_PNSSET2_RTC | SCU_PNSSET2_EADC \
    | SCU_PNSSET2_ACMP01 | SCU_PNSSET2_DAC | SCU_PNSSET2_I2S0 | SCU_PNSSET2_OTG \
    | SCU_PNSSET2_TMR23 | SCU_PNSSET2_EPWM0 | SCU_PNSSET2_EPWM1 \
    | SCU_PNSSET2_BPWM0 | SCU_PNSSET2_BPWM1)

#ifdef CONFIG_QSPI0_NONSECURE
# define SCU_PNSSET3_QSPI0    SCU_PNSSET3_QSPI0_Msk
#else
# define SCU_PNSSET3_QSPI0    0x0ul
#endif

#ifdef CONFIG_SPI0_NONSECURE
# define SCU_PNSSET3_SPI0     SCU_PNSSET3_SPI0_Msk
#else
# define SCU_PNSSET3_SPI0     0x0ul
#endif

#ifdef CONFIG_SPI1_NONSECURE
# define SCU_PNSSET3_SPI1     SCU_PNSSET3_SPI1_Msk
#else
# define SCU_PNSSET3_SPI1     0x0ul
#endif

#ifdef CONFIG_SPI2_NONSECURE
# define SCU_PNSSET3_SPI2     SCU_PNSSET3_SPI2_Msk
#else
# define SCU_PNSSET3_SPI2     0x0ul
#endif

#ifdef CONFIG_SPI3_NONSECURE
# define SCU_PNSSET3_SPI3     SCU_PNSSET3_SPI3_Msk
#else
# define SCU_PNSSET3_SPI3     0x0ul
#endif

#ifdef CONFIG_UART0_NONSECURE
# define SCU_PNSSET3_UART0    SCU_PNSSET3_UART0_Msk
#else
# define SCU_PNSSET3_UART0    0x0ul
#endif

#ifdef CONFIG_UART1_NONSECURE
# define SCU_PNSSET3_UART1    SCU_PNSSET3_UART1_Msk
#else
# define SCU_PNSSET3_UART1    0x0ul
#endif

#ifdef CONFIG_UART2_NONSECURE
# define SCU_PNSSET3_UART2    SCU_PNSSET3_UART2_Msk
#else
# define SCU_PNSSET3_UART2    0x0ul
#endif

#ifdef CONFIG_UART3_NONSECURE
# define SCU_PNSSET3_UART3    SCU_PNSSET3_UART3_Msk
#else
# define SCU_PNSSET3_UART3    0x0ul
#endif

#ifdef CONFIG_UART4_NONSECURE
# define SCU_PNSSET3_UART4    SCU_PNSSET3_UART4_Msk
#else
# define SCU_PNSSET3_UART4    0x0ul
#endif

#ifdef CONFIG_UART5_NONSECURE
# define SCU_PNSSET3_UART5    SCU_PNSSET3_UART5_Msk
#else
# define SCU_PNSSET3_UART5    0x0ul
#endif

#define SCU_INIT_PNSSET3_VAL (SCU_PNSSET3_QSPI0 | SCU_PNSSET3_SPI0 \
    | SCU_PNSSET3_SPI1 | SCU_PNSSET3_SPI2 | SCU_PNSSET3_SPI3 | SCU_PNSSET3_UART0 \
    | SCU_PNSSET3_UART1 | SCU_PNSSET3_UART2 | SCU_PNSSET3_UART3 \
    | SCU_PNSSET3_UART4 | SCU_PNSSET3_UART5)

#ifdef CONFIG_I2C0_NONSECURE
# define SCU_PNSSET4_I2C0     SCU_PNSSET4_I2C0_Msk
#else
# define SCU_PNSSET4_I2C0     0x0ul
#endif

#ifdef CONFIG_I2C1_NONSECURE
# define SCU_PNSSET4_I2C1     SCU_PNSSET4_I2C1_Msk
#else
# define SCU_PNSSET4_I2C1     0x0ul
#endif

#ifdef CONFIG_I2C2_NONSECURE
# define SCU_PNSSET4_I2C2     SCU_PNSSET4_I2C2_Msk
#else
# define SCU_PNSSET4_I2C2     0x0ul
#endif

#ifdef CONFIG_SC0_NONSECURE
# define SCU_PNSSET4_SC0      SCU_PNSSET4_SC0_Msk
#else
# define SCU_PNSSET4_SC0      0x0ul
#endif

#ifdef CONFIG_SC1_NONSECURE
# define SCU_PNSSET4_SC1      SCU_PNSSET4_SC1_Msk
#else
# define SCU_PNSSET4_SC1      0x0ul
#endif

#ifdef CONFIG_SC2_NONSECURE
# define SCU_PNSSET4_SC2      SCU_PNSSET4_SC2_Msk
#else
# define SCU_PNSSET4_SC2      0x0ul
#endif

#define SCU_INIT_PNSSET4_VAL (SCU_PNSSET4_I2C0 | SCU_PNSSET4_I2C1 \
    | SCU_PNSSET4_I2C2 | SCU_PNSSET4_SC0 | SCU_PNSSET4_SC1 | SCU_PNSSET4_SC2)

#ifdef CONFIG_CAN0_NONSECURE
# define SCU_PNSSET5_CAN0     SCU_PNSSET5_CAN0_Msk
#else
# define SCU_PNSSET5_CAN0     0x0ul
#endif

#ifdef CONFIG_QEI0_NONSECURE
# define SCU_PNSSET5_QEI0     SCU_PNSSET5_QEI0_Msk
#else
# define SCU_PNSSET5_QEI0     0x0ul
#endif

#ifdef CONFIG_QEI1_NONSECURE
# define SCU_PNSSET5_QEI1     SCU_PNSSET5_QEI1_Msk
#else
# define SCU_PNSSET5_QEI1     0x0ul
#endif

#ifdef CONFIG_ECAP0_NONSECURE
# define SCU_PNSSET5_ECAP0    SCU_PNSSET5_ECAP0_Msk
#else
# define SCU_PNSSET5_ECAP0    0x0ul
#endif

#ifdef CONFIG_ECAP1_NONSECURE
# define SCU_PNSSET5_ECAP1    SCU_PNSSET5_ECAP1_Msk
#else
# define SCU_PNSSET5_ECAP1    0x0ul
#endif

#ifdef CONFIG_TRNG_NONSECURE
# define SCU_PNSSET5_TRNG     SCU_PNSSET5_TRNG_Msk
#else
# define SCU_PNSSET5_TRNG     0x0ul
#endif

#define SCU_INIT_PNSSET5_VAL (SCU_PNSSET5_CAN0 | SCU_PNSSET5_QEI0 \
    | SCU_PNSSET5_QEI1 | SCU_PNSSET5_ECAP0 | SCU_PNSSET5_ECAP1 | SCU_PNSSET5_TRNG)

#ifdef CONFIG_USBD_NONSECURE
# define SCU_PNSSET6_USBD     SCU_PNSSET6_USBD_Msk
#else
# define SCU_PNSSET6_USBD     0x0ul
#endif

#ifdef CONFIG_USCI0_NONSECURE
# define SCU_PNSSET6_USCI0    SCU_PNSSET6_USCI0_Msk
#else
# define SCU_PNSSET6_USCI0    0x0ul
#endif

#ifdef CONFIG_USCI1_NONSECURE
# define SCU_PNSSET6_USCI1    SCU_PNSSET6_USCI1_Msk
#else
# define SCU_PNSSET6_USCI1    0x0ul
#endif

#define SCU_INIT_PNSSET6_VAL (SCU_PNSSET6_USBD | SCU_PNSSET6_USCI0 \
    | SCU_PNSSET6_USCI1)

  /* GPIO Secure Attribution Configuration */
  /* Px     0 = Secure; 1 = Non-Secure */

  /* Set GPIO Port A to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PA
# define SCU_IONSSET_PA       SCU_IONSSET_PA_Msk
#else
# define SCU_IONSSET_PA       0x0ul
#endif

  /* Set GPIO Port B to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PB
# define SCU_IONSSET_PB       SCU_IONSSET_PB_Msk
#else
# define SCU_IONSSET_PB       0x0ul
#endif

  /* Set GPIO Port C to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PC
# define SCU_IONSSET_PC       SCU_IONSSET_PC_Msk
#else
# define SCU_IONSSET_PC       0x0ul
#endif

  /* Set GPIO Port D to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PD
# define SCU_IONSSET_PD       SCU_IONSSET_PD_Msk
#else
# define SCU_IONSSET_PD       0x0ul
#endif

  /* Set GPIO Port E to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PE
# define SCU_IONSSET_PE       SCU_IONSSET_PE_Msk
#else
# define SCU_IONSSET_PE       0x0ul
#endif

  /* Set GPIO Port F to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PF
# define SCU_IONSSET_PF       SCU_IONSSET_PF_Msk
#else
# define SCU_IONSSET_PF       0x0ul
#endif

  /* Set GPIO Port G to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PG
# define SCU_IONSSET_PG       SCU_IONSSET_PG_Msk
#else
# define SCU_IONSSET_PG       0x0ul
#endif

  /* Set GPIO Port H to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PH
# define SCU_IONSSET_PH       SCU_IONSSET_PH_Msk
#else
# define SCU_IONSSET_PH       0x0ul
#endif

#define SCU_INIT_IONSSET_VAL (SCU_IONSSET_PA | SCU_IONSSET_PB | SCU_IONSSET_PC \
    | SCU_IONSSET_PD | SCU_IONSSET_PE | SCU_IONSSET_PF | SCU_IONSSET_PG \
    | SCU_IONSSET_PH)

//#ifdef CONFIG_APB0IEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_APB0IEN_Msk);
//#endif
//
//#ifdef CONFIG_APB1IEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_APB1IEN_Msk);
//#endif
//
//#ifdef CONFIG_GPIOIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_GPIOIEN_Msk);
//#endif
//
//#ifdef CONFIG_EBIIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_EBIIEN_Msk);
//#endif
//
//#ifdef CONFIG_USBHIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_USBHIEN_Msk);
//#endif
//
//#ifdef CONFIG_CRCIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_CRCIEN_Msk);
//#endif
//
//#ifdef CONFIG_SDH0IEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_SDH0IEN_Msk);
//#endif
//
//#ifdef CONFIG_PDMA0IEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_PDMA0IEN_Msk);
//#endif
//
//#ifdef CONFIG_PDMA1IEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_PDMA1IEN_Msk);
//#endif
//
//#ifdef CONFIG_SRAM0IEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_SRAM0IEN_Msk);
//#endif
//
//#ifdef CONFIG_SRAM1IEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_SRAM1IEN_Msk);
//#endif
//
//#ifdef CONFIG_FMCIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_FMCIEN_Msk);
//#endif
//
//#ifdef CONFIG_FLASHIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_FLASHIEN_Msk);
//#endif
//
//#ifdef CONFIG_SCUIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_SCUIEN_Msk);
//#endif
//
//#ifdef CONFIG_SYSIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_SYSIEN_Msk);
//#endif
//
//#ifdef CONFIG_CRPTIEN_SECURE_VIOLATION_INTERRUPT
//  SCU_ENABLE_INT(SCU_SVIOIEN_CRPTIEN_Msk);
//#endif

/* Setup behavior of Sleep and Exception Handling */
#define SCB_CSR_AIRCR_INIT  1

/* Deep Sleep can be enabled by
     0 = Secure and Non-Secure state
     1 = Secure state only */
#define SCB_CSR_DEEPSLEEPS_VAL  0

/* System reset request accessible from
     0 = Secure and Non-Secure state
     1 = Secure state only */
#define SCB_AIRCR_SYSRESETREQS_VAL  0

/* Priority of Non-Secure exceptions is
     0 = Not altered
     1 = Lowered to 0x80-0xFF */
#define SCB_AIRCR_PRIS_VAL      0

/* Assign HardFault to be always secure for safe */
#define SCB_AIRCR_BFHFNMINS_VAL 0

/* max 128 SAU regions. SAU regions are defined in config.h */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (CONFIG_SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (CONFIG_SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U

#if defined (__SAU_PRESENT) && (__SAU_PRESENT == 1U)

#ifdef CONFIG_SAU_INIT_REGION0
# ifdef CONFIG_REGION0_SECURE
#   define SAU_INIT_NSC0 1
# else
#   define SAU_INIT_NSC0 0
# endif
# define SAU_INIT_REGION0 1
#endif

#ifdef CONFIG_SAU_INIT_REGION1
# ifdef CONFIG_REGION1_SECURE
#   define SAU_INIT_NSC1 1
# else
#   define SAU_INIT_NSC1 0
# endif
# define SAU_INIT_REGION1 1
#endif

#ifdef CONFIG_SAU_INIT_REGION2
# ifdef CONFIG_REGION2_SECURE
#   define SAU_INIT_NSC2 1
# else
#   define SAU_INIT_NSC2 0
# endif
# define SAU_INIT_REGION2 1
#endif

#ifdef CONFIG_SAU_INIT_REGION3
# ifdef CONFIG_REGION3_SECURE
#   define SAU_INIT_NSC3 1
# else
#   define SAU_INIT_NSC3 0
# endif
# define SAU_INIT_REGION3 1
#endif

#ifdef CONFIG_SAU_INIT_REGION4
# ifdef CONFIG_REGION4_SECURE
#   define SAU_INIT_NSC4 1
# else
#   define SAU_INIT_NSC4 0
# endif
# define SAU_INIT_REGION4 1
#endif

#ifdef CONFIG_SAU_INIT_REGION5
# ifdef CONFIG_REGION5_SECURE
#   define SAU_INIT_NSC5 1
# else
#   define SAU_INIT_NSC5 0
# endif
# define SAU_INIT_REGION5 1
#endif

#ifdef CONFIG_SAU_INIT_REGION6
# ifdef CONFIG_REGION6_SECURE
#   define SAU_INIT_NSC6 1
# else
#   define SAU_INIT_NSC6 0
# endif
# define SAU_INIT_REGION6 1
#endif

#ifdef CONFIG_SAU_INIT_REGION7
# ifdef CONFIG_REGION7_SECURE
#   define SAU_INIT_NSC7 1
# else
#   define SAU_INIT_NSC7 0
# endif
# define SAU_INIT_REGION7 1
#endif

  /* repeat this for all possible SAU regions */

#ifdef CONFIG_ENABLE_SAU
#define SAU_INIT_CTRL_ENABLE 1
#else
#define SAU_INIT_CTRL_ENABLE 0
#endif

#ifdef CONFIG_ALL_MEMORY_NONSECURE
#define SAU_INIT_CTRL_ALLNS 1
#else
#define SAU_INIT_CTRL_ALLNS 0
#endif

#ifdef CONFIG_SAU_CONTROL
# define SAU_INIT_CTRL 1
#endif

#endif /* defined (__SAU_PRESENT) && (__SAU_PRESENT == 1U) */

//#define SCU_SET_INTSSET(IRQn) ((uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL)))
//
//  NVIC->ITNS[0] =
//#ifdef  CONFIG_RTC_INTERRUPT_NONSECURE
//      SCU_SET_INTSSET(RTC_IRQn) |
//#endif
//#ifdef CONFIG_TAMPER_INTERRUPT_NONSECURE
//          SCU_SET_INTSSET(TAMPER_IRQn) |
//#endif
//#ifdef CONFIG_EINT0_INTERRUPT_NONSECURE
//          SCU_SET_INTSSET(EINT0_IRQn) |
//#endif
//#ifdef CONFIG_EINT1_INTERRUPT_NONSECURE
//          SCU_SET_INTSSET(EINT1_IRQn) |
//#endif
//#ifdef CONFIG_EINT2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EINT2_IRQn) |
//#endif
//#ifdef CONFIG_EINT3_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EINT3_IRQn) |
//#endif
//#ifdef CONFIG_EINT4_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EINT4_IRQn) |
//#endif
//#ifdef CONFIG_EINT5_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EINT5_IRQn) |
//#endif
//#ifdef CONFIG_GPA_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPA_IRQn) |
//#endif
//#ifdef CONFIG_GPB_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPB_IRQn) |
//#endif
//#ifdef CONFIG_GPC_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPC_IRQn) |
//#endif
//#ifdef CONFIG_GPD_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPD_IRQn) |
//#endif
//#ifdef CONFIG_GPE_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPE_IRQn) |
//#endif
//#ifdef CONFIG_GPF_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPF_IRQn) |
//#endif
//#ifdef CONFIG_QSPI0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(QSPI0_IRQn) |
//#endif
//#ifdef CONFIG_SPI0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SPI0_IRQn) |
//#endif
//#ifdef CONFIG_BRAKE0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(BRAKE0_IRQn) |
//#endif
//#ifdef CONFIG_EPWM0_P0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EPWM0_P0_IRQn) |
//#endif
//#ifdef CONFIG_EPWM0_P1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EPWM0_P1_IRQn) |
//#endif
//#ifdef CONFIG_EPWM0_P2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EPWM0_P2_IRQn) |
//#endif
//#ifdef CONFIG_BRAKE1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(BRAKE1_IRQn) |
//#endif
//#ifdef CONFIG_EPWM1_P0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EPWM1_P0_IRQn) |
//#endif
//#ifdef CONFIG_EPWM1_P1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EPWM1_P1_IRQn) |
//#endif
//#ifdef CONFIG_EPWM1_P2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EPWM1_P2_IRQn) |
//#endif
//  0;
//
//  NVIC->ITNS[1] =
//#ifdef CONFIG_TMR2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(TMR2_IRQn) |
//#endif
//#ifdef CONFIG_TMR3_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(TMR3_IRQn) |
//#endif
//#ifdef CONFIG_UART0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(UART0_IRQn) |
//#endif
//#ifdef CONFIG_UART1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(UART1_IRQn) |
//#endif
//#ifdef CONFIG_I2C0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(I2C0_IRQn) |
//#endif
//#ifdef CONFIG_I2C1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(I2C1_IRQn) |
//#endif
//#ifdef CONFIG_DAC_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(DAC_IRQn) |
//#endif
//#ifdef CONFIG_EADC0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EADC0_IRQn) |
//#endif
//#ifdef CONFIG_EADC1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EADC1_IRQn) |
//#endif
//#ifdef CONFIG_ACMP01_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(ACMP01_IRQn) |
//#endif
//#ifdef CONFIG_EADC2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EADC2_IRQn) |
//#endif
//#ifdef CONFIG_EADC3_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EADC3_IRQn) |
//#endif
//#ifdef CONFIG_UART2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(UART2_IRQn) |
//#endif
//#ifdef CONFIG_UART3_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(UART3_IRQn) |
//#endif
//#ifdef CONFIG_SPI1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SPI1_IRQn) |
//#endif
//#ifdef CONFIG_SPI2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SPI2_IRQn) |
//#endif
//#ifdef CONFIG_USBD_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(USBD_IRQn) |
//#endif
//#ifdef CONFIG_USBH_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(USBH_IRQn) |
//#endif
//#ifdef CONFIG_USBOTG_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(USBOTG_IRQn) |
//#endif
//#ifdef CONFIG_CAN0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(CAN0_IRQn) |
//#endif
//#ifdef CONFIG_SC0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SC0_IRQn) |
//#endif
//#ifdef CONFIG_SC1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SC1_IRQn) |
//#endif
//#ifdef CONFIG_SC2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SC2_IRQn) |
//#endif
//#ifdef CONFIG_SPI3_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SPI3_IRQn) |
//#endif
//  0;
//
//  NVIC->ITNS[2] =
//#ifdef CONFIG_SDH0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(SDH0_IRQn) |
//#endif
//#ifdef CONFIG_I2S0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(I2S0_IRQn) |
//#endif
//#ifdef CONFIG_CRYPTO_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(CRYPTO_IRQn) |
//#endif
//#ifdef CONFIG_GPG_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPG_IRQn) |
//#endif
//#ifdef CONFIG_EINT6_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EINT6_IRQn) |
//#endif
//#ifdef CONFIG_UART4_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(UART4_IRQn) |
//#endif
//#ifdef CONFIG_UART5_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(UART5_IRQn) |
//#endif
//#ifdef CONFIG_USCI0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(USCI0_IRQn) |
//#endif
//#ifdef CONFIG_USCI1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(USCI1_IRQn) |
//#endif
//#ifdef CONFIG_BPWM0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(BPWM0_IRQn) |
//#endif
//#ifdef CONFIG_BPWM1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(BPWM1_IRQn) |
//#endif
//#ifdef CONFIG_I2C2_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(I2C2_IRQn) |
//#endif
//#ifdef CONFIG_QEI0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(QEI0_IRQn) |
//#endif
//#ifdef CONFIG_QEI1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(QEI1_IRQn) |
//#endif
//#ifdef CONFIG_ECAP0_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(ECAP0_IRQn) |
//#endif
//#ifdef CONFIG_ECAP1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(ECAP1_IRQn) |
//#endif
//#ifdef CONFIG_GPH_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(GPH_IRQn) |
//#endif
//#ifdef CONFIG_EINT7_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(EINT7_IRQn) |
//#endif
//#ifdef CONFIG_USBH_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(USBH_IRQn) |
//#endif
//  0;
//
//  NVIC->ITNS[3] =
//#ifdef CONFIG_PDMA1_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(PDMA1_IRQn) |
//#endif
//#ifdef CONFIG_TRNG_INTERRUPT_NONSECURE
//  SCU_SET_INTSSET(TRNG_IRQn) |
//#endif
//  0;

/* Public Types */
/* Any types, enumerations, structures or unions are defined here. */


/* Public Data */
/* All data declarations with global scope appear here, preceded by
 * the definition EXTERN.
 */


/* Inline Functions */
/* Any static inline functions may be defined in this grouping.
 * Each is preceded by a function header similar to the below.
 */

/* Public Function Prototypes */
/* All global functions in the file are prototyped here. The keyword
 * extern or the definition EXTERN are never used with function
 * prototypes.
 */

#endif /* __ARCH_CORTEX_M23_M2351_SRC_NUMAKER_PFM_M2351_PARTITION_M2351_H_ */

