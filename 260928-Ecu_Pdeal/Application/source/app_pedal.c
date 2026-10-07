/* Includes */
#include "app_pedal.h"
#include "app_motor.h"
#include "app_timer.h"
#include "app_power.h"
#include "hal_drv8718.h"
#include <stddef.h>



MotorInst_t motor_l = {
	.param = {
		.dir = PEDAL_L_STOP_DIR,
		.pwmduty = 0.0f,
		.voltage = PEDAL_L_STOP_VOL,
		.adc = 0,
		.hall_value = 0,
		.hall_pos = 0,
		.hall_cnt = 0,
	},
	
	.state = {
		.time_run = 0,
		.time_current_pinch = 0,
		.time_current_stall = 0,
		.time_short_current = 0,
		.time_hall_pinch = 0,
		.time_hall_stall = 0,
		.time_hall_lose = 0,
		.flag_ovortime = false,
		.flag_current_pinch = false,
		.flag_current_stall = false,
		.flag_short_current = false,
		.flag_hall_pinch = false,
		.flag_hall_stall = false,
		.flag_hall_lose = false,
	},
	
	.threshold = {
		.pinch_current = 0,
		.stall_current = 0,
		.short_current = 0,
		.pinch_time = 0,
		.stall_time = 0,
		.short_time = 0,
		.hall_pinch_time = 0,
		.hall_stall_time = 0,
		.hall_lose_time = PEDAL_L_HALL_LOSE_TIME,
	},

	.func_en = {
		.current_pinch = false,
		.current_stall = true,
		.current_short = false,
		.hall_pinch = false,
		.hall_stall = false,
	},
	
	.id = MOTOR_INST_R,
};	


PedalInst pedal_l = {
	.travel_pos = PEDAL_L_CLOSE_STOP_POS,
	.travel_pos_last_cnt = 0u,
	.place = _pedal_close,
};



#define PEDAL_L_SHORT_ADC			(4000u)


// 电机停止状态阈值
static const MotorThreshold_t pedal_l_idle_th = {
	.pinch_current = PEDAL_L_OPEN_PINCH_ADC,
	.stall_current = PEDAL_L_OPEN_STALL_ADC,
	.short_current = PEDAL_L_SHORT_ADC,
	
	.pinch_time = 0u,
	.stall_time = 0u,
	.short_time = 0u,
	.hall_pinch_time = 0u,
	.hall_stall_time = 0u,
	.hall_lose_time = PEDAL_L_HALL_LOSE_TIME,
};

// 电机伸出状态阈值
static const MotorThreshold_t pedal_l_open_th = {
	.pinch_current = PEDAL_L_OPEN_PINCH_ADC,
	.stall_current = PEDAL_L_OPEN_STALL_ADC,
	.short_current = PEDAL_L_SHORT_ADC,
	
	.pinch_time = PEDAL_L_OPEN_PINCH_TIME,
	.stall_time = PEDAL_L_OPEN_CURRENT_STALL_TIME,
	.short_time = PEDAL_L_OPEN_SHORT_CURRENT_TIME,
	.hall_pinch_time = PEDAL_L_OPEN_HALL_PINCH_TIME,
	.hall_stall_time = PEDAL_L_OPEN_HALL_STALL_TIME,
	.hall_lose_time = PEDAL_L_HALL_LOSE_TIME,
};

// 电机收回状态阈值
static const MotorThreshold_t pedal_l_close_th = {
	.pinch_current = PEDAL_L_CLOSE_PINCH_ADC,
	.stall_current = PEDAL_L_CLOSE_STALL_ADC,
	.short_current = PEDAL_L_SHORT_ADC,
	
	.pinch_time = PEDAL_L_CLOSE_PINCH_TIME,
	.stall_time = PEDAL_L_CLOSE_CURRENT_STALL_TIME,
	.short_time = PEDAL_L_CLOSE_SHORT_CURRENT_TIME,
	.hall_pinch_time = PEDAL_L_CLOSE_HALL_PINCH_TIME,
	.hall_stall_time = PEDAL_L_CLOSE_HALL_STALL_TIME,
	.hall_lose_time = PEDAL_L_HALL_LOSE_TIME,
};






