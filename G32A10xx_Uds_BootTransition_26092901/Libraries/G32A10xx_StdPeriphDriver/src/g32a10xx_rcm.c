/*!
 * @file        g32a10xx_rcm.c
 *
 * @brief       This file provides all the RCM firmware functions
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

#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup RCM_Driver
  @{
*/

/** @defgroup RCM_Functions Functions
  @{
*/

/*!
 * @brief   Resets the clock configuration,HSI as system clock,others clock disabled.
 *
 * @param   None
 *
 * @retval  None
 */
void Rcm_Reset(void)
{
    /** Set HSIEN bit */
    RCM->CTRL1_R.CTRL1_B.HSIEN = BIT_SET;
    RCM->CFG1_R.CFG1 &= (uint32_t)0x04C0F80C;
    RCM->CTRL1_R.CTRL1 &= (uint32_t)0xFEF6FFFFU;
    RCM->CTRL1_R.CTRL1_B.HSEBCFG = BIT_RESET;
    RCM->CFG1_R.CFG1 &= (uint32_t)0xFFC0FFFFU;
    RCM->CFG2_R.CFG2 &= (uint32_t)0xFFFFFFF0U;
    RCM->CFG3_R.CFG3 &= (uint32_t)0xFFFFEFF0U;
    RCM->CTRL2_R.CTRL2_B.HSI14EN = BIT_RESET;

    /* Disable all interrupts and clear pending bits */
    RCM->INT1_R.INT1 = 0x00FF0000;
}

/*!
 * @brief       Configures HSE
 *
 * @param       State:   state of the HSE
 *                       This parameter can be one of the following values:
 *                       @arg RCM_HSE_CLOSE: turn OFF the HSE oscillator
 *                       @arg RCM_HSE_OPEN: turn ON the HSE oscillator
 *                       @arg RCM_HSE_BYPASS: HSE oscillator bypassed with external clock
 *
 * @retval      None
 *
 * @note        HSE can not be stopped if it is used directly or through the PLL as system clock
 */
void Rcm_ConfigHSE(Rcm_HseType State)
{
    RCM->CTRL1_R.CTRL1_B.HSEEN = BIT_RESET;

    RCM->CTRL1_R.CTRL1_B.HSEBCFG = BIT_RESET;

    if (State == RCM_HSE_OPEN)
    {
        RCM->CTRL1_R.CTRL1_B.HSEEN = BIT_SET;
    }
    else if (State == RCM_HSE_BYPASS)
    {
        RCM->CTRL1_R.CTRL1_B.HSEBCFG = BIT_SET;
        RCM->CTRL1_R.CTRL1_B.HSEEN = BIT_SET;
    }
    else if(State == RCM_HSE_CLOSE)
    {
        RCM->CTRL1_R.CTRL1_B.HSEEN = BIT_RESET;
    }
    else 
    {
        /* nothing */
    }
}

/*!
 * @brief       Waits for HSE be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     HSE oscillator is stable and ready to use
 *              ERROR       HSE oscillator not yet ready
 */
uint8_t Rcm_WaitHseReady(void)
{
    __IO uint32_t cnt = 0;
    uint8_t status = ERROR;

    while ((cnt < HSE_STARTUP_TIMEOUT) && (RCM->CTRL1_R.CTRL1_B.HSERDYFLG != (uint32_t)BIT_SET))
    {
        cnt = cnt + 1U;
    }

    if (RCM->CTRL1_R.CTRL1_B.HSERDYFLG == (uint32_t)BIT_SET)
    {
        status = SUCCESS;
    }
    else
    {
        /* nothing */
    }

    return status;
}

/*!
 * @brief       Waits for Pll be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     Pll is stable and ready to use
 *              ERROR       Pll not yet ready
 */
uint8_t Rcm_WaitPllReady(void)
{
    __IO uint32_t cnt = 0;
    uint8_t status = ERROR;

    while ((cnt < PLL_STARTUP_TIMEOUT) && (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG != (uint32_t)BIT_SET))
    {
        cnt = cnt + 1U;
    }

    if (RCM->CTRL1_R.CTRL1_B.PLLRDYFLG == (uint32_t)BIT_SET)
    {
        status = SUCCESS;
    }
    else
    {
        /* nothing */
    }

    return status;
}

/*!
 * @brief       Waits for HSI be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     HSI oscillator is stable and ready to use
 *              ERROR       HSI oscillator not yet ready
 */
uint8_t Rcm_WaitHsiReady(void)
{
    __IO uint32_t cnt = 0;
    uint8_t status = ERROR;

    while ((cnt < HSI_STARTUP_TIMEOUT) && (RCM->CTRL1_R.CTRL1_B.HSIRDYFLG != (uint32_t)BIT_SET))
    {
        cnt = cnt + 1U;
    }

    if (RCM->CTRL1_R.CTRL1_B.HSIRDYFLG == (uint32_t)BIT_SET)
    {
        status = SUCCESS;
    }
    else
    {
        /* nothing */
    }

    return status;
}

/*!
 * @brief       Set HSI trimming value
 *
* @param        HsiTrim:    HSI trimming value
 *                          This parameter must be a number between 0 and 0x1F.
 *
 * @retval      None
 */
void Rcm_SetHsiTrim(uint8_t HsiTrim)
{
    RCM->CTRL1_R.CTRL1_B.HSITRM = HsiTrim;
}

/*!
 * @brief       Enable HSI
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        HSI can not be stopped if it is used directly or through the PLL as system clock
 */
