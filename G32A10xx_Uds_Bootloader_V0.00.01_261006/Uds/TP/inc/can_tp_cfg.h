#ifndef CAN_TP_CFG_H_  // 头文件保护宏：防止重复包含
#define CAN_TP_CFG_H_  // 定义头文件唯一标识
#include "includes.h"  // 包含工程公共头文件
#ifdef ALLOW_CAN_TP   // 条件编译：仅启用CAN TP时编译以下内容
#include "TP_cfg.h"   // 包含传输层通用配置头文件

/**************************************************************************
                    MACRO DEFINITION  // 宏定义区域
**************************************************************************/
#define NORMAL_ADDRESSING (0u)  // 普通寻址模式（单字节地址）
#define MIXED_ADDRESSING  (1u)  // 混合寻址模式（双字节地址）
#define CF_MAX_DATA_LENGTH      (7u)   // 经典CAN连续帧最大有效数据字节数
#define TX_SF_DATA_MAX_LEN      (7u)   // 发送单帧最大有效数据字节数
#define SF_CAN_DATA_MAX_LEN     (7u)   // 经典CAN单帧最大有效数据字节数
#define FF_MIN_DATA_LENGTH      (8u)   // 首帧最小有效数据字节数
#define DATA_LENGTH             (8u)   // 经典CAN帧数据域总长度（8字节）
#define CAN_TP_PADDING_BYTE     (0x55u) /* 诊断帧不足 DLC=8 时填充 */
#define SF_CANFD_DATA_MAX_LEN   (62u)  // CAN FD单帧最大有效数据字节数
#define MAX_CF_DATA_LENGTH      (UDS_MAX_PDU_LEN) // ISO-TP拼接缓冲：需容纳完整UDS PDU（≥1326）

/**************************************************************************
                    OTHER TYPE DEFINITION  // 类型重定义区域
**************************************************************************/
typedef uint32 TP_UdsIdType;          // UDS CAN ID类型，32位无符号
typedef uint32 TP_LengthType;         // 数据长度类型，32位无符号
typedef uint16 TP_TimingType;         // 计时类型，16位无符号
typedef uint16 TP_BlockSizeType;      // 块大小类型，16位无符号
typedef void (*TxCompletionCallback)(void); // 发送完成回调函数类型，无入参无返回
// 发送消息处理函数类型：入参=ID、长度、数据指针、回调、超时；返回执行结果
typedef uint8 (*TxMessageHandler)(const TP_UdsIdType, const uint16, const uint8 *, const TxCompletionCallback, const uint32);
// 接收消息处理函数类型：出参=ID、长度、数据指针；返回执行结果
typedef uint8 (*RxMessageHandler)(TP_UdsIdType *, uint8 *, uint8 *);
typedef uint16 tCanTpDataLen;         // CAN TP数据长度类型，16位无符号
typedef void (*AbortTransmissionHandler)(void); // 中止传输回调函数类型

/**************************************************************************
                    STRUCT DEFINITION  // 结构体定义区域
**************************************************************************/
// CAN TP网络层配置参数结构体
typedef struct
{
    uint8 executionInterval;               // 主函数调度执行周期（单位ms）
    TP_UdsIdType functionalRxIdentifier;   // 功能寻址接收CAN ID
    TP_UdsIdType physicalRxIdentifier;     // 物理寻址接收CAN ID
    TP_UdsIdType transmissionIdentifier;   // 发送报文CAN ID
    TP_BlockSizeType blockLimit;           // 流控块大小限制（每块连续帧数量）
    TP_TimingType stMinSepTime;            // 最小帧间隔时间STmin（单位ms）
    TP_TimingType senderTimeoutValue;      // 发送方超时时间
    TP_TimingType receiverTimeoutValue;    // 接收方超时时间
    TP_TimingType senderBufferThreshold;   // 发送方缓冲区阈值
    TP_TimingType receiverBufferThreshold; // 接收方缓冲区阈值
    TP_TimingType senderConsecutiveLimit;  // 发送方连续帧数量限制
    TP_TimingType receiverConsecutiveLimit;// 接收方连续帧数量限制
    uint32 maxTransmitBlockTimeMs;         // 最大发送块持续时间（单位ms）
    TxMessageHandler transmitMessage;      // 底层CAN发送函数指针
    RxMessageHandler receiveMessage;       // 底层CAN接收函数指针
    AbortTransmissionHandler cancelTransmission; // 中止传输函数指针
} tUdsCANNetLayerCfg;  // 定义配置结构体类型名

extern const tUdsCANNetLayerCfg g_CanTPInformation; // 声明全局配置常量（在.c中定义）

/**************************************************************************
                    FUNCTION DECLARATION  // 函数声明区域
**************************************************************************/
TP_UdsIdType GainCanTpCfgTransContentId(void);  // 获取配置的发送内容ID
TP_UdsIdType GainCanTpCfgRecContentId(void);    // 获取配置的接收内容ID
TP_UdsIdType GainCanTpCfgRecPhyId(void);        // 获取配置的物理寻址接收ID
boolean VerifyRecContentId(const uint32 recId); // 校验接收ID是否合法
void CanTpRegAbortContent(const AbortTransmissionHandler cbk); // 注册中止传输回调
boolean CanTpDrvWriteData(const uint32 id, const uint32 space, const uint8 *buf); // CAN TP驱动写数据接口
boolean CanTpDrvReadData(const uint32 readSize, uint8 *buf, TP_TransportHeaderType *head); // CAN TP驱动读数据接口
void TransSucCallBackFunc(void); // 传输成功回调函数

#endif  // 结束 ALLOW_CAN_TP 条件编译
#endif  // 结束头文件保护