/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"
#include "task.h"
#include "queue.h"

#include <printf.h>
#include "version.h">

#include "tzc_sec_reg.h"
#include "rv_hart.h"
#include "rv_pmp.h"

#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "bflb_flash.h"
#include "bl808_glb.h"
#include "bl808_psram_uhs.h"
#include "bl808_tzc_sec.h"
#include "bl808_ef_cfg.h"
#include "bl808_uhs_phy.h"
#include "board.h"

#include "mem.h"

#ifdef CONFIG_BSP_SDH_SDCARD
#include "sdh_sdcard.h"
#endif

struct bflb_device_s *uart0;
extern void bflb_uart_set_console(struct bflb_device_s *dev);

extern pmp_info_t xPmpInfo;

#define CONFIG_APPS_SPY
#define CONFIG_APPS_HELLO_WORLD

/* Priorities used by the tasks. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

#ifndef _RTL_
/* The 1s value is converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_TICK_COUNT_FOR_1S			pdMS_TO_TICKS( 1000 )
#else
/* For RTL Simulation we reduce the waiting timing, otherwise the simulation 
 * will take too more time */
/* The 10ms value is converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_TICK_COUNT_FOR_1S			pdMS_TO_TICKS( 10 )
#endif
/* The maximum number items the queue can hold.  The priority of the receiving
task is above the priority of the sending task, so the receiving task will
preempt the sending task and remove the queue items each time the sending task
writes to the queue.  Therefore the queue will never have more than one item in
it at any time, and even with a queue length of 1, the sending task will never
find the queue full. */
#define mainQUEUE_LENGTH					( 1 )

#if( portUSING_MPU_WRAPPERS == 1 )
/**
 * @brief Calls the port specific code to raise the privilege.
 *
 * @return pdFALSE if privilege was raised, pdTRUE otherwise.
 */
BaseType_t xPortRaisePrivilege( void ) FREERTOS_SYSTEM_CALL;
extern pmp_info_t xPmpInfo;
#endif

/*-----------------------------------------------------------*/
/*
 * Functions:
 * 		- prvSetupHardware: Setup Hardware according CPU and Board.
 */
static void prvSetupHardware( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );
// void vTaskCode( void * pvParameters ) FREERTOS_SYSTEM_CALL; // PRIVILEGED_FUNCTION;
static void vTaskCode( void * pvParameters ) PRIVILEGED_FUNCTION;

/**
 * @brief Implements the task which has Read Only access to the memory region
 * ucSharedMemory.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void prvROAccessTask( void * pvParameters );

/**
 * @brief Implements the task which has Read Write access to the memory region
 * ucSharedMemory.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void prvRWAccessTask( void * pvParameters );

/**
 * @brief Size of the shared memory region.
 */
#define SHARED_MEMORY_SIZE    16

/**
 * @brief Memory region shared between two tasks.
 */
__attribute__ ((aligned(128))) uint8_t ucSharedMemory[ SHARED_MEMORY_SIZE ];
// static uint8_t ucSharedMemory1[ SHARED_MEMORY_SIZE ] __attribute__( ( aligned( 32 ) ) );
// static uint8_t ucSharedMemory2[ SHARED_MEMORY_SIZE ] __attribute__( ( aligned( 32 ) ) );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

void _putchar(char character){
    MPU_bflb_uart_putchar(uart0, character);
}

/* Task to be created. */
// void vTaskCode( void * pvParameters ) FREERTOS_SYSTEM_CALL // PRIVILEGED_FUNCTION
// static void vTaskCode( void * pvParameters ) PRIVILEGED_FUNCTION
// {
// 	unsigned long ulCounter = 0;
// 	char pcWriteBuffer[200];

// 	( void ) pvParameters;

// 	/* Must not just run off the end of a task function, so delete this task.
// 	Note that because this task was created using xTaskCreate() the stack was
// 	allocated dynamically and I have not included any code to free it again. */
// 	// vTaskDelete( NULL );

//     /* The parameter value is expected to be 1 as 1 is passed in the
//     pvParameters value in the call to xTaskCreate() below. */
//     // configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	
// 	if (portIS_PRIVILEGED() == 1) {
// 		printf("%s task is running in priveleged mode\r\n", pcTaskGetName(NULL));
// 	} else {
// 		printf("%s task is running in unpriveleged mode\r\n", pcTaskGetName(NULL));
// 	}
// 	write( STDOUT_FILENO, "Test task\r\n", strlen( "Test task\r\n" ) );

