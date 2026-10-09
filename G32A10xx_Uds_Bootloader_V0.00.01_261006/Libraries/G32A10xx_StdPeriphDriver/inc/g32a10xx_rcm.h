/*!
 * @file        g32a10xx_rcm.h
 *
 * @brief       This file contains all the functions prototypes for the RCM firmware library
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

#ifndef G32A10xx_RCM_H
#define G32A10xx_RCM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup RCM_Driver
  @{
*/

/** @defgroup RCM_Enumerations Enumerations
  @{
*/

/**
 * @brief   HSE enum
 */
typedef enum
{
    RCM_HSE_CLOSE = 0x00, /*!< turn OFF the HSE oscillator */
    RCM_HSE_OPEN  = 0x01, /*!< turn ON the HSE oscillator */
    RCM_HSE_BYPASS = 0x05 /*!< HSE oscillator bypassed with external clock */
} Rcm_HseType;

/**
 * @brief   System clock select
 */
typedef enum
{
    RCM_PLL_SEL_HSI_DIV2, /*!< HSI clock divided by 2 selected as PLL clock source */
    RCM_PLL_SEL_HSE       /*!< HSE/CLKDIV1 selected as PLL clock entry */
} Rcm_PllSelType;

/**
 * @brief   PLL multiplication factor
 */
typedef enum
{
    RCM_PLLMF_2,  /*!< specifies the PLLMULCFG clock multiple factor as 2 */
    RCM_PLLMF_3,  /*!< specifies the PLLMULCFG clock multiple factor as 3 */
    RCM_PLLMF_4,  /*!< specifies the PLLMULCFG clock multiple factor as 4 */
    RCM_PLLMF_5,  /*!< specifies the PLLMULCFG clock multiple factor as 5 */
    RCM_PLLMF_6,  /*!< specifies the PLLMULCFG clock multiple factor as 6 */
    RCM_PLLMF_7,  /*!< specifies the PLLMULCFG clock multiple factor as 7 */
    RCM_PLLMF_8,  /*!< specifies the PLLMULCFG clock multiple factor as 8 */
    RCM_PLLMF_9,  /*!< specifies the PLLMULCFG clock multiple factor as 9 */
    RCM_PLLMF_10, /*!< specifies the PLLMULCFG clock multiple factor as 10 */
    RCM_PLLMF_11, /*!< specifies the PLLMULCFG clock multiple factor as 11 */
    RCM_PLLMF_12, /*!< specifies the PLLMULCFG clock multiple factor as 12 */
    RCM_PLLMF_13, /*!< specifies the PLLMULCFG clock multiple factor as 13 */
    RCM_PLLMF_14, /*!< specifies the PLLMULCFG clock multiple factor as 14 */
    RCM_PLLMF_15, /*!< specifies the PLLMULCFG clock multiple factor as 15 */
    RCM_PLLMF_16  /*!< specifies the PLLMULCFG clock multiple factor as 16 */
} Rcm_PllMfType;

/**
 * @brief   RCM clock division
 */
typedef enum
{
    RCM_CLK_DIV_1,  /*!< specifies the PLLDIVCFG clock division factor as 1 */
    RCM_CLK_DIV_2,  /*!< specifies the PLLDIVCFG clock division factor as 2 */
    RCM_CLK_DIV_3,  /*!< specifies the PLLDIVCFG clock division factor as 3 */
    RCM_CLK_DIV_4,  /*!< specifies the PLLDIVCFG clock division factor as 4 */
    RCM_CLK_DIV_5,  /*!< specifies the PLLDIVCFG clock division factor as 5 */
    RCM_CLK_DIV_6,  /*!< specifies the PLLDIVCFG clock division factor as 6 */
    RCM_CLK_DIV_7,  /*!< specifies the PLLDIVCFG clock division factor as 7 */
    RCM_CLK_DIV_8,  /*!< specifies the PLLDIVCFG clock division factor as 8 */
    RCM_CLK_DIV_9,  /*!< specifies the PLLDIVCFG clock division factor as 9 */
    RCM_CLK_DIV_10, /*!< specifies the PLLDIVCFG clock division factor as 10 */
    RCM_CLK_DIV_11, /*!< specifies the PLLDIVCFG clock division factor as 11 */
    RCM_CLK_DIV_12, /*!< specifies the PLLDIVCFG clock division factor as 12 */
    RCM_CLK_DIV_13, /*!< specifies the PLLDIVCFG clock division factor as 13 */
    RCM_CLK_DIV_14, /*!< specifies the PLLDIVCFG clock division factor as 14 */
    RCM_CLK_DIV_15, /*!< specifies the PLLDIVCFG clock division factor as 15 */
    RCM_CLK_DIV_16  /*!< specifies the PLLDIVCFG clock division factor as 16 */
} Rcm_ClkDivType;

