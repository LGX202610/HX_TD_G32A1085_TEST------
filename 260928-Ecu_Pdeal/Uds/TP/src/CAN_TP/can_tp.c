#include "includes.h" // 工程全局统一基础头文件，包含标准类型、基础宏、中断开关等通用定义
#ifdef ALLOW_CAN_TP // 条件编译宏：开启CAN TP传输层功能时，才编译本文件
#include "can_tp.h" // CAN TP协议核心头文件，存放状态枚举、全局结构体、函数声明、帧类型宏定义
#include "TP_cfg.h" // CAN TP协议配置头文件，存放块大小、超时时间、缓冲区长度等项目可配置参数
// 全局常量数组：CAN TP状态机映射表，将每个工作状态绑定对应的状态处理函数
static const CAN_TP_FunInfoType gs_astCANTpFunInfo[] =
{
    {CAN_TP_IDLE_STATUS, CAN_TP_DoIdleProcess},                // 空闲状态 -> 空闲状态主处理函数
    {CAN_TP_RX_SF_STATUS, CAN_TP_DoReceiveSingleFrame},        // 接收单帧SF状态 -> 单帧接收处理函数
    {CAN_TP_RX_FF_STATUS, CAN_TP_DoReceiveFirstFrame},         // 接收首帧FF状态 -> 多帧传输首帧接收处理函数
    {CAN_TP_TX_FC_STATUS, CAN_TP_DoTxFlowControlFrame},        // 发送流控FC帧状态 -> 发送流控帧处理函数
    {CAN_TP_RX_CF_STATUS, CAN_TP_DoConsecutiveFrame},          // 接收连续CF帧状态 -> 多帧连续帧接收处理函数
    {CAN_TP_TX_SF_STATUS, CAN_TP_DoTxSingleFrame},             // 发送单帧SF状态 -> 单帧发送处理函数
    {CAN_TP_TX_FF_STATUS, CAN_TP_DoTxFirstFrame},              // 发送首帧FF状态 -> 多帧传输首帧发送处理函数
    {CAN_TP_RX_FC_STATUS, CAN_TP_DoReceiveFlowControlFrame},   // 接收流控FC帧状态 -> 接收对方下发流控帧处理函数
    {CAN_TP_TX_CF_STATUS, CAN_TP_DoTxConsecutiveFrame},       // 发送连续CF帧状态 -> 多帧连续帧发送处理函数
    {CAN_TP_WAIT_TX_STATUS, CAN_TP_DoWaitTxMessage}            // 等待CAN硬件发送完成状态 -> 发送等待超时检测处理函数
};
/**
 * @brief   将接收完成的完整TP报文拷贝写入UDS上层接收FIFO
 * @desc    功能说明：底层CAN TP拼接完成一整包UDS数据后，把完整载荷存入上层诊断接收队列，供UDS服务读取
 * @param udsId 报文对应的CAN收发ID（物理寻址/功能寻址ID）
 * @param dataLen 拼接完成后完整有效数据字节长度
 * @param dataBuf 存储完整报文载荷的数据缓冲区指针
 * @retval uint8 TRUE=写入FIFO成功；FALSE=FIFO空间不足/参数非法/写入失败
 */
static uint8 CAN_TP_CopyFrameIntoRxFifo(const TP_UdsIdType udsId, // 入参：报文CAN ID
                                        const fifoSizeType dataLen, // 入参：完整有效数据长度
                                        const uint8 *dataBuf) // 入参：完整数据缓存指针
{
    errorStateType locErr; // FIFO操作返回错误状态变量
    fifoSizeType locCanWrite = 0u; // TP接收FIFO当前剩余可写入字节容量
    TP_TransportExchangeInfoType stLocExMsg; // 传输交互临时结构体，存储单条报文ID、长度、发送回调
    
    ASSERT(NULL_PTR == dataBuf); // 空指针断言调试：数据缓存指针为空时触发断言报错
    if (0u != dataLen) // 仅有效数据长度大于0时，执行FIFO写入逻辑，空报文直接返回失败
    {
        /* 查询接收队列剩余可写入空间 */
        GainAccessProgramSize(ID_RX_TP_QUEUE, &locCanWrite, &locErr);
        // 判断：FIFO操作无错误 且 剩余空间足够存放交互头结构体 + 报文载荷
        if ((STATE_NO_ERROR == locErr) && (locCanWrite >= (dataLen + sizeof(TP_TransportExchangeInfoType))))
        {
            stLocExMsg.messageID = udsId; // 填充交互头：本条报文对应的CAN ID
            stLocExMsg.dataLen = dataLen; // 填充交互头：本条报文有效数据长度
            stLocExMsg.pfTxCall = NULL_PTR; // 接收报文无发送完成回调函数，置空
            /* 第一步：将交互信息头部写入TP接收FIFO */
            ProgramToFifo(ID_RX_TP_QUEUE, (uint8 *)&stLocExMsg, sizeof(TP_TransportExchangeInfoType), &locErr);
            if (STATE_NO_ERROR == locErr) // 交互头写入成功，继续写入报文载荷
            {
                /* 第二步：把完整报文载荷写入TP接收FIFO */
                ProgramToFifo(ID_RX_TP_QUEUE, (uint8 *)dataBuf, dataLen, &locErr);
                if (STATE_NO_ERROR == locErr) // 载荷写入正常，返回成功标识
                {
                    return TRUE;
                }
                else
                {
                    return FALSE; // 载荷数据写入FIFO失败
                }
            }
            else
            {
                return FALSE; // 交互头部写入FIFO失败
            }
        }
        else
        {
            return FALSE; // FIFO存储空间不足 或 查询队列状态异常
        }
    }
    else
    {
        return FALSE; // 报文有效数据长度为0，无需存储，直接返回失败
    }
}
/**
 * @brief   从TP发送FIFO读取待发送完整UDS报文数据
 * @desc    功能说明：上层UDS下发待发送数据存入TX FIFO后，TP层读取FIFO，取出CAN ID、长度、载荷用于CAN帧分包发送
 * @param outTxCanID 出参：输出待发送报文的CAN ID
 * @param outTxDataLen 出参：输出完整报文总数据长度
 * @param outDataBuf 出参：输出报文载荷存储缓冲区指针
 * @retval uint8 TRUE=读取到有效待发报文；FALSE=发送队列为空/读取失败/参数错误
 */