void Rcm_EnableHsi(void)
{
    RCM->CTRL1_R.CTRL1_B.HSIEN = BIT_SET;
}

/*!
 * @brief       Disable HSI
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        HSI can not be stopped if it is used directly or through the PLL as system clock
 */
void Rcm_DisableHsi(void)
{
    RCM->CTRL1_R.CTRL1_B.HSIEN = BIT_RESET;
}

/*!
 * @brief       Set HSI14 trimming value
 *
 * @param        Hsi14Trim:  HSI trimming value
 *                            This parameter must be a number between 0 and 0x1F.
 *
 * @retval      None
 */
void Rcm_SetHsi14Trim(uint8_t Hsi14Trim)
{
    RCM->CTRL2_R.CTRL2_B.HSI14TRM = Hsi14Trim;
}

/*!
 * @brief       Enable HSI14
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Rcm_EnableHsi14(void)
{
    RCM->CTRL2_R.CTRL2_B.HSI14EN = BIT_SET;
}

/*!
 * @brief       Disable HSI14
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Rcm_DisableHsi14(void)
{
    RCM->CTRL2_R.CTRL2_B.HSI14EN = BIT_RESET;
}

/*!
 * @brief       Enable HSI14 ADC
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Rcm_EnableHsi14Adc(void)
{
    RCM->CTRL2_R.CTRL2_B.HSI14TO = BIT_SET;
}

/*!
 * @brief       Disable HSI14 ADC
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Rcm_DisableHsi14Adc(void)
{
    RCM->CTRL2_R.CTRL2_B.HSI14TO = BIT_RESET;
}

/*!
 * @brief       Enable LSI
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        LSI can not be stopped if the IWDT is running
 */
void Rcm_EnableLsi(void)
{
    RCM->CSTS_R.CSTS_B.LSIEN = BIT_SET;
}

/*!
 * @brief       Disable LSI
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        LSI can not be stopped if the IWDT is running
 */
void Rcm_DisableLsi(void)
{
    RCM->CSTS_R.CSTS_B.LSIEN = BIT_RESET;
}

/*!
 * @brief       Configures the PLL clock source and multiplication factor
 *
 * @param       PllSelect:   PLL entry clock source select
 *                           This parameter can be one of the following values:
 *                           @arg RCM_PLL_SEL_HSI_DIV2: HSI clock divided by 2 selected as PLL clock source
 *                           @arg RCM_PLL_SEL_HSE: HSE/CLKDIV1 selected as PLL clock entry
 *                           @arg RCM_PLL_SEL_HSI: HSI clock selected as PLL clock entry, It's only for 072 and 091 devices
 *
 * @param       PllMf:       PLL multiplication factor
 *                           This parameter can be RCM_PLLMF_x where x:[2,16]
 *
 * @retval      None
 *
 * @note
 */
void Rcm_ConfigPll(Rcm_PllSelType PllSelect, Rcm_PllMfType PllMf)
{
    RCM->CFG1_R.CFG1_B.PLLMULCFG = (uint32_t)PllMf;
    RCM->CFG1_R.CFG1_B.PLLSRCSEL = (uint32_t)PllSelect;
}

/*!
 * @brief       Enables PLL
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        The PLL can not be disabled if it is used as system clock
 */
void Rcm_EnablePll(void)
{
    RCM->CTRL1_R.CTRL1_B.PLLEN = BIT_SET;
}

/*!
 * @brief       Disable PLL
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        The PLL can not be disabled if it is used as system clock
 */
void Rcm_DisablePll(void)
{
    RCM->CTRL1_R.CTRL1_B.PLLEN = BIT_RESET;
}


/*!
 * @brief       RCM_ClockSwitch_Cfg_Variables
 *
 * @param       McoDiv
 *
 * @retval      None
 *
 * @note        It is strongly recommended to change the prescaler settings 
 *              only after reset and before enabling the external oscillator and the PLL.
 */
void Rcm_McoConfig(Rcm_McoCfgType McoDiv)
{
    RCM->CFG1_R.CFG1_B.MCOPRE = (uint32_t)McoDiv;
}

/*!
 * @brief       Configures the CLK division factor
 *
 * @param       State:   specifies the PLLDIVCFG clock division factor.
 *                       This parameter can be RCM_CLK_Divx where x:[1,16]
 *
 * @retval      None
 *
 * @note        This function must be used only when the PLL is disabled
 */
void Rcm_ConfigClkDiv(Rcm_ClkDivType State)
{
    RCM->CFG2_R.CFG2_B.PLLDIVCFG = (uint32_t)State;
}

/*!
 * @brief        Enable Clock Security System
 *
 * @param        None
 *
 * @retval       None
 */
void Rcm_EnableCcs(void)
{
    RCM->CTRL1_R.CTRL1_B.CSSEN = BIT_SET;
}

/*!
 * @brief        Disable Clock Security System
 *
 * @param        None
 *
 * @retval       None
 */
void Rcm_DisableCcs(void)
{
    RCM->CTRL1_R.CTRL1_B.CSSEN = BIT_RESET;
}

/*!
 * @brief       Selects clock ouput source
 *
 * @param       CocClock:  specifies the clock source to output
 *                         This parameter can be one of the following values:
 *                         @arg RCM_COC_NO_CLOCK:     No clock selected.
 *                         @arg RCM_COC_HSI14:        HSI14 oscillator clock selected.
 *                         @arg RCM_COC_LSI:          LSI oscillator clock selected.
 *                         @arg RCM_COC_SYSCLK:       System clock selected.
 *                         @arg RCM_COC_HSI:          HSI oscillator clock selected.
 *                         @arg RCM_COC_HSE:          HSE oscillator clock selected.
 *                         @arg RCM_COC_PLLCLK:       PLL clock selected.
 *
 * @retval      None
 */