// 	for( ;; )
//     {
//         /* Task code goes here. */
// 		vTaskDelay(mainQUEUE_TICK_COUNT_FOR_1S);
// 		vTaskList( pcWriteBuffer );
// 		printf("\033[2J\r\n");
// 		printf( pcWriteBuffer );
// 		printf("\r\n");
// 		// printf("[ %s ] : %d\r\n", pcTaskGetName(NULL), ++ulCounter );
// 		// write( STDOUT_FILENO, "Test task\r\n", strlen( "Test task\r\n" ) );
//     }
// }

/*-----------------------------------------------------------*/

static void prvROAccessTask( void * pvParameters )
{
    /* Unused parameters. */
    ( void ) pvParameters;

	if (portIS_PRIVILEGED() == 1) {
		printf("ROAccess task is running in priveleged mode\r\n");
	} else {
		printf("ROAccess task is running in unpriveleged mode\r\n");
	}

	// ucSharedMemory[ 0 ] = 1;
        
    for( ; ; )
    {
        /* Wait for a 3 seconds. */
        vTaskDelay( pdMS_TO_TICKS( 3 * 1000 ) );
		// vTaskDelete(NULL);
    }
}
/*-----------------------------------------------------------*/

static void prvRWAccessTask( void * pvParameters )
{
    /* Unused parameters. */
    ( void ) pvParameters;

	if (portIS_PRIVILEGED() == 1) {
		printf("RWAccess task is running in priveleged mode\r\n");
	} else {
		printf("RWAccess task is running in unpriveleged mode\r\n");
	}

	// printf("[ RWAccess ] : ucSharedMemory[0] = %d\r\n", ucSharedMemory[0]);
	// ucSharedMemory[0] = 1;
	// printf("[ RWAccess ] : ucSharedMemory[0] = %d\r\n", ucSharedMemory[0]);
	// metal_tty_putc( '#' );

    for( ; ; )
    {
        /* This task has RW access to ucSharedMemory and therefore can write to
         * it. */
        // ucSharedMemory[ 0 ] = 0;

        /* Wait for a 3 seconds. */
        vTaskDelay( pdMS_TO_TICKS( 3 * 1000 ) );
		// vTaskDelete(NULL);
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief         clnSrvTask - cleaning service.
 *
 * @param         None
 *
 * @returns       None
 */
static void clnSrvTask( void *pvParameters )
{
  const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    vTaskPrioritySet( xTaskGetIdleTaskHandle(), uxTaskPriorityGet(xTaskGetCurrentTaskHandle()));
//    taskYIELD();
    vTaskDelay( xDelay );
    vTaskPrioritySet( xTaskGetIdleTaskHandle(), tskIDLE_PRIORITY);
    vTaskDelay( xDelay );
  } while (1);
}

#define BUFFER_SIZE 300
// static __attribute__ ((aligned(16))) StackType_t xISRStack[ configMINIMAL_STACK_SIZE  + 1 ] = { 0 }; //__attribute__ ((section (".heap"))) ;
static __attribute__ ((aligned(16))) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 }; //__attribute__ ((section (".heap"))) ;
__attribute__ ((aligned(4))) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((section (".heap")));

//static StackType_t xSendTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 128 ) ) );
//static StackType_t xReceiveTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 128 ) ) );
/**
 * @brief         menuTask1 - .
 *
 * @param         None
 *
 * @returns       None
 */

void* aligned_malloc(size_t size, size_t alignment) {
    // Allocate additional space for saving the original address and for alignment
    uintptr_t raw_address = (uintptr_t)pvPortMalloc(size + alignment - 1 + sizeof(void*));

    if (!raw_address) {
        return NULL;  // Memory allocation error
    }

    // Find the nearest aligned address
    uintptr_t aligned_address = (raw_address + sizeof(void*) + alignment - 1) & ~(alignment - 1);

    // Store the original address before the aligned address
    ((void**)aligned_address)[-1] = (void*)raw_address;

    return (void*)aligned_address;
}

void aligned_free(void* aligned_ptr) {
    // Restore the original address and release it
    void* raw_address = ((void**)aligned_ptr)[-1];
    vPortFree(raw_address);
}