static uint8 CAN_TP_CopyFrameDataToFifo(TP_UdsIdType *outTxCanID, // 出参：待发送CAN ID
                                        uint8 *outTxDataLen, // 出参：待发送完整数据长度
                                        uint8 *outDataBuf) // 出参：待发送数据缓存
{
    errorStateType locErr; // FIFO操作错误状态码
    fifoSizeType locReadLen = 0u; // FIFO单次可读字节长度
    /* 本地临时结构体，存储从发送FIFO读出的报文交互头部信息 */
    TP_TransportExchangeInfoType stLocExMsg;
    
    ASSERT(NULL_PTR == outTxCanID); // 空指针断言：输出CAN ID指针为空报错
    ASSERT(NULL_PTR == outTxDataLen); // 空指针断言：输出长度指针为空报错
    ASSERT(NULL_PTR == outDataBuf); // 空指针断言：输出数据缓存指针为空报错
    
    /* 获取TP发送队列当前可读总字节数 */
    GainAccessReadSize(ID_TX_TP_QUEUE, &locReadLen, &locErr);
    // 判断：队列操作无异常、存在可读数据、可读长度至少包含交互头部
    if ((STATE_NO_ERROR == locErr) && 
        (0u != locReadLen) && 
        (locReadLen >= sizeof(TP_TransportExchangeInfoType)))
    {
        /* 第一步：从发送FIFO读取报文交互头部（ID+长度+回调） */
        GainInfoDataInFifo(ID_TX_TP_QUEUE,
                         sizeof(TP_TransportExchangeInfoType),
                         (uint8 *)&stLocExMsg,
                         &locReadLen,
                         &locErr);
        // 判断头部读取无错误，且读取字节数和头部结构体大小匹配
        if (STATE_NO_ERROR == locErr && 
            sizeof(TP_TransportExchangeInfoType) == locReadLen)
        {
            /* 第二步：读取报文载荷数据到输出缓存 */
            GainInfoDataInFifo(ID_TX_TP_QUEUE,
                             stLocExMsg.dataLen,
                             outDataBuf,
                             &locReadLen,
                             &locErr);
            // 判断载荷读取正常，读取长度和报文定义长度一致
            if (STATE_NO_ERROR == locErr && 
                stLocExMsg.dataLen == locReadLen)
            {
                *outTxCanID = stLocExMsg.messageID; // 将读出的CAN ID赋值给出参
                *outTxDataLen = stLocExMsg.dataLen; // 将读出的数据长度赋值给出参
                TP_RegisterFrameTxCallback(stLocExMsg.pfTxCall); // 注册上层传入的发送完成回调函数
                
                return TRUE; // 完整报文读取成功
            }
            else
            {
                return FALSE; // 报文载荷读取长度不匹配或读取报错
            }
        }
        else
        {
            return FALSE; // 报文交互头部读取失败/长度不匹配
        }
    }
    else
    {
        return FALSE; // 发送FIFO无有效待发送报文或队列状态异常
    }
}
// 空闲状态处理函数：TP无收发任务时循环执行，区分两种分支：收到底层CAN报文、上层有待发报文
static CAN_TP_ResultType CAN_TP_DoIdleProcess(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    uint8 localTxLen = (uint8)gs_CanInfoTXData.stCanData.xLocalFFLen; // 全局发送缓存中待发送完整报文总长度
    /* 只在进入 IDLE 时清一次缓存，空闲轮询不再反复 memset */
    static uint8 s_idle_buf_cleared = 0u;
    
    ASSERT(NULL_PTR == penNextState); // 空指针断言：下一状态输出指针为空
    
    if (0u == s_idle_buf_cleared)
    {
        /* 清空全局接收方向TP缓存结构体，重置所有接收状态、长度、缓冲区 */
        Momory_Fill_Function((void *)&gs_CanInfoRXData, 0u, sizeof(CAN_TP_InfoType));
        /* 清空全局发送方向TP缓存结构体，重置所有发送状态、长度、缓冲区 */
        Momory_Fill_Function((void *)&gs_CanInfoTXData, 0u, sizeof(CAN_TP_InfoType));
        /* 清空发送等待超时计数器 */
        gs_CanTXMsgWait = 0u;
        /* 全局发送回调置空，清除上一次报文的发送完成回调 */
        TP_RegisterFrameTxCallback(NULL_PTR);
        s_idle_buf_cleared = 1u;
        localTxLen = 0u;
    }
    
    /* 分支1：硬件CAN收到报文，本地接收缓存存在数据，仅处理SF单帧/FF首帧，CF/FC直接丢弃 */
    switch (pstLocalMsg->msgIsFree) // 判断本地CAN接收缓存是否存有硬件收到的报文
    {
        case FALSE: // 缓存内存在刚收到的CAN报文
        {
            if (TRUE == IS_CAN_SF(pstLocalMsg->msgLocalBuf[0u])) // 判断帧类型为单帧SF（数据≤7字节标准CAN）
            {
                *penNextState = CAN_TP_RX_SF_STATUS; // 切换下一状态：进入单帧接收流程
            }
            else if (TRUE == IS_CAN_FF(pstLocalMsg->msgLocalBuf[0u])) // 判断帧类型为首帧FF（长报文多帧起始帧）
            {
                *penNextState = CAN_TP_RX_FF_STATUS; // 切换下一状态：进入多帧首帧接收流程
            }
            else
            {
                /* CF连续帧、FC流控帧在空闲状态直接忽略，不切换状态 */
            }
            break;
        }
        case TRUE:
        default: // 本地CAN接收缓存无新报文，检查上层UDS是否有待发送报文
        {
            /* 尝试从TX FIFO读取上层待发送完整报文 */
            if (TRUE != CAN_TP_CopyFrameDataToFifo(&gs_CanInfoTXData.stCanData.xLocalId,
                                                   &localTxLen,
                                                   gs_CanInfoTXData.stCanData.aDataBuf))
            // 读取失败：发送队列为空，无待发数据，保持空闲状态
            {
                /* 无操作 */
            }
            else // 成功读取到上层下发的完整待发送报文
            {
                gs_CanInfoTXData.stCanData.xLocalFFLen = localTxLen; // 保存报文总长度到全局发送缓存
                if (TRUE != CAN_IS_TXDATA_LEN_OFSF()) // 判断总长度≤7字节，单帧即可发送
                {
                    *penNextState = CAN_TP_TX_SF_STATUS; // 切换状态：单帧发送流程
                }
                else // 报文长度超过单帧上限，需要FF+CF多帧分包传输
                {
                    *penNextState = CAN_TP_TX_FF_STATUS; // 切换状态：首帧FF发送流程
                }
            }
            break;
        }
    }
    /* 离开 IDLE 后下次再进时重新清缓存 */
    if (CAN_TP_IDLE_STATUS != *penNextState)
    {
        s_idle_buf_cleared = 0u;
    }
    return CAN_TP_SUCCESS; // 空闲状态处理完成，无协议错误
}
/**
 * @brief   发送方等待接收方下发FC流控帧处理函数
 * @desc    功能说明：发送完FF首帧后进入该状态，等待对方回复FC流控帧，解析BS块大小、STmin间隔，控制CF连续帧发送节奏
 * @param pstLocalMsg 底层CAN接收报文临时缓存结构体
 * @param penNextState 出参：处理完成后切换的下一TP状态
 * @retval CAN_TP_ResultType 协议处理结果码（成功/超时/缓存溢出/非法流控）
 */
