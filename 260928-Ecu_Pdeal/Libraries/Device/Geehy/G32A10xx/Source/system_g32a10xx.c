
/*!
 * @file        system_g32a10xx.c
 *
 * @brief       CMSIS Cortex-M0+ Device Peripheral Access Layer System Source File
 *
 * @version     V1.0.3
 *
 * @date        2025-07-03
 *
 * @attention
 *
 *  Copyright (C) 2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (Geehy Semiconductor Software License Agreement).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the Geehy Semiconductor Software License Agreement for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "g32a10xx.h"
#include "g32a10xx_fls.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup ADC_TMRTrigger
  @{
  */

/** @defgroup ADC_TMRTrigger_System_Macros System_Macros
  @{
  */

/* HSE is used as system clock source */
/* HSE (8MHz) used to clock the PLL, and the PLL is used as system clock source */
#define SYSTEM_CLOCK_64MHz  (64000000U)

/* Vector Table location in Internal SRAM or FLASH */
/* Vector Table base offset field. This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET       0x00U

#define FMC_SetLatency_2   FMC_SetWS2

/**@} end of group ADC_TMRTrigger_System_Macros */


/** @defgroup ADC_TMRTrigger_System_Variables System_Variables
  @{
  */

#ifdef SYSTEM_CLOCK_HSE
    uint32_t SystemCoreClock         = SYSTEM_CLOCK_HSE;
#elif defined SYSTEM_CLOCK_24MHz
    uint32_t SystemCoreClock         = SYSTEM_CLOCK_24MHz;
#elif defined SYSTEM_CLOCK_36MHz
    uint32_t SystemCoreClock         = SYSTEM_CLOCK_36MHz;
#elif defined SYSTEM_CLOCK_48MHz
    uint32_t SystemCoreClock         = SYSTEM_CLOCK_48MHz;
#elif defined SYSTEM_CLOCK_64MHz
    uint32_t SystemCoreClock         = SYSTEM_CLOCK_64MHz;
#else
    uint32_t SystemCoreClock         = HSI_VALUE;
#endif

void SystemClockConfig(void);

#ifdef SYSTEM_CLOCK_HSE
    static void SystemClockHSE(void);
#elif defined SYSTEM_CLOCK_24MHz
    static void SystemClock24M(void);
#elif defined SYSTEM_CLOCK_36MHz
    static void SystemClock36M(void);
#elif defined SYSTEM_CLOCK_48MHz
    static void SystemClock48M(void);
#elif defined SYSTEM_CLOCK_64MHz
    static void SystemClock64M(void);

#endif

/**@} end of group ADC_TMRTrigger_System_Variables */

/** @defgroup ADC_TMRTrigger_System_Functions System_Functions
  @{
  */

/*!
 * @brief       Setup the microcontroller system
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SystemInit(void)
{
    /* Set HSIEN bit */
    RCM->CTRL1_R.CTRL1_B.HSIEN = BIT_SET;
    /* Reset SCLKSEL, AHBPSC, APB1PSC, APB2PSC, ADCPSC and COC bits */
    RCM->CFG1_R.CFG1 &= (uint32_t)0x08FFB80CU;
    /* Reset HSEEN, CSSEN and PLLEN bits */
    RCM->CTRL1_R.CTRL1 &= (uint32_t)0xFEF6FFFFU;
    /* Reset HSEBCFG bit */
    RCM->CTRL1_R.CTRL1_B.HSEBCFG = BIT_RESET;
    /* Reset PLLSRCSEL, PLLHSEPSC, PLLMULCFG bits */
    RCM->CFG1_R.CFG1 &= (uint32_t)0xFFC0FFFFU;
    /* Reset PREDIV[3:0] bits */
    RCM->CFG1_R.CFG1 &= (uint32_t)0xFFFFFFF0U;
    /* Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
    RCM->CFG3_R.CFG3 &= (uint32_t)0xFFFFFEACU;
    /* Reset  HSI14 bit */
    RCM->CTRL2_R.CTRL2_B.HSI14EN = BIT_RESET;
    /* Disable all interrupts */
    RCM->INT1_R.INT1 = 0x00000000U;

    SystemClockConfig();
    
    RCM->CTRL1_R.CTRL1_B.CSSEN = BIT_SET;

