/* Includes */
#include "hal_usart.h"
#include "drv_usart.h"




#define LOG_SOFTWART_VARSION		"20260904\r\n"

/**********************************************************
  * @brief	hal USART 初始化
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_usart1_init(void)
{
	drv_usart1_init();
	
	hal_usart_log((const uint8_t *)LOG_SOFTWART_VARSION);
	hal_usart_log((const uint8_t *)LOG_SOFTWART_VARSION);
}


/**********************************************************
  * @brief	hal USART 休眠
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_usart1_sleep(void)
{
  drv_usart1_sleep();
}


/**********************************************************
  * @brief	hal USART 唤醒
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_usart1_wakeup(void)
{
  drv_usart1_wakeup();
}



 /**********************************************************
  * @brief	串口日志
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_usart_log(const uint8_t *dat)
{
	drv_usart1_write(dat);
}
