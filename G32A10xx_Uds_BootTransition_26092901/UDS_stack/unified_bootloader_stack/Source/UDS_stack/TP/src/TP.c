#include "TP.h" // 传输层总头文件
#ifdef ALLOW_CAN_TP
#include "can_tp.h" // 开启CAN TP引入CAN协议接口
#endif
#ifdef ALLOW_LIN_TP
#include "LIN_TP.h" // 开启LIN TP引入LIN协议接口
#endif
#include "multi_cyc_fifo.h" // 环形FIFO缓冲区驱动

/**
 * @brief   This function initial this module
 * @desc    功能：传输层整体初始化，分别初始化CAN TP/LIN TP
 */
void TP_Init(void)
{
#ifdef ALLOW_CAN_TP
    CAN_TP_Init(); // 初始化CAN传输协议栈（FIFO、全局变量清零）
#endif
#ifdef ALLOW_LIN_TP
    LIN_TP_Init(); // 初始化LIN传输协议栈
#endif
}

/**
 * @brief   To period run the function for TP
 * @desc    功能：传输层周期主函数，调度CAN/LIN TP状态机处理
 */
void TP_MainFunction(void)
{
#ifdef ALLOW_CAN_TP
    CAN_TP_MainFunction(); // 周期执行CAN TP状态机
#endif
#ifdef ALLOW_LIN_TP
    LIN_TP_MainFunction(); // 周期执行LIN TP状态机
#endif
}

/**
 * @brief   TP system tick control
 * @desc    功能：系统滴答计时处理，所有超时计数器递减
 */
void TP_SytstemTickControl(void)
{
#ifdef ALLOW_CAN_TP
    CAN_TP_SysTickController(); // CAN TP所有超时计时递减
#endif
#ifdef ALLOW_LIN_TP
    LIN_TP_SysTickController(); // LIN TP所有超时计时递减
#endif
}

/**
 * @brief   Retrieve a single frame from the transport protocol reception buffer
 * @desc    功能：上层UDS从TP接收FIFO取出一帧完整拼接完成的报文
 */
boolean TP_DataTransferRetrieveFrame(uint32 *outputMsgIdentifier, // 出参：报文CAN/LIN ID
                                     uint32 *outputDataLength, // 出参：完整数据长度
                                     uint8 *outputDataBuffer) // 出参：完整数据缓存
{
    errorStateType operationResult; // FIFO操作错误状态
    fifoSizeType retrievedDataSize  = 0u; // FIFO单次读取字节
    TP_TransportExchangeInfoType frameInfo; // 临时存储报文交互头部
    /* Validate input parameters */
    ASSERT(NULL_PTR == outputMsgIdentifier); // ID输出指针空指针断言
    ASSERT(NULL_PTR == outputDataBuffer); // 数据缓存空指针断言
    ASSERT(NULL_PTR == outputDataLength); // 长度输出指针空指针断言
    
    /* Verify buffer availability */
    GainAccessReadSize(ID_RX_TP_QUEUE, &retrievedDataSize, &operationResult); // 获取TP接收队列可读长度
    /* Check if buffer contains sufficient metadata */
    if (STATE_NO_ERROR == operationResult && (retrievedDataSize >= sizeof(TP_TransportExchangeInfoType)))
    // FIFO正常且可读长度足够读取头部
    {
        /* Read receive ID and data len */
        GainInfoDataInFifo(ID_RX_TP_QUEUE,
                         sizeof(frameInfo),
                         (uint8 *)&frameInfo,
                         &retrievedDataSize,
                         &operationResult); // 读取报文头部ID、长度、回调
        /* Validate metadata extraction */
        if (STATE_NO_ERROR == operationResult && sizeof(frameInfo) == retrievedDataSize) // 头部读取正常
        {
            /* Read data from FIFO */
            GainInfoDataInFifo(ID_RX_TP_QUEUE,
                             frameInfo.dataLen,
                             outputDataBuffer,
                             &retrievedDataSize,
                             &operationResult); // 读取完整报文载荷
            /* Verify payload integrity */
            if (STATE_NO_ERROR == operationResult && (frameInfo.dataLen == retrievedDataSize)) // 载荷读取长度匹配
            {
                *outputMsgIdentifier = frameInfo.messageID; // 输出报文ID
                *outputDataLength = frameInfo.dataLen; // 输出有效数据长度
                return TRUE; // 上层取报文成功
            }
            else
            {
                TPDebugLog("Read data error!\n"); // 载荷读取失败打印日志
                return FALSE;
            }
        }
        else
        {
            TPDebugLog("Read data len error!\n"); // 头部读取长度不匹配日志
            return FALSE;
        }
    }
    else
    {
        return FALSE; // TP接收FIFO无完整报文
    }
}

