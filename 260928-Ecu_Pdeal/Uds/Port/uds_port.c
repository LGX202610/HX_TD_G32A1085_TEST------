#include "uds_port.h"
#include "uds_can_if.h"
#include "app_timer.h"
#include "uds_app.h"
#include "TP.h"
#include "user_versions.h"
#include "uds_dtc_nvm.h"
#include "boot.h"
#include <stdio.h>

/* 1ms 中断里只置位，真正超时处理放到主循环，避免在中断里跑 TP/UDS */
static volatile uint8_t s_uds_1ms_flag = 0u;
/* 刷写后第一次主循环才补发 0x51 01，保证此时 CAN 已初始化 */
static uint8_t s_post_download_rsp_done = 0u;

/**********************************************************
  * @brief  UDS 1ms 节拍回调
  * @note   由 app_timer 在 1ms 中断中调用，仅置标志
 **********************************************************/
static void uds_1ms_tick(void)
{
    s_uds_1ms_flag = 1u;
}

/**********************************************************
  * @brief  APP 侧 UDS 初始化
  * @note   必须在 CAN 使能前调用，保证 FIFO 已建好再收诊断报文
 **********************************************************/
void uds_app_init(void)
{
    TP_Init();
    UDS_Init();
    Did_Info_Init();
    FaultInfo_Init();
    (void)app_register_timer_callback(uds_1ms_tick, TIMER_1MS_CALLBACK_TYPE);
}

/**********************************************************
  * @brief  APP 侧 UDS 主任务
  * @note   放在 while(1) 中周期调用；有 1ms 标志时才走超时计数
 **********************************************************/
void uds_app_main(void)
{

    /* CAN 已在 main 里初始化完成后才会进到这里，再补发刷写后的 51 01 */
    if (0u == s_post_download_rsp_done)
    {
        s_post_download_rsp_done = 1u;
        (void)VerifyAppStatusAfterDownlaod();
    }

    if (s_uds_1ms_flag != 0u)
    {
        s_uds_1ms_flag = 0u;
        TP_SytstemTickControl();
        UDS_APP_HandleSystemTicks();
    }

    TP_MainFunction();
    UDS_MainFunction();
    uds_can_tx_poll();
}
