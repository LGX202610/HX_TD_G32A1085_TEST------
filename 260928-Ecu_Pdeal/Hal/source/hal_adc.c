
/* Includes */
#include "hal_adc.h"
#include "drv_adc.h"





/**********************************************************
  * @brief	hal ADC 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_adc_init(void)
{
	drv_adc_init();		
}


/**********************************************************
  * @brief	hal ADC 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_adc_sleep(void)
{
	drv_adc_sleep();
}


/**********************************************************
  * @brief	hal ADC 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
 void hal_adc_wakeup(void)
 {
   drv_adc_wakeup();
 }



/**********************************************************
  * @brief	获取母线电压 ADC
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint16_t hal_get_vst_adc_value(void)
{
	return drv_get_adc_buf(ADC_BUF_CH_VST);
}