static CAN_TP_ResultType CAN_TP_DoReceiveFlowControlFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    CAN_TP_FlowStatusType flowState; // 存储FC帧FS流控状态字段（0=继续发、1=等待、2=接收缓存溢出）
    ASSERT(NULL_PTR == penNextState); // 空指针断言：下一状态指针为空报错
    /* 判断等待FC帧是否超时，超时直接终止本次多帧传输 */
    if (TRUE != CAN_ISTX_WAIT_TIMEOUT())
    {
        // 未超时，判断本地缓存是否收到新CAN报文
        if ((0u != pstLocalMsg->msgLength) && (TRUE != pstLocalMsg->msgIsFree))
        {
            if (TRUE == IS_CAN_FC(pstLocalMsg->msgLocalBuf[0u])) // 当前收到的报文是FC流控帧
            {
                /* 提取FC帧首字节内FS流控状态值 */
                CAN_GET_FS(pstLocalMsg->msgLocalBuf[0u], &flowState);
                if (OVERFLOW_BUF != flowState) // FS≠2，接收方缓存未溢出，正常流控指令
                {
                    if (WAIT_FC != flowState) // FS≠1，允许继续发送连续CF帧
                    {
                        if (CONTINUE_TO_SEND == flowState) // FS=0：正常下发流控，可发送CF
                        {
                            CAN_SET_BLK_SIZE(&gs_CanInfoTXData.ucBlkSize, pstLocalMsg->msgLocalBuf[1u]);
                            // 读取FC帧BS块大小，设置每块连续帧发送数量上限
                            CAN_SAVE_TX_STMIN(pstLocalMsg->msgLocalBuf[2u]); // 读取FC帧STmin，保存CF帧最小发送间隔(ms)
                            CAN_SET_TXMSG_WAIT_TIME(g_CanTPInformation.senderConsecutiveLimit);
                            // 设置连续帧发送整体超时时间
#if 0
                            CAN_ADD_TX_SN();
#endif
                        }
                        else
                        {
                            /* 非法FS流控值，协议异常，切回空闲 */
                            *penNextState = CAN_TP_IDLE_STATUS;
                            return CAN_TP_INVALID_FS;
                        }
                        *penNextState = CAN_TP_TX_CF_STATUS; // 切换状态：开始发送连续CF帧
                        return CAN_TP_SUCCESS;
                    }
                    else
                    {
                        /* FS=1：接收方要求暂停发送，保持当前状态继续等待FC */
                        CAN_SET_RXMSG_WAIT_TIME(g_CanTPInformation.senderBufferThreshold);
                        return CAN_TP_SUCCESS;
                    }
                }
                else
                {
                    /* FS=2：接收端缓冲区溢出，终止本次传输，切空闲 */
                    *penNextState = CAN_TP_IDLE_STATUS;
                    return CAN_TP_BUFF_OVFLW;
                }
            }
            else
            {
                /* 当前收到的不是FC帧，协议错误 */
                return CAN_TP_ERROR;
            }
        }
        else
        {
            /* 暂无新收到CAN报文，持续等待FC帧，无错误返回 */
            return CAN_TP_SUCCESS;
        }
    }
    else
    {
        TPDebugLog("Wait flow control timeout.\n"); // 打印调试日志：等待FC流控帧超时
        *penNextState = CAN_TP_IDLE_STATUS; // 超时切回空闲状态，终止传输
        return CAN_TP_TIMEOUT_CR;
    }
}
/**
 * @brief   单帧SF接收状态处理函数
 * @desc    功能说明：收到单帧报文后解析有效长度，将完整数据存入UDS接收FIFO
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理完成后切换的TP状态
 * @retval CAN_TP_ResultType 协议处理结果码
 */
static CAN_TP_ResultType CAN_TP_DoReceiveSingleFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    uint32 sfLength = 0u; // 单帧报文中有效UDS数据长度
    ASSERT(NULL_PTR == penNextState);
    // 校验缓存存在有效报文
    if ((0u != pstLocalMsg->msgLength) && (TRUE != pstLocalMsg->msgIsFree))
    {
        if (TRUE == IS_CAN_SF(pstLocalMsg->msgLocalBuf[0u])) // 确认当前报文为SF单帧
        {
            /* 解析SF帧，提取真实有效数据长度 */
            if (TRUE == CAN_TP_AnalyzeSingleFrameLen(pstLocalMsg->msgLength, pstLocalMsg->msgLocalBuf, &sfLength))
            {
                /* 把解析完成的完整单帧数据写入上层UDS接收FIFO */
                if (FALSE != CAN_TP_CopyFrameIntoRxFifo(pstLocalMsg->msgLocalId,
                                                          sfLength,
                                                          &pstLocalMsg->msgLocalBuf[1u]))
                {
                    *penNextState = CAN_TP_IDLE_STATUS; // 单帧接收完成，切回空闲
                    return CAN_TP_SUCCESS;
                }
                else
                {
                    TPDebugLog("Copy data error!\n"); // FIFO写入失败日志
                    return CAN_TP_ERROR;
                }
            }
            else
            {
                TPDebugLog("SF:CAN_TP_AnalyzeSingleFrameLen failed!\n"); // SF长度解析失败日志
                return CAN_TP_ERROR;
            }
        }
        else
        {
            return CAN_TP_ERROR; // 帧类型不是SF，协议错误
        }
    }
    else
    {
        return CAN_TP_ERROR; // 接收缓存无有效报文
    }
}
/**
 * @brief   首帧FF接收状态处理函数
 * @desc    功能说明：收到多帧报文首帧，解析总长度，缓存FF内载荷，下发FC流控帧给发送方
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理完成后切换TP状态
 * @retval CAN_TP_ResultType 协议处理结果码
 */
static CAN_TP_ResultType CAN_TP_DoReceiveFirstFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    uint32 localFFLen = 0u; // FF帧解析出的报文总长度
    ASSERT(NULL_PTR == penNextState);
    if ((0u != pstLocalMsg->msgLength) && (TRUE != pstLocalMsg->msgIsFree))
    {
        if (TRUE == IS_CAN_FF(pstLocalMsg->msgLocalBuf[0u])) // 确认当前报文是FF首帧
        {
            /* 解析FF帧，获取整条报文完整数据长度 */
            if (TRUE == CAN_TP_AnalyzeFFFrameLen(pstLocalMsg->msgLength, pstLocalMsg->msgLocalBuf, &localFFLen))
            {
                /* 保存本条多帧报文对应的CAN ID到全局接收缓存 */
                CAN_SAVE_RXMSG_ID(pstLocalMsg->msgLocalId);
                /* 存储整条报文总长度 */
                CAN_SAVE_FF_DATA_LENGTH(localFFLen);
                /* 设置等待CF连续帧的超时阈值 */
                CAN_RXFRAME_TXMSG_WAIT_TIME(g_CanTPInformation.receiverBufferThreshold);
                /* 将FF帧内携带的有效载荷拷贝到全局接收缓存 */
                Momory_Copy_Function(gs_CanInfoRXData.stCanData.aDataBuf, (const void *)&pstLocalMsg->msgLocalBuf[2u], pstLocalMsg->msgLength - 2u);
                /* 累加已接收数据长度 */
                CAN_ADD_RXDATA_LENGTH(pstLocalMsg->msgLength - 2u);
                /* 切换状态：发送FC流控帧告知发送方可发送CF */
                *penNextState = CAN_TP_TX_FC_STATUS;
                CAN_CLEAR_RX_BUFF(pstLocalMsg); // 清空本地CAN接收缓存
                return CAN_TP_SUCCESS;
            }
            else
            {
                TPDebugLog("FF:GetRXFrameMsgLength failed!\n"); // FF总长度解析失败日志
                return CAN_TP_ERROR;
            }
        }
        else
        {
            TPDebugLog("Received not FF\n"); // 当前报文不是FF首帧，报错
            return CAN_TP_ERROR;
        }
    }
    else
    {
        return CAN_TP_ERROR; // 接收缓存无有效报文
    }
}
/**
 * @brief   连续CF帧接收状态处理函数
 * @desc    功能说明：接收多帧传输的连续CF帧，校验SN序列号，拼接载荷；全部接收完成后存入UDS FIFO
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理完成后TP状态
 * @retval CAN_TP_ResultType 协议处理结果码（序列号错误/超时/成功）
 */
