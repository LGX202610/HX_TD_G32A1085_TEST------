#ifndef TP_H  // 头文件保护宏：防止传输层总头文件被重复包含
#define TP_H  // 定义头文件唯一标识
#include "includes.h"  // 包含工程公共基础头文件
#include "TP_cfg.h"  // 包含传输层配置头文件

#ifdef ALLOW_CAN_TP  // 条件编译：仅当启用CAN TP功能时编译以下内容
#include "can_tp.h"  // 包含CAN传输层协议头文件
#endif
#ifdef ALLOW_LIN_TP  // 条件编译：仅当启用LIN TP功能时编译以下内容
#include "LIN_TP.h"  // 包含LIN传输层协议头文件
#endif

void TP_Init(void);  // 函数声明：传输层模块整体初始化
void TP_MainFunction(void);  // 函数声明：传输层主处理函数，需周期调度
void TP_SytstemTickControl(void);  // 函数声明：传输层系统滴答计时处理，用于超时计数
boolean TP_DataTransferRetrieveFrame(uint32 *outputMsgIdentifier,
                                     uint32 *outputDataLength,
                                     uint8 *outputDataBuffer);
// 函数声明：从传输层接收队列中取出一帧完整拼接后的上层数据
// 出参：报文ID、数据长度、数据缓存区；返回值为是否取到有效数据
boolean TP_DataTransferQueueFrame(const uint32 targetMessageID,
                                   const tpTransportCallback txCompletionCallback,
                                   const uint32 dataLength,
                                   const uint8 *dataBuffer);
// 函数声明：将上层待发送数据入队到传输层发送队列
// 入参：目标报文ID、发送完成回调、数据长度、数据指针；返回值为入队是否成功

#endif  // 结束头文件保护