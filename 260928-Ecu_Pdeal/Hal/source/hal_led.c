/* Includes */
#include "hal_led.h"
#include "drv_gpio.h"





/**********************************************************
  * @brief	LED ≥ı ºªØ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_led_init(void)
{
	drv_led_gpio_init();	
}


/**********************************************************
  * @brief	LED –›√ﬂ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_led_sleep(void)
{
	hal_set_led1_state(_led_off);
	hal_set_led2_state(_led_off);
}


/**********************************************************
  * @brief	LED ªΩ–—
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_led_wakeup(void)
{
	hal_set_led1_state(_led_on);
	hal_set_led2_state(_led_on);
}


/**********************************************************
  * @brief	…Ë÷√LED1◊¥Ã¨
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_set_led1_state(LedState led)
{
	switch (led)
	{
		case _led_off:
			drv_set_led1_pin_high();
			break;
				
		case _led_on:
			drv_set_led1_pin_low();
			break;
				
		case _led_toggle:
			drv_set_led1_pin_toggle();
			break;
				
		default:
			break;
	}
}


/**********************************************************
  * @brief	…Ë÷√LED2◊¥Ã¨
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_set_led2_state(LedState led)
{
	switch (led)
	{
		case _led_off:
			drv_set_led2_pin_high();
			break;
				
		case _led_on:
			drv_set_led2_pin_low();
			break;
				
		case _led_toggle:
			drv_set_led2_pin_toggle();
			break;
				
		default:
			break;
	}
}


