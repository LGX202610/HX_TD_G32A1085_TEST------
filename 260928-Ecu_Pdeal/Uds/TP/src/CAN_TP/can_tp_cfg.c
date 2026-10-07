/*******************************************************************************
* Project Name      : CAN/LIN Protocol Stack  // 项目名称：CAN/LIN UDS传输协议栈
* Platform          : Arm                     // 硬件平台：ARM内核单片机
* Revision Number   : V1.0                    // 代码版本：V1.0
* Compiled Version  : G32A1xxx_01-June-25     // 编译适配芯片：极海G32A系列，编译日期2025.6.1
*
* Copyright (C) 2025 Geehy Semiconductor      // 版权归属：珠海极海半导体2025
*
* You may not use this file except in compliance with the GEEHY COPYRIGHT NOTICE
* (GEEHY SOFTWARE PACKAGE LICENSE).
// 使用限制：必须遵守极海软件许可协议才可使用本源码
*
* The program is only for reference, which is distributed in the hope that it
* will be useful and instructional for customers to develop their software.
* Unless required by applicable law or agreed to in writing, the program is
* distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY
* KIND, either express or implied. See the GEEHY SOFTWARE PACKAGE LICENSE for
* the governing permissions and limitations under the License.
// 说明：源码仅作参考，无任何担保，商用需阅读完整许可协议
*
*******************************************************************************/
#include "includes.h" // 包含工程全局公共头文件（基础类型、中断、调试打印等）
#ifdef ALLOW_CAN_TP // 条件编译：开启CAN传输层协议才编译本文件全部内容
#include "can_tp_cfg.h" // 包含CAN TP配置头文件，定义类型、宏、接口
#include "multi_cyc_fifo.h" // 多周期环形FIFO驱动头文件，收发报文缓冲区依赖

/**************************************************************************
                    GLOBAL VARIABLE  // 全局静态变量定义区
**************************************************************************/
static TxCompletionCallback g_TransmitSucFunc = NULL_PTR; // 静态全局：底层CAN发送成功回调函数指针，初始空
static AbortTransmissionHandler gs_CanTPAbortTransmitContent = NULL_PTR; // 静态全局：传输中止回调函数指针

/**************************************************************************
                    FUNCTION DECLARATION  // 内部静态函数前置声明
**************************************************************************/
static boolean EraseTransBusFifo(void); // 清空发送总线FIFO缓冲区
static void CanTpTransmitAbortContent(void); // CAN TP传输中止处理函数
static uint8 CanTpTransmitInfo(const TP_UdsIdType oppositeId, const uint16 dataSize, const uint8 *buf,
                               const TxCompletionCallback cbf, const uint32 longesttime);
// 底层CAN报文发送接口：入参CAN ID、数据长度、数据缓存、发送完成回调、最大发送超时；返回执行状态
static uint8 CanTpReceiveInfo(TP_UdsIdType *oppositeId, uint8 *RecDataSize, uint8 *recBuf);
// 底层CAN报文接收接口：出参CAN ID、接收长度、接收数据缓存；返回读取是否成功

/**************************************************************************
                    CONSTANT DEFINITION  // 全局常量配置结构体（CAN TP核心参数）
**************************************************************************/
const tUdsCANNetLayerCfg g_CanTPInformation =
{
    0x01u,                          // executionInterval：主函数调度周期1ms
    RECEIVE_FUN,                    // functionalRxIdentifier：功能寻址接收CAN ID
    RECEIVE_ADDR,                   // physicalRxIdentifier：物理寻址接收CAN ID
    TRANSMIT_RESP,                  // transmissionIdentifier：应答发送CAN ID
    0x00u,                          // blockLimit：流控帧块大小限制，0代表不限制块
    0x01u,                          // stMinSepTime：连续帧最小间隔STmin=1ms
    0x19u,                          // senderTimeoutValue：发送方整体超时25ms
    0x19u,                          // receiverTimeoutValue：接收方整体超时25ms
    0x4Bu,                          // senderBufferThreshold：发送方缓存等待阈值75ms
    0x00u,                          // receiverBufferThreshold：接收方缓存等待阈值0ms
    0x64u,                          // senderConsecutiveLimit：发送连续帧等待上限100ms
    0x96u,                          // receiverConsecutiveLimit：接收连续帧等待上限150ms
    0x32u,                          // maxTransmitBlockTimeMs：单块报文最大发送时长50ms
    CanTpTransmitInfo,              // transmitMessage：底层发送函数绑定
    CanTpReceiveInfo,               // receiveMessage：底层接收函数绑定
    CanTpTransmitAbortContent,      // cancelTransmission：传输中止处理函数绑定
};

