/* Includes */
#include "hal_pmu.h"
#include "drv_pmu.h"


/**********************************************************
  * @brief	进入停止模式1
  * @param
  * @retval
  * @note		
 **********************************************************/
void hal_pmu_enter_stop_1(void)
{
	drv_pmu_gpio_sleep();
	drv_pmu_enter_stop_1();
}


/**********************************************************
  * @brief	进入停止模式2
  * @param
  * @retval
  * @note		
 **********************************************************/
void hal_pmu_enter_stop_2(void)
{
	drv_pmu_gpio_sleep();
	drv_pmu_enter_stop_2();
}



/**********************************************************
  * @brief	进入待机模式
  * @param
  * @retval
  * @note		
 **********************************************************/
 void hal_pmu_enter_standby(void)
 {
	drv_pmu_gpio_sleep();
	drv_pmu_enter_standby();
 }



 /**********************************************************
  * @brief	恢复时钟
  * @param
  * @retval
  * @note		
 **********************************************************/
void hal_pmu_restore_clock(void)
{
	drv_pmu_restore_clock();
}


