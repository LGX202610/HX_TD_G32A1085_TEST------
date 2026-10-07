#ifndef LIN_TP_H
#define LIN_TP_H

#include "LIN_tp_cfg.h"

#ifdef ALLOW_LIN_TP
#include "multi_cyc_fifo.h"

/**
 * @brief   Enumeration for LIN TP work state
 */
typedef enum
{
    LIN_TP_IDLE_STATUS,
    LIN_TP_RX_SF_STATUS,
    LIN_TP_RX_FF_STATUS,
    LIN_TP_RX_CF_STATUS,
    LIN_TP_TX_SF_STATUS,
    LIN_TP_TX_FF_STATUS,
    LIN_TP_TX_CF_STATUS,
    LIN_TP_WAIT_TX_STATUS,
    LIN_TP_WAIT_CONFIRM_STATUS
} LIN_TP_WorkStatusType;


/**
 * @brief   Enumeration for network frame type
 */
typedef enum
{
    SF,
    FF,
    CF
} LIN_TP_FrameType;

/**
 * @brief   Enumeration for result codes
 */
typedef enum
{
    LIN_TP_SUCCESS = 0,
    LIN_TP_TIMEOUT_A,
    LIN_TP_TIMEOUT_BS,
    LIN_TP_TIMEOUT_CR,
    LIN_TP_WRONG_SN,
    LIN_TP_INVALID_FS,
    LIN_TP_UNEXP_PDU,
    LIN_TP_WTF_OVRN,
    LIN_TP_BUFF_OVFLW,
    LIN_TP_ERROR
} LIN_TP_ResultType;


/**
 * @brief   Enumeration for LIN TP transmit message status
 */
typedef enum
{
    LIN_TP_TXMSG_STATUS_IDLE = 0,
    LIN_TP_TXMSG_STATUS_SUCCESS,
    LIN_TP_TXMSG_STATUS_FAIL,
    LIN_TP_TXMSG_STATUS_WAIT
} LIN_TP_TxMsgStatusType;


/**
 * @brief   Structure for LIN TP data information
 */
typedef struct
{
    TP_UdsIdType xLocalId;
    TP_LINDataLengthType xLocalPduLen;
    TP_LINDataLengthType xLocalFFLen;
    uint8 aDataBuf[MAX_CF_DATA_LENGTH];
} LIN_TP_DataInfoType;


/**
 * @brief   Structure for LIN TP info
 */
typedef struct
{
    uint8 seqNum;
    uint8 ucBlkSize;
    TP_TimingType stMinSepTime;
    TP_TimingType stMaxTimeoutTime;
    LIN_TP_DataInfoType stLinData;
} LIN_TP_InfoType;


/**
 * @brief   Structure for LIN TP message
 */
typedef struct
{
    uint8 msgIsFree;
    TP_UdsIdType msgLocalId;
    uint8 msgLength;
    uint8 msgLocalBuf[DATA_LENGTH];
} LIN_TP_MsgType;

/**
 * @brief   Function pointer definition for LIN TP functions
 */
typedef LIN_TP_ResultType (*LIN_TP_ResFunctionType)(LIN_TP_MsgType *, LIN_TP_WorkStatusType *);


/**
 * @brief   Structure to hold work state and associated function
 */
typedef struct
{
    LIN_TP_WorkStatusType workState;
    LIN_TP_ResFunctionType pointForFunc;
} LIN_TP_FunInfoType;

static LIN_TP_InfoType gs_LinInfoTXData;
static TP_TimingType gs_LinTPTXMin = 0u;
static LIN_TP_InfoType gs_LinInfoRXData;
static uint32 gs_LinTXMsgWait = 0u;
static LIN_TP_WorkStatusType gs_LinGlobalWorkStatus = LIN_TP_IDLE_STATUS;
static LIN_TP_TxMsgStatusType gs_LinTXMsgStatus = LIN_TP_TXMSG_STATUS_IDLE;
static TxCompletionCallback gs_LinTXMsgCB = NULL_PTR;

