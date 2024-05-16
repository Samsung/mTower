/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */



/******************************************************************************
 *
 * main() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 1000 milliseconds, before sending the value 100 to the queue that
 * was created within main().  Once the value is sent, the task loops
 * back around to block for another 1000 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, writes 'Blink' to the UART
 * (the UART is used in place of the LED to allow easy execution in QEMU).  The
 * 'block time' parameter passed to the queue receive function specifies that
 * the task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 1000 milliseconds, the queue receive
 * task leaves the Blocked state every 1000 milliseconds, and therefore toggles
 * the LED every 1 second.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Freedom metal includes. */
#include <metal.h>
#include <metal-platform.h>

#include <metal/lock.h>
#include <metal/uart.h>
#include <metal/interrupt.h>
#include <metal/clock.h>
#include <metal/led.h>
#include <metal/gpio.h>

#include <printf.h>
#include "version.h">

extern struct metal_led *led0_red, *led0_green, *led0_blue;

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
// BaseType_t xPortRaisePrivilege( void ) FREERTOS_SYSTEM_CALL;
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
static uint8_t ucSharedMemory[ SHARED_MEMORY_SIZE ] __attribute__( ( aligned( 32 ) ) );
// static uint8_t ucSharedMemory1[ SHARED_MEMORY_SIZE ] __attribute__( ( aligned( 32 ) ) );
// static uint8_t ucSharedMemory2[ SHARED_MEMORY_SIZE ] __attribute__( ( aligned( 32 ) ) );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

struct metal_cpu *cpu0;
struct metal_interrupt *cpu_intr, *tmr_intr;
struct metal_led *led0_red, *led0_green, *led0_blue;

#define LED_ERROR ((led0_red == NULL) || (led0_green == NULL) || (led0_blue == NULL))

void _putchar(char character){
	metal_tty_putc( character );
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

	// if (portIS_PRIVILEGED() == 1) {
	// 	// printf("ROAccess task is running in priveleged mode\r\n");
	// 	printf("ROAccess\r\n");
	// } else {
	// 	printf("ROAccess task\r\n");
	// // 	printf("ROAccess task is running in unpriveleged mode\r\n");
	// }

	ucSharedMemory[ 0 ] = 1;
        
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
		// write( STDOUT_FILENO, "RWAccess\r\n", strlen("RWAccess\r\n") );
		// printf("RWAccess task is running in priveleged mode\r\n");
		// printf("RWAccess\r\n");
	} else {
		// write( STDOUT_FILENO, "RWAccess\r\n", strlen("RWAccess\r\n") );
		// printf("RWAccess\r\n");
	// 	printf("RWAccess task is running in unpriveleged mode\r\n");
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

#define BUFFER_SIZE 200

/**
 * @brief         menuTask1 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void menuTask(void *pvParameters) PRIVILEGED_FUNCTION
{
  /* Define a buffer that is large enough to hold the generated table.  In most
   * cases the buffer will be too large to allocate on the stack, hence in this
   * example it is declared static. */

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
			.uxPriority = mainQUEUE_RECEIVE_TASK_PRIORITY,
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
	metal_tty_getc(&ch);	
    printf("\n[%c]\r\n", (char) ch);

    switch (ch) {
#ifdef CONFIG_APPS_HELLO_WORLD
    case '1':
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

	if( xQueue != NULL )
	{
		extern uint32_t __unprivileged_data_section_start__[];
		extern uint32_t __unprivileged_data_section_end__[];

				/* 
		 * Prepare xRegions for Receive Task 
		 */
        memset(&xTaskRXDefinition.xRegions, 0, sizeof(xTaskRXDefinition.xRegions));

        // authorize access to data and bss
		// 		Low address 
        xTaskRXDefinition.xRegions[0].ulLengthInBytes = 4;
        xTaskRXDefinition.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_NA4));
        addr_modifier ( xPmpInfo.granularity,
                        ( size_t ) __unprivileged_data_section_start__,
                        (size_t *) &xTaskRXDefinition.xRegions[0].pvBaseAddress);
		// printf("xTaskRXDefinition.xRegions[0].pvBaseAddress = %p\r\n", xTaskRXDefinition.xRegions[0].pvBaseAddress);
		// 		High address 
        xTaskRXDefinition.xRegions[1].ulLengthInBytes = 4;
        xTaskRXDefinition.xRegions[1].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_TOR));

        addr_modifier ( xPmpInfo.granularity,
                        ( size_t ) __unprivileged_data_section_end__,
                        (size_t *) &xTaskRXDefinition.xRegions[1].pvBaseAddress);
		// printf("xTaskRXDefinition.xRegions[1].pvBaseAddress = %p\r\n", xTaskRXDefinition.xRegions[1].pvBaseAddress);

