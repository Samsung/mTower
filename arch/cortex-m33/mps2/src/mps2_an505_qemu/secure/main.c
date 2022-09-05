/**
* @file        arch/cortex-m33/mps2/src/mps2_an505_qemu/secure/main.c
* @brief       Provides functionality to start secure world, initialize secure
*              and normal worlds, pass to execution to normal world.
*
* @copyright   Copyright (c) 2022 Samsung Electronics Co., Ltd. All Rights Reserved.
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

/* Included Files. */
#include <arm_cmse.h>
#include <stdio.h>
#include "config.h"
#include "version.h"
#include "tee_api_types.h"

#include "printf.h"
#include "ARMCM33_DSP_FP_TZ.h"
#include "partition_ARMCM33.h"

#include <stdint.h>

/* Pre-processor Definitions. */
#define NORMAL  "\e[0m"
#define BLACK   "\e[0;30m1"
#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define BLUE    "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN    "\e[0;36m"
#define GRAY    "\e[0;37m"

// /** Instruction Code of "B ."  */
// #define JUMP_HERE       0xe7fee7ff

/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */
/* typedef for NonSecure callback functions */
// typedef __NONSECURE_CALL int32_t (*NonSecure_funcptr)(uint32_t);
//typedef int32_t (*Secure_funcptr)(uint32_t);
//
/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */

void initUart1(void);
char _getchar(void);
void _putchar(char ch);

// static void SYS_Init(void);
// static void DEBUG_PORT_Init(void);
// static void Boot_Init(uint32_t u32BootBase);

///* Private Data. */
///* All static data definitions appear here. */
//NonSecure_funcptr pfNonSecure_LED_On = (NonSecure_funcptr) NULL;
//NonSecure_funcptr pfNonSecure_LED_Off = (NonSecure_funcptr) NULL;
//
///* Public Data. */
///* All data definitions with global scope appear here. */
//
//
void SysTick_Handler (void);

/* Public Function Prototypes */
#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
void menu_security_exception_example(void);
#endif

extern TEE_Result tee_cryp_init(void);

///* Private Functions. */
//
///**
// * @brief         SYS_Init - Init System Clock.
// *
// * @param         None
// *
// * @returns       None
// */
//static void SYS_Init(void)
//{
//  /* Enable PLL */
//  CLK->PLLCTL = CLK_PLLCTL_128MHz_HIRC;
//
//  /* Waiting for PLL stable */
//  while ((CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == 0)
//    ;
//
//  /* Set HCLK divider to 2 */
//  CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_HCLKDIV_Msk)) | 1;
//
//  /* Switch HCLK clock source to PLL */
//  CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk))
//      | CLK_CLKSEL0_HCLKSEL_PLL;
//
//  /* Enable UART clock */
//  CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk;
//
//  /* Select UART clock source */
//  CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk))
//      | CLK_CLKSEL1_UART0SEL_HIRC;
//
//  /* Update System Core Clock */
//  /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
//  //SystemCoreClockUpdate();
//  PllClock = 128000000;             // PLL
//  SystemCoreClock = 128000000 / 2;  // HCLK
//  CyclesPerUs = 64000000 / 1000000; // For SYS_SysTickDelay()
//
//  /* Init I/O Multi-function */
//  /* Set multi-function pins for UART0 RXD and TXD */
//  SYS->GPB_MFPH = (SYS->GPB_MFPH & (~(UART0_RXD_PB12_Msk | UART0_TXD_PB13_Msk)))
//      | UART0_RXD_PB12 | UART0_TXD_PB13;
//
//  /* Init for nonsecure code */
//  CLK->APBCLK0 |= CLK_APBCLK0_UART1CKEN_Msk;
//  CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART1SEL_Msk))
//      | CLK_CLKSEL1_UART1SEL_HIRC;
//  SYS->GPA_MFPL = (SYS->GPA_MFPL & (~UART1_RXD_PA2_Msk)) | UART1_RXD_PA2;
//  SYS->GPA_MFPL = (SYS->GPA_MFPL & (~UART1_TXD_PA3_Msk)) | UART1_TXD_PA3;
//
//}

/* Non-Secure Callable functions */
typedef int __attribute__((cmse_nonsecure_call)) ns_func_void(void);

void __attribute__((cmse_nonsecure_entry)) sec_sum(void)
{
  static uint32_t ticks = 0;

  ticks++;

  printf("Called secure function! #%d\n", ticks);
}

