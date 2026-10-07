/* Includes */
#include "app_power.h"

#include "app_timer.h"
#include "hal_pmu.h"
#include "hal_adc.h"
#include "hal_canfd.h"
#include "CanNm.h"
#include "hal_rtc.h"
#include "hal_drv8718.h"
#include "hal_timer.h"
#include "hal_usart.h"
#include "hal_dma.h"
#include "app_led.h"
#include "hal_iwdt.h"
#include "app_motor.h"


static uint16_t time_ecu_start = 1;
static uint16_t time_ecu_sleep = 1;

bool flag_ecu_start = false;
bool flag_ecu_sleep = false;



/******************************************************
  * @brief	power sleep
  * @param  
  * @retval 
  * @note		
******************************************************/
static void ecu_sleep_config(void)
{
	hal_drv8718_sleep();
	motor_sleep();

	hal_timer_sleep();
	hal_usart1_sleep();
	hal_adc_sleep();
	hal_dma_sleep();	
	hal_can_sleep();	
	app_led_sleep();

	
	hal_rtc_enable_wakeup_alarm();
	hal_iwdt_set_mode(HAL_IWDT_MODE_SLEEP);

	hal_pmu_enter_stop_2();
}
  
  
/******************************************************
* @brief	power wakeup
* @param  
* @retval 
* @note		
******************************************************/
static void ecu_rtc_wakeup_config(void)
{
	hal_pmu_restore_clock();

	hal_iwdt_refresh();
	hal_rtc_disable_wakeup_alarm();

	hal_rtc_clear_wakeup_flag();
}

/******************************************************
* @brief	power wakeup
* @param  
* @retval 
* @note		
******************************************************/
static void ecu_can_wakeup_config(void)
{
	hal_pmu_restore_clock();
	hal_iwdt_set_mode(HAL_IWDT_MODE_RUN);
	hal_rtc_disable_wakeup_alarm();

	hal_can_clear_wakeup_flag();
	
	hal_can_wakeup();
	app_led_wakeup();
	hal_drv8718_wakeup();
	motor_wakeup();	
	hal_dma_wakeup();
	hal_adc_wakeup();
	hal_usart1_wakeup();
	hal_timer_wakeup();
}



/******************************************************
  * @brief	休眠唤醒 
  * @param  
  * @retval 
  * @note		
  ******************************************************/
void ecu_power_sleep_wakeup(void)
{
	/************** 休眠条件 **************/
	if(flag_ecu_sleep == false)
	{
		if((flag_ecu_start == true) && (CanNm_GetState() == CANNM_STATE_BUS_SLEEP))
		{
			if(time_ecu_sleep==0) time_ecu_sleep=1;	// 开始进入休眠计时
		}
		else 
		{
			time_ecu_sleep = 0;
			flag_ecu_sleep = false;
		}
	}
	

	/************** 休眠触发 **************/
	if(flag_ecu_sleep == true)
	{
		ecu_sleep_config();
	}


	/************** RTC 唤醒 **************/
	if((flag_ecu_sleep == true) && (hal_rtc_get_wakeup_flag() == true))
	{
		ecu_rtc_wakeup_config();
	}

	/************** 中断 唤醒 **************/
	if((flag_ecu_sleep == true) && (hal_can_get_wakeup_flag() == true))
	{
		flag_ecu_sleep = false;
		time_ecu_sleep = 0;
		
		ecu_can_wakeup_config();
	}
}
  


static uint16_t time_check_vin = 0;		//母线电压检测
static bool flag_check_vin = false;		

static uint8_t keep_low_vin = 0;
static uint8_t keep_high_vin = 0;

bool flag_low_vin_protect = false;
bool flag_high_vin_protect = false;

float vin_voltage = 12.5f;
uint16_t vin_adc = 0;


#define VCC_ADC_TRANSFORM		0.001221f		//电压采样转换
#define VIN_MAGNIFICATION		6						//放大倍数
#define VIN_OFFSET				0.76f				//偏差电压
/******************************************************
  * @brief	计算总线电压
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static float cal_vin_voltage(uint16_t adc)
{	
	float temp_vin=0.0f;
	
	/************** 计算输入电压 **************/
	temp_vin = (adc * VCC_ADC_TRANSFORM * VIN_MAGNIFICATION) + VIN_OFFSET;		

	return temp_vin;	
}