#ifdef METAL_SIFIVE_UART0
    // allow access to UART peripheral
        xTaskRXDefinition.xRegions[2].ulLengthInBytes = METAL_SIFIVE_UART0_0_SIZE;
        xTaskRXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) METAL_SIFIVE_UART0_0_BASE_ADDRESS,
                                (size_t *) &xTaskRXDefinition.xRegions[2].pvBaseAddress,
                                xTaskRXDefinition.xRegions[2].ulLengthInBytes);
#endif /* METAL_SIFIVE_UART0 */

        // allocate stack (It will take 2 PMP Slot - So it is not needed to put align the StackBuffer)
        xTaskRXDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskRXDefinition.usStackDepth * sizeof( StackType_t ) );
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

        // authorize access to data and bss
		// 		Low address 
        xTaskTXDefinition.xRegions[0].ulLengthInBytes = 4;
        xTaskTXDefinition.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_NA4));
        addr_modifier ( xPmpInfo.granularity,
                        ( size_t ) __unprivileged_data_section_start__,
                        (size_t *) &xTaskTXDefinition.xRegions[0].pvBaseAddress);

		// 		High address 
        xTaskTXDefinition.xRegions[1].ulLengthInBytes = 4;
        xTaskTXDefinition.xRegions[1].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_TOR));

        addr_modifier ( xPmpInfo.granularity,
                        ( size_t ) __unprivileged_data_section_end__,
                        (size_t *) &xTaskTXDefinition.xRegions[1].pvBaseAddress);

// #ifdef METAL_SIFIVE_GPIO0
//         // allow access to GPIO (Each peripheral are on 4Kb mapping area)
//         xTaskTXDefinition.xRegions[2].ulLengthInBytes = METAL_SIFIVE_GPIO0_0_SIZE;
//         xTaskTXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                      (portPMP_REGION_ADDR_MATCH_NAPOT));

//         napot_addr_modifier (	xPmpInfo.granularity,
//                                 (size_t) METAL_SIFIVE_GPIO0_0_BASE_ADDRESS,
//                                 (size_t *) &xTaskTXDefinition.xRegions[2].pvBaseAddress,
//                                 xTaskTXDefinition.xRegions[2].ulLengthInBytes);
// #endif /* METAL_SIFIVE_GPIO0 */

#ifdef METAL_SIFIVE_UART0
    // allow access to UART peripheral
        xTaskTXDefinition.xRegions[2].ulLengthInBytes = METAL_SIFIVE_UART0_0_SIZE;
        xTaskTXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) METAL_SIFIVE_UART0_0_BASE_ADDRESS,
                                (size_t *) &xTaskTXDefinition.xRegions[2].pvBaseAddress,
                                xTaskTXDefinition.xRegions[2].ulLengthInBytes);
