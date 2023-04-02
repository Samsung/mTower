/*
 * FreeRTOS Kernel V10.4.6
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

/* Standard includes. */
#include "string.h"

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

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
	static __attribute__ ((aligned(16))) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
	const StackType_t xISRStackTop = ( StackType_t ) &( xISRStack[ configISR_STACK_SIZE_WORDS & ~portBYTE_ALIGNMENT_MASK ] );

	/* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
	the task stacks, and so will legitimately appear in many positions within
	the ISR stack. */
	#define portISR_STACK_FILL_BYTE	0xee
#else
	extern const uint32_t __freertos_irq_stack_top[];
	const StackType_t xISRStackTop = ( StackType_t ) __freertos_irq_stack_top;
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void ) __attribute__(( weak ));

/*-----------------------------------------------------------*/

/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
const size_t uxTimerIncrementsForOneTick = ( size_t ) ( ( configCPU_CLOCK_HZ ) / ( configTICK_RATE_HZ ) ); /* Assumes increment won't go over 32-bits. */
uint32_t const ullMachineTimerCompareRegisterBase = configMTIMECMP_BASE_ADDRESS;
volatile uint64_t * pullMachineTimerCompareRegister = NULL;
BaseType_t TrapNetCounter = 0;
const BaseType_t  *pTrapNetCounter = &TrapNetCounter;

#if( portUSING_MPU_WRAPPERS == 1 )
/** Variable that contains the current privilege state */
volatile uint32_t privilege_status = ePortMACHINE_MODE;

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
//typedef void TCB_t;
//extern volatile TCB_t * volatile pxCurrentTCB;
#endif

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
	#warning This path not tested, or even compiled yet.

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
#endif
/*-----------------------------------------------------------*/

/**
 * @brief 
 * 
 * @param xIsrTop
 */
BaseType_t xPortFreeRTOSInit( void ) PRIVILEGED_FUNCTION
{
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

	return 0;
}
/*-----------------------------------------------------------*/

#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

	void vPortSetupTimerInterrupt( void )
	{
	uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
	volatile uint32_t * const pulTimeHigh = ( volatile uint32_t * const ) ( ( configMTIME_BASE_ADDRESS ) + 4UL ); /* 8-byte typer so high 32-bit word is 4 bytes up. */
	volatile uint32_t * const pulTimeLow = ( volatile uint32_t * const ) ( configMTIME_BASE_ADDRESS );
	volatile uint32_t ulHartId;

		__asm volatile( "csrr %0, mhartid" : "=r"( ulHartId ) );
		pullMachineTimerCompareRegister  = ( volatile uint64_t * ) ( ullMachineTimerCompareRegisterBase + ( ulHartId * sizeof( uint64_t ) ) );

		do
		{
			ulCurrentTimeHigh = *pulTimeHigh;
			ulCurrentTimeLow = *pulTimeLow;
		} while( ulCurrentTimeHigh != *pulTimeHigh );

		ullNextTime = ( uint64_t ) ulCurrentTimeHigh;
		ullNextTime <<= 32ULL; /* High 4-byte word is 32-bits up. */
		ullNextTime |= ( uint64_t ) ulCurrentTimeLow;
		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
		*pullMachineTimerCompareRegister = ullNextTime;

		/* Prepare the time to use after the next tick interrupt. */
		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
	}

#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIME_BASE_ADDRESS != 0 ) */
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
extern void xPortStartFirstTask( void );

	#if( configASSERT_DEFINED == 1 )
	{
		volatile uint32_t mtvec = 0;

		/* Check the least significant two bits of mtvec are 00 - indicating
		single vector mode. */
		__asm volatile( "csrr %0, mtvec" : "=r"( mtvec ) );
		configASSERT( ( mtvec & 0x03UL ) == 0x03 );

		/* Check alignment of the interrupt stack - which is the same as the
		stack that was being used by main() prior to the scheduler being
		started. */
		configASSERT( ( xISRStackTop & portBYTE_ALIGNMENT_MASK ) == 0 );

		#ifdef configISR_STACK_SIZE_WORDS
		{
			memset( ( void * ) xISRStack, portISR_STACK_FILL_BYTE, sizeof( xISRStack ) );
		}
		#endif	 /* configISR_STACK_SIZE_WORDS */
	}
	#endif /* configASSERT_DEFINED */

	/* If there is a CLINT then it is ok to use the default implementation
	in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
	configure whichever clock is to be used to generate the tick interrupt. */
	vPortSetupTimerInterrupt();

	#if( ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) )
	{
		/* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
		for external interrupt.  _RB_ What happens here when mtime is not present as
		with pulpino? */
		__asm volatile( "csrs mie, %0" :: "r"(0x880) );
	}
	#else
	{
		/* Enable external interrupts. */
		__asm volatile( "csrs mie, %0" :: "r"(0x800) );
	}
	#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */
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
	/* Not implemented. */
	for( ;; );
}
/*-----------------------------------------------------------*/