void Rcm_ConfigCoc(Rcm_CocClkType CocClock)
{
    RCM->CFG1_R.CFG1_B.MCOSEL = (uint32_t)CocClock;
}

/*!
 * @brief       Configures the system clock
 *
 * @param       SysClkSelect:   specifies the clock source used as system clock
 *                              This parameter can be one of the following values:
 *                              @arg RCM_SYSCLK_SEL_HSI:    HSI selected as system clock source
 *                              @arg RCM_SYSCLK_SEL_HSE:    HSE selected as system clock source
 *                              @arg RCM_SYSCLK_SEL_PLL:    PLL selected as system clock source
 *
 * @retval      None
 */
void Rcm_ConfigSysClk(Rcm_SysClkSelType SysClkSelect)
{
    RCM->CFG1_R.CFG1_B.SCLKSEL = (uint32_t)SysClkSelect;
}

/*!
 * @brief       returns the clock source used as system clock
 *
 * @param       None
 *
 * @retval      The clock source used as system clock
 */
uint32_t Rcm_ReadSysClkSource(void)
{
    uint32_t sysClock;

    sysClock = RCM->CFG1_R.CFG1_B.SCLKSELSTS;

    return sysClock;
}

/*!
 * @brief       Configures the AHB clock
 *
 * @param       AhbDiv:   AHB divider number. This clock is derived from the system clock (SYSCLK)
 *                        This parameter can be one of the following values:
 *                        @arg RCM_SYSCLK_DIV_1:   AHB clock = SYSCLK
 *                        @arg RCM_SYSCLK_DIV_2:   AHB clock = SYSCLK/2
 *                        @arg RCM_SYSCLK_DIV_4:   AHB clock = SYSCLK/4
 *                        @arg RCM_SYSCLK_DIV_8:   AHB clock = SYSCLK/8
 *                        @arg RCM_SYSCLK_DIV_16:  AHB clock = SYSCLK/16
 *                        @arg RCM_SYSCLK_DIV_64:  AHB clock = SYSCLK/64
 *                        @arg RCM_SYSCLK_DIV_128: AHB clock = SYSCLK/128
 *                        @arg RCM_SYSCLK_DIV_256: AHB clock = SYSCLK/256
 *                        @arg RCM_SYSCLK_DIV_512: AHB clock = SYSCLK/512
 *
 * @retval      None
 */
void Rcm_ConfigAhb(Rcm_AhbDivType AhbDiv)
{
    RCM->CFG1_R.CFG1_B.AHBPSC = (uint32_t)AhbDiv;
}

/*!
 * @brief       Configures the APB clock
 *
 * @param       ApbDiv:   defines the APB clock divider. This clock is derived from the AHB clock (HCLK)
 *                        This parameter can be one of the following values:
 *                        @arg RCM_HCLK_DIV_1:  APB clock = HCLK
 *                        @arg RCM_HCLK_DIV_2:  APB clock = HCLK/2
 *                        @arg RCM_HCLK_DIV_4:  APB clock = HCLK/4
 *                        @arg RCM_HCLK_DIV_8:  APB clock = HCLK/8
 *                        @arg RCM_HCLK_DIV_16: APB clock = HCLK/16
 *
 * @retval      None
 */
void Rcm_ConfigApb(Rcm_ApbDivType ApbDiv)
{
    RCM->CFG1_R.CFG1_B.APB1PSC = (uint32_t)ApbDiv;
}

/*!
 * @brief       Configures the USART clock (UsartClk)
 *
 * @param       UsartId: USART_1 or USART_2
 * @param       UsartClk: defines the USART clock source. This clock is derived
 *                        from the HSI or System clock.
 *                        This parameter can be one of the following values:
 *                        @arg RCM_USART1CLK_PCLK:   USART1 clock = APB Clock (PCLK)
 *                        @arg RCM_USART1CLK_SYSCLK: USART1 clock = System Clock
 *                        @arg RCM_USART1CLK_HSI:    USART1 clock = HSI Clock
 *
 * @retval      None
 */
void Rcm_ConfigUsartClk(Rcm_UsartSelType UsartId, Rcm_UsartClkType UsartClk)
{
    uint32_t clkVal = ((uint32_t)UsartClk & 0x00000003U);

    switch (UsartId)
    {
        case USART_1:
            RCM->CFG3_R.CFG3_B.USART1SEL = clkVal;
            break;
            
        case USART_2:
            RCM->CFG3_R.CFG3_B.USART2SEL = clkVal;
            break;
            
        default:
            {
                /* nothing */
            }
            break;
    }
}

/*!
 * @brief       Configures the CAN clock (CANCLK)
 *
 * @param       CANCLK: defines the CAN clock source. This clock is derived
 *                        from the HSE or PLL clock.
 *                        This parameter can be one of the following values:
 *                        @arg RCM_CANCLK_HSECLK:   CANCLK = HSECLK
 *                        @arg RCM_CANCLK_PLLCLK:   CANCLK = PLLCLK
 *
 * @retval      None
 */
void Rcm_ConfigCanClk(Rcm_CanClkType CanClk)
{
    RCM->CFG3_R.CFG3_B.CANSEL = ((uint32_t)CanClk & 0x00000001U);
}