static void menuTask(void *pvParameters) PRIVILEGED_FUNCTION
{
  /* Define a buffer that is large enough to hold the generated table.  In most
   * cases the buffer will be too large to allocate on the stack, hence in this
   * example it is declared static. */
  ( void ) pvParameters;

  static char cBuffer[BUFFER_SIZE];
  extern uint32_t __unprivileged_data_section_start__[];
  extern uint32_t __unprivileged_data_section_end__[];
  TaskHandle_t xHandle_TaskROAccess = NULL, xHandle_TaskRWAccess = NULL, xHandle_ReceiveTask = NULL, xHandle_SendTask = NULL;

		static TaskParameters_t xROAccessTaskParameters =
		{
			.pvTaskCode = prvROAccessTask,
			.pcName = "ROAccess",
			.usStackDepth = 128, //configMINIMAL_STACK_SIZE,
			.pvParameters = NULL,
			.uxPriority = mainQUEUE_SEND_TASK_PRIORITY,
			.puxStackBuffer = NULL,
			// .puxStackBuffer = xROAccessTaskStack,
		};

	    static TaskParameters_t xRWAccessTaskParameters =
    	{
        	.pvTaskCode     = prvRWAccessTask,
        	.pcName         = "RWAccess",
        	.usStackDepth   = 128, //configMINIMAL_STACK_SIZE,
        	.pvParameters   = NULL, 
        	.uxPriority     = mainQUEUE_SEND_TASK_PRIORITY,
			.puxStackBuffer = NULL,
        	// .puxStackBuffer = xRWAccessTaskStack,
		};
		static TaskParameters_t xTaskRXDefinition =
		{
			.pvTaskCode = prvQueueReceiveTask,
			.pcName = "Rx",
			.usStackDepth = configMINIMAL_STACK_SIZE,
			.pvParameters = NULL,
			.uxPriority = mainQUEUE_RECEIVE_TASK_PRIORITY - 1 ,
			.puxStackBuffer = NULL,
		};
		static TaskParameters_t xTaskTXDefinition =
		{
			.pvTaskCode = prvQueueSendTask,
			.pcName = "Tx",
			.usStackDepth = configMINIMAL_STACK_SIZE,
			.pvParameters = NULL,
			.uxPriority = mainQUEUE_SEND_TASK_PRIORITY,
			.puxStackBuffer = NULL,
		};



  (void)pvParameters;
  int32_t lResult;
		extern uint32_t __unprivileged_data_section_start__[];
		extern uint32_t __unprivileged_data_section_end__[];
		extern uint32_t __tcm_code_start__[];
		extern uint32_t __tcm_code_end__[];
		extern uint32_t __tcm_data_end__[];
		extern uint32_t __tcm_data_start__[];
		extern uint32_t __ram_data_start__[];

		extern uint32_t privilege_status;

  int ch;
  do
  {
	portDISABLE_INTERRUPTS();
	printf("\n+- Menu -----------------------------------------------------+\r\n");
#ifdef CONFIG_APPS_HELLO_WORLD
	printf("| [1] - Run 2 unprvl apps & demo interaction between them    |\r\n");
	printf("| [2] - Delete 2 unprvl apps                                 |\r\n");
#endif
#ifdef CONFIG_APPS_HOTP
    printf("| [3] - Run HOTP (Ported from OP-TEE, user TA)               |\r\n");
#endif
    printf("| [4] - Get FreeRTOS Task List                               |\r\n");
#ifdef CONFIG_APPS_TEST
    printf("| [5] - Run Test Suite for GP TEE Client & Internal API      |\r\n");
#endif
#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
    printf("| [6] - Run H/W Security exception example (from non-secure) |\r\n");
    printf("| [7] - Run H/W Security exception example (from secure)     |\r\n");
#endif
#ifdef CONFIG_APPS_SPY
    printf("| [8] - Run test & spy (trying to get protected data) unprvl |\r\n");
    printf("| [9] - Delete test & spy unprvl apps                        |\r\n");
#endif
    printf("+------------------------------------------------------------+\r\n");
	portENABLE_INTERRUPTS();
    while (1) {
        ch = bflb_uart_getchar(uart0);
        if (ch == -1) {
            // taskYIELD();
            // vTaskDelay(mainQUEUE_TICK_COUNT_FOR_1S);
        } else break;
    }
    printf("\n[%c]\r\n", (char) ch);

    switch (ch) {
#ifdef CONFIG_APPS_HELLO_WORLD
    case '1':
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

	if( xQueue != NULL )
	{
				/* 
		 * Prepare xRegions for Receive Task 
		 */
        memset(&xTaskRXDefinition.xRegions, 0, sizeof(xTaskRXDefinition.xRegions));

        // authorize access to data and bss
		// 		Low address 
		// printf("xTaskRXDefinition.xRegions[0].pvBaseAddress = %p\r\n", xTaskRXDefinition.xRegions[0].pvBaseAddress);
		// 		High address 

        xTaskRXDefinition.xRegions[0].ulLengthInBytes = 0x10000;
        xTaskRXDefinition.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));
        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) 0xE0000000UL,
                                (size_t *) &xTaskRXDefinition.xRegions[0].pvBaseAddress,
                                xTaskRXDefinition.xRegions[0].ulLengthInBytes);
		if(lResult)
			printf("lResult R[0] = %d\r\n",lResult);

        xTaskRXDefinition.xRegions[1].ulLengthInBytes = 8192UL;
        xTaskRXDefinition.xRegions[1].ulParameters = ((portPMP_REGION_READ_ONLY) |
													(portPMP_REGION_READ_WRITE) |
													(portPMP_REGION_EXECUTE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));
        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) 0x62020000UL,
                                (size_t *) &xTaskRXDefinition.xRegions[1].pvBaseAddress,
                                xTaskRXDefinition.xRegions[1].ulLengthInBytes);
		if(lResult)
			printf("lResult R[1] = %d\r\n",lResult);

        xTaskRXDefinition.xRegions[2].ulLengthInBytes = 0x10000;
        xTaskRXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) __ram_data_start__,
                                (size_t *) &xTaskRXDefinition.xRegions[2].pvBaseAddress,
                                xTaskRXDefinition.xRegions[2].ulLengthInBytes);
		if(lResult)
			printf("lResult R[2] = %d\r\n",lResult);


        // allocate stack (It will take 2 PMP Slot - So it is not needed to put align the StackBuffer)
        // xTaskRXDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskRXDefinition.usStackDepth * sizeof( StackType_t ) * 2);
	xTaskRXDefinition.puxStackBuffer = ( StackType_t * )aligned_malloc(xTaskRXDefinition.usStackDepth * sizeof( StackType_t ),128);
        // xTaskRXDefinition.puxStackBuffer = xReceiveTaskStack;
		// portDISABLE_INTERRUPTS();
		// printf("xTaskRXDefinition.puxStackBuffer = %p\r\n", xTaskRXDefinition.puxStackBuffer);
		// portENABLE_INTERRUPTS();
        xTaskCreateRestricted(  &xTaskRXDefinition,
                                &xHandle_ReceiveTask);

		/*
		 * Prepare xRegions for Send Task 
		 */
        memset(&xTaskTXDefinition.xRegions, 0, sizeof(xTaskTXDefinition.xRegions));

        xTaskTXDefinition.xRegions[2].ulLengthInBytes = 0x10000;
        xTaskTXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) __ram_data_start__,
                                (size_t *) &xTaskTXDefinition.xRegions[2].pvBaseAddress,
                                xTaskTXDefinition.xRegions[2].ulLengthInBytes);
		if(lResult)
			printf("lResult R[2] = %d\r\n",lResult);

        // authorize access to data and bss

		// 		High address 

        // allocate stack (It will take 2 PMP Slot - So it is not needed to put align the StackBuffer)
        // xTaskTXDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskTXDefinition.usStackDepth * sizeof( StackType_t ) );
		xTaskTXDefinition.puxStackBuffer = ( StackType_t * )aligned_malloc(xTaskTXDefinition.usStackDepth * sizeof( StackType_t ),128);

		// printf("xTaskTXDefinition.puxStackBuffer = %p\r\n", xTaskTXDefinition.puxStackBuffer);

        xTaskCreateRestricted(  &xTaskTXDefinition,
                                &xHandle_SendTask);

	}

        break;
		case '2':
			if (xHandle_ReceiveTask != NULL)
				vTaskDelete(xHandle_ReceiveTask);
			if (xHandle_SendTask != NULL)
				vTaskDelete(xHandle_SendTask);
			if (xTaskRXDefinition.puxStackBuffer != NULL)
				vPortFree(xTaskRXDefinition.puxStackBuffer);
			if (xTaskTXDefinition.puxStackBuffer != NULL)
				vPortFree(xTaskTXDefinition.puxStackBuffer);
 		break;
