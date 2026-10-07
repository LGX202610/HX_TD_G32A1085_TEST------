/* Includes */
#include "hal_timer.h"
#include "drv_tmr_base.h"




/**********************************************************
  * @brief	hal 定时器 初始化
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_timer_init(void)
{
  drv_tmr6_base_init();
}
 
 
/**********************************************************
  * @brief	hal 定时器 休眠
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_timer_sleep(void)
{
  drv_tmr6_base_sleep();
}
 
 
/**********************************************************
  * @brief	hal 定时器 唤醒
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_timer_wakeup(void)
{
  drv_tmr6_base_wakeup();
}

 



/**********************************************************
  * @brief	hal层 定时器 钩子注册
  * @param
  * @retval
  * @note		转发 TMR6 驱动层钩子，HAL 不再二次缓存
 **********************************************************/
void hal_register_timer_irq_hook(Hal_Timer_IrqHookFunc_t hook_func)
{
	drv_register_tmr6_irq_hook((Drv_Tmr6_IrqHookFunc_t)hook_func);
}