void nonsecure_init(void) {
  /* Set the address of the Vector Table of the Non-Secure */
  uint32_t *vtor = (uint32_t *) TZ_VTOR_TABLE_ADDR;
  // SCB_NS->VTOR = (uint32_t) 0x10200000;
  SCB_NS->VTOR = (uint32_t) 0x00200000;

  /* 
   * Update the Non-Secure Stack Pointer
   * (first element of the Vector Table)
   */
  __TZ_set_MSP_NS(*vtor);
  __TZ_set_PRIMASK_NS(0x0);

  /*
   * Jump to the reset handler (Second element of the Vector Table)
   * of the Non-Secure
   */
    ns_func_void *ns_reset = (ns_func_void*) (*(vtor + 1));
    // printCmseAddressInfo(ns_reset);

    ns_reset();
}
// /**
//  * @brief         Boot_Init - Boot_Init function is used to jump to next boot
//  *                code.
//  *
//  * @param         None
//  *
//  * @returns       None
//  */
// static void Boot_Init(uint32_t u32BootBase)
// {
//   NonSecure_funcptr fp;

//   /* SCB_NS.VTOR points to the Non-secure vector table base address. */
//   SCB_NS->VTOR = u32BootBase;

//   /* 1st Entry in the vector table is the Non-secure Main Stack Pointer. */
//   __TZ_set_MSP_NS(*((uint32_t *) SCB_NS->VTOR)); /* Set up MSP in Non-secure code */

//   /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
//   fp = ((NonSecure_funcptr) (*(((uint32_t *) SCB_NS->VTOR) + 1)));

//   /* Clear the LSB of the function address to indicate the function-call
//    will cause a state switch from Secure to Non-secure */
//   fp = cmse_nsfptr_create(fp);

//   /* Check if the Reset_Handler address is in Non-secure space */
//   if (cmse_is_nsfptr(fp) && (((uint32_t) fp & 0xf0000000) == 0x10000000)) {
//     printf("Jump to execute Non-secure FreeRTOS (BL33) ...\n");
//     fp(0); /* Non-secure function call */
//   } else {
//     /* Something went wrong */
//     printf("No code in non-secure region!\n");
//     printf("CPU will halted at non-secure state\n");

//     /* Set nonsecure MSP in nonsecure region */
//     __TZ_set_MSP_NS(NON_SECURE_SRAM_BASE + 512);

//     /* Try to halted in non-secure state (SRAM) */
//     M32(NON_SECURE_SRAM_BASE) = JUMP_HERE;
//     fp = (NonSecure_funcptr) (NON_SECURE_SRAM_BASE + 1);
//     fp(0);

//     while (1);
//   }
// }

// ///**
// // * @brief         DEBUG_PORT_Init - performs initialization of UART.
// // *
// // * @param         None
// // *
// // * @returns       None
// // */
// //static void DEBUG_PORT_Init(void)
// //{
// //  DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
// //  DEBUG_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
// //}
// //
/* Public Functions. */

/* Secure functions exported to NonSecure application. Must place in Non-secure
 * Callable */
/**
* @brief         Secure_func - .
*
* @param         None
*
* @returns       None
*/
__attribute__((cmse_nonsecure_entry)) int32_t Secure_func(void)
{
  printf("Secure NSC func\n");
#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
  menu_security_exception_example();
#endif

  return 1;
}