#endif
      case '4':
        /* Obtain the current tick count. */
        printf("\nAuto-reload timer callback executing = %d\r\n", xTaskGetTickCount());

        /* Pass the buffer into vTaskList() to generate the table of information. */
        vTaskList(cBuffer);
        portDISABLE_INTERRUPTS();
        printf("%s\n", cBuffer);
        portENABLE_INTERRUPTS();
        break;
#ifdef CONFIG_APPS_SPY
        case '8':
		/* 
		 * Prepare xRegions for xROAccess Task 
		 */
        memset(&xROAccessTaskParameters.xRegions, 0, sizeof(xROAccessTaskParameters.xRegions));

        // authorize access to data and bss
				// Low address 

        xROAccessTaskParameters.xRegions[0].ulLengthInBytes = 0x10000;
        xROAccessTaskParameters.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));
        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) 0xE0000000UL,
                                (size_t *) &xROAccessTaskParameters.xRegions[0].pvBaseAddress,
                                xROAccessTaskParameters.xRegions[0].ulLengthInBytes);
		if(lResult)
			printf("lResult R[0] = %d\r\n",lResult);


        xROAccessTaskParameters.xRegions[1].ulLengthInBytes = 8192UL;
        xROAccessTaskParameters.xRegions[1].ulParameters = ((portPMP_REGION_READ_ONLY) |
													(portPMP_REGION_READ_WRITE) |
													(portPMP_REGION_EXECUTE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));
        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) 0x62020000UL,
                                (size_t *) &xROAccessTaskParameters.xRegions[1].pvBaseAddress,
                                xROAccessTaskParameters.xRegions[1].ulLengthInBytes);
		if(lResult)
			printf("lResult R[1] = %d\r\n",lResult);

        xROAccessTaskParameters.xRegions[2].ulLengthInBytes = 0x10000;
        xROAccessTaskParameters.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) __ram_data_start__,
                                (size_t *) &xROAccessTaskParameters.xRegions[2].pvBaseAddress,
                                xROAccessTaskParameters.xRegions[2].ulLengthInBytes);
		if(lResult)
			printf("lResult R[2] = %d\r\n",lResult);

        // xROAccessTaskParameters.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xROAccessTaskParameters.usStackDepth * sizeof( StackType_t ) );
		xROAccessTaskParameters.puxStackBuffer = ( StackType_t * )aligned_malloc(xROAccessTaskParameters.usStackDepth * sizeof( StackType_t ),128);
        // xROAccessTaskParameters.puxStackBuffer = xROAccessTaskStack;
		// printf("xROAccessTaskParameters.puxStackBuffer = %p\r\n", xROAccessTaskParameters.puxStackBuffer);


        xTaskCreateRestricted(  &xROAccessTaskParameters,
                                &xHandle_TaskROAccess);

		/* 
		 * Prepare xRegions for xRWAccess Task 
		 */
        memset(&xRWAccessTaskParameters.xRegions, 0, sizeof(xRWAccessTaskParameters.xRegions));

        // authorize access to data and bss
				// Low address 

        xRWAccessTaskParameters.xRegions[0].ulLengthInBytes = 0x10000;
        xRWAccessTaskParameters.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_NAPOT));
        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) 0xE0000000UL,
                                (size_t *) &xRWAccessTaskParameters.xRegions[0].pvBaseAddress,
                                xRWAccessTaskParameters.xRegions[0].ulLengthInBytes);
		if(lResult)
			printf("lResult R[0] = %d\r\n",lResult);


        xRWAccessTaskParameters.xRegions[1].ulLengthInBytes = 8192UL;
        xRWAccessTaskParameters.xRegions[1].ulParameters = ((portPMP_REGION_READ_ONLY) |
													(portPMP_REGION_READ_WRITE) |
													(portPMP_REGION_EXECUTE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));
        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) 0x62020000UL,
                                (size_t *) &xRWAccessTaskParameters.xRegions[1].pvBaseAddress,
                                xRWAccessTaskParameters.xRegions[1].ulLengthInBytes);
		if(lResult)
			printf("lResult R[1] = %d\r\n",lResult);

        xRWAccessTaskParameters.xRegions[2].ulLengthInBytes = 0x10000;
        xRWAccessTaskParameters.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        lResult = napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) __ram_data_start__,
                                (size_t *) &xRWAccessTaskParameters.xRegions[2].pvBaseAddress,
                                xRWAccessTaskParameters.xRegions[2].ulLengthInBytes);
		if(lResult)
			printf("lResult R[2] = %d\r\n",lResult);

		// printf("xROAccessTaskParameters.xRegions[0].ulLengthInBytes = %d\r\n", xROAccessTaskParameters.xRegions[0].ulLengthInBytes);

        // xRWAccessTaskParameters.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xRWAccessTaskParameters.usStackDepth * sizeof( StackType_t ) );
		xRWAccessTaskParameters.puxStackBuffer = ( StackType_t * )aligned_malloc(xRWAccessTaskParameters.usStackDepth * sizeof( StackType_t ),128);
        // xRWAccessTaskParameters.puxStackBuffer = xRWAccessTaskStack;
		// printf("xRWAccessTaskParameters.puxStackBuffer = %p\r\n", xRWAccessTaskParameters.puxStackBuffer);

        xTaskCreateRestricted(  &xRWAccessTaskParameters,
                                &xHandle_TaskRWAccess);

        // spyAppTask();
        break;
		case '9':
			if (xHandle_TaskROAccess != NULL)
				vTaskDelete(xHandle_TaskROAccess);
			if (xHandle_TaskRWAccess != NULL)
				vTaskDelete(xHandle_TaskRWAccess);
			if (xROAccessTaskParameters.puxStackBuffer != NULL)
				vPortFree(xROAccessTaskParameters.puxStackBuffer);
			if (xRWAccessTaskParameters.puxStackBuffer != NULL)
				vPortFree(xRWAccessTaskParameters.puxStackBuffer);
 		break;
