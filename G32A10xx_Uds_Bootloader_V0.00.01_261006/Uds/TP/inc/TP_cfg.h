#ifndef TP_CFG_H  // 头文件保护宏：防止该配置头文件被重复包含
#define TP_CFG_H  // 定义头文件唯一标识
#include "includes.h"  // 包含工程公共基础头文件

/* Max length of a single frame message */  // 单帧消息最大长度定义
#define DEF_MAX_FRAME_LEN (64u)  // 传输层单帧最大数据长度64字节
/* 完整UDS PDU最大长度：0x31 0x01 0x60 0x00 + 1322字节加密文件 = 1326，向上取整留余量 */
#define UDS_MAX_PDU_LEN (1400u)
/* Macro for Rx queue identifier */  // 接收队列标识宏定义
#define ID_RX_TP_QUEUE ('R')  // 接收队列的标识字符，用于区分队列类型
/* Macro for Tx queue identifier */  // 发送队列标识宏定义
#define ID_TX_TP_QUEUE ('T')  // 发送队列的标识字符，用于区分队列类型
/* Define queue lengths for TX/RX（单位：字节，含TP交互头） */
#define LEN_TX_TP_QUEUE (50u)  // 发送队列容量（短响应足够）
#define LEN_RX_TP_QUEUE (1500u)  // 接收队列：交互头 + 完整UDS PDU，至少容纳1326字节

/**
 * @brief   Type definition for TX callback function  // 发送完成回调函数类型定义
 */ 
typedef void (*tpTransportCallback)(uint8);  // 传输层发送完成回调函数指针，入参为发送结果状态

/**
 * @brief   Enum for transport TX message states  // 传输层发送消息状态枚举定义
 */ 
typedef enum
{
    E_TX_MSG_OK = 0u,  // 0：消息发送成功
    E_TX_MSG_FAIL,     // 1：消息发送失败
    E_TX_MSG_TIMEOUT   // 2：消息发送超时
} TP_TransportTxStatusType;  // 定义传输层发送状态枚举类型名

/**
 * @brief   Struct for receiving message information  // 接收消息信息结构体定义
 */ 
typedef struct
{
    uint32 rxHWdataLen;                // 硬件接收到的原始数据长度
    uint32 rxIdentifier;               // 接收到的CAN/LIN报文ID
    uint8 dataBuffer[DEF_MAX_FRAME_LEN]; // 接收数据缓存数组，最大长度为单帧上限
} TP_RxFrameInfoType;  // 定义接收帧信息结构体类型名

/**
 * @brief   Struct for UDS & TP exchange info  // UDS上层与TP传输层交互信息结构体
 */ 
typedef struct
{
    uint32 messageID;               // 报文标识符（CAN ID/LIN ID）
    uint32 dataLen;                 // 待传输的完整数据总长度
    tpTransportCallback pfTxCall;   // 本次传输对应的发送完成回调函数指针
} TP_TransportExchangeInfoType;  // 定义传输交互信息结构体类型名

/**
 * @brief   Struct for TP TX message header  // 传输层发送消息头结构体
 */ 
typedef struct
{
    uint32 txMessageID;    // 发送报文ID
    uint32 txMessageLen;   // 发送数据总长度
    uint32 txMessageCB;    // 发送回调函数标识（用于队列存储回调索引）
} TP_TransportHeaderType;  // 定义传输层消息头结构体类型名

uint32 TP_GetTransportTxID(void);  // 函数声明：获取传输层配置的发送报文ID
uint32 TP_GetTransportRxFunID(void);  // 函数声明：获取传输层配置的功能寻址接收ID
uint32 TP_GetTransportRxPhyID(void);  // 函数声明：获取传输层配置的物理寻址接收ID
boolean TP_WriteDataInTransport(const uint32 paramRxId, const uint32 paramRxLen, const uint8 *paramBuf);  // 函数声明：向传输层写入接收到的底层数据
boolean TP_ReadDataInTransport(const uint32 paramRdLen,  uint8 *paramRdBuf,  uint32 *paramTxId,  uint32 *paramTxLen);  // 函数声明：从传输层读取待发送的上层数据
void TP_RegisterAbortTxCall(void (*i_pfAbortTxMsg)(void));  // 函数声明：注册传输中止回调函数
void TP_DoTxMessageSusCallback(void);  // 函数声明：执行发送挂起的回调处理
void TP_RegisterFrameTxCallback(const tpTransportCallback i_pfTxMsgCallBack);  // 函数声明：注册帧发送完成回调函数
void TP_TransmitSingleFrameCallback(const uint8 i_result);  // 函数声明：单帧发送完成回调入口，传入结果状态

#endif  // 结束头文件保护