// ///**
// // * @brief         Secure_LED_On - .
// // *
// // * @param         None
// // *
// // * @returns       None
// // */
// //__NONSECURE_ENTRY
// //int32_t Secure_LED_On(uint32_t num)
// //{
// //  (void) num;
// //
// ////  printf("Secure LED On call by Non-secure\n");
// //  PA11 = 0;
// //  PD11 = 0;
// //  PD10 = 0;
// //
// ////  PA10 = 0;
// //  PB0 = 0;
// //
// //  return 0;
// //}
// //
// ///**
// // * @brief         Secure_LED_Off - .
// // *
// // * @param         None
// // *
// // * @returns       None
// // */
// //__NONSECURE_ENTRY
// //int32_t Secure_LED_Off(uint32_t num)
// //{
// //  (void) num;
// //
// ////  printf("Secure LED Off call by Non-secure\n");
// //  PA11 = 1;
// //  PD11 = 1;
// //  PD10 = 1;
// ////  PA10 = 1;
// //  PB0 = 1;
// //
// //  return 1;
// //}
// //
// ///* NonSecure callable function for NonSecure callback. */
// ///**
// // * @brief         Secure_LED_On_callback - .
// // *
// // * @param         None
// // *
// // * @returns       None
// // */
// //__NONSECURE_ENTRY
// //int32_t Secure_LED_On_callback(NonSecure_funcptr *callback)
// //{
// //  pfNonSecure_LED_On = (NonSecure_funcptr) cmse_nsfptr_create(callback);
// //  return 0;
// //}
// //
// ///**
// // * @brief         Secure_LED_Off_callback - .
// // *
// // * @param         None
// // *
// // * @returns       None
// // */
// //__NONSECURE_ENTRY
// //int32_t Secure_LED_Off_callback(NonSecure_funcptr *callback)
// //{
// //  pfNonSecure_LED_Off = (NonSecure_funcptr) cmse_nsfptr_create(callback);
// //  return 0;
// //}
// //
// ///* Secure LED control function */
// ///**
// // * @brief         LED_On - .
// // *
// // * @param         None
// // *
// // * @returns       None
// // */
// //int32_t LED_On(void)
// //{
// //  printf("Secure LED On\n");
// //  PA11 = 0;
// //  PB1 = 0;
// //  return 1;
// //}
// //
// ///**
// // * @brief         LED_Off - .
// // *
// // * @param         None
// // *
// // * @returns       None
// // */
// //int32_t LED_Off(void)
// //{
// //  printf("Secure LED Off\n");
// //  PA11 = 1;
// //  PB1 = 1;
// //  return 1;
// //}
// //
#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
void hw_security_exception_init(void)
{
 /* Init PC for Nonsecure LED control */
//  GPIO_SetMode(PC_NS, BIT1, GPIO_MODE_OUTPUT);

 /* Set Hard Fault to secure */
//  SCB->AIRCR = (0x05FA << 16) | ((SCB->AIRCR&0xfffful) & (~(1 << SCB_AIRCR_BFHFNMINS_Pos)));
//  SCB->AIRCR = (0x05FA << 16) | ((SCB->AIRCR & 0xfffful) | (1 << SCB_AIRCR_BFHFNMINS_Pos));

//  if (SCB->AIRCR & SCB_AIRCR_BFHFNMINS_Msk) {
//    printf("Non-secure Hard Fault handled by Non-secure code.\n");
//  } else {
//    printf("Non-secure Hard Fault handled by secure code.\n");
//  }
//  printf("SCB->AIRCR = 0x%08x\n", SCB->AIRCR);

//  NVIC_EnableIRQ(SCU_IRQn);
}
void menu_security_exception_example(void)
{
  char ch;
  int temp = 1;
//
//  printf("SCU->SVIOIEN = 0x%08x\n",SCU->SVIOIEN);
//  printf("SCU->SRAMNSSET = 0x%08x\n",SCU->SRAMNSSET);

  printf("+---------------------------------------------------------------------+\n");
  printf("| Type  | Range addresses         | Size      | Remarks               |\n");
  printf("+---------------------------------------------------------------------+\n");
  printf("| FLASH | 0x00000000 - 0x0003F000 | 0x3F000   | 252k                  |\n");
  printf("| RAM   | 0x20000000 - 0x20007FFF | 0x8000    | 32k                   |\n");
  printf("| NCS   | 0x0003F000 - 0x0003FFFF | 0x1000    | 4k                    |\n");
  printf("| GPIO  | PC                      |           | Port C is non-secure  |\n");
  printf("| GPIO  | PA,PB                   |           | Port A,B are secure   |\n");
  printf("+---------------------------------------------------------------------+\n\n");

  printf("+---------------------------------------------------------------------+\n");
  printf("| Key | Action                                   | Expected response  |\n");
  printf("+---------------------------------------------------------------------+\n");
  printf("| [1] | Read SRAM non-secure address 0x30017000  | Access successful  |\n");
  printf("|     | Read SRAM non-secure address 0x20017000  | RAZWI              |\n");
//  printf("| [2] | Read SRAM secure address 0x%08X      | Access successful  |\n",(unsigned int)&temp);
//  printf("|     | Read SRAM secure address 0x%08X      | RAZWI              |\n",(0x10000000 + (unsigned int)&temp));
  // printf("| [3] | Read FLASH secure address 0x00000000     | Access successful  |\n");
  // printf("|     | Read FLASH secure address 0x10000000     | RAZWI              |\n");
//  printf("| [4] | Read FLASH non-secure address 0x%08X | Access successful  |\n",0 /*CONFIG_START_ADDRESS_BL33*/);
//  printf("|     | Read FLASH non-secure address 0x%08X | RAZWI              |\n",(/*CONFIG_START_ADDRESS_BL33*/ - 0x10000000));
  // printf("| [5] | Read GPIO non-secure port PC1_NS         | Access successful  |\n");
  // printf("|     | Write 0 GPIO non-secure port by PC1_NS   | Access successful  |\n");
  // printf("|     | Write 1 GPIO non-secure port by PC1      | RAZWI              |\n");
  // printf("| [6] | Read GPIO secure port PA10               | Access successful  |\n");
  // printf("|     | Write 0 GPIO secure port by PA10         | Access successful  |\n");
  // printf("|     | Write 1 GPIO secure port by PA10_NS      | RAZWI, sec. violat.|\n");
  // printf("| [7] | Write 0 FLASH to address 0x0 (directly)  | Hard fault         |\n");
  // printf("| [8] | Read 0x20018000 address (nonexistent)    | Hard fault         |\n");
  printf("|[any]| Go non-secure code                       | Exec. non-sec code |\n");
  printf("+---------------------------------------------------------------------+\n");

  printf("\n[%c] ", ch = _getchar());

 switch (ch) {
   case '1':
//      printf("Read SRAM non-secure address 0x30017000 = %08x\n", M32(0x30017000));
//      printf("    Read SRAM non-secure address 0x20017000 = %08x\n", M32(0x20017000));
     break;
   case '2':
//      printf("Read SRAM secure address 0x%08X = %08x\n", (unsigned int)&temp, M32(&temp));
//      printf("    Read SRAM secure address 0x%08X = %08x\n",(0x10000000 + (unsigned int)&temp), M32(0x10000000 + (unsigned int)&temp));
     break;
   case '3':
//      printf("Read FLASH secure address 0x00000000 = %08x\n", M32(0x00000000));
//      printf("    Read FLASH secure address 0x10000000 = %08x\n", M32(0x10000000));
     break;
   case '4':
//      printf("Read FLASH non-secure address 0x%08X = %08x\n", CONFIG_START_ADDRESS_BL33, M32(CONFIG_START_ADDRESS_BL33));
//      printf("    Read FLASH non-secure address 0x%08X = %08x\n", (CONFIG_START_ADDRESS_BL33 - 0x10000000), M32(CONFIG_START_ADDRESS_BL33 - 0x10000000));
     break;
   case '5':
//      printf("Read GPIO non-secure port PC1_NS = %d\n", PC1_NS);
//      printf("    Write 0 GPIO non-secure port by PC1_NS.");
//      PC1_NS = 0;
//      printf(" Result: PC1_NS = %d\n", PC1_NS);
//      printf("    Write 1 GPIO non-secure port by PC1.");
//      PC1 = 1;
//      printf(" Result: PC1_NS = %d\n", PC1_NS);
     break;
   case '6':
//      printf("    Read GPIO secure port PA10 = %d\n", PA10);
//      printf("    Write 0 GPIO secure port by PA10.");
//      PA10 = 0;
//      printf(" Result: PA10 = %d\n", PA10);
//      printf("    Write 1 GPIO secure port by PA10_NS. Result: GPIO violation interrupt\n");
//      PA10_NS = 1;
     break;
   case '7':
//      M32(0) = 0;
     break;
   case '8':
//      M32(0x20018000);
     break;
   default:
     break;
 }
}
#endif