/*!
 * @brief       Read frequency of SYSCLK
 *
 * @param       None
 *
 * @retval      Return frequency of SYSCLK
 */
uint32_t Rcm_ReadSysClkFreq(void)
{
    uint32_t sysClock = 0;
    uint32_t pllMull = 0;
    uint32_t plldiv = 0;
    uint32_t pllSource = 0;

    sysClock = RCM->CFG1_R.CFG1_B.SCLKSEL;

    switch (sysClock)
    {
        case (uint32_t)RCM_SYSCLK_SEL_HSI:
            sysClock = HSI_VALUE;
            break;

        case (uint32_t)RCM_SYSCLK_SEL_HSE:
            sysClock = HSE_VALUE;
            break;

        case (uint32_t)RCM_SYSCLK_SEL_PLL:
            pllMull = RCM->CFG1_R.CFG1_B.PLLMULCFG + (uint32_t)2U;
            pllSource = RCM->CFG1_R.CFG1_B.PLLSRCSEL;
            plldiv = RCM->CFG2_R.CFG2_B.PLLDIVCFG + (uint32_t)1U;

            if (pllSource == 0x00U)
            {
                sysClock = (HSI_VALUE >> 1) * pllMull;
            }
            else if (pllSource == 0x01U)
            {
                sysClock = (HSE_VALUE / plldiv) * pllMull;
            }
            else
            {
                /* nothing */
            }
            break;

        default:
            sysClock  = HSI_VALUE;
            break;
    }

    return sysClock;
}

/*!
 * @brief       Read frequency of HCLK(AHB)
 *
 * @param       None
 *
 * @retval      Return frequency of HCLK
 */
uint32_t Rcm_ReadHclkFreq(void)
{
    uint32_t divider = 0;
    uint32_t sysClk = 0;
    uint32_t hclk = 0;
    const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

    sysClk = Rcm_ReadSysClkFreq();
    divider = AHBPrescTable[RCM->CFG1_R.CFG1_B.AHBPSC];
    hclk = sysClk >> divider;

    return hclk;
}

/*!
 * @brief       Read frequency of PCLK
 *
 * @param       None
 *
 * @retval      PCLK1   return frequency of PCLK
 */
uint32_t Rcm_ReadPclkFreq(void)
{
    uint32_t hclk = 0;
    uint32_t pclk = 0;
    uint32_t divider = 0;
    const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

    hclk = Rcm_ReadHclkFreq();

    divider = APBPrescTable[RCM->CFG1_R.CFG1_B.APB1PSC];
    pclk = hclk >> divider;

    return pclk;

}

/*!
 * @brief       Read frequency of ADC CLK
 *
 * @param       None
 *
 * @retval      Return frequency of ADC CLK
 */
uint32_t Rcm_ReadAdcClkFreq(void)
{
    uint32_t adcClk = 0;
    uint32_t pclk = 0;

    pclk = Rcm_ReadPclkFreq();

    if (1U == (ADC->CFG2_R.CFG2_B.CLKCFG))
    {
        adcClk = pclk >> 1;
    }
    else if (2U == (ADC->CFG2_R.CFG2_B.CLKCFG))
    {
        adcClk = pclk >> 2;
    }
    else
    {
        adcClk = HSI14_VALUE;
    }

    return adcClk;
}

/*!
 * @brief       Read frequency of USART1 CLK
 *
 * @param       UsartNum: USART_1 or USART_2
 *
 * @retval      Return frequency of USART1 CLK
 */
uint32_t Rcm_ReadUsartClkFreq(Rcm_UsartSelType UsartNum)
{
    uint32_t usartClkSel = 0;
    uint32_t freq = 0;

    if (UsartNum == USART_1)
    {
        usartClkSel = RCM->CFG3_R.CFG3_B.USART1SEL;
    }
    else
    {
        usartClkSel = RCM->CFG3_R.CFG3_B.USART2SEL;
    }

    switch (usartClkSel)
    {
        case 0x00:
            freq = Rcm_ReadPclkFreq();
            break;
        case 0x01:
            freq = Rcm_ReadSysClkFreq();
            break;
        case 0x03:
            freq = HSI_VALUE;
            break;
        default:
            freq = 0;  
            break;
    }

    return freq;
}

/*!
 * @brief       Configures the RTC clock (RTCCLK)
 *
 * @param       RtcClk:   specifies the RTC clock source
 *                        This parameter can be one of the following values:
 *                        @arg RCM_RTCCLK_LSI:       LSI selected as RTC clock
 *                        @arg RCM_RTCCLK_HSE_DIV_32: HSE divided by 32 selected as RTC clock
 *
 * @retval      None
 *
 * @note        Once the RTC clock is selected it can't be changed unless the Backup domain is reset
 */
void Rcm_ConfigRtcClk(Rcm_RtcClkType RtcClk)
{
    Rcm_EnableBackupReset();
    Rcm_DisableBackupReset();
    RCM->RTCCTRL_R.RTCCTRL_B.RTCSRCSEL = (uint32_t)RtcClk;
}

/*!
 * @brief       Enables the RTC clock
 *
 * @param       None
 *
 * @retval      None
 */
void Rcm_EnableRtcClk(void)
{
    RCM->RTCCTRL_R.RTCCTRL_B.RTCCLKEN = BIT_SET;
}

/*!
 * @brief       Disables the RTC clock
 *
 * @param       None
 *
 * @retval      None
 */
void Rcm_DisableRtcClk(void)
{
    RCM->RTCCTRL_R.RTCCTRL_B.RTCCLKEN = BIT_RESET;
}