/**************************************************************************
                    LOCAL FUNCTION  // 内部静态函数实现
**************************************************************************/
// 函数功能：清空发送总线FIFO缓冲区
static boolean EraseTransBusFifo(void)
{
    errorStateType flag = STATE_NO_ERROR; // 定义FIFO操作错误状态，初始无错误
    eraseFifoContent(TRANSMIT_BUS_FIFO_CHAR, &flag); // 调用FIFO接口清空发送总线缓冲区
    return ((STATE_NO_ERROR == flag) ? TRUE : FALSE); // 操作无错误返回TRUE，失败返回FALSE
}

// 函数功能：CAN TP传输异常中止处理，清空缓存并执行注册的中止回调
static void CanTpTransmitAbortContent(void)
{
    boolean ret = FALSE; // 定义FIFO清空操作返回值
    TPDebugLog("CanTpTransmitAbortContent\n"); // 打印调试日志：触发传输中止
    if (NULL_PTR == gs_CanTPAbortTransmitContent) // 判断是否注册了中止回调函数
    {
        /** do nothing */ // 未注册回调，无需额外操作
    }
    else
    {
        /** operate callback function */
        (gs_CanTPAbortTransmitContent)(); // 执行上层注册的传输中止回调函数
        /** Set transmit success callback as NULL_PTR */
         g_TransmitSucFunc = NULL_PTR; // 清空发送成功回调指针，防止野调用
    }
    ret = EraseTransBusFifo(); // 调用函数清空发送总线FIFO
    if (TRUE == ret) // FIFO清空成功
    {
        /** do nothing */
    }
    else // FIFO清空失败
    {
        /** print debug log */
        TPDebugLog("CanTpTransmitAbortContent: Clear TX BUS FIFO failed!\n"); // 打印清空失败日志
    }
}

// 函数功能：底层CAN报文发送接口，将单帧数据写入发送总线FIFO
static uint8 CanTpTransmitInfo(const TP_UdsIdType oppositeId, const uint16 dataSize, const uint8 *buf,
                               const TxCompletionCallback cbf, const uint32 longesttime)
{
    errorStateType flag; // FIFO操作错误状态变量
    uint8 infoBuf[8]; /* 单帧缓存，先填 0x55 再拷有效数据 */
    Momory_Fill_Function(infoBuf, CAN_TP_PADDING_BYTE, sizeof(infoBuf));
    fifoSizeType accessDataSize = 0u; // FIFO剩余可写入空间
    TP_TransportHeaderType infoContent; // 传输层消息头结构体，存储ID、长度、回调索引
    const uint32 contentSize = sizeof(TP_TransportHeaderType) + sizeof(infoBuf); // 单条发送数据总占用长度（头+8字节数据）
    ASSERT(NULL_PTR == buf); // 入参数据缓存空指针断言，调试模式触发报错
    if (dataSize > 8u) // 数据长度超过经典CAN 8字节，非法，直接返回FALSE
    {
        return FALSE;
    }
    GainAccessProgramSize(TRANSMIT_BUS_FIFO_CHAR, &accessDataSize, &flag); // 获取发送总线FIFO剩余可写空间
    if ((STATE_NO_ERROR != flag) || (contentSize > accessDataSize)) // FIFO操作出错 / 剩余空间不足
    {
        return TRUE; // 返回TRUE代表发送失败（上层判断逻辑）
    }
    infoContent.txMessageID = oppositeId; // 填充消息头：目标CAN ID
    infoContent.txMessageLen = sizeof(infoBuf); // 填充消息头：单帧最大长度8
    infoContent.txMessageCB = (uint32)cbf; // 填充消息头：发送完成回调函数指针转存为数字
    Momory_Copy_Function(&infoBuf[0u], buf, dataSize); // 将待发送数据拷贝至临时缓存
    ProgramToFifo(TRANSMIT_BUS_FIFO_CHAR, (uint8 *)&infoContent, sizeof(TP_TransportHeaderType), &flag); // 先写入消息头到发送FIFO
    if (STATE_NO_ERROR != flag) // 消息头写入失败
    {
        eraseFifoContent(TRANSMIT_BUS_FIFO_CHAR, &flag); // 清空整条FIFO，防止脏数据
        return FALSE;
    }
    ProgramToFifo(TRANSMIT_BUS_FIFO_CHAR, (uint8 *)infoBuf, 8, &flag); // 写入8字节数据缓存
    if (STATE_NO_ERROR != flag) // 数据写入失败
    {
        eraseFifoContent(TRANSMIT_BUS_FIFO_CHAR, &flag); // 清空FIFO
        return FALSE;
    }
    return TRUE; // 报文完整写入FIFO，发送请求成功
}

