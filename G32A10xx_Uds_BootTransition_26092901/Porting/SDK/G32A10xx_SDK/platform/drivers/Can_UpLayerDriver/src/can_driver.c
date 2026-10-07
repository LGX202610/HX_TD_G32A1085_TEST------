/*
 * @file        can_driver.c
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

#include "user_can_config.h"
#include "can_config.h"
#include "can_driver.h"
#include "user_config.h"
#include "TP.h"
#include "devassert.h"

#ifdef ALLOW_CAN_TP

Can_RxFrameType g_RxFrame = {0U};
Can_HandleType g_CanHandle = {0U};
Can_BufTransInfoType g_TxInfoBuf = {0U};
Can_RxFifoTransInfoType g_RxInfoBuf = {0U};
Can_TxFrameType g_TxFrame = {0U};

static uint8_t IsRxCANMsgId(uint32_t i_usRxMsgId);
static void CheckCANTranmittedStatus(void);

static uint8_t IsRxCANMsgId(uint32_t i_usRxMsgId)
{
    uint8_t Index = 0u;

    while (Index < g_ucRxCANMsgIDNum)
    {
        if (i_usRxMsgId == g_astRxMsgConfig[Index].usRxID)
        {
            return TRUE;
        }

        Index++;
    }

    return FALSE;
}

static void CheckCANTranmittedStatus(void)
{
    if (NULL != g_stTxMsgConfig.pfTxCall)
    {
        g_stTxMsgConfig.pfTxCall();
        g_stTxMsgConfig.pfTxCall = NULL;
    }
}

/*!
* @brief    Callback function called when a CAN frame has been received
*/
static void Can_RxIndication(Can_RxFrameType *FramePtr)
{
    tRxCanMsg stRxCANMsg = {0u};
    uint8_t CANDataIndex = 0u;

    /* 按本帧 IDE 解 ID：标准帧在 M_CAN 中占 bit28:18，扩展帧为 29 位原值 */
    if (CAN_FRAME_STD_ID == FramePtr->Can_RxFrameHead0.xtd)
    {
        stRxCANMsg.usRxDataId = (FramePtr->Can_RxFrameHead0.id >> CAN_FRAME_STD_ID_SHIFT);
    }
    else
    {
        stRxCANMsg.usRxDataId = FramePtr->Can_RxFrameHead0.id;
    }
    
    stRxCANMsg.ucRxDataLen = g_CanGblFdDlcConvDb[FramePtr->Can_RxFrameHead1.dlc];

    if ((0u != stRxCANMsg.ucRxDataLen) &&
            (TRUE == IsRxCANMsgId(stRxCANMsg.usRxDataId)))
    {
        /* read CAN message */
        for (CANDataIndex = 0u; CANDataIndex < stRxCANMsg.ucRxDataLen; CANDataIndex++)
        {
            stRxCANMsg.dataBuffer[CANDataIndex] = FramePtr->data[CANDataIndex];
        }

        if (TRUE != TP_WriteDataInTransport(stRxCANMsg.usRxDataId, stRxCANMsg.ucRxDataLen, stRxCANMsg.dataBuffer))
        {
            /* here is TP driver write data in TP failed, TP will lost CAN message */
            while (1)
            {
            }
        }
    }
}

static void Can_IntCallback(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_TransferStsType Status, 
                            uint32_t Result, uint8_t *InputParaPtr)
{
    switch (Status)
    {
        case CAN_FIFO0_RX_IDLE:
        {                   
            Can_RxIndication(&g_RxFrame);
            g_RxInfoBuf.rxFramePtr = &g_RxFrame;
            Can_ReceiveFifoNonBlocking(EXAMPLE_MCAN, CAN_RX_FIFO0, &g_CanHandle, &g_RxInfoBuf);
        }
        break;

        case CAN_TX_IDLE:
        {
            CheckCANTranmittedStatus();
        }
        break;

        case CAN_FIFO0_RX_LOST:
        break;

        case CAN_FIFO1_RX_IDLE:
        break;
        
        case CAN_FIFO1_RX_LOST:
        break;

        /* Rx dedicate buffer successfully */
        case CAN_RX_IDLE:
        break;

        default:
            break;
    }
}