/*!
 * @brief       Enable the Backup domain reset
 *
 * @param       None
 *
 * @retval      None
 */
void Rcm_EnableBackupReset(void)
{
    RCM->RTCCTRL_R.RTCCTRL_B.RTCRST = BIT_SET;
}

/*!
 * @brief       Disable the Backup domain reset
 *
 * @param       None
 *
 * @retval      None
 */
void Rcm_DisableBackupReset(void)
{
    RCM->RTCCTRL_R.RTCCTRL_B.RTCRST = BIT_RESET;
}

/*!
 * @brief       Enables AHB peripheral clock
 *
 * @param       AhbPeriph:   specifies the AHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA1:  DMA1 clock
 *                           @arg RCM_AHB_PERIPH_SRAM:  SRAM clock
 *                           @arg RCM_AHB_PERIPH_FMC:   FMC clock
 *                           @arg RCM_AHB_PERIPH_CRC:   CRC clock
 *                           @arg RCM_AHB_PERIPH_TRNG:  TRNG clock
 *                           @arg RCM_AHB_PERIPH_SHA:   SHA clock
 *                           @arg RCM_AHB_PERIPH_AES256: AES256clock
 *                           @arg RCM_AHB_PERIPH_GPIOA: GPIOA clock
 *                           @arg RCM_AHB_PERIPH_GPIOB: GPIOB clock
 *                           @arg RCM_AHB_PERIPH_GPIOC: GPIOC clock
 *                           @arg RCM_AHB_PERIPH_GPIOD: GPIOD clock
 *                           @arg RCM_AHB_PERIPH_GPIOF: GPIOF clock
 *
 * @retval      None
 */
void Rcm_EnableAhbPeriphClock(Rcm_AhbPeriphType AhbPeriph)
{
    RCM->AHBCLKEN_R.AHBCLKEN |= (uint32_t)AhbPeriph;
}

/*!
 * @brief       Disable AHB peripheral clock
 *
 * @param       AhbPeriph:   specifies the AHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA1:  DMA1 clock
 *                           @arg RCM_AHB_PERIPH_SRAM:  SRAM clock
 *                           @arg RCM_AHB_PERIPH_FMC:   FMC clock
 *                           @arg RCM_AHB_PERIPH_CRC:   CRC clock
 *                           @arg RCM_AHB_PERIPH_TRNG:  TRNG clock
 *                           @arg RCM_AHB_PERIPH_SHA:   SHA clock
 *                           @arg RCM_AHB_PERIPH_AES256: AES256clock
 *                           @arg RCM_AHB_PERIPH_GPIOA: GPIOA clock
 *                           @arg RCM_AHB_PERIPH_GPIOB: GPIOB clock
 *                           @arg RCM_AHB_PERIPH_GPIOC: GPIOC clock
 *                           @arg RCM_AHB_PERIPH_GPIOD: GPIOD clock
 *                           @arg RCM_AHB_PERIPH_GPIOF: GPIOF clock
 *
 * @retval      None
 */
void Rcm_DisableAhbPeriphClock(Rcm_AhbPeriphType AhbPeriph)
{
    RCM->AHBCLKEN_R.AHBCLKEN &= ~((uint32_t)AhbPeriph);
}

/*!
 * @brief       Enable the High Speed APB (APB2) peripheral clock
 *
 * @param       Apb2Periph:  specifies the APB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG: SYSCFG clock
 *                           @arg RCM_APB2_PERIPH_ADC1:   ADC1 clock
 *                           @arg RCM_APB2_PERIPH_TMR1:   TMR1 clock
 *                           @arg RCM_APB2_PERIPH_SPI1:   SPI1 clock
 *                           @arg RCM_APB2_PERIPH_USART1: USART1 clock
 *                           @arg RCM_APB2_PERIPH_TMR8:   TMR8 clock
 *                           @arg RCM_APB2_PERIPH_DBGMCU: DBGMCU clock
 *                           @arg RCM_APB2_PERIPH_SMS:    SMS clock
 *
 * @retval      None
 */
void Rcm_EnableApb2PeriphClock(Rcm_Apb2PeriphType Apb2Periph)
{
    RCM->APBCLKEN2_R.APBCLKEN2 |= (uint32_t)Apb2Periph;
}

/*!
 * @brief       Disable the High Speed APB (APB2) peripheral clock
 *
 * @param       Apb2Periph:  specifies the APB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG: SYSCFG clock
 *                           @arg RCM_APB2_PERIPH_ADC1:   ADC1 clock
 *                           @arg RCM_APB2_PERIPH_TMR1:   TMR1 clock
 *                           @arg RCM_APB2_PERIPH_SPI1:   SPI1 clock
 *                           @arg RCM_APB2_PERIPH_USART1: USART1 clock
 *                           @arg RCM_APB2_PERIPH_TMR8:   TMR8 clock
 *                           @arg RCM_APB2_PERIPH_DBGMCU: DBGMCU clock
 *                           @arg RCM_APB2_PERIPH_SMS:    SMS clock
 *
 * @retval      None
 */
void Rcm_DisableApb2PeriphClock(Rcm_Apb2PeriphType Apb2Periph)
{
    RCM->APBCLKEN2_R.APBCLKEN2 &= ~((uint32_t)Apb2Periph);
}

