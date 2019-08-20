/**************************************************************************//**
 * @file     partition_M2351.c
 * @version  V3.00
 * @brief    SAU configuration for secure/nonsecure region settings.
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include "M2351.h"
#include "config.h"

#ifndef PARTITION_M2351
#define PARTITION_M2351

/* SRAMNSSET */
//#define SCU_SECURE_SRAM_SIZE      0x8000
#define NON_SECURE_SRAM_BASE    (0x30000000 + CONFIG_SCU_SECURE_SRAM_SIZE)

/*--------------------------------------------------------------------------------------------------------*/

/*
    NSBA
*/
#define FMC_INIT_NSBA          1
/*
//     <o>Secure Flash ROM Size <0x800-0x7FFFF:0x800>
*/

#define FMC_SECURE_ROM_SIZE      0x40000

#define FMC_NON_SECURE_BASE     (0x10000000 + FMC_SECURE_ROM_SIZE)

__STATIC_INLINE void FMC_NSBA_Setup(void)
{
    /* Skip NSBA Setupt according config */
    if(FMC_INIT_NSBA == 0)
        return;

    /* Check if NSBA value with current active NSBA */
    if(SCU->FNSADDR != FMC_SECURE_ROM_SIZE)
    {
        /* Unlock Protected Register */
        SYS_UnlockReg();

        /* Enable ISP and config update */
        FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_CFGUEN_Msk;

        /* Config Base of NSBA */
        FMC->ISPADDR = 0x200800;

        /* Read Non-secure base address config */
        FMC->ISPCMD = FMC_ISPCMD_READ;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
        while(FMC->ISPTRG);

        /* Setting NSBA when it is empty */
        if(FMC->ISPDAT == 0xfffffffful)
        {
            FMC->ISPDAT = FMC_SECURE_ROM_SIZE;
            FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
            FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
            while(FMC->ISPTRG);

            /* Force Chip Reset to valid new setting */
            SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
        }

        /* Fatal Error:
           FMC NSBA setting is different to FMC_INIT_NSBA_VAL.
           User must double confirm which one is wrong.

           If user need to change NSBA config of FMC, user must do Mess-erase by
           ISP or ICP.
        */
        while(1);
    }

}


/*--------------------------------------------------------------------------------------------------------*/


/*
// <h> Peripheral Secure Attribution Configuration
*/


/**
  \brief   Setup SCU Configuration Unit
  \details

 */
