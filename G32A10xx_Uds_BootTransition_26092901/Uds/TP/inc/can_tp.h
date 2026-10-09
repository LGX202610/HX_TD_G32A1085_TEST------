/*******************************************************************************
* Project Name      : CAN/LIN Protocol Stack  // 项目名称：CAN/LIN协议栈
* Platform          : Arm                     // 运行平台：Arm架构
* Revision Number   : V1.0                    // 版本号：V1.0
* Compiled Version  : G32A1xxx_01-June-25     // 编译版本：G32A1xxx 2025年6月1日
*
* Copyright (C) 2025 Geehy Semiconductor      // 版权所有 (C) 2025 珠海极海半导体
*
* You may not use this file except in compliance with the GEEHY COPYRIGHT NOTICE
* (GEEHY SOFTWARE PACKAGE LICENSE).
// 您仅可在遵守极海版权声明（极海软件包许可协议）的前提下使用本文件
*
* The program is only for reference, which is distributed in the hope that it
* will be useful and instructional for customers to develop their software.
* Unless required by applicable law or agreed to in writing, the program is
* distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY
* KIND, either express or implied. See the GEEHY SOFTWARE PACKAGE LICENSE for
* the governing permissions and limitations under the License.
// 本程序仅供参考，旨在为客户软件开发提供帮助与指导；除非法律要求或书面约定，本程序按"原样"分发，
// 不提供任何明示或默示的担保与条件，具体权限与限制请参阅极海软件包许可协议
*
*******************************************************************************/
#ifndef CAN_TP_H  // 头文件保护宏：防止该头文件被重复包含
#define CAN_TP_H  // 定义头文件唯一标识
#include "can_tp_cfg.h"  // 包含CAN TP模块的配置头文件
#ifdef ALLOW_CAN_TP      // 条件编译：仅当定义了ALLOW_CAN_TP宏时才编译以下内容
#include "multi_cyc_fifo.h"  // 包含多周期环形缓冲区(FIFO)头文件

/**************************************************************************
                    ENUMERATION DEFINITION  // 枚举类型定义区域
**************************************************************************/
/**
 * @brief   Enumeration for CAN TP work state  // CAN TP模块工作状态枚举定义
 */
typedef enum
{
    CAN_TP_IDLE_STATUS,        // 0：空闲状态，无收发任务
    CAN_TP_RX_SF_STATUS,       // 1：接收单帧(SF)处理状态
    CAN_TP_RX_FF_STATUS,       // 2：接收首帧(FF)处理状态
    CAN_TP_RX_FC_STATUS,       // 3：接收流控帧(FC)处理状态
    CAN_TP_RX_CF_STATUS,       // 4：接收连续帧(CF)处理状态
    CAN_TP_TX_SF_STATUS,       // 5：发送单帧(SF)处理状态
    CAN_TP_TX_FF_STATUS,       // 6：发送首帧(FF)处理状态
    CAN_TP_TX_FC_STATUS,       // 7：发送流控帧(FC)处理状态
    CAN_TP_TX_CF_STATUS,       // 8：发送连续帧(CF)处理状态
    CAN_TP_WAIT_TX_STATUS,     // 9：等待发送完成状态
    CAN_TP_WAIT_CONFIRM_STATUS // 10：等待确认状态
} CAN_TP_WorkStatusType;  // 定义CAN TP工作状态枚举类型名

/**
 * @brief   Enumeration for network frame type  // CAN TP网络帧类型枚举定义
 */
typedef enum
{
    SF,   // 0：单帧(Single Frame)，短数据单次传输
    FF,   // 1：首帧(First Frame)，长数据传输的第一帧
    CF,   // 2：连续帧(Consecutive Frame)，长数据传输的后续帧
    FC    // 3：流控帧(Flow Control)，接收方控制发送节奏
} CAN_TP_FrameType;  // 定义CAN TP帧类型枚举类型名

/**
 * @brief   Enumeration for flow status  // 流控帧状态枚举定义
 */
typedef enum
{
    CONTINUE_TO_SEND,  // 0：继续发送，接收方允许发送方继续传数据
    WAIT_FC,           // 1：等待流控帧，接收方暂忙，需等待新的流控帧
    OVERFLOW_BUF       // 2：缓冲区溢出，接收方缓存不足无法接收
} CAN_TP_FlowStatusType;  // 定义流控状态枚举类型名

