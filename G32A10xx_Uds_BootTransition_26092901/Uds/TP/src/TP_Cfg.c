#include "TP_cfg.h" // 传输层通用配置头文件
#ifdef ALLOW_CAN_TP // 开启CAN TP协议时包含CAN配置
#include "can_tp_cfg.h"
#endif
#ifdef ALLOW_LIN_TP // 开启LIN TP协议时包含LIN配置
#include "LIN_tp_cfg.h"
#endif
static tpTransportCallback g_pfTransportTxCB = NULL_PTR; /* TX message callback */
// 静态全局：上层UDS下发的帧发送完成回调函数指针，初始空

/**
 * @brief   Register abort transmit message to transport layer
 * @desc    功能：向上传输层注册传输中止回调函数，分CAN/LIN分别绑定
 */
void TP_RegisterAbortTxCall(void (*i_pfAbortTxMsg)(void))
{
/* For CAN transport */
#ifdef ALLOW_CAN_TP
    CanTpRegAbortContent(i_pfAbortTxMsg); // 将中止回调传递给CAN TP层
#endif
    
/* For LIN transport */
#ifdef ALLOW_LIN_TP
    LIN_TP_SetAbortHandler((AbortTransmissionHandler)i_pfAbortTxMsg); // 将中止回调传递给LIN TP层
#endif
}

/**
 * @brief   Do transport transmit message successful callback
 * @desc    功能：统一执行底层CAN/LIN发送成功回调
 */
void TP_DoTxMessageSusCallback(void)
{
/* For CAN transport */
#ifdef ALLOW_CAN_TP
    TransSucCallBackFunc(); // 执行CAN TP发送完成回调
#endif
/* For LIN transport */
#ifdef ALLOW_LIN_TP
    LIN_TP_TriggerCompletionCallback(); // 执行LIN TP发送完成回调
#endif
}

/**
 * @brief   Get transport config TX message ID
 * @desc    功能：获取协议栈配置的发送应答CAN/LIN ID
 */
uint32 TP_GetTransportTxID(void)
{
    /* Define local variable for Tx ID */
    uint32 localTxID = 0u; // 本地临时存储发送ID
    
/* For CAN transport */
#ifdef ALLOW_CAN_TP
    localTxID = GainCanTpCfgTransContentId(); // 读取CAN TP配置发送ID
#endif
    
/* For LIN transport */
#ifdef ALLOW_LIN_TP
    localTxID = g_stUdsLINLayerCfg.transmissionIdentifier; // 读取LIN TP配置发送ID
#endif
    return localTxID; // 返回对应总线的发送ID
}

/**
 * @brief   Get transport config receive Function ID
 * @desc    功能：获取功能寻址接收ID（广播报文ID）
 */
uint32 TP_GetTransportRxFunID(void)
{
    /* Define local variable for Rx Function ID */
    uint32 localFunID = 0u; // 临时存储功能接收ID
    
/* If CAN transport is enabled */
#ifdef ALLOW_CAN_TP
    localFunID = GainCanTpCfgRecContentId(); // CAN功能接收ID
#endif
    
/* If LIN transport is enabled */
#ifdef ALLOW_LIN_TP
    localFunID = g_stUdsLINLayerCfg.functionalRxIdentifier; // LIN功能接收ID
#endif
    return localFunID;
}

/**
 * @brief   Get transport config receive Physical ID
 * @desc    功能：获取点对点物理寻址接收ID
 */
uint32 TP_GetTransportRxPhyID(void)
{
    /* Define local variable for Rx Physical ID */
    uint32 localPhyID = 0u; // 临时存储物理接收ID
    
/* If CAN transport is enabled */
#ifdef ALLOW_CAN_TP
    localPhyID = GainCanTpCfgRecPhyId(); // CAN物理接收ID
#endif
    
/* If LIN transport is enabled */
#ifdef ALLOW_LIN_TP
    localPhyID = g_stUdsLINLayerCfg.physicalRxIdentifier; // LIN物理接收ID
#endif
    return localPhyID;
}