#ifdef CONFIG_APPS_SPY
const char mal_detected[] = ""
"\n               MMMM      MMMM      MMMM"
"\n               MMMM      MMMM      MMMM"
"\n               MMMM      MMMM      MMMM"
"\n          -ohMMMMMMMMMMMMMMMMMMMMMMMMMMMMho-"
"\n        /dMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMd/"
"\n      `hMMMM/`                          `/MMMMy`"
"\n      hMMN/        "RED"MALWARE DETECTED!!!"GREEN"     /MMMh"
"\n      MMM+                                  oMMM"
"\n MMMMMMMM          .odNMMMMMMMMMNms-         MMMMMMMM"
"\n MMMMMMMM        `omMMMMMMMMMMMMMMMNs`       MMMMMMMM"
"\n      MMM       `yMMMMMMMMMMMMMMMMMMMh.      MMM"
"\n      MMM       +MMMMNddmMMMMMmddNMMMMs      MMM"
"\n      MMM       hMMMd-``./MMMo.``-yMMMm`     MMM"
"\n MMMMMMMM       hMMMs    `MMM-    +MMMm`     MMMMMMMM"
"\n MMMMMMMM       +MMMNy==+MMMMMo==sNMMMy      MMMMMMMM"
"\n      MMM       `hMMMMMMMMh  MMMMMMMMd.      MMM"
"\n      MMM        `sNMMMMMMb  dMMMMMNy.       MMM"
"\n      MMM          -yMMMMMMMMMMMMMd:`        MMM"
"\n MMMMMMMM           :MMM  MMM  MMM+          MMMMMMMM"
"\n MMMMMMMM           `:MM  MMM  MM:`          MMMMMMMM"
"\n      MMM+                                   MMM"
"\n      hMMM/        "RED"MALWARE DETECTED!!!"GREEN"     /MMMh"
"\n      `hMMMh/`                          `/hMMMy`"
"\n        /dMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMd:"
"\n          -ohMMMMMMMMMMMMMMMMMMMMMMMMMMMMho-"
"\n               MMMM      MMMM      MMMM"
"\n               MMMM      MMMM      MMMM"
"\n               MMMM      MMMM      MMMM\n"
"\n\t\t "RED"PLEASE RESTART BOARD!!!\n";
#endif