static CAN_TP_ResultType CAN_TP_DoConsecutiveFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    ASSERT(NULL_PTR == penNextState);
    /* 判断接收CF帧是否超时，超时直接终止传输 */
    if (TRUE != CAN_IS_WAITCF_TIMEOUT())
    {
        if (0u != pstLocalMsg->msgLength && TRUE != pstLocalMsg->msgIsFree)
        {
            /* 接收CF期间不能收到SF/FF新报文，收到则判定意外报文 */
            if ((TRUE != IS_CAN_SF(pstLocalMsg->msgLocalBuf[0u])) && (TRUE != IS_CAN_FF(pstLocalMsg->msgLocalBuf[0u])))
            {
                /* 校验当前报文CAN ID和FF首帧ID一致，过滤其他ECU报文干扰 */
                if (gs_CanInfoRXData.stCanData.xLocalId == pstLocalMsg->msgLocalId)
                {
                    if (TRUE == IS_CAN_CF(pstLocalMsg->msgLocalBuf[0u])) // 确认报文为CF连续帧
                    {
                        /* 校验CF帧SN序列号是否连续合法 */
                        if (TRUE == IS_CAN_SN_RX_VALID(pstLocalMsg->msgLocalBuf[0u]))
                        {
                            /* 判断当前所有CF是否接收完毕，达到总长度 */
                            if (TRUE == CAN_IS_RX_CFALL(pstLocalMsg->msgLength - 1u))
                            {
                                /* 拷贝最后一段CF载荷到全局接收缓存 */
                                Momory_Copy_Function(&gs_CanInfoRXData.stCanData.aDataBuf[gs_CanInfoRXData.stCanData.xLocalPduLen],
                                           &pstLocalMsg->msgLocalBuf[1u],
                                           gs_CanInfoRXData.stCanData.xLocalFFLen - gs_CanInfoRXData.stCanData.xLocalPduLen);
                                /* 完整报文拼接完成，写入上层UDS接收FIFO */
                                (void)CAN_TP_CopyFrameIntoRxFifo(gs_CanInfoRXData.stCanData.xLocalId,
                                                                   gs_CanInfoRXData.stCanData.xLocalFFLen,
                                                                   gs_CanInfoRXData.stCanData.aDataBuf);
                                *penNextState = CAN_TP_IDLE_STATUS; // 全部接收完成，切空闲
                            }
                            else
                            {
                                /* 未收完一块数据，判断是否达到BS块大小上限 */
                                if (TRUE != CAN_IS_RXBLK_SIZE_OF())
                                {
                                    /* 块内CF未发完，自增SN序列号，继续等待下一条CF */
                                    CAN_ADD_WAIT_SN();
                                    /* 设置下一条CF接收超时时间 */
                                    CAN_RXFRAME_RXMSG_WAIT_TIME(g_CanTPInformation.receiverConsecutiveLimit);
                                }
                                else
                                {
                                    /* 当前块CF全部接收完成，需要下发FC流控帧请求下一块 */
                                    CAN_RXFRAME_TXMSG_WAIT_TIME(g_CanTPInformation.receiverBufferThreshold);
                                    *penNextState = CAN_TP_TX_FC_STATUS;
                                }
                                /* 将本条CF载荷拷贝至全局接收缓存，累加接收长度 */
                                Momory_Copy_Function(&gs_CanInfoRXData.stCanData.aDataBuf[gs_CanInfoRXData.stCanData.xLocalPduLen],
                                           &pstLocalMsg->msgLocalBuf[1u],
                                           pstLocalMsg->msgLength - 1u);
                                CAN_ADD_RXDATA_LENGTH(pstLocalMsg->msgLength - 1u);
                            }
                            return CAN_TP_SUCCESS;
                        }
                        else
                        {
                            TPDebugLog("Msg SN invalid in CF!\n"); // CF序列号不连续报错日志
                            return CAN_TP_WRONG_SN;
                        }
                    }
                    else
                    {
#ifdef EN_TP_DEBUG
                        TPDebugLog("Msg type invalid in CF %X!\n", pstLocalMsg->msgLocalBuf[0u]);
#endif					
                        return CAN_TP_ERROR; // 帧类型不是CF，协议错误
                    }
                }
                else
                {
#ifdef EN_TP_DEBUG
                    TPDebugLog("Msg ID invalid in CF! F RX ID = %X, RX ID = %X\n",
                                  gs_CanInfoRXData.stCanData.xLocalId, pstLocalMsg->msgLocalId);
#endif
                    return CAN_TP_ERROR; // CAN ID和首帧不匹配，过滤干扰报文
                }
            }
            else
            {
                TPDebugLog("In receive progresses: received SF\n"); // 多帧接收中途收到新单帧，协议异常
                *penNextState = CAN_TP_IDLE_STATUS;
                return CAN_TP_UNEXP_PDU;
            }
        }
        else
        {
            /* 暂无新CF报文，保持等待，无错误返回 */
            return CAN_TP_SUCCESS;
        }
    }
    else
    {
        TPDebugLog("Wait consecutive frame timeout!\n"); // 等待CF连续帧超时日志
        *penNextState = CAN_TP_IDLE_STATUS;
        return CAN_TP_TIMEOUT_CR;
    }
}
/**
 * @brief   FC流控帧发送完成回调函数
 * @desc    功能说明：硬件成功发出FC帧后触发，切换状态继续接收CF连续帧
 */
static void CAN_TP_DoTxFlowControlFrameCB(void)
{
    /* 判断整条报文总长度是否小于单帧上限，无需CF */
    if (gs_CanInfoRXData.stCanData.xLocalFFLen <= MAX_CF_DATA_LENGTH)
    {
        /* Set wait STmin */
        CAN_RXFRAME_RXMSG_WAIT_TIME(g_CanTPInformation.receiverConsecutiveLimit);
        CAN_SET_CUR_STATUS(CAN_TP_RX_CF_STATUS);
    }
    else
    {
        CAN_SET_CUR_STATUS(CAN_TP_IDLE_STATUS);
    }
}
/**
 * @brief   FC流控帧发送状态处理函数
 * @desc    功能说明：组装FC帧（FS/BS/STmin），调用底层CAN发送接口，注册发送完成回调
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理后切换TP状态
 * @retval CAN_TP_ResultType 协议处理结果码
 */
static CAN_TP_ResultType CAN_TP_DoTxFlowControlFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    uint8 aucTransDataBuf[DATA_LENGTH] = {0u}; /* PCI 先按 0 组，空位再填 0x55 */
    /* 判断是否达到发送FC的等待超时时间 */
    if (TRUE == CAN_IS_WAITFC_TIMEOUT())
    {
        /* 填充FC帧首字节帧类型标识 */
        (void)CAN_TP_SetTransmitFrameType(FC, &aucTransDataBuf[0u]);
        /* 根据报文总长度设置FS流控状态：超长报文标记缓存溢出，否则允许继续发送 */
        if (gs_CanInfoRXData.stCanData.xLocalFFLen > MAX_CF_DATA_LENGTH)
        {
            CAN_SET_FS(&aucTransDataBuf[1u], OVERFLOW_BUF);
        }
        else
        {
            CAN_SET_FS(&aucTransDataBuf[1u], CONTINUE_TO_SEND);
        }
        CAN_SET_BLK_SIZE(&aucTransDataBuf[1u], g_CanTPInformation.blockLimit); // 填充BS块大小
        CAN_ADD_BLK_SIZE(); // 更新块计数
        CAN_ADD_WAIT_SN(); // 序列号等待计数重置
        CAN_SET_ST_MIN(&aucTransDataBuf[2u], g_CanTPInformation.stMinSepTime); // 填充STmin最小间隔
        Momory_Fill_Function(&aucTransDataBuf[3u], CAN_TP_PADDING_BYTE, 5u); /* FC 有效 3 字节，其余 0x55 */
        CAN_RXFRAME_TXMSG_WAIT_TIME(g_CanTPInformation.receiverTimeoutValue); // 设置接收整体超时
        CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_WAIT); // 标记当前发送任务等待硬件完成
        CAN_TP_RegTxMessageCB(CAN_TP_DoTxFlowControlFrameCB); // 注册FC发送完成回调
        /* 调用底层CAN发送接口，发送FC流控帧 */
        if (TRUE != g_CanTPInformation.transmitMessage(g_CanTPInformation.transmissionIdentifier,
                                                         sizeof(aucTransDataBuf),
                                                         aucTransDataBuf,
                                                         CAN_TP_TxMsgSuccessCB,
                                                         g_CanTPInformation.maxTransmitBlockTimeMs))
        {
            /* CAN硬件发送FC失败，重置发送状态，切空闲 */
            CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_FAIL);
            CAN_TP_RegTxMessageCB(NULL_PTR);
            *penNextState = CAN_TP_IDLE_STATUS;
            return CAN_TP_ERROR;
        }
        else
        {
            /* FC帧下发成功，进入等待硬件发送完成状态 */
            *penNextState = CAN_TP_WAIT_TX_STATUS;
            return CAN_TP_SUCCESS;
        }
    }
    else
    {
        TPDebugLog("\n Waiting transmit FC not timeout!\n");
        /* 未到下发FC的等待时间，保持当前状态 */
        return CAN_TP_SUCCESS;
    }
}
/**
 * @brief   SF单帧发送完成回调函数
 * @desc    功能说明：单帧硬件发送成功后，通知上层发送完成，切回空闲状态
 */
