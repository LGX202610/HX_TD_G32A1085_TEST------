/*!
 * @file        g32a10xx_int.h
 *
 * @brief       This file contains the headers of the interrupt handlers
 *
 * @version     V1.0.0
 *
 * @date        2026-02-25
 *
 * @attention
 *
 *  Copyright (C) 2025-2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef __G32A10xx_INT_H
#define __G32A10xx_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup ADC_TMRTrigger
  @{
  */

/** @defgroup ADC_TMRTrigger_INT_Macros INT_Macros
  @{
  */

/**@} end of group ADC_TMRTrigger_INT_Macros */

/** @defgroup ADC_TMRTrigger_INT_Enumerations INT_Enumerations
  @{
  */

/**@} end of group ADC_TMRTrigger_INT_Enumerations */

/** @defgroup ADC_TMRTrigger_INT_Structures INT_Structures
  @{
  */

/**@} end of group ADC_TMRTrigger_INT_Structures */

/** @defgroup ADC_TMRTrigger_INT_Variables INT_Variables
  @{
  */

/**@} end of group ADC_TMRTrigger_INT_Variables */

/** @defgroup ADC_TMRTrigger_INT_Functions INT_Functions
  @{
  */
 
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /*__G32A10xx_INT_H */

/**@} end of group ADC_TMRTrigger_INT_Functions */
/**@} end of group ADC_TMRTrigger */
/**@} end of group Examples */
