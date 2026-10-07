/*
 * @file        can_cfg.h
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

#ifndef CAN_CFG_H_
#define CAN_CFG_H_

#include "includes.h"
#include "user_config.h"

#ifdef ALLOW_CAN_TP

//#define CAN_DRIVER_DEBUG

#ifdef CAN_DRIVER_DEBUG
#include "bootloader_debug.h"
#endif

#define CAN_ERRO_INTERRUPUT     (0u)
#define CAN_ERRO_POLLING        (1u)

#define CAN_WAKE_UP_INTERRUPT   (0u)
#define CAN_WAKE_UP_POLLING     (1u)

#define USE_CAN_ERRO            (CAN_ERRO_POLLING)
#define USE_CAN_WAKE_UP         (CAN_WAKE_UP_POLLING)

/* CAN RX and TX ID type, ID mask configuration */
#if defined (ADAPTE_STD_CAN_ID)
#define RX_FUN_ADDR_ID_TYPE  CAN_FRAME_STD_ID
#define RX_FUN_ADDR_ID_MASK  0xFFFFFFFFu

#define RX_PHY_ADDR_ID_TYPE  CAN_FRAME_STD_ID
#define RX_PHY_ADDR_ID_MASK  0xFFFFFFFFu

#define TX_RESP_ADDR_ID_TYPE CAN_FRAME_STD_ID
#elif defined (USE_CAN_EXT_ID)
#define RX_FUN_ADDR_ID_TYPE  CAN_FRAME_EXT_ID
#define RX_FUN_ADDR_ID_MASK  0xFFFFFFFFu

#define RX_PHY_ADDR_ID_TYPE  CAN_FRAME_EXT_ID
#define RX_PHY_ADDR_ID_MASK  0xFFFFFFFFu

#define TX_RESP_ADDR_ID_TYPE CAN_FRAME_EXT_ID
#endif

/* TX mailbox number configuration */
#define TX_RESP_ADDR_ID_MAILBOX (Can_TX_BUF_ID_0)

#ifdef CAN_DRIVER_DEBUG
#define CANDebugPrintf PrintDebugLog
#else
#define CANDebugPrintf(...)
#endif


typedef void (*tpfTxSuccesfullCallBack)(void);

typedef struct
{
    uint32_t usTxID;
    Can_FrameIdType TxID_Type;
    tpfTxSuccesfullCallBack pfTxCall;
} tTxMsgConfig;

typedef struct
{
    uint32_t usRxID;
    uint32_t usRxMask;
    Can_FrameIdType RxID_Type;
} tRxMsgConfig;

extern const tRxMsgConfig g_astRxMsgConfig[];
extern const uint8_t g_ucRxCANMsgIDNum;
extern tTxMsgConfig g_stTxMsgConfig;

#endif

#endif
