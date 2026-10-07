/* Includes */
#include "app_motor.h"
#include "hal_motor.h"
#include <stddef.h>





/*********************************************** 内部私有状态 ***********************************************/

/******************************************************
  * @brief	计算占空比
  * @param  
  * @retval 
  * @note		
  ******************************************************/
static float cal_motor_pwmduty(float vin, float set)
{	
	float temp_duty=0.0f;
	
	/************** 计算占空比 **************/
	if(vin <= 0.0f)
	{
		return 0.0f;
	}
	temp_duty = set/vin;

	/************** 限幅 **************/
	temp_duty = (temp_duty>1.0f) ? 1.0f:((temp_duty<0.0f) ? 0.0f:temp_duty);
		
	return temp_duty;
}



/******************************************************
	* @brief	计算输出值
	* @param  
	* @retval 
	* @note		
 ******************************************************/
static uint16_t cal_motor_pwmvalue(float duty, uint16_t outputmax)
{
	uint16_t temp_output = 0;
	
	/**************  **************/
	if(duty<0.0f || duty>1.0f || outputmax==0)	return 0;

	/************** 计算输出值 **************/
	temp_output = (uint16_t)(duty * outputmax);
				
	/************** 限幅 **************/
	temp_output = (temp_output>outputmax) ? outputmax:((temp_output<0) ? 0:temp_output);
	
	return temp_output;
}



/******************************************************
* @brief	判断电机是否运行
* @param  
* @retval 
* @note		
******************************************************/
static bool motor_is_running(MotorDir_t dir)
{
	return (dir == MOTOR_DIR_CW) || (dir == MOTOR_DIR_CCW);
}



/******************************************************
  * @brief	读取霍尔采样
  * @param  
  * @retval 
  * @note		桥接HAL硬件映射
  ******************************************************/
static uint8_t motor_read_hall(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0u;
	}

	return hal_motor_get_hall(motor->id);
}


/**********************************************************
* @brief	霍尔采样处理
* @param   
* @param   
* @retval 
* @note		
**********************************************************/
static void motor_hall_on_sample(MotorInst_t *motor, uint8_t hall_value)
{	
	if(motor == NULL)
	{
		return;
	}

	if(motor_is_running(motor->param.dir) == false)
	{
		motor->param.hall_value = hall_value;
		motor->param.hall_cnt = 0u;
		motor->state.time_hall_lose = 0u;
		motor->state.time_hall_stall = 0u;
		motor->state.time_hall_pinch = 0u;
		motor->state.flag_hall_lose = false;
		motor->state.flag_hall_stall = false;
		motor->state.flag_hall_pinch = false;
		
		return;
	}

	/*************** 霍尔跳变 ***************/
	if(motor->param.hall_value != hall_value)
	{
		motor->param.hall_value = hall_value;
		motor->param.hall_cnt = (motor->param.hall_cnt < 0xffffu) ? (motor->param.hall_cnt + 1u) : 0xffffu;

		/*************** CW ***************/
		if(motor->param.dir == MOTOR_DIR_CW)
		{
			motor->param.hall_pos = (motor->param.hall_pos < 0xffffu) ? (motor->param.hall_pos + 1u) : 0xffffu;
		}
		/*************** CCW ***************/
		else
		{
			motor->param.hall_pos = (motor->param.hall_pos > 0u) ? (motor->param.hall_pos - 1u) : 0u;
		}

		motor->state.time_hall_lose = 0u;
		motor->state.time_hall_stall = 0u;
		motor->state.flag_hall_lose = false;
		motor->state.flag_hall_stall = false;

		if(motor->func_en.hall_pinch == true)
		{
			motor->state.time_hall_pinch = 1u;
			motor->state.flag_hall_pinch = false;
		}
	}
	
	/*************** 霍尔未跳变 ***************/
	else
	{
		/*************** 霍尔丢失 ***************/
		if(motor->state.time_hall_lose == 0u)
		{
			motor->state.time_hall_lose = 1u;
		}

		/*************** 霍尔堵转 ***************/
		if(motor->func_en.hall_stall == true)
		{
			if(motor->state.time_hall_stall == 0u)
			{
				motor->state.time_hall_stall = 1u;
			}
		}
		else
		{
			motor->state.time_hall_stall = 0u;
			motor->state.flag_hall_stall = false;
		}
	}

	/*************** 霍尔防夹 ***************/
	if(motor->func_en.hall_pinch == false)
	{
		motor->state.time_hall_pinch = 0u;
		motor->state.flag_hall_pinch = false;
	}
}



