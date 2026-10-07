/* Includes */
#include "hal_iwdt.h"
#include "drv_iwdt.h"


/**********************************************************
  * @brief	hal 看门狗 初始化
  * @param
  * @retval
  * @note		上电进入运行模式短超时
 **********************************************************/
void hal_iwdt_init(void)
{
	drv_iwdt_init();
}


/**********************************************************
  * @brief	切换 IWDT 工作模式
  * @param	mode	HAL_IWDT_MODE_RUN 或 HAL_IWDT_MODE_SLEEP
  * @retval
  * @note		
 **********************************************************/
void hal_iwdt_set_mode(Hal_IwdtModeType mode)
{
	if(mode == HAL_IWDT_MODE_SLEEP)
	{
		drv_iwdt_set_sleep_mode();		// 休眠长狗
	}
	else
	{
		drv_iwdt_set_run_mode();			// 运行短狗
	}
}


/**********************************************************
  * @brief	hal 喂狗
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_iwdt_refresh(void)
{
	drv_iwdt_refresh();
}