#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;
#else
    SCB->VTOR = FMC_BASE | VECT_TAB_OFFSET;
#endif
}

void NMI_Handler(void)
{
    uint32_t tempPllDiv = 0u;
    uint32_t tempPllMf = 0u;
    uint32_t tempVal = 0u;
    
    if (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG)
    {
        /* nothing */
    }
    else
    {
        tempPllDiv = RCM->CFG2_R.CFG2_B.PLLDIVCFG;
        tempPllMf = RCM->CFG1_R.CFG1_B.PLLMULCFG;
        tempVal = HSE_VALUE * (tempPllMf + 2)/(tempPllDiv + 1);
        
        if (RCM->INT1_R.INT1_B.CSSFLG)
        {
            if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == (uint8_t)1U)
            {
                RCM->INT1_R.INT1_B.CSSCLR = BIT_SET;
            }
            else
            {
                /* Disable PLL */
                RCM->CTRL1_R.CTRL1_B.PLLEN = BIT_RESET;
                
                RCM->CFG1_R.CFG1_B.PLLSRCSEL = BIT_RESET;
                RCM->CFG1_R.CFG1_B.PLLMULCFG = (tempVal/4000000u) - 2u;
                
                /* Enable PLL */
                RCM->CTRL1_R.CTRL1_B.PLLEN = BIT_SET;
                
                /* Wait until Pll is ready */
                while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == 0U)
                {
                    /* nothing */
                }
                
                RCM->CFG1_R.CFG1_B.SCLKSEL = 2;
                RCM->INT1_R.INT1_B.CSSCLR = BIT_SET;
            }
        }
        else
        {
            /* nothing */
        }
    }
}