/*!
 * @brief       Enable the Low Speed APB (APB1) peripheral clock
 *
 * @param       Apb1Periph:  specifies the APB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2:   TMR2 clock
 *                           @arg RCM_APB1_PERIPH_TMR3:   TMR3 clock
 *                           @arg RCM_APB1_PERIPH_TMR4:   TMR4 clock
 *                           @arg RCM_APB1_PERIPH_TMR6:   TMR6 clock
 *                           @arg RCM_APB1_PERIPH_TMR7:   TMR7 clock
 *                           @arg RCM_APB1_PERIPH_USART2: USART2 clock
 *                           @arg RCM_APB1_PERIPH_CAN:    CAN clock
 *                           @arg RCM_APB1_PERIPH_PMU:    PMU clock
 *
 * @retval      None
 */
void Rcm_EnableApb1PeriphClock(Rcm_Apb1PeriphType Apb1Periph)
{
    RCM->APBCLKEN1_R.APBCLKEN1 |= (uint32_t)Apb1Periph;
}

/*!
 * @brief       Disable the Low Speed APB (APB1) peripheral clock
 *
 * @param       Apb1Periph:  specifies the APB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2:   TMR2 clock
 *                           @arg RCM_APB1_PERIPH_TMR3:   TMR3 clock
 *                           @arg RCM_APB1_PERIPH_TMR4:   TMR4 clock
 *                           @arg RCM_APB1_PERIPH_TMR6:   TMR6 clock
 *                           @arg RCM_APB1_PERIPH_TMR7:   TMR7 clock
 *                           @arg RCM_APB1_PERIPH_USART2: USART2 clock
 *                           @arg RCM_APB1_PERIPH_CAN:    CAN clock
 *                           @arg RCM_APB1_PERIPH_PMU:    PMU clock
 *
 * @retval      None
 */
void Rcm_DisableApb1PeriphClock(Rcm_Apb1PeriphType Apb1Periph)
{
    RCM->APBCLKEN1_R.APBCLKEN1 &= ~((uint32_t)Apb1Periph);
}

/*!
 * @brief       Enable Low Speed AHB peripheral reset
 *
 * @param       AhbPeriph:   specifies the AHB peripheral to reset
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_TRNG:   TRNG reset
 *                           @arg RCM_AHB_PERIPH_SHA:    SHA reset
 *                           @arg RCM_AHB_PERIPH_AES256: AES256 reset
 *                           @arg RCM_AHB_PERIPH_GPIOA:  GPIOA reset
 *                           @arg RCM_AHB_PERIPH_GPIOB:  GPIOB reset
 *                           @arg RCM_AHB_PERIPH_GPIOC:  GPIOC reset
 *                           @arg RCM_AHB_PERIPH_GPIOD:  GPIOD reset
 *                           @arg RCM_AHB_PERIPH_GPIOF:  GPIOF reset
 *
 * @retval      None
 */
void Rcm_EnableAhbPeriphReset(Rcm_AhbPeriphType AhbPeriph)
{
    RCM->AHBRST_R.AHBRST |= (uint32_t)AhbPeriph;
}

/*!
 * @brief       Disable Low Speed AHB peripheral reset
 *
 * @param       AhbPeriph:   specifies the AHB peripheral to reset
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_TRNG:   TRNG reset
 *                           @arg RCM_AHB_PERIPH_SHA:    SHA reset
 *                           @arg RCM_AHB_PERIPH_AES256: AES256 reset
 *                           @arg RCM_AHB_PERIPH_GPIOA:  GPIOA reset
 *                           @arg RCM_AHB_PERIPH_GPIOB:  GPIOB reset
 *                           @arg RCM_AHB_PERIPH_GPIOC:  GPIOC reset
 *                           @arg RCM_AHB_PERIPH_GPIOD:  GPIOD reset
 *                           @arg RCM_AHB_PERIPH_GPIOF:  GPIOF reset
 *
 * @retval      None
 */
void Rcm_DisableAhbPeriphReset(Rcm_AhbPeriphType AhbPeriph)
{
    RCM->AHBRST_R.AHBRST &= ~((uint32_t)AhbPeriph);
}

/*!
 * @brief       Enable Low Speed APB (APB1) peripheral reset
 *
 * @param       Apb1Periph:  specifies the APB1 peripheral to reset
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2:   TMR2 clock
 *                           @arg RCM_APB1_PERIPH_TMR3:   TMR3 clock
 *                           @arg RCM_APB1_PERIPH_TMR4:   TMR4 clock
 *                           @arg RCM_APB1_PERIPH_TMR6:   TMR6 clock
 *                           @arg RCM_APB1_PERIPH_TMR7:   TMR7 clock
 *                           @arg RCM_APB1_PERIPH_USART2: USART2 clock
 *                           @arg RCM_APB1_PERIPH_CAN:    CAN clock
 *                           @arg RCM_APB1_PERIPH_PMU:    PMU clock
 *
 * @retval      None
 */
void Rcm_EnableApb1PeriphReset(Rcm_Apb1PeriphType Apb1Periph)
{
    RCM->APBRST1_R.APBRST1 |= (uint32_t)Apb1Periph;
}

/*!
 * @brief       Disable Low Speed APB (APB1) peripheral reset
 *
 * @param       Apb1Periph:  specifies the APB1 peripheral to reset
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2:   TMR2 clock
 *                           @arg RCM_APB1_PERIPH_TMR3:   TMR3 clock
 *                           @arg RCM_APB1_PERIPH_TMR4:   TMR4 clock
 *                           @arg RCM_APB1_PERIPH_TMR6:   TMR6 clock
 *                           @arg RCM_APB1_PERIPH_TMR7:   TMR7 clock
 *                           @arg RCM_APB1_PERIPH_USART2: USART2 clock
 *                           @arg RCM_APB1_PERIPH_CAN:    CAN clock
 *                           @arg RCM_APB1_PERIPH_PMU:    PMU clock
 *
 * @retval      None
 */