/**
 * @brief   Enumeration for result codes  // CAN TP执行结果错误码枚举定义
 */
typedef enum
{
    CAN_TP_SUCCESS = 0,    // 0：执行成功
    CAN_TP_TIMEOUT_A,      // 1：A类超时（发送方未收到确认超时）
    CAN_TP_TIMEOUT_BS,     // 2：BS超时（发送首帧后未收到流控帧超时）
    CAN_TP_TIMEOUT_CR,     // 3：CR超时（接收连续帧间隔超时）
    CAN_TP_WRONG_SN,       // 4：序列号错误，连续帧序号不匹配
    CAN_TP_INVALID_FS,     // 5：无效流控状态，流控帧状态字段非法
    CAN_TP_UNEXP_PDU,      // 6：意外PDU，收到不符合当前状态的帧
    CAN_TP_WTF_OVRN,       // 7：等待帧溢出，等待流控帧次数超限
    CAN_TP_BUFF_OVFLW,     // 8：缓冲区溢出，数据长度超出缓存
    CAN_TP_ERROR           // 9：通用错误
} CAN_TP_ResultType;  // 定义执行结果枚举类型名

/**
 * @brief   Enumeration for CAN TP transmit message status  // CAN TP发送消息状态枚举
 */
typedef enum
{
    CAN_TP_TXMSG_STATUS_IDLE = 0,  // 0：发送消息空闲，无待发数据
    CAN_TP_TXMSG_STATUS_SUCCESS,   // 1：发送消息成功
    CAN_TP_TXMSG_STATUS_FAIL,      // 2：发送消息失败
    CAN_TP_TXMSG_STATUS_WAIT       // 3：发送消息等待中
} CAN_TP_TxMsgStatusType;  // 定义发送消息状态枚举类型名

/**************************************************************************
                    STRUCT DEFINITION  // 结构体定义区域
**************************************************************************/
/**
 * @brief   Structure for CAN TP data information  // CAN TP数据信息结构体
 */
typedef struct
{
    TP_UdsIdType xLocalId;                  // 本地UDS通信ID（CAN标识符）
    tCanTpDataLen xLocalPduLen;             // 当前已接收/发送的PDU数据总长度
    tCanTpDataLen xLocalFFLen;              // 首帧中记录的完整数据总长度
    uint8 aDataBuf[MAX_CF_DATA_LENGTH];     // 数据缓存数组，存储拼接后的完整数据
} CAN_TP_DataInfoType;  // 定义数据信息结构体类型名

/**
 * @brief   Structure for CAN TP info  // CAN TP运行状态信息结构体
 */
typedef struct
{
    uint8 seqNum;                      // 帧序列号，连续帧序号计数
    uint8 ucBlkSize;                   // 块大小计数，当前块内已收发的连续帧数量
    TP_TimingType stMinSepTime;        // 最小帧间隔时间（调度计数单位）
    TP_TimingType stMaxTimeoutTime;    // 超时时间计数器（调度计数单位）
    CAN_TP_DataInfoType stCanData;     // 关联的数据信息结构体
} CAN_TP_InfoType;  // 定义运行信息结构体类型名

/**
 * @brief   Structure for CAN TP message  // CAN TP单帧消息结构体
 */
typedef struct
{
    uint8 msgIsFree;                // 消息缓冲区是否空闲标志，1=空闲可用
    TP_UdsIdType msgLocalId;        // 消息对应的CAN ID
    uint8 msgLength;                // 消息数据有效长度
    uint8 msgLocalBuf[DATA_LENGTH]; // 消息数据缓存区（单帧8字节）
} CAN_TP_MsgType;  // 定义消息结构体类型名

/**
 * @brief   Function pointer definition for CAN TP functions  // CAN TP状态处理函数指针类型
 */
typedef CAN_TP_ResultType (*CAN_TP_ResFunctionType)(CAN_TP_MsgType *, CAN_TP_WorkStatusType *);
// 函数指针说明：入参为消息结构体指针、下一状态指针，返回执行结果码

/**
 * @brief   Structure to hold work state and associated function  // 状态-处理函数映射结构体
 */
