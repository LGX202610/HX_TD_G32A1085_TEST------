/* Includes */
#include "app_usart.h"
#include "hal_usart.h"


#define LOG_HARDWARE_VARSION		"260827-DT17.0\r\n"
#define LOG_SOFTWART_VARSION		"260914-LP-CU25\r\n"


/**********************************************************
  * @brief	 USART 初始化
  * @param
  * @retval
  * @note
 **********************************************************/
void app_usart_init(void)
{
	hal_usart1_init();
  app_usart_print_version();
}

 /**********************************************************
  * @brief	版本号打印
  * @param
  * @retval
  * @note
 **********************************************************/
void app_usart_print_version(void)
{
  hal_usart_log((const uint8_t *)LOG_HARDWARE_VARSION);
	hal_usart_log((const uint8_t *)LOG_SOFTWART_VARSION);
}
