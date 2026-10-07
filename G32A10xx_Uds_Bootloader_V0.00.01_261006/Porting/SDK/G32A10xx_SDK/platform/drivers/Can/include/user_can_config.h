/*!
 * @file        user_can_config.h
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

#ifndef USER_CAN_CONFIG_H
#define USER_CAN_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx_can.h"

/** @addtogroup G32A1085_Examples
  @{
*/
/*******************************************************************************
                                MACROS
*******************************************************************************/
/* The CAN TX PIN and CAN RX PIN */
#define CANTX_PIN                      GPIO_PIN_12
#define CANTX_GPIO_PORT                GPIOA
#define CANTX_GPIO_CLK                 RCM_AHB_PERIPH_GPIOA
#define CANTX_PIN_SOURCE               GPIO_PIN_SOURCE_12
#define CANTX_AF                       GPIO_AF_PIN4

#define CANRX_PIN                      GPIO_PIN_11
#define CANRX_GPIO_PORT                GPIOA
#define CANRX_GPIO_CLK                 RCM_AHB_PERIPH_GPIOA
#define CANRX_PIN_SOURCE               GPIO_PIN_SOURCE_11
#define CANRX_AF                       GPIO_AF_PIN4

/* If not define CANFD_FEATURE_SUPPORTED or define it 0, CAN_DATASIZE should be 8. */
#define CAN_DATASIZE                   (8U)
#define EXAMPLE_MCAN                   CAN

/* The address offset of the STD filers, EXT filters,Rx FIFO 0 and Tx Buffers
 * in the message ram
 */
#define STD_FILTER_OFS                 (0x0U)
#define EXT_FILTER_OFS                 (0x04U)
#define RX_FIFO0_OFS                   (0x10U)
#define TX_BUFFER_OFS                  (0x60U)
/* Message RAM SIZE:2KB */             
#define MSG_RAM_SIZE                   (0x800U)

/*******************************************************************************
                                GLOBAL VARS
*******************************************************************************/

/** @addtogroup CAN_RxFifo
  @{
*/
/**@} end of group CAN_RxFifo*/
/**@} end of group Examples*/

/* CAN user configuration */
extern Can_ConfigType g_canConfig;

#ifdef __cplusplus
}
#endif

#endif /* USER_CAN_CONFIG_H */
