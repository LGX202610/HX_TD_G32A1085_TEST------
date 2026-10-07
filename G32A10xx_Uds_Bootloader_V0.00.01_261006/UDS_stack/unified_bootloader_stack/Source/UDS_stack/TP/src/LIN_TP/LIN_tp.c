#include "LIN_tp.h"

#ifdef ALLOW_LIN_TP
#include "TP_cfg.h"

static const LIN_TP_FunInfoType gs_astLINTpFunInfo[] =
{
    {LIN_TP_IDLE_STATUS, LIN_TP_DoIdleProcess},
    {LIN_TP_RX_SF_STATUS, LIN_TP_DoReceiveSingleFrame},
    {LIN_TP_RX_FF_STATUS, LIN_TP_DoReceiveFirstFrame},
    {LIN_TP_RX_CF_STATUS, LIN_TP_DoConsecutiveFrame},

    {LIN_TP_TX_SF_STATUS, LIN_TP_DoTxSingleFrame},
    {LIN_TP_TX_FF_STATUS, LIN_TP_DoTxFirstFrame},
    {LIN_TP_TX_CF_STATUS, LIN_TP_DoTxConsecutiveFrame},
    {LIN_TP_WAIT_TX_STATUS, LIN_TP_DoWaitTxMessage}
};

/**
 * @brief   Received a Lin TP frame, copy these data in UDS RX FIFO.
 */