void Rcm_DisableApb1PeriphReset(Rcm_Apb1PeriphType Apb1Periph)
{
    RCM->APBRST1_R.APBRST1 &= ~((uint32_t)Apb1Periph);
}

/*!
 * @brief       Enable High Speed APB (APB2) peripheral reset
 *
 * @param       Apb2Periph:  specifies the APB2 peripheral to reset
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG: SYSCFG clock
 *                           @arg RCM_APB2_PERIPH_ADC1:   ADC1 clock
 *                           @arg RCM_APB2_PERIPH_TMR1:   TMR1 clock
 *                           @arg RCM_APB2_PERIPH_SPI1:   SPI1 clock
 *                           @arg RCM_APB2_PERIPH_USART1: USART1 clock
 *                           @arg RCM_APB2_PERIPH_TMR8:   TMR8 clock
 *                           @arg RCM_APB2_PERIPH_DBGMCU: DBGMCU clock
 *                           @arg RCM_APB2_PERIPH_SMS:    SMS clock
 *
 * @retval      None
 */
void Rcm_EnableApb2PeriphReset(Rcm_Apb2PeriphType Apb2Periph)
{
    RCM->APBRST2_R.APBRST2 |= (uint32_t)Apb2Periph;
}

/*!
 * @brief       Disable High Speed APB (APB2) peripheral reset
 *
 * @param       Apb2Periph:  specifies the APB2 peripheral to reset
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG: SYSCFG clock
 *                           @arg RCM_APB2_PERIPH_ADC1:   ADC1 clock
 *                           @arg RCM_APB2_PERIPH_TMR1:   TMR1 clock
 *                           @arg RCM_APB2_PERIPH_SPI1:   SPI1 clock
 *                           @arg RCM_APB2_PERIPH_USART1: USART1 clock
 *                           @arg RCM_APB2_PERIPH_TMR8:   TMR8 clock
 *                           @arg RCM_APB2_PERIPH_DBGMCU: DBGMCU clock
 *                           @arg RCM_APB2_PERIPH_SMS:    SMS clock
 *
 * @retval      None
 */
void Rcm_DisableApb2PeriphReset(Rcm_Apb2PeriphType Apb2Periph)
{
    RCM->APBRST2_R.APBRST2 &= ~((uint32_t)Apb2Periph);
}

/*!
 * @brief       Enable the specified RCM interrupts
 *
 * @param       Interrupt:   specifies the RCM interrupt source to check
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_INT_LSIRDY:    LSI ready interrupt
 *                           @arg RCM_INT_HSIRDY:    HSI ready interrupt
 *                           @arg RCM_INT_HSERDY:    HSE ready interrupt
 *                           @arg RCM_INT_PLLRDY:    PLL ready interrupt
 *                           @arg RCM_INT_HSI14RDY:  HSI14 ready interrupt
 *
 * @retval  None
 */
void Rcm_EnableInterrupt(Rcm_IntType Interrupt)
{
    RCM->INT1_R.INT1 |= ((uint32_t)Interrupt << 8);
}

/*!
 * @brief       Disable the specified RCM interrupts
 *
 * @param       Interrupt:   specifies the RCM interrupt source to check
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_INT_LSIRDY:    LSI ready interrupt
 *                           @arg RCM_INT_HSIRDY:    HSI ready interrupt
 *                           @arg RCM_INT_HSERDY:    HSE ready interrupt
 *                           @arg RCM_INT_PLLRDY:    PLL ready interrupt
 *                           @arg RCM_INT_HSI14RDY:  HSI14 ready interrupt
 *
 * @retval  None
 */
void Rcm_DisableInterrupt(Rcm_IntType Interrupt)
{
    RCM->INT1_R.INT1 &= ~((uint32_t)Interrupt << 8);
}

/*!
 * @brief       Read the specified RCM flag status
 *
 * @param       Flag:   specifies the flag to check
 *                      This parameter can be one of the following values:
 *                      @arg RCM_FLAG_HSIRDY:   HSI oscillator clock ready
 *                      @arg RCM_FLAG_HSERDY:   HSE oscillator clock ready
 *                      @arg RCM_FLAG_PLLRDY:   PLL clock ready
 *                      @arg RCM_FLAG_LSIRDY:   LSI oscillator clock ready
 *                      @arg RCM_FLAG_LOCRST:   LOC Reset  
 *                      @arg RCM_FLAG_OBRST:    Option Byte Loader (OBL) reset
 *                      @arg RCM_FLAG_PINRST:   Pin reset
 *                      @arg RCM_FLAG_PWRRST:   POR/PDR reset
 *                      @arg RCM_FLAG_SWRST:    Software reset
 *                      @arg RCM_FLAG_IWDTRST:  Independent Watchdog reset
 *                      @arg RCM_FLAG_LPRRST:   Low Power reset
 *                      @arg RCM_FLAG_HSI14RDY: HSI14 clock ready
 *
 * @retval      The new state of Flag (SET or RESET)
 */
