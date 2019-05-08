/**
 * @file        arch/arm/m2351/src/numaker_pfm_m2351/nonsecure/main_ns.c
 * @brief       Provides functionality to start, initialize, load and execute
 *              FreeRTOS tasks
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

/* Included Files. */
#include "string.h"

#include <arm_cmse.h>
#include "M2351.h"
#include "version.h"
#include "config.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo application include. */

/* Pre-processor Definitions. */
/* All C pre-processor macros are defined here. */
#define NORMAL  "\033[0m"
#define BLACK   "\033[0;30m1"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define GRAY    "\033[0;37m"

/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */
typedef int32_t (*funcptr)(uint32_t);

/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */
static void prvSetupHardware( void );

static void App_Init(uint32_t u32BootBase);
static void DEBUG_PORT_Init(void);

/* NonSecure functions used for callbacks */
static int32_t NonSecure_LED_On(uint32_t num);
static int32_t NonSecure_LED_Off(uint32_t num);

static void LED_On(uint32_t us);
static void LED_Off(uint32_t us);

static void testTask1( void *pvParameters );
static void testTask2( void *pvParameters );
static void testTask3( void *pvParameters );

static void teeTest( void *pvParameters );
static void teeTest2( void *pvParameters );
static void teeTest3( void *pvParameters );

/* Private Data. */
/* All static data definitions appear here. */


/* Public Data. */
/* All data definitions with global scope appear here. */


/* Extern Function Prototypes */
/* CMSIS clock configuration function. */
extern void SystemCoreClockUpdate( void );
extern int tee_hello_world(void);
extern int tee_aes(void);

/* NonSecure Callable Functions from Secure Region */
extern int32_t Secure_func(void);
extern int32_t Secure_LED_On_callback(int32_t (*)(uint32_t));
extern int32_t Secure_LED_Off_callback(int32_t (*)(uint32_t));
extern int32_t Secure_LED_On(uint32_t num);
extern int32_t Secure_LED_Off(uint32_t num);

/* Private Functions. */

/**
 * @brief         prvSetupHardware - performs any application specific hardware
 *                configuration. The clocks, memory, etc. are configured before
 *                main() is called..
 *
 * @param         None
 *
 * @returns       None
 */
static void prvSetupHardware( void )
{
  DEBUG_PORT_Init();

  SystemCoreClockUpdate();
}

/**
 * @brief         DEBUG_PORT_Init - performs initialization of UART.
 *
 * @param         None
 *
 * @returns       None
 */
