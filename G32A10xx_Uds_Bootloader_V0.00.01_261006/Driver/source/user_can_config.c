/*!
 * @file        user_can_config.c
 *
 * @brief       CAN configurations
 *
 * @version     V1.0.0
 *
 * @date        2026-03-25
 *
 * @attention
 *
 *  Copyright (C) 2026 Geehy Semiconductor
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

/* Includes */
#include "user_can_config.h"

/** @addtogroup G32A1085_Examples
  @{
*/

/** @addtogroup CAN_RxFifo
  @{
*/

/** @defgroup CAN_RxFifo_Variables Variables
  @{
*/

/* Configurations for the CAN controller */
Can_ConfigType g_canConfig = {
    .arbBaudrate                        = 500000U,
    .dataBaudrate                       = 2000000U,
    .canfdNorEn                         = ENABLE,
    .canfdBrsEn                         = ENABLE,
    .loopBackInterEn                    = DISABLE,
    .loopBackExtEn                      = DISABLE,
    .busMonEn                           = DISABLE,
    .baudrateConfig.clkPsc              = 1U,
    .baudrateConfig.phaseSeg1           = 0x31U,
    .baudrateConfig.phaseSeg2           = 0x0CU,
    .baudrateConfig.resyncJumpWidth     = 0x0CU,
    .baudrateConfig.dataClkPsc          = 1U,
    .baudrateConfig.dataPhaseSeg1       = 0x0BU,
    .baudrateConfig.dataPhaseSeg2       = 0x02U,
    .baudrateConfig.dataResyncJumpWidth = 0x02U,
};

/**@} end of group CAN_RxFifo_Variables*/
/**@} end of group CAN_RxFifo*/
/**@} end of group Examples*/
