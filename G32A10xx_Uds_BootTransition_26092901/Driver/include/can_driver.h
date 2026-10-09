/*
 * @file        can_driver.h
 *
 * @brief       This file provides all the CAN firmware functions
 *
 * @version     V1.0.1
 *
 * @date        2024-03-20
 *
 * @attention
 *
 *  Copyright (C) 2023-2024 Geehy Semiconductor
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
 
#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

#include "includes.h"
#include "can_tp_cfg.h"

#ifdef ALLOW_CAN_TP

#define CAN_RESPONSE_PDU_HANDLE (0xA5)

#define CANCOM1_ID (CanConf_CanController_CAN0)

typedef struct
{
    uint32_t ucRxDataLen;
    uint32_t usRxDataId;
    uint8_t dataBuffer[64u];
} tRxCanMsg;

void InitCAN(void);

uint8_t TransmitCANMsg(const uint32_t i_usCANMsgID,
                       const uint8_t i_ucDataLen,
                       const uint8_t *i_pucDataBuf,
                       const TxCompletionCallback i_pfNetTxCallBack,
                       const uint32_t i_txBlockingMaxtime);

#endif

#endif
