/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* ------------------------------------------------------------------
 * This file is part of the FreeRTOS distribution and was contributed
 * to the project by SiFive
 * 
 * Implementation of functions defined in portable.h for the RISC-V 
 * RV32 port.
 * ------------------------------------------------------------------
 */


/* Scheduler includes. */
#include "FreeRTOS.h"
#include <stdio.h>
#include "task.h"
#include "portmacro.h"
#include "string.h"

#include "csi_rv32_gcc.h"

PRIVILEGED_DATA __attribute__ ((aligned(16))) StackType_t xISRStackTop;
// PRIVILEGED_DATA __attribute__ ((aligned(128))) StackType_t xISRStackTop;

#ifdef configCLINT_BASE_ADDRESS
	#warning The configCLINT_BASE_ADDRESS constant has been deprecated.  configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS are currently being derived from the (possibly 0) configCLINT_BASE_ADDRESS setting.  Please update to define configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS dirctly in place of configCLINT_BASE_ADDRESS.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef configMTIME_BASE_ADDRESS
	#warning configMTIME_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtime register then set configMTIME_BASE_ADDRESS to the mapped address.  Otherwise set configMTIME_BASE_ADDRESS to 0.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef configMTIMECMP_BASE_ADDRESS
	#warning configMTIMECMP_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtimecmp register then set configMTIMECMP_BASE_ADDRESS to the mapped address.  Otherwise set configMTIMECMP_BASE_ADDRESS to 0.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
	#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
	#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void ) __attribute__( ( weak ) );

#if( configENABLE_FPU == 1 )
	/*
	 * Setup the Floating Point Unit (FPU).
	 */
	void prvSetupFPU( void ) PRIVILEGED_FUNCTION;
#endif /* configENABLE_FPU */

/*-----------------------------------------------------------*/

/* Used to program the machine timer compare register. */
//PRIVILEGED_DATA 
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
const size_t uxTimerIncrementsForOneTick = ( size_t ) ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ); /* Assumes increment won't go over 32-bits. */
//uint32_t const ullMachineTimerCompareRegisterBase = configMTIMECMP_BASE_ADDRESS;
// PRIVILEGED_DATA
volatile uint64_t * pullMachineTimerCompareRegister;
BaseType_t TrapNetCounter = 0;
const BaseType_t  *pTrapNetCounter = &TrapNetCounter;

#if( portUSING_MPU_WRAPPERS == 1 )
/** Variable that contains the current privilege state */
volatile __attribute__ ((aligned(128))) uint32_t privilege_status = ePortMACHINE_MODE;

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;
#endif

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
	#warning This path not tested, or even compiled yet.
	/* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
	the task stacks, and so will legitimately appear in many positions within
	the ISR stack. */
	#define portISR_STACK_FILL_BYTE	0xee

	static const uint8_t ucExpectedStackBytes[] = {
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE };	\

	#define portCHECK_ISR_STACK() configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else
	/* Define the function away. */
	#define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

#if( portUSING_MPU_WRAPPERS == 1 )
void vResetPrivilege( void ) __attribute__ (( naked ));

pmp_info_t xPmpInfo = {0,0};

BaseType_t xIsPrivileged( void )
{
    return(privilege_status == ePortMACHINE_MODE);
}

/**
 * @brief Setup of base region (first 3 regions)
 * @details those regions won't be reconfigured during context switch
 * 
 */
