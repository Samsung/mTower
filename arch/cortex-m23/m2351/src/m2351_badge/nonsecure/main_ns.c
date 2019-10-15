/**
 * @file        arch/arm/m2351/src/m2351_badge/nonsecure/main_ns.c
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
#define NORMAL  "\e[0m"
#define BLACK   "\e[0;30m"
#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define BLUE    "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN    "\e[0;36m"
#define GRAY    "\e[0;37m"

#define BBLACK   "\e[40m"
#define BRED     "\e[41m"
#define BGREEN   "\e[42m"
#define BYELLOW  "\e[43m"
#define BBLUE    "\e[44m"
#define BMAGENTA "\e[45m"
#define BCYAN    "\e[46m"
#define BGRAY    "\e[30;47m"

#define BUFFER_SIZE 700

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

static void teeLedBlinkTask( void *pvParameters );
static void testTask2( void *pvParameters );
static void clnSrvTask( void *pvParameters );
static void menuTask( void *pvParameters );
#ifdef CONFIG_APPS_SPY
static void spyAppTask( void /**pvParameters*/ );
#endif
static void teeHelloWorldTask( void *pvParameters );
static void teeAESTask( void *pvParameters );
static void teeHotpTask( void *pvParameters );
static void teeTestSuiteTask( void *pvParameters );


/* Private Data. */
/* All static data definitions appear here. */


/* Public Data. */
/* All data definitions with global scope appear here. */


/* Extern Function Prototypes */
/* CMSIS clock configuration function. */
extern void SystemCoreClockUpdate( void );

extern int tee_hello_world(void);
extern int tee_aes(void);
extern int tee_hotp(void);
extern int tee_tests(void);

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
 * @brief         menuTask1 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void menuTask(void *pvParameters)
{
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

  /* Define a buffer that is large enough to hold the generated table.  In most
   * cases the buffer will be too large to allocate on the stack, hence in this
   * example it is declared static. */

  static char cBuffer[BUFFER_SIZE];

  (void) pvParameters;

  char ch;
  do {
    printf("\n+- Menu ----------------------------------------------------+\n");
#ifdef CONFIG_APPS_HELLO_WORLD
    printf("| [1] - Run Hello World (Ported from OP-TEE, pseudo TA)     |\n");
#endif
#ifdef CONFIG_APPS_AES
    printf("| [2] - Run AES (Ported from OP-TEE, user TA)               |\n");
#endif
#ifdef CONFIG_APPS_HOTP
    printf("| [3] - Run HOTP (Ported from OP-TEE, user TA)              |\n");
#endif
    printf("| [4] - Get FreeRTOS Task List                              |\n");
#ifdef CONFIG_APPS_TEST
    printf("| [5] - Run Test Suite for GP TEE Client & Internal API     |\n");
#endif
#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
    printf("| [6] - Run H/W Security exception example (from non-secure)|\n");
    printf("| [7] - Run H/W Security exception example (from secure)    |\n");
#endif
//#ifdef CONFIG_APPS_SPY
    printf("| [8] - Run spy app that trying to get protected data       |\n");
//#ifend
    printf("+-----------------------------------------------------------+\n");

    printf("\n[%c]\n", ch = getchar());

    switch (ch) {
#ifdef CONFIG_APPS_HELLO_WORLD
      case '1':
        xTaskCreate(teeHelloWorldTask,  /* The function that implements the task. */
        "tHelloWorld",           /* The text name assigned to the task - for debug only as it is not used by the kernel. */
        256,                  /* The size of the stack to allocate to the task. */
        (void *) NULL,        /* The parameter passed to the task - just to check the functionality. */
        tskIDLE_PRIORITY + 2, /* The priority assigned to the task. */
        NULL);
        break;
#endif
#ifdef CONFIG_APPS_AES
      case '2':
        xTaskCreate(teeAESTask, /* The function that implements the task. */
        "tAES",          /* The text name assigned to the task - for debug only as it is not used by the kernel. */
        256,                  /* The size of the stack to allocate to the task. */
        (void *) NULL,        /* The parameter passed to the task - just to check the functionality. */
        tskIDLE_PRIORITY + 2, /* The priority assigned to the task. */
        NULL);
        break;
#endif
#ifdef CONFIG_APPS_HOTP
      case '3':
        xTaskCreate(teeHotpTask, /* The function that implements the task. */
        "tHOTP",          /* The text name assigned to the task - for debug only as it is not used by the kernel. */
        256,                  /* The size of the stack to allocate to the task. */
        (void *) NULL,        /* The parameter passed to the task - just to check the functionality. */
        tskIDLE_PRIORITY + 2, /* The priority assigned to the task. */
        NULL);
        break;
#endif
      case '4':
        /* Obtain the current tick count. */
        printf("\nAuto-reload timer callback executing = %d\n", xTaskGetTickCount());

        /* Pass the buffer into vTaskList() to generate the table of information. */
        vTaskList(cBuffer);
        portDISABLE_INTERRUPTS();
        printf("%s\n", cBuffer);
        portENABLE_INTERRUPTS();
        break;
#ifdef CONFIG_APPS_TEST
        case '5':
        xTaskCreate( teeTestSuiteTask,  /* The function that implements the task. */
            "tTestSuite",               /* The text name assigned to the task - for debug only as it is not used by the kernel. */
            256,                        /* The size of the stack to allocate to the task. */
            ( void * ) NULL,            /* The parameter passed to the task - just to check the functionality. */
            tskIDLE_PRIORITY + 2,       /* The priority assigned to the task. */
            NULL );
        break;
#endif
#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
      case '6':
        menu_security_exception_example_ns();
        break;
      case '7':
        portDISABLE_INTERRUPTS();
        Secure_func();
        portENABLE_INTERRUPTS();
        break;
#endif
#ifdef CONFIG_APPS_SPY
        case '8':
//        xTaskCreate( spyAppTask,  /* The function that implements the task. */
//            "spyAppTask",               /* The text name assigned to the task - for debug only as it is not used by the kernel. */
//            256,                        /* The size of the stack to allocate to the task. */
//            ( void * ) NULL,            /* The parameter passed to the task - just to check the functionality. */
//            tskIDLE_PRIORITY + 2,       /* The priority assigned to the task. */
//            NULL );

//        taskYIELD();
        spyAppTask();
        break;
#endif
      default:
        break;
    }
    taskYIELD();
  } while (1);
}