/**
 * @brief   Register a frame transmit message callback
 * @desc    功能：注册单帧发送完成上层回调函数
 */
void TP_RegisterFrameTxCallback(const tpTransportCallback paramCB)
{
    /* Save callback pointer to global pointer */
    g_pfTransportTxCB = (tpTransportCallback)paramCB; // 保存上层回调指针至全局变量
}

/**
 * @brief   Callback after transmitting a single frame message
 * @desc    功能：底层单帧发送完成后触发上层回调入口
 */
void TP_TransmitSingleFrameCallback(const uint8 paramStatus)
{
    /* Check if callback pointer is null */
    if (NULL_PTR == g_pfTransportTxCB) // 未注册上层回调
    {
        /* nothing */
    }
    else
    {
        /* Execute callback function */
        (g_pfTransportTxCB)(paramStatus); // 执行上层回调，传入发送状态码
        g_pfTransportTxCB = NULL_PTR; // 清空回调指针，防止重复触发
    }
}

/**
 * @brief   Write data in transport layer for reading message from BUS
 * @desc    功能：底层总线硬件收到报文后，写入传输层接收缓存
 */
boolean TP_WriteDataInTransport(const uint32 paramRxId, 
                                const uint32 paramRxLen,
                                const uint8 *paramBuf)
{
    boolean retResult = FALSE; // 写入操作结果，默认失败
    
    ASSERT(NULL_PTR == paramBuf); // 输入数据缓存空指针断言
    ASSERT(0u == paramRxLen); // 接收长度为0调试断言
    
/* If CAN transport is enabled */
#ifdef ALLOW_CAN_TP
    retResult = CanTpDrvWriteData(paramRxId, paramRxLen, paramBuf); // 调用CAN TP写入接口
#endif
    
/* If LIN transport is enabled */
#ifdef ALLOW_LIN_TP
    retResult = LIN_TP_WriteProtectedData(paramBuf[0u], paramRxLen - 1u, &paramBuf[1u]); // 调用LIN TP写入接口
#endif
    
    /* Return operation result */
    return retResult; // 返回写入是否成功
}

/**
 * @brief   Driver read data from TP for TX message to BUS
 * @desc    功能：底层驱动读取传输层待发送报文，下发至CAN/LIN硬件
 */
boolean TP_ReadDataInTransport(const uint32 paramRdLen, 
                               uint8 *paramRdBuf, 
                               uint32 *paramTxId, 
                               uint32 *paramTxLen)
{
    boolean retResult = FALSE; // 读取结果标记
    TP_TransportHeaderType tempHeader; // 临时存储报文头部信息
    
    ASSERT(0u == paramRdLen); // 读取长度0调试断言
    ASSERT(NULL_PTR == paramRdBuf); // 输出数据缓存空指针断言
    ASSERT(NULL_PTR == paramTxId); // 输出ID指针空指针断言
    ASSERT(NULL_PTR == paramTxLen); // 输出长度指针空指针断言
    
/* If LIN transport is enabled */
#ifdef ALLOW_LIN_TP
    retResult = LIN_TP_ReadProtectedDriverData(paramRdLen, 
                                               paramRdBuf, 
                                               &tempHeader); // LIN TP读取报文
#endif
    
/* If CAN transport is enabled */
#ifdef ALLOW_CAN_TP
    retResult = CanTpDrvReadData(paramRdLen, 
                                              paramRdBuf, 
                                              &tempHeader); // CAN TP读取报文
#endif
    if (TRUE != retResult) // 报文读取失败，直接返回
    {
        /* nothing */
    }
    else // 读取成功，输出ID与长度给底层驱动
    {
        /* Save Tx message ID and length if read succeeds */
        *paramTxId = tempHeader.txMessageID;
        *paramTxLen = tempHeader.txMessageLen;
    }
    return retResult; // 返回读取操作结果
}