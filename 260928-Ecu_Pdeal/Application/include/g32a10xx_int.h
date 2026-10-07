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
#ifndef G32A10xx_TEMPLATE_INT_H
#define G32A10xx_TEMPLATE_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup Template_Examples
  @{
*/

/** @addtogroup Template
  @{
*/

/** @defgroup Template_INT_Functions INT_Functions
  @{
*/


void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void drv_usart1_rx_irq_handle(void);

#ifdef __cplusplus
}
#endif

#endif /*G32A10xx_TEMPLATE_INT_H */

/**@} end of group Template_INT_Functions*/
/**@} end of group Template*/
/**@} end of group Template_Examples*/
/**@} end of group Examples*/