/******************************************************
  * @brief	读取电流 ADC
  * @param  
  * @retval 
  * @note		桥接HAL硬件映射
  ******************************************************/
static uint16_t motor_read_adc(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0u;
	}

	return hal_motor_get_current_adc(motor->id);
}

/**********************************************************
 * @brief	电流采样处理
 * @param   
 * @param  
 * @retval 
 * @note		
**********************************************************/
static void motor_current_on_sample(MotorInst_t *motor, uint16_t adc)
{
	if(motor == NULL)
	{
		return;
	}

	if(motor_is_running(motor->param.dir) == false)
	{
		motor->state.time_current_pinch = 0;
		motor->state.flag_current_pinch = false;
		
		motor->state.time_current_stall = 0;	
		motor->state.flag_current_stall = false;
			
		motor->state.time_short_current = 0;
		motor->state.flag_short_current = false;
		
		return;
	}
		
	motor->param.adc = adc;

	/*************** 防夹电流 ***************/
	if(motor->func_en.current_pinch == true)
	{
		if(motor->param.adc > motor->threshold.pinch_current)
		{
			if(motor->state.time_current_pinch==0)	motor->state.time_current_pinch = 1;	
		}
		else
		{
			motor->state.time_current_pinch = 0;
			motor->state.flag_current_pinch = false;
		}
	}
	else
	{
		motor->state.time_current_pinch = 0;
		motor->state.flag_current_pinch = false;
	}

	/*************** 堵转电流 ***************/
	if(motor->func_en.current_stall == true)
	{
		if(motor->param.adc > motor->threshold.stall_current)
		{
			if(motor->state.time_current_stall==0)	motor->state.time_current_stall = 1;	
		}
		else
		{
			motor->state.time_current_stall = 0;
			motor->state.flag_current_stall = false;
		}
	}
	else
	{
		motor->state.time_current_stall = 0;	
		motor->state.flag_current_stall = false;
	}

	/*************** 短路电流 ***************/
	if(motor->func_en.current_short == true)
	{
		if(motor->param.adc > motor->threshold.short_current)
		{
			if(motor->state.time_short_current==0)	motor->state.time_short_current = 1;
		}
		else
		{
			motor->state.time_short_current = 0;
			motor->state.flag_short_current = false;
		}
	}
	else
	{
		motor->state.time_short_current = 0;
		motor->state.flag_short_current = false;
	}
} 



/**********************************************************
 * @brief		运行状态计时
 * @param   
 * @retval 
 * @note		
**********************************************************/
static void motor_state_on_tick(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}
	
	/*************** 运行时间 ***************/
	if(motor->state.time_run > 0u)
	{
		motor->state.time_run++;

		if(motor->state.time_run > 0x0FFFu)
		{
			motor->state.time_run = 0x0FFFu;
		}

		if((motor->threshold.run_time_max > 0u) &&
		   (motor->state.time_run >= motor->threshold.run_time_max))
		{
			motor->state.flag_ovortime = true;
		}
	}

	/*************** 电流防夹 ***************/
	if(motor->state.time_current_pinch > 0)
	{
		motor->state.time_current_pinch++;

		if(motor->state.time_current_pinch > motor->threshold.pinch_time)
		{
			motor->state.time_current_pinch = motor->threshold.pinch_time;
			motor->state.flag_current_pinch = true;
		}
	}

	/*************** 电流堵转 ***************/
	if(motor->state.time_current_stall > 0)
	{
		motor->state.time_current_stall++;

		if(motor->state.time_current_stall > motor->threshold.stall_time)
		{
			motor->state.time_current_stall = motor->threshold.stall_time;
			motor->state.flag_current_stall = true;
		}
	}

	/*************** 电流短路 ***************/
	if(motor->state.time_short_current > 0)
	{
		motor->state.time_short_current++;

		if(motor->state.time_short_current > motor->threshold.short_time)
		{
			motor->state.time_short_current = motor->threshold.short_time;
			motor->state.flag_short_current = true;
		}
	}


	/*************** 霍尔防夹 ***************/
	if(motor->state.time_hall_pinch > 0)
	{
		motor->state.time_hall_pinch++;

		if(motor->state.time_hall_pinch > motor->threshold.hall_pinch_time)
		{
			motor->state.time_hall_pinch = motor->threshold.hall_pinch_time;
			motor->state.flag_hall_pinch = true;
		}
	}

	/*************** 霍尔堵转 ***************/
	if(motor->state.time_hall_stall > 0)
	{
		motor->state.time_hall_stall++;

		if(motor->state.time_hall_stall > motor->threshold.hall_stall_time)
		{
			motor->state.time_hall_stall = motor->threshold.hall_stall_time;
			motor->state.flag_hall_stall = true;
		}
	}

	/*************** 霍尔丢失 ***************/
	if(motor->state.time_hall_lose > 0)
	{
		motor->state.time_hall_lose++;

		if(motor->state.time_hall_lose > motor->threshold.hall_lose_time)
		{
			motor->state.time_hall_lose = motor->threshold.hall_lose_time;
			motor->state.flag_hall_lose = true;
		}
	}
}