/**********************************************************
  * @brief	左踏板 设置转向和电压
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void set_pedal_l_dir_vol(MotorDir_t dir, float vol)
{
	motor_set_dir_vol(&motor_l, dir, vol, vin_voltage);
}



volatile uint8_t stage_pedal_l = 0;					//电机进程

/**********************************************************
  * @brief	设置踏板 伸出
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void set_pedal_l_open(void)
{
	if(pedal_l.place != _pedal_opening && pedal_l.place != _pedal_open)
	{
		set_pedal_l_dir_vol(PEDAL_L_STOP_DIR, PEDAL_L_STOP_VOL);
		
		stage_pedal_l = 1;
		Pedal_L_Pend_Delay(2);
	}
}

/**********************************************************
  * @brief	设置踏板 收回
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void set_pedal_l_close(void)
{
	if(pedal_l.place != _pedal_closing && pedal_l.place != _pedal_close)
	{
		set_pedal_l_dir_vol(PEDAL_L_STOP_DIR, PEDAL_L_STOP_VOL);
		
		stage_pedal_l = 11;
		Pedal_L_Pend_Delay(2);
	}
}




/**********************************************************
  * @brief	左踏板 业务行程加法
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static uint16_t pedal_l_travel_pos_add(uint16_t pos, uint16_t delta)
{
	uint32_t sum = (uint32_t)pos + (uint32_t)delta;

	if(sum > (uint32_t)PEDAL_L_TRAVEL_MAX)
	{
		return (uint16_t)PEDAL_L_TRAVEL_MAX;
	}

	return (uint16_t)sum;
}

/**********************************************************
  * @brief	左踏板 业务行程减法
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static uint16_t pedal_l_travel_pos_sub(uint16_t pos, uint16_t delta)
{
	if(pos <= delta)
	{
		return (uint16_t)PEDAL_L_TRAVEL_MIN;
	}

	return (uint16_t)(pos - delta);
}

/**********************************************************
  * @brief	左踏板 业务行程计数器重置
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void pedal_l_travel_reset_cnt(void)
{
	pedal_l.travel_pos_last_cnt = 0u;
}

/**********************************************************
  * @brief	左踏板 业务行程读取
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint16_t pedal_l_get_travel_pos(void)
{
	return pedal_l.travel_pos;
}

/**********************************************************
  * @brief	左踏板 业务行程校准
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void pedal_l_set_travel_pos(uint16_t travel_pos)
{
	if(travel_pos > PEDAL_L_TRAVEL_MAX)
	{
		travel_pos = (uint16_t)PEDAL_L_TRAVEL_MAX;
	}

	pedal_l.travel_pos = travel_pos;
	pedal_l_travel_reset_cnt();
}

/**********************************************************
  * @brief	左踏板 业务行程更新
  * @param  
  * @retval 
  * @note	由hall_cnt增量更新，方向由OPEN/CLOSE宏决定
 **********************************************************/
static void pedal_l_travel_pos_update(void)
{
	MotorDir_t dir = motor_get_dir(&motor_l);
	uint16_t hall_cnt = motor_get_hall_cnt(&motor_l);
	uint16_t delta;

	if((pedal_l.place != _pedal_opening) && (pedal_l.place != _pedal_closing))
	{
		pedal_l_travel_reset_cnt();
		return;
	}

	if((dir != PEDAL_L_OPEN_DIR) && (dir != PEDAL_L_CLOSE_DIR))
	{
		pedal_l_travel_reset_cnt();
		return;
	}

	if(hall_cnt >= pedal_l.travel_pos_last_cnt)
	{
		delta = (uint16_t)(hall_cnt - pedal_l.travel_pos_last_cnt);
	}
	else
	{
		pedal_l_travel_reset_cnt();
		delta = hall_cnt;
	}

	pedal_l.travel_pos_last_cnt = hall_cnt;

	if(delta == 0u)
	{
		return;
	}

	/*************** 更新踏板位置 ***************/
	if(dir == PEDAL_L_OPEN_DIR)
	{
#if (PEDAL_L_OPEN_TRAVEL_INC == 1)
		pedal_l.travel_pos = pedal_l_travel_pos_add(pedal_l.travel_pos, delta);
#else
		pedal_l.travel_pos = pedal_l_travel_pos_sub(pedal_l.travel_pos, delta);
#endif
	}
	else
	{
#if (PEDAL_L_OPEN_TRAVEL_INC == 1)
		pedal_l.travel_pos = pedal_l_travel_pos_sub(pedal_l.travel_pos, delta);
#else
		pedal_l.travel_pos = pedal_l_travel_pos_add(pedal_l.travel_pos, delta);
#endif
	}
}