/**
 * @brief   Clock output control
 */
typedef enum
{
    RCM_COC_NO_CLOCK,     /*!< No clock selected */
    RCM_COC_HSI14,        /*!< HSI14 oscillator clock selected */
    RCM_COC_LSI,          /*!< LSI oscillator clock selected */
    RCM_COC_RESERVED,     /*!< No clock selected */
    RCM_COC_SYSCLK,       /*!< System clock selected */
    RCM_COC_HSI,          /*!< HSI oscillator clock selected */
    RCM_COC_HSE,          /*!< HSE oscillator clock selected */
    RCM_COC_PLLCLK        /*!< PLL clock selected */
} Rcm_CocClkType;

/**
 * @brief   Main Clock Output Prescaler Factor
 */
typedef enum
{
    RCM_MCOPRE_DIV_1,     /*!< MCO clock = SYSCLK */
    RCM_MCOPRE_DIV_2,     /*!< MCO clock = SYSCLK/2 */
    RCM_MCOPRE_DIV_4,     /*!< MCO clock = SYSCLK/4 */
    RCM_MCOPRE_DIV_8,     /*!< MCO clock = SYSCLK/8 */
    RCM_MCOPRE_DIV_16,    /*!< MCO clock = SYSCLK/16 */
    RCM_MCOPRE_DIV_64,    /*!< MCO clock = SYSCLK/64 */
    RCM_MCOPRE_DIV_128    /*!< MCO clock = SYSCLK/128 */
} Rcm_McoCfgType;

/**
 * @brief   System clock select
 */
typedef enum
{
    RCM_SYSCLK_SEL_HSI,     /*!< HSI selected as system clock source */
    RCM_SYSCLK_SEL_HSE,     /*!< HSE selected as system clock source */
    RCM_SYSCLK_SEL_PLL      /*!< PLL selected as system clock source */
} Rcm_SysClkSelType;

/**
 * @brief   AHB divider Number
 */
typedef enum
{
    RCM_SYSCLK_DIV_1 = 7, /*!< AHB clock = SYSCLK */
    RCM_SYSCLK_DIV_2,     /*!< AHB clock = SYSCLK/2 */
    RCM_SYSCLK_DIV_4,     /*!< AHB clock = SYSCLK/4 */
    RCM_SYSCLK_DIV_8,     /*!< AHB clock = SYSCLK/8 */
    RCM_SYSCLK_DIV_16,    /*!< AHB clock = SYSCLK/16 */
    RCM_SYSCLK_DIV_64,    /*!< AHB clock = SYSCLK/64 */
    RCM_SYSCLK_DIV_128,   /*!< AHB clock = SYSCLK/128 */
    RCM_SYSCLK_DIV_256,   /*!< AHB clock = SYSCLK/256 */
    RCM_SYSCLK_DIV_512    /*!< AHB clock = SYSCLK/512 */
} Rcm_AhbDivType;

/**
 * @brief   APB divider Number
 */
typedef enum
{
    RCM_HCLK_DIV_1 = 3,  /*!< APB clock = HCLK */
    RCM_HCLK_DIV_2,      /*!< APB clock = HCLK/2 */
    RCM_HCLK_DIV_4,      /*!< APB clock = HCLK/4 */
    RCM_HCLK_DIV_8,      /*!< APB clock = HCLK/8 */
    RCM_HCLK_DIV_16      /*!< APB clock = HCLK/16 */
} Rcm_ApbDivType;