__STATIC_INLINE void SCU_Setup(void)
{
  int32_t i;

  /* Peripheral Secure Attribution Configuration */
#ifdef CONFIG_USBH_NONSECURE
  SCU_SET_PNSSET(USBH_Attr);
#endif

#ifdef CONFIG_SD0_NONSECURE
  SCU_SET_PNSSET(SD0_Attr);
#endif

#ifdef CONFIG_EBI_NONSECURE
  SCU_SET_PNSSET(EBI_Attr);
#endif

#ifdef CONFIG_PDMA1_NONSECURE
  SCU_SET_PNSSET(PDMA1_Attr);
#endif

#ifdef CONFIG_CRC_NONSECURE
  SCU_SET_PNSSET(CRC_Attr);
#endif

#ifdef CONFIG_CRPT_NONSECURE
  SCU_SET_PNSSET(CRPT_Attr);
#endif

#ifdef CONFIG_RTC_NONSECURE
  SCU_SET_PNSSET(RTC_Attr);
#endif

#ifdef CONFIG_EADC_NONSECURE
  SCU_SET_PNSSET(EADC_Attr);
#endif

#ifdef CONFIG_ACMP01_NONSECURE
  SCU_SET_PNSSET(ACMP01_Attr);
#endif

#ifdef CONFIG_DAC_NONSECURE
  SCU_SET_PNSSET(DAC_Attr);
#endif

#ifdef CONFIG_I2S0_NONSECURE
  SCU_SET_PNSSET(I2S0_Attr);
#endif

#ifdef CONFIG_OTG_NONSECURE
  SCU_SET_PNSSET(OTG_Attr);
#endif

#ifdef CONFIG_TMR23_NONSECURE
  SCU_SET_PNSSET(TMR23_Attr);
#endif

#ifdef CONFIG_EPWM0_NONSECURE
  SCU_SET_PNSSET(EPWM0_Attr);
#endif

#ifdef CONFIG_EPWM1_NONSECURE
  SCU_SET_PNSSET(EPWM1_Attr);
#endif

#ifdef CONFIG_BPWM0_NONSECURE
  SCU_SET_PNSSET(BPWM0_Attr);
#endif

#ifdef CONFIG_BPWM1_NONSECURE
  SCU_SET_PNSSET(BPWM1_Attr);
#endif

#ifdef CONFIG_QSPI0_NONSECURE
  SCU_SET_PNSSET(QSPI0_Attr);
#endif

#ifdef CONFIG_SPI0_NONSECURE
  SCU_SET_PNSSET(SPI0_Attr);
#endif

#ifdef CONFIG_SPI1_NONSECURE
  SCU_SET_PNSSET(SPI1_Attr);
#endif

#ifdef CONFIG_SPI2_NONSECURE
  SCU_SET_PNSSET(SPI2_Attr);
#endif

#ifdef CONFIG_SPI3_NONSECURE
  SCU_SET_PNSSET(SPI3_Attr);
#endif

#ifdef CONFIG_UART0_NONSECURE
  SCU_SET_PNSSET(UART0_Attr);
#endif

#ifdef CONFIG_UART1_NONSECURE
  SCU_SET_PNSSET(UART1_Attr);
#endif

#ifdef CONFIG_UART2_NONSECURE
  SCU_SET_PNSSET(UART2_Attr);
#endif

#ifdef CONFIG_UART3_NONSECURE
  SCU_SET_PNSSET(UART3_Attr);
#endif

#ifdef CONFIG_UART4_NONSECURE
  SCU_SET_PNSSET(UART4_Attr);
#endif

#ifdef CONFIG_UART5_NONSECURE
  SCU_SET_PNSSET(UART5_Attr);
#endif

#ifdef CONFIG_I2C0_NONSECURE
  SCU_SET_PNSSET(I2C0_Attr);
#endif

#ifdef CONFIG_I2C1_NONSECURE
  SCU_SET_PNSSET(I2C1_Attr);
#endif

#ifdef CONFIG_I2C2_NONSECURE
  SCU_SET_PNSSET(I2C2_Attr);
#endif

#ifdef CONFIG_SC0_NONSECURE
  SCU_SET_PNSSET(SC0_Attr);
#endif

#ifdef CONFIG_SC1_NONSECURE
  SCU_SET_PNSSET(SC1_Attr);
#endif

#ifdef CONFIG_SC2_NONSECURE
  SCU_SET_PNSSET(SC2_Attr);
#endif

#ifdef CONFIG_CAN0_NONSECURE
  SCU_SET_PNSSET(CAN0_Attr);
#endif

#ifdef CONFIG_QEI0_NONSECURE
  SCU_SET_PNSSET(QEI0_Attr);
#endif

#ifdef CONFIG_QEI1_NONSECURE
  SCU_SET_PNSSET(QEI1_Attr);
#endif

#ifdef CONFIG_ECAP0_NONSECURE
  SCU_SET_PNSSET(ECAP0_Attr);
#endif

#ifdef CONFIG_ECAP1_NONSECURE
  SCU_SET_PNSSET(ECAP1_Attr);
#endif

#ifdef CONFIG_TRNG_NONSECURE
  SCU_SET_PNSSET(TRNG_Attr);
#endif

#ifdef CONFIG_USBD_NONSECURE
  SCU_SET_PNSSET(USBD_Attr);
#endif

#ifdef CONFIG_USCI0_NONSECURE
  SCU_SET_PNSSET(USCI0_Attr);
#endif

#ifdef CONFIG_USCI1_NONSECURE
  SCU_SET_PNSSET(USCI1_Attr);
#endif

  /* GPIO Secure Attribution Configuration */
  /* Px     0 = Secure; 1 = Non-Secure */

  /* Set GPIO Port A to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PA
  SCU_SET_IONSSET(SCU_IONSSET_PA_Msk);
#endif

  /* Set GPIO Port B to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PB
  SCU_SET_IONSSET(SCU_IONSSET_PB_Msk);
#endif

  /* Set GPIO Port C to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PC
  SCU_SET_IONSSET(SCU_IONSSET_PC_Msk);
#endif

  /* Set GPIO Port D to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PD
  SCU_SET_IONSSET(SCU_IONSSET_PD_Msk);
#endif

  /* Set GPIO Port E to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PE
  SCU_SET_IONSSET(SCU_IONSSET_PE_Msk);
#endif

  /* Set GPIO Port F to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PF
  SCU_SET_IONSSET(SCU_IONSSET_PF_Msk);
#endif

  /* Set GPIO Port G to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PG
  SCU_SET_IONSSET(SCU_IONSSET_PG_Msk);
#endif

  /* Set GPIO Port H to non-secure */
#ifdef CONFIG_GPIO_NONSECURE_PH
  SCU_SET_IONSSET(SCU_IONSSET_PH_Msk);
#endif

  /* Set Non-secure SRAM */
  for (i = 11; i >= CONFIG_SCU_SECURE_SRAM_SIZE / 8192; i--) {
    SCU->SRAMNSSET |= (1U << i);
  }

#ifdef CONFIG_APB0IEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_APB0IEN_Msk);
#endif

#ifdef CONFIG_APB1IEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_APB1IEN_Msk);
#endif

#ifdef CONFIG_GPIOIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_GPIOIEN_Msk);
#endif

#ifdef CONFIG_EBIIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_EBIIEN_Msk);
#endif

#ifdef CONFIG_USBHIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_USBHIEN_Msk);
#endif

#ifdef CONFIG_CRCIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_CRCIEN_Msk);
#endif

#ifdef CONFIG_SDH0IEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_SDH0IEN_Msk);
#endif

#ifdef CONFIG_PDMA0IEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_PDMA0IEN_Msk);
#endif

#ifdef CONFIG_PDMA1IEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_PDMA1IEN_Msk);
#endif

#ifdef CONFIG_SRAM0IEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_SRAM0IEN_Msk);
#endif

#ifdef CONFIG_SRAM1IEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_SRAM1IEN_Msk);
#endif

#ifdef CONFIG_FMCIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_FMCIEN_Msk);
#endif

#ifdef CONFIG_FLASHIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_FLASHIEN_Msk);
#endif

#ifdef CONFIG_SCUIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_SCUIEN_Msk);
#endif

#ifdef CONFIG_SYSIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_SYSIEN_Msk);
#endif

#ifdef CONFIG_CRPTIEN_SECURE_VIOLATION_INTERRUPT
  SCU_ENABLE_INT(SCU_SVIOIEN_CRPTIEN_Msk);
#endif

}