/**********************************************************
  * @brief	左踏板 位置更新
  * @param  
  * @retval 
  * @note		
 **********************************************************/
 void pedal_l_pos_update(void)
 {
	 uint16_t pos = pedal_l_get_travel_pos();
 
	 if(pedal_l.place == _pedal_opening)
	 {
		 if(pos > PEDAL_L_OPEN_STOP_POS)
		 {
			 pedal_l.phase = _phase_stop;
		 }
		 else if(pos > PEDAL_L_OPEN_SLOW_POS)
		 {
			 pedal_l.phase = _phase_slow;
		 }
		 else if(pos > PEDAL_L_OPEN_FAST_POS)
		 {
			 pedal_l.phase = _phase_fast;
		 }
		 else if(pos > PEDAL_L_OPEN_START_POS)
		 {
			 pedal_l.phase = _phase_start;
		 }
		 else
		 {
			 pedal_l.phase = _phase_stop;
		 }
	 }
 
	 else if(pedal_l.place == _pedal_closing)
	 {
		 if(pos < PEDAL_L_CLOSE_STOP_POS)
		 {
			 pedal_l.phase = _phase_stop;
		 }
		 else if(pos < PEDAL_L_CLOSE_SLOW_POS)
		 {
			 pedal_l.phase = _phase_slow;
		 }
		 else if(pos < PEDAL_L_CLOSE_FAST_POS)
		 {
			 pedal_l.phase = _phase_fast;
		 }
		 else if(pos < PEDAL_L_CLOSE_START_POS)
		 {
			 pedal_l.phase = _phase_start;
		 }
		 else
		 {
			 pedal_l.phase = _phase_stop;
		 }
	 }
 
	 else
	 {
		 pedal_l.phase = _phase_stop;
	 }
 }
 
 
 /**********************************************************
   * @brief	左踏板 运行阶段更新
   * @param  
   * @retval 
   * @note		
  **********************************************************/
 void pedal_l_phase_update(void)
 {	
	 static PedalPhase last_phase = _phase_stop;
 
	 if(last_phase == pedal_l.phase)
	 {
		 return;
	 }
	 else
	 {
		 last_phase = pedal_l.phase;
 
		 if(pedal_l.place == _pedal_opening)
		 {
			 if(pedal_l.phase == _phase_start)
			 {
				 set_pedal_l_dir_vol(PEDAL_L_OPEN_DIR, PEDAL_L_OPEN_START_VOL);
			 }
			 else if(pedal_l.phase == _phase_fast)
			 {
				 set_pedal_l_dir_vol(PEDAL_L_OPEN_DIR, PEDAL_L_OPEN_FAST_VOL);
			 }
			 else if(pedal_l.phase == _phase_slow)
			 {
				 set_pedal_l_dir_vol(PEDAL_L_OPEN_DIR, PEDAL_L_OPEN_SLOW_VOL);
			 }
		 }
 
		 else if(pedal_l.place == _pedal_closing)
		 {
			 if(pedal_l.phase == _phase_start)
			 {
				 set_pedal_l_dir_vol(PEDAL_L_CLOSE_DIR, PEDAL_L_CLOSE_START_VOL);
			 }
			 else if(pedal_l.phase == _phase_fast)
			 {
				 set_pedal_l_dir_vol(PEDAL_L_CLOSE_DIR, PEDAL_L_CLOSE_FAST_VOL);
			 }
			 else if(pedal_l.phase == _phase_slow)
			 {
				 set_pedal_l_dir_vol(PEDAL_L_CLOSE_DIR, PEDAL_L_CLOSE_SLOW_VOL);
			 }
		 }
 
		 else
		 {
			 set_pedal_l_dir_vol(PEDAL_L_STOP_DIR, PEDAL_L_STOP_VOL);
		 }
	 }
 }
 