#ifdef CONFIG_APPS_SPY
const char app_protected[] =
    "\n ########################################"
    "\n ## Data protected by TrustZone        ##"
    "\n ########################################"
    "\n ## Non-secure      |  "GREEN"Secure          "NORMAL"##"
    "\n ## ##############  |  "GREEN"############### "NORMAL"##"
    "\n ## # Spy App    #"RED"--X->"GREEN"# Protected   # "NORMAL"##"
    "\n ## ##############  |  "GREEN"# data ######## "NORMAL"##"
    "\n ##                 |         "GREEN"|        "NORMAL"##"
    "\n ## ##############  |  "GREEN"############### "NORMAL"##"
    "\n ## # App        #  |  "GREEN"# Trusted App # "NORMAL"##"
    "\n ## ##############  |  "GREEN"######### "YELLOW"#########"
    "\n ##        "GREEN"|        "NORMAL"|         "GREEN"| "YELLOW"##       ##"
    "\n ## ##############  |  "GREEN"####### "YELLOW"## "GREEN"####    "YELLOW"##"
    "\n ## # TEE CL API #"GREEN"----># TEE IN"YELLOW"##"GREEN"API #    "YELLOW"##"
    "\n ## ##############  |  "GREEN"####### "YELLOW"## "GREEN"####    "YELLOW"##"
    "\n ##                 |         "YELLOW"###############"
    "\n ## ##############  |  "GREEN"###### "YELLOW"######   ######"
    "\n ## # FreeRTOS   #  |  "GREEN"# Reso "YELLOW"####### #######"
    "\n ## ##############  |  "GREEN"###### "YELLOW"####### #######"
    "\n ############################ "YELLOW"###############\n";

const char app_description[] =
    "ARM TrustZone technology protects data in secure memory\n"
    "from unauthorized access from Non-Secure app.\n"
    "Spy app cannot access data in secure memory.\n"
    "\nHowever, application can use special GP TEE API calls\n"
    "to perform operations with data in secure memory.\n"
    "\nFor example, data can be sent into secure memory,\n"
    "encrypted there and returned to non-secure memory,\n"
    "with encryption keys never leaving secure memory.\n";

