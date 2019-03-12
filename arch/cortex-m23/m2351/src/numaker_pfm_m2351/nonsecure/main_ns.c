/**************************************************************************//**
 * @file     main_ns.c
 * @version  V1.00
 * @brief    Non-secure sample code for TrustZone
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <arm_cmse.h>
#include "NuMicro.h"
#include "xom2_func.h"
#include "xom3_func.h"

typedef int32_t (*funcptr)(uint32_t);

extern int32_t Secure_func(void);
void App_Init(uint32_t u32BootBase);
void DEBUG_PORT_Init(void);

/*----------------------------------------------------------------------------
  NonSecure Callable Functions from Secure Region
 *----------------------------------------------------------------------------*/
extern int32_t Secure_LED_On_callback(int32_t (*)(uint32_t));
extern int32_t Secure_LED_Off_callback(int32_t (*)(uint32_t));
extern int32_t Secure_LED_On(uint32_t num);
extern int32_t Secure_LED_Off(uint32_t num);

/*----------------------------------------------------------------------------
  NonSecure functions used for callbacks
 *----------------------------------------------------------------------------*/
int32_t NonSecure_LED_On(uint32_t num)
{
    printf("Nonsecure LED On call by Secure\n");
    PC0_NS = 0;
    return 0;
}

int32_t NonSecure_LED_Off(uint32_t num)
{
    printf("Nonsecure LED Off call by Secure\n");
    PC0_NS = 1;
    return 0;
}

/*----------------------------------------------------------------------------
  NonSecure LED control
 *----------------------------------------------------------------------------*/
void LED_On(uint32_t us)
{
    printf("Nonsecure LED On\n");
    PC1_NS = 0;
}

void LED_Off(uint32_t us)
{
    printf("Nonsecure LED Off\n");
    PC1_NS = 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* SysTick IRQ Handler                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    static uint32_t u32Ticks;

    switch(u32Ticks++)
    {
        case   0:
            LED_On(7u);
            break;
        case 200:
            Secure_LED_On(6u);
            break;
        case 300:
            LED_Off(7u);
            break;
        case 500:
            Secure_LED_Off(6u);
            break;
        default:
            if(u32Ticks > 600)
            {
                u32Ticks = 0;
            }
    }
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
    DEBUG_PORT_Init();

    printf("+---------------------------------------------+\n");
    printf("|           Nonsecure is running ...          |\n");
    printf("+---------------------------------------------+\n");

    Secure_func();

    /* Init PC for Nonsecure LED control */
    GPIO_SetMode(PC_NS, BIT1 | BIT0, GPIO_MODE_OUTPUT);
    PC1_NS = 1;

    /* register NonSecure callbacks in Secure application */
    Secure_LED_On_callback(&NonSecure_LED_On);
    Secure_LED_Off_callback(&NonSecure_LED_Off);

    /* Generate Systick interrupt each 10 ms */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 100);

    XOM3_Func(3);

    XOM2_Func(2);

    while(1);
}

void DEBUG_PORT_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/

    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
    DEBUG_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

void App_Init(uint32_t u32BootBase)
{
    funcptr fp;
    uint32_t u32StackBase;

    /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
    fp = ((funcptr)(*(((uint32_t *)SCB->VTOR) + 1)));

    /* Check if the stack is in secure SRAM space */
    u32StackBase = M32(u32BootBase);
    if((u32StackBase >= 0x30000000UL) && (u32StackBase < 0x40000000UL))
    {
        printf("Execute non-secure code ...\n");
        /* SCB.VTOR points to the target Secure vector table base address. */
        SCB->VTOR = u32BootBase;

        fp(0); /* Non-secure function call */
    }
    else
    {
        /* Something went wrong */
        printf("No code in non-secure region!\n");

        while(1);
    }
}

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
