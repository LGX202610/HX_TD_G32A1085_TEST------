
/* Includes */

#include "hal_motor.h"
#include "hal_adc.h"
#include "drv_tmr_pwm.h"
#include "drv_gpio.h"
#include "drv_adc.h"






/*********************************************** 硬件适配抽象 ***********************************************/



/*************************  硬件操作函数声明 *************************/
static void hal_motor_l_set_output(MotorDir_t dir, uint16_t comp);	// 左电机设置输出
static uint8_t hal_motor_l_get_hall(void);													// 左电机获取霍尔
static uint16_t hal_motor_l_get_current_adc(void); 									// 左电机获取电流 ADC

static void hal_motor_r_set_output(MotorDir_t dir, uint16_t comp); 	// 右电机设置输出
static uint8_t hal_motor_r_get_hall(void);										 			// 右电机获取霍尔
static uint16_t hal_motor_r_get_current_adc(void); 									// 右电机获取电流 ADC





/*************************  硬件用户配置 *************************/

/* 驱动电路类型：drv8718预驱，2路PWM驱动H桥 */
typedef struct
{
	uint8_t pwm_ch_a;   // 通道A
	uint8_t pwm_ch_b;   // 通道B
} MotorPwmHwCfg_t;


/* 驱动电路配置表：drv8718预驱，2路PWM驱动H桥 */
static const MotorPwmHwCfg_t s_motor_pwm_cfg[MOTOR_INST_MAX] =
{
  /* 左电机 */
	[MOTOR_INST_L] = {
		.pwm_ch_a = 3u,
		.pwm_ch_b = 4u,
	},

  /* 右电机 */
	[MOTOR_INST_R] = {
		.pwm_ch_a = 1u,
		.pwm_ch_b = 2u,
	},
};




/*************************  电机实例功能函数映射 *************************/
/* 功能函数类型 */
typedef struct
{
	void     (*set_output)(MotorDir_t dir, uint16_t pwm_comp);   // 设置电机驱动输出
	uint8_t  (*get_hall)(void);                                 // 获取霍尔采样 
	uint16_t (*get_current_adc)(void);                          // 获取电流 ADC 采样 

} MotorHwOps_t;


/* 功能函数映射表：移植或改驱动方式时只改各实例函数及本表 */
static const MotorHwOps_t s_motor_ops[MOTOR_INST_MAX] =
{
  /* 左电机 */
	[MOTOR_INST_L] = {

		.set_output      = hal_motor_l_set_output,
		.get_hall        = hal_motor_l_get_hall,
		.get_current_adc = hal_motor_l_get_current_adc,

	},

	
  /* 右电机 */
	[MOTOR_INST_R] = {

		.set_output      = hal_motor_r_set_output,
		.get_hall        = hal_motor_r_get_hall,
		.get_current_adc = hal_motor_r_get_current_adc,

	},

};




/*************************  硬件操作函数实现 *************************/

/******************************************************
  * @brief	电机驱动 
  * @param   
  * @retval 
  * @note		
  ******************************************************/
static void hal_motor_apply_pwm_hbridge(uint8_t ch_a, uint8_t ch_b, MotorDir_t dir, uint16_t comp)
{
	switch(dir)
	{
		case MOTOR_DIR_CCW:
			drv_set_tmr1_ch_comp(ch_a, 0u);
			drv_set_tmr1_ch_comp(ch_b, comp);
		break;

		case MOTOR_DIR_CW:
			drv_set_tmr1_ch_comp(ch_a, comp);
			drv_set_tmr1_ch_comp(ch_b, 0u);
		break;

		default:
			drv_set_tmr1_ch_comp(ch_a, 0u);
			drv_set_tmr1_ch_comp(ch_b, 0u);
		break;
	}

	drv_enable_tmr1_pwm_output();
}
  



/******************************************************
  * @brief	按实例 PWM 配置驱动输出
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static void hal_motor_pwm_set_output(MotorInstId_t id, MotorDir_t dir, uint16_t comp)
{
	if(id >= MOTOR_INST_MAX)
	{
		return;
	}

	hal_motor_apply_pwm_hbridge(s_motor_pwm_cfg[id].pwm_ch_a,
	                            s_motor_pwm_cfg[id].pwm_ch_b,
	                            dir, comp);
}


/******************************************************
  * @brief	左电机驱动输出
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static void hal_motor_l_set_output(MotorDir_t dir, uint16_t comp)
{
	hal_motor_pwm_set_output(MOTOR_INST_L, dir, comp);
}



/******************************************************
  * @brief	左电机霍尔采样
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static uint8_t hal_motor_l_get_hall(void)
{
	return drv_get_hall_1_pin();
}



/******************************************************
  * @brief	左电机电流 ADC 采样
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static uint16_t hal_motor_l_get_current_adc(void)
{
	return drv_get_adc_buf(ADC_BUF_CH_MOTOR_L);
}




/******************************************************
  * @brief	右电机驱动输出
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static void hal_motor_r_set_output(MotorDir_t dir, uint16_t comp)
{
	hal_motor_pwm_set_output(MOTOR_INST_R, dir, comp);
}



/******************************************************
  * @brief	右电机霍尔采样
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static uint8_t hal_motor_r_get_hall(void)
{
	return drv_get_hall_2_pin();
}



/******************************************************
  * @brief	右电机电流 ADC 采样
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static uint16_t hal_motor_r_get_current_adc(void)
{
	return drv_get_adc_buf(ADC_BUF_CH_MOTOR_R);
}






/*********************************************** 电机组件API接口 ***********************************************/

/**********************************************************
  * @brief	hal 电机初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_motor_init(void)
{
	drv_tmr_pwm_init();

	drv_hall_gpio_init();

	drv_con_power_gpio_init();
}





/**********************************************************
  * @brief	hal 电机休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_motor_sleep(void)
{ 
	drv_tmr_pwm_sleep();

	hal_hall_power_off();
}





/**********************************************************
  * @brief	hal 电机唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_motor_wakeup(void)
{
	drv_tmr_pwm_wakeup();

	hal_hall_power_on();
}





/**********************************************************
  * @brief	hal 霍尔供电
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_hall_power_on(void)
{
	drv_set_con_vst_pin_high();
}





/**********************************************************
  * @brief	hal 霍尔供电
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_hall_power_off(void)
{
	drv_set_con_vst_pin_low();
}





/**********************************************************
  * @brief	设置电机驱动输出
  * @param   id 电机实例标识
  * @param   dir 电机转向
  * @param   pwm_comp 电机占空比
  * @retval 
  * @note		
 **********************************************************/
void hal_motor_set_output(MotorInstId_t id, MotorDir_t dir, uint16_t pwm_comp)
{
	if((id >= MOTOR_INST_MAX) || (s_motor_ops[id].set_output == NULL))
	{
		return;
	}

	s_motor_ops[id].set_output(dir, pwm_comp);
}





/******************************************************
  * @brief	获取霍尔
  * @param  
  * @retval 
  * @note		
  ******************************************************/
uint8_t hal_motor_get_hall(MotorInstId_t id)
{
	if((id >= MOTOR_INST_MAX) || (s_motor_ops[id].get_hall == NULL))
	{
		return 0u;
	}

	return s_motor_ops[id].get_hall();
}





/**********************************************************
  * @brief	获取电流
  * @param  
  * @retval 
  * @note		
  **********************************************************/
uint16_t hal_motor_get_current_adc(MotorInstId_t id)
{
	if((id >= MOTOR_INST_MAX) || (s_motor_ops[id].get_current_adc == NULL))
	{
		return 0u;
	}

	return s_motor_ops[id].get_current_adc();
}