void HardFault_Handler(void)
{
  printf(RED "Secure Hard Fault Handler: invalid memory access or malware activity detected\n" NORMAL);
#ifdef CONFIG_APPS_SPY
  printf("%s\n", mal_detected);
#endif
  while(1);
}

void SCU_IRQHandler(void)
{
  printf(RED "SCU_IRQHandler detected: ");
// //#ifdef CONFIG_APB0IEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_APB0IEN_Msk) == SCU_SVIOIEN_APB0IEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_APB0IEN_Msk;
// //    printf("APB0 ");
// //  }
// //#endif
// //#ifdef CONFIG_APB1IEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_APB1IEN_Msk) == SCU_SVIOIEN_APB1IEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_APB1IEN_Msk;
// //    printf("APB1 ");
// //  }
// //#endif
// //#ifdef CONFIG_GPIOIEN_SECURE_VIOLATION_INTERRUPT
// //  if ((SCU->SVINTSTS & SCU_SVIOIEN_GPIOIEN_Msk) == SCU_SVIOIEN_GPIOIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_GPIOIEN_Msk;
// //    printf("GPIO ");
// //  }
// //#endif
// //#ifdef CONFIG_EBIIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_EBIIEN_Msk) == SCU_SVIOIEN_EBIIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_EBIIEN_Msk;
// //    printf("EBI ");
// //  }
// //#endif
// //#ifdef CONFIG_USBHIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_USBHIEN_Msk) == SCU_SVIOIEN_USBHIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_USBHIEN_Msk;
// //    printf("USBH ");
// //  }
// //#endif
// //#ifdef CONFIG_CRCIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_CRCIEN_Msk) == SCU_SVIOIEN_CRCIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_CRCIEN_Msk;
// //    printf("CRC ");
// //  }
// //#endif
// //#ifdef CONFIG_SDH0IEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_SDH0IEN_Msk) == SCU_SVIOIEN_SDH0IEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_SDH0IEN_Msk;
// //    printf("SDH0 ");
// //  }
// //#endif
// //#ifdef CONFIG_PDMA0IEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_PDMA0IEN_Msk) == SCU_SVIOIEN_PDMA0IEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_PDMA0IEN_Msk;
// //    printf("PDMA0 ");
// //  }
// //#endif
// //#ifdef CONFIG_PDMA1IEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_PDMA1IEN_Msk) == SCU_SVIOIEN_PDMA1IEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_PDMA1IEN_Msk;
// //    printf("PDMA1 ");
// //  }
// //#endif
// //#ifdef CONFIG_SRAM0IEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_SRAM0IEN_Msk) == SCU_SVIOIEN_SRAM0IEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_SRAM0IEN_Msk;
// //    printf("SRAM0 ");
// //  }
// //#endif
// //#ifdef CONFIG_SRAM1IEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_SRAM1IEN_Msk) == SCU_SVIOIEN_SRAM1IEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_SRAM1IEN_Msk;
// //    printf("SRAM1 ");
// //  }
// //#endif
// //#ifdef CONFIG_FMCIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_FMCIEN_Msk) == SCU_SVIOIEN_FMCIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_FMCIEN_Msk;
// //    printf("FMC ");
// //  }
// //#endif
// //#ifdef CONFIG_FLASHIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_FLASHIEN_Msk) == SCU_SVIOIEN_FLASHIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_FLASHIEN_Msk;
// //    printf("FLASH ");
// //  }
// //#endif
// //#ifdef CONFIG_SCUIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_SCUIEN_Msk) == SCU_SVIOIEN_SCUIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_SCUIEN_Msk;
// //    printf("SCU ");
// //  }
// //#endif
// //#ifdef CONFIG_SYSIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_SYSIEN_Msk) == SCU_SVIOIEN_SYSIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_SYSIEN_Msk;
// //    printf("SYS ");
// //  }
// //#endif
// //#ifdef CONFIG_CRPTIEN_SECURE_VIOLATION_INTERRUPT
// //  if((SCU->SVINTSTS & SCU_SVIOIEN_CRPTIEN_Msk) == SCU_SVIOIEN_CRPTIEN_Msk) {
// //    SCU->SVINTSTS |= SCU_SVIOIEN_CRPTIEN_Msk;
// //    printf("CRPT ");
// //  }
// //#endif
  printf("violation interrupt event\n"NORMAL);
}
// //#endif