typedef struct
{
    CAN_TP_WorkStatusType workState;    // 工作状态枚举值
    CAN_TP_ResFunctionType pointForFunc;// 该状态对应的处理函数指针
} CAN_TP_FunInfoType;  // 定义函数映射结构体类型名

static CAN_TP_InfoType gs_CanInfoTXData;      // 静态全局变量：发送方向的运行信息
static TP_TimingType gs_CanTPTXMin = 0u;      // 静态全局变量：发送方向STmin原始值(ms)
static uint32 gs_CanTXMsgWait = 0u;           // 静态全局变量：发送消息等待计数器
static CAN_TP_InfoType gs_CanInfoRXData;      // 静态全局变量：接收方向的运行信息
static CAN_TP_WorkStatusType gs_CanGlobalWorkStatus = CAN_TP_IDLE_STATUS; // 静态全局变量：CAN TP全局工作状态
static volatile CAN_TP_TxMsgStatusType gs_CanTXMsgStatus = CAN_TP_TXMSG_STATUS_IDLE; // 静态全局变量：发送消息状态（易失，防止编译器优化）
static TxCompletionCallback gs_CanTXMsgCB = NULL_PTR; // 静态全局变量：发送完成回调函数指针

/**************************************************************************
                    MACRO DEFINITION  // 宏定义区域
**************************************************************************/
// 时间转调度计数：将毫秒时间转换为主函数周期计数
#define CAN_TIME_TO_COUNT(time) ((time) / g_CanTPInformation.executionInterval)
// 判断是否为单帧：取字节高4位，等于SF则为真
#define IS_CAN_SF(xNetWorkFrameType) ((((xNetWorkFrameType) >> 4u) == SF) ? TRUE : FALSE)
// 判断是否为首帧：取字节高4位，等于FF则为真
#define IS_CAN_FF(xNetWorkFrameType) ((((xNetWorkFrameType) >> 4u) == FF) ? TRUE : FALSE)
// 判断是否为连续帧：取字节高4位，等于CF则为真
#define IS_CAN_CF(xNetWorkFrameType) ((((xNetWorkFrameType) >> 4u) == CF) ? TRUE : FALSE)
// 判断是否为流控帧：取字节高4位，等于FC则为真
#define IS_CAN_FC(xNetWorkFrameType) ((((xNetWorkFrameType)>> 4u) == FC) ? TRUE : FALSE)
// 校验接收连续帧序列号是否有效：与本地期望序号一致则有效
#define IS_CAN_SN_RX_VALID(xSN) ((gs_CanInfoRXData.seqNum == ((xSN) & 0x0Fu)) ? TRUE : FALSE)
// 接收方期望序列号自增，超过15则归零（0~15循环）
#define CAN_ADD_WAIT_SN()\
    do{\
        gs_CanInfoRXData.seqNum++;\
        if(gs_CanInfoRXData.seqNum > 0x0Fu)\
        {\
            gs_CanInfoRXData.seqNum = 0u;\
        }\
    }while(0u)

/* Get RX SF frame message length */  // 函数声明：解析接收单帧的数据长度
static boolean CAN_TP_AnalyzeSingleFrameLen(const uint32 i_RxMsgLen, const uint8 *i_pMsgBuf, uint32 *o_pFrameLen);
/* Get RX FF frame message length */  // 函数声明：解析接收首帧的总数据长度
static boolean CAN_TP_AnalyzeFFFrameLen(const uint32 i_RxMsgLen, const uint8 *i_pMsgBuf, uint32 *o_pFrameLen);
/* Check received message length valid or not? */  // 宏：校验接收帧长度是否合法
#define CAN_CHECK_RXMSG_LEN_VALID(addr, frameLen, RxMsgLen) ((addr == NORMAL_ADDRESSING) ? (frameLen <= RxMsgLen - 1) : (frameLen <= RxMsgLen - 2))
// 普通寻址：数据长度 <= 总长度-1；混合寻址：数据长度 <= 总长度-2

/* Save FF data len */  // 宏：将首帧解析出的总长度保存到接收信息结构体
#define CAN_SAVE_FF_DATA_LENGTH(rxDataLen) (gs_CanInfoRXData.stCanData.xLocalFFLen = rxDataLen)