static uint8 LIN_TP_CopyFrameIntoRxFifo(const TP_UdsIdType udsId,
                                        const fifoSizeType dataLen,
                                        const uint8 *dataBuf)
{
    errorStateType locErr;
    fifoSizeType locCanWrite = 0u;
    TP_TransportExchangeInfoType stLocExMsg;
    
    ASSERT(NULL_PTR == dataBuf);

    if (0u != dataLen)
    {
        /* Check Lin Write Data Length */
        GainAccessProgramSize(ID_RX_TP_QUEUE, &locCanWrite, &locErr);

        if ((STATE_NO_ERROR == locErr) && (locCanWrite >= (dataLen + sizeof(TP_TransportExchangeInfoType))))
        {
            stLocExMsg.messageID = udsId;
            stLocExMsg.dataLen = dataLen;
            stLocExMsg.pfTxCall = NULL_PTR;
            /* Write data UDS transfer ID and data length */
            ProgramToFifo(ID_RX_TP_QUEUE, (uint8 *)&stLocExMsg, sizeof(TP_TransportExchangeInfoType), &locErr);

            if (STATE_NO_ERROR == locErr)
            {
                /* write exchange info into FIFO */
                ProgramToFifo(ID_RX_TP_QUEUE, (uint8 *)dataBuf, dataLen, &locErr);

                if (STATE_NO_ERROR == locErr)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief   UDS transmitted an application frame data, copy these data in TX FIFO.
 */
static uint8 LIN_TP_CopyFrameDataToFifo(TP_UdsIdType *outTxCanID,
                                        uint8 *outTxDataLen,
                                        uint8 *outDataBuf)
{
    errorStateType locErr;
    fifoSizeType locReadLen = 0u;
    /* local structure to store transport exchange info */
    TP_TransportExchangeInfoType stLocExMsg;
    
    ASSERT(NULL_PTR == outTxCanID);
    ASSERT(NULL_PTR == outTxDataLen);
    ASSERT(NULL_PTR == outDataBuf);
    
    /* LIN read data from buffer */
    GainAccessReadSize(ID_TX_TP_QUEUE, &locReadLen, &locErr);

    if ((STATE_NO_ERROR == locErr) && 
        (0u != locReadLen) && 
        (locReadLen >= sizeof(TP_TransportExchangeInfoType)))
    {
        /* Read receive ID */
        GainInfoDataInFifo(ID_TX_TP_QUEUE,
                         sizeof(TP_TransportExchangeInfoType),
                         (uint8 *)&stLocExMsg,
                         &locReadLen,
                         &locErr);

        if (STATE_NO_ERROR == locErr && 
            sizeof(TP_TransportExchangeInfoType) == locReadLen)
        {
            /* Read data from FIFO */
            GainInfoDataInFifo(ID_TX_TP_QUEUE,
                             stLocExMsg.dataLen,
                             outDataBuf,
                             &locReadLen,
                             &locErr);

            if (STATE_NO_ERROR == locErr && 
                stLocExMsg.dataLen == locReadLen)
            {
                *outTxCanID = stLocExMsg.messageID;
                *outTxDataLen = stLocExMsg.dataLen;
                TP_RegisterFrameTxCallback(stLocExMsg.pfTxCall);
                if ((0u != stLocExMsg.dataLen) && (0x77u == outDataBuf[0u]))
                {
                    TPDebugLog("LIN TP take 77 from TX queue, len:%d\r\n", stLocExMsg.dataLen);
                }
                
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

static LIN_TP_ResultType LIN_TP_DoIdleProcess(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    uint8 localTxLen = (uint8)gs_LinInfoTXData.stLinData.xLocalFFLen;
    
    ASSERT(NULL_PTR == penNextState);
    
    /* Clear LIN TP data */
    Momory_Fill_Function((void *)&gs_LinInfoRXData, 0u, sizeof(LIN_TP_InfoType));
    Momory_Fill_Function((void *)&gs_LinInfoTXData, 0u, sizeof(LIN_TP_InfoType));
    
    /* Clear waiting time */
    gs_LinTXMsgWait = 0u;
    
    /* Set NULL to transmitted message callback */
    TP_RegisterFrameTxCallback(NULL_PTR);

    /* If receive LIN TP message, judge type. Only received SF or FF message. Other frames ignore. */
    switch (pstLocalMsg->msgIsFree)
    {
        case FALSE:
        {
            if (TRUE != IS_LIN_SF(pstLocalMsg->msgLocalBuf[0u]))
            {
                /* nothing */
            }
            else
            {
                *penNextState = LIN_TP_RX_SF_STATUS;
            }

            if (TRUE != IS_LIN_FF(pstLocalMsg->msgLocalBuf[0u]))
            {
                /* nothing */
            }
            else
            {
                *penNextState = LIN_TP_RX_FF_STATUS;
            }
            break;
        }
        case TRUE:
        default:
        {
            /* Judge have message LIN will TX. */
            if (TRUE != LIN_TP_CopyFrameDataToFifo(&gs_LinInfoTXData.stLinData.xLocalId,
                                                   &localTxLen,
                                                   gs_LinInfoTXData.stLinData.aDataBuf))
            {
                /* nothing */
            }
            else
            {
                gs_LinInfoTXData.stLinData.xLocalFFLen = localTxLen;

                if (TRUE != LIN_IS_TXDATA_LEN_OFSF())
                {
                    *penNextState = LIN_TP_TX_SF_STATUS;
                }
                else
                {
                    *penNextState = LIN_TP_TX_FF_STATUS;
                }
            }
            break;
        }
    }
    return LIN_TP_SUCCESS;
}

/**
 * @brief   Do receive single frame.
 */
static LIN_TP_ResultType LIN_TP_DoReceiveSingleFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    uint8 sfLength = 0u;
    ASSERT(NULL_PTR == penNextState);

    if ((0u != pstLocalMsg->msgLength) && (TRUE != pstLocalMsg->msgIsFree))
    {
        if (TRUE == IS_LIN_SF(pstLocalMsg->msgLocalBuf[0u]))
        {
            LIN_GET_FRAME_LENGTH(pstLocalMsg->msgLocalBuf, &sfLength);
            if (sfLength <= SF_MAX_DATA_LENGTH)
            {
                /* Write data to UDS FIFO */
                if (FALSE != LIN_TP_CopyFrameIntoRxFifo(pstLocalMsg->msgLocalId,
                                                          sfLength,
                                                          &pstLocalMsg->msgLocalBuf[1u]))
                {
                    *penNextState = LIN_TP_IDLE_STATUS;
                    return LIN_TP_SUCCESS;
                }
                else
                {
                    TPDebugLog("Copy data error!\n");
                    *penNextState = LIN_TP_IDLE_STATUS;
                    return LIN_TP_ERROR;
                }
            }
            else
            {
                return LIN_TP_ERROR;
            }
        }
        else
        {
            return LIN_TP_ERROR;
        }
    }
    else
    {
        return LIN_TP_ERROR;
    }
}

/**
 * @brief   Do receive first frame
 */
static LIN_TP_ResultType LIN_TP_DoReceiveFirstFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    uint16 localFFLen = 0u;
    ASSERT(NULL_PTR == penNextState);

    if ((0u != pstLocalMsg->msgLength) && (TRUE != pstLocalMsg->msgIsFree))
    {
        if (TRUE == IS_LIN_FF(pstLocalMsg->msgLocalBuf[0u]))
        {
            /* Get FF Data len */
            LIN_GET_FRAME_LENGTH(pstLocalMsg->msgLocalBuf, &localFFLen);

            if (localFFLen >= FF_MIN_DATA_LENGTH)
            {
                /* Save received msg ID */
                LIN_SAVE_RXMSG_ID(pstLocalMsg->msgLocalId);
                /* Write data in global buffer. When receive all data, write these data in FIFO. */
                LIN_SAVE_FF_DATA_LENGTH(localFFLen);
                /* Set wait consecutive frame */
                LIN_SET_RXMSG_WAIT_TIME(g_stUdsLINLayerCfg.receiverConsecutiveLimit);
                /* Copy data in global buffer */
                Momory_Copy_Function(gs_LinInfoRXData.stLinData.aDataBuf, (const void *)&pstLocalMsg->msgLocalBuf[2u], pstLocalMsg->msgLength - 2u);
                LIN_ADD_RXDATA_LENGTH(pstLocalMsg->msgLength - 2u);
                /* Count SN and set STmin, wait timeout time */
                LIN_ADD_WAIT_SN();
                /* Jump to next status */
                *penNextState = LIN_TP_RX_CF_STATUS;
                LIN_CLEAR_RX_BUFF(pstLocalMsg);
                return LIN_TP_SUCCESS;
            }
            else
            {
                TPDebugLog("Received not FF data len less than min.\n");
        #ifdef EN_TP_DEBUG
                TPDebugLog("Received FF data len = %d\n", localFFLen);
        #endif
                return LIN_TP_ERROR;
            }
        }
        else
        {
            TPDebugLog("Received not FF\n");
            return LIN_TP_ERROR;
        }
    }
    else
    {
        return LIN_TP_ERROR;
    }
}


/**
 * @brief   Do receive consecutive frame
 */
static LIN_TP_ResultType LIN_TP_DoConsecutiveFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    ASSERT(NULL_PTR == penNextState);

    /* Is timeout RX wait timeout? If wait timeout receive CF over. */
    if (TRUE != LIN_IS_WAITCF_TIMEOUT())
    {
        if (0u != pstLocalMsg->msgLength && TRUE != pstLocalMsg->msgIsFree)
        {
            /* Check received message is SF or FF? If received SF or FF, start new receive progresses. */
            if ((TRUE != IS_LIN_SF(pstLocalMsg->msgLocalBuf[0u])) && (TRUE != IS_LIN_FF(pstLocalMsg->msgLocalBuf[0u])))
            {
                if (gs_LinInfoRXData.stLinData.xLocalId == pstLocalMsg->msgLocalId)
                {
                    if (TRUE == IS_LIN_CF(pstLocalMsg->msgLocalBuf[0u]))
                    {
                        /* Get received SN. If SN invalid, return FALSE. */
                        if (TRUE == IS_LIN_SN_RX_VALID(pstLocalMsg->msgLocalBuf[0u]))
                        {
                            /* Check receive CF all? If receive all, copy data in FIFO and clear receive
                            buffer information. Else count SN and add receive data len. */
                            switch (LIN_IS_RX_CFALL(pstLocalMsg->msgLength - 1u))
                            {
                                case TRUE:
                                {
                                    /* Copy all data in FIFO and receive over. */
                                    Momory_Copy_Function(&gs_LinInfoRXData.stLinData.aDataBuf[gs_LinInfoRXData.stLinData.xLocalPduLen],
                                               &pstLocalMsg->msgLocalBuf[1u],
                                               gs_LinInfoRXData.stLinData.xLocalFFLen - gs_LinInfoRXData.stLinData.xLocalPduLen);
                                    (void)LIN_TP_CopyFrameIntoRxFifo(gs_LinInfoRXData.stLinData.xLocalId,
                                                                     gs_LinInfoRXData.stLinData.xLocalFFLen,
                                                                     gs_LinInfoRXData.stLinData.aDataBuf);
                                    *penNextState = LIN_TP_IDLE_STATUS;
                                    break;
                                }
                                case FALSE:
                                default:
                                {
                                    /* Count SN and set STmin, wait timeout time */
                                    LIN_ADD_WAIT_SN();
                                    LIN_SET_RXMSG_WAIT_TIME(g_stUdsLINLayerCfg.receiverConsecutiveLimit);
                                    /* Copy data in global FIFO */
                                    Momory_Copy_Function(&gs_LinInfoRXData.stLinData.aDataBuf[gs_LinInfoRXData.stLinData.xLocalPduLen],
                                               &pstLocalMsg->msgLocalBuf[1u],
                                               pstLocalMsg->msgLength - 1u);
                                    LIN_ADD_RXDATA_LENGTH(pstLocalMsg->msgLength - 1u);
                                    break;
                                }
                            }
                            return LIN_TP_SUCCESS;
                        }
                        else
                        {
                            TPDebugLog("Msg SN invalid in CF!\n");
                            return LIN_TP_WRONG_SN;
                        }
                    }
                    else
                    {
                #ifdef EN_TP_DEBUG
                        TPDebugLog("Msg type invalid in CF %X!\n", pstLocalMsg->msgLocalBuf[0u]);
                #endif
                        return LIN_TP_ERROR;
                    }
                }
                else
                {
            #ifdef EN_TP_DEBUG
                    TPDebugLog("Msg ID invalid in CF! F RX ID = %X, RX ID = %X\n",
                                  gs_LinInfoRXData.stLinData.xLocalId, pstLocalMsg->msgLocalId);
            #endif
                    return LIN_TP_ERROR;
                }
            }
            else
            {
                *penNextState = LIN_TP_IDLE_STATUS;
                return LIN_TP_UNEXP_PDU;
            }
        }
        else
        {
            /* It's normally return LIN_TP_SUCCESS when waiting received LIN message. */
            return LIN_TP_SUCCESS;
        }
    }
    else
    {
        TPDebugLog("Wait consecutive frame timeout!\n");
        *penNextState = LIN_TP_IDLE_STATUS;
        return LIN_TP_TIMEOUT_CR;
    }
}

/**
 * @brief   Transmit SF callback
 */
static void LIN_TP_DoTxSingleFrameCB(void)
{
    TP_TransmitSingleFrameCallback(E_TX_MSG_OK);
    LIN_SET_CUR_STATUS(LIN_TP_IDLE_STATUS);
}

/**
 * @brief   Transmit single frame
 */
static LIN_TP_ResultType LIN_TP_DoTxSingleFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    uint8 localBuf[DATA_LENGTH] = {0u};
    uint8 localLen = 0u;
    ASSERT(NULL_PTR == penNextState);

    /* invert condition check for SF overflow. */
    if (TRUE != LIN_IS_TXDATA_LEN_OFSF())
    {
        if (TRUE != LIN_IS_TXDATA_LEN_LESS())
        {
            /* set frame type to single frame */
            (void)LIN_TP_SetTransmitFrameType(SF, &localBuf[0u]);
            /* set transmitted data length */
            LIN_SET_TXSF_DATA_LENGTH(&localBuf[0u], gs_LinInfoTXData.stLinData.xLocalFFLen);
            localLen = localBuf[0u] + 1u;
            /* copy actual data to TX buffer */
            Momory_Copy_Function(&localBuf[1u],
                       gs_LinInfoTXData.stLinData.aDataBuf,
                       gs_LinInfoTXData.stLinData.xLocalFFLen);
            /* set TX message status and register callback */
            LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_WAIT);
            LIN_TP_RegTxMessageCB(LIN_TP_DoTxSingleFrameCB);

            /* request to transmit the application message. */
            if (TRUE == g_stUdsLINLayerCfg.transmitMessage(gs_LinInfoTXData.stLinData.xLocalId,
                                                             localLen,
                                                             localBuf,
                                                             LIN_TP_TxMsgSuccessCB,
                                                             g_stUdsLINLayerCfg.maxTransmitBlockTimeMs))
            {
                /* set the max wait time for TX frame. */
                LIN_SET_TXMSG_WAIT_TIME(g_stUdsLINLayerCfg.senderTimeoutValue);
                /* move to waiting for TX state. */
                *penNextState = LIN_TP_WAIT_TX_STATUS;
                return LIN_TP_SUCCESS;
            }
            else
            {
                /* if fail, set fail status and clear callback */
                LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_FAIL);
                LIN_TP_RegTxMessageCB(NULL_PTR);
                *penNextState = LIN_TP_IDLE_STATUS;
                return LIN_TP_ERROR;
            }
        }
        else
        {
            *penNextState = LIN_TP_IDLE_STATUS;
            return LIN_TP_ERROR;
        }
    }
    else
    {
        *penNextState = LIN_TP_TX_FF_STATUS;
        return LIN_TP_ERROR;
    }
}


/**
 * @brief   Transmit FF callback
 */
static void LIN_TP_DoTxFirstFrameCB(void)
{
    /* Add TX data len */
    LIN_ADD_TX_DATA_LENGTH(FF_MIN_DATA_LENGTH - 2);
    /* Set TX wait time */
    LIN_SET_TXMSG_WAIT_TIME(g_stUdsLINLayerCfg.senderConsecutiveLimit);
    LIN_ADD_TX_SN();
    LIN_SET_CUR_STATUS(LIN_TP_TX_CF_STATUS);
}


/**
 * @brief   Transmit first frame
 */
static LIN_TP_ResultType LIN_TP_DoTxFirstFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    uint8 localBuf[DATA_LENGTH] = {0u};
    
    ASSERT(NULL_PTR == penNextState);

    switch (LIN_IS_TXDATA_LEN_OFSF())
    {
        case TRUE:
        {
            /* set frame type to first frame */
            (void)LIN_TP_SetTransmitFrameType(FF, &localBuf[0u]);

            /* set the transmitted data length */
            LIN_SET_TXFF_DATA_LENGTH(localBuf, gs_LinInfoTXData.stLinData.xLocalFFLen);

            /* copy the first frame data to TX buffer */
            Momory_Copy_Function(&localBuf[2u],
                       gs_LinInfoTXData.stLinData.aDataBuf,
                       FF_MIN_DATA_LENGTH - 2);

            /* set TX message status and register callback */
            LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_WAIT);
            LIN_TP_RegTxMessageCB(LIN_TP_DoTxFirstFrameCB);

            /* request to transmit the application message. */
            if (TRUE == g_stUdsLINLayerCfg.transmitMessage(gs_LinInfoTXData.stLinData.xLocalId,
                                                           sizeof(localBuf),
                                                           localBuf,
                                                           LIN_TP_TxMsgSuccessCB,
                                                           g_stUdsLINLayerCfg.maxTransmitBlockTimeMs))
            {
                /* set the max wait time for TX frame. */
                LIN_SET_TXMSG_WAIT_TIME(g_stUdsLINLayerCfg.senderTimeoutValue);

                /* jump to waiting TX status. */
                *penNextState = LIN_TP_WAIT_TX_STATUS;
                return LIN_TP_SUCCESS;
            }
            else
            {
                /* if fail, set fail status and clear callback */
                LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_FAIL);
                LIN_TP_RegTxMessageCB(NULL_PTR);
                *penNextState = LIN_TP_IDLE_STATUS;
                return LIN_TP_ERROR;
            }
            break;
        }
        case FALSE:
        default:
        {
            *penNextState = LIN_TP_TX_SF_STATUS;
            return LIN_TP_BUFF_OVFLW;
        }
    }
}


