/*
 * @file        can_cfg.c
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

#include "can_config.h"
#include "user_config.h"

#ifdef ALLOW_CAN_TP

const tRxMsgConfig g_astRxMsgConfig[] =
{
    {RECEIVE_FUN, RX_FUN_ADDR_ID_MASK, RX_FUN_ADDR_ID_TYPE},
    {RECEIVE_ADDR, RX_PHY_ADDR_ID_MASK, RX_PHY_ADDR_ID_TYPE}
};

const unsigned char g_ucRxCANMsgIDNum = sizeof(g_astRxMsgConfig) / sizeof(g_astRxMsgConfig[0u]);

tTxMsgConfig g_stTxMsgConfig =
{
    TRANSMIT_RESP,
    TX_RESP_ADDR_ID_TYPE,
    NULL
};

#endif