/*********************************************** 对外API ***********************************************/

/**********************************************************
  * @brief	电机 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_init(void)
{
	hal_motor_init();
}


/**********************************************************
  * @brief	电机 睡眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_sleep(void)
{
	hal_motor_sleep();
}


/**********************************************************
  * @brief	电机 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_wakeup(void)
{
	hal_motor_wakeup();
}


/**********************************************************
  * @brief	电机实例初始化
  * @param   motor 电机实例
  * @param   id    硬件实例 ID
  * @retval 
  * @note		
 **********************************************************/
void motor_inst_init(MotorInst_t *motor, MotorInstId_t id)
{
	if(motor == NULL)
	{
		return;
	}

	motor->param.dir = MOTOR_DIR_STOP;
	motor->param.pwmduty = 0.0f;
	motor->param.voltage = 0.0f;
	motor->param.adc = 0u;
	motor->param.hall_value = 0u;
	motor->param.hall_pos = 0u;
	motor->param.hall_cnt = 0u;

	motor->state.time_run = 0u;
	motor->state.time_current_pinch = 0u;
	motor->state.time_current_stall = 0u;
	motor->state.time_short_current = 0u;
	motor->state.time_hall_pinch = 0u;
	motor->state.time_hall_stall = 0u;
	motor->state.time_hall_lose = 0u;
	motor->state.flag_ovortime = false;
	motor->state.flag_current_pinch = false;
	motor->state.flag_current_stall = false;
	motor->state.flag_short_current = false;
	motor->state.flag_hall_pinch = false;
	motor->state.flag_hall_stall = false;
	motor->state.flag_hall_lose = false;

	motor->threshold.pinch_current = 0u;
	motor->threshold.stall_current = 0u;
	motor->threshold.short_current = 0u;
	motor->threshold.pinch_time = 0u;
	motor->threshold.stall_time = 0u;
	motor->threshold.short_time = 0u;
	motor->threshold.hall_pinch_time = 0u;
	motor->threshold.hall_stall_time = 0u;
	motor->threshold.hall_lose_time = 0u;
	motor->threshold.run_time_max = 0u;

	motor->func_en.current_pinch = false;
	motor->func_en.current_stall = false;
	motor->func_en.current_short = false;
	motor->func_en.hall_pinch = false;
	motor->func_en.hall_stall = false;

	motor->id = id;
}





/*********************************************** 电机驱动 ***********************************************/

/******************************************************
* @brief	设置电机转向和驱动电压
* @param  
* @retval 
* @note		
******************************************************/
void motor_set_dir_vol(MotorInst_t *motor, MotorDir_t dir, float vol, float vin)
{
	uint16_t temp_pwmvalue = 0;

	if((motor == NULL) || (motor->id >= MOTOR_INST_MAX))
	{
		return;
	}

	motor->param.dir = dir;
	motor->param.pwmduty = cal_motor_pwmduty(vin, vol);
	temp_pwmvalue = cal_motor_pwmvalue(motor->param.pwmduty, MOTOR_PWM_MAX);
	motor->param.voltage = motor->param.pwmduty * vin;

	hal_motor_set_output(motor->id, dir, temp_pwmvalue);

	// if(motor_is_running(dir) == false)
	// {
	// 	motor_stop_run_timer(motor);
	// }
}


/******************************************************
* @brief	使能电机霍尔电源
* @param  
* @retval 
* @note		
******************************************************/
void motor_hall_power_on(void)
{
	hal_hall_power_on();
}


/******************************************************
* @brief	禁用电机霍尔电源
* @param  
* @retval 
* @note		
******************************************************/
void motor_hall_power_off(void)
{
	hal_hall_power_off();
}



/**********************************************************
 * @brief	霍尔
 * @param   
 * @retval 
 * @note		
**********************************************************/
void motor_hall_tick(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}

	motor_hall_on_sample(motor, motor_read_hall(motor));
}



/**********************************************************
 * @brief	电流
 * @param   
 * @retval 
 * @note		
**********************************************************/
void motor_current_tick(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}

	motor_current_on_sample(motor, motor_read_adc(motor));
}