static void CAN_TP_DoTxSingleFrameCB(void)
{
    TP_TransmitSingleFrameCallback(E_TX_MSG_OK); // 向上层UDS上报发送成功
    CAN_SET_CUR_STATUS(CAN_TP_IDLE_STATUS); // 发送结束，切空闲
}
/**
 * @brief   SF单帧发送状态处理函数
 * @desc    功能说明：上层待发数据≤7字节时，组装SF单帧，调用CAN底层发送
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理后TP状态
 * @retval CAN_TP_ResultType 协议处理结果码
 */
static CAN_TP_ResultType CAN_TP_DoTxSingleFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    uint8 localBuf[DATA_LENGTH] = {0u}; // SF帧发送临时缓存
    uint8 localLen = 0u; // SF帧完整发送长度
    ASSERT(NULL_PTR == penNextState);
    /* 校验报文长度符合SF单帧范围 */
    if (TRUE != CAN_IS_TXDATA_LEN_OFSF())
    {
        if (TRUE != CAN_IS_TXDATA_LEN_LESS())
        {
            /* 填充SF帧类型标识+有效长度 */
            (void)CAN_TP_SetTransmitFrameType(SF, &localBuf[0u]);
            CAN_SET_TXSF_DATA_LENGTH(&localBuf[0u], gs_CanInfoTXData.stCanData.xLocalFFLen);
            localLen = localBuf[0u] + 1u;
            /* 将待发载荷拷贝至发送缓存 */
            Momory_Copy_Function(&localBuf[1u],
                       gs_CanInfoTXData.stCanData.aDataBuf,
                       gs_CanInfoTXData.stCanData.xLocalFFLen);
            CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_WAIT); // 标记等待硬件发送完成
            CAN_TP_RegTxMessageCB(CAN_TP_DoTxSingleFrameCB); // 注册SF发送完成回调
            /* 调用底层CAN发送接口输出单帧 */
            if (TRUE == g_CanTPInformation.transmitMessage(gs_CanInfoTXData.stCanData.xLocalId,
                                                             localLen,
                                                             localBuf,
                                                             CAN_TP_TxMsgSuccessCB,
                                                             g_CanTPInformation.maxTransmitBlockTimeMs))
            {
                CAN_SET_TXMSG_WAIT_TIME(g_CanTPInformation.senderTimeoutValue); // 设置发送超时
                *penNextState = CAN_TP_WAIT_TX_STATUS; // 等待硬件发送完成
                return CAN_TP_SUCCESS;
            }
            else
            {
                /* CAN底层发送失败，重置状态切空闲 */
                CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_FAIL);
                CAN_TP_RegTxMessageCB(NULL_PTR);
                *penNextState = CAN_TP_IDLE_STATUS;
                return CAN_TP_ERROR;
            }
        }
        else
        {
            *penNextState = CAN_TP_IDLE_STATUS;
            return CAN_TP_ERROR;
        }
    }
    else
    {
        /* 报文长度超过SF上限，切换至FF首帧发送 */
        *penNextState = CAN_TP_TX_FF_STATUS;
        return CAN_TP_ERROR;
    }
}
/**
 * @brief   FF首帧发送完成回调函数
 * @desc    功能说明：FF帧发送成功后，序列号自增，切换至等待FC流控帧状态
 */
static void CAN_TP_DoTxFirstFrameCB(void)
{
    /* 累加已发送数据长度（FF固定携带6字节载荷） */
    CAN_ADD_TX_DATA_LENGTH(FF_MIN_DATA_LENGTH - 2);
    /* 设置等待FC帧超时时间 */
    CAN_SET_RXMSG_WAIT_TIME(g_CanTPInformation.senderBufferThreshold);
    CAN_ADD_TX_SN(); // CF起始序列号+1
    CAN_SET_CUR_STATUS(CAN_TP_RX_FC_STATUS); // 切换状态等待接收FC
}
/**
 * @brief   FF首帧发送状态处理函数
 * @desc    功能说明：长报文多帧传输起始帧，组装FF帧（总长度+前6字节载荷）下发CAN
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理后TP状态
 * @retval CAN_TP_ResultType 协议处理结果码
 */
static CAN_TP_ResultType CAN_TP_DoTxFirstFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    uint8 localBuf[DATA_LENGTH] = {0u}; // FF帧发送缓存
    ASSERT(NULL_PTR == penNextState);
    /* 判断报文长度是否超出SF单帧上限，需要FF多帧 */
    switch (CAN_IS_TXDATA_LEN_OFSF())
    {
        case TRUE:
        {
            /* 填充FF帧类型标识 */
            (void)CAN_TP_SetTransmitFrameType(FF, &localBuf[0u]);
            /* 将报文总长度填入FF帧头部 */
            CAB_SET_TXFF_DATA_LENGTH(localBuf, gs_CanInfoTXData.stCanData.xLocalFFLen);
            CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_WAIT); // 标记等待硬件发送完成
            CAN_TP_RegTxMessageCB(CAN_TP_DoTxFirstFrameCB); // 注册FF发送回调
            /* 拷贝前6字节载荷到FF发送缓存 */
            Momory_Copy_Function(&localBuf[2u], gs_CanInfoTXData.stCanData.aDataBuf, FF_MIN_DATA_LENGTH - 2);
            /* 底层CAN发送FF首帧 */
            if (TRUE == g_CanTPInformation.transmitMessage(gs_CanInfoTXData.stCanData.xLocalId,
                                                             sizeof(localBuf),
                                                             localBuf,
                                                             CAN_TP_TxMsgSuccessCB,
                                                             g_CanTPInformation.maxTransmitBlockTimeMs))
            {
                CAN_SET_TXMSG_WAIT_TIME(g_CanTPInformation.senderTimeoutValue); // 设置发送超时
                *penNextState = CAN_TP_WAIT_TX_STATUS; // 等待硬件发送完成
                return CAN_TP_SUCCESS;
            }
            else
            {
                /* FF发送失败，重置状态切空闲 */
                CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_FAIL);
                CAN_TP_RegTxMessageCB(NULL_PTR);
                *penNextState = CAN_TP_IDLE_STATUS;
                return CAN_TP_ERROR;
            }
            break;
        }
        case FALSE:
        default:
        {
            /* 报文长度可用SF发送，切换至单帧发送状态 */
            *penNextState = CAN_TP_TX_SF_STATUS;
            return CAN_TP_BUFF_OVFLW;
        }
    }
}
/**
 * @brief   CF连续帧发送完成回调函数
 * @desc    功能说明：单条CF发送成功后，判断整块/整条报文是否发完，切换对应发送状态
 */