void InitCAN(void)
{
    Can_FilterConfigType rxFilter;
    Can_StdFilterEleConfigType stdFilter;
    Can_ExtFilterEleConfigType extFilter;
    Can_RxFifoConfigType rxFifo0;
    Can_TxBufConfigType txBuffer;

    Gpio_ConfigType GPIO_InitStructure;
    Rcm_ConfigCanClk(RCM_CANCLK_PLLCLK);
    Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_CAN);
    Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_GPIOA);
    
    Gpio_ConfigPinAF(CANTX_GPIO_PORT, CANTX_PIN_SOURCE, CANTX_AF);
    Gpio_ConfigPinAF(CANRX_GPIO_PORT, CANRX_PIN_SOURCE, CANRX_AF);
    
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.pupd = GPIO_PUPD_PD;
    GPIO_InitStructure.outtype = GPIO_OUT_TYPE_PP;
    GPIO_InitStructure.pin = CANTX_PIN; 
    Gpio_Config(CANTX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.pin = CANRX_PIN; 
    Gpio_Config(CANRX_GPIO_PORT, &GPIO_InitStructure);

    Can_Init(EXAMPLE_MCAN, &g_canConfig);

    /* Create MCAN handle structure and set call back function */
    Can_CreateHandle(EXAMPLE_MCAN, &g_CanHandle, Can_IntCallback, NULL);

   /* Set Message RAM base address and clear to avoid ECC error */
    memset((void *)CAN_SRAM_BASE, 0, MSG_RAM_SIZE);

    /* 标准/扩展未匹配帧一律拒收，避免 GFC 默认进 FIFO0 */
    rxFilter.address  = STD_FILTER_OFS;
    rxFilter.idFormat = CAN_FRAME_STD_ID;
    rxFilter.nmFrame  = CAN_REJECT;
    rxFilter.remFrame = CAN_REJECT_REMOTE_FRAME;
#if defined (USE_CAN_EXT_ID)
    rxFilter.listSize = 0U; /* 扩展帧模式：不收标准 ID */
#else
    rxFilter.listSize = 1U;
#endif
    Can_SetFilterConfig(EXAMPLE_MCAN, &rxFilter);

#if defined (ADAPTE_STD_CAN_ID)
    stdFilter.sfec = CAN_FILTER_STORE_IN_FIFO0;
    stdFilter.sft = CAN_FILTER_RANGE_MODE;
    stdFilter.sfid1 = RECEIVE_ADDR;
    stdFilter.sfid2 = RECEIVE_FUN;
    Can_SetStdFilterEle(&rxFilter, &stdFilter, 0);
    (void)extFilter;
#else
    (void)stdFilter;
#endif

    rxFilter.address  = EXT_FILTER_OFS;
    rxFilter.idFormat = CAN_FRAME_EXT_ID;
    rxFilter.nmFrame  = CAN_REJECT;
    rxFilter.remFrame = CAN_REJECT_REMOTE_FRAME;
#if defined (USE_CAN_EXT_ID)
    rxFilter.listSize = 1U;
#else
    rxFilter.listSize = 0U; /* 标准帧模式：不收扩展 ID */
#endif
    Can_SetFilterConfig(EXAMPLE_MCAN, &rxFilter);

#if defined (USE_CAN_EXT_ID)
    /* 一条 dual 滤波精确匹配物理/功能两个扩展 ID */
    extFilter.efid1 = RECEIVE_ADDR;
    extFilter.efid2 = RECEIVE_FUN;
    extFilter.efec = CAN_FILTER_STORE_IN_FIFO0;
    extFilter.eft = CAN_FILTER_DUAL_MODE;
    Can_SetExtFilterEle(&rxFilter, &extFilter, 0);
#endif

    /* RX fifo0 config. */
    rxFifo0.address       = RX_FIFO0_OFS;
    rxFifo0.elementSize   = 1U;
    rxFifo0.watermark     = 0;
    rxFifo0.opmode        = CAN_RX_FIFO_BLOCK_OPERATION;
    rxFifo0.datafieldSize = CAN_DATA_SIZE_64;
    Can_SetRxFifo0Config(EXAMPLE_MCAN, &rxFifo0);

    /* TX buffer config. */
    memset(&txBuffer, 0, sizeof(txBuffer));
    txBuffer.address       = TX_BUFFER_OFS;
    txBuffer.dedicatedSize = 1U;
    txBuffer.fifoQueCnt    = 0;
    txBuffer.datafieldSize = CAN_DATA_SIZE_64;

    Can_SetTxBufConfig(EXAMPLE_MCAN, &txBuffer);

    /* Disable the Automatic Retransmission */
    //Can_SetAutomaticRetrans(EXAMPLE_MCAN, TRUE);
    
    /* Finish software initialization and enter normal mode, synchronizes to
       CAN bus, ready for communication */
    Can_EnterNorMode(EXAMPLE_MCAN);

    /* the MCAN engine can't auto to get rx payload size, we need set it. */
    g_RxInfoBuf.rxFramePtr = &g_RxFrame;
    Can_ReceiveFifoNonBlocking(EXAMPLE_MCAN, CAN_RX_FIFO0, &g_CanHandle, &g_RxInfoBuf);
}