#endif /* METAL_SIFIVE_UART0 */
        // allocate stack (It will take 2 PMP Slot - So it is not needed to put align the StackBuffer)
        xTaskTXDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskTXDefinition.usStackDepth * sizeof( StackType_t ) );
        // xTaskTXDefinition.puxStackBuffer = xSendTaskStack;

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
        xROAccessTaskParameters.xRegions[0].ulLengthInBytes = 4;
        xROAccessTaskParameters.xRegions[0].ulParameters = ((portPMP_REGION_READ_ONLY) |
                                                    (portPMP_REGION_ADDR_MATCH_NA4));
        addr_modifier ( xPmpInfo.granularity,
                        // ( size_t ) __unprivileged_data_section_start__,
						( size_t ) ucSharedMemory,
                        (size_t *) &xROAccessTaskParameters.xRegions[0].pvBaseAddress);
		// printf("xROAccessTaskParameters.xRegions[0].pvBaseAddress = %p\r\n", xROAccessTaskParameters.xRegions[0].ulLengthInBytes);

				// High address 
        xROAccessTaskParameters.xRegions[1].ulLengthInBytes = 4;
        xROAccessTaskParameters.xRegions[1].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_TOR));

        addr_modifier ( xPmpInfo.granularity,
                        ( size_t ) __unprivileged_data_section_end__,
                        (size_t *) &xROAccessTaskParameters.xRegions[1].pvBaseAddress);

#ifdef METAL_SIFIVE_UART0
    // allow access to UART peripheral
        xROAccessTaskParameters.xRegions[2].ulLengthInBytes = METAL_SIFIVE_UART0_0_SIZE;
        xROAccessTaskParameters.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) METAL_SIFIVE_UART0_0_BASE_ADDRESS,
                                (size_t *) &xROAccessTaskParameters.xRegions[2].pvBaseAddress,
                                xROAccessTaskParameters.xRegions[2].ulLengthInBytes);
#endif /* METAL_SIFIVE_UART0 */

        xROAccessTaskParameters.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xROAccessTaskParameters.usStackDepth * sizeof( StackType_t ) );
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
		// printf("xROAccessTaskParameters.xRegions[0].ulLengthInBytes = %d\r\n", xROAccessTaskParameters.xRegions[0].ulLengthInBytes);
        xRWAccessTaskParameters.xRegions[0].ulLengthInBytes = 4;
        xRWAccessTaskParameters.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_NA4));
        addr_modifier ( xPmpInfo.granularity,
                        // ( size_t ) __unprivileged_data_section_start__,
						( size_t ) ucSharedMemory,
                        (size_t *) &xRWAccessTaskParameters.xRegions[0].pvBaseAddress);

		// printf("xRWAccessTaskParameters.xRegions[0].pvBaseAddress = %p\r\n", xRWAccessTaskParameters.xRegions[0].pvBaseAddress);
				// High address 
        xRWAccessTaskParameters.xRegions[1].ulLengthInBytes = 4;
        xRWAccessTaskParameters.xRegions[1].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                    (portPMP_REGION_ADDR_MATCH_TOR));

        addr_modifier ( xPmpInfo.granularity,
                        ( size_t ) __unprivileged_data_section_end__,
                        (size_t *) &xRWAccessTaskParameters.xRegions[1].pvBaseAddress);

#ifdef METAL_SIFIVE_UART0
    // allow access to UART peripheral
        xRWAccessTaskParameters.xRegions[2].ulLengthInBytes = METAL_SIFIVE_UART0_0_SIZE;
        xRWAccessTaskParameters.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
                                                     (portPMP_REGION_ADDR_MATCH_NAPOT));

        napot_addr_modifier (	xPmpInfo.granularity,
                                (size_t) METAL_SIFIVE_UART0_0_BASE_ADDRESS,
                                (size_t *) &xRWAccessTaskParameters.xRegions[2].pvBaseAddress,
                                xRWAccessTaskParameters.xRegions[2].ulLengthInBytes);