/**
 * @brief   Transmit Consecutive Frame callback
 */
static void LIN_TP_DoTxConsecutiveFrameCB(void)
{
    if (TRUE != LIN_ISTX_ALL())
    {
        LIN_SET_TX_STMIN();
        LIN_SET_TXMSG_WAIT_TIME(g_stUdsLINLayerCfg.senderConsecutiveLimit);
        LIN_ADD_TX_SN();
        LIN_SET_CUR_STATUS(LIN_TP_TX_CF_STATUS);
    }
    else
    {
        TP_TransmitSingleFrameCallback(E_TX_MSG_OK);
        LIN_SET_CUR_STATUS(LIN_TP_IDLE_STATUS);
        return;
    }
}


/**
 * @brief   Transmit consecutive frame
 */
static LIN_TP_ResultType LIN_TP_DoTxConsecutiveFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    uint8 localBuf[DATA_LENGTH] = {0u};
    uint8 localLen = 0u;
    uint8 localALLLen = 0u;
    
    ASSERT(NULL_PTR == penNextState);

    /* invert condition to check STmin timeout */
    if (FALSE != LIN_ISTX_STMIN_TIMEOUT())
    {
        /* invert condition to check wait frame timeout */
        if (TRUE != LIN_SET_TX_WAIT_TIMEOUT())
        {
            (void)LIN_TP_SetTransmitFrameType(CF, &localBuf[0u]);
            LIN_SET_TX_SN(&localBuf[0u]);
            /* calculate how many bytes remain to send */
            localLen = gs_LinInfoTXData.stLinData.xLocalFFLen - gs_LinInfoTXData.stLinData.xLocalPduLen;
            /* set TX message status and register callback */
            LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_WAIT);
            LIN_TP_RegTxMessageCB(LIN_TP_DoTxConsecutiveFrameCB);
            LIN_SET_TXMSG_WAIT_TIME(g_stUdsLINLayerCfg.senderTimeoutValue);

            if (localLen >= (CF_MAX_DATA_LENGTH))
            {
                /* copy maximum CF chunk to buffer */
                Momory_Copy_Function(&localBuf[1u],
                           &gs_LinInfoTXData.stLinData.aDataBuf[gs_LinInfoTXData.stLinData.xLocalPduLen],
                           (CF_MAX_DATA_LENGTH));

                /* request transmit of this max CF. */
                if (TRUE == g_stUdsLINLayerCfg.transmitMessage(gs_LinInfoTXData.stLinData.xLocalId,
                                                                 sizeof(localBuf),
                                                                 localBuf,
                                                                 LIN_TP_TxMsgSuccessCB,
                                                                 g_stUdsLINLayerCfg.maxTransmitBlockTimeMs))
                {
                    /* nothing */
                }
                else
                {
                    /* if fail, set fail status and nullify callback */
                    LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_FAIL);
                    LIN_TP_RegTxMessageCB(NULL_PTR);
                    *penNextState = LIN_TP_IDLE_STATUS;
                    return LIN_TP_ERROR;
                }
                LIN_ADD_TX_DATA_LENGTH((CF_MAX_DATA_LENGTH));
            }
            else
            {
                /* copy the smaller chunk to buffer */
                Momory_Copy_Function(&localBuf[1u],
                           &gs_LinInfoTXData.stLinData.aDataBuf[gs_LinInfoTXData.stLinData.xLocalPduLen],
                           localLen);
                /* calculate total length for this CF */
                localALLLen = localLen + 1u;

                /* request transmit of partial CF */
                if (TRUE == g_stUdsLINLayerCfg.transmitMessage(gs_LinInfoTXData.stLinData.xLocalId,
                                                                 localALLLen,
                                                                 localBuf,
                                                                 LIN_TP_TxMsgSuccessCB,
                                                                 g_stUdsLINLayerCfg.maxTransmitBlockTimeMs))
                {
                    /* nothing */
                }
                else
                {
                    /* if fail, set fail status and nullify callback */
                    LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_FAIL);
                    LIN_TP_RegTxMessageCB(NULL_PTR);
                    *penNextState = LIN_TP_IDLE_STATUS;
                    return LIN_TP_ERROR;
                }
                LIN_ADD_TX_DATA_LENGTH(localLen);
            }
            *penNextState = LIN_TP_WAIT_TX_STATUS;
            return LIN_TP_SUCCESS;
        }
        else
        {
            *penNextState = LIN_TP_IDLE_STATUS;
            return LIN_TP_TIMEOUT_BS;
        }
    }
    else
    {
        return LIN_TP_SUCCESS;
    }

}


