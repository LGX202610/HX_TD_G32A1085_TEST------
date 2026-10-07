#ifndef UDS_CAN_IF_H
#define UDS_CAN_IF_H

#include <stdint.h>

/* 从 TP 发送 FIFO 取出一帧，调用 APP 的 app_can_tx_fram 发出 */
void uds_can_tx_poll(void);

#endif