static void prvSetupPMP( void ) PRIVILEGED_FUNCTION
{
    extern uint32_t __unprivileged_section_start__[];
    extern uint32_t __unprivileged_section_end__[];

	uint8_t ucDefaultAttribute;
	size_t uxDefaultBaseAddr;
    /**
     *  considered as unused in certain cases because of macro
     * configASSERT_DEFINED
     */
	int32_t lResult __attribute__((unused)) = PMP_DEFAULT_ERROR;

	if(0 == xPmpInfo.granularity) {
		lResult = init_pmp (&xPmpInfo);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif
	}

	/* Check the expected PMP is present. */
	if( portMINIMAL_NB_PMP <= xPmpInfo.nb_pmp)
	{
		/* First setup the start address of the unprivilleged flash */
		ucDefaultAttribute = 0;
		uxDefaultBaseAddr = 0;

        lResult = addr_modifier (xPmpInfo.granularity,
                                ( size_t ) __unprivileged_section_start__,
                                &uxDefaultBaseAddr);
		if(lResult)
			printf("lResult [0] = %d\r\n", lResult);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif

		ucDefaultAttribute =
				((portPMP_REGION_READ_ONLY) |
				(portPMP_REGION_EXECUTE) |
				(portPMP_REGION_ADDR_MATCH_NA4));

		lResult = write_pmp_config (&xPmpInfo, portUNPRIVILEGED_EXECUTE_REGION_START,
                         ucDefaultAttribute, uxDefaultBaseAddr);
		if(lResult)
			printf("lResult [0] = %d\r\n", lResult);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif

		/* Setup the end address of the unprivilleged flash */
		ucDefaultAttribute = 0;
		uxDefaultBaseAddr = 0;
        lResult = addr_modifier (xPmpInfo.granularity,
                                ( size_t ) __unprivileged_section_end__,
                                &uxDefaultBaseAddr);
		if(lResult)
			printf("lResult [1] = %d\r\n", lResult);

		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif

		ucDefaultAttribute = ((portPMP_REGION_READ_ONLY) |
                            (portPMP_REGION_EXECUTE) |
                            (portPMP_REGION_ADDR_MATCH_TOR));

		lResult = write_pmp_config (&xPmpInfo, portUNPRIVILEGED_EXECUTE_REGION_END,
                         ucDefaultAttribute, uxDefaultBaseAddr);
		if(lResult)
			printf("lResult [1] = %d\r\n", lResult);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif

        /* Allow read only on the privilege status varibale */
        // ucDefaultAttribute = 0;
		// uxDefaultBaseAddr = 0;

        // lResult = addr_modifier (xPmpInfo.granularity,
        //                         ( size_t ) &privilege_status,
        //                         &uxDefaultBaseAddr);
		// if(lResult)
		// printf("lResult [2] = %d\r\n", lResult);
		// #if( configASSERT_DEFINED == 1 )
		// {
		// 	configASSERT(0 <= lResult);
		// }
		// #endif

		// ucDefaultAttribute =
		// 		((portPMP_REGION_READ_WRITE) |
		// 		// (portPMP_REGION_ADDR_MATCH_NAPOT));
		// 		(portPMP_REGION_ADDR_MATCH_NA4));			

		// lResult = write_pmp_config (&xPmpInfo, portPRIVILEGE_STATUS_REGION,
        //                  ucDefaultAttribute, uxDefaultBaseAddr);
		// printf("lResult [2] = %d\r\n", lResult);
		// #if( configASSERT_DEFINED == 1 )
		// {
		// 	configASSERT(0 <= lResult);
		// }
		// #endif
        ucDefaultAttribute = 0;
		uxDefaultBaseAddr = 0x2000A000;

        lResult = napot_addr_modifier (xPmpInfo.granularity,
                                ( size_t ) 0x2000A000,
                                &uxDefaultBaseAddr, 512);
		if(lResult)
			printf("lResult [2] = %d\r\n", lResult);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif

		ucDefaultAttribute =
				((portPMP_REGION_READ_WRITE) |
				(portPMP_REGION_ADDR_MATCH_NAPOT));
				// (portPMP_REGION_ADDR_MATCH_NA4));			

		lResult = write_pmp_config (&xPmpInfo, portPRIVILEGE_STATUS_REGION,
                         ucDefaultAttribute, uxDefaultBaseAddr);
		if(lResult)
			printf("lResult [2] = %d\r\n", lResult);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif
	}
}
#endif
/*-----------------------------------------------------------*/

#define CSR_STR(csr) #csr

#define READ_CSR(reg) \
  ({ \
     unsigned long tmp; \
     asm volatile("csrr %0, " CSR_STR(reg) : "=r"(tmp)); \
     tmp; \
  })

/**
 * @brief 
 * 
 * @param xIsrTop 
 */