/**
 * @brief   Waiting TX message
 */
static LIN_TP_ResultType LIN_TP_DoWaitTxMessage(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState)
{
    /* invert condition check for waiting timeout */
    if (TRUE != LIN_ISTXMSG_WAIT_TIMEOUT())
    {
        /* nothing */
    }
    else
    {
        /* if timeout occurred, cancel transmission if possible */
        if (NULL_PTR == g_stUdsLINLayerCfg.cancelTransmission)
        {
            /* nothing */
        }
        else
        {
            (g_stUdsLINLayerCfg.cancelTransmission)();
        }

        /* tell upper layer TX message timeout */
        TP_TransmitSingleFrameCallback(E_TX_MSG_TIMEOUT);
        /* set TX message status to fail and unregister callback */
        LIN_TP_SetTxMessageState(LIN_TP_TXMSG_STATUS_FAIL);
        LIN_TP_RegTxMessageCB(NULL_PTR);
        *penNextState = LIN_TP_IDLE_STATUS;
    }

    /* return normal status */
    return LIN_TP_SUCCESS;
}


/**
 * @brief   Set transmit frame type
 */
static uint8 LIN_TP_SetTransmitFrameType(const LIN_TP_FrameType localFrameType,
                                         uint8 *pucOutFrameType)
{
    /* ensure pointer is valid */
    ASSERT(NULL_PTR == pucOutFrameType);

    /* invert condition to check frame type */
    if ((SF != localFrameType) && (FF != localFrameType) && (CF != localFrameType))
    {
        return FALSE;
    }
    else
    {
        /* if valid type, set upper nibble */
        *pucOutFrameType &= 0x0Fu;
        *pucOutFrameType |= ((uint8)localFrameType << 4u);
        return TRUE;
    }
}

