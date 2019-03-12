/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Secure sample code for TrustZone
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <arm_cmse.h>
#include <stdio.h>
#include "NuMicro.h"                      /* Device header */
#include "partition_M2351.h"
#include "xom0_func.h"
#include "xom1_func.h"

#define NEXT_BOOT_BASE  0x10040000
#define JUMP_HERE       0xe7fee7ff      /* Instruction Code of "B ." */

/* typedef for NonSecure callback functions */
typedef __NONSECURE_CALL int32_t (*NonSecure_funcptr)(uint32_t);
typedef int32_t (*Secure_funcptr)(uint32_t);


/*----------------------------------------------------------------------------
  Secure functions exported to NonSecure application
  Must place in Non-secure Callable
 *----------------------------------------------------------------------------*/
__NONSECURE_ENTRY
int32_t Secure_func(void)
{
    printf("Secure NSC func\n");

    return 1;
}

__NONSECURE_ENTRY
int32_t Secure_LED_On(uint32_t num)
{
    printf("Secure LED On call by Non-secure\n");
    PA10 = 0;
    PB0 = 0;
    return 0;
}

__NONSECURE_ENTRY
int32_t Secure_LED_Off(uint32_t num)
{
    printf("Secure LED Off call by Non-secure\n");
    PA10 = 1;
    PB0 = 1;
    return 1;
}

/*----------------------------------------------------------------------------
  NonSecure callable function for NonSecure callback
 *----------------------------------------------------------------------------*/

NonSecure_funcptr pfNonSecure_LED_On = (NonSecure_funcptr)NULL;
NonSecure_funcptr pfNonSecure_LED_Off = (NonSecure_funcptr)NULL;

__NONSECURE_ENTRY
int32_t Secure_LED_On_callback(NonSecure_funcptr *callback)
{
    pfNonSecure_LED_On = (NonSecure_funcptr)cmse_nsfptr_create(callback);
    return 0;
}

__NONSECURE_ENTRY
int32_t Secure_LED_Off_callback(NonSecure_funcptr *callback)
{
    pfNonSecure_LED_Off = (NonSecure_funcptr)cmse_nsfptr_create(callback);
    return 0;
}

/*----------------------------------------------------------------------------
  Secure LED control function
 *----------------------------------------------------------------------------*/
int32_t LED_On(void)
{
    printf("Secure LED On\n");
    PA11 = 0;
    PB1 = 0;
    return 1;
}

int32_t LED_Off(void)
{
    printf("Secure LED Off\n");
    PA11 = 1;
    PB1 = 1;
    return 1;
}

/*----------------------------------------------------------------------------
  SysTick IRQ Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    static uint32_t u32Ticks;

    switch(u32Ticks++)
    {
        case   0:
            LED_On();
            break;
        case 200:
            LED_Off();
            break;
        case 300:
            if(pfNonSecure_LED_On != NULL)
            {
                pfNonSecure_LED_On(1u);
            }
            break;
        case 500:
            if(pfNonSecure_LED_Off != NULL)
            {
              pfNonSecure_LED_Off(1u);
            }
            break;

        default:
            if(u32Ticks > 600)
            {
                u32Ticks = 0;
            }
    }

}


void SYS_Init(void);
void DEBUG_PORT_Init(void);
void Boot_Init(uint32_t u32BootBase);

/*----------------------------------------------------------------------------
    Boot_Init function is used to jump to next boot code.
 *----------------------------------------------------------------------------*/
void Boot_Init(uint32_t u32BootBase)
{
    NonSecure_funcptr fp;

    /* SCB_NS.VTOR points to the Non-secure vector table base address. */
    SCB_NS->VTOR = u32BootBase;

    /* 1st Entry in the vector table is the Non-secure Main Stack Pointer. */
    __TZ_set_MSP_NS(*((uint32_t *)SCB_NS->VTOR));      /* Set up MSP in Non-secure code */

    /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
    fp = ((NonSecure_funcptr)(*(((uint32_t *)SCB_NS->VTOR) + 1)));

    /* Clear the LSB of the function address to indicate the function-call
       will cause a state switch from Secure to Non-secure */
    fp = cmse_nsfptr_create(fp);

    /* Check if the Reset_Handler address is in Non-secure space */
    if(cmse_is_nsfptr(fp) && (((uint32_t)fp & 0xf0000000) == 0x10000000))
    {
        printf("Execute non-secure code ...\n");
        fp(0); /* Non-secure function call */
    }
    else
    {
        /* Something went wrong */
        printf("No code in non-secure region!\n");
        printf("CPU will halted at non-secure state\n");

        /* Set nonsecure MSP in nonsecure region */
        __TZ_set_MSP_NS(NON_SECURE_SRAM_BASE + 512);

        /* Try to halted in non-secure state (SRAM) */
        M32(NON_SECURE_SRAM_BASE) = JUMP_HERE;
        fp = (NonSecure_funcptr)(NON_SECURE_SRAM_BASE + 1);
        fp(0);

        while(1);
    }
}

/** mTower version */
#define VERSION "0.1"
/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
    SYS_UnlockReg();

    SYS_Init();

    /* UART is configured as debug port */
    DEBUG_PORT_Init();

    printf("\n\n\t-=mTower v" VERSION "=-  " __DATE__ "  " __TIME__"\n\n");
    printf("+---------------------------------------------+\n");
    printf("|              Secure is running ...          |\n");
    printf("+---------------------------------------------+\n");

    /* Init GPIO Port A for secure LED control */
    GPIO_SetMode(PA, BIT11 | BIT10, GPIO_MODE_OUTPUT);

    /* Init GPIO Port B for secure LED control */
    GPIO_SetMode(PB, BIT1 | BIT0, GPIO_MODE_OUTPUT);

    /* Generate Systick interrupt each 10 ms */
    SysTick_Config(SystemCoreClock / 100);

    /* Set GPIO Port C to non-secure for LED control */
    SCU_SET_IONSSET(SCU_IONSSET_PC_Msk);

    XOM1_Func(1);

    XOM0_Func(0);

    Boot_Init(NEXT_BOOT_BASE);

    do
    {
        __WFI();
    }
    while(1);

}


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable PLL */
    CLK->PLLCTL = CLK_PLLCTL_128MHz_HIRC;

    /* Waiting for PLL stable */
    while((CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == 0);

    /* Set HCLK divider to 2 */
    CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_HCLKDIV_Msk)) | 1;

    /* Switch HCLK clock source to PLL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;

    /* Enable UART clock */
    CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk;

    /* Select UART clock source */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | CLK_CLKSEL1_UART0SEL_HIRC;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    PllClock        = 128000000;           // PLL
    SystemCoreClock = 128000000 / 2;       // HCLK
    CyclesPerUs     = 64000000 / 1000000;  // For SYS_SysTickDelay()

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~(UART0_RXD_PB12_Msk | UART0_TXD_PB13_Msk))) | UART0_RXD_PB12 | UART0_TXD_PB13;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init for nonsecure code                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    CLK->APBCLK0 |= CLK_APBCLK0_UART1CKEN_Msk;
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART1SEL_Msk)) | CLK_CLKSEL1_UART1SEL_HIRC;
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~UART1_RXD_PA2_Msk)) | UART1_RXD_PA2;
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~UART1_TXD_PA3_Msk)) | UART1_TXD_PA3;

}

void DEBUG_PORT_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/

    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
    DEBUG_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