BaseType_t xPortFreeRTOSInit( StackType_t xIsrTop ) PRIVILEGED_FUNCTION
{
	UBaseType_t uxHartid;

	extern BaseType_t xPortMoveISRStackTop( StackType_t *xISRStackTop);
	extern void freertos_risc_v_trap_handler(void);

	/** 
	 * We do the initialization of FreeRTOS priviledged data section
	 */
	#if( portUSING_MPU_WRAPPERS == 1 )
		{
			/** 
			 * Considered by the compiler as unused because of the inline asm block
			 */
			extern uint32_t __privileged_data_start__[] __attribute__((unused));
			extern uint32_t __privileged_data_end__[] __attribute__((unused));
			__asm__ __volatile__ (
				/* Zero the privileged_data segment. */
				"la t1, __privileged_data_start__ \n"
				"la t2, __privileged_data_end__ \n"

				"bge t1, t2, 2f \n"

				"1: \n"
				#if __riscv_xlen == 32
				"sw   x0, 0(t1) \n"
				"addi t1, t1, 4 \n"
				"blt  t1, t2, 1b \n"
				#else
				"sd   x0, 0(t1) \n"
				"addi t1, t1, 8 \n"
				"blt  t1, t2, 1b \n"
				#endif
				"2: \n"
				::
				: "t1", "t2"

			);

		}
	#endif

	/*
	* xIsrStack Is a Buffer Allocated into Application
	* xIsrStack_top is the Top adress of this Buffer 
	* it will contain  the isrstack and space or the registeries backup
	*
	*                 Top +----------------------+ xIsrTop
	*                     | ....                 |
	*                     |                      |
	*              Bottom +----------------------+ xISRStack
	*
	* stack mapping after :
	*                 Top +----------------------+ xIsrTop
	*                     | Space to store       |
	*                     | context before       |
	*                     | FreeRtos scheduling  |
	*                     | ....                 |
	*                     +----------------------+ xISRStackTop
	*                     | stack space for      |
	*                     | ISR execution        |
	*                     | ....                 |
	*                     |                      |
	*              Bottom +----------------------+ xISRStack
	*/

	xISRStackTop = xIsrTop;

	if ( 0 == xPortMoveISRStackTop(&xISRStackTop)){
		/* Error no enough place to store cntext or bad parameter */
		return -1;
	}

	#if( configASSERT_DEFINED == 1 )
	{
        /* Check alignment of the interrupt stack - which is the same as the
        stack that was being used by main() prior to the scheduler being
        started. */
        configASSERT( ( xISRStackTop & portBYTE_ALIGNMENT_MASK ) == 0 );
	}
	#endif /* configASSERT_DEFINED */

    __asm__ __volatile__ ("csrr %0, mhartid" : "=r"(uxHartid));

	pullMachineTimerCompareRegister = ( volatile uint64_t *) ( configMTIMECMP_BASE_ADDRESS + uxHartid * sizeof(uint64_t) );

	#if( configCLINT_BASE_ADDRESS != 0 )
		/* There is a clint then interrupts can branch directly to the FreeRTOS 
		* trap handler.
		*/
		// unsigned long tval;

		// tval = READ_CSR(0x305);
		// printf("mtval:%08x\r\n", (int)tval);
		// printf("freertos_risc_v_trap_handler:%p\r\n", freertos_risc_v_trap_handler);
		
		__asm__ __volatile__ (
			"la t0, freertos_risc_v_trap_handler\n"
			"csrw mtvec, t0\n"
		);

		// tval = READ_CSR(0x305);
		// printf("mtval:%08x\r\n", (int)tval);
		// printf("freertos_risc_v_trap_handler:%08x\r\n", freertos_risc_v_trap_handler);
	#else
		#warning "*** The interrupt controller must to be configured before (ouside of this file). ***"
	#endif

	#if( portUSING_MPU_WRAPPERS == 1 )
			/* Configure the regions in the PMP that are common to all tasks. */
		prvSetupPMP();
	#endif

	return 0;
}
/*-----------------------------------------------------------*/

/**
 * @brief Do the PMP config switch when switching task
 * @param ulNbPmp       number of configurable PMP
 * @param xPMPSettings  PMP configs stored in task TCB
 * @warning the number of configurable PMP is not the total number of PMP
 * 
 */
#if( portUSING_MPU_WRAPPERS == 1 )
__attribute__ (( naked )) void vPortPmpSwitch (	uint32_t ulNbPmp,
												xMPU_SETTINGS * xPMPSettings) PRIVILEGED_FUNCTION
