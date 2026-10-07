/* Includes */
#include "hal_buzzer.h"
#include "drv_gpio.h"



/**********************************************************
  * @brief	hal buzzer ≥ı ºªØ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_buzzer_init(void)
{
	drv_buzzer_gpio_init();
	hal_set_buzzer_state(_buzzer_off);
}


/**********************************************************
  * @brief	hal buzzer –›√ﬂ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_buzzer_sleep(void)
{
	hal_set_buzzer_state(_buzzer_off);
}


/**********************************************************
  * @brief	…Ë÷√buzzer◊¥Ã¨
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_set_buzzer_state(BuzzerState buzzer)
{
	switch (buzzer)
	{
		case _buzzer_off:
			drv_set_buzzer_pin_low();
			break;
				
		case _buzzer_on:
			drv_set_buzzer_pin_high();
			break;
				
		case _buzzer_toggle:
			drv_set_buzzer_pin_toggle();
			break;
				
		default:
			break;
	}
}