static void CAN_TP_DoTxConsecutiveFrameCB(void)
{
    /* 判断整条报文所有CF是否全部发送完成 */
    switch (CAN_ISTX_ALL())
    {
        case TRUE:
        {
            TP_TransmitSingleFrameCallback(E_TX_MSG_OK); // 全部发送完成，上报上层成功
            CAN_SET_CUR_STATUS(CAN_TP_IDLE_STATUS); // 切空闲
            return;
        }
        default:
        {
            /* 更新CF最小发送间隔STmin计时 */
            CAN_SET_TX_STMIN();
            if (gs_CanInfoTXData.ucBlkSize)
            {
                gs_CanInfoTXData.ucBlkSize--; // 当前块剩余CF数量-1
                if (0u == gs_CanInfoTXData.ucBlkSize)
                {
                    CAN_SET_CUR_STATUS(CAN_TP_RX_FC_STATUS);
                    CAN_SET_RXMSG_WAIT_TIME(g_CanTPInformation.senderBufferThreshold);
                    return;
                }
            }
            CAN_ADD_TX_SN(); // CF序列号自增
            CAN_SET_RXMSG_WAIT_TIME(g_CanTPInformation.senderConsecutiveLimit); // 块内CF发送超时
            CAN_SET_CUR_STATUS(CAN_TP_TX_CF_STATUS); // 继续发送下一条CF
        }
        break;
    }
}
/**
 * @brief   CF连续帧发送状态处理函数
 * @desc    功能说明：多帧传输连续CF帧组装发送，受FC下发BS、STmin控制发送节奏
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理后TP状态
 * @retval CAN_TP_ResultType 协议处理结果码
 */
static CAN_TP_ResultType CAN_TP_DoTxConsecutiveFrame(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    uint8 localBuf[DATA_LENGTH] = {0u}; // CF帧发送缓存
    uint8 localLen = 0u; // 当前CF有效载荷长度
    uint8 localALLLen = 0u; // CF帧完整发送字节
    
    ASSERT(NULL_PTR == penNextState);
    /* 判断FC下发的STmin最小帧间隔是否超时，未到间隔禁止发CF */
    if (FALSE != CAN_ISTX_STMIN_TIMEOUT())
    {
        /* 判断整块CF发送是否超时 */
        if (TRUE != CAN_ISTX_WAIT_TIMEOUT())
        {
            /* 填充CF帧类型标识+SN序列号 */
            (void)CAN_TP_SetTransmitFrameType(CF, &localBuf[0u]);
            CAN_SET_TX_SN(&localBuf[0u]);
            localLen = gs_CanInfoTXData.stCanData.xLocalFFLen - gs_CanInfoTXData.stCanData.xLocalPduLen;
            /* CAN TP set TX message status and register TX message successful callback. */
            CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_WAIT);
            CAN_TP_RegTxMessageCB(CAN_TP_DoTxConsecutiveFrameCB);

            if (localLen >= CF_MAX_DATA_LENGTH)
            {
                /* 拷贝7字节载荷到CF缓存 */
                Momory_Copy_Function(&localBuf[1u],
                           &gs_CanInfoTXData.stCanData.aDataBuf[gs_CanInfoTXData.stCanData.xLocalPduLen],
                           CF_MAX_DATA_LENGTH);

                /* Request transmitted application message. */
                if (TRUE == g_CanTPInformation.transmitMessage(gs_CanInfoTXData.stCanData.xLocalId,
                                                                 sizeof(localBuf),
                                                                 localBuf,
                                                                 CAN_TP_TxMsgSuccessCB,
                                                                 g_CanTPInformation.maxTransmitBlockTimeMs))
                {
                    CAN_ADD_TX_DATA_LENGTH(CF_MAX_DATA_LENGTH); // 累加已发送字节
                }
                else
                {
                    /* CF发送失败，重置状态切空闲 */
                    CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_FAIL);
                    CAN_TP_RegTxMessageCB(NULL_PTR);
                    *penNextState = CAN_TP_IDLE_STATUS;
                    return CAN_TP_ERROR;
                }
            }
            else
            {
                /* 剩余不足7字节，拷贝全部剩余载荷 */
                Momory_Copy_Function(&localBuf[1u],
                           &gs_CanInfoTXData.stCanData.aDataBuf[gs_CanInfoTXData.stCanData.xLocalPduLen],
                           localLen);
                localALLLen = localLen + 1u;

                /* Request transmitted application message. */
                if (TRUE == g_CanTPInformation.transmitMessage(gs_CanInfoTXData.stCanData.xLocalId,
                                                                 localALLLen,
                                                                 localBuf,
                                                                 CAN_TP_TxMsgSuccessCB,
                                                                 g_CanTPInformation.maxTransmitBlockTimeMs))
                {
                    CAN_ADD_TX_DATA_LENGTH(localLen); // 累加剩余发送字节
                }
                else
                {
                    /* CF发送失败 */
                    CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_FAIL);
                    CAN_TP_RegTxMessageCB(NULL_PTR);
                    *penNextState = CAN_TP_IDLE_STATUS;
                    return CAN_TP_ERROR;
                }
            }
            CAN_SET_TXMSG_WAIT_TIME(g_CanTPInformation.senderTimeoutValue); // 设置发送超时
            *penNextState = CAN_TP_WAIT_TX_STATUS; // 等待CAN硬件发送完成
            return CAN_TP_SUCCESS;
        }
        else
        {
            /* 整块CF发送超时，终止传输切空闲 */
            *penNextState = CAN_TP_IDLE_STATUS;
            return CAN_TP_TIMEOUT_BS;
        }
    }
    else
    {
        /* STmin间隔未到，暂不发送CF，保持当前状态 */
        return CAN_TP_SUCCESS;
    }
}
/**
 * @brief   等待CAN硬件发送完成状态处理函数
 * @desc    功能说明：发送SF/FF/CF/FC后进入该状态，检测发送超时，超时则取消发送并上报上层失败
 * @param pstLocalMsg 底层CAN接收报文缓存
 * @param penNextState 出参：处理后TP状态
 * @retval CAN_TP_ResultType 协议处理结果码
 */