#else
__attribute__ (( naked )) void vPortPmpSwitch (	uint32_t ulNbPmp) PRIVILEGED_FUNCTION
#endif
{
#if( portUSING_MPU_WRAPPERS == 1 )
	/**
	 * a0: xPmpInfo.nb_pmp - 3 (3 because We use 3 pmp config by default)
	 * a1: pxCurrentTCB->xPMPSettings (supposed to be the 2nd element of structure TCB_t)
	 */
	#if( __riscv_xlen == 32 )
		__asm__ __volatile__ (
			"addi a0, %0, -3	\n\t"
			"addi a1, %1, 4		\n\t"
			:: "r"(xPmpInfo.nb_pmp), "r"(pxCurrentTCB) : "a0", "a1"
		);
	#endif /* ( __riscv_xlen == 32 ) */
	#if( __riscv_xlen == 64 )
		__asm__ __volatile__ (
			"addi a0, %0, -3	\n\t"
			"addi a1, %1, 8		\n\t"
			:: "r"(xPmpInfo.nb_pmp), "r"(pxCurrentTCB) : "a0", "a1"
		);
	#endif /* ( __riscv_xlen == 64 ) */

    /* Compute jump offset to avoid configure unuse PMP */
	__asm__ __volatile__ (
		"li t0, 13 \n" /* maximum number of reconfigurable PMP for a core */
		"sub a2, t0, a0 \n"
		"slli a2, a2, 3 \n" /* compute the jump offset */
		::: "a0", "a2", "t0"
	);

    /* clear pmp config before setting addr */
#if __riscv_xlen == 32
    __asm__ __volatile__ (
        "li t1, 0x18181818 \n"
        /** 
         * we avoid disabling permanent PMP config, therefore those region mask
         * are set to 0
        */
        "li t2, 0x18000000 \n"
        "csrc pmpcfg0, t2 \n"
        "csrc pmpcfg1, t1 \n"
        "csrc pmpcfg2, t1 \n"
        "csrc pmpcfg3, t1 \n"
        :::"t1", "t2"
    );
#elif __riscv_xlen == 64
    __asm__ __volatile__ (
        "li t1, 0x1818181818181818 \n"
        /** 
         * we avoid disabling permanent PMP config, therefore those region mask
         * are set to 0
         */
        "li t2, 0x1818181818000000 \n"
        "csrc pmpcfg0, t2 \n"
        "csrc pmpcfg2, t1 \n"
        :::"t1", "t2"
    );
#endif

	/**
     * Save pmp address in pmpaddrx registers 
     * Please note that pmpaddr0, pmpaddr1, pmpaddr2 are not reconfigured,
     * they are configured once at the initialization of the scheduler
     */
#if __riscv_xlen == 32
	__asm__ __volatile__ (
		"add t0, a1, 32 \n" /* get pmp address configs */
        "la t1, 1f \n" /* compute the jump address */
        "add t2, t1, a2 \n"
        "jr t2 \n"
        "1: \n"
        "lw t2, 48(t0) \n"
        "csrw pmpaddr15, t2 \n"
        "lw t1, 44(t0) \n"
        "csrw pmpaddr14, t1 \n"
        "lw t2, 40(t0) \n"
        "csrw pmpaddr13, t2 \n"
        "lw t1, 36(t0) \n"
        "csrw pmpaddr12, t1 \n"
        "lw t2, 32(t0) \n"
        "csrw pmpaddr11, t2 \n"
        "lw t1, 28(t0) \n"
        "csrw pmpaddr10, t1 \n"
        "lw t2, 24(t0) \n"
        "csrw pmpaddr9, t2 \n"
        "lw t1, 20(t0) \n"
        "csrw pmpaddr8, t1 \n"
        "lw t2, 16(t0) \n"
        "csrw pmpaddr7, t2 \n"
        "lw t1, 12(t0) \n"
        "csrw pmpaddr6, t1 \n"
        "lw t2, 8(t0) \n"
        "csrw pmpaddr5, t2 \n"
        "lw t1, 4(t0) \n"
        "csrw pmpaddr4, t1 \n"
        "lw t2, 0(t0) \n"
        "csrw pmpaddr3, t2 \n"
        ::: "t0", "t1", "t2"
    );
#elif __riscv_xlen == 64
	__asm__ __volatile__ (
		"add t0, a1, 32 \n" /* get pmp address configs */
        "la t1, 1f \n" /* compute the jump address */
        "add t2, t1, a2 \n"
        "jr t2 \n"
        "1: \n"
        "ld t2, 96(t0) \n"
        "csrw pmpaddr15, t2 \n"
        "ld t1, 88(t0) \n"
        "csrw pmpaddr14, t1 \n"
        "ld t2, 80(t0) \n"
        "csrw pmpaddr13, t2 \n"
        "ld t1, 72(t0) \n"
        "csrw pmpaddr12, t1 \n"
        "ld t2, 64(t0) \n"
        "csrw pmpaddr11, t2 \n"
        "ld t1, 56(t0) \n"
        "csrw pmpaddr10, t1 \n"
        "ld t2, 48(t0) \n"
        "csrw pmpaddr9, t2 \n"
        "ld t1, 40(t0) \n"
        "csrw pmpaddr8, t1 \n"
        "ld t2, 32(t0) \n"
        "csrw pmpaddr7, t2 \n"
        "ld t1, 24(t0) \n"
        "csrw pmpaddr6, t1 \n"
        "ld t2, 16(t0) \n"
        "csrw pmpaddr5, t2 \n"
        "ld t1, 8(t0) \n"
        "csrw pmpaddr4, t1 \n"
        "ld t2, 0(t0) \n"
        "csrw pmpaddr3, t2 \n"
        ::: "t0", "t1", "t2"
    );
#endif

	
#if __riscv_xlen == 32
    /* Compute jump offset to avoid configure unuse PMP 32bits version */
	__asm__ __volatile__ (
        "addi a0, a0, 2 \n"
		"srli t1, a0, 2 \n" /* divide by 4 */
		"li t2, 3 \n" /* number of config regs (4) */
		"sub t2, t2, t1 \n"
		"slli t2, t2, 4 \n"
		::: "a0", "t1", "t2"
	);
#elif __riscv_xlen == 64
    /* Compute jump offset to avoid configure unuse PMP 64bits version */
	__asm__ __volatile__ (
        "addi a0, a0, 2 \n"
		"srli t1, a0, 3 \n" /* divide by 8 */
		"li t2, 1 \n" /* number of config regs (2) */
		"sub t2, t2, t1 \n"
		"slli t2, t2, 4 \n"
		::: "a0", "t1", "t2"
	);
#endif
		
	/* Configure PMP mode (rights and mode) */
#if __riscv_xlen == 32
	__asm__ __volatile__ (
		"add a0, a1, 16 \n" /* get pmp config mask */

        "la t0, 1f \n"
        "add t0, t0, t2 \n"
        "jr t0 \n"
        "1: \n"

		"lw t1, 12(a0) \n"
		"lw t2, 12(a1)\n"
		"csrc pmpcfg3, t1 \n"
        "csrs pmpcfg3, t2 \n" 

		"lw t1, 8(a0) \n"
		"lw t2, 8(a1)\n"
		"csrc pmpcfg2, t1 \n"
        "csrs pmpcfg2, t2 \n" 

		"lw t1, 4(a0) \n"
		"lw t2, 4(a1)\n"
		"csrc pmpcfg1, t1 \n"
        "csrs pmpcfg1, t2 \n" 

		"lw t1, 0(a0) \n"
		"lw t2, 0(a1)\n"
		"csrc pmpcfg0, t1 \n"
        "csrs pmpcfg0, t2 \n" 
        ::: "t0", "t1", "t2", "a0", "a1"
	);
#elif __riscv_xlen == 64
	__asm__ __volatile__ (
		"add a0, a1, 16 \n" /* get pmp config mask */

        "la t0, 1f \n"
        "add t0, t0, t2 \n"
        "jr t0 \n"
        "1: \n"

		"ld t1, 8(a0) \n"
		"ld t2, 8(a1)\n"
		"csrc pmpcfg2, t1 \n"
        "csrs pmpcfg2, t2 \n" 

		"ld t1, 0(a0) \n"
		"ld t2, 0(a1)\n"
		"csrc pmpcfg0, t1 \n"
        "csrs pmpcfg0, t2 \n" 
        ::: "t0", "t1", "t2", "a0", "a1"
	);
#endif
	__asm__ __volatile__ (
	    "fence.i \n"
		:::
	);
#endif /* ( portUSING_MPU_WRAPPERS == 1 ) */

	__asm__ __volatile__ (
	    "ret \n"
		:::
	);
}

