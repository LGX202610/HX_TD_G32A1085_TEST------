#ifndef UDS_PORT_H
#define UDS_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UDS 初始化：TP/会话/版本信息，并注册 1ms 节拍 */
void uds_app_init(void);
/* UDS 主循环：超时计数、TP 状态机、诊断服务、CAN 发送轮询 */
void uds_app_main(void);
/* CAN 接收回调：物理/功能/响应 ID 共用，写入 TP 接收 FIFO */
void uds_can_rx_callback(uint32_t id, uint32_t len, uint8_t *data);
/* CAN 发送完成回调入口（当前发送成功已在 poll 里确认，此处保留给 APP 中断调用） */
void uds_can_tx_confirmation(uint8_t box, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif
