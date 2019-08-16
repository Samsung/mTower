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

  SCU->SVIOIEN = SCU_SVIOIEN_GPIOIEN_Msk;
//    | SCU_SVIOIEN_SRAM0IEN_Msk | SCU_SVIOIEN_SRAM1IEN_Msk;

}


/* ---------------------------------------------------------------------------------------------------- */

/*
// <e>Secure Attribute Unit (SAU) Control
*/
#define SAU_INIT_CTRL 1

/*
//   <q> Enable SAU
//   <i> To enable Secure Attribute Unit (SAU).
*/
#define SAU_INIT_CTRL_ENABLE 1

/*
//   <o> All Memory Attribute When SAU is disabled
//     <0=> All Memory is Secure
//     <1=> All Memory is Non-Secure
//   <i> To set the ALLNS bit in SAU CTRL.
//   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
*/
#define SAU_INIT_CTRL_ALLNS  0

/*
// </e>
*/

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
// </e>
*/


/*
// <h>Assign Interrupt to Secure or Non-secure Vector
*/


/*
    Initialize ITNS 0 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS0    1
/*
// BODOUT       Always secure
// IRC          Always secure
// PWRWU_       Always secure
// SRAM_PERR    Always secure
// CLKFAIL      Always secure

//   <o.6>  RTC                   <0=> Secure <1=> Non-Secure
//   <o.7>  TAMPER                <0=> Secure <1=> Non-Secure
// WDT  Always secure
// WWDT Always secure
//   <h> EINT
//   <o.10> EINT0                 <0=> Secure <1=> Non-Secure
//   <o.11> EINT1                 <0=> Secure <1=> Non-Secure
//   <o.12> EINT2                 <0=> Secure <1=> Non-Secure
//   <o.13> EINT3                 <0=> Secure <1=> Non-Secure
//   <o.14> EINT4                 <0=> Secure <1=> Non-Secure
//   <o.15> EINT5                 <0=> Secure <1=> Non-Secure
//   </h>
//   <h> GPIO
//   <o.16> GPA                   <0=> Secure <1=> Non-Secure
//   <o.17> GPB                   <0=> Secure <1=> Non-Secure
//   <o.18> GPC                   <0=> Secure <1=> Non-Secure
//   <o.19> GPD                   <0=> Secure <1=> Non-Secure
//   <o.20> GPE                   <0=> Secure <1=> Non-Secure
//   <o.21> GPF               <0=> Secure <1=> Non-Secure
//   </h>
//   <o.22> QSPI0              <0=> Secure <1=> Non-Secure
//   <o.23> SPI0              <0=> Secure <1=> Non-Secure
//   <h> EPWM
//   <o.24> BRAKE0            <0=> Secure <1=> Non-Secure
//   <o.25> EPWM0_P0          <0=> Secure <1=> Non-Secure
//   <o.26> EPWM0_P1          <0=> Secure <1=> Non-Secure
//   <o.27> EPWM0_P2          <0=> Secure <1=> Non-Secure
//   <o.28> BRAKE1            <0=> Secure <1=> Non-Secure
//   <o.29> EPWM1_P0          <0=> Secure <1=> Non-Secure
//   <o.30> EPWM1_P1          <0=> Secure <1=> Non-Secure
//   <o.31> EPWM1_P2          <0=> Secure <1=> Non-Secure
//   </h>
//
*/
#define NVIC_INIT_ITNS0_VAL      0x0