/******************************************************
  * @brief	母线电压检测 
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static void vin_detect(void)
{
	/************** 母线电压检测 **************/
	if(flag_check_vin == true)
	{
		vin_adc = hal_get_vst_adc_value();
		vin_voltage = cal_vin_voltage(vin_adc);
		
		/************** 低压保护 **************/
		if(flag_low_vin_protect == false)
		{
			if(vin_voltage < LOW_VIN_PROTECT_THRESHOLD)
			{
				if(keep_low_vin < VIN_PROTECT_PERIOD)
				{
					keep_low_vin++;
				}
				if(keep_low_vin >= VIN_PROTECT_PERIOD)
				{
					keep_low_vin = VIN_PROTECT_PERIOD;
					flag_low_vin_protect = true;

					/************** 警报 **************/
					app_led_indicate(LED_CH_2, APP_LED_IND_VIN_LOW);
				}
			}
			else
			{
				keep_low_vin = 0;	/* 未确认前电压正常，去抖清零 */
			}
		}

		else
		{
			if(vin_voltage > LOW_VIN_RECOVER_THRESHOLD)	/* 带回差 */
			{
				if(keep_low_vin > 0)
				{
					keep_low_vin--;
				}
				if(keep_low_vin == 0)
				{
					flag_low_vin_protect = false;

					/************** 解除警报 **************/
					app_led_clear(LED_CH_2, APP_LED_IND_VIN_LOW);
				}
			}
			else
			{
				keep_low_vin = VIN_PROTECT_PERIOD;	/* 仍低压，保持锁定 */
			}
		}
		
		/************** 高压保护 **************/
		if(flag_high_vin_protect == false)
		{
			if(vin_voltage > HIGH_VIN_PROTECT_THRESHOLD)
			{				
				if(keep_high_vin < VIN_PROTECT_PERIOD)	
				{
					keep_high_vin++;
				}
				
				if(keep_high_vin >= VIN_PROTECT_PERIOD)
				{
					keep_high_vin = VIN_PROTECT_PERIOD;
					flag_high_vin_protect = true;
						
					/************** 警报 **************/
					app_led_indicate(LED_CH_2, APP_LED_IND_VIN_HIGH);
				}
			}
			else
			{
				keep_high_vin = 0;	/* 未确认前电压正常，去抖清零 */
			}
		}

		else
		{
			if(vin_voltage < HIGH_VIN_RECOVER_THRESHOLD)	/* 带回差 */
			{
				if(keep_high_vin > 0)
				{
					keep_high_vin--;
				}
				if(keep_high_vin == 0)
				{
					flag_high_vin_protect = false;

					/************** 解除警报 **************/
					app_led_clear(LED_CH_2, APP_LED_IND_VIN_HIGH);
				}
			}
			else
			{
				keep_high_vin = VIN_PROTECT_PERIOD;	/* 仍高压，保持锁定 */
			}
		}
		
		flag_check_vin = false;
	}
}





/******************************************************
  * @brief	power timer
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static void power_timer(void)
{
	/************** 上电滤波 **************/
	if(time_ecu_start > 0)
	{
		time_ecu_start++;
		
		if(time_ecu_start >= ECU_START_TIME)
		{
			time_ecu_start = 0;
			flag_ecu_start = true;
		}
	}

	/************** 休眠计时 **************/
	if(time_ecu_sleep > 0)
	{
		time_ecu_sleep++;
		
		if(time_ecu_sleep >= ECU_SLEEP_TIME)
		{
			time_ecu_sleep = 0;
			flag_ecu_sleep = true;
		}
	}


	/************** 母线电压检测 **************/
	if(flag_check_vin == false)
	{
		time_check_vin++;
		
		if(time_check_vin > VIN_DETECT_PERIOD)
		{
			time_check_vin = 0;
			flag_check_vin = true;
		}
	}
}

/******************************************************
  * @brief	power main
  * @param  
  * @retval 
  * @note		
  ******************************************************/
void app_power_main(void)
{
	vin_detect();
}


/******************************************************
  * @brief	power init
  * @param  
  * @retval 
  * @note		
  ******************************************************/
void app_power_init(void)
{
	app_register_timer_callback(power_timer, TIMER_10MS_CALLBACK_TYPE);
}