#define LIN_TIME_TO_COUNT(time) ((time) / g_stUdsLINLayerCfg.executionInterval)
#define IS_LIN_SF(xNetWorkFrameType) ((((xNetWorkFrameType) >> 4u) == SF) ? TRUE : FALSE)
#define IS_LIN_FF(xNetWorkFrameType) ((((xNetWorkFrameType) >> 4u) == FF) ? TRUE : FALSE)
#define IS_LIN_CF(xNetWorkFrameType) ((((xNetWorkFrameType) >> 4u) == CF) ? TRUE : FALSE)
#define IS_LIN_FC(xNetWorkFrameType) ((((xNetWorkFrameType)>> 4u) == FC) ? TRUE : FALSE)
#define IS_LIN_SN_RX_VALID(SN) ((gs_LinInfoRXData.seqNum == ((SN) & 0x0Fu)) ? TRUE : FALSE)
#define LIN_ADD_WAIT_SN()\
    do{\
        gs_LinInfoRXData.seqNum++;\
        if(gs_LinInfoRXData.seqNum > 0x0Fu)\
        {\
            gs_LinInfoRXData.seqNum = 0u;\
        }\
    }while(0u)

#define LIN_GET_FRAME_LENGTH(pData, pDataLen)\
    do{\
        if(TRUE == IS_LIN_FF(pData[0u]))\
        {\
            *(pDataLen) = ((uint16)(pData[0u] & 0x0fu) << 8u) | (uint16)pData[1u];\
        }\
        else\
        {\
            *(pDataLen) = (uint8)(pData[0u] & 0x0fu);\
        }\
    }while(0u)

/* Save FF data len */
#define LIN_SAVE_FF_DATA_LENGTH(rxDataLen) (gs_LinInfoRXData.stLinData.xLocalFFLen = rxDataLen)

/* Set wait frame time */
#define LIN_SET_WAIT_RXFRAME_TIME(waitTimeout)\
    do{\
        (gs_LinInfoRXData.stMaxTimeoutTime = LIN_TIME_TO_COUNT(waitTimeout));\
        gs_LinTXMsgWait = gs_LinInfoRXData.stMaxTimeoutTime;\
    }while(0u);

/* RX frame set RX msg wait time */
#define LIN_SET_RXMSG_WAIT_TIME(waitTimeout) LIN_SET_WAIT_RXFRAME_TIME(waitTimeout)

/* Add received data len */
#define LIN_ADD_RXDATA_LENGTH(dataLen) (gs_LinInfoRXData.stLinData.xLocalPduLen += (dataLen))

/* Is received consecutive frame all. */
#define LIN_IS_RX_CFALL(dataLen) (((gs_LinInfoRXData.stLinData.xLocalPduLen + (uint8)(dataLen))\
                                    >= gs_LinInfoRXData.stLinData.xLocalFFLen) ? TRUE : FALSE)

/* Is wait consecutive frame timeout? */
#define LIN_IS_WAITCF_TIMEOUT() ((0u == gs_LinInfoRXData.stMaxTimeoutTime) ? TRUE : FALSE)

/* Is transmitted data len overflow max SF? */
#define LIN_IS_TXDATA_LEN_OFSF() ((gs_LinInfoTXData.stLinData.xLocalFFLen > SF_MAX_DATA_LENGTH) ? TRUE : FALSE)

/* Is transmitted data less than min? */
#define LIN_IS_TXDATA_LEN_LESS() ((0u == gs_LinInfoTXData.stLinData.xLocalFFLen) ? TRUE : FALSE)

/* Set transmitted SF data len */
#define LIN_SET_TXSF_DATA_LENGTH(dataLengthBuff, dataLength)\
    do{\
        *(dataLengthBuff) &= 0xF0u;\
        (*(dataLengthBuff) |= (dataLength));\
    }while(0u)

/* Set transmitted FF data len */
#define LIN_SET_TXFF_DATA_LENGTH(dataLengthBuff, dataLength)\
    do{\
        *(dataLengthBuff + 0u) &= 0xF0u;\
        *(dataLengthBuff + 0u) |= (uint8)((dataLength) >> 8u);\
        *(dataLengthBuff + 1u) |= (uint8)(dataLength);\
    }while(0u)

/* Add TX data len */
#define LIN_ADD_TX_DATA_LENGTH(dataLen) (gs_LinInfoTXData.stLinData.xLocalPduLen += (dataLen))

/* Set TX STmin */
#define LIN_SET_TX_STMIN() (gs_LinInfoTXData.stMinSepTime = LIN_TIME_TO_COUNT(gs_LinTPTXMin))

/* Is TX STmin timeout? */
#define LIN_ISTX_STMIN_TIMEOUT() ((0u == gs_LinInfoTXData.stMinSepTime) ? TRUE : FALSE)

