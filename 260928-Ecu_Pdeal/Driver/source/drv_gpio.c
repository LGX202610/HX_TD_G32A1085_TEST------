
/* Includes */
#include "g32a10xx_gpio.h"
#include "g32a10xx_rcm.h"
#include "board.h"
#include "drv_gpio.h"



/**********************************************************
  * @brief	 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_led_gpio_init(void)
{
	Gpio_ConfigType gpioConfig;
	
	Rcm_EnableAhbPeriphClock(LED_GPIO_PER_CLOCK);
	
	gpioConfig.pin = (uint16_t)LED1_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_OUT;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	Gpio_Config(LED1_GPIO_PORT, &gpioConfig);
	
	gpioConfig.pin = (uint16_t)LED2_GPIO_PIN;
	Gpio_Config(LED2_GPIO_PORT, &gpioConfig);
}


void drv_set_led1_pin_high(void)
{
	Gpio_SetBit(LED1_GPIO_PORT, LED1_GPIO_PIN);
}
void drv_set_led1_pin_low(void)
{
	Gpio_ClearBit(LED1_GPIO_PORT, LED1_GPIO_PIN);
}
void drv_set_led1_pin_toggle(void)
{
	Gpio_Toggle(LED1_GPIO_PORT, LED1_GPIO_PIN);
}


void drv_set_led2_pin_high(void)
{
	Gpio_SetBit(LED2_GPIO_PORT, LED2_GPIO_PIN);
}
void drv_set_led2_pin_low(void)
{
	Gpio_ClearBit(LED2_GPIO_PORT, LED2_GPIO_PIN);
}
void drv_set_led2_pin_toggle(void)
{
	Gpio_Toggle(LED2_GPIO_PORT, LED2_GPIO_PIN);
}

	

/**********************************************************
  * @brief	 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_buzzer_gpio_init(void)
{
	Gpio_ConfigType gpioConfig;
	
	Rcm_EnableAhbPeriphClock(BUZZER_GPIO_PER_CLOCK);
	
	gpioConfig.pin = (uint16_t)BUZZER_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_OUT;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pupd = GPIO_PUPD_PD;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	Gpio_Config(BUZZER_GPIO_PORT, &gpioConfig);
}

void drv_set_buzzer_pin_high(void)
{
	Gpio_SetBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}
void drv_set_buzzer_pin_low(void)
{
	Gpio_ClearBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}
void drv_set_buzzer_pin_toggle(void)
{
	Gpio_Toggle(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}




/**********************************************************
  * @brief	 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_con_power_gpio_init(void)
{
	Gpio_ConfigType gpioConfig;
	
	Rcm_EnableAhbPeriphClock(OUTPUT_GPIO_PER_CLOCK);
	
	gpioConfig.pin = (uint16_t)CON_VST_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_OUT;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pupd = GPIO_PUPD_PD;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	Gpio_Config(CON_VST_GPIO_PORT, &gpioConfig);
}

void drv_set_con_vst_pin_high(void)
{
	Gpio_SetBit(CON_VST_GPIO_PORT, CON_VST_GPIO_PIN);
}
void drv_set_con_vst_pin_low(void)
{
	Gpio_ClearBit(CON_VST_GPIO_PORT, CON_VST_GPIO_PIN);
}
void drv_set_con_vst_pin_toggle(void)
{
	Gpio_Toggle(CON_VST_GPIO_PORT, CON_VST_GPIO_PIN);
}




/**********************************************************
  * @brief	 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_canstb_gpio_init(void)
{
	Gpio_ConfigType gpioConfig;
	
	Rcm_EnableAhbPeriphClock(OUTPUT_GPIO_PER_CLOCK);
	
	gpioConfig.pin = (uint16_t)CAN_STB_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_OUT;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pupd = GPIO_PUPD_PD;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	Gpio_Config(CAN_STB_GPIO_PORT, &gpioConfig);
	drv_set_canstb_pin_low();		/* 待机脚低：收发器工作，须在进 Normal 之前完成 */
}


void drv_set_canstb_pin_high(void)
{
	Gpio_SetBit(CAN_STB_GPIO_PORT, CAN_STB_GPIO_PIN);
}
void drv_set_canstb_pin_low(void)
{
	Gpio_ClearBit(CAN_STB_GPIO_PORT, CAN_STB_GPIO_PIN);
}




