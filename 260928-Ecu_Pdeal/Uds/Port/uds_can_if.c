#include "uds_port.h"
#include "uds_can_if.h"
#include "app_canfd.h"
#include "TP.h"
#include "user_config.h"
#include "hal_canfd.h"
/**********************************************************
  * @brief  UDS CAN 接收回调
  * @param  id   CAN ID（物理 RECEIVE_ADDR / 功能 RECEIVE_FUN，扩展帧）
  * @param  len  数据长度，经典 CAN 最大 8
  * @param  data 数据指针
  * @note   由 HAL 接收中断回调，写入 TP 接收总线 FIFO
 **********************************************************/
void uds_can_rx_callback(uint32_t id, uint32_t len, uint8_t *data)
{
    if ((data == NULL) || (len == 0u) || (len > 8u))
    {
        return;
    }

    (void)CanTpDrvWriteData(id, len, data);
}

/**********************************************************
  * @brief  UDS CAN 发送完成通知
  * @param  box 发送邮箱号
  * @param  id  发送 CAN ID
  * @note   发送成功确认已在 uds_can_tx_poll 中完成，避免重复回调
 **********************************************************/
void uds_can_tx_confirmation(uint8_t box, uint32_t id)
{
    (void)box;
    (void)id;
}

/**********************************************************
  * @brief  UDS CAN 发送轮询
  * @note   占用 CAN_TX_BUF_ID_1，与 CanNm 的邮箱 0 错开；
  *         邮箱忙则本周期直接返回，下一圈再发，避免空转 2000 次堵主循环
 **********************************************************/
void uds_can_tx_poll(void)
{
    static uint8_t s_tx_pending = 0u;
    static uint8_t s_tx_buf[8];
    static uint32_t s_tx_id = 0u;
    static uint32_t s_tx_len = 0u;

    if (0u == s_tx_pending)
    {
        if (TRUE != TP_ReadDataInTransport(8u, s_tx_buf, &s_tx_id, &s_tx_len))
        {
            return;
        }
        if (s_tx_len > 8u)
        {
            s_tx_len = 8u;
        }
        s_tx_pending = 1u;
    }

    if (1 == app_can_tx_fram(CAN_TX_BUF_ID_1, s_tx_id, CAN_FRAME_TYPE_EXT, s_tx_len, s_tx_buf))
    {
        s_tx_pending = 0u;
        TP_DoTxMessageSusCallback();
    }
}