const char spy_description[] =
    "Demonstration of unauthorized access to data in secure\n"
    "memory. When such access is detected,\n"
    "system generates a security hard fault and enters\n"
    "endless loop, requiring restart to continue operation.\n";

const char spy[] =
"\n                      MMMMMMMMMMMM-"
"\n                  .hMMMMMMMMMMMMMMMmo`"
"\n                `oMMMMMMMMMMMMMMMMMMMm+"
"\n               `MMh- `.MMMMMMMMMMM/-`/MM"
"\n               sMs`                   oM+"
"\n              .NN MMMMMMMm    mMMMMMMm Mm"
"\n              -MMMMP     qMMMMMp     qMMMM`"
"\n              .MMMMc     dMMMMMc     dMMMM`"
"\n               mM MMMMMMMp    qdMMMMMMh Mn`"
"\n                dMM`                 +MMp"
"\n           .dMMMMMMs-    ======    :hMMMMMMm."
"\n          hNMMMMMMMMMh+-         odMMMMMMMMMmy"
"\n        mMMMMMMMMMMMMMMMmddhhdmmMMMMMMMMMMMMMMMh "
"\n       mMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMh "
"\n     -mMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMh "
"\n   `mMMMMMo                                 sMMMMMh "
"\n   `dMMMMM/ "YELLOW"MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"NORMAL" oMMMMMy "
"\n   yMMMMMM/ "YELLOW"MM----------------------------MM"NORMAL"  sMMMMMMo"
"\n  /MMMMMMMs "YELLOW"oM- "GREEN"101010101011101010010100"YELLOW" -MM"NORMAL"  hMMMMMMN-"
"\n `mMMMMMMMd "YELLOW"/M- "GREEN"001010101010101000010101"YELLOW" -M."NORMAL"  NMMMMMMMd"
"\n +MMMMMMMMN`"YELLOW".M- "GREEN"101010101010101000010101"YELLOW" -M`"NORMAL" .MMMMMMMMM:"
"\n dMMMMMMMMM."YELLOW"`N- "GREEN"101011101000100011010100"YELLOW" -N"NORMAL"  /MMMMMMMMMs"
"\n yMMMMMMMMM/ "YELLOW"hM "GREEN"110101010111010101011011"YELLOW" Mh"NORMAL"  oMMMMMMMMMo"
"\n `oNMMMMMMMs "YELLOW"sM "GREEN"111101000101011010101111"YELLOW" Mh"NORMAL" hMMMMMMMm+"
"\n   `+hNMMMMd "YELLOW"/MM-----------------------MMM"NORMAL" -MMMMNy/`"
"\n      `:sdNm`"YELLOW".MMMMMMMMMMMMMMMMMMMMMMMMMMMN`"NORMAL".NNho:`"
"\n          `-` "YELLOW"MMMMMMMMMMMMMMMMMMMMMMMMMMMM "NORMAL"`-`"
"\n "
"\n              MMMMMMMMMMMMMMMMMMMMMMMMMMM/\n";
/**
 * @brief         spyAppTask - .
 *
 * @param         None
 *
 * @returns       None
 */
static void spyAppTask( void /**pvParameters*/ )
{
//  (void)pvParameters;
  printf("%s\n", app_protected);
  vTaskDelay( (TickType_t)1000 / portTICK_PERIOD_MS);
  printf("%s\n", app_description);
  vTaskDelay( (TickType_t)9000 / portTICK_PERIOD_MS);

//  printf("\e[2J");
//  UART_WAIT_TX_EMPTY((UART_T *)DEBUG_PORT);
  printf("%s\n", spy);
  vTaskDelay( (TickType_t)2000 / portTICK_PERIOD_MS);
  printf("\n%s\n", spy_description);
  vTaskDelay( (TickType_t)9000 / portTICK_PERIOD_MS);
  M32(0x20000000);

  do {
  } while (1);
}
#endif

/**
 * @brief         testTask1 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void teeLedBlinkTask( void *pvParameters )
{
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
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
    portDISABLE_INTERRUPTS();
    printf(YELLOW "Thread 2 test\n" NORMAL);
    portENABLE_INTERRUPTS();
    LED_On(7u);
    vTaskDelay( xDelay );
    LED_Off(7u);
    vTaskDelay( xDelay );
  } while (1);
}

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

#ifdef CONFIG_APPS_HELLO_WORLD
/**
 * @brief         teeHelloWorldTask - .
 *
 * @param         None
 *
 * @returns       None
 */