// 函数功能：底层CAN报文读取接口，从接收总线FIFO取出硬件收到的CAN报文
static uint8 CanTpReceiveInfo(TP_UdsIdType *oppositeId, uint8 *RecDataSize, uint8 *recBuf)
{
    uint8 count = 0u; // 数据拷贝循环计数器
    errorStateType flag; // FIFO操作错误状态
    fifoSizeType readDataSize = 0u; // 单次读取字节长度
    fifoSizeType accessRecDataSize = 0u; // 接收FIFO剩余可读空间
    TP_RxFrameInfoType recCanInfo = {0u}; // 接收帧信息临时结构体，存储ID、长度、原始数据
    const uint32 headSize = sizeof(recCanInfo.rxIdentifier) + sizeof(recCanInfo.rxHWdataLen); // 接收帧头部长度（ID+数据长度）
    ASSERT(NULL_PTR == oppositeId); // 输出ID指针空指针断言
    ASSERT(NULL_PTR == recBuf); // 输出数据缓存空指针断言
    ASSERT(NULL_PTR == RecDataSize); // 输出长度指针空指针断言
    GainAccessReadSize(RECEIVE_BUS_FIFO_CHAR, &accessRecDataSize, &flag); // 获取接收总线FIFO可读长度
    if ((STATE_NO_ERROR == flag) && (headSize <= accessRecDataSize)) // FIFO正常且可读空间足够读取头部
    {
        GainInfoDataInFifo(RECEIVE_BUS_FIFO_CHAR, headSize, (uint8 *)&recCanInfo, &readDataSize, &flag); // 读取帧头部信息
        if(STATE_NO_ERROR == flag) // 头部读取成功
        {
            if(headSize <= accessRecDataSize) // 校验可读空间
            {
                GainInfoDataInFifo(RECEIVE_BUS_FIFO_CHAR, recCanInfo.rxHWdataLen, (uint8 *)&recCanInfo.dataBuffer, &accessRecDataSize, &flag); // 读取原始数据
                if (TRUE != VerifyRecContentId(recCanInfo.rxIdentifier)) // 校验收到的CAN ID是否为配置的接收ID
                {
                    return FALSE; // ID不匹配，丢弃报文返回失败
                }
                *oppositeId = recCanInfo.rxIdentifier; // 将接收ID输出给上层				
                *RecDataSize = recCanInfo.rxHWdataLen; // 将接收数据长度输出给上层
				//TPDebugLog("id 0x%x len %d CanTpReceiveInfo\r\n",recCanInfo.rxIdentifier,recCanInfo.rxHWdataLen);
                count = 0u;
                while(count < recCanInfo.rxHWdataLen) // 循环拷贝原始数据到上层缓存
                {
                    recBuf[count] = recCanInfo.dataBuffer[count];
                    count++;
                }
                return TRUE; // 报文读取解析成功
            }
        }
    }
    return FALSE; // 报文读取失败
}

/**************************************************************************
                    GLOBAL FUNCTION  // 对外全局接口函数，供can_tp.h/TP层调用
**************************************************************************/
// 接口功能：上层驱动写入硬件收到的CAN报文至接收总线FIFO
boolean CanTpDrvWriteData(const uint32 id, const uint32 space, const uint8 *buf)
{
    errorStateType flag; // FIFO操作错误状态
    boolean result = TRUE; // 操作结果标记，默认成功
    TP_RxFrameInfoType RecCanInfo; // 接收帧信息临时结构体
    fifoSizeType accessDataSize = 0u; // FIFO剩余可写空间
    const uint32 headSize = sizeof(RecCanInfo.rxIdentifier) + sizeof(RecCanInfo.rxHWdataLen); // 接收帧头部字节长度
    ASSERT(NULL_PTR == buf); // 输入数据缓存空指针断言
    if (space > 8u) // 经典CAN单帧数据超过8字节，非法
    {
        result = FALSE; // 写入失败
    }
    else
    {
        GainAccessProgramSize(RECEIVE_BUS_FIFO_CHAR, &accessDataSize, &flag); // 获取接收FIFO剩余可写空间
        if (STATE_NO_ERROR == flag) // FIFO状态正常
        {
            if ((space + headSize) <= accessDataSize) // 剩余空间足够存储头部+数据
            {
                RecCanInfo.rxIdentifier = id; // 填充接收CAN ID
                RecCanInfo.rxHWdataLen = space; // 填充接收数据长度
                ProgramToFifo(RECEIVE_BUS_FIFO_CHAR, (uint8 *)&RecCanInfo, headSize, &flag); // 写入帧头部
                if (STATE_NO_ERROR != flag) // 头部写入失败
                {
                    result = FALSE;
                }
                else
                {
                    ProgramToFifo(RECEIVE_BUS_FIFO_CHAR, (uint8 *)buf, RecCanInfo.rxHWdataLen, &flag); // 写入原始数据
                    if (STATE_NO_ERROR != flag) // 数据写入失败
                    {
                        result = FALSE;
                    }
                }
            }
        }
    }
    return result; // 返回写入操作结果
}