/**********************************************************
  * @brief	PEDAL 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void pedal_l_timer(void);
static void pedal_l_hall_timer_1ms(void);

void app_pedal_init(void)
{
	motor_init();
	hal_drv8718_init();	
	
	app_register_timer_callback(pedal_l_hall_timer_1ms, TIMER_1MS_CALLBACK_TYPE);
	app_register_timer_callback(pedal_l_timer, TIMER_10MS_CALLBACK_TYPE);
}





/******************** 待机进程 ********************/
static void Pedal_L_Stage0(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 电机 ***************/
		set_pedal_l_dir_vol(PEDAL_L_STOP_DIR, PEDAL_L_STOP_VOL);
		motor_set_threshold(&motor_l, &pedal_l_idle_th);
		
		/*************** 踏板 ***************/
		
		
		/*************** 霍尔 ***************/
		
		
		/*************** 蜂鸣器/指示灯 ***************/
		
		
		/*************** 标志位 ***************/
		motor_clear_state(&motor_l);
		pedal_l_accomplish = false;
		
		
		/*************** 进程 ***************/

	}
}

/******************** 伸出启动进程 ********************/
static void Pedal_L_Stage1(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 清除错误标志 ***************/
		hal_drv8718_clr_flt();
		
		
		/*************** 打开预驱通道 ***************/
		
		
		/*************** 标志位 ***************/
		motor_clear_state(&motor_l);
		
		
		/*************** 进程 ***************/
		stage_pedal_l = 2;
		Pedal_L_Pend_Delay(3);
	}
}

/******************** 伸出启动进程 ********************/
static void Pedal_L_Stage2(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 电机 ***************/
		motor_set_threshold(&motor_l, &pedal_l_open_th);
		pedal_l_travel_reset_cnt();
		set_pedal_l_dir_vol(PEDAL_L_OPEN_DIR, PEDAL_L_OPEN_START_VOL);
		
		
		/*************** 踏板 ***************/
		pedal_l.place = _pedal_opening;
		
		
		/*************** 霍尔 ***************/
		motor_hall_power_on();
		
		
		/*************** 蜂鸣器/指示灯 ***************/
		
		
		/*************** 标志位 ***************/
		motor_l.state.time_run = 1;
		
		
		/*************** 进程 ***************/
		stage_pedal_l = 3;
		Pedal_L_Pend_Delay(PEDAL_L_OPEN_RUN_TIME);
	}
}


/******************** 伸出运行进程 ********************/
static void Pedal_L_Stage3(void)
{
	pedal_l_pos_update();
	pedal_l_phase_update();
	
	// /******************** 缓速段 ********************/
	// if(pedal_l.flag_slow_run==true && pedal_l.flag_slow_run_finish==false)	
	// {
	// 	set_pedal_l_dir_vol(PEDAL_L_OPEN_DIR, PEDAL_L_OPEN_SLOW_VOL);
		
	// 	pedal_l.flag_slow_run_finish = true;
	// }
	// /******************** 加速段 ********************/
	// else if(pedal_l.flag_fast_run==true && pedal_l.flag_slow_run==false && pedal_l.flag_fast_run_finish==false)	
	// {
	// 	set_pedal_l_dir_vol(PEDAL_L_OPEN_DIR, PEDAL_L_OPEN_FAST_VOL);		
	
	// 	pedal_l.flag_fast_run_finish = true;
	// }	
	
	/*************** 电流防夹 ***************/
	if(true == motor_get_flag_current_pinch(&motor_l))
	{
		stage_pedal_l = 4;
		Pedal_L_Pend_Delay(0);
	}
		
	/*************** 电流堵转 ***************/
	if(true == motor_get_flag_current_stall(&motor_l))
	{
		stage_pedal_l = 4;
		Pedal_L_Pend_Delay(0);
	}
	
	/*************** 电流短路 ***************/
	if(true == motor_get_flag_short_current(&motor_l))
	{
		stage_pedal_l = 4;
		Pedal_L_Pend_Delay(0);
	}
	
	/*************** 霍尔防夹 ***************/
	if(true == motor_get_flag_hall_pinch(&motor_l))
	{
		stage_pedal_l = 4;
		Pedal_L_Pend_Delay(0);
	}
		
	/*************** 霍尔堵转 ***************/
	if(true == motor_get_flag_hall_stall(&motor_l))
	{
		stage_pedal_l = 4;
		Pedal_L_Pend_Delay(0);
	}
	
//	/*************** 霍尔丢失 ***************/
//	if(true == motor_get_flag_hall_lose(&motor_l))
//	{
//		stage_pedal_l = 4;
//		Pedal_L_Pend_Delay(0);
//	}
	
	/*************** 超时 ***************/
	if(true == pedal_l_accomplish)
	{
		stage_pedal_l = 4;
		Pedal_L_Pend_Delay(0);
	}
}