#endif
      default:
        break;
    }
    taskYIELD();
		vTaskDelay(mainQUEUE_TICK_COUNT_FOR_1S);

  } while (1);
}


static void ledBlinkTask( void *pvParameters ) PRIVILEGED_FUNCTION
{
	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
	
 	struct bflb_device_s *gpio; //make instance of GPIO

	//Get gpio device handle, i.e.) define IC pin here where IC's GPIO = 26, pin silkscreen = 13
	//this is the GPIO device index that is referenced from 0, make sure to check the schematic
	gpio = bflb_device_get_by_name("gpio");

	//Set as gpio as output
	bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

	//Turn ON pin
	bflb_gpio_set(gpio, GPIO_PIN_26);

	if (portIS_PRIVILEGED() == 1) {
		printf("ledBlinkTask task is running in priveleged mode\r\n");
	} else {
		printf("ledBlinkTask task is running in unpriveleged mode\r\n");
	}


	for( ;; )
	{
		vTaskDelay(mainQUEUE_TICK_COUNT_FOR_1S);
		bflb_gpio_reset(gpio, GPIO_PIN_26);
		printf("[ledBlinkTask] Led OFF\r\n");
		vTaskDelay(mainQUEUE_TICK_COUNT_FOR_1S);
		bflb_gpio_set(gpio, GPIO_PIN_26);
		printf("[ledBlinkTask] Led ON\r\n");
	}
}