static void DEBUG_PORT_Init(void)
{
  DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
  DEBUG_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/* NonSecure functions used for callbacks */

/**
 * @brief         NonSecure_LED_On - callback for LED On call by Secure.
 *
 * @param         None
 *
 * @returns       None
 */
static int32_t NonSecure_LED_On(uint32_t num)
{
  (void) num;

  printf("Nonsecure LED On call by Secure\n");
  PC0_NS = 0;
  return 0;
}

/**
 * @brief         NonSecure_LED_Off - callback for LED Off call by Secure.
 *
 * @param         None
 *
 * @returns       None
 */
static int32_t NonSecure_LED_Off(uint32_t num)
{
  (void) num;

  printf("Nonsecure LED Off call by Secure\n");
  PC0_NS = 1;
  return 0;
}

/* NonSecure LED control */
/**
 * @brief         LED_On - performs LED On.
 *
 * @param         None
 *
 * @returns       None
 */
static void LED_On(uint32_t us)
{
  (void) us;

  printf("Nonsecure LED On\n");
  PC1_NS = 0;
}

/**
 * @brief         LED_Off - performs LED Off.
 *
 * @param         None
 *
 * @returns       None
 */
static void LED_Off(uint32_t us)
{
  (void) us;

  printf("Nonsecure LED Off\n");
  PC1_NS = 1;
}

/**
 * @brief         App_Init.
 *
 * @param         None
 *
 * @returns       None
 */
static void App_Init(uint32_t u32BootBase)
{
  funcptr fp;
  uint32_t u32StackBase;

  /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
  fp = ((funcptr) (*(((uint32_t *) SCB->VTOR) + 1)));

  /* Check if the stack is in secure SRAM space */
  u32StackBase = M32(u32BootBase);
  if ((u32StackBase >= 0x30000000UL) && (u32StackBase < 0x40000000UL)) {
    printf("Execute non-secure code ...\n");
    /* SCB.VTOR points to the target Secure vector table base address. */
    SCB->VTOR = u32BootBase;

    fp(0); /* Non-secure function call */
  } else {
    /* Something went wrong */
    printf("No code in non-secure region!\n");

    while (1)
      ;
  }
}

/**
 * @brief         testTask1 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void testTask1( void *pvParameters )
{
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    printf(CYAN "Thread 1 test\n" NORMAL);
    portDISABLE_INTERRUPTS();
    Secure_LED_On(6u);
    portENABLE_INTERRUPTS();
    vTaskDelay( xDelay );
    portDISABLE_INTERRUPTS();
    Secure_LED_Off(6u);
    portENABLE_INTERRUPTS();
    vTaskDelay( xDelay );
  } while (1);
}

/**
 * @brief         testTask2 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void testTask2( void *pvParameters )
{
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    printf(YELLOW "Thread 2 test\n" NORMAL);
    LED_On(7u);
    vTaskDelay( xDelay );
    LED_Off(7u);
    vTaskDelay( xDelay );
  } while (1);
}

#define BUFFER_SIZE 700

/**
 * @brief         testTask3 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void testTask3( void *pvParameters )
{
  const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;

  TickType_t xTimeNow;
  /* Define a buffer that is large enough to hold the generated table.  In most
   * cases the buffer will be too large to allocate on the stack, hence in this
   * example it is declared static. */

  static char cBuffer[BUFFER_SIZE];

  (void)pvParameters;

  do {
    printf("Thread 3 test\n");
    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();
    printf("\nAuto-reload timer callback executing = %d\n", xTimeNow );
    /* Pass the buffer into vTaskList() to generate the table of information. */
    vTaskList(cBuffer);
    portDISABLE_INTERRUPTS();
    printf(GREEN "%s\n" NORMAL, cBuffer);
    portENABLE_INTERRUPTS();
    vTaskDelay( xDelay );
  } while (1);
}

#ifdef CONFIG_APPS_HELLO_WORLD
/**
 * @brief         teeTest - .
 *
 * @param         None
 *
 * @returns       None
 */
static void teeTest( void *pvParameters )
{
  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    printf(RED "TEE test: Hello_world\n");
    portDISABLE_INTERRUPTS();
    tee_hello_world();
    printf(NORMAL);
    portENABLE_INTERRUPTS();
    vTaskDelay( xDelay );
  } while (1);
}
#endif

#ifdef CONFIG_APPS_AES
/**
 * @brief         teeTest - .
 *
 * @param         None
 *
 * @returns       None
 */
static void teeTest3( void *pvParameters )
{
  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    printf(RED "TEE test: AES\n");
    portDISABLE_INTERRUPTS();
    tee_aes();
    printf(NORMAL);
    portENABLE_INTERRUPTS();
    vTaskDelay( xDelay );
  } while (1);
}
#endif

#ifdef CONFIG_APPS_TEMPLATE
/**
 * @brief         teeTest - .
 *
 * @param         None
 *
 * @returns       None
 */
static void teeTest2( void *pvParameters )
{
  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    printf(RED "TEE test: template\n");
    portDISABLE_INTERRUPTS();
    tee_template();
    printf(NORMAL);
    portENABLE_INTERRUPTS();
    vTaskDelay( xDelay );
  } while (1);
}
#endif

/* Public Functions. */
/**
 * @brief         vApplicationMallocFailedHook - vApplicationMallocFailedHook()
 *                will only be called if configUSE_MALLOC_FAILED_HOOK is set to
 *                1 in FreeRTOSConfig.h.  It is a hook function that will get
 *                called if a call to pvPortMalloc() fails. pvPortMalloc() is
 *                called internally by the kernel whenever a task, queue, timer
 *                or semaphore is created.  It is also called by various parts
 *                of the demo application.  If heap_1.c or heap_2.c are used,
 *                then the size of the heap available to pvPortMalloc() is
 *                defined by configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the
 *                xPortGetFreeHeapSize() API function can be used to query the
 *                size of free heap space that remains (although it does not
 *                provide information on how the remaining heap might be
 *                fragmented).
 *
 * @param         None
 *
 * @returns       None
 */