/*-----------------------------------------------------------*/

#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

	void vPortSetupTimerInterrupt( void )
	{
        #if( __riscv_xlen == 32 )
			uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
			// volatile uint32_t * const pulTimeHigh = ( volatile uint32_t * const ) ( configCLINT_BASE_ADDRESS + 0xBFFC );
			// volatile uint32_t * const pulTimeLow = ( volatile uint32_t * const ) ( configCLINT_BASE_ADDRESS + 0xBFF8 );
			volatile uint32_t * const pulTimeHigh = ( volatile uint32_t * const ) ( ( configMTIME_BASE_ADDRESS ) + 4UL ); /* 8-byte typer so high 32-bit word is 4 bytes up. */
			volatile uint32_t * const pulTimeLow = ( volatile uint32_t * const ) ( configMTIME_BASE_ADDRESS );

        #endif /* __riscv_xlen == 32 */
        #if( __riscv_xlen == 64 )
			volatile uint64_t * const pulTime = ( volatile uint64_t * const ) ( configMTIME_BASE_ADDRESS );
        #endif /* __riscv_xlen == 64 */

        #if( __riscv_xlen == 32 )
			do
			{
				ulCurrentTimeHigh = *pulTimeHigh;
				ulCurrentTimeLow = *pulTimeLow;
			} while( ulCurrentTimeHigh != *pulTimeHigh );

			ullNextTime = ( uint64_t ) ulCurrentTimeHigh;
			ullNextTime <<= 32ULL;
			ullNextTime |= ( uint64_t ) ulCurrentTimeLow;
        #endif /* __riscv_xlen == 32 */
        #if( __riscv_xlen == 64 )
             ullNextTime = *pulTime;
        #endif /* __riscv_xlen == 64 */

		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
		*pullMachineTimerCompareRegister = ullNextTime;

		/* Prepare the time to use after the next tick interrupt. */
		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
		// printf("pulTimeLow = %p\r\n",pulTimeLow);
		// printf("pullMachineTimerCompareRegister = %p\r\n",pullMachineTimerCompareRegister);
		// printf("pulTimeLow = %x\r\n",*pulTimeLow);
		// printf("pulTimeHigh = %x\r\n",*pulTimeHigh);
		// printf("ullNextTime = %llx\r\n",ullNextTime);
		// printf("pullNextTime = %llx\r\n",*pullNextTime);
		// printf("pullNextTime = %p\r\n",pullNextTime);
		// printf("pullMachineTimerCompareRegister = %llx\r\n",*pullMachineTimerCompareRegister);
		// printf("uxTimerIncrementsForOneTick = %x\r\n", uxTimerIncrementsForOneTick);
	}