/*-----------------------------------------------------------*/
int main( void )
{
	// TaskHandle_t xHandle_ReceiveTask, xHandle_SendTask, xHandle_TaskListTasks, xHandle_TaskROAccess, xHandle_TaskRWAccess;

    // static StackType_t xTaskListTasksShowStack[ configMINIMAL_STACK_SIZE * 3] __attribute__( ( aligned( 32 ) ) ); 
    // static StackType_t xSendTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 128 ) ) );
    // static StackType_t xReceiveTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 128 ) ) );
    // static StackType_t xROAccessTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    // static StackType_t xRWAccessTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );


	// const char * const pcMessage = "FreeRTOS-TEE Demo start\r\n";
	// const char * const pcMessageEnd = "FreeRTOS-TEE Demo end\r\n";
	const char * const pcMessageEndError = "FreeRTOS-TEE Demo end - Error no enough PMP entry\r\n";
    const char * const pcMessageGranularityError = "FreeRTOS-TEE Demo end - Error platform granularity no supported\r\n";

	extern uint32_t __unprivileged_data_section_start__[];
	extern uint32_t __unprivileged_data_section_end__[];
    extern uint32_t __unprivileged_section_start__[];
    extern uint32_t __unprivileged_section_end__[];

	// extern uint32_t __heap_start[];
	// extern uint32_t __heap_end[];
	//extern unsigned long __privileged_data_start__[];
	//extern unsigned long __privileged_data_end__[];
	extern unsigned long __privileged_functions_start__[];
	extern unsigned long __privileged_functions_end__[];
	// extern unsigned long metal_segment_stack_begin[];
	// extern unsigned long metal_segment_stack_end[];
	volatile unsigned long *pul;

	prvSetupHardware();
	// __asm volatile ( "csrc mstatus, 0x8" );
	printf("\n\n\t-= mTower v" VERSION " =-  " __DATE__ "  " __TIME__"\r\n\n");
	// printf("ucSharedMemory = %p\r\n", ucSharedMemory);
	__enable_irq();

	// printf("__ricsv_xlen = %d\r\n", __riscv_xlen);

	// pul = __unprivileged_data_section_start__;
	// printf("__unprivileged_data_section_start__ = %p\r\n", pul);

	// pul = __unprivileged_data_section_end__;
	// printf("__unprivileged_data_section_end__ = %p\r\n", pul);

	extern uint32_t __text_code_start__[];
	extern uint32_t __text_code_end__[];
	extern uint32_t __tcm_code_start__[];
	extern uint32_t __tcm_code_end__[];

    extern uint32_t __fsymtab_start[];
    extern uint32_t __fsymtab_end[];
    extern uint32_t __vsymtab_start[];
    extern uint32_t __vsymtab_end[];
	extern uint32_t __usbh_class_info_end__[];

	// printf("__fsymtab_start                = %p\r\n", &__fsymtab_start);
	// printf("__fsymtab_end              	   = %p\r\n", &__fsymtab_end);
	// printf("__vsymtab_start            	   = %p\r\n", &__vsymtab_start);
	// printf("__vsymtab_end              	   = %p\r\n", &__vsymtab_end);

	// printf("__usbh_class_info_end__        = %p\r\n", &__usbh_class_info_end__);
	// pul = __unprivileged_section_start__;
	// printf("__unprivileged_section_start__ = %p\r\n", pul);

	// pul = __unprivileged_section_end__;
	// printf("__unprivileged_section_end__   = %p\r\n", pul);

	// pul = __privileged_functions_start__;
	// printf("__privileged_functions_start__ = %p\r\n", pul);

	// pul = __privileged_functions_end__;
	// printf("__privileged_functions_end__   = %p\r\n", pul);

	// printf("__text_code_start__            = %p\r\n", &__text_code_start__);
	extern uint32_t __tcm_data_end__[];
	extern uint32_t __tcm_data_start__[];
	// printf("__text_code_end__              = %p\r\n", __text_code_end__);
	// printf("__tcm_code_start__             = %p\r\n", __tcm_code_start__);
	// printf("__tcm_code_end__               = %p\r\n", __tcm_code_end__);
	// printf("__tcm_data_start__             = %p\r\n", __tcm_data_start__);
	// printf("__tcm_data_end__               = %p\r\n", __tcm_data_end__);
	// printf("uart0			               = %p\r\n", uart0);

	// printf("prvQueueReceiveTask = %p\r\n", prvQueueReceiveTask);
	// printf("MPU_vTaskDelay = %p\r\n", vTaskDelay);
	// printf("xIsPrivileged = %p\r\n", xIsPrivileged);
	// printf("bflb_uart_putchar = %p\r\n", bflb_uart_putchar);
	// bflb_uart_putchar

	// pul = __privileged_data_start__;
	// printf("__privileged_data_start__ = %p\r\n", pul);

	// pul = __privileged_data_end__;
	// printf("__privileged_data_end__ = %p\r\n", pul);

	// pul = metal_segment_stack_begin;
	// printf("metal_segment_stack_begin = %p\r\n", pul);

	// pul = metal_segment_stack_end;
	// printf("metal_segment_stack_end = %p\r\n", pul);

	// pul = __heap_start;
	// printf("__heap_start = %p\r\n", pul);

	// pul = __heap_end;
	// printf("__heap_end = %p\r\n", pul);

	// printf("sizeof( StackType_t ) = %d\r\n", sizeof( StackType_t ));
	// printf("portTOTAL_NUM_CFG_REG = %d\r\n", portTOTAL_NUM_CFG_REG);
	// printf("\r\nBefore hello\r\n");

	/*
	 * Call xPortFreeRTOSInit in order to set xISRTopStack
	 */
	// printf("xISRStack = %p\r\n",(StackType_t)&( xISRStack[ ( (configMINIMAL_STACK_SIZE - 1) & ~portBYTE_ALIGNMENT_MASK ) ] ));
	// printf("xISRStack = %p\r\n",&( xISRStack ));
	// printf("xISRStack = %p\r\n",(StackType_t)&( xISRStack[ ( (configMINIMAL_STACK_SIZE - 1) & ~portBYTE_ALIGNMENT_MASK ) ] ));
	// __enable_irq();

	if ( 0 != xPortFreeRTOSInit((StackType_t)&( xISRStack[ ( (configMINIMAL_STACK_SIZE - 1) & ~portBYTE_ALIGNMENT_MASK ) ] ))) {
		printf("ERROR");
		while(1);
		// _exit(-1);
	}
	
	BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

	// xReturned = xTaskCreate(
    //                 vTaskCode,       /* Function that implements the task. */
    //                 "PRVLGD",          /* Text name for the task. */
    //                 512,      /* Stack size in words, not bytes. */
    //                 NULL, //( void * ) 1,    /* Parameter passed into the task. */
    //                 mainQUEUE_SEND_TASK_PRIORITY | portPRIVILEGE_BIT,/* Priority at which the task is created. */
    //                 &xHandle );      /* Used to pass out the created task's handle. */

	xTaskCreate(menuTask,			  /* The function that implements the task. */
				"menu",				  /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				768,				  /* The size of the stack to allocate to the task. */
				(void *)NULL,		  /* The parameter passed to the task - just to check the functionality. */
				mainQUEUE_SEND_TASK_PRIORITY | portPRIVILEGE_BIT, /* The priority assigned to the task. */
				NULL);
	__enable_irq();

	xTaskCreate(ledBlinkTask,	  /* The function that implements the task. */
				"ledBlink",		  /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				128,				  /* The size of the stack to allocate to the task. */
				(void *)NULL,		  /* The parameter passed to the task - just to check the functionality. */
				mainQUEUE_SEND_TASK_PRIORITY | portPRIVILEGE_BIT, /* The priority assigned to the task. */
				NULL);
	__enable_irq();

#if( portUSING_MPU_WRAPPERS == 1 )
	if (xPmpInfo.nb_pmp < 8)
	{
		printf("%s", pcMessageEndError);
		_exit(0);
	// } else if (xPmpInfo.granularity > 4) {
	} else if (xPmpInfo.granularity > 128) {
        /* 
         * platfrom granularity > 4 bytes is not supported yet, some
         * modifications are needed on FreeRTOS port to do so.
         */
        printf("%s", pcMessageGranularityError);
		_exit(0);
    }
	// printf("xPmpInfo.nb_pmp = %d\r\n", xPmpInfo.nb_pmp);
	// printf("xPmpInfo.granularity= %d\r\n", xPmpInfo.granularity);

        if(0 == xPmpInfo.granularity) 
		{
		    init_pmp (&xPmpInfo);
        }
	__enable_irq();

		/* Start the tasks and timer running. */
		vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created. 
	or task have stoppped the Scheduler */

		// vTaskDelete( xHandle_SendTask );
		// vTaskDelete( xHandle_ReceiveTask );
#endif /* ( portUSING_MPU_WRAPPERS == 1 ) */
}