/**********************************************************
  * @brief	 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_bdc_driver_gpio_init(void)
{
	Gpio_ConfigType gpioConfig;
	
	Rcm_EnableAhbPeriphClock(BDC_GPIO_PER_CLOCK);
	
	gpioConfig.pin = (uint16_t)BDC_nFAULT_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_IN;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	Gpio_Config(BDC_nFAULT_GPIO_PORT, &gpioConfig);
	
	gpioConfig.pin = (uint16_t)BDC_nSLP_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_OUT;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	Gpio_Config(BDC_nSLP_GPIO_PORT, &gpioConfig);
	
	gpioConfig.pin = (uint16_t)BDC_BRAKE_GPIO_PIN;
	Gpio_Config(BDC_BRAKE_GPIO_PORT, &gpioConfig);
}


uint8_t drv_get_bdc_nfault_pin(void)
{
	return Gpio_ReadInputBit(BDC_nFAULT_GPIO_PORT, BDC_nFAULT_GPIO_PIN);
}

void drv_set_bdc_nslp_pin_high(void)
{
	Gpio_SetBit(BDC_nSLP_GPIO_PORT, BDC_nSLP_GPIO_PIN);
}
void drv_set_bdc_nslp_pin_low(void)
{
	Gpio_ClearBit(BDC_nSLP_GPIO_PORT, BDC_nSLP_GPIO_PIN);
}


void drv_set_bdc_brake_pin_high(void)
{
	Gpio_SetBit(BDC_BRAKE_GPIO_PORT, BDC_BRAKE_GPIO_PIN);
}
void drv_set_bdc_brake_pin_low(void)
{
	Gpio_ClearBit(BDC_BRAKE_GPIO_PORT, BDC_BRAKE_GPIO_PIN);
}




/**********************************************************
  * @brief	 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_hall_gpio_init(void)
{
	Gpio_ConfigType gpioConfig;
	
	Rcm_EnableAhbPeriphClock(HALL_GPIO_PER_CLOCK);
	
	gpioConfig.pin = (uint16_t)HALL_1_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_IN;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	Gpio_Config(HALL_1_GPIO_PORT, &gpioConfig);
	
	gpioConfig.pin = (uint16_t)HALL_2_GPIO_PIN;
	Gpio_Config(HALL_2_GPIO_PORT, &gpioConfig);
}

uint8_t drv_get_hall_1_pin(void)
{
	return Gpio_ReadInputBit(HALL_1_GPIO_PORT, HALL_1_GPIO_PIN);
}

uint8_t drv_get_hall_2_pin(void)
{
	return Gpio_ReadInputBit(HALL_2_GPIO_PORT, HALL_2_GPIO_PIN);
}




/**********************************************************
  * @brief	 休眠gpio引脚配置
  * @param  
  * @retval 
  * @note	配置为模拟输入，无上拉电阻，无下拉电阻，无速度
 **********************************************************/
void drv_gpio_sleep(void)
{
	Gpio_ConfigType gpioConfig;

	Rcm_EnableAhbPeriphClock(SLEEP_GPIO_PER_CLOCK);
		
	gpioConfig.pin = (uint16_t)SLEEP_GPIOA_PIN;
	gpioConfig.mode = GPIO_MODE_AN;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_2MHz;
	Gpio_Config(SLEEP_GPIOA_PORT, &gpioConfig);
	
	gpioConfig.pin = (uint16_t)SLEEP_GPIOB_PIN;
	gpioConfig.mode = GPIO_MODE_AN;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_2MHz;
	Gpio_Config(SLEEP_GPIOB_PORT, &gpioConfig);
	
	gpioConfig.pin = (uint16_t)SLEEP_GPIOC_PIN;
	gpioConfig.mode = GPIO_MODE_AN;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_2MHz;
	Gpio_Config(SLEEP_GPIOC_PORT, &gpioConfig);

	gpioConfig.pin = (uint16_t)SLEEP_GPIOD_PIN;
	gpioConfig.mode = GPIO_MODE_AN;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_2MHz;
	Gpio_Config(SLEEP_GPIOD_PORT, &gpioConfig);

	gpioConfig.pin = (uint16_t)SLEEP_GPIOF_PIN;
	gpioConfig.mode = GPIO_MODE_AN;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_2MHz;
	Gpio_Config(SLEEP_GPIOF_PORT, &gpioConfig);	

//	Rcm_DisableAhbPeriphClock(SLEEP_GPIO_PER_CLOCK);
	Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_ADC1);
	Rcm_DisableAhbPeriphClock(RCM_AHB_PERIPH_DMA1);
	Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_SPI1);
	Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_TMR1);
	Rcm_DisableApb1PeriphClock(RCM_APB1_PERIPH_TMR6);
	Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_USART1);
}