/**
 * @brief   USART clock source select
 */
typedef enum
{
    RCM_USART1CLK_PCLK    = ((uint32_t)0x00000000),    /*!< USART1 clock = APB Clock (PCLK) */
    RCM_USART1CLK_SYSCLK  = ((uint32_t)0x00000001),    /*!< USART1 clock = System Clock */
    RCM_USART1CLK_HSI     = ((uint32_t)0x00000003)     /*!< USART1 clock = HSI Clock */
} Rcm_UsartClkType;

/**
 * @brief   USART select
 */
typedef enum 
{
    USART_1,
    USART_2
} Rcm_UsartSelType;

/**
 * @brief   CAN clock source select
 */
typedef enum
{
    RCM_CANCLK_HSECLK    = ((uint32_t)0x00000000),    /*!< CANCLK = HSECLK  */
    RCM_CANCLK_PLLCLK    = ((uint32_t)0x00000001)     /*!< CANCLK = PLLCLK */
} Rcm_CanClkType;

/**
 * @brief   RTC clock select
 */
typedef enum
{
    RCM_RTCCLK_LSI = 0x02,        /*!< LSI selected as RTC clock */
    RCM_RTCCLK_HSE_DIV_32  /*!< HSE divided by 32 selected as RTC clock */
} Rcm_RtcClkType;

/**
 * @brief   AHB peripheral
 */
typedef enum
{
    RCM_AHB_PERIPH_DMA1     = BIT0,  /*!< DMA1 peripheral clock */
    RCM_AHB_PERIPH_SRAM     = BIT2,  /*!< SRAM peripheral clock */
    RCM_AHB_PERIPH_FMC      = BIT4,  /*!< FMC peripheral clock */
    RCM_AHB_PERIPH_CRC      = BIT6,  /*!< CRC peripheral clock */
    RCM_AHB_PERIPH_TRNG     = BIT7,  /*!< TRNG peripheral clock */
    RCM_AHB_PERIPH_SHA      = BIT8,  /*!< SHA peripheral clock */
    RCM_AHB_PERIPH_AES256   = BIT9,  /*!< AES256 peripheral clock */
    RCM_AHB_PERIPH_GPIOA    = BIT17, /*!< GPIOA peripheral clock */
    RCM_AHB_PERIPH_GPIOB    = BIT18, /*!< GPIOB peripheral clock */
    RCM_AHB_PERIPH_GPIOC    = BIT19, /*!< GPIOC peripheral clock */
    RCM_AHB_PERIPH_GPIOD    = BIT20, /*!< GPIOD peripheral clock */
    RCM_AHB_PERIPH_GPIOF    = BIT22  /*!< GPIOF peripheral clock */
} Rcm_AhbPeriphType;

/**
 * @brief   AHB2 peripheral
 */
typedef enum
{
    RCM_APB2_PERIPH_SYSCFG  = BIT0,  /*!< SYSCFG peripheral clock */
    RCM_APB2_PERIPH_ADC1    = BIT9,  /*!< ADC1 peripheral clock */
    RCM_APB2_PERIPH_TMR1    = BIT11, /*!< TMR1 peripheral clock */
    RCM_APB2_PERIPH_SPI1    = BIT12, /*!< SPI1 peripheral clock */
    RCM_APB2_PERIPH_USART1  = BIT14, /*!< USART1 peripheral clock */
    RCM_APB2_PERIPH_TMR8    = BIT15, /*!< TMR8 peripheral clock */
    RCM_APB2_PERIPH_DBGMCU  = BIT22, /*!< DBGMCU peripheral clock */
    RCM_APB2_PERIPH_SMS     = BIT23  /*!< SMS peripheral clock */
} Rcm_Apb2PeriphType;

/**
 * @brief   AHB1 peripheral
 */