/**
 * @brief   Write a frame data to TP TX FIFO
 * @desc    功能：上层UDS下发待发送完整报文，写入TP发送FIFO等待分包发送
 */
boolean TP_DataTransferQueueFrame(const uint32 targetMessageID, // 入参：目标发送CAN/LIN ID
                                   const tpTransportCallback txCompletionCallback, // 入参：发送完成上层回调
                                   const uint32 dataLength, // 入参：完整待发送数据长度
                                   const uint8 *dataBuffer) // 入参：待发送数据缓存
{
    errorStateType operationStatus; // FIFO操作错误状态
    fifoSizeType writeLen = 0u; // FIFO剩余可写空间
    fifoSizeType writDataLen = (fifoSizeType)dataLength; // 待写入数据字节
    TP_TransportExchangeInfoType exchangeMsgInfo; // 报文交互头部临时结构体
    uint32 totalWriteDataLen = dataLength + sizeof(TP_TransportExchangeInfoType); // 头部+数据总占用FIFO空间
    
    exchangeMsgInfo.messageID = (uint32)targetMessageID; // 填充目标报文ID
    exchangeMsgInfo.dataLen = (uint32)dataLength; // 填充完整数据长度
    exchangeMsgInfo.pfTxCall = (tpTransportCallback)txCompletionCallback; // 绑定上层发送完成回调
    
    ASSERT(NULL_PTR == dataBuffer); // 输入数据缓存空指针断言
    /* Check transmit ID */
    if (targetMessageID == TP_GetTransportTxID()) // 下发ID与配置发送ID匹配，合法报文
    {
        if (0u != writDataLen) // 待发送数据长度大于0
        {
            /* Check can write data len */
            GainAccessProgramSize(ID_TX_TP_QUEUE, &writeLen, &operationStatus); // 获取TP发送FIFO剩余可写空间
            if (STATE_NO_ERROR == operationStatus && writeLen >= totalWriteDataLen) // 空间足够存储头部+载荷
            {
                /* Write UDS transmit ID */
                ProgramToFifo(ID_TX_TP_QUEUE, (uint8 *)&exchangeMsgInfo, sizeof(TP_TransportExchangeInfoType), &operationStatus);
                // 第一步写入报文交互头部
                if (STATE_NO_ERROR == operationStatus) // 头部写入成功
                {
                    /* Write data in FIFO */
                    ProgramToFifo(ID_TX_TP_QUEUE, (uint8 *)dataBuffer, writDataLen, &operationStatus); // 第二步写入报文载荷
                    if (STATE_NO_ERROR == operationStatus)
                    {
                        return TRUE; // 报文入队发送FIFO成功
                    }
                    else
                    {
                        return FALSE; // 载荷写入FIFO失败
                    }
                }
                else
                {
                    return FALSE; // 头部写入FIFO失败
                }
            }
            else
            {
                return FALSE; // FIFO剩余空间不足，无法入队报文
            }
        }
        else
        {
            return FALSE; // 待发送数据长度为0，非法报文
        }
    }
    else
    {
        return FALSE; // 下发报文ID与配置发送ID不匹配，丢弃报文
    }
}