/* Set BS */  // 宏：设置流控帧中的块大小(BS)字段
#define CAN_SET_BLK_SIZE(pBSBuf, blkLimit) (*(pBSBuf) = (uint8)(blkLimit))

/* Add block size */  // 宏：接收块计数自增（仅当配置了块限制时生效）
#define CAN_ADD_BLK_SIZE()\
    do{\
        if(0u != g_CanTPInformation.blockLimit)\
        {\
            gs_CanInfoRXData.ucBlkSize++;\
        }\
    }while(0u)

/* Set STmin */  // 宏：设置流控帧中的最小间隔时间(STmin)字段
#define CAN_SET_ST_MIN(pucSTminBuf, stMinSepTime) (*(pucSTminBuf) = (uint8)(stMinSepTime))

/* Set wait frame time */  // 宏：设置接收方向的超时等待计数
#define CAN_SET_WAIT_RXFRAME_TIME(waitTimeout)\
    do{\
        (gs_CanInfoRXData.stMaxTimeoutTime = CAN_TIME_TO_COUNT(waitTimeout));\
        gs_CanTXMsgWait = gs_CanInfoRXData.stMaxTimeoutTime;\
    }while(0u);

/* RX frame set RX msg wait time */  // 宏：接收场景下设置接收消息等待时间（复用上面的宏）
#define CAN_RXFRAME_RXMSG_WAIT_TIME(waitTimeout) CAN_SET_WAIT_RXFRAME_TIME(waitTimeout)
/* RX frame set TX msg wait time */  // 宏：接收场景下设置发送消息等待时间（复用上面的宏）
#define CAN_RXFRAME_TXMSG_WAIT_TIME(waitTimeout) CAN_SET_WAIT_RXFRAME_TIME(waitTimeout)

/* Set FS */  // 宏：设置流控帧中的流控状态(FS)字段（低4位）
#define CAN_SET_FS(pucFsBuf, xFlowStatus) (*(pucFsBuf) = (*(pucFsBuf) & 0xF0u) | (uint8)(xFlowStatus))

/* Add received data len */  // 宏：累加已接收的数据总长度
#define CAN_ADD_RXDATA_LENGTH(dataLen) (gs_CanInfoRXData.stCanData.xLocalPduLen += (dataLen))

/* Is received consecutive frame all. */  // 宏：判断连续帧是否全部接收完成
#define CAN_IS_RX_CFALL(dataLen) (((gs_CanInfoRXData.stCanData.xLocalPduLen + (uint8)(dataLen))\
                                    >= gs_CanInfoRXData.stCanData.xLocalFFLen) ? TRUE : FALSE)
// 已接收长度 + 当前帧数据长度 >= 总长度，则认为接收完成

/* Is wait Flow control timeout? */  // 宏：判断等待流控帧是否超时
#define CAN_IS_WAITFC_TIMEOUT()  ((0u == gs_CanInfoRXData.stMaxTimeoutTime) ? TRUE : FALSE)
// 计数器减到0则判定超时

/* Is wait consecutive frame timeout? */  // 宏：判断等待连续帧是否超时
#define CAN_IS_WAITCF_TIMEOUT() ((0u == gs_CanInfoRXData.stMaxTimeoutTime) ? TRUE : FALSE)

/* Is block size overflow */  // 宏：判断接收块大小是否达到配置上限
#define CAN_IS_RXBLK_SIZE_OF() (((0u != g_CanTPInformation.blockLimit) &&\
                                  (gs_CanInfoRXData.ucBlkSize >= g_CanTPInformation.blockLimit))\
                                 ? TRUE : FALSE)

/* Is transmitted data len overflow max SF? */  // 宏：判断发送数据是否超过单帧最大长度
#define CAN_IS_TXDATA_LEN_OFSF() ((gs_CanInfoTXData.stCanData.xLocalFFLen > TX_SF_DATA_MAX_LEN) ? TRUE : FALSE)

/* Is transmitted data less than min? */  // 宏：判断发送数据长度是否为0（无效）
#define CAN_IS_TXDATA_LEN_LESS() ((0u == gs_CanInfoTXData.stCanData.xLocalFFLen) ? TRUE : FALSE)