/*!
 * @brief       Update SystemCoreClock variable according to Clock Register Values
 *              The SystemCoreClock variable contains the core clock (HCLK)
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SystemCoreClockUpdate(void)
{
    uint32_t sysClock, pllMull, pllSource, Prescaler;
    const uint8_t AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};

    /* Get SYSCLK source */
    sysClock = RCM->CFG1_R.CFG1_B.SCLKSELSTS;

    switch (sysClock)
    {
        case 0:
            SystemCoreClock = HSI_VALUE;
            break;

        /* sys clock is HSE */
        case 1:
            SystemCoreClock = HSE_VALUE;
            break;

        /* sys clock is PLL */
        case 2:
            pllMull = (uint32_t)RCM->CFG1_R.CFG1_B.PLLMULCFG + (uint32_t)2U;
            pllSource = RCM->CFG1_R.CFG1_B.PLLSRCSEL;

            /* PLL entry clock source is HSE */
            if (pllSource == 1U)
            {
                SystemCoreClock = HSE_VALUE * pllMull;

                /* HSE clock divided by 2 */
                if (RCM->CFG1_R.CFG1_B.PLLHSEPSC != 0U)
                {
                    SystemCoreClock >>= 1U;
                }
                else
                {
                    /* do nothing */
                }
            }
            /* PLL entry clock source is HSI/2 */
            else
            {
                SystemCoreClock = (HSI_VALUE >> 1U) * pllMull;
            }

            break;

        default:
            SystemCoreClock  = HSI_VALUE;
            break;
    }

    Prescaler = AHBPrescTable[(RCM->CFG1_R.CFG1_B.AHBPSC)];
    SystemCoreClock >>= Prescaler;
}
/*!
 * @brief       Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SystemClockConfig(void)
{
#ifdef SYSTEM_CLOCK_HSE
    SystemClockHSE();
#elif defined SYSTEM_CLOCK_24MHz
    SystemClock24M();
#elif defined SYSTEM_CLOCK_36MHz
    SystemClock36M();
#elif defined SYSTEM_CLOCK_48MHz
    SystemClock48M();
#elif defined SYSTEM_CLOCK_64MHz
    SystemClock64M();
#endif
}

#if defined SYSTEM_CLOCK_HSE

/*!
 * @brief       Selects HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClockHSE(void)
{
    uint32_t i = 0U;

    RCM->CTRL1_R.CTRL1_B.HSEEN = (uint8_t)BIT_SET;

    for (i = 0U; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
        {
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = BIT_SET;
        /* Flash 0 wait state */
        FMC->CTRL1_R.CTRL1_B.WS = 0U;

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0x00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0x00U;

        /* Select HSE as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 1U;

        /* Wait till HSE is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x01U)
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }
}

#elif defined SYSTEM_CLOCK_24MHz

/*!
 * @brief       Sets System clock frequency to 24MHz and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClock24M(void)
{
    uint32_t i = 0U;

    RCM->CTRL1_R.CTRL1_B.HSEEN = (uint8_t)BIT_SET;

    for (i = 0U; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
        {
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_R.CTRL1_B.WS = 1U;

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0x00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0x00U;

        /* PLL: (HSE / 2) * 6 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 1U;
        RCM->CFG1_R.CFG1_B.PLLHSEPSC = 1U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 4U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
    else
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_R.CTRL1_B.WS = 1U;

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0x00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0x00U;

        /* PLL: (HSI / 2) * 6 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 0U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 4U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
}

#elif defined SYSTEM_CLOCK_36MHz

/*!
 * @brief       Sets System clock frequency to 36MHz and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClock36M(void)
{
    uint32_t i = 0U;

    RCM->CTRL1_R.CTRL1_B.HSEEN = BIT_SET;

    for (i = 0U; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
        {
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = (uint8_t)BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_R.CTRL1_B.WS = 1U;

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0x00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0x00U;

        /* PLL: (HSE / 2) * 9 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 1U;
        RCM->CFG1_R.CFG1_B.PLLHSEPSC = 1U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 7U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
    else
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = (uint8_t)BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_R.CTRL1_B.WS = 1U;

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0x00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0x00U;

        /* PLL: (HSI / 2) * 9 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 0U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 7U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
}

#elif defined SYSTEM_CLOCK_48MHz

/*!
 * @brief       Sets System clock frequency to 46MHz and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClock48M(void)
{
    uint32_t i = 0U;

    RCM->CTRL1_R.CTRL1_B.HSEEN = (uint8_t)BIT_SET;

    for (i = 0U; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
        {
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = (uint8_t)BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_R.CTRL1_B.WS = 1U;

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0x00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0x00U;

        /* PLL: HSE * 6 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 1U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 4U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
    else
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = (uint8_t)BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_R.CTRL1_B.WS = 1U;

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0x00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0x00U;

        /* PLL: HSI/2 * 12 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 0U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 10U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
}

#elif defined SYSTEM_CLOCK_64MHz

/*!
 * @brief       Sets System clock frequency to 64MHz and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClock64M(void)
{
    uint32_t i = 0U;

    RCM->CTRL1_R.CTRL1_B.HSEEN = (uint8_t)BIT_SET;

    for (i = 0U; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
        {
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == 1U)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = (uint8_t)BIT_SET;
        /* Flash 2 wait state */
        FMC_SetLatency_2();

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0X00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0X00U;

        /* PLL: HSE(16MHz) * 4 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 1U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 2U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
    else
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_R.CTRL1_B.PBEN = (uint8_t)BIT_SET;
        /* Flash 2 wait state */
        FMC_SetLatency_2();

        /* HCLK = SYSCLK */
        RCM->CFG1_R.CFG1_B.AHBPSC = 0X00U;

        /* PCLK = HCLK */
        RCM->CFG1_R.CFG1_B.APB1PSC = 0X00U;
        
        /* PLL: HSI(8/2 = 4MHz) * 16 */
        RCM->CFG1_R.CFG1_B.PLLSRCSEL = 0U;
        RCM->CFG1_R.CFG1_B.PLLMULCFG = 14U;

        /* Enable PLL */
        RCM->CTRL1_R.CTRL1_B.PLLEN = 1U;

        /* Wait PLL Ready */
        while (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint8_t)BIT_RESET)
        {
            /* do nothing */
        }

        /* Select PLL as system clock source */
        RCM->CFG1_R.CFG1_B.SCLKSEL = 2U;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_R.CFG1_B.SCLKSELSTS != 0x02U)
        {
            /* do nothing */
        }
    }
}

#endif

/**@} end of group ADC_TMRTrigger_System_Functions */
/**@} end of group ADC_TMRTrigger */
/**@} end of group Examples */
