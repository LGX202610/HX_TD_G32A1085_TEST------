/* Includes */
#include "g32a10xx_pmu.h"
#include "drv_pmu.h"
#include "drv_gpio.h"



/**********************************************************
  * @brief	drv enter STOP1
  * @param
  * @retval
  * @note		regulator On, WFI
 **********************************************************/
 void drv_pmu_enter_stop_1(void)
 {
   Pmu_EnterStopMode(PMU_REGULATOR_ON, PMU_STOPENTRY_WFI);
 }


/**********************************************************
  * @brief	drv enter STOP2
  * @param
  * @retval
  * @note		regulator low power, WFI
 **********************************************************/
void drv_pmu_enter_stop_2(void)
{
	Pmu_EnterStopMode(PMU_REGULATOR_LowPower, PMU_STOPENTRY_WFI);
}


/**********************************************************
  * @brief	drv enter Standy
  * @param
  * @retval
  * @note		
 **********************************************************/
 void drv_pmu_enter_standby(void)
{
  Pmu_EnterStandbyMode();
}


/**********************************************************
  * @brief	drv 恢复时钟
  * @param
  * @retval
  * @note		restore project clock
 **********************************************************/
extern void SystemClockConfig(void);
void drv_pmu_restore_clock(void)
{
	SystemClockConfig();
}



/**********************************************************
  * @brief	drv gpio休眠
  * @param
  * @retval
  * @note		gpio sleep
 **********************************************************/
void drv_pmu_gpio_sleep(void)
{
	drv_gpio_sleep();
}