/**
* @brief         SysTick_Handler - SysTick IRQ Handler.
*
* @param         None
*
* @returns       None
*/
void SysTick_Handler(void)
{
  static uint32_t u32Ticks;

  switch (u32Ticks++) {
    // case 0:
    //   LED_On();
    //   break;
    // case 200:
    //   LED_Off();
    //   break;
    // case 300:
    //   if (pfNonSecure_LED_On != NULL) {
    //     pfNonSecure_LED_On(1u);
    //   }
    //   break;
    case 500:
      // logPrint("Hello world\n");
      // if (pfNonSecure_LED_Off != NULL) {
      //   pfNonSecure_LED_Off(1u);
      // }
      break;
    default:
      if (u32Ticks > 600) {
        u32Ticks = 0;
      }
  }
}

/**
 * @brief         main - entry point of mTower: secure world.
 *
 * @param         None
 *
 * @returns       None (function is not supposed to return)
 */
int main(void)
{
  SystemInit();

//   SystemCoreClockUpdate();

//  SYS_UnlockReg();
//
//  SYS_Init();
//
  /* UART is configured as debug port */
  initUart1();

  printf(NORMAL "\n\n\t-=mTower v" VERSION "=-  " __DATE__ "  " __TIME__"\n\n");

  printf("+---------------------------------------------+\n");
  printf("|     Secure handler (BL32) is running ...    |\n");
  printf("+---------------------------------------------+\n");

//  /* Init GPIO Port A for secure LED control */
//  GPIO_SetMode(PA, BIT11 | BIT10, GPIO_MODE_OUTPUT);
//
//  /* Init GPIO Port D for secure LED control */
//  GPIO_SetMode(PD, BIT11 | BIT10, GPIO_MODE_OUTPUT);
//
//  /* Init GPIO Port B for secure LED control */
//  GPIO_SetMode(PB, BIT1 | BIT0, GPIO_MODE_OUTPUT);
//
//  /* Generate Systick interrupt each 10 ms */
////  SysTick_Config(SystemCoreClock / 100);
//
#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
  hw_security_exception_init();
  menu_security_exception_example();
#endif

 tee_cryp_init();
//
//  Boot_Init(CONFIG_START_ADDRESS_BL33);

//  do {
//    __WFI();
//  } while (1);

    
    volatile uint32_t* spcb = 0x50080000 + 0x14;
    *spcb |= 1 | 2;

    // logPrint("SPCB content 0x%x\n", *spcb);
   printf("SPCB content 0x%x\n", *spcb);
   
    initMpc();

    /* 
    
    The Non-secure Callable Configuration register allows software to define
     callable regions of memory. The register can do this if the Secure Code
     region is 0x1000_0000 to 0x1FFF_FFFF and the Secure RAM region is
      0x3000_0000 to 0x3FFF_FFFF.
    */

    // printCmseAddressInfo(0x00000000);
    // printCmseAddressInfo(0x00200000);
    // printCmseAddressInfo(0x10000000);
    // printCmseAddressInfo(0x20000000);
    // printCmseAddressInfo(SAU_INIT_START2);
    // printCmseAddressInfo(SAU_INIT_END2);

    // volatile uint32_t* mytest5 = 0x50080000 + 0xB0;
    // *mytest5 |= 0xFFFFFFFF;
    // *mytest5 |= 2;
  
    // volatile uint32_t* mytest4 = 0x50080000 + 0xB4;
    // *mytest4 |= (1 << 5);
    // *mytest4 |= 0xFFFFFFFF;
    // *mytest4 |= 2;
    
    // volatile uint32_t* mytest3 = 0x50080000 + 0x70;
    // *mytest3 |= 0xFFFFFFFF;
    // *mytest3 |= 2;
  
    // volatile uint32_t* mytest2 = 0x50080000 + 0x74;
    // *mytest2 |= 0xFFFFFFFF;
    // *mytest2 |= 2;
    
    // volatile uint32_t* mytest1 = 0x50080000 + 0xC4;
    // *mytest1 |= (1 << 5);
    // // *mytest1 |= 0xFFFFFFFF;
    // *mytest1 |= 2;

    // work
    // volatile uint32_t* mytest = 0x50080000 + 0x84;
    // *mytest |= (1 << 5);

    // *mytest |= 0xFFFFFFFF;
    // // *mytest |= 2;

  // SystemCoreClockUpdate();
  // SysTick_Config(SystemCoreClock / 1000);  /* Generate interrupt each 10 ms */
  // TZ_SysTick_Config_NS(SystemCoreClock / 1000);  /* Generate interrupt each 10 ms */

    // while (1)
    // {
    //   __NOP();
    // }

  volatile uint32_t* mytest = 0x50080000 + 0x84;
  // *mytest &= ~(1 << 5);

  // volatile uint32_t* mytest = 0x50080000 + 0x84;
  *mytest |= (1 << 5);

    // volatile uint32_t* mytest = 0x50080000 + 0x84;
    // *mytest |= (1 << 5);

    /* Jump to Non-Secure main address */
    nonsecure_init();

    while (1)
    {
      __NOP();
      printf("Should not happen..\n");
    }
    return -1;

}