#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIME_BASE_ADDRESS != 0 ) */
/*-----------------------------------------------------------*/

/**
 * @brief Start scheduler
 * 
 * @return BaseType_t error code (pdFAIL or pdPASS)
 */
BaseType_t xPortStartScheduler( void ) PRIVILEGED_FUNCTION
{
	extern void xPortStartFirstTask( void );

	#if( configASSERT_DEFINED == 1 )
	{
		volatile uint32_t mtvec = 0;

		/* Check the least significant two bits of mtvec are 00 - indicating
		single vector mode. */
		__asm__ __volatile__ (
            "	csrr %0, mtvec		\n"
            : "=r"( mtvec )
		);
		// printf("mtvec = %x\r\n", mtvec);
		configASSERT( ( mtvec & 0x03UL ) == 0 );
	}
	#endif /* configASSERT_DEFINED */

	/* If there is a CLINT then it is ok to use the default implementation
	in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
	configure whichever clock is to be used to generate the tick interrupt. */
	vPortSetupTimerInterrupt();
	// #if( ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) )
	// {
	// 	/* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
	// 	for external interrupt.  _RB_ What happens here when mtime is not present as
	// 	with pulpino? */
	// 	__asm volatile( "csrs mie, %0" :: "r"(0x880) );
	// }
	// #else
	// {
	// 	/* Enable external interrupts. */
	// 	__asm volatile( "csrs mie, %0" :: "r"(0x800) );
	// }
	// #endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */
	extern void bflb_irq_enable(int irq);
	bflb_irq_enable(7);
	xPortStartFirstTask();

	/* Should not get here as after calling xPortStartFirstTask() only tasks
	should be executing. */
	return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	//extern void xPortRestoreBeforeFirstTask(void);

	//xPortRestoreBeforeFirstTask();

	/* 
	 * Should not get here as after calling xPortRestoreBeforeFirstTask() we should 
	 * return after de execution of xPortStartFirstTask in xPortStartScheduler function.
	 */
	for( ;; );
}
/*-----------------------------------------------------------*/

#if( portUSING_MPU_WRAPPERS == 1 )
__attribute__((naked)) void vPortSyscall( unsigned int Value ) PRIVILEGED_FUNCTION
{
	/* Remove compiler warning about unused parameter. */
	( void ) Value;

 	__asm__ __volatile__ ( 
        "	ecall 		\n"
        "	ret 		\n"
        :::
	);
}
/*-----------------------------------------------------------*/

__attribute__((naked)) void vRaisePrivilege( void ) PRIVILEGED_FUNCTION
{
	__asm__ __volatile__ (
		"	.extern privilege_status \n"
        "	li	a0,%0 		\n"
        "	ecall 			\n"
		"	la 	a0, privilege_status \n"
		"	li 	t0, %1 		\n"
		"	sw	t0, 0(a0)	\n" /* we use sw because privilege_status is uint32_t */
        "	ret 			\n"
        ::"i"(portSVC_SWITCH_TO_MACHINE), "i"(ePortMACHINE_MODE):
	);
}
/*-----------------------------------------------------------*/

__attribute__((naked)) void vResetPrivilege( void ) PRIVILEGED_FUNCTION
{
	__asm__ __volatile__ (
		"	.extern privilege_status \n"
        "	li	a0,%0 		\n"
        "	ecall 			\n"
		"	la 	a0, privilege_status \n"
		"	li 	t0, %1 		\n"
		"	sw	t0, 0(a0)	\n" /* we use sw because privilege_status is uint32_t */
        "	ret 			\n"
        ::"i"(portSVC_SWITCH_TO_USER), "i"(ePortUSER_MODE):
	);
}
/*-----------------------------------------------------------*/

/**
 * @brief Store PMP settings in Task TCB - the name this function
 * 		  is vPortStoreTaskMPUSettings in order to be MPU compliant
 * 
 * @param[out]  xPMPSettings    PMP settings stored in Task TCB
 * @param[in]   xRegions        PMP configuration of the task
 * @param[in]   pxBottomOfStack address of bottom of stack
 * @param[in]   ulStackDepth    size of stack
 */