typedef enum
{
    RCM_APB1_PERIPH_TMR2    = BIT0,  /*!< TMR2 peripheral clock */
    RCM_APB1_PERIPH_TMR3    = BIT1,  /*!< TMR3 peripheral clock */
    RCM_APB1_PERIPH_TMR4    = BIT2,  /*!< TMR4 peripheral clock */
    RCM_APB1_PERIPH_TMR6    = BIT4, /*!< TMR6 peripheral clock */
    RCM_APB1_PERIPH_TMR7    = BIT5, /*!< TMR7 peripheral clock */
    RCM_APB1_PERIPH_USART2  = BIT17,/*!< USART2 peripheral clock */
    RCM_APB1_PERIPH_CAN     = BIT25,/*!< CAN peripheral clock */
    RCM_APB1_PERIPH_PMU     = BIT28 /*!< PMU peripheral clock */
} Rcm_Apb1PeriphType;

/**
 * @brief   RCM Interrupt Source
 */
typedef enum
{
    RCM_INT_LSIRDY      = BIT0,      /*!< LSI ready interrupt */
    RCM_INT_HSIRDY      = BIT2,      /*!< HSI ready interrupt */
    RCM_INT_HSERDY      = BIT3,      /*!< HSE ready interrupt */
    RCM_INT_PLLRDY      = BIT4,      /*!< PLL ready interrupt */
    RCM_INT_HSI14RDY    = BIT5,      /*!< HSI14 ready interrupt */
    RCM_INT_CSS         = BIT7       /*!< Clock security system interrupt */
} Rcm_IntType;

/**
 * @brief   RCM FLAG define
 */
typedef enum
{
    RCM_FLAG_HSIRDY     = 0x001,      /*!< HSI Ready Flag */
    RCM_FLAG_HSERDY     = 0x011,      /*!< HSE Ready Flag */
    RCM_FLAG_PLLRDY     = 0x019,      /*!< PLL Ready Flag */
    RCM_FLAG_LSIRDY     = 0x201,      /*!< LSI Ready Flag */
    RCM_FLAG_LOCRST     = 0x217,      /*!< LOC Reset Flag */
    RCM_FLAG_OBRST      = 0x219,      /*!< Option byte loader reset flag */
    RCM_FLAG_PINRST     = 0x21A,      /*!< PIN reset flag */
    RCM_FLAG_PWRRST     = 0x21B,      /*!< POR/PDR reset flag */
    RCM_FLAG_SWRST      = 0x21C,      /*!< Software reset flag */
    RCM_FLAG_IWDTRST    = 0x21D,      /*!< Independent watchdog reset flag */
    RCM_FLAG_LPRRST     = 0x21F,      /*!< Low-power reset flag */
    RCM_FLAG_HSI14RDY   = 0x301       /*!< HSI14 Ready Flag */
} Rcm_FlagType;