/*
    Initialize ITNS 1 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS1    1
/*
//   <h> TIMER
// TMR0 Always secure
// TMR1 Always secure
//   <o.2>  TMR2              <0=> Secure <1=> Non-Secure
//   <o.3>  TMR3              <0=> Secure <1=> Non-Secure
//   </h>
//   <o.4>  UART0             <0=> Secure <1=> Non-Secure
//   <o.5>  UART1             <0=> Secure <1=> Non-Secure
//   <o.6>  I2C0              <0=> Secure <1=> Non-Secure
//   <o.7>  I2C1              <0=> Secure <1=> Non-Secure
// PDMA0 is secure only
//   <o.9>  DAC               <0=> Secure <1=> Non-Secure
//   <o.10> EADC0             <0=> Secure <1=> Non-Secure
//   <o.11> EADC1             <0=> Secure <1=> Non-Secure
//   <o.12> ACMP01            <0=> Secure <1=> Non-Secure

//   <o.14> EADC2             <0=> Secure <1=> Non-Secure
//   <o.15> EADC3             <0=> Secure <1=> Non-Secure
//   <o.16> UART2             <0=> Secure <1=> Non-Secure
//   <o.17> UART3             <0=> Secure <1=> Non-Secure

//   <o.19> SPI1              <0=> Secure <1=> Non-Secure
//   <o.20> SPI2              <0=> Secure <1=> Non-Secure
//   <o.21> USBD              <0=> Secure <1=> Non-Secure
//   <o.22> USBH              <0=> Secure <1=> Non-Secure
//   <o.23> USBOTG            <0=> Secure <1=> Non-Secure
//   <o.24> CAN0              <0=> Secure <1=> Non-Secure

//   <h> Smart Card
//   <o.26> SC0               <0=> Secure <1=> Non-Secure
//   <o.27> SC1               <0=> Secure <1=> Non-Secure
//   <o.28> SC2               <0=> Secure <1=> Non-Secure
//   </h>

//   <o.30> SPI3              <0=> Secure <1=> Non-Secure

//
*/
#define NVIC_INIT_ITNS1_VAL      0x20

/*
    Initialize ITNS 2 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS2    1
/*
//   <o.0>  SDH0              <0=> Secure <1=> Non-Secure



//   <o.4>  I2S0              <0=> Secure <1=> Non-Secure

//
//   <o.7>  CRYPTO                <0=> Secure <1=> Non-Secure
//   <o.8>  GPG               <0=> Secure <1=> Non-Secure
//   <o.9>  EINT6             <0=> Secure <1=> Non-Secure
//   <o.10> UART4             <0=> Secure <1=> Non-Secure
//   <o.11> UART5             <0=> Secure <1=> Non-Secure
//   <o.12> USCI0             <0=> Secure <1=> Non-Secure
//   <o.13> USCI1             <0=> Secure <1=> Non-Secure
//   <o.14> BPWM0             <0=> Secure <1=> Non-Secure
//   <o.15> BPWM1             <0=> Secure <1=> Non-Secure


//   <o.18> I2C2              <0=> Secure <1=> Non-Secure

//   <o.20> QEI0              <0=> Secure <1=> Non-Secure
//   <o.21> QEI1              <0=> Secure <1=> Non-Secure
//   <o.22> ECAP0             <0=> Secure <1=> Non-Secure
//   <o.23> ECAP1             <0=> Secure <1=> Non-Secure
//   <o.24> GPH               <0=> Secure <1=> Non-Secure
//   <o.25> EINT7             <0=> Secure <1=> Non-Secure


//   <o.28> USBH              <0=> Secure <1=> Non-Secure



//
*/
#define NVIC_INIT_ITNS2_VAL      0x0


/*
    Initialize ITNS 3 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS3    1
/*
//   <o.2>  PDMA1             <0=> Secure <1=> Non-Secure
// SCU  Always secure
//
//   <o.5>  TRNG              <0=> Secure <1=> Non-Secure
*/
#define NVIC_INIT_ITNS3_VAL      0x0



/*
// </h>
*/



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


#if defined (SAU_INIT_CTRL) && (SAU_INIT_CTRL == 1U)
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

#if defined (NVIC_INIT_ITNS0) && (NVIC_INIT_ITNS0 == 1U)
    NVIC->ITNS[0] = NVIC_INIT_ITNS0_VAL;
#endif

#if defined (NVIC_INIT_ITNS1) && (NVIC_INIT_ITNS1 == 1U)
    NVIC->ITNS[1] = NVIC_INIT_ITNS1_VAL;
#endif

#if defined (NVIC_INIT_ITNS2) && (NVIC_INIT_ITNS2 == 1U)
    NVIC->ITNS[2] = NVIC_INIT_ITNS2_VAL;
#endif

#if defined (NVIC_INIT_ITNS3) && (NVIC_INIT_ITNS3 == 1U)
    NVIC->ITNS[3] = NVIC_INIT_ITNS3_VAL;
#endif


    /* repeat this for all possible ITNS elements */

}


#endif  /* PARTITION_M2351 */

