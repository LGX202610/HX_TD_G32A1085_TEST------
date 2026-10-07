/*!
 * @file        g32a10xx_misc.h
 *
 * @brief       This file contains all the functions prototypes for the miscellaneous
 *              firmware library functions (add-on to CMSIS functions).
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

/* Define to prevent recursive inclusion */
#ifndef G32A10xx_MISC_H
#define G32A10xx_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup MISC_Driver
  @{
*/

/** @defgroup MISC_Macros Macros
  @{
*/

/* MISC SysTick clock source */
#define SYSTICK_CLKSOURCE_HCLK_DIV8 ((uint32_t)0xFFFFFFFBU)
#define SYSTICK_CLKSOURCE_HCLK      ((uint32_t)0x00000004U)

/* Disable all the interrupts */
#define SuspendAllInterrupts()       __asm(" cpsid i")
/* Enable all the interrupts */
#define ResumeAllInterrupts()        __asm(" cpsie i")

/**@} end of group MISC_Macros */

/** @defgroup MISC_Enumerations Enumerations
  @{
*/

/**
 * @brief    System low power mode
 */
typedef enum
{
    NVIC_LOWPOER_SEVONPEND   = 0x10, /*!< Wake up according to pending request */
    NVIC_LOWPOER_SLEEPDEEP   = 0x04, /*!< Enable sleep deep */
    NVIC_LOWPOER_SLEEPONEXIT = 0x02  /*!< Sleep after exit ISR */
} Nvic_LowPowerType;

/**@} end of group MISC_Enumerations */


/** @defgroup MISC_Functions Functions
  @{
*/

/* NVIC */
void Nvic_EnableIrqRequest(IRQn_Type Irq, uint8_t Priority);
void Nvic_DisableIrqRequest(IRQn_Type Irq);

/* Low Power */
void Nvic_EnableSystemLowPower(uint8_t LowPowerMode);
void Nvic_DisableSystemLowPower(uint8_t LowPowerMode);

/* SysTick */
void SysTick_ConfigClkSource(uint32_t SysTickClkSource);

/* PMU */
void Pmu_EnterWaitMode(void);
void Pmu_EnterHaltModeWfi(void);
void Pmu_EnterHaltModeWfe(void);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_MISC_H */

/**@} end of group MISC_Functions */
/**@} end of group MISC_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