void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xPMPSettings,
								const struct xMEMORY_REGION * const xRegions,
								StackType_t *pxBottomOfStack,
								uint32_t ulStackDepth ) PRIVILEGED_FUNCTION
{
	int32_t lIndex;
	uint32_t ul;

    /**
     *  considered as unused in certain cases because of macro
     * configASSERT_DEFINED
     */
	int32_t lResult __attribute__((unused)) = PMP_DEFAULT_ERROR;
	size_t uxBaseAddressChecked = 0;

	if(0 == xPmpInfo.granularity) {
		lResult = init_pmp (&xPmpInfo);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif
	}

	memset(xPMPSettings, 0, sizeof(xMPU_SETTINGS));

	if( xRegions == NULL ) {
        /* No PMP regions are specified so allow access to all data section */

        /* Config stack start address */
        uxBaseAddressChecked = 0;
        lResult = addr_modifier (xPmpInfo.granularity,
                                (size_t) pxBottomOfStack,
                                &uxBaseAddressChecked);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif

		xPMPSettings->uxRegionBaseAddress[0] = uxBaseAddressChecked;

		xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
                ((UBaseType_t)((portPMP_REGION_READ_WRITE) |
                (portPMP_REGION_ADDR_MATCH_NA4)) <<
                portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

		xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
			    ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

		/* Config stack end address and TOR */
		uxBaseAddressChecked = 0;
        lResult = addr_modifier (xPmpInfo.granularity,
                                (size_t) pxBottomOfStack + ( size_t ) ulStackDepth * sizeof( StackType_t ), 
                                &uxBaseAddressChecked);
		#if( configASSERT_DEFINED == 1 )
		{
			configASSERT(0 <= lResult);
		}
		#endif

		xPMPSettings->uxRegionBaseAddress[1] = uxBaseAddressChecked;

		xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_END)] +=
				((UBaseType_t)((portPMP_REGION_READ_WRITE) |
				(portPMP_REGION_ADDR_MATCH_TOR)) <<
				portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));

		xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_END)] += 
			((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));

		/* Invalidate all other configurable regions. */
		for( ul = 2; ul < portNUM_CONFIGURABLE_REGIONS_REAL (xPmpInfo.nb_pmp) + 2; ul++ )
		{
            xPMPSettings->uxRegionBaseAddress[ul] = 0UL;
            xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] += 
                ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));
		}
	}
	else
	{
		/* This function is called automatically when the task is created - in
		which case the stack region parameters will be valid.  At all other
		times the stack parameters will not be valid and it is assumed that the
		stack region has already been configured. */
		if( ulStackDepth > 0 )
		{
            /* Config stack start address */
            uxBaseAddressChecked = 0;
            lResult = addr_modifier (xPmpInfo.granularity,
                                    (size_t) pxBottomOfStack,
                                    &uxBaseAddressChecked);
			if(lResult)
				printf("lResult [3] = %d\r\n",lResult);
			#if( configASSERT_DEFINED == 1 )
			{
	            configASSERT(0 <= lResult);
			}
			#endif

            xPMPSettings->uxRegionBaseAddress[0] = uxBaseAddressChecked;

            xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
                    ((UBaseType_t)((portPMP_REGION_READ_WRITE) |
                    (portPMP_REGION_ADDR_MATCH_NA4)) <<
                    portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

            xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
                    ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

            /* Config stack end address and TOR */
            uxBaseAddressChecked = 0;
            lResult = addr_modifier (xPmpInfo.granularity,
                                    (size_t) pxBottomOfStack + ( size_t ) ulStackDepth * sizeof( StackType_t ), 
                                    &uxBaseAddressChecked);
			if(lResult)
				printf("lResult [4] = %d\r\n",lResult);
			#if( configASSERT_DEFINED == 1 )
			{
	            configASSERT(0 <= lResult);
			}
			#endif

            xPMPSettings->uxRegionBaseAddress[1] = uxBaseAddressChecked;

            xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_END)] +=
                    ((UBaseType_t)((portPMP_REGION_READ_WRITE) |
                    (portPMP_REGION_ADDR_MATCH_TOR)) <<
                    portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));

            xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_END)] += 
                ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));
		}

		lIndex = 0;

		for( ul = 2; ul < (portNUM_CONFIGURABLE_REGIONS_REAL (xPmpInfo.nb_pmp) + 2); ul++ )
		{
			if( ( xRegions[ lIndex ] ).ulLengthInBytes > 0UL )
			{
				xPMPSettings->uxRegionBaseAddress[ul] = (size_t) xRegions[ lIndex ].pvBaseAddress;

				xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] +=
					((UBaseType_t)( xRegions[ lIndex ].ulParameters ) <<
					portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));

				xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] += 
					((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));
			}
			else
			{
				/* Invalidate the region. */
				xPMPSettings->uxRegionBaseAddress[ul] = 0UL;
                xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] += 
                    ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));
			}

			lIndex++;
		}

		#if( configASSERT_DEFINED == 1 )
		{
			// check we do not want to configure unavailable regions
			if(xPmpInfo.nb_pmp < MAX_PMP_REGION) {
				configASSERT(xRegions[ lIndex ].ulLengthInBytes == 0UL);
			}
		}
		#endif
	}
}
/*-----------------------------------------------------------*/
#endif