/*------------------- Memory Protection Controller -----------------------------*/
typedef struct /* see "ARM CoreLink SSE-200 Subsystem Technical Reference Manual r1p0" */
{
  __IOM  uint32_t CTRL;                     /* Offset: 0x000 (R/W) Control Register */
         uint32_t RESERVED0[3];
  __IM   uint32_t BLK_MAX;                  /* Offset: 0x010 (R/ ) Block Maximum Register */
  __IM   uint32_t BLK_CFG;                  /* Offset: 0x014 (R/ ) Block Configuration Register */
  __IOM  uint32_t BLK_IDX;                  /* Offset: 0x018 (R/W) Block Index Register */
  __IOM  uint32_t BLK_LUT;                  /* Offset: 0x01C (R/W) Block Lookup Tabe Register */
  __IM   uint32_t INT_STAT;                 /* Offset: 0x020 (R/ ) Interrupt Status Register */
  __OM   uint32_t INT_CLEAR;                /* Offset: 0x024 ( /W) Interrupt Clear Register */
  __IOM  uint32_t INT_EN;                   /* Offset: 0x028 (R/W) Interrupt Enable Register */
  __IM   uint32_t INT_INFO1;                /* Offset: 0x02C (R/ ) Interrupt Info1 Register */
  __IM   uint32_t INT_INFO2;                /* Offset: 0x030 (R/ ) Interrupt Info2 Register */
  __OM   uint32_t INT_SET;                  /* Offset: 0x034 ( /W) Interrupt Set Register */
} MPC_TypeDef;

#define MPC_SRAM0 ((MPC_TypeDef *) 0x58007000) /* Control for 0x[01]0000000 */
//https://developer.arm.com/documentation/101104/0200/programmers-model/base-element/secure-privilege-control-block
#define SPCBlock_Base ((uint32_t*)0x58008000)