// 接口功能：获取配置中发送报文的CAN ID
TP_UdsIdType GainCanTpCfgTransContentId(void)
{
    return g_CanTPInformation.transmissionIdentifier; // 直接返回全局配置结构体中的发送ID
}

// 接口功能：底层CAN发送硬件完成回调入口
void TransSucCallBackFunc(void)
{
    if (NULL_PTR == g_TransmitSucFunc) // 未注册上层发送完成回调
    {
        /** do nothing */
    }
    else
    {
        /** operate callback function */
        (g_TransmitSucFunc)(); // 执行上层注册的发送成功回调
        /** Set transmit success callback as NULL_PTR */
        g_TransmitSucFunc = NULL_PTR; // 清空回调指针，防止重复调用
    }
}

// 接口功能：注册传输中止回调函数给CAN TP层
void CanTpRegAbortContent(const AbortTransmissionHandler cbk)
{
    gs_CanTPAbortTransmitContent = (AbortTransmissionHandler)cbk; // 保存上层传入的中止回调指针
}

// 接口功能：获取配置中功能寻址接收CAN ID
TP_UdsIdType GainCanTpCfgRecContentId(void)
{
    return g_CanTPInformation.functionalRxIdentifier; // 返回配置的功能接收ID
}

// 接口功能：获取配置中物理寻址接收CAN ID
TP_UdsIdType GainCanTpCfgRecPhyId(void)
{
    return g_CanTPInformation.physicalRxIdentifier; // 返回配置的物理接收ID
}

// 接口功能：校验收到的CAN ID是否为合法接收ID（物理/功能寻址）
boolean VerifyRecContentId(const uint32 recId)
{
    boolean ret = FALSE; // 匹配标记，默认不匹配
    ret = (recId == g_CanTPInformation.functionalRxIdentifier) ? TRUE : FALSE; // 判断是否等于功能寻址ID
    ret = (recId == g_CanTPInformation.physicalRxIdentifier) ? TRUE : ret; // 判断是否等于物理寻址ID，覆盖原有标记
    return ret; // 返回ID是否合法
}

// 接口功能：TP层读取待发送报文，从发送总线FIFO取出数据、ID、回调信息
boolean CanTpDrvReadData(const uint32 readSize, uint8 *buf, TP_TransportHeaderType *head)
{
    errorStateType flag; // FIFO操作错误状态
    boolean ret = FALSE; // 读取结果标记，默认失败
    fifoSizeType accessDataSize = 0u; // FIFO可读空间
    TP_TransportHeaderType transmitContent; // 临时存储读取到的消息头
    const uint32 infoSize = sizeof(TP_TransportHeaderType); // 消息头固定长度
    ASSERT(NULL_PTR == buf); // 输出数据缓存空指针断言
    ASSERT(NULL_PTR == head); // 输出消息头指针空指针断言
    ASSERT(0u == readSize); // 读取长度为0断言（调试校验）
    GainAccessReadSize(TRANSMIT_BUS_FIFO_CHAR, &accessDataSize, &flag); // 获取发送FIFO可读字节
    if(STATE_NO_ERROR == flag) // FIFO操作无错误
    {
        if(accessDataSize > infoSize) // 可读空间大于消息头长度，存在完整报文
        {
            GainInfoDataInFifo(TRANSMIT_BUS_FIFO_CHAR, sizeof(TP_TransportHeaderType), (uint8 *)&transmitContent, &accessDataSize, &flag); // 读取消息头
            if(STATE_NO_ERROR == flag) // 消息头读取成功
            {
                if(accessDataSize == sizeof(TP_TransportHeaderType)) // 校验读取长度匹配头大小
                {
                    ret = TRUE; // 头读取有效，标记成功
                }
            }
            if (TRUE == ret) // 头解析正常，继续读取数据
            {
                GainInfoDataInFifo(TRANSMIT_BUS_FIFO_CHAR, readSize, buf, &accessDataSize, &flag); // 读取报文数据
                if(STATE_NO_ERROR == flag) // 数据读取无错误
                {
                    if(accessDataSize == readSize) // 实际读取长度等于请求长度
                    {
                        if(readSize >= transmitContent.txMessageLen) // 读取长度满足报文数据长度
                        {
                            ret = TRUE;
                            *head = transmitContent; // 将消息头拷贝输出给上层
                            g_TransmitSucFunc = (TxCompletionCallback)transmitContent.txMessageCB; // 保存发送完成回调
                        }
                    }
                }
            }
        }
    }
    return ret; // 返回报文读取是否成功
}
#endif