/******************** 伸出停止进程 ********************/
static void Pedal_L_Stage4(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 电机 ***************/
		set_pedal_l_dir_vol(PEDAL_L_STOP_DIR, PEDAL_L_STOP_VOL);
		motor_set_threshold(&motor_l, &pedal_l_idle_th);

		
		/*************** 踏板 ***************/
		pedal_l.place = _pedal_open;
		
		
		/*************** 霍尔 ***************/
		if(true == motor_get_flag_current_stall(&motor_l))
		{
			pedal_l_set_travel_pos(PEDAL_L_OPEN_STOP_POS);
		}
		
		
		/*************** 蜂鸣器/指示灯 ***************/
		
		
		/*************** 标志位 ***************/
		pedal_l.cnt_continue_pinch = (motor_get_flag_current_pinch(&motor_l)==true || motor_get_flag_hall_pinch(&motor_l)==true) ? ((pedal_l.cnt_continue_pinch>=PEDAL_L_PINCH_CNT_MAX) ? (PEDAL_L_PINCH_CNT_MAX):(pedal_l.cnt_continue_pinch+1)):(pedal_l.flag_pinch_back==false ? 0:pedal_l.cnt_continue_pinch);
		pedal_l.flag_pinch_back = ((motor_get_flag_current_pinch(&motor_l)==true || motor_get_flag_hall_pinch(&motor_l)==true) && pedal_l.cnt_continue_pinch<PEDAL_L_PINCH_CNT_MAX) ? true:false;
		
		motor_clear_state(&motor_l);
		

		/*************** 进程 ***************/
		if(pedal_l.flag_pinch_back==true)
		{						
			stage_pedal_l = 11;
			Pedal_L_Pend_Delay(PEDAL_L_PINCH_BACK_DELAY_TIME);
		}
		else
		{
#if (PEDAL_L_OPEN_HOLD_ENABLE==1)			
			stage_pedal_l = 5;
			Pedal_L_Pend_Delay(2);
#else 			
			stage_pedal_l=0;
			Pedal_L_Pend_Delay(0);
#endif			
		}
	}
}

/******************** 伸出保持/刹车进程 ********************/
static void Pedal_L_Stage5(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 电机 ***************/
		set_pedal_l_dir_vol(PEDAL_L_OPEN_DIR, PEDAL_L_OPEN_HOLD_VOL);
		
		
		/*************** 踏板 ***************/
		
		
		/*************** 霍尔 ***************/
		
		
		/*************** 蜂鸣器/指示灯 ***************/
		
		
		/*************** 标志位 ***************/
		
		
		/*************** 进程 ***************/
		stage_pedal_l = 0;
		Pedal_L_Pend_Delay(PEDAL_L_OPEN_HOLD_TIME);
	}
}


/******************** 进程 ********************/
static void Pedal_L_Stage6(void)
{
	
}

static void Pedal_L_Stage7(void)
{

}

static void Pedal_L_Stage8(void)
{

}

static void Pedal_L_Stage9(void)
{

}

static void Pedal_L_Stage10(void)
{

}


/******************** 收回启动进程 ********************/
static void Pedal_L_Stage11(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 清除错误标志 ***************/
		hal_drv8718_clr_flt();
		
		/*************** 打开预驱通道 ***************/
		motor_clear_state(&motor_l);
		
		/*************** 进程 ***************/
		stage_pedal_l = 12;
		Pedal_L_Pend_Delay(3);
	}
}

/******************** 收回启动进程 ********************/
static void Pedal_L_Stage12(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 电机 ***************/
		motor_set_threshold(&motor_l, &pedal_l_close_th);
		pedal_l_travel_reset_cnt();
		set_pedal_l_dir_vol(PEDAL_L_CLOSE_DIR, PEDAL_L_CLOSE_START_VOL);
		
		
		/*************** 踏板 ***************/
		pedal_l.place = _pedal_closing;
		
		
		/*************** 霍尔 ***************/
		motor_hall_power_on();
		
		
		/*************** 蜂鸣器/指示灯 ***************/
		
		
		/*************** 标志位 ***************/
		motor_l.state.time_run = 1;
		
		/*************** 进程 ***************/
		stage_pedal_l = 13;
		Pedal_L_Pend_Delay(PEDAL_L_CLOSE_RUN_TIME);
	}
}