void printCmseAddressInfo(uint32_t addr)
{
    cmse_address_info_t c = cmse_TTA(addr);
    printf("Address: 0x%x\n", addr);
    printf(" |mpu_region: %x\n", c.flags.mpu_region);
    printf(" |sau_region: %x\n", c.flags.sau_region);
    printf(" |mpu_region_valid: %x\n", c.flags.mpu_region_valid);
    printf(" |sau_region_valid: %x\n", c.flags.sau_region_valid);
    printf(" |read_ok: %x\n", c.flags.read_ok);
    printf(" |readwrite_ok: %x\n", c.flags.readwrite_ok);
    printf(" |nonsecure_read_ok: %x\n", c.flags.nonsecure_read_ok);
    printf(" |nonsecure_readwrite_ok: %x\n", c.flags.nonsecure_readwrite_ok);
    printf(" |secure: %x\n", c.flags.secure);
    printf(" |idau_region_valid: %x\n", c.flags.idau_region_valid);
    printf(" |idau_region: %x\n", c.flags.idau_region);
}

void __aeabi_unwind_cpp_pr0()
{
}

void initMpc(void)
{
  uint32_t blockCtlr = MPC_SRAM0->CTRL;
  uint32_t blockMax = MPC_SRAM0->BLK_MAX;
  uint32_t blockSize = MPC_SRAM0->BLK_CFG & (0b1111); 
  uint32_t blockSizeByte = 1 << (blockSize + 5);

  printf("Config: %x\n", blockCtlr);
  printf(" | block max: %x\n", blockMax);
  printf(" | block size: 0x%x (0x%x bytes)\n", blockSize, blockSizeByte);

  uint32_t addrStartRam = 0x00200000;
  uint32_t addrStopRam  = 0x003fffff;
  // Find the corresponding group id.
  // BLK_IDX is the index of the group. If N, it selects Blocks
  //  [32 * N .. (32 * N+1) - 1] where 32 is the number of bits of the register.
  // Each block is M bytes depending on the block size where
  //  Block size = 1 << (BLK_CFG+5)
  // Which means that if we have address X, finding the corresponding
  // block is done by:
  //  Y = (X % 32) will be the bit index in the 32bit register
  //  Z = (Y % blockSize) will 
  uint32_t startGroup = (addrStartRam >> (blockSize + 5)) / 32;
  uint32_t endGroup = (addrStopRam >> (blockSize + 5)) / 32;

  if (endGroup > blockMax)
  {
    endGroup = blockMax;
  }
  printf("startGroup: 0x%x\n", startGroup);
  printf("endGroup: 0x%x\n", endGroup);

  MPC_SRAM0->CTRL |= 1 << 4;
  MPC_SRAM0->CTRL &= ~(1 << 8); // No autoincrement
  for (uint32_t i = startGroup; i <= endGroup; i++)
  {
    MPC_SRAM0->BLK_IDX = i;
    MPC_SRAM0->BLK_LUT = 0xffffffff;
  }

  MPC_SRAM0->CTRL |= (1 << 8); // autoincrement
  MPC_SRAM0->BLK_IDX = 0;
  for (uint32_t i = 0; i <= blockMax; i++)
  {
  //   printf("Value[0x%x]>", i);
  //   printf(" 0x%x-0x%x:\t\t",
  //            i * 32 * blockSizeByte,
  //            ((i+1) * (32*blockSizeByte)) - 1);
    // printf("0x%x\n", MPC_SRAM0->BLK_LUT);
    int dummy = MPC_SRAM0->BLK_LUT;
  }
  MPC_SRAM0->CTRL &= ~(1 << 8); // No autoincrement
}

#define UARTDR()    (*((volatile uint32_t *)(0x50201000 + 0x00)))
#define UARTSTATE() (*((volatile uint32_t *)(0x50201000 + 0x04)))
#define UARTCTRL()  (*((volatile uint32_t *)(0x50201000 + 0x08)))

void initUart1(void)
{
  /* RX_ENABLE, TX_ENABLE	*/
  UARTCTRL() = 3;
}

void _putchar(char ch)
{
  UARTDR() = ch;
  return ;
}

char _getchar(void)
{
  char ch;
  while (!(UARTSTATE() & 2));

  ch = UARTDR();
  return ch;
}