static void teeHelloWorldTask( void *pvParameters )
{
//  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    portDISABLE_INTERRUPTS();
    printf("TEE test: Hello_world\n");
    tee_hello_world();
    portENABLE_INTERRUPTS();
//    vTaskDelay( xDelay );
    vTaskDelete(NULL);
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
static void teeAESTask( void *pvParameters )
{
//  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    portDISABLE_INTERRUPTS();
    printf("TEE test: AES\n");
    tee_aes();
    portENABLE_INTERRUPTS();
    vTaskDelete(NULL);
//    vTaskDelay( xDelay );
  } while (1);
}
#endif

#ifdef CONFIG_APPS_HOTP
/**
 * @brief         teeTest4 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void teeHotpTask( void *pvParameters )
{
//  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    portDISABLE_INTERRUPTS();
    printf("TEE test: HOTP\n");
    tee_hotp();
    portENABLE_INTERRUPTS();
    vTaskDelete(NULL);
//    vTaskDelay( xDelay );
  } while (1);
}
#endif // CONFIG_APPS_HOTP

#ifdef CONFIG_APPS_TEST
/**
 * @brief         teeTest4 - .
 *
 * @param         None
 *
 * @returns       None
 */
static void teeTestSuiteTask( void *pvParameters )
{
//  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  (void)pvParameters;

  do {
    portDISABLE_INTERRUPTS();
    printf("\nTEE test: test suite\n");
    tee_tests();
    portENABLE_INTERRUPTS();
    vTaskDelete(NULL);
//    vTaskDelay( xDelay );
  } while (1);
}
#endif // CONFIG_APPS_TEST

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

#ifdef CONFIG_APPS_HW_SECURITY_EXCEPTION_EXAMPLE
void menu_security_exception_example_ns(void)
{
  char ch;
  int temp = 1;

  printf("+---------------------------------------------------------------------+\n");
  printf("| Type  | Range addresses         | Size      | Remarks               |\n");
  printf("+---------------------------------------------------------------------+\n");
  printf("| FLASH | 0x10040000 - 0x0007FFFF | 0x40000   | 256k                  |\n");
  printf("| RAM   | 0x30008000 - 0x30017FFF | 0x10000   | 64k                   |\n");
  printf("| GPIO  | PC                      |           | Port C is non-secure  |\n");
  printf("| GPIO  | PA,PB                   |           | Port A,B are secure   |\n");
  printf("+---------------------------------------------------------------------+\n\n");

  printf("+---------------------------------------------------------------------+\n");
  printf("| Key | Action                                   | Expected response  |\n");
  printf("+---------------------------------------------------------------------+\n");
  printf("| [1] | Read SRAM secure address 0x30000000      | Hard fault         |\n");
  printf("| [2] | Read SRAM secure address 0x20000000      | Hard fault         |\n");
  printf("| [3] | Read SRAM non-secure address 0x%08X  | Access successful  |\n",&temp);
  printf("|     | Read SRAM non-secure address 0x%08X  | Hard fault         |\n",(~0x10000000 & (unsigned int)&temp));
  printf("| [4] | Read FLASH secure address 0x00000000     | Hard fault         |\n");
  printf("| [5] | Read FLASH secure address 0x10000000     | Hard fault         |\n");
  printf("| [6] | Read FLASH non-secure address 0x10040000 | Access successful  |\n");
  printf("|     | Read FLASH non-secure address 0x00040000 | Hard fault         |\n");
  printf("| [7] | Read GPIO non-secure port PC1_NS         | Access successful  |\n");
  printf("|     | Write 0 GPIO non-secure port by PC1_NS   | Access successful  |\n");
  printf("|     | Write 1 GPIO non-secure port by PC1      | Hard fault         |\n");
  printf("| [8] | Read GPIO secure port PA10_NS            | RAZWI, sec. violat.|\n");
  printf("|     | Write 0 GPIO secure port by PA10_NS      | RAZWI, sec. violat.|\n");
  printf("|     | Write 1 GPIO secure port by PA10         | Hard fault         |\n");
  printf("| [9] | Write 0 to address 0x0 (directly)        | Hard fault         |\n");
  printf("| [a] | Read 0x30018000 address (nonexistent)    | Hard fault         |\n");
  printf("+---------------------------------------------------------------------+\n");

  printf("\n[%c] ", ch = getchar());

  switch (ch) {
    case '1':
      printf("Read SRAM secure address 0x30000000: Hard fault occurs in secure\n");
      M32(0x30000000);
      break;
    case '2':
      printf("Read SRAM secure address 0x20000000: Hard fault occurs in secure\n");
      M32(0x20000000);
      break;
    case '3':
      printf("Read SRAM non-secure address 0x%08X = %08x\n", &temp, M32(&temp));
      printf("    Read SRAM non-secure address 0x%08X: Hard fault occurs in secure\n", (~0x10000000 & (unsigned int)&temp));
      M32((~0x10000000 & (unsigned int)&temp));
      break;
    case '4':
      printf("Read FLASH secure address 0x00000000: Hard fault occurs in secure\n");
      M32(0x00000000);
      break;
    case '5':
      printf("Read FLASH secure address 0x10000000: Hard fault occurs in secure\n");
      M32(0x10000000);
      break;
    case '6':
      printf("Read FLASH non-secure address 0x10040000 = %08x\n", M32(0x10040000));
      printf("    Read FLASH non-secure address 0x00040000: Hard fault occurs in secure\n");
      M32(0x00040000);
      break;
    case '7':
      printf("Read GPIO non-secure port PC1_NS = %d\n", PC1_NS);
      printf("    Write 0 GPIO non-secure port by PC1_NS.");
      PC1_NS = 0;
      printf(" Result: PC1_NS = %d\n", PC1_NS);
      printf("    Write 1 GPIO non-secure port by PC1. Hard fault occurs in secure\n");
      PC1 = 1;
      break;
    case '8':
      printf("Read GPIO secure port PA10_NS = %d. Result: GPIO violation interrupt occurs\n", PA10_NS);
      printf("    Write 1 GPIO secure port by PA10_NS. Result: GPIO violation interrupt occurs\n");
      PA10_NS = 1;
      printf("    Write 1 GPIO secure port by PA10. Hard fault occurs in secure\n");
      PA10 = 1;
      break;
    case '9':
      M32(0x10000000) = 0;
      break;
    case 'a':
      M32(0x30018000);
      break;
    default:
      break;
  }
while(1);
}
#endif
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
  printf("|  Nonsecure FreeRTOS (BL33) is running ...   |\n");
  printf("+---------------------------------------------+\n");

  /* Init PC for Nonsecure LED control */
  GPIO_SetMode(PC_NS, BIT1 | BIT0, GPIO_MODE_OUTPUT);