static CAN_TP_ResultType CAN_TP_DoWaitTxMessage(CAN_TP_MsgType *pstLocalMsg, CAN_TP_WorkStatusType *penNextState)
{
    /* 判断硬件发送等待是否超时 */
    if (TRUE != CAN_ISTXMSG_WAIT_TIMEOUT())
    {
        /* 未超时，无操作继续等待 */
    }
    else
    {
        /* 发送超时，若底层支持取消报文则调用取消发送接口 */
        if (NULL_PTR == g_CanTPInformation.cancelTransmission)
        {
            /* 无底层取消接口，跳过 */
        }
        else
        {
            (g_CanTPInformation.cancelTransmission) ();
        }
        /* 向上层UDS上报发送超时错误 */
        TP_TransmitSingleFrameCallback(E_TX_MSG_TIMEOUT);
        CAN_TP_SetTxMessageState(CAN_TP_TXMSG_STATUS_FAIL); // 标记发送失败
        CAN_TP_RegTxMessageCB(NULL_PTR); // 清空发送回调
        *penNextState = CAN_TP_IDLE_STATUS; // 切空闲终止本次传输
    }
    return CAN_TP_SUCCESS;
}
/**
 * @brief   组装TP帧头部类型（SF/FF/CF/FC）
 * @desc    功能说明：将帧类型编码写入帧首字节高4位
 * @param localFrameType 帧类型枚举 SF/FF/CF/FC
 * @param pucOutFrameType 入出参：帧首字节缓存指针
 * @retval uint8 TRUE=类型合法写入成功；FALSE=非法帧类型
 */
static uint8 CAN_TP_SetTransmitFrameType(const CAN_TP_FrameType localFrameType,
                                         uint8 *pucOutFrameType)
{
    ASSERT(NULL_PTR == pucOutFrameType);
    /* 校验帧类型仅支持四种标准TP帧 */
    if (SF != localFrameType &&
        FF != localFrameType &&
        FC != localFrameType &&
        CF != localFrameType)
    {
        return FALSE;
    }
    else
    {
        *pucOutFrameType &= 0x0Fu; // 清空高4位
        *pucOutFrameType |= ((uint8)localFrameType << 4u); // 帧类型写入高4位
        return TRUE;
    }
}
/**
 * @brief   CAN底层报文发送成功回调
 * @desc    功能说明：硬件CAN发送中断触发，标记发送状态为成功，主循环执行对应上层回调
 */
static void CAN_TP_TxMsgSuccessCB(void)
{
    gs_CanTXMsgStatus = CAN_TP_TXMSG_STATUS_SUCCESS;
}
/**
 * @brief   设置TP全局发送任务状态（空闲/等待发送/发送失败/发送成功）
 * @param txMessageType 发送状态枚举
 */
static void CAN_TP_SetTxMessageState(const CAN_TP_TxMsgStatusType txMessageState)
{
    gs_CanTXMsgStatus = txMessageState;
}
/**
 * @brief   注册报文发送完成全局回调
 * @param regTxMegCB 上层传入回调函数指针，NULL清空
 */
static void CAN_TP_RegTxMessageCB(const TxCompletionCallback regTxMegCB)
{
    gs_CanTXMsgCB = regTxMegCB;
}
/**
 * @brief   执行已注册的发送完成回调
 * @desc    功能说明：主循环轮询调用，发送成功/失败后触发上层回调通知UDS
 */
static void CAN_TP_DoRegTxMessageCB(void)
{
    CAN_TP_TxMsgStatusType localStatus = CAN_TP_TXMSG_STATUS_IDLE;
    /* 关中断读取发送状态，防止多任务抢占篡改变量 */
    DisableAllInterrupts();
    localStatus = gs_CanTXMsgStatus;
    EnableAllInterrupts();
    switch (localStatus)
    {
        case CAN_TP_TXMSG_STATUS_SUCCESS:
        {
            if (NULL_PTR == gs_CanTXMsgCB)
            {
                /* 无上层回调，跳过 */
            }
            else
            {
                (gs_CanTXMsgCB)(); // 执行发送成功回调
                gs_CanTXMsgCB = NULL_PTR; // 回调执行后置空，防止重复触发
            }
            break;
        }
        case CAN_TP_TXMSG_STATUS_FAIL:
        {
            //TPDebugLog("\n TX msg failed callback=%X, status=%d\n", gs_CanTXMsgCB, gs_CanTXMsgStatus);
            gs_CanTXMsgStatus = CAN_TP_TXMSG_STATUS_IDLE;
            /* If TX message failed, clear TX message callback */
            gs_CanTXMsgCB = NULL_PTR;
            break;
        }
        default:
        {
            break;
        }
    }
}
/**
 * @brief   解析接收SF单帧的有效数据长度
 * @desc    功能说明：区分标准CAN(8字节)与CANFD长帧两种格式，提取UDS有效载荷长度
 * @param inLen 收到CAN报文DLC长度
 * @param inBuf CAN报文完整数据缓存
 * @param outLen 出参：解析得到的UDS有效数据长度
 * @retval boolean TRUE=长度合法解析成功；FALSE=帧格式/长度非法
 */
static boolean CAN_TP_AnalyzeSingleFrameLen(const uint32 inLen, const uint8 *inBuf, uint32 *outLen)
{
    boolean bCheck = FALSE;
    uint32 tempLen = 0u;
    ASSERT(NULL_PTR == inBuf);
    ASSERT(NULL_PTR == outLen);
    /* 报文长度至少2字节且帧类型为SF才解析 */
    if ((inLen > 1u) && (TRUE == IS_CAN_SF(inBuf[0u])))
    {
        /* 区分CANFD长帧和标准CAN 8字节帧两种SF长度编码规则 */
        if (inLen > 8u)
        {
            tempLen = inBuf[0u] & 0x0Fu;
            if (0u != tempLen)
            {
                /* 高4位直接存储长度 */
            }
            else
            {
                /* CANFD SF高4位为0，第2字节存储16位长度 */
                tempLen = inBuf[1u];
                if ((tempLen <= SF_CANFD_DATA_MAX_LEN) && (tempLen > 0u))
                {
                    bCheck = CAN_CHECK_RXMSG_LEN_VALID(NORMAL_ADDRESSING, tempLen, inLen);
                }
            }
        }
        else
        {
            /* 标准CAN 8字节以内SF，高4位存储有效长度 */
            tempLen = inBuf[0u] & 0x0Fu;
            if ((tempLen <= SF_CAN_DATA_MAX_LEN) && (tempLen > 0u))
            {
                bCheck = CAN_CHECK_RXMSG_LEN_VALID(NORMAL_ADDRESSING, tempLen, inLen);
            }
        }
        if (TRUE == bCheck)
        {
            *outLen = tempLen; // 输出解析后的有效长度
        }
        return bCheck;
    }
    else
    {
        return FALSE;
    }
}
/**
 * @brief   解析接收FF首帧的完整报文总长度
 * @desc    功能说明：兼容标准CAN FF(2字节长度)和CANFD FF(6字节扩展长度)两种格式
 * @param inLen 收到CAN报文DLC长度
 * @param inBuf CAN报文数据缓存
 * @param outLen 出参：整条UDS报文总字节长度
 * @retval boolean TRUE=解析成功长度合法；FALSE=帧非法/长度过小
 */
static boolean CAN_TP_AnalyzeFFFrameLen(const uint32 inLen, const uint8 *inBuf, uint32 *outLen)
{
    boolean localResult  = FALSE;
    uint32 localLen = 0u;
    uint8 localIndex = 0u;
    
    ASSERT(NULL_PTR == inBuf);
    ASSERT(NULL_PTR == outLen);
    /* FF帧最小DLC为8字节，且帧类型为FF才解析 */
    if ((inLen >= 8u) && (TRUE == IS_CAN_FF(inBuf[0u])))
    {
        /* 先读取前2字节基础长度 */
        localLen = (uint32)((inBuf[0u] & 0x0Fu) << 8u) | inBuf[1u];
        if (0u != localLen)
        {
            /* 基础长度非0，标准CAN FF长度 */
        }
        else
        {
            /* 基础长度为0，CANFD扩展FF，后4字节为32位完整长度 */
            localIndex = 0u;
            while (localIndex < 4u)
            {
                localLen <<= 8u;
                localLen |= inBuf[localIndex + 2u];
                localIndex++;
            }
        }
        /* FF报文总长度必须大于6字节（FF自带6字节载荷） */
        if (localLen < FF_MIN_DATA_LENGTH)
        {
            localResult = FALSE;
        }
        else
        {
            localResult = TRUE;
        }
        if (TRUE != localResult)
        {
            /* 报文总长度过小非法 */
        }
        else
        {
            *outLen = localLen; // 输出整条报文总长度
        }
        return localResult;
    }
    else
    {
        return FALSE;
    }
}
/**
 * @brief   CAN TP协议初始化函数
 * @desc    功能说明：初始化TP收发FIFO缓冲区，分配队列内存，失败卡死调试
 */