uint16_t Rcm_ReadStatusFlag(Rcm_FlagType Flag)
{
    uint32_t reg = 0;
    uint32_t bit = 0;
    uint16_t status = RESET;
    
    bit = (uint32_t)1U << ((uint32_t)Flag & 0xffU);

    reg = ((uint32_t)Flag >> 8) & 0xffU;

    switch (reg)
    {
        case 0:
            reg = RCM->CTRL1_R.CTRL1;
            break;

        case 1:
            
            break;

        case 2:
            reg = RCM->CSTS_R.CSTS;
            break;

        case 3:
            reg = RCM->CTRL2_R.CTRL2;
            break;

        default:
            {
                /* nothing */
            }
            break;
    }

    if ((reg & bit) != 0U)
    {
        status = SET;
    }
    else
    {
        /* nothing */
    }

    return status;
}

/*!
 * @brief       Clears the RCM reset flags
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        The reset flags are:
 *              RCM_FLAG_LOCRST; RCM_FLAG_OBRST; RCM_FLAG_PINRST; RCM_FLAG_PWRRST;
 *              RCM_FLAG_SWRST; RCM_FLAG_IWDTRST; RCM_FLAG_WWDTRST; RCM_FLAG_LPRRST;
 */
void Rcm_ClearStatusFlag(void)
{
    RCM->CSTS_R.CSTS_B.RSTFLGCLR = BIT_SET;
}

/*!
 * @brief       Read the specified RCM interrupt Flag
 *
 * @param       Flag:   specifies the RCM interrupt source to check
 *                      This parameter can be one of the following values:
 *                      @arg RCM_INT_LSIRDY:    LSI ready interrupt
 *                      @arg RCM_INT_HSIRDY:    HSI ready interrupt
 *                      @arg RCM_INT_HSERDY:    HSE ready interrupt
 *                      @arg RCM_INT_PLLRDY:    PLL ready interrupt
 *                      @arg RCM_INT_HSI14RDY:  HSI14 ready interrupt
 *                      @arg RCC_IT_CSS:        Clock Security System interrupt
 *
 * @retval      The new state of intFlag (SET or RESET)
 */
uint8_t Rcm_ReadIntFlag(Rcm_IntType Flag)
{
    uint8_t ret = (uint8_t)RESET;

    if ((RCM->INT1_R.INT1 & (uint32_t)Flag) != 0U)
    {
        ret = SET;
    }
    else
    {
        ret = RESET;
    }

    return  ret;
}

/*!
 * @brief       Clears the interrupt flag
 *
 * @param       Flag:   specifies the RCM interrupt source to check
 *                      This parameter can be any combination of the following values:
 *                      @arg RCM_INT_LSIRDY:    LSI ready interrupt
 *                      @arg RCM_INT_HSIRDY:    HSI ready interrupt
 *                      @arg RCM_INT_HSERDY:    HSE ready interrupt
 *                      @arg RCM_INT_PLLRDY:    PLL ready interrupt
 *                      @arg RCM_INT_HSI14RDY:  HSI14 ready interrupt
 *                      @arg RCC_IT_CSS:        Clock Security System interrupt
 *
 * @retval      None
 */
void Rcm_ClearIntFlag(Rcm_IntType Flag)
{
    uint32_t temp = 0;

    temp = (uint32_t)Flag << 16;
    RCM->INT1_R.INT1 |= temp;
}

/*!
* @brief       
* 
* @param       
* 
* @return      None
* 
*/
void Rcm_CmuConfigReferenceClock(Rcm_CmuClkDivType cmuRefPre)
{
    RCM->CFG4_R.CFG4_B.REFPRE = (uint32_t)cmuRefPre;
}


/*!
* @brief       
* 
* @param       
* 
* @return      None
* 
*/
void Rcm_CmuMeasureClockSelect(Rcm_CmuClockSelectType selectClock)
{
    if (selectClock == CMU_MEASURE_HSI)
    {
        RCM->CFG4_R.CFG4_B.CLK_SEL = BIT_RESET;
    }
    else
    {
        RCM->CFG4_R.CFG4_B.CLK_SEL = BIT_SET;
    }
}


/*!
* @brief       
* 
* @param       
* 
* @return      None
* 
*/
void Rcm_LOCResetEnable(uint8_t status)
{
    if (status == (uint8_t)DISABLE)
    {
        RCM->INT2_R.INT2_B.SLOCEN = BIT_RESET;
    }
    else
    {
        RCM->INT2_R.INT2_B.SLOCEN = BIT_SET;
    }
}


/*!
* @brief       
* 
* @param       
* 
* @return      None
* 
*/
void Rcm_ReferenceClockCounts(uint8_t count)
{
    RCM->REF_CNT_R.REF_CNT_B.REF_CNT = count;
}


/*!
* @brief       
* 
* @param       
* 
* @return      None
* 
*/
void Rcm_CmuEnable(uint8_t status)
{
    if (status == (uint8_t)DISABLE)
    {
        RCM->CFG4_R.CFG4_B.CMU_EN = BIT_RESET;
    }
    else
    {
        RCM->CFG4_R.CFG4_B.CMU_EN = BIT_SET;
    }
}


/*!
* @brief       
* 
* @param       
* 
* @return      None
* 
*/
void Rcm_LocInterruptEnable(uint8_t status)
{
    if (status == (uint8_t)DISABLE)
    {
        RCM->INT2_R.INT2_B.LOCIE = BIT_RESET;
    }
    else
    {
        RCM->INT2_R.INT2_B.LOCIE = BIT_SET;
    }
}


/*!
* @brief       
* 
* @param       
* 
* @return      None
* 
*/
uint16_t Rcm_CmuGetMeasValue(void)
{
    return (uint16_t)RCM->MEAS_R.MEAS;
}


/**@} end of group RCM_Functions*/
/**@} end of group RCM_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/