/* Set TX wait frame time */
#define LIN_SET_TX_WAIT_TIME(waitTime)\
    do{\
        (gs_LinInfoTXData.stMaxTimeoutTime = LIN_TIME_TO_COUNT(waitTime));\
        gs_LinTXMsgWait = gs_LinInfoTXData.stMaxTimeoutTime;\
    }while(0u);

/* Is TX wait frame timeout? */
#define LIN_SET_TX_WAIT_TIMEOUT() ((0u == gs_LinInfoTXData.stMaxTimeoutTime) ? TRUE : FALSE)

/* TX frame set TX message wait time */
#define LIN_SET_TXMSG_WAIT_TIME(waitTime) LIN_SET_TX_WAIT_TIME(waitTime)

/* Check timer in waiting status */
#define LIN_ISTXMSG_WAIT_TIMEOUT() ((0u == gs_LinTXMsgWait) ? TRUE : FALSE)

/* Set TX SN */
#define LIN_SET_TX_SN(setTxBuff) (*(setTxBuff) = gs_LinInfoTXData.seqNum | (*(setTxBuff) & 0xF0u))

/* Add TX SN */
#define LIN_ADD_TX_SN()\
    do{\
        gs_LinInfoTXData.seqNum++;\
        if(gs_LinInfoTXData.seqNum > 0x0Fu)\
        {\
            gs_LinInfoTXData.seqNum = 0u;\
        }\
    }while(0u)

/* Is TX all */
#define LIN_ISTX_ALL() ((gs_LinInfoTXData.stLinData.xLocalPduLen >= \
                    gs_LinInfoTXData.stLinData.xLocalFFLen) ? TRUE : FALSE)

/* Save received message ID */
#define LIN_SAVE_RXMSG_ID(msgId) (gs_LinInfoRXData.stLinData.xLocalId = (msgId))

/* Clear LIN TP RX msg buffer */
#define LIN_CLEAR_RX_BUFF(msgBuffer)\
    do{\
        (msgBuffer)->msgIsFree = TRUE;\
        (msgBuffer)->msgLength = 0u;\
        (msgBuffer)->msgLocalId = 0u;\
    }while(0u)

/* Get cur LIN TP status */
#define LIN_GET_CUR_STATUS() (gs_LinGlobalWorkStatus)

/* Set cur LIN TP status */
#define LIN_SET_CUR_STATUS(status) \
    do{\
        gs_LinGlobalWorkStatus = status;\
    }while(0u)

/* Get cur LIN TP status PTR */
#define LIN_GET_CUR_STATUS_PTR() (&gs_LinGlobalWorkStatus)

static LIN_TP_ResultType LIN_TP_DoIdleProcess(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static LIN_TP_ResultType LIN_TP_DoReceiveSingleFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static LIN_TP_ResultType LIN_TP_DoReceiveFirstFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static LIN_TP_ResultType LIN_TP_DoConsecutiveFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static LIN_TP_ResultType LIN_TP_DoTxSingleFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static void LIN_TP_DoTxSingleFrameCB(void);
static void LIN_TP_DoTxFirstFrameCB(void);
static LIN_TP_ResultType LIN_TP_DoTxFirstFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static void LIN_TP_DoTxConsecutiveFrameCB(void);
static LIN_TP_ResultType LIN_TP_DoTxConsecutiveFrame(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static LIN_TP_ResultType LIN_TP_DoWaitTxMessage(LIN_TP_MsgType *pstLocalMsg, LIN_TP_WorkStatusType *penNextState);
static uint8 LIN_TP_SetTransmitFrameType(const LIN_TP_FrameType localFrameType, uint8 *pucOutFrameType);
static uint8 LIN_TP_CopyFrameIntoRxFifo(const TP_UdsIdType udsId, const fifoSizeType dataLen, const uint8 *dataBuf);
static uint8 LIN_TP_CopyFrameDataToFifo(TP_UdsIdType *outTxCanID, uint8 *outTxDataLen, uint8 *outDataBuf);
static void LIN_TP_TxMsgSuccessCB(void);
static void LIN_TP_SetTxMessageState(const LIN_TP_TxMsgStatusType paramTxStatus);
static void LIN_TP_RegTxMessageCB(const TxCompletionCallback pfCallback);
static void LIN_TP_DoRegTxMessageCB(void);

void LIN_TP_MainFunction(void);
void LIN_TP_SysTickController(void);
void LIN_TP_Init(void);

#endif

#endif