/*-----------------------------------------------------------*/
static __attribute__( ( section( ".unprivileged_section" ) ) ) void prvQueueSendTask( void *pvParameters )
{
	TickType_t xNextWakeTime;
	BaseType_t xReturned;
	unsigned long ulCounter = 0;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
	( void ) xReturned;

	printf("prvQueueSendTask task: ");
	if (portIS_PRIVILEGED() == 1) {
		printf("TX task is running in priveleged mode\r\n");
	} else {
		printf("Tx task is running in unpriveleged mode\r\n");
	}

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ; ; )
	{
		printf("[ Tx ] : %d\r\n", ++ulCounter );

		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_TICK_COUNT_FOR_1S);
		// vTaskDelay( mainQUEUE_TICK_COUNT_FOR_1S * 500);


		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xReturned = xQueueSend( xQueue, &ulCounter, 0U );
		configASSERT( xReturned == pdPASS );
	}

	/** 
	 * SiFive CI/CD need to have a exit(0) status to pass
	 */
#if( portUSING_MPU_WRAPPERS == 1 )
	/* We run into user mode, so need to be machine mode before to call vTaskEndScheduler */
	// xPortRaisePrivilege();
#endif /* ( portUSING_MPU_WRAPPERS == 1 ) */
	// vTaskEndScheduler();
}
/*-----------------------------------------------------------*/