/**
* @brief   CMU clock division
*/
typedef enum
{
    CMU_CLK_DIV_1,   //!< specifies the cmu clock division factor as 1
    CMU_CLK_DIV_2,   //!< specifies the cmu clock division factor as 2
    CMU_CLK_DIV_3,   //!< specifies the cmu clock division factor as 3
    CMU_CLK_DIV_4,   //!< specifies the cmu clock division factor as 4
    CMU_CLK_DIV_5,   //!< specifies the cmu clock division factor as 5
    CMU_CLK_DIV_6,   //!< specifies the cmu clock division factor as 6
    CMU_CLK_DIV_7,   //!< specifies the cmu clock division factor as 7
    CMU_CLK_DIV_8,   //!< specifies the cmu clock division factor as 8
    CMU_CLK_DIV_9,   //!< specifies the cmu clock division factor as 9
    CMU_CLK_DIV_10,  //!< specifies the cmu clock division factor as 10
    CMU_CLK_DIV_11,  //!< specifies the cmu clock division factor as 11
    CMU_CLK_DIV_12,  //!< specifies the cmu clock division factor as 12
    CMU_CLK_DIV_13,  //!< specifies the cmu clock division factor as 13
    CMU_CLK_DIV_14,  //!< specifies the cmu clock division factor as 14
    CMU_CLK_DIV_15,  //!< specifies the cmu clock division factor as 15
    CMU_CLK_DIV_16,  //!< specifies the cmu clock division factor as 16
    CMU_CLK_DIV_17,  //!< specifies the cmu clock division factor as 17
    CMU_CLK_DIV_18,  //!< specifies the cmu clock division factor as 18
    CMU_CLK_DIV_19,  //!< specifies the cmu clock division factor as 19
    CMU_CLK_DIV_20,  //!< specifies the cmu clock division factor as 20
    CMU_CLK_DIV_21,  //!< specifies the cmu clock division factor as 21
    CMU_CLK_DIV_22,  //!< specifies the cmu clock division factor as 22
    CMU_CLK_DIV_23,  //!< specifies the cmu clock division factor as 23
    CMU_CLK_DIV_24,  //!< specifies the cmu clock division factor as 24
    CMU_CLK_DIV_25,  //!< specifies the cmu clock division factor as 25
    CMU_CLK_DIV_26,  //!< specifies the cmu clock division factor as 26
    CMU_CLK_DIV_27,  //!< specifies the cmu clock division factor as 27
    CMU_CLK_DIV_28,  //!< specifies the cmu clock division factor as 28
    CMU_CLK_DIV_29,  //!< specifies the cmu clock division factor as 29
    CMU_CLK_DIV_30,  //!< specifies the cmu clock division factor as 30
    CMU_CLK_DIV_31,  //!< specifies the cmu clock division factor as 31
    CMU_CLK_DIV_32,  //!< specifies the cmu clock division factor as 32
    CMU_CLK_DIV_33,  //!< specifies the cmu clock division factor as 33
    CMU_CLK_DIV_34,  //!< specifies the cmu clock division factor as 34
    CMU_CLK_DIV_35,  //!< specifies the cmu clock division factor as 35
    CMU_CLK_DIV_36,  //!< specifies the cmu clock division factor as 36
    CMU_CLK_DIV_37,  //!< specifies the cmu clock division factor as 37
    CMU_CLK_DIV_38,  //!< specifies the cmu clock division factor as 38
    CMU_CLK_DIV_39,  //!< specifies the cmu clock division factor as 39
    CMU_CLK_DIV_40,  //!< specifies the cmu clock division factor as 40
    CMU_CLK_DIV_41,  //!< specifies the cmu clock division factor as 41
    CMU_CLK_DIV_42,  //!< specifies the cmu clock division factor as 42
    CMU_CLK_DIV_43,  //!< specifies the cmu clock division factor as 43
    CMU_CLK_DIV_44,  //!< specifies the cmu clock division factor as 44
    CMU_CLK_DIV_45,  //!< specifies the cmu clock division factor as 45
    CMU_CLK_DIV_46,  //!< specifies the cmu clock division factor as 46
    CMU_CLK_DIV_47,  //!< specifies the cmu clock division factor as 47
    CMU_CLK_DIV_48,  //!< specifies the cmu clock division factor as 48
    CMU_CLK_DIV_49,  //!< specifies the cmu clock division factor as 49
    CMU_CLK_DIV_50   //!< specifies the cmu clock division factor as 50
} Rcm_CmuClkDivType;

typedef enum
{
    CMU_MEASURE_HSI = 0U,    /* Measure HSI Clock */
    CMU_MEASURE_PLL = 1U     /* Measure PLL Clock */
} Rcm_CmuClockSelectType;

/**@} end of group RCM_Enumerations*/

/** @defgroup RCM_Functions Functions
  @{
*/

/** Function description */

void Rcm_Reset(void);

void Rcm_ConfigHSE(Rcm_HseType State);
uint8_t Rcm_WaitHseReady(void);
uint8_t Rcm_WaitHsiReady(void);
uint8_t Rcm_WaitPllReady(void);
void Rcm_SetHsiTrim(uint8_t HsiTrim);
void Rcm_EnableHsi(void);
void Rcm_DisableHsi(void);

void Rcm_SetHsi14Trim(uint8_t Hsi14Trim);
void Rcm_EnableHsi14(void);
void Rcm_DisableHsi14(void);
void Rcm_EnableHsi14Adc(void);
void Rcm_DisableHsi14Adc(void);

