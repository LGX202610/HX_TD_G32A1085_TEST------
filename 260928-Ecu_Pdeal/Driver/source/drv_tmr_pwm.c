
/* Includes */
#include "g32a10xx_gpio.h"
#include "g32a10xx_tmr.h"
#include "g32a10xx_rcm.h"
#include "board.h"
#include "drv_tmr_pwm.h"

#define DRV_TMR1_PWM_PERIOD		(1000U)

/**********************************************************
  * @brief	drv 定时器PWM 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_tmr_pwm_init(void)
{
	Gpio_ConfigType		gpioConfig;
	Tmr_TimeBaseType	tmr1_BaseConfig;
	Tmr_OcConfigType	tmr1_OC1PWMConfig;
	Tmr_BdtInitType 	tmrBdtCfg;
	
	/* Enable Clock */
	Rcm_EnableAhbPeriphClock(TMR_PWM_GPIO_PER_CLOCK);
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_TMR1);
	
	
	gpioConfig.mode = GPIO_MODE_AF;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pin = (uint16_t)TMR1_CH1_GPIO_PIN;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	
	/*  Connect TMR1 to CH1 */
	Gpio_ConfigPinAF(TMR1_CH1_GPIO_PORT, TMR1_CH1_PIN_SOURCE, TMR1_CH1_GPIO_AF);
	Gpio_Config(TMR1_CH1_GPIO_PORT, &gpioConfig);
	
	/*  Connect TMR1 to CH2 */
	gpioConfig.pin = (uint16_t)TMR1_CH2_GPIO_PIN;
	Gpio_ConfigPinAF(TMR1_CH2_GPIO_PORT, TMR1_CH2_PIN_SOURCE, TMR1_CH2_GPIO_AF);
	Gpio_Config(TMR1_CH2_GPIO_PORT, &gpioConfig);
	
	/*  Connect TMR1 to CH3 */
	gpioConfig.pin = (uint16_t)TMR1_CH3_GPIO_PIN;
	Gpio_ConfigPinAF(TMR1_CH3_GPIO_PORT, TMR1_CH3_PIN_SOURCE, TMR1_CH3_GPIO_AF);
	Gpio_Config(TMR1_CH3_GPIO_PORT, &gpioConfig);
	
	/*  Connect TMR1 to CH4 */
	gpioConfig.pin = (uint16_t)TMR1_CH4_GPIO_PIN;
	Gpio_ConfigPinAF(TMR1_CH4_GPIO_PORT, TMR1_CH4_PIN_SOURCE, TMR1_CH4_GPIO_AF);
	Gpio_Config(TMR1_CH4_GPIO_PORT, &gpioConfig);
	
	
	/* config TMR1 */
	tmr1_BaseConfig.clockDivision = TMR_CKD_DIV1;
	tmr1_BaseConfig.counterMode = TMR_COUNTER_MODE_UP;
	tmr1_BaseConfig.div = (4-1);
	tmr1_BaseConfig.period = (DRV_TMR1_PWM_PERIOD - 1U);
	tmr1_BaseConfig.repetitionCounter = 0;
	
	
	/* Configure channel */
	tmr1_OC1PWMConfig.OC_Mode = TMR_OC_MODE_PWM1;
	tmr1_OC1PWMConfig.OC_Idlestate = TMR_OCIDLESTATE_RESET;
	tmr1_OC1PWMConfig.OC_NIdlestate = TMR_OCNIDLESTATE_RESET;
	tmr1_OC1PWMConfig.OC_OutputNState = TMR_OUTPUT_NSTATE_DISABLE;
	tmr1_OC1PWMConfig.OC_OutputState = TMR_OUTPUT_STATE_ENABLE;
	tmr1_OC1PWMConfig.OC_Polarity = TMR_OC_POLARITY_HIGH;
	tmr1_OC1PWMConfig.OC_NPolarity = TMR_OC_NPOLARITY_HIGH;
	tmr1_OC1PWMConfig.Pulse = 0;
	
	
//	/* Configures the Break feature, dead time, Lock level, the IMOS */
//	//16MHz，TDTS=62.5ns，步长(10xx xxxx)：Tdts=2*TDTS=125ns，
//	//DTS[5:0]=(111 000)=56，死区时间：(64+56)*125ns = 1.5us
//	tmrBdtCfg.RMOS_State      = TMR_RMOS_STATE_ENABLE;
//	tmrBdtCfg.IMOS_State      = TMR_IMOS_STATE_ENABLE;
//	tmrBdtCfg.lockLevel       = TMR_LOCK_LEVEL_OFF;
//	tmrBdtCfg.deadTime        = 0xB8;
//	tmrBdtCfg.breakState      = TMR_BREAK_STATE_DISABLE;
//	tmrBdtCfg.breakPolarity   = TMR_BREAK_POLARITY_HIGH;
//	tmrBdtCfg.automaticOutput = TMR_AUTOMATIC_OUTPUT_ENABLE;		//当前接法定时器内置死区无用
	

	/* Config TimeBase */
	Tmr_ConfigTimeBase(TMR1, &tmr1_BaseConfig);

//	/* Configures the Break feature, dead time, Lock level, the IMOS */
//	Tmr1_ConfigBDT(&tmrBdtCfg);
	
	/* Config PWM Output */
	Tmr_OC1Config(TMR1, &tmr1_OC1PWMConfig);
	Tmr_OC2Config(TMR1, &tmr1_OC1PWMConfig);
	Tmr_OC3Config(TMR1, &tmr1_OC1PWMConfig);
	Tmr_OC4Config(TMR1, &tmr1_OC1PWMConfig);
	
	
	/* Enable PWM output */
	Tmr1_EnablePWMOutputs();
	/*  Enable TMR1  */
	Tmr_Enable(TMR1);
}


/**********************************************************
  * @brief	drv 定时器PWM 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_tmr_pwm_sleep(void)
{
	Tmr1_DisablePWMOutputs();
	Tmr_Disable(TMR1);
	Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_TMR1);
}

/**********************************************************
  * @brief	drv 定时器PWM 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_tmr_pwm_wakeup(void)
{
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_TMR1);
	Tmr1_EnablePWMOutputs();
	Tmr_Enable(TMR1);
}

/**********************************************************
  * @brief	drv 设置比较值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_set_tmr1_ch_comp(uint8_t ch, uint16_t comp)
{
	comp = (comp >= (DRV_TMR1_PWM_PERIOD - 1U)) ? (uint16_t)(DRV_TMR1_PWM_PERIOD - 1U) : comp;
	
	if(ch==1)					Tmr_SetCompare1(TMR1, comp);
	else if(ch==2)		Tmr_SetCompare2(TMR1, comp);
	else if(ch==3)		Tmr_SetCompare3(TMR1, comp);
	else if(ch==4)		Tmr_SetCompare4(TMR1, comp);
}


/**********************************************************
  * @brief	drv 使能PWM输出
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_enable_tmr1_pwm_output(void)
{
	Tmr1_EnablePWMOutputs();
}

/**********************************************************
  * @brief	drv 失能PWM输出
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_disable_tmr1_pwm_output(void)
{
	Tmr1_DisablePWMOutputs();
}
