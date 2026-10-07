/*!
 * @file        lin_app.h
 *
 * @brief       Header for lin module
 *
 * @version     V1.0.0
 *
 * @date        2026-03-20
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

/* Define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

/* Includes */
#include "includes.h"

/** @addtogroup Examples
  @{
*/
typedef struct
{
    uint8_t     ID;         /*!< Specifies the id */
    uint8_t     data[8];    /*!< Specifies the data */
    uint8_t     CheckSum;   /*!< Specifies the check sum */
} Lin_MessageType;
/** @addtogroup USART_Interrupt
  @{
*/
extern boolean g_needTxMsg;

/** @defgroup USART_Interrupt_Functions Functions
  @{
*/
void Lin_Init(void);

uint8_t Lin_CheckPid(uint8_t id);
uint8_t Lin_CheckSum(uint8_t id, uint8_t* data);

void Lin_SendBreak(USART_T* usart);
void Lin_SendSyncSegment(USART_T* usart);
void LIN_SendHead(USART_T* usart, Lin_MessageType data);
void Lin_SendAnswer(USART_T* usart, Lin_MessageType data);

uint8_t LIN_SlaveReceivedProces(void);
uint8_t LIN_MasterReceivedProces(void);

void Lin_TxData(USART_T* usart, uint8_t* data);
void LINBUSAbortTxMsg(void);

/* USART1_LIN_SLAVE receiver interrupt service routine   */
void Lin_SlaveISR(void);
/* USART1_LIN_MASTER receiver interrupt service routine   */
void Lin_MasterISR(void);

/**@} end of group USART_Interrupt_Functions */
/**@} end of group USART_Interrupt */
/**@} end of group Examples */

#endif /*__MAIN_H */