/******************** 收回运行进程 ********************/
static void Pedal_L_Stage13(void)
{
	pedal_l_pos_update();
	pedal_l_phase_update();

	// /******************** 缓速段 ********************/
	// if(pedal_l.flag_slow_run==true && pedal_l.flag_slow_run_finish==false)	
	// {
	// 	set_pedal_l_dir_vol(PEDAL_L_CLOSE_DIR, PEDAL_L_CLOSE_SLOW_VOL);
		
	// 	pedal_l.flag_slow_run_finish = true;
	// }
	// /******************** 加速段 ********************/
	// else if(pedal_l.flag_fast_run==true && pedal_l.flag_slow_run==false && pedal_l.flag_fast_run_finish==false)	
	// {
	// 	set_pedal_l_dir_vol(PEDAL_L_CLOSE_DIR, PEDAL_L_CLOSE_FAST_VOL);		
	
	// 	pedal_l.flag_fast_run_finish = true;
	// }	
	
	/*************** 电流防夹 ***************/
	if(true == motor_get_flag_current_pinch(&motor_l))
	{
		stage_pedal_l = 14;
		Pedal_L_Pend_Delay(0);
	}
		
	/*************** 电流堵转 ***************/
	if(true == motor_get_flag_current_stall(&motor_l))
	{
		stage_pedal_l = 14;
		Pedal_L_Pend_Delay(0);
	}
	
	/*************** 电流短路 ***************/
	if(true == motor_get_flag_short_current(&motor_l))
	{
		stage_pedal_l = 14;
		Pedal_L_Pend_Delay(0);
	}
	
	/*************** 霍尔防夹 ***************/
	if(true == motor_get_flag_hall_pinch(&motor_l))
	{
		stage_pedal_l = 14;
		Pedal_L_Pend_Delay(0);
	}
		
	/*************** 霍尔堵转 ***************/
	if(true == motor_get_flag_hall_stall(&motor_l))
	{
		stage_pedal_l = 14;
		Pedal_L_Pend_Delay(0);
	}
	
//	/*************** 霍尔丢失 ***************/
//	if(true == motor_get_flag_hall_lose(&motor_l))
//	{
//		stage_pedal_l = 14;
//		Pedal_L_Pend_Delay(0);
//	}
	
	/*************** 超时 ***************/
	if(true == pedal_l_accomplish)
	{
		stage_pedal_l = 14;
		Pedal_L_Pend_Delay(0);
	}
}

/******************** 收回停止进程 ********************/
static void Pedal_L_Stage14(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 电机 ***************/
		set_pedal_l_dir_vol(PEDAL_L_STOP_DIR, PEDAL_L_STOP_VOL);
		motor_set_threshold(&motor_l, &pedal_l_idle_th);
		
		
		/*************** 踏板 ***************/
		pedal_l.place = _pedal_close;
		
		
		/*************** 霍尔 ***************/
		if(true == motor_get_flag_current_stall(&motor_l))
		{
			pedal_l_set_travel_pos(PEDAL_L_CLOSE_STOP_POS);
		}
		
		
		/*************** 蜂鸣器/指示灯 ***************/
		
		
		/*************** 标志位 ***************/
		pedal_l.cnt_continue_pinch = (motor_get_flag_current_pinch(&motor_l)==true || motor_get_flag_hall_pinch(&motor_l)==true) ? ((pedal_l.cnt_continue_pinch>=PEDAL_L_PINCH_CNT_MAX) ? (PEDAL_L_PINCH_CNT_MAX):(pedal_l.cnt_continue_pinch+1)):(pedal_l.flag_pinch_back==false ? 0:pedal_l.cnt_continue_pinch);
		pedal_l.flag_pinch_back = ((motor_get_flag_current_pinch(&motor_l)==true || motor_get_flag_hall_pinch(&motor_l)==true) && pedal_l.cnt_continue_pinch<PEDAL_L_PINCH_CNT_MAX) ? true:false;
		
		motor_clear_state(&motor_l);
		
		
		/*************** 进程 ***************/
		if(pedal_l.flag_pinch_back==true)
		{						
			stage_pedal_l = 1;
			Pedal_L_Pend_Delay(PEDAL_L_PINCH_BACK_DELAY_TIME);
		}
		else
		{
#if (PEDAL_L_OPEN_HOLD_ENABLE==1)			
			stage_pedal_l = 15;
			Pedal_L_Pend_Delay(2);
#else 			
			stage_pedal_l=0;
			Pedal_L_Pend_Delay(0);
#endif			
		}
	}
}