void Rcm_EnableLsi(void);
void Rcm_DisableLsi(void);

void Rcm_ConfigPll(Rcm_PllSelType PllSelect, Rcm_PllMfType PllMf);
void Rcm_EnablePll(void);
void Rcm_DisablePll(void);
void Rcm_McoConfig(Rcm_McoCfgType McoDiv);
    
void Rcm_ConfigClkDiv(Rcm_ClkDivType State);

void Rcm_EnableCcs(void);
void Rcm_DisableCcs(void);

void Rcm_ConfigCoc(Rcm_CocClkType CocClock);

void Rcm_ConfigSysClk(Rcm_SysClkSelType SysClkSelect);
uint32_t Rcm_ReadSysClkSource(void);

void Rcm_ConfigAhb(Rcm_AhbDivType AhbDiv);
void Rcm_ConfigApb(Rcm_ApbDivType ApbDiv);
void Rcm_ConfigUsartClk(Rcm_UsartSelType UsartId, Rcm_UsartClkType UsartClk);
void Rcm_ConfigCanClk(Rcm_CanClkType CanClk);
    
uint32_t Rcm_ReadSysClkFreq(void);
uint32_t Rcm_ReadHclkFreq(void);
uint32_t Rcm_ReadPclkFreq(void);
uint32_t Rcm_ReadAdcClkFreq(void);
uint32_t Rcm_ReadUsartClkFreq(Rcm_UsartSelType UsartNum);

void Rcm_ConfigRtcClk(Rcm_RtcClkType RtcClk);
void Rcm_EnableRtcClk(void);
void Rcm_DisableRtcClk(void);

void Rcm_EnableBackupReset(void);
void Rcm_DisableBackupReset(void);

void Rcm_EnableAhbPeriphClock(Rcm_AhbPeriphType AhbPeriph);
void Rcm_DisableAhbPeriphClock(Rcm_AhbPeriphType AhbPeriph);
void Rcm_EnableApb2PeriphClock(Rcm_Apb2PeriphType Apb2Periph);
void Rcm_DisableApb2PeriphClock(Rcm_Apb2PeriphType Apb2Periph);
void Rcm_EnableApb1PeriphClock(Rcm_Apb1PeriphType Apb1Periph);
void Rcm_DisableApb1PeriphClock(Rcm_Apb1PeriphType Apb1Periph);

void Rcm_EnableAhbPeriphReset(Rcm_AhbPeriphType AhbPeriph);
void Rcm_DisableAhbPeriphReset(Rcm_AhbPeriphType AhbPeriph);
void Rcm_EnableApb1PeriphReset(Rcm_Apb1PeriphType Apb1Periph);
void Rcm_DisableApb1PeriphReset(Rcm_Apb1PeriphType Apb1Periph);
void Rcm_EnableApb2PeriphReset(Rcm_Apb2PeriphType Apb2Periph);
void Rcm_DisableApb2PeriphReset(Rcm_Apb2PeriphType Apb2Periph);

void Rcm_EnableInterrupt(Rcm_IntType Interrupt);
void Rcm_DisableInterrupt(Rcm_IntType Interrupt);
uint16_t Rcm_ReadStatusFlag(Rcm_FlagType Flag);
void Rcm_ClearStatusFlag(void);
uint8_t Rcm_ReadIntFlag(Rcm_IntType Flag);
void Rcm_ClearIntFlag(Rcm_IntType Flag);

void Rcm_CmuConfigReferenceClock(Rcm_CmuClkDivType cmuRefPre);
void Rcm_CmuMeasureClockSelect(Rcm_CmuClockSelectType selectClock);
void Rcm_LOCResetEnable(uint8_t status);
void Rcm_ReferenceClockCounts(uint8_t count);
void Rcm_CmuEnable(uint8_t status);
void Rcm_LocInterruptEnable(uint8_t status);
uint16_t Rcm_CmuGetMeasValue(void);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_RCM_H */

/**@} end of group RCM_Functions*/
/**@} end of group RCM_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/