/**
 * @brief   LIN TP TX message callback
 */
static void LIN_TP_TxMsgSuccessCB(void)
{
    gs_LinTXMsgStatus = LIN_TP_TXMSG_STATUS_SUCCESS;
}

/**
 * @brief   LIN TP set TX message status
 */
static void LIN_TP_SetTxMessageState(const LIN_TP_TxMsgStatusType paramTxStatus)
{
    gs_LinTXMsgStatus = paramTxStatus;
}

/**
 * @brief   Register TX message successful callback
 */
static void LIN_TP_RegTxMessageCB(const TxCompletionCallback pfCallback)
{
    gs_LinTXMsgCB = pfCallback;
}

/**
 * @brief   Do register TX message callback
 */
static void LIN_TP_DoRegTxMessageCB(void)
{
    LIN_TP_TxMsgStatusType localStatus = LIN_TP_TXMSG_STATUS_IDLE;
    /* get the TX message status with interrupts disabled to protect variable updates */
    DisableAllInterrupts();
    localStatus = gs_LinTXMsgStatus;
    EnableAllInterrupts();

    switch (localStatus)
    {
        case LIN_TP_TXMSG_STATUS_SUCCESS:
        {
            if (NULL_PTR == gs_LinTXMsgCB)
            {
                /* nothing */
            }
            else
            {
                (gs_LinTXMsgCB)();
                gs_LinTXMsgCB = NULL_PTR;
            }
            gs_LinTXMsgStatus = LIN_TP_TXMSG_STATUS_IDLE;
            break;
        }
        case LIN_TP_TXMSG_STATUS_FAIL:
        {
            TPDebugLog("\n TX msg failed status=%d\n", gs_LinTXMsgStatus);
            gs_LinTXMsgStatus = LIN_TP_TXMSG_STATUS_IDLE;
            gs_LinTXMsgCB = NULL_PTR;
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * @brief   Initialize LIN transport process
 */
void LIN_TP_Init(void)
{
    /* local variable for error code */
    errorStateType eLocErr;
    
    /* apply FIFO for RX queue */
    OperateFifoLogic(LEN_RX_TP_QUEUE, ID_RX_TP_QUEUE, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* nothing if no error */
    }
    else
    {
        TPDebugLog("Apply FIFO RX error!\n");
        for (;;)
        {
            /* infinite loop */
        }
    }

    /* apply FIFO for TX queue */
    OperateFifoLogic(LEN_TX_TP_QUEUE, ID_TX_TP_QUEUE, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* nothing if no error */
    }
    else
    {
#ifdef EN_TP_DEBUG
        TPDebugLog("Apply FIFO TX error code!\n");
#endif
        for (;;)
        {
            /* infinite loop */
        }
    }

    /* apply FIFO for RX BUS */
    OperateFifoLogic(RECEIVE_BUS_FIFO_SIZE, RECEIVE_BUS_FIFO_CHAR, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* nothing if no error */
    }
    else
    {
        TPDebugLog("Apply RX FIFO from BUS error code!\n");
        for (;;)
        {
            /* infinite loop */
        }
    }

#ifdef ALLOW_LIN_TP
    /* apply FIFO for TX BUS if LIN TP is enabled */
    OperateFifoLogic(TRANSMIT_BUS_FIFO_SIZE, TRANSMIT_BUS_FIFO_CHAR, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* nothing if no error */
    }
    else
    {
        TPDebugLog("Apply TX FIFO TO BUS error code!\n");
        for (;;)
        {
            /* infinite loop */
        }
    }
#endif
}

/**
 * @brief   LIN TP system tick control, called periodically by system
 */
void LIN_TP_SysTickController(void)
{
    if (0 == gs_LinInfoRXData.stMinSepTime)
    {
        /* nothing */
    }
    else
    {
        gs_LinInfoRXData.stMinSepTime--;
    }

    if (0 == gs_LinInfoRXData.stMaxTimeoutTime)
    {
        /* nothing */
    }
    else
    {
        gs_LinInfoRXData.stMaxTimeoutTime--;
    }

    if (0 == gs_LinInfoTXData.stMinSepTime)
    {
        /* nothing */
    }
    else
    {
        gs_LinInfoTXData.stMinSepTime--;
    }

    if (0 == gs_LinInfoTXData.stMaxTimeoutTime)
    {
        /* nothing */
    }
    else
    {
        gs_LinInfoTXData.stMaxTimeoutTime--;
    }

    if (0 == gs_LinTXMsgWait)
    {
        /* nothing */
    }
    else
    {
        gs_LinTXMsgWait--;
    }
}

/**
 * @brief   UDS network man function
 */
void LIN_TP_MainFunction(void)
{
    /* local variable for loop index */
    uint8 ucIdx = 0u;
    /* local constant for find count */
    const uint8 ucFindCount = sizeof(gs_astLINTpFunInfo) / sizeof(gs_astLINTpFunInfo[0u]);
    /* local struct for LIN TP message */
    LIN_TP_MsgType stLocalRxMsg = {TRUE, 0u, 0u, {0u}};
    /* local variable for function result */
    LIN_TP_ResultType eLocResult = LIN_TP_SUCCESS;
    errorStateType eTxQueueErr = STATE_NO_ERROR;
    fifoSizeType xTxQueueReadLen = 0u;
    static LIN_TP_WorkStatusType eLastBlockedStatus = LIN_TP_IDLE_STATUS;

    /* Handle TX completion first so the next request is not delayed by WAIT_TX status. */
    LIN_TP_DoRegTxMessageCB();

    GainAccessReadSize(ID_TX_TP_QUEUE, &xTxQueueReadLen, &eTxQueueErr);
    if ((STATE_NO_ERROR == eTxQueueErr) &&
        (0u != xTxQueueReadLen) &&
        (LIN_TP_IDLE_STATUS != LIN_GET_CUR_STATUS()) &&
        (eLastBlockedStatus != LIN_GET_CUR_STATUS()))
    {
        TPDebugLog("LIN TP TX pending but busy, status:%d, wait:%d, txStatus:%d, q:%d\r\n",
                   LIN_GET_CUR_STATUS(),
                   gs_LinTXMsgWait,
                   gs_LinTXMsgStatus,
                   xTxQueueReadLen);
        eLastBlockedStatus = LIN_GET_CUR_STATUS();
    }
    else if (LIN_TP_IDLE_STATUS == LIN_GET_CUR_STATUS())
    {
        eLastBlockedStatus = LIN_TP_IDLE_STATUS;
    }

    /* if we are in waiting TX state */
    if (LIN_TP_WAIT_TX_STATUS == LIN_GET_CUR_STATUS())
    {
        /* nothing */
    }
    else
    {
        if (TRUE != g_stUdsLINLayerCfg.receiveMessage(&stLocalRxMsg.msgLocalId,
                                                       &stLocalRxMsg.msgLength,
                                                       stLocalRxMsg.msgLocalBuf))
        {
            /* nothing */
        }
        else
        {
            /* check incoming ID */
            if (TRUE != LIN_TP_ValidateIncomingIdentifier(stLocalRxMsg.msgLocalId))
            {
                /* invalid message ID */
                TPDebugLog("Received invalid message ID\n");
            }
            else
            {
                /* mark buffer as occupied */
                stLocalRxMsg.msgIsFree = FALSE;
            }
        }
    }
    for (; ucIdx < ucFindCount; )
    {
        if (LIN_GET_CUR_STATUS() != gs_astLINTpFunInfo[ucIdx].workState)
        {
            /* nothing */
        }
        else
        {
            if (NULL_PTR == gs_astLINTpFunInfo[ucIdx].pointForFunc)
            {
                /* nothing */
            }
            else
            {
                /* call the function and get result */
                eLocResult = gs_astLINTpFunInfo[ucIdx].pointForFunc(&stLocalRxMsg, LIN_GET_CUR_STATUS_PTR());
            }
        }
        
        if (LIN_TP_UNEXP_PDU == eLocResult)
        {
            ucIdx = 0u;
        }
        else
        {
            if (LIN_TP_SUCCESS == eLocResult)
            {
                /* nothing */
            }
            else
            {
                /* set status to IDLE if not OK */
                LIN_SET_CUR_STATUS(LIN_TP_IDLE_STATUS);
            }
            /* increment loop index */
            ucIdx++;
        }
    }

    /* clear local Rx buffer */
    LIN_CLEAR_RX_BUFF(&stLocalRxMsg);
}

#endif