/* Set transmitted SF data len */  // 宏：设置单帧的数据长度字段（低4位）
#define CAN_SET_TXSF_DATA_LENGTH(dataLengthBuff, dataLength)\
    do{\
        *(dataLengthBuff) &= 0xF0u;\
        (*(dataLengthBuff) |= (dataLength));\
    }while(0u)

/* Set transmitted FF data len */  // 宏：设置首帧的12位总数据长度字段
#define CAB_SET_TXFF_DATA_LENGTH(dataLengthBuff, dataLength)\
    do{\
        *(dataLengthBuff + 0u) &= 0xF0u;\
        *(dataLengthBuff + 0u) |= (uint8)((dataLength) >> 8u);\
        *(dataLengthBuff + 1u) |= (uint8)(dataLength);\
    }while(0u)

/* Add TX data len */  // 宏：累加已发送的数据总长度
#define CAN_ADD_TX_DATA_LENGTH(dataLen) (gs_CanInfoTXData.stCanData.xLocalPduLen += (dataLen))

/* Set TX STmin */  // 宏：将发送最小间隔时间转换为调度计数
#define CAN_SET_TX_STMIN() (gs_CanInfoTXData.stMinSepTime = CAN_TIME_TO_COUNT(gs_CanTPTXMin))

/* Save TX STmin */  // 宏：保存从流控帧解析出的STmin原始值
#define CAN_SAVE_TX_STMIN(txSTmin) (gs_CanTPTXMin = txSTmin)

/* Is TX STmin timeout? */  // 宏：判断发送最小间隔时间是否到达
#define CAN_ISTX_STMIN_TIMEOUT() ((0u == gs_CanInfoTXData.stMinSepTime) ? TRUE : FALSE)

/* Set TX wait frame time */  // 宏：设置发送方向的超时等待计数
#define CAN_SET_TX_WAIT_TIME(waitTime)\
    do{\
        (gs_CanInfoTXData.stMaxTimeoutTime = CAN_TIME_TO_COUNT(waitTime));\
        gs_CanTXMsgWait = gs_CanInfoTXData.stMaxTimeoutTime;\
    }while(0u);

/* TX frame set TX message wait time */  // 宏：发送场景下设置发送消息等待时间
#define CAN_SET_TXMSG_WAIT_TIME(waitTime) CAN_SET_TX_WAIT_TIME(waitTime)
/* TX frame set TX message wait time */  // 宏：发送场景下设置接收消息等待时间
#define CAN_SET_RXMSG_WAIT_TIME(waitTime) CAN_SET_TX_WAIT_TIME(waitTime)

/* Is TX wait frame timeout? */  // 宏：判断发送方向等待帧是否超时
#define CAN_ISTX_WAIT_TIMEOUT() ((0u == gs_CanInfoTXData.stMaxTimeoutTime) ? TRUE : FALSE)

/* Is TX message wait frame timeout? */  // 宏：判断发送消息等待是否超时
#define CAN_ISTXMSG_WAIT_TIMEOUT() ((0u == gs_CanTXMsgWait) ? TRUE : FALSE)

/* Get FS */  // 宏：从流控帧中解析流控状态字段（低4位）
#define CAN_GET_FS(fs, fsBuff) (*(fsBuff) = (CAN_TP_FlowStatusType)((fs) & 0x0Fu))

/* Set TX SN */  // 宏：设置连续帧的序列号字段（低4位）
#define CAN_SET_TX_SN(setTxBuff) (*(setTxBuff) = gs_CanInfoTXData.seqNum | (*(setTxBuff) & 0xF0u))

/* Add TX SN */  // 宏：发送方序列号自增，超过15则归零
#define CAN_ADD_TX_SN()\
    do{\
        gs_CanInfoTXData.seqNum++;\
        if(gs_CanInfoTXData.seqNum > 0x0Fu)\
        {\
            gs_CanInfoTXData.seqNum = 0u;\
        }\
    }while(0u)

/* Is TX all */  // 宏：判断数据是否全部发送完成
#define CAN_ISTX_ALL() ((gs_CanInfoTXData.stCanData.xLocalPduLen >= \
                    gs_CanInfoTXData.stCanData.xLocalFFLen) ? TRUE : FALSE)