//  Secure_func();

  /* register NonSecure callbacks in Secure application */
  Secure_LED_On_callback(&NonSecure_LED_On);
  Secure_LED_Off_callback(&NonSecure_LED_Off);

  xTaskCreate( menuTask,     /* The function that implements the task. */
        "menu",                /* The text name assigned to the task - for debug only as it is not used by the kernel. */
        768,                    /* The size of the stack to allocate to the task. */
        ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
        tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
        NULL );

  xTaskCreate( teeLedBlinkTask,     /* The function that implements the task. */
      "tLedBlink",                /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                    /* The size of the stack to allocate to the task. */
      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
      NULL );

//  xTaskCreate( testTask2,     /* The function that implements the task. */
//      "test2",                /* The text name assigned to the task - for debug only as it is not used by the kernel. */
//      256,                    /* The size of the stack to allocate to the task. */
//      ( void * ) NULL,        /* The parameter passed to the task - just to check the functionality. */
//      tskIDLE_PRIORITY + 2,   /* The priority assigned to the task. */
//      NULL );
//
  xTaskCreate( clnSrvTask,    /* The function that implements the task. */
      "clnSrv",               /* The text name assigned to the task - for debug only as it is not used by the kernel. */
      256,                     /* The size of the stack to allocate to the task. */
      ( void * ) NULL,         /* The parameter passed to the task - just to check the functionality. */
      tskIDLE_PRIORITY + 2,    /* The priority assigned to the task. */
      NULL );

  vTaskStartScheduler();

  /* If all is well, the scheduler will now be running, and the following
  line will never be reached.  If the following line does execute, then
  there was insufficient FreeRTOS heap memory available for the idle and/or
  timer tasks to be created.  See the memory management section on the
  FreeRTOS web site for more details. */
  for( ;; );

  return 0;
}