static __attribute__( ( section( ".unprivileged_section" ) ) ) void prvQueueReceiveTask( void *pvParameters )
{
	unsigned long ulReceivedValue;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
	
	printf("prvQueueReceiveTask task: ");
	if (portIS_PRIVILEGED() == 1) {
		printf("RX task is running in priveleged mode\r\n");
	} else {
		printf("Rx task is running in unpriveleged mode\r\n");
	}

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */

		printf("[ Rx ] : %d\r\n", ulReceivedValue );
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	// const char * const pcWarningMsg = "At least one of LEDs is null.\r\n";
    board_init();
    uart0 = bflb_device_get_by_name("uart0");
	bflb_uart_set_console(bflb_device_get_by_name("uart0"));
}
/*-----------------------------------------------------------*/

__attribute__((weak)) void vApplicationTickHook(void)
{
    //printf("vApplicationTickHook\r\n");
}

__attribute__((weak)) void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("vApplicationStackOverflowHook\r\n");

    while (1)
        ;
}

__attribute__((weak)) void vApplicationMallocFailedHook(void)
{
	const char * const pcErrorMsg = "ERROR malloc \r\n";
    printf("vApplicationMallocFailedHook\r\n");
	printf(pcErrorMsg);
    while (1)
        ;
}

__attribute__((weak)) void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
__attribute__((weak)) void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