/* Save received message ID */  // 宏：保存接收到的消息ID到接收信息结构体
#define CAN_SAVE_RXMSG_ID(msgId) (gs_CanInfoRXData.stCanData.xLocalId = (msgId))

/* Clear CAN TP RX msg buffer */  // 宏：清空接收消息缓冲区
#define CAN_CLEAR_RX_BUFF(msgBuffer)\
    do{\
        (msgBuffer)->msgIsFree = TRUE;\
        (msgBuffer)->msgLength = 0u;\
        (msgBuffer)->msgLocalId = 0u;\
    }while(0u)

/* Get cur CAN TP status */  // 宏：获取CAN TP全局工作状态
#define CAN_GET_CUR_STATUS() (gs_CanGlobalWorkStatus)

/* Set cur CAN TP status */  // 宏：设置CAN TP全局工作状态
#define CAN_SET_CUR_STATUS(status)\
    do{\
        gs_CanGlobalWorkStatus = status;\
    }while(0u)

/* Get cur CAN TP status PTR */  // 宏：获取CAN TP全局工作状态的指针
#define CAN_GET_CUR_STATUS_PTR() (&gs_CanGlobalWorkStatus)

// 函数声明：空闲状态处理
static CAN_TP_ResultType CAN_TP_DoIdleProcess(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：接收单帧处理
static CAN_TP_ResultType CAN_TP_DoReceiveSingleFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：接收首帧处理
static CAN_TP_ResultType CAN_TP_DoReceiveFirstFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：接收连续帧处理
static CAN_TP_ResultType CAN_TP_DoConsecutiveFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：发送流控帧完成回调
static void CAN_TP_DoTxFlowControlFrameCB(void);
// 函数声明：发送流控帧处理
static CAN_TP_ResultType CAN_TP_DoTxFlowControlFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：发送单帧处理
static CAN_TP_ResultType CAN_TP_DoTxSingleFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：发送单帧完成回调
static void CAN_TP_DoTxSingleFrameCB(void);
// 函数声明：发送首帧完成回调
static void CAN_TP_DoTxFirstFrameCB(void);
// 函数声明：发送首帧处理
static CAN_TP_ResultType CAN_TP_DoTxFirstFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：接收流控帧处理
static CAN_TP_ResultType CAN_TP_DoReceiveFlowControlFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：发送连续帧完成回调
static void CAN_TP_DoTxConsecutiveFrameCB(void);
// 函数声明：发送连续帧处理
static CAN_TP_ResultType CAN_TP_DoTxConsecutiveFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：等待发送消息处理
static CAN_TP_ResultType CAN_TP_DoWaitTxMessage(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState);
// 函数声明：设置发送帧的类型字段
static uint8 CAN_TP_SetTransmitFrameType(const CAN_TP_FrameType i_eFrameType, uint8 *o_pucFrameType);
// 函数声明：将接收完整数据拷贝到接收FIFO
static uint8 CAN_TP_CopyFrameIntoRxFifo(const TP_UdsIdType udsId, const fifoSizeType dataLen, const uint8 *dataBuf);
// 函数声明：从发送FIFO取出一帧待发数据
static uint8 CAN_TP_CopyFrameDataToFifo(TP_UdsIdType *outTxCanID, uint8 *outTxDataLen, uint8 *outDataBuf);
// 函数声明：发送消息成功回调处理
static void CAN_TP_TxMsgSuccessCB(void);
// 函数声明：设置发送消息状态
static void CAN_TP_SetTxMessageState(const CAN_TP_TxMsgStatusType txMessageState);
// 函数声明：注册发送消息完成回调
static void CAN_TP_RegTxMessageCB(const TxCompletionCallback regTxMegCB);
// 函数声明：执行注册的发送回调
static void CAN_TP_DoRegTxMessageCB(void);
// 函数声明：CAN TP主处理函数（周期调用）
void CAN_TP_MainFunction(void);
// 函数声明：系统滴答定时处理（计时递减）
void CAN_TP_SysTickController(void);
// 函数声明：CAN TP模块初始化
void CAN_TP_Init(void);

#endif  // 结束 ALLOW_CAN_TP 条件编译
#endif  // 结束头文件保护