void CAN_TP_Init(void)
{
    /* 局部错误状态码 */
    errorStateType eLocErr;
    
    /* 1. 创建UDS接收TP队列FIFO */
    OperateFifoLogic(LEN_RX_TP_QUEUE, ID_RX_TP_QUEUE, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* 队列创建成功无操作 */
    }
    else
    {
        TPDebugLog("Apply FIFO RX error!\n"); // 接收FIFO创建失败，死循环调试
        for (;;)
        {
            /* 无限循环卡死，便于调试定位 */
        }
    }
    /* 2. 创建UDS发送TP队列FIFO */
    OperateFifoLogic(LEN_TX_TP_QUEUE, ID_TX_TP_QUEUE, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* 创建成功 */
    }
    else
    {
        TPDebugLog("Apply FIFO TX error code!\n"); // 发送FIFO创建失败卡死
        for (;;)
        {
            /* 无限循环 */
        }
    }
    /* 3. 创建底层CAN硬件接收原始报文FIFO */
    OperateFifoLogic(RECEIVE_BUS_FIFO_SIZE, RECEIVE_BUS_FIFO_CHAR, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* 创建成功 */
    }
    else
    {
        TPDebugLog("Apply RX FIFO from BUS error code!\n"); // CAN接收FIFO失败卡死
        for (;;)
        {
            /* 无限循环 */
        }
    }
    /* 4. 创建底层CAN硬件发送原始报文FIFO */
    OperateFifoLogic(TRANSMIT_BUS_FIFO_SIZE, TRANSMIT_BUS_FIFO_CHAR, &eLocErr);
    if (STATE_NO_ERROR == eLocErr)
    {
        /* 创建成功 */
    }
    else
    {
        TPDebugLog("Apply TX FIFO TO BUS error code!\n"); // CAN发送FIFO失败卡死
        for (;;)
        {
            /* 无限循环 */
        }
    }
}
/**
 * @brief   TP系统1ms周期调度函数
 * @desc    功能说明：每1ms调用一次，递减所有STmin、收发超时计数器
 */
void CAN_TP_SysTickController(void)
{
    /* 接收侧CF最小发送间隔计时器递减 */
    if (0 == gs_CanInfoRXData.stMinSepTime)
    {
    }
    else
    {
        gs_CanInfoRXData.stMinSepTime--;
    }
    /* 接收侧报文整体超时计时器递减 */
    if (0 == gs_CanInfoRXData.stMaxTimeoutTime)
    {
    }
    else
    {
        gs_CanInfoRXData.stMaxTimeoutTime--;
    }
    /* 发送侧CF最小间隔计时器递减 */
    if (0 == gs_CanInfoTXData.stMinSepTime)
    {
    }
    else
    {
        gs_CanInfoTXData.stMinSepTime--;
    }
    /* 发送侧整块传输超时计时器递减 */
    if (0 == gs_CanInfoTXData.stMaxTimeoutTime)
    {
    }
    else
    {
        gs_CanInfoTXData.stMaxTimeoutTime--;
    }
    /* 硬件发送等待计时器递减 */
    if (0 == gs_CanTXMsgWait)
    {
    }
    else
    {
        gs_CanTXMsgWait--;
    }
}
/**
 * @brief   CAN TP协议主循环处理函数
 * @desc    功能说明：TP顶层入口，周期调用；读取CAN硬件报文、状态机匹配、执行对应收发处理逻辑
 */
void CAN_TP_MainFunction(void)
{
    /* 循环索引变量 */
    uint8 ucIdx = 0u;
    /* 状态机映射表总条目数 */
    const uint8 ucFindCount = sizeof(gs_astCANTpFunInfo) / sizeof(gs_astCANTpFunInfo[0u]);
    /* 本地临时存储CAN硬件收到的单条报文 */
    CAN_TP_MsgType stLocalRxMsg = {TRUE, 0u, 0u, {0u}};
    /* 状态处理返回结果码 */
    CAN_TP_ResultType eLocResult = CAN_TP_SUCCESS;
    /* 若当前处于等待硬件发送完成状态，不处理接收报文 */
    if (CAN_TP_WAIT_TX_STATUS == CAN_GET_CUR_STATUS())
    {
        /* 无操作 */
    }
    else
    {
        /* 调用底层CAN接口读取硬件收到的报文 */
        if (TRUE != g_CanTPInformation.receiveMessage(&stLocalRxMsg.msgLocalId,
                                                      &stLocalRxMsg.msgLength,
                                                      stLocalRxMsg.msgLocalBuf))
        {
            /* 无新CAN报文，跳过 */
        }
        else
        {
            /* 校验报文ID为本ECU诊断ID，过滤无关CAN信号 */
            if (TRUE != VerifyRecContentId(stLocalRxMsg.msgLocalId))
            {
                /* 非诊断报文，丢弃 */
            }
            else
            {
                stLocalRxMsg.msgIsFree = FALSE; // 标记本地缓存存在有效报文
            }
        }
    }
    /* 遍历状态机映射表，匹配当前TP状态并执行对应处理函数 */
    while (ucIdx < ucFindCount)
    {
        if (CAN_GET_CUR_STATUS() != gs_astCANTpFunInfo[ucIdx].workState)
        {
            /* 当前状态不匹配，跳过本条映射 */
			//TPDebugLog("Current state does not match %d %d\r\n",CAN_GET_CUR_STATUS(),gs_astCANTpFunInfo[ucIdx].workState);
        }
        else
        {
            if (NULL_PTR == gs_astCANTpFunInfo[ucIdx].pointForFunc)
            {
                /* 状态无绑定处理函数，跳过 */
            }
            else
            {
                /* 执行当前状态对应的收发处理函数 */
				//TPDebugLog("Execute the corresponding state function,%d\r\n",gs_astCANTpFunInfo[ucIdx].workState);
                eLocResult = gs_astCANTpFunInfo[ucIdx].pointForFunc(&stLocalRxMsg, CAN_GET_CUR_STATUS_PTR());
            }
        }
        
        if (CAN_TP_UNEXP_PDU == eLocResult)
        {
            /* 收到意外报文，重置循环从头匹配状态 */
            ucIdx = 0u;
        }
        else
        {
            if (CAN_TP_SUCCESS == eLocResult)
            {
                /* 处理正常，状态无需重置 */
            }
            else
            {
                /* 处理报错，强制切回空闲状态终止本次传输 */
                CAN_SET_CUR_STATUS(CAN_TP_IDLE_STATUS);
            }
            ucIdx++; // 匹配下一条状态映射
        }
    }
    /* 清空本地CAN接收缓存 */
    CAN_CLEAR_RX_BUFF(&stLocalRxMsg);
    /* 执行本次发送完成回调函数 */
    CAN_TP_DoRegTxMessageCB();
}
#endif