#endif /* METAL_SIFIVE_UART0 */

        xRWAccessTaskParameters.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xRWAccessTaskParameters.usStackDepth * sizeof( StackType_t ) );
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
	unsigned long ulCounter = 0;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	struct metal_gpio *led0; //make instance of GPIO

  //Get gpio device handle, i.e.) define IC pin here where IC's GPIO = 5, pin silkscreen = 13
  //this is the GPIO device index that is referenced from 0, make sure to check the schematic
  led0 = metal_gpio_get_device(0);

  //Pins are set when initialized so we must disable it when we use it as an input/output
  metal_gpio_disable_input(led0, 5);

  //Set as gpio as output
  metal_gpio_enable_output(led0, 5);

  //Pins have more than one function, make sure we disconnect anything connected...
  metal_gpio_disable_pinmux(led0, 5);

  //Turn ON pin
  metal_gpio_set_pin(led0, 5, 1);

	for( ;; )
	{
        metal_gpio_set_pin(led0, 5, ++ulCounter&0x01);
		vTaskDelay(mainQUEUE_TICK_COUNT_FOR_1S);

	}
}

// void print_stack_pointer() {
//   void* p = NULL;
//   printf("%p\n", (void*)&p);
// }
/*-----------------------------------------------------------*/
int main( void )
{
	TaskHandle_t xHandle_ReceiveTask, xHandle_SendTask, xHandle_TaskListTasks, xHandle_TaskROAccess, xHandle_TaskRWAccess;

    // static StackType_t xTaskListTasksShowStack[ configMINIMAL_STACK_SIZE * 3] __attribute__( ( aligned( 32 ) ) ); 
    // static StackType_t xSendTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    // static StackType_t xReceiveTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    // static StackType_t xROAccessTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    // static StackType_t xRWAccessTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );


	const char * const pcMessage = "FreeRTOS-TEE Demo start\r\n";
	const char * const pcMessageEnd = "FreeRTOS-TEE Demo end\r\n";
	const char * const pcMessageEndError = "FreeRTOS-TEE Demo end - Error no enough PMP entry\r\n";
    const char * const pcMessageGranularityError = "FreeRTOS-TEE Demo end - Error platform granularity no supported\r\n";

	extern uint32_t __unprivileged_data_section_start__[];
	extern uint32_t __unprivileged_data_section_end__[];
	extern uint32_t __heap_start[];
	extern uint32_t __heap_end[];
	extern unsigned long __privileged_data_start__[];
	extern unsigned long __privileged_data_end__[];
	extern unsigned long __privileged_functions_start__[];
	extern unsigned long __privileged_functions_end__[];
	extern unsigned long metal_segment_stack_begin[];
	extern unsigned long metal_segment_stack_end[];
	volatile unsigned long *pul;

	prvSetupHardware();

	printf("\n\n\t-= mTower v" VERSION " =-  " __DATE__ "  " __TIME__"\r\n\n");

	// printf("__ricsv_xlen = %d\r\n", __riscv_xlen);

	// pul = __unprivileged_data_section_start__;
	// printf("__unprivileged_data_section_start__ = %p\r\n", pul);

	// pul = __unprivileged_data_section_end__;
	// printf("__unprivileged_data_section_end__ = %p\r\n", pul);

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

	// pul = __privileged_functions_start__;
	// printf("__privileged_functions_start__ = %p\r\n", pul);

	// pul = __privileged_functions_end__;
	// printf("__privileged_functions_end__ = %p\r\n", pul);

	// printf("sizeof( StackType_t ) = %d\r\n", sizeof( StackType_t ));
	// printf("portTOTAL_NUM_CFG_REG = %d\r\n", portTOTAL_NUM_CFG_REG);

	// write( STDOUT_FILENO, pcMessage, strlen( pcMessage ) );
	BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

	// char *buf = pvPortMalloc(8);
	// if (buf != NULL) {
	// 	printf("pvPortMalloc: *buf = %p\r\n", buf);
	// }

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

	// xTaskCreate(ledBlinkTask,	  /* The function that implements the task. */
	// 			"ledBlink",		  /* The text name assigned to the task - for debug only as it is not used by the kernel. */
	// 			128,				  /* The size of the stack to allocate to the task. */
	// 			(void *)NULL,		  /* The parameter passed to the task - just to check the functionality. */
	// 			mainQUEUE_SEND_TASK_PRIORITY | portPRIVILEGE_BIT, /* The priority assigned to the task. */
	// 			NULL);

#if( portUSING_MPU_WRAPPERS == 1 )
	if (xPmpInfo.nb_pmp < 8)
	{
		write( STDOUT_FILENO, pcMessageEndError, strlen( pcMessageEndError ) );
		_exit(0);
	} else if (xPmpInfo.granularity > 4) {
        /* 
         * platfrom granularity > 4 bytes is not supported yet, some
         * modifications are needed on FreeRTOS port to do so.
         */
        write( STDOUT_FILENO, pcMessageGranularityError, strlen( pcMessageGranularityError ) );
		_exit(0);
    }

	// printf("xPmpInfo.nb_pmp = %d\r\n", xPmpInfo.nb_pmp);

	/* Create the queue. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

	if( xQueue != NULL )
	{
		extern uint32_t __unprivileged_data_section_start__[];
		extern uint32_t __unprivileged_data_section_end__[];

		// static TaskParameters_t xTaskListTasksShowDefinition =
		// {
		// 	.pvTaskCode = prvListTasksShowTask,
		// 	.pcName = "UNPRVLGD",
		// 	.usStackDepth = 224, //configMINIMAL_STACK_SIZE,
		// 	.pvParameters = NULL,
		// 	.uxPriority = mainQUEUE_RECEIVE_TASK_PRIORITY,
		// 	.puxStackBuffer = NULL,
		// };

		// static TaskParameters_t xTaskRXDefinition =
		// {
		// 	.pvTaskCode = prvQueueReceiveTask,
		// 	.pcName = "Rx",
		// 	.usStackDepth = configMINIMAL_STACK_SIZE,
		// 	.pvParameters = NULL,
		// 	.uxPriority = mainQUEUE_RECEIVE_TASK_PRIORITY,
		// 	.puxStackBuffer = NULL,
		// };

		// static TaskParameters_t xTaskTXDefinition =
		// {
		// 	.pvTaskCode = prvQueueSendTask,
		// 	.pcName = "Tx",
		// 	.usStackDepth = configMINIMAL_STACK_SIZE,
		// 	.pvParameters = NULL,
		// 	.uxPriority = mainQUEUE_SEND_TASK_PRIORITY,
		// 	.puxStackBuffer = NULL,
		// };

		// static TaskParameters_t xROAccessTaskParameters =
		// {
		// 	.pvTaskCode = prvROAccessTask,
		// 	.pcName = "ROAccess",
		// 	.usStackDepth = 224, //configMINIMAL_STACK_SIZE,
		// 	.pvParameters = NULL,
		// 	.uxPriority = mainQUEUE_SEND_TASK_PRIORITY,
		// 	.puxStackBuffer = NULL,
		// 	// .puxStackBuffer = xROAccessTaskStack,
		// };

	    // static TaskParameters_t xRWAccessTaskParameters =
    	// {
        // 	.pvTaskCode     = prvRWAccessTask,
        // 	.pcName         = "RWAccess",
        // 	.usStackDepth   = 128, //configMINIMAL_STACK_SIZE,
        // 	.pvParameters   = NULL, 
        // 	.uxPriority     = mainQUEUE_SEND_TASK_PRIORITY,
		// 	.puxStackBuffer = NULL,
        // 	// .puxStackBuffer = xRWAccessTaskStack,
		// };

		// printf("xROAccessTaskParameters.pcName = %s\r\n", xROAccessTaskParameters.pcName);
		// printf("xTaskTXDefinition.pcName = %s\r\n", xTaskTXDefinition.pcName);

        if(0 == xPmpInfo.granularity) 
		{
		    init_pmp (&xPmpInfo);
        }
		// printf("xPmpInfo.granularity = %d\r\n", xPmpInfo.granularity);
// 		/* 
// 		 * Prepare xRegions for Receive Task 
// 		 */
//         memset(&xTaskRXDefinition.xRegions, 0, sizeof(xTaskRXDefinition.xRegions));

//         // authorize access to data and bss
// 		// 		Low address 
//         xTaskRXDefinition.xRegions[0].ulLengthInBytes = 4;
//         xTaskRXDefinition.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                     (portPMP_REGION_ADDR_MATCH_NA4));
//         addr_modifier ( xPmpInfo.granularity,
//                         ( size_t ) __unprivileged_data_section_start__,
//                         (size_t *) &xTaskRXDefinition.xRegions[0].pvBaseAddress);
// 		// printf("xTaskRXDefinition.xRegions[0].pvBaseAddress = %p\r\n", xTaskRXDefinition.xRegions[0].pvBaseAddress);
// 		// 		High address 
//         xTaskRXDefinition.xRegions[1].ulLengthInBytes = 4;
//         xTaskRXDefinition.xRegions[1].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                     (portPMP_REGION_ADDR_MATCH_TOR));

//         addr_modifier ( xPmpInfo.granularity,
//                         ( size_t ) __unprivileged_data_section_end__,
//                         (size_t *) &xTaskRXDefinition.xRegions[1].pvBaseAddress);
// 		// printf("xTaskRXDefinition.xRegions[1].pvBaseAddress = %p\r\n", xTaskRXDefinition.xRegions[1].pvBaseAddress);

// #ifdef METAL_SIFIVE_UART0
//     // allow access to UART peripheral
//         xTaskRXDefinition.xRegions[2].ulLengthInBytes = METAL_SIFIVE_UART0_0_SIZE;
//         xTaskRXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                      (portPMP_REGION_ADDR_MATCH_NAPOT));

//         napot_addr_modifier (	xPmpInfo.granularity,
//                                 (size_t) METAL_SIFIVE_UART0_0_BASE_ADDRESS,
//                                 (size_t *) &xTaskRXDefinition.xRegions[2].pvBaseAddress,
//                                 xTaskRXDefinition.xRegions[2].ulLengthInBytes);
// #endif /* METAL_SIFIVE_UART0 */

//         // allocate stack (It will take 2 PMP Slot - So it is not needed to put align the StackBuffer)
//         xTaskRXDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskRXDefinition.usStackDepth * sizeof( StackType_t ) );
//         // xTaskRXDefinition.puxStackBuffer = xReceiveTaskStack;
// 		// portDISABLE_INTERRUPTS();
// 		printf("xTaskRXDefinition.puxStackBuffer = %p\r\n", xTaskRXDefinition.puxStackBuffer);
// 		// portENABLE_INTERRUPTS();
//         xTaskCreateRestricted(  &xTaskRXDefinition,
//                                 &xHandle_ReceiveTask);

// 		/*
// 		 * Prepare xRegions for Send Task 
// 		 */
//         memset(&xTaskTXDefinition.xRegions, 0, sizeof(xTaskTXDefinition.xRegions));

//         // authorize access to data and bss
// 		// 		Low address 
//         xTaskTXDefinition.xRegions[0].ulLengthInBytes = 4;
//         xTaskTXDefinition.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                     (portPMP_REGION_ADDR_MATCH_NA4));
//         addr_modifier ( xPmpInfo.granularity,
//                         ( size_t ) __unprivileged_data_section_start__,
//                         (size_t *) &xTaskTXDefinition.xRegions[0].pvBaseAddress);

// 		// 		High address 
//         xTaskTXDefinition.xRegions[1].ulLengthInBytes = 4;
//         xTaskTXDefinition.xRegions[1].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                     (portPMP_REGION_ADDR_MATCH_TOR));

//         addr_modifier ( xPmpInfo.granularity,
//                         ( size_t ) __unprivileged_data_section_end__,
//                         (size_t *) &xTaskTXDefinition.xRegions[1].pvBaseAddress);

// #ifdef METAL_SIFIVE_GPIO0
//         // allow access to GPIO (Each peripheral are on 4Kb mapping area)
//         xTaskTXDefinition.xRegions[2].ulLengthInBytes = METAL_SIFIVE_GPIO0_0_SIZE;
//         xTaskTXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                      (portPMP_REGION_ADDR_MATCH_NAPOT));

//         napot_addr_modifier (	xPmpInfo.granularity,
//                                 (size_t) METAL_SIFIVE_GPIO0_0_BASE_ADDRESS,
//                                 (size_t *) &xTaskTXDefinition.xRegions[2].pvBaseAddress,
//                                 xTaskTXDefinition.xRegions[2].ulLengthInBytes);
// #endif /* METAL_SIFIVE_GPIO0 */

// // #ifdef METAL_SIFIVE_UART0
// //     // allow access to UART peripheral
// //         xTaskTXDefinition.xRegions[2].ulLengthInBytes = METAL_SIFIVE_UART0_0_SIZE;
// //         xTaskTXDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
// //                                                      (portPMP_REGION_ADDR_MATCH_NAPOT));

// //         napot_addr_modifier (	xPmpInfo.granularity,
// //                                 (size_t) METAL_SIFIVE_UART0_0_BASE_ADDRESS,
// //                                 (size_t *) &xTaskTXDefinition.xRegions[2].pvBaseAddress,
// //                                 xTaskTXDefinition.xRegions[2].ulLengthInBytes);
// // #endif /* METAL_SIFIVE_UART0 */
//         // allocate stack (It will take 2 PMP Slot - So it is not needed to put align the StackBuffer)
//         xTaskTXDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskTXDefinition.usStackDepth * sizeof( StackType_t ) );
//         // xTaskTXDefinition.puxStackBuffer = xSendTaskStack;

// 		printf("xTaskTXDefinition.puxStackBuffer = %p\r\n", xTaskTXDefinition.puxStackBuffer);

//         xTaskCreateRestricted(  &xTaskTXDefinition,
//                                 &xHandle_SendTask);

		/* 
		 * Prepare xRegions for ListTasksShow Task 
		 */
//         memset(&xTaskListTasksShowDefinition.xRegions, 0, sizeof(xTaskListTasksShowDefinition.xRegions));

// 		xTaskListTasksShowDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskListTasksShowDefinition.usStackDepth * sizeof( StackType_t ) );

//         // xTaskListTasksShowDefinition.puxStackBuffer = xTaskListTasksShowStack;

//         xTaskListTasksShowDefinition.xRegions[0].ulLengthInBytes = xTaskListTasksShowDefinition.usStackDepth * sizeof( StackType_t );
//         xTaskListTasksShowDefinition.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                      (portPMP_REGION_ADDR_MATCH_NAPOT));

//         napot_addr_modifier (	xPmpInfo.granularity,
//                                 (size_t) xTaskListTasksShowDefinition.puxStackBuffer,
//                                 (size_t *) &xTaskListTasksShowDefinition.xRegions[0].pvBaseAddress,
//                                 xTaskListTasksShowDefinition.xRegions[0].ulLengthInBytes);


//         // authorize access to data and bss
// 				// Low address 
//         // xTaskListTasksShowDefinition.xRegions[0].ulLengthInBytes = 4;
//         // xTaskListTasksShowDefinition.xRegions[0].ulParameters = ((portPMP_REGION_READ_WRITE) |
//         //                                             (portPMP_REGION_ADDR_MATCH_NA4));
//         // addr_modifier ( xPmpInfo.granularity,
//         //                 ( size_t ) __unprivileged_data_section_start__,
//         //                 (size_t *) &xTaskListTasksShowDefinition.xRegions[0].pvBaseAddress);

// 				// High address 
//         xTaskListTasksShowDefinition.xRegions[1].ulLengthInBytes = 4;
//         xTaskListTasksShowDefinition.xRegions[1].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                     (portPMP_REGION_ADDR_MATCH_TOR));

//         addr_modifier ( xPmpInfo.granularity,
//                         ( size_t ) __unprivileged_data_section_end__,
//                         (size_t *) &xTaskListTasksShowDefinition.xRegions[1].pvBaseAddress);

// #ifdef METAL_SIFIVE_UART0
//     // allow access to UART peripheral
//         xTaskListTasksShowDefinition.xRegions[2].ulLengthInBytes = METAL_SIFIVE_UART0_0_SIZE;
//         xTaskListTasksShowDefinition.xRegions[2].ulParameters = ((portPMP_REGION_READ_WRITE) |
//                                                      (portPMP_REGION_ADDR_MATCH_NAPOT));

//         napot_addr_modifier (	xPmpInfo.granularity,
//                                 (size_t) METAL_SIFIVE_UART0_0_BASE_ADDRESS,
//                                 (size_t *) &xTaskListTasksShowDefinition.xRegions[2].pvBaseAddress,
//                                 xTaskListTasksShowDefinition.xRegions[2].ulLengthInBytes);
// #endif /* METAL_SIFIVE_UART0 */

//         // xTaskListTasksShowDefinition.puxStackBuffer = ( StackType_t * ) pvPortMalloc( xTaskListTasksShowDefinition.usStackDepth * sizeof( StackType_t ) );
// //         // xTaskListTasksShowDefinition.puxStackBuffer = xTaskListTasksShowStack;

// 		printf("xTaskListTasksShowDefinition.puxStackBuffer = %p\r\n", xTaskListTasksShowDefinition.puxStackBuffer);
// // 		// printf("Test = ");
// // 		// print_stack_pointer();
//         xTaskCreateRestricted(  &xTaskListTasksShowDefinition,
//                                 &xHandle_TaskListTasks);

		/* Start the tasks and timer running. */
		vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created. 
	or task have stoppped the Scheduler */

		// vTaskDelete( xHandle_SendTask );
		// vTaskDelete( xHandle_ReceiveTask );
	}