/******************** 收回保持/刹车进程 ********************/
static void Pedal_L_Stage15(void)
{
	if(true == pedal_l_accomplish)
	{
		/*************** 电机 ***************/
		set_pedal_l_dir_vol(PEDAL_L_CLOSE_DIR, PEDAL_L_CLOSE_HOLD_VOL);
		
		
		/*************** 踏板 ***************/
		
		
		/*************** 霍尔 ***************/
		
		
		/*************** 蜂鸣器/指示灯 ***************/
		
		
		/*************** 标志位 ***************/
		
		
		/*************** 进程 ***************/
		stage_pedal_l = 0;
		Pedal_L_Pend_Delay(PEDAL_L_CLOSE_HOLD_TIME);
	}
}

static void Pedal_L_Stage16(void)
{

}

static void Pedal_L_Stage17(void)
{

}

static void Pedal_L_Stage18(void)
{

}

static void Pedal_L_Stage19(void)
{

}

static void Pedal_L_Stage20(void)
{

}


void pedal_l_stage(void)
{
	switch(stage_pedal_l)
	{
		case 0:
			Pedal_L_Stage0();
			break;
		
		case 1:
			Pedal_L_Stage1();
			break;
		case 2:
			Pedal_L_Stage2();
			break;
		case 3:
			Pedal_L_Stage3();
			break;
		case 4:
			Pedal_L_Stage4();
			break;
		case 5:
			Pedal_L_Stage5();
			break;
		case 6:
			Pedal_L_Stage6();
			break;
		case 7:
			Pedal_L_Stage7();
			break;		
		case 8:
			Pedal_L_Stage8();
			break;
		case 9:
			Pedal_L_Stage9();
			break;
		case 10:
			Pedal_L_Stage10();
			break;

		case 11:
			Pedal_L_Stage11();
			break;
		case 12:
			Pedal_L_Stage12();
			break;
		case 13:
			Pedal_L_Stage13();
			break;
		case 14:
			Pedal_L_Stage14();
			break;
		case 15:
			Pedal_L_Stage15();
			break;		
		case 16:
			Pedal_L_Stage16();
			break;
		case 17:
			Pedal_L_Stage17();
			break;
		case 18:
			Pedal_L_Stage18();
			break;
		case 19:
			Pedal_L_Stage19();
			break;
		case 20:
			Pedal_L_Stage20();
			break;

		default:
			break;
	}
}




static uint16_t pedal_l_timer_count=0;
static uint16_t pedal_l_continue_ten_ms=0;
bool pedal_l_accomplish=false;

void Pedal_L_Pend_Delay(uint16_t pedal_l_ten_ms)	//非阻塞延时
{
	if(pedal_l_ten_ms==0)	
	{
		pedal_l_accomplish=true;	//不延时
	}
	else
	{
		pedal_l_continue_ten_ms=pedal_l_ten_ms;
		pedal_l_timer_count=0;
		pedal_l_accomplish=false;
	}
}

/**********************************************************
  * @brief	左踏板 霍尔 1ms 节拍
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void pedal_l_hall_timer_1ms(void)
{
	motor_hall_tick(&motor_l);
	pedal_l_travel_pos_update();
}

/**********************************************************
  * @brief	左踏板 定时器
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void pedal_l_timer(void)
{
	// if(pedal_l_accomplish==true || pedal_l_continue_ten_ms==0)
	// {
	// 	pedal_l_accomplish=true;
	// }
	// else
	// {
	// 	pedal_l_timer_count++;
	// 	if(pedal_l_timer_count==pedal_l_continue_ten_ms)
	// 	{
	// 		pedal_l_accomplish=true;
	// 	}
	// 	if(pedal_l_timer_count>pedal_l_continue_ten_ms)
	// 	{
	// 		pedal_l_accomplish=true;
	// 		pedal_l_timer_count=pedal_l_continue_ten_ms+1;
	// 	}
	// }
	

	if((pedal_l_accomplish == false) && (pedal_l_continue_ten_ms != 0))
	{
		pedal_l_timer_count++;
		if(pedal_l_timer_count >= pedal_l_continue_ten_ms)
		{
			pedal_l_accomplish = true;
			pedal_l_timer_count = pedal_l_continue_ten_ms;
		}
	}
	

	pedal_l_pos_update();
	
	/*************** 电机采样节拍 ***************/
	motor_current_tick(&motor_l);
	motor_state_tick(&motor_l);
}





