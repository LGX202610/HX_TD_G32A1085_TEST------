/*!
 * @file        g32a10xx_misc.c
 *
 * @brief       This file provides all the miscellaneous firmware functions (add-on to CMSIS functions).
 *
 * @version     V1.0.0
 *
 * @date        2026-02-25
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
#include "g32a10xx_misc.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup MISC_Driver
  @{
*/

/** @defgroup MISC_Functions Functions
  @{
*/

/*!
 * @brief       Enable NVIC request
 *
 * @param       Irq:        The NVIC interrupt request, detailed in IRQn_Type
 *
 * @param       Priority:   Specifies the priority needed to set
 *
 * @retval      None
 */
void Nvic_EnableIrqRequest(IRQn_Type Irq, uint8_t Priority)
{
    NVIC_SetPriority(Irq, Priority);

    NVIC_EnableIRQ(Irq);
}

/*!
 * @brief       Disable NVIC request
 *
 * @param       Irq:    The NVIC interrupt request, detailed in IRQn_Type
 *
 * @retval      None
 */
void Nvic_DisableIrqRequest(IRQn_Type Irq)
{
    NVIC_DisableIRQ(Irq);
}

/**
 * @brief       Enables the system to enter low power mode.
 *
 * @param       LowPowerMode: Specifies the system to enter low power mode.
 *                      This parameter can be one of the following values:
 *                      @arg NVIC_LOWPOER_SEVONPEND:   Low Power SEV on Pend.
 *                      @arg NVIC_LOWPOER_SLEEPDEEP:   Low Power DEEPSLEEP request.
 *                      @arg NVIC_LOWPOER_SLEEPONEXIT: Low Power Sleep on Exit.
 *
 * @retval      None
 */
void Nvic_EnableSystemLowPower(uint8_t LowPowerMode)
{
    SCB->SCR |= LowPowerMode;
}

/**
 * @brief       Disables the system to enter low power mode.
 *
 * @param       LowPowerMode: Specifies the system to enter low power mode.
 *                      This parameter can be one of the following values:
 *                      @arg NVIC_LOWPOER_SEVONPEND:   Low Power SEV on Pend.
 *                      @arg NVIC_LOWPOER_SLEEPDEEP:   Low Power DEEPSLEEP request.
 *                      @arg NVIC_LOWPOER_SLEEPONEXIT: Low Power Sleep on Exit.
 *
 * @retval      None
 */
void Nvic_DisableSystemLowPower(uint8_t LowPowerMode)
{
    SCB->SCR &= (uint32_t)(~(uint32_t)LowPowerMode);
}

/**
 * @brief       Configures the SysTick clock source.
 *
 * @param       SysTickClkSource: specifies the SysTick clock source.
 *                     This parameter can be one of the following values:
 *                     @arg SYSTICK_CLKSOURCE_HCLK_DIV8: AHB clock divided by 8 selected as SysTick clock source.
 *                     @arg SYSTICK_CLKSOURCE_HCLK:      AHB clock selected as SysTick clock source.
 *
 * @retval      None
 */
void SysTick_ConfigClkSource(uint32_t SysTickClkSource)
{
    if (SysTickClkSource == SYSTICK_CLKSOURCE_HCLK)
    {
        SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
    }
    else
    {
        SysTick->CTRL &= SYSTICK_CLKSOURCE_HCLK_DIV8;
    }
}

/*!
 * @brief       Enter Wait Mode
 *
 * @param       None
 *
 * @retval      None
 */
void Pmu_EnterWaitMode(void)
{
    SCB->SCR &= (uint32_t)(~(uint32_t)NVIC_LOWPOER_SLEEPDEEP);
    __WFI();
}

/*!
 * @brief       Enter Stop Mode with WFI instruction
 *
 * @param       None
 *
 * @retval      None
 */
void Pmu_EnterHaltModeWfi(void)
{
    SCB->SCR |= (uint32_t)NVIC_LOWPOER_SLEEPDEEP;
    __DSB();
    __WFI();
}

/*!
 * @brief       Enter Stop Mode with WFE instruction
 *
 * @param       None
 *
 * @retval      None
 */
void Pmu_EnterHaltModeWfe(void)
{
    SCB->SCR |= (uint32_t)NVIC_LOWPOER_SLEEPDEEP;
    __DSB();
    __WFE();
}

/**@} end of group MISC_Functions */
/**@} end of group MISC_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