/*
// <e>Setup behavior of Sleep and Exception Handling
*/
#define SCB_CSR_AIRCR_INIT  1

/*
//   <o> Deep Sleep can be enabled by
//     <0=>Secure and Non-Secure state
//     <1=>Secure state only
//   <i> Value for SCB->CSR register bit DEEPSLEEPS
*/
#define SCB_CSR_DEEPSLEEPS_VAL  0

/*
//   <o>System reset request accessible from
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for SCB->AIRCR register bit SYSRESETREQS
*/
#define SCB_AIRCR_SYSRESETREQS_VAL  0

/*
//   <o>Priority of Non-Secure exceptions is
//     <0=> Not altered
//     <1=> Lowered to 0x80-0xFF
//   <i> Value for SCB->AIRCR register bit PRIS
*/
#define SCB_AIRCR_PRIS_VAL      0

/* Assign HardFault to be always secure for safe */
#define SCB_AIRCR_BFHFNMINS_VAL 0

/*
    max 128 SAU regions.
    SAU regions are defined in partition.h
 */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (CONFIG_SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (CONFIG_SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U

/**
  \brief   Setup a SAU Region
  \details Writes the region information contained in SAU_Region to the
           registers SAU_RNR, SAU_RBAR, and SAU_RLAR
 */
__STATIC_INLINE void TZ_SAU_Setup(void)
{

#if defined (__SAU_PRESENT) && (__SAU_PRESENT == 1U)

#ifdef CONFIG_SAU_INIT_REGION0
# ifdef CONFIG_REGION0_SECURE
#   define SAU_INIT_NSC0 1
# else
#   define SAU_INIT_NSC0 0
# endif
  SAU_INIT_REGION(0);
#endif

#ifdef CONFIG_SAU_INIT_REGION1
# ifdef CONFIG_REGION1_SECURE
#   define SAU_INIT_NSC1 1
# else
#   define SAU_INIT_NSC1 0
# endif
  SAU_INIT_REGION(1);
#endif

#ifdef CONFIG_SAU_INIT_REGION2
# ifdef CONFIG_REGION2_SECURE
#   define SAU_INIT_NSC2 1
# else
#   define SAU_INIT_NSC2 0
# endif
  SAU_INIT_REGION(2);
#endif

#ifdef CONFIG_SAU_INIT_REGION3
# ifdef CONFIG_REGION3_SECURE
#   define SAU_INIT_NSC3 1
# else
#   define SAU_INIT_NSC3 0
# endif
  SAU_INIT_REGION(3);
#endif

#ifdef CONFIG_SAU_INIT_REGION4
# ifdef CONFIG_REGION4_SECURE
#   define SAU_INIT_NSC4 1
# else
#   define SAU_INIT_NSC4 0
# endif
  SAU_INIT_REGION(4);
#endif

#ifdef CONFIG_SAU_INIT_REGION5
# ifdef CONFIG_REGION5_SECURE
#   define SAU_INIT_NSC5 1
# else
#   define SAU_INIT_NSC5 0
# endif
  SAU_INIT_REGION(5);
#endif

#ifdef CONFIG_SAU_INIT_REGION6
# ifdef CONFIG_REGION6_SECURE
#   define SAU_INIT_NSC6 1
# else
#   define SAU_INIT_NSC6 0
# endif
  SAU_INIT_REGION(6);
#endif

#ifdef CONFIG_SAU_INIT_REGION7
# ifdef CONFIG_REGION7_SECURE
#   define SAU_INIT_NSC7 1
# else
#   define SAU_INIT_NSC7 0
# endif
  SAU_INIT_REGION(7);
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
    SAU->CTRL = ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk) |
                ((SAU_INIT_CTRL_ALLNS  << SAU_CTRL_ALLNS_Pos)  & SAU_CTRL_ALLNS_Msk)   ;
#endif

#endif /* defined (__SAU_PRESENT) && (__SAU_PRESENT == 1U) */

#if defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U)
    SCB->SCR   = (SCB->SCR   & ~(SCB_SCR_SLEEPDEEPS_Msk)) |
                 ((SCB_CSR_DEEPSLEEPS_VAL     << SCB_SCR_SLEEPDEEPS_Pos)     & SCB_SCR_SLEEPDEEPS_Msk);

//    SCB->AIRCR = (SCB->AIRCR & ~(SCB_AIRCR_SYSRESETREQS_Msk | SCB_AIRCR_BFHFNMINS_Msk |  SCB_AIRCR_PRIS_Msk)) |
//                 ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) |
//                 ((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk)    |
//                 ((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk);

    SCB->AIRCR = (0x05FA << 16) |
                 ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) |
                 ((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk)    |
                 ((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk);



#endif /* defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U) */

#if defined (SCB_ICSR_INIT) && (SCB_ICSR_INIT == 1U)
    SCB->ICSR  = (SCB->ICSR  & ~(SCB_ICSR_STTNS_Msk)) |
                 ((SCB_ICSR_STTNS_VAL         << SCB_ICSR_STTNS_Pos)         & SCB_ICSR_STTNS_Msk);
#endif /* defined (SCB_ICSR_INIT) && (SCB_ICSR_INIT == 1U) */

#define SCU_SET_INTSSET(IRQn) ((uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL)))

  NVIC->ITNS[0] =
#ifdef  CONFIG_RTC_INTERRUPT_NONSECURE
      SCU_SET_INTSSET(RTC_IRQn) |
#endif
#ifdef CONFIG_TAMPER_INTERRUPT_NONSECURE
          SCU_SET_INTSSET(TAMPER_IRQn) |
#endif
#ifdef CONFIG_EINT0_INTERRUPT_NONSECURE
          SCU_SET_INTSSET(EINT0_IRQn) |
#endif
#ifdef CONFIG_EINT1_INTERRUPT_NONSECURE
          SCU_SET_INTSSET(EINT1_IRQn) |
#endif
#ifdef CONFIG_EINT2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EINT2_IRQn) |
#endif
#ifdef CONFIG_EINT3_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EINT3_IRQn) |
#endif
#ifdef CONFIG_EINT4_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EINT4_IRQn) |
#endif
#ifdef CONFIG_EINT5_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EINT5_IRQn) |
#endif
#ifdef CONFIG_GPA_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPA_IRQn) |
#endif
#ifdef CONFIG_GPB_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPB_IRQn) |
#endif
#ifdef CONFIG_GPC_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPC_IRQn) |
#endif
#ifdef CONFIG_GPD_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPD_IRQn) |
#endif
#ifdef CONFIG_GPE_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPE_IRQn) |
#endif
#ifdef CONFIG_GPF_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPF_IRQn) |
#endif
#ifdef CONFIG_QSPI0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(QSPI0_IRQn) |
#endif
#ifdef CONFIG_SPI0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SPI0_IRQn) |
#endif
#ifdef CONFIG_BRAKE0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(BRAKE0_IRQn) |
#endif
#ifdef CONFIG_EPWM0_P0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EPWM0_P0_IRQn) |
#endif
#ifdef CONFIG_EPWM0_P1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EPWM0_P1_IRQn) |
#endif
#ifdef CONFIG_EPWM0_P2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EPWM0_P2_IRQn) |
#endif
#ifdef CONFIG_BRAKE1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(BRAKE1_IRQn) |
#endif
#ifdef CONFIG_EPWM1_P0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EPWM1_P0_IRQn) |
#endif
#ifdef CONFIG_EPWM1_P1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EPWM1_P1_IRQn) |
#endif
#ifdef CONFIG_EPWM1_P2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EPWM1_P2_IRQn) |
#endif
  0;

  NVIC->ITNS[1] =
#ifdef CONFIG_TMR2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(TMR2_IRQn) |
#endif
#ifdef CONFIG_TMR3_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(TMR3_IRQn) |
#endif
#ifdef CONFIG_UART0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(UART0_IRQn) |
#endif
#ifdef CONFIG_UART1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(UART1_IRQn) |
#endif
#ifdef CONFIG_I2C0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(I2C0_IRQn) |
#endif
#ifdef CONFIG_I2C1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(I2C1_IRQn) |
#endif
#ifdef CONFIG_DAC_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(DAC_IRQn) |
#endif
#ifdef CONFIG_EADC0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EADC0_IRQn) |
#endif
#ifdef CONFIG_EADC1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EADC1_IRQn) |
#endif
#ifdef CONFIG_ACMP01_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(ACMP01_IRQn) |
#endif
#ifdef CONFIG_EADC2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EADC2_IRQn) |
#endif
#ifdef CONFIG_EADC3_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EADC3_IRQn) |
#endif
#ifdef CONFIG_UART2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(UART2_IRQn) |
#endif
#ifdef CONFIG_UART3_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(UART3_IRQn) |
#endif
#ifdef CONFIG_SPI1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SPI1_IRQn) |
#endif
#ifdef CONFIG_SPI2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SPI2_IRQn) |
#endif
#ifdef CONFIG_USBD_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(USBD_IRQn) |
#endif
#ifdef CONFIG_USBH_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(USBH_IRQn) |
#endif
#ifdef CONFIG_USBOTG_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(USBOTG_IRQn) |
#endif
#ifdef CONFIG_CAN0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(CAN0_IRQn) |
#endif
#ifdef CONFIG_SC0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SC0_IRQn) |
#endif
#ifdef CONFIG_SC1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SC1_IRQn) |
#endif
#ifdef CONFIG_SC2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SC2_IRQn) |
#endif
#ifdef CONFIG_SPI3_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SPI3_IRQn) |
#endif
  0;

  NVIC->ITNS[2] =
#ifdef CONFIG_SDH0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(SDH0_IRQn) |
#endif
#ifdef CONFIG_I2S0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(I2S0_IRQn) |
#endif
#ifdef CONFIG_CRYPTO_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(CRYPTO_IRQn) |
#endif
#ifdef CONFIG_GPG_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPG_IRQn) |
#endif
#ifdef CONFIG_EINT6_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EINT6_IRQn) |
#endif
#ifdef CONFIG_UART4_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(UART4_IRQn) |
#endif
#ifdef CONFIG_UART5_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(UART5_IRQn) |
#endif
#ifdef CONFIG_USCI0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(USCI0_IRQn) |
#endif
#ifdef CONFIG_USCI1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(USCI1_IRQn) |
#endif
#ifdef CONFIG_BPWM0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(BPWM0_IRQn) |
#endif
#ifdef CONFIG_BPWM1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(BPWM1_IRQn) |
#endif
#ifdef CONFIG_I2C2_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(I2C2_IRQn) |
#endif
#ifdef CONFIG_QEI0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(QEI0_IRQn) |
#endif
#ifdef CONFIG_QEI1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(QEI1_IRQn) |
#endif
#ifdef CONFIG_ECAP0_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(ECAP0_IRQn) |
#endif
#ifdef CONFIG_ECAP1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(ECAP1_IRQn) |
#endif
#ifdef CONFIG_GPH_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(GPH_IRQn) |
#endif
#ifdef CONFIG_EINT7_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(EINT7_IRQn) |
#endif
#ifdef CONFIG_USBH_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(USBH_IRQn) |
#endif
  0;

  NVIC->ITNS[3] =
#ifdef CONFIG_PDMA1_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(PDMA1_IRQn) |
#endif
#ifdef CONFIG_TRNG_INTERRUPT_NONSECURE
  SCU_SET_INTSSET(TRNG_IRQn) |
#endif
  0;

}

#endif  /* PARTITION_M2351 */