/**********************************************************
 * @brief		状态
 * @param   
 * @retval 
 * @note		
**********************************************************/
void motor_state_tick(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}

	motor_state_on_tick(motor);
}






/******************************************** 电机阈值类API ********************************************/

/**********************************************************
  * @brief	设置电机 阈值
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_threshold(MotorInst_t *motor, const MotorThreshold_t *th)
{
	if((motor == NULL) || (th == NULL))
	{
		return;
	}
	motor->threshold = *th;
}


/**********************************************************
  * @brief	设置电机 防夹电流阈值
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_pinch_current_th(MotorInst_t *motor, uint16_t adc)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.pinch_current = adc;
}

/**********************************************************
  * @brief	设置电机 防夹电流时间阈值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_pinch_time_th(MotorInst_t *motor, uint16_t time)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.pinch_time = time;
}



/**********************************************************
  * @brief	设置电机 堵转电流阈值
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_stall_current_th(MotorInst_t *motor, uint16_t adc)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.stall_current = adc;
}

/**********************************************************
  * @brief	设置电机 堵转电流时间阈值
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_stall_time_th(MotorInst_t *motor, uint16_t time)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.stall_time = time;
}


/**********************************************************
  * @brief	设置电机 短路电流阈值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_short_current_th(MotorInst_t *motor, uint16_t adc)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.short_current = adc;
}

/**********************************************************
  * @brief	设置电机 短路电流时间阈值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_short_time_th(MotorInst_t *motor, uint16_t time)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.short_time = time;
}


/**********************************************************
  * @brief	设置电机 霍尔防夹时间阈值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_hall_pinch_time_th(MotorInst_t *motor, uint16_t time)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.hall_pinch_time = time;
}


 /**********************************************************
  * @brief	设置电机 霍尔堵转时间阈值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_hall_stall_time_th(MotorInst_t *motor, uint16_t time)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.hall_stall_time = time;
}


/**********************************************************
  * @brief	设置电机 霍尔丢失时间阈值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_hall_lose_time_th(MotorInst_t *motor, uint16_t time)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.hall_lose_time = time;
}

 
/**********************************************************
  * @brief	设置运行超时阈值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_run_timeout_th(MotorInst_t *motor, uint16_t ten_ms)
{
	if(motor == NULL)
	{
		return;
	}
	motor->threshold.run_time_max = ten_ms;
}






/******************************************** 电机功能类API ********************************************/

/**********************************************************
  * @brief	使能电机 电流防夹
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_current_pinch_en(MotorInst_t *motor, bool en)
{
	if(motor == NULL)
	{
		return;
	}

	if(motor->func_en.current_pinch == en)
	{
		return;
	}

	motor->func_en.current_pinch = en;
	motor->state.time_current_pinch = 0;
	motor->state.flag_current_pinch = false;
}


/**********************************************************
  * @brief	使能电机 堵转电流
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_current_stall_en(MotorInst_t *motor, bool en)
{
	if(motor == NULL)
	{
		return;
	}

	if(motor->func_en.current_stall == en)
	{
		return;
	}

	motor->func_en.current_stall = en;
	motor->state.time_current_stall = 0;
	motor->state.flag_current_stall = false;
}


/**********************************************************
  * @brief	使能电机 短路电流
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_current_short_en(MotorInst_t *motor, bool en)
{
	if(motor == NULL)
	{
		return;
	}

	if(motor->func_en.current_short == en)
	{
		return;
	}

	motor->func_en.current_short = en;
	motor->state.time_short_current = 0;
	motor->state.flag_short_current = false;
}

/**********************************************************
  * @brief	使能电机 霍尔防夹
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_hall_pinch_en(MotorInst_t *motor, bool en)
{
	if(motor == NULL)
	{
		return;
	}

	if(motor->func_en.hall_pinch == en)
	{
		return;
	}

	motor->func_en.hall_pinch = en;
	motor->state.time_hall_pinch = 0;
	motor->state.flag_hall_pinch = false;
}

/**********************************************************
  * @brief	使能电机 霍尔堵转
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_set_hall_stall_en(MotorInst_t *motor, bool en)
{
	if(motor == NULL)
	{
		return;
	}

	if(motor->func_en.hall_stall == en)
	{
		return;
	}

	motor->func_en.hall_stall = en;
	motor->state.time_hall_stall = 0;
	motor->state.flag_hall_stall = false;
}




/******************************************** 电机状态类API ********************************************/

/**********************************************************
  * @brief	获取电机方向
  * @param  
  * @retval 
  * @note		
 **********************************************************/
MotorDir_t motor_get_dir(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return MOTOR_DIR_STOP;
	}
	return motor->param.dir;
}