#if USE_CAN_ERRO == CAN_ERRO_INTERRUPUT

#else
void CANErrorMainFun(void)
{
}
#endif /* USE_CAN_ERRO == CAN_ERRO_INTERRUPUT */

uint8_t TransmitCANMsg(const uint32_t i_usCANMsgID,
                       const uint8_t i_ucDataLen,
                       const uint8_t *i_pucDataBuf,
                       const TxCompletionCallback i_pfNetTxCallBack,
                       const uint32_t i_txBlockingMaxtime)
{
    Can_ApiRetStsType CANTxStatus = CAN_API_FAIL;
    uint8_t ret = 0u;
    uint8_t dlc = 0u;

    DEV_ASSERT(i_pucDataBuf != NULL);

    if (i_usCANMsgID != g_stTxMsgConfig.usTxID)
    {
        return FALSE;
    }

    dlc = Can_ConvertLenByteToDlc(i_ucDataLen);

    g_TxFrame.Can_TxFrameHead0.xtd  = TX_RESP_ADDR_ID_TYPE;
    g_TxFrame.Can_TxFrameHead0.rtr  = CAN_DATA_FRAME;
    g_TxFrame.Can_TxFrameHead1.fdf  = 0U;
    g_TxFrame.Can_TxFrameHead1.brs  = 0U;
    /* 标准帧 ID 放 bit28:18；扩展帧 29 位原值写入（编译期分支，避免 29 位 ID 左移溢出告警） */
#if defined (USE_CAN_EXT_ID)
    g_TxFrame.Can_TxFrameHead0.id = TRANSMIT_RESP;
#else
    g_TxFrame.Can_TxFrameHead0.id = TRANSMIT_RESP << CAN_FRAME_STD_ID_SHIFT;
#endif
    g_TxFrame.data = (uint8_t*)i_pucDataBuf;
    g_TxFrame.dataSize = CAN_DATASIZE;
    g_TxFrame.Can_TxFrameHead1.dlc = dlc;
            
    g_TxInfoBuf.txFramePtr = &g_TxFrame;
    g_TxInfoBuf.bufferIdx = CAN_TX_BUF_ID_0;

    DisableAllInterrupts();
    Can_SendNonBlocking(CAN, &g_CanHandle, &g_TxInfoBuf);
    EnableAllInterrupts();
    g_stTxMsgConfig.pfTxCall = i_pfNetTxCallBack;

    if (CAN_API_SUCCESS == CANTxStatus)
    {
        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }

    return ret;
}

#endif