#if( portUSING_MPU_WRAPPERS == 1 )


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
	// int32_t lIndex;
	// uint32_t ul;

    // /**
    //  *  considered as unused in certain cases because of macro
    //  * configASSERT_DEFINED
    //  */
	// int32_t lResult __attribute__((unused)) = PMP_DEFAULT_ERROR;
	// size_t uxBaseAddressChecked = 0;

	// if(0 == xPmpInfo.granularity) {
	// 	lResult = init_pmp (&xPmpInfo);
	// 	#if( configASSERT_DEFINED == 1 )
	// 	{
	// 		configASSERT(0 <= lResult);
	// 	}
	// 	#endif
	// }

	// memset(xPMPSettings, 0, sizeof(xMPU_SETTINGS));

	// if( xRegions == NULL ) {
    //     /* No PMP regions are specified so allow access to all data section */

    //     /* Config stack start address */
    //     uxBaseAddressChecked = 0;
    //     lResult = addr_modifier (xPmpInfo.granularity,
    //                             (size_t) pxBottomOfStack,
    //                             &uxBaseAddressChecked);
	// 	#if( configASSERT_DEFINED == 1 )
	// 	{
	// 		configASSERT(0 <= lResult);
	// 	}
	// 	#endif

	// 	xPMPSettings->uxRegionBaseAddress[0] = uxBaseAddressChecked;

	// 	xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
    //             ((UBaseType_t)((portPMP_REGION_READ_WRITE) |
    //             (portPMP_REGION_ADDR_MATCH_NA4)) <<
    //             portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

	// 	xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
	// 		    ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

	// 	/* Config stack end address and TOR */
	// 	uxBaseAddressChecked = 0;
    //     lResult = addr_modifier (xPmpInfo.granularity,
    //                             (size_t) pxBottomOfStack + ( size_t ) ulStackDepth * sizeof( StackType_t ), 
    //                             &uxBaseAddressChecked);
	// 	#if( configASSERT_DEFINED == 1 )
	// 	{
	// 		configASSERT(0 <= lResult);
	// 	}
	// 	#endif

	// 	xPMPSettings->uxRegionBaseAddress[1] = uxBaseAddressChecked;

	// 	xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_END)] +=
	// 			((UBaseType_t)((portPMP_REGION_READ_WRITE) |
	// 			(portPMP_REGION_ADDR_MATCH_TOR)) <<
	// 			portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));

	// 	xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_END)] += 
	// 		((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));

	// 	/* Invalidate all other configurable regions. */
	// 	for( ul = 2; ul < portNUM_CONFIGURABLE_REGIONS_REAL (xPmpInfo.nb_pmp) + 2; ul++ )
	// 	{
    //         xPMPSettings->uxRegionBaseAddress[ul] = 0UL;
    //         xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] += 
    //             ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));
	// 	}
	// }
	// else
	// {
	// 	/* This function is called automatically when the task is created - in
	// 	which case the stack region parameters will be valid.  At all other
	// 	times the stack parameters will not be valid and it is assumed that the
	// 	stack region has already been configured. */
	// 	if( ulStackDepth > 0 )
	// 	{
    //         /* Config stack start address */
    //         uxBaseAddressChecked = 0;
    //         lResult = addr_modifier (xPmpInfo.granularity,
    //                                 (size_t) pxBottomOfStack,
    //                                 &uxBaseAddressChecked);
	// 		#if( configASSERT_DEFINED == 1 )
	// 		{
	//             configASSERT(0 <= lResult);
	// 		}
	// 		#endif

    //         xPMPSettings->uxRegionBaseAddress[0] = uxBaseAddressChecked;

    //         xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
    //                 ((UBaseType_t)((portPMP_REGION_READ_WRITE) |
    //                 (portPMP_REGION_ADDR_MATCH_NA4)) <<
    //                 portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

    //         xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START)] += 
    //                 ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

    //         /* Config stack end address and TOR */
    //         uxBaseAddressChecked = 0;
    //         lResult = addr_modifier (xPmpInfo.granularity,
    //                                 (size_t) pxBottomOfStack + ( size_t ) ulStackDepth * sizeof( StackType_t ), 
    //                                 &uxBaseAddressChecked);
	// 		#if( configASSERT_DEFINED == 1 )
	// 		{
	//             configASSERT(0 <= lResult);
	// 		}
	// 		#endif

    //         xPMPSettings->uxRegionBaseAddress[1] = uxBaseAddressChecked;

    //         xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_END)] +=
    //                 ((UBaseType_t)((portPMP_REGION_READ_WRITE) |
    //                 (portPMP_REGION_ADDR_MATCH_TOR)) <<
    //                 portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));

    //         xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_END)] += 
    //             ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));
	// 	}

	// 	lIndex = 0;

	// 	for( ul = 2; ul < (portNUM_CONFIGURABLE_REGIONS_REAL (xPmpInfo.nb_pmp) + 2); ul++ )
	// 	{
	// 		if( ( xRegions[ lIndex ] ).ulLengthInBytes > 0UL )
	// 		{
	// 			xPMPSettings->uxRegionBaseAddress[ul] = (size_t) xRegions[ lIndex ].pvBaseAddress;

	// 			xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] +=
	// 				((UBaseType_t)( xRegions[ lIndex ].ulParameters ) <<
	// 				portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));

	// 			xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] += 
	// 				((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));
	// 		}
	// 		else
	// 		{
	// 			/* Invalidate the region. */
	// 			xPMPSettings->uxRegionBaseAddress[ul] = 0UL;
    //             xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START + ul)] += 
    //                 ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START + ul));
	// 		}

	// 		lIndex++;
	// 	}

	// 	#if( configASSERT_DEFINED == 1 )
	// 	{
	// 		// check we do not want to configure unavailable regions
	// 		if(xPmpInfo.nb_pmp < MAX_PMP_REGION) {
	// 			configASSERT(xRegions[ lIndex ].ulLengthInBytes == 0UL);
	// 		}
	// 	}
	// 	#endif
	// }
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