/**********************************************************
  * @brief	获取电机占空比
  * @param  
  * @retval 
  * @note		
 **********************************************************/
float motor_get_pwmduty(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0.0f;
	}
	return motor->param.pwmduty;
}

/**********************************************************
  * @brief	获取电机电压
  * @param  
  * @retval 
  * @note		
 **********************************************************/
float motor_get_voltage(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0.0f;
	}
	return motor->param.voltage;
}

/**********************************************************
  * @brief	获取电流 ADC 采样值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint16_t motor_get_adc(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0u;
	}
	return motor->param.adc;
}

/**********************************************************
  * @brief	获取霍尔原始采样值
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint8_t motor_get_hall_value(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0u;
	}
	return motor->param.hall_value;
}

/**********************************************************
  * @brief	设置电机 霍尔位置
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_set_hall_pos(MotorInst_t *motor, uint16_t pos)
{
	if(motor == NULL)
	{
		return;
	}
	motor->param.hall_pos = pos;
}

/**********************************************************
  * @brief	获取霍尔行程位置
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint16_t motor_get_hall_pos(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0u;
	}
	return motor->param.hall_pos;
}

/**********************************************************
  * @brief	获取霍尔跳变计数
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint16_t motor_get_hall_cnt(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0u;
	}
	return motor->param.hall_cnt;
}

/**********************************************************
  * @brief	获取电机 运行时间
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint16_t motor_get_time_run(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return 0u;
	}
	return motor->state.time_run;
}

/**********************************************************
  * @brief	获取运行超时标志（flag_ovortime）
  * @param  
  * @retval 
  * @note		
 **********************************************************/
bool motor_get_flag_ovortime(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return false;
	}
	return motor->state.flag_ovortime;
}

/**********************************************************
  * @brief	获取电机 电流防夹标志位
  * @param  
  * @retval 
  * @note		
 **********************************************************/
bool motor_get_flag_current_pinch(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return false;
	}
	return motor->state.flag_current_pinch;
}

/**********************************************************
  * @brief	获取电机 电流堵转标志位
  * @param  
  * @retval 
  * @note		
 **********************************************************/
bool motor_get_flag_current_stall(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return false;
	}
	return motor->state.flag_current_stall;
}

/**********************************************************
  * @brief	获取电机 短路标志位
  * @param  
  * @retval 
  * @note		
 **********************************************************/
bool motor_get_flag_short_current(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return false;
	}
	return motor->state.flag_short_current;
}

/**********************************************************
  * @brief	获取电机 霍尔防夹标志位
  * @param  
  * @retval 
  * @note		
 **********************************************************/
bool motor_get_flag_hall_pinch(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return false;
	}
	return motor->state.flag_hall_pinch;
}

/**********************************************************
  * @brief	获取电机 霍尔堵转标志位
  * @param  
  * @retval 
  * @note		
 **********************************************************/
bool motor_get_flag_hall_stall(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return false;
	}
	return motor->state.flag_hall_stall;
}

/**********************************************************
  * @brief	获取电机 丢失霍尔标志位
  * @param  
  * @retval 
  * @note		
 **********************************************************/
bool motor_get_flag_hall_lose(const MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return false;
	}
	return motor->state.flag_hall_lose;
}





/**********************************************************
  * @brief	启动运行计时
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_start_run_timer(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}

	motor->state.time_run = 1u;
	motor->state.flag_ovortime = false;
}

/**********************************************************
  * @brief	停止运行计时
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_stop_run_timer(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}

	motor->state.time_run = 0u;
	motor->state.flag_ovortime = false;
}

/**********************************************************
  * @brief	清除state
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void motor_clear_state(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}

	motor->state.time_run = 0u;
	motor->state.time_current_pinch = 0u;
	motor->state.time_current_stall = 0u;
	motor->state.time_short_current = 0u;
	motor->state.time_hall_pinch = 0u;
	motor->state.time_hall_stall = 0u;
	motor->state.time_hall_lose = 0u;
	motor->state.flag_ovortime = false;
	motor->state.flag_current_pinch = false;
	motor->state.flag_current_stall = false;
	motor->state.flag_short_current = false;
	motor->state.flag_hall_pinch = false;
	motor->state.flag_hall_stall = false;
	motor->state.flag_hall_lose = false;
}

/**********************************************************
  * @brief	清除霍尔行程
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void motor_clear_hall_travel(MotorInst_t *motor)
{
	if(motor == NULL)
	{
		return;
	}

	motor->param.hall_pos = 0u;
	motor->param.hall_cnt = 0u;
}




