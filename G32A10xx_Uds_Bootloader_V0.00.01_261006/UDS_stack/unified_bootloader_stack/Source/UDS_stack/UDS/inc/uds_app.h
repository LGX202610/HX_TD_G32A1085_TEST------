#ifndef UDS_APP_H
#define UDS_APP_H
#include "uds_app_cfg.h"
// UDS消息重传计数全局变量，boot下载分段重传逻辑使用
extern uint8 g_udsMsgRetransCount; 

/**
 * @brief UDS主循环调度函数，放在1ms/周期任务中轮询处理UDS诊断报文收发、服务解析
 */
void UDS_MainFunction(void);
/**
 * @brief UDS模块初始化，上电/复位时调用，初始化延时跳转、会话、下载相关全局变量
 */
void UDS_Init(void);
#endif