__attribute__((naked)) void vPortUpdatePrivilegeStatus( UBaseType_t status ) PRIVILEGED_FUNCTION
{
	/* Remove compiler warning about unused parameter. */
	( void ) status;

	#if ( portUSING_MPU_WRAPPERS == 1 )
        __asm__ __volatile__(
			"	.extern privilege_status \n"
			"	li t0, 0x1800		\n"
			"	and t0, a0, t0		\n"
			"	srli t0, t0, 11		\n"
			"	la a0, privilege_status \n"
			"	sw	t0, 0(a0)"
            ::: "t0", "a0"
        );
	#endif /* if ( portUSING_MPU_WRAPPERS == 1 ) */

 	__asm__ __volatile__ ( 
        "	ret 		\n"
        :::
	);
}
/*-----------------------------------------------------------*/

#if ( portUSING_MPU_WRAPPERS == 1 )
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
										TaskFunction_t pxCode,
										void * pvParameters,
										BaseType_t xRunPrivileged ) PRIVILEGED_FUNCTION
#else /* if ( portUSING_MPU_WRAPPERS == 1 ) */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
										TaskFunction_t pxCode,
										void * pvParameters ) PRIVILEGED_FUNCTION
#endif /* if ( portUSING_MPU_WRAPPERS == 1 ) */
{
	UBaseType_t mstatus;
	extern StackType_t * pxPortAsmInitialiseStack( StackType_t *, TaskFunction_t, void *, UBaseType_t);

	/* Generate the value to set in mstatus. */
	#if( portUSING_MPU_WRAPPERS == 1 )
		/** 
		 * Generate the value:
		 * - 0x80   (MPIE set and MPP = usermode) if run in unprivilege mode (xRunPrivileged == 0). 
		 * - 0x1880 (MPIE set and MPP = machinemode) if run in privilege mode (xRunPrivileged != 0). 
		 */
			__enable_irq();

		__asm__ __volatile__ (
			"	csrr t0, mstatus	\n"		/* Obtain current mstatus value. */
			"	andi t0, t0, ~0x8	\n"		/* Ensure interrupts are disabled when the stack is restored within an ISR.  Required when a task is created after the schedulre has been started, otherwise interrupts would be disabled anyway. */
			"	addi t1, x0, 0x188	\n"		/* Generate the value 0x1880, which are the MPIE and MPP bits to set in mstatus. */
			"	slli t1, t1, 4		\n"
			"	not t2, t1			\n"		/* reset previous value */
			"	and t0, t0, t2		\n"
			"	mv t1, x0			\n"
			"	beqz a3, 1f			\n"
			"	addi t1, t1, 0x180	\n"
			"	1:					\n"
			"	ori t1, t1, 0x8		\n"
			"	slli t1, t1, 4		\n"
			"	or %0, t0, t1		\n"
			:"=r" ( mstatus )::"t0", "t1", "t2", "a3"
		);
	#else
		__asm__ __volatile__ (
			"	csrr t0, mstatus	\n"		/* Obtain current mstatus value. */
			"	andi t0, t0, ~0x8	\n"		/* Ensure interrupts are disabled when the stack is restored within an ISR.  Required when a task is created after the schedulre has been started, otherwise interrupts would be disabled anyway. */
			"	addi t1, x0, 0x188	\n"		/* Generate the value 0x1880, which are the MPIE and MPP bits to set in mstatus. */
			"	slli t1, t1, 4		\n"
			"	not t2, t1			\n"		/* reset previous value */
			"	and t0, t0, t2		\n"
			"	addi t1, x0, 0x188	\n"
			"	slli t1, t1, 4		\n"
			"	or %0, t0, t1		\n"
			:"=r" ( mstatus )::"t0", "t1", "t2"
		);
	#endif /* ( portUSING_MPU_WRAPPERS == 1 ) */

	return pxPortAsmInitialiseStack(pxTopOfStack, pxCode, pvParameters, mstatus);
}
/*-----------------------------------------------------------*/

#if( configENABLE_FPU == 1 )
	void prvSetupFPU( void ) /* PRIVILEGED_FUNCTION */
	{
		__asm__ __volatile__ (
			"csrr t0, misa \n"			/* Get misa */
			"li   t1, 0x10028 \n"		/* 0x10028 = Q,F,D Quad, Single or Double precission floating point */
			"and  t0, t0, t1 \n"
			"beqz t0, 1f \n"			/* check if Q,F or D is present into misa */
			"csrr t0, mstatus \n"		/* Floating point unit is present so need to put it into initial state */
			"lui  t1, 0x2 \n"			/* t1 =  0x1 << 12 */
			"or   t0, t0, t1 \n"
			"csrw mstatus, t0 \n"		/* Set FS to initial state */
			"csrwi fcsr, 0 \n"			/* Clear Floating-point Control and Status Register */
			"1: \n"
			:::
		);
	}
#endif /* configENABLE_FPU */
/*-----------------------------------------------------------*/