void vApplicationMallocFailedHook( void )
{
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

/**
 * @brief         vApplicationIdleHook - vApplicationIdleHook() will only be
 *                called if configUSE_IDLE_HOOK is set to 1 in FreeRTOSConfig.h.
 *                It will be called on each iteration of the idle task.  It is
 *                essential that code added to this hook function never attempts
 *                to block in any way (for example, call xQueueReceive() with a
 *                block time specified, or call vTaskDelay()).  If the
 *                application makes use of the vTaskDelete() API function (as
 *                this demo application does) then it is also important that
 *                vApplicationIdleHook() is permitted to return to its calling
 *                function, because it is the responsibility of the idle task
 *                to clean up memory allocated by the kernel to any task that
 *                has since been deleted.
 *
 * @param         None
 *
 * @returns       None
 */
void vApplicationIdleHook( void )
{
  ;
}

/**
 * @brief         vApplicationStackOverflowHook - .
 *
 * @param         None
 *
 * @returns       None
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
  ( void ) pcTaskName;
  ( void ) pxTask;

  /* Run time stack overflow checking is performed if
  configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
  function is called if a stack overflow is detected. */
  taskDISABLE_INTERRUPTS();

  //__BKPT();

    printf("Stack overflow task name=%s\n", pcTaskName);

  for( ;; );
}

/**
 * @brief         vApplicationTickHook - This function will be called by each
 *                tick interrupt if configUSE_TICK_HOOK is set to 1 in
 *                FreeRTOSConfig.h.  User code can be added here, but the tick
 *                hook is called from an interrupt context, so code must not
 *                attempt to block, and only the interrupt safe FreeRTOS API
 *                functions can be used (those that end in FromISR()).
 *
 * @param         None
 *
 * @returns       None (function is not supposed to return)
 */
void vApplicationTickHook( void )
{
  ;
}

/**
 * @brief         main - entry point of mTower: nFreeRTOS.
 *
 * @param         None
 *
 * @returns       None (function is not supposed to return)
 */
int main( void )
{
  /* Prepare the hardware to run this demo. */
  prvSetupHardware();

  printf("\n\n\t-=mTower v" VERSION "=-  " __DATE__ "  " __TIME__"\n\n");

  printf("+---------------------------------------------+\n");
  printf("|     Nonsecure FreeRTOS is running ...       |\n");
  printf("+---------------------------------------------+\n");

  Secure_func();

  /* Init PC for Nonsecure LED control */
  GPIO_SetMode(PC_NS, BIT1 | BIT0, GPIO_MODE_OUTPUT);

  /* register NonSecure callbacks in Secure application */
  Secure_LED_On_callback(&NonSecure_LED_On);
  Secure_LED_Off_callback(&NonSecure_LED_Off);

  xTaskCreate( testTask1,     /* The function that implements the task. */
      "test1",                /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );

  xTaskCreate( testTask2,     /* The function that implements the task. */
      "test2",                /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );

  xTaskCreate( testTask3,     /* The function that implements the task. */
      "test3",                /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );
#ifdef CONFIG_APPS_HELLO_WORLD
  xTaskCreate( teeTest,       /* The function that implements the task. */
      "tee_test",             /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );
#endif

#ifdef CONFIG_APPS_AES
  xTaskCreate( teeTest3,       /* The function that implements the task. */
      "tee_test3",             /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );
#endif

#ifdef CONFIG_APPS_TEMPLATE
  xTaskCreate( teeTest2,       /* The function that implements the task. */
      "tee_test2",             /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );
#endif

  vTaskStartScheduler();

  /* If all is well, the scheduler will now be running, and the following
  line will never be reached.  If the following line does execute, then
  there was insufficient FreeRTOS heap memory available for the idle and/or
  timer tasks to be created.  See the memory management section on the
  FreeRTOS web site for more details. */
  for( ;; );

  return 0;
}