#endif /* ( portUSING_MPU_WRAPPERS == 1 ) */
}

/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
	TickType_t xNextWakeTime;
	BaseType_t xReturned;
	unsigned long ulCounter = 0;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
	( void ) xReturned;

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

static void prvQueueReceiveTask( void *pvParameters )
{
	unsigned long ulReceivedValue;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

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

	// // This demo will toggle LEDs colors so we define them here
	// led0_red = metal_led_get_rgb("LD0", "red");
	// led0_green = metal_led_get_rgb("LD0", "green");
	// led0_blue = metal_led_get_rgb("LD0", "blue");
	// if ((led0_red == NULL) || (led0_green == NULL) || (led0_blue == NULL))
	// {
	// 	printf(pcWarningMsg);
	// 	// write( STDOUT_FILENO, pcWarningMsg, strlen( pcWarningMsg ) );
	// }
	// else
	// {
	// 	// Enable each LED
	// 	metal_led_enable(led0_red);
	// 	metal_led_enable(led0_green);
	// 	metal_led_enable(led0_blue);

	// 	// All Off
	// 	metal_led_off(led0_red);
	// 	metal_led_off(led0_green);
	// 	metal_led_off(led0_blue);
	// 	// metal_led_on(led0_red);
	// 	// metal_led_on(led0_green);
	// 	// metal_led_on(led0_blue);
	// }
}
/*-----------------------------------------------------------*/


void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */

	taskDISABLE_INTERRUPTS();

#if( portUSING_MPU_WRAPPERS == 1 )
	/* need to be machine mode */
//	xPortRaisePrivilege();
#endif /* ( portUSING_MPU_WRAPPERS == 1 ) */
	printf("ERROR malloc\r\n");

	if ( led0_red != NULL )
	{
		// Red light on
		metal_led_off(led0_red);
	}

	_exit(1);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();

	printf("ERROR Stack overflow on func: %s\r\n",pcTaskName);

	// if ( led0_red != NULL )
	// {
	// 	// Red light on
	// 	metal_led_off(led0_red);
	// }

	_exit(1);
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* The tests in the full demo expect some interaction with interrupts. */
}
/*-----------------------------------------------------------*/

void vAssertCalled( void )
{
	taskDISABLE_INTERRUPTS();

	if ( led0_red != NULL )
	{
		// Red light on
		metal_led_off(led0_red);
	}

	_exit(1);
}
