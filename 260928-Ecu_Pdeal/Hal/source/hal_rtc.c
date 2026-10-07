/* Includes */
#include "hal_rtc.h"
#include "drv_rtc.h"
#include <stdbool.h>



#define HAL_RTC_WAKEUP_SEC    25U   // 唤醒闹钟时间，单位秒

static volatile bool flag_rtc_wakeup = false;

/**********************************************************
  * @brief	hal RTC 唤醒中断钩子实现
  * @param
  * @retval
  * @note		设置标志位，通知 app 唤醒中断
 **********************************************************/
static void hal_receive_rtc_irq_hook(void)
{
	flag_rtc_wakeup = true;
}

/**********************************************************
  * @brief	hal RTC 中断标志位获取
  * @param
  * @retval
  * @note		
 **********************************************************/
bool hal_rtc_get_wakeup_flag(void)
{
	return flag_rtc_wakeup;
}

/**********************************************************
  * @brief	hal RTC 中断标志位清除
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_rtc_clear_wakeup_flag(void)
{
	flag_rtc_wakeup = false;
}

/**********************************************************
  * @brief	hal RTC 初始化
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_rtc_init(void)
{
	drv_register_rtc_irq_hook(hal_receive_rtc_irq_hook);
	drv_rtc_init();
	drv_rtc_config_alarm(HAL_RTC_WAKEUP_SEC);
}


/**********************************************************
  * @brief	hal 复位 RTC 日历
  * @param
  * @retval
  * @note		
 **********************************************************/
void hal_rtc_reset_time(void)
{
	drv_rtc_reset_time();
}


/**********************************************************
  * @brief	hal 使能 10s 唤醒闹钟
  * @param
  * @retval
  * @note		进低功耗前由 app 调用
 **********************************************************/
void hal_rtc_enable_wakeup_alarm(void)
{
	drv_rtc_enable_wakeup_alarm();
  hal_rtc_clear_wakeup_flag();
}


/**********************************************************
  * @brief	hal 关闭 10s 唤醒闹钟
  * @param
  * @retval
  * @note		低功耗结束后由 app 调用
 **********************************************************/
void hal_rtc_disable_wakeup_alarm(void)
{
	drv_rtc_disable_wakeup_alarm();
  hal_rtc_clear_wakeup_flag();
}


// /**********************************************************
//   * @brief	hal 层 RTC 钩子注册
//   * @param
//   * @retval
//   * @note		转发 drv 钩子，HAL 不再二次缓存
//  **********************************************************/
// void hal_register_rtc_irq_hook(Hal_Rtc_IrqHookFunc_t hook_func)
// {
// 	drv_register_rtc_irq_hook((Drv_Rtc_IrqHookFunc_t)hook_func);
// }

