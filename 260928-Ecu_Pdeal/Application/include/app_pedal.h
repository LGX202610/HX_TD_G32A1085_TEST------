#ifndef APP_PEDAL_H
#define APP_PEDAL_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes */
#include <stdint.h>
#include "stdbool.h"
#include "motor_types.h"



/* Define */



/******************** 方向宏 ********************/
#define	PEDAL_L_OPEN_DIR				(MOTOR_DIR_CCW)			//伸出方向
#define	PEDAL_L_CLOSE_DIR				(MOTOR_DIR_CW)			//收回方向
#define	PEDAL_L_STOP_DIR				(MOTOR_DIR_STOP)		//停止方向
#define	PEDAL_L_BRAKE_DIR				(MOTOR_DIR_BRAKE)		//刹车方向		

#define PEDAL_L_TRAVEL_MIN				(0u)
#define PEDAL_L_TRAVEL_MAX				(1000u)

/* 1: OPEN方向travel_pos递增；0: OPEN方向递减（实机标定） */
#define PEDAL_L_OPEN_TRAVEL_INC			(1u)



/******************** 驱动宏 ********************/
#define	PEDAL_L_OPEN_START_VOL				(8.0f)			//伸出启动段电压
#define	PEDAL_L_OPEN_FAST_VOL				(12.2f)			//伸出加速段电压
#define	PEDAL_L_OPEN_SLOW_VOL				(8.0f)			//伸出减速段电压
#define	PEDAL_L_OPEN_HOLD_VOL				(1.4f)			//伸出保持电压

#define	PEDAL_L_CLOSE_START_VOL				(8.0f)			//收回启动段电压
#define	PEDAL_L_CLOSE_FAST_VOL				(12.2f)			//收回加速段电压
#define	PEDAL_L_CLOSE_SLOW_VOL				(8.0f)			//收回减速段电压
#define	PEDAL_L_CLOSE_HOLD_VOL				(1.2f)			//收回保持电压

#define	PEDAL_L_STOP_VOL						(0.0f)			//停止电压
#define	PEDAL_L_BRAKE_VOL						(0.0f)			//刹车电压



/******************** 时间宏，单位10ms ********************/
#define	PEDAL_L_OPEN_RUN_TIME									350				//伸出 运行时间
#define	PEDAL_L_OPEN_PINCH_TIME								10				//伸出 防夹时间
#define	PEDAL_L_OPEN_CURRENT_STALL_TIME				20				//伸出 电流堵转时间
#define	PEDAL_L_OPEN_SHORT_CURRENT_TIME				5					//伸出 电流短路时间
#define	PEDAL_L_OPEN_HALL_PINCH_TIME					20				//伸出 霍尔防夹时间
#define	PEDAL_L_OPEN_HALL_STALL_TIME					15				//伸出 霍尔堵转时间
#define	PEDAL_L_OPEN_HOLD_TIME								70				//伸出 保持时间
#define	PEDAL_L_OPEN_BRAKE_TIME								10				//伸出 刹车时间
#define	PEDAL_L_OPEN_PINCH_ENABLE_TIME				50				//伸出 防夹启动时间
#define	PEDAL_L_OPEN_STALL_ENABLE_TIME				50				//伸出 堵转启动时间

	
#define	PEDAL_L_CLOSE_RUN_TIME								350				//收回 运行时间	
#define	PEDAL_L_CLOSE_PINCH_TIME							12				//收回 防夹时间
#define	PEDAL_L_CLOSE_CURRENT_STALL_TIME			20				//收回 电流堵转时间
#define	PEDAL_L_CLOSE_SHORT_CURRENT_TIME			5					//收回 电流短路时间
#define	PEDAL_L_CLOSE_HALL_PINCH_TIME					20				//收回 霍尔防夹时间
#define	PEDAL_L_CLOSE_HALL_STALL_TIME					15				//收回 霍尔堵转时间
#define	PEDAL_L_CLOSE_HOLD_TIME								70				//收回 保持时间
#define	PEDAL_L_CLOSE_BRAKE_TIME							10				//收回 刹车时间
#define	PEDAL_L_CLOSE_PINCH_ENABLE_TIME				50				//收回 防夹启动时间
#define	PEDAL_L_CLOSE_STALL_ENABLE_TIME				50				//收回 堵转启动时间


#define PEDAL_L_HALL_LOSE_TIME								100




/******************** 电流宏 ********************/
#define	PEDAL_L_OPEN_PINCH_ADC							1200					//伸出防夹电流，
#define	PEDAL_L_OPEN_STALL_ADC							1600				//伸出堵转电流

#define	PEDAL_L_CLOSE_PINCH_ADC							1200					//收回防夹电流，
#define	PEDAL_L_CLOSE_STALL_ADC							1600				//收回堵转电流




/******************** 霍尔宏 ********************/
#define PEDAL_L_OPEN_START_POS						0
#define PEDAL_L_OPEN_FAST_POS							100
#define PEDAL_L_OPEN_SLOW_POS							800
#define PEDAL_L_OPEN_STOP_POS							1000

#define PEDAL_L_CLOSE_START_POS						1000
#define PEDAL_L_CLOSE_FAST_POS						800
#define PEDAL_L_CLOSE_SLOW_POS						200
#define PEDAL_L_CLOSE_STOP_POS						10




/******************** 功能宏 ********************/
#define PEDAL_L_PINCH_CNT_MAX							6						//连续防夹次数
#define PEDAL_L_PINCH_BACK_DELAY_TIME			5						//防夹反转时间

#define PEDAL_L_OPEN_HOLD_ENABLE					1						//1，伸出到位后启动保持
#define PEDAL_L_CLOSE_HOLD_ENABLE					1						//1，收回到位后启动保持





/* Enum */ 
/*************** 踏板状态 ***************/
typedef enum            
{
	_pedal_stop  		=0,		//上电停止
	_pedal_closing 	=1,		//正在关闭
	_pedal_opening  =2,		//正在打开	
	_pedal_open  		=3,		//打开	
	_pedal_close  	=4,		//关闭		
}PedalPlace;	


/*************** 踏板动作指令 ***************/
typedef enum            
{
	_com_idle  		=0,		//
	_com_stop  		=1,		//停止
	_com_close 		=2,		//关闭
	_com_open  		=3,		//打开	
}PedalCommand;	


/*************** 防夹力档位 ***************/
typedef enum            
{
	_pinch_disable  =0,		
	_pinch_1  			=1,		
	_pinch_2 				=2,		
	_pinch_3  			=3,
	_pinch_4  			=4,
	_pinch_5  			=5,	
}PinchGear;		


/*************** 踏板运行阶段 ***************/
typedef enum            
{
	_phase_stop  		=0,		//停止
	_phase_start  	=1,		//启动
	_phase_fast  		=2,		//快速
	_phase_slow  		=3,		//慢速
}PedalPhase;	


/* Struct */ 
/*************** 电机状态 ***************/
typedef struct           
{
	uint32_t cnt_runs;						//运行次数
	uint32_t hall_pos_min;				//最小行程限制
	uint32_t hall_pos_max;				//最大行程限制

	uint16_t travel_pos;				//业务行程坐标
	uint16_t travel_pos_last_cnt;		//hall_cnt快照
	
		
	bool flag_pinch_enable;				//防夹使能标志位
	bool flag_pinch_back;					//防夹反转标志位	
	uint8_t cnt_continue_pinch;		//连续防夹次数
	
	PinchGear pinch_gear;					//防夹档位	
	PedalCommand cmd;							//踏板指令
	PedalPlace place;							//踏板位置
	PedalPhase phase;							//踏板阶段
	
}PedalInst;



extern bool pedal_l_accomplish;
uint16_t pedal_l_get_travel_pos(void);
void pedal_l_set_travel_pos(uint16_t travel_pos);
void app_pedal_init(void);
void Pedal_L_Pend_Delay(uint16_t pedal_l_ten_ms);	//非阻塞延时
void pedal_l_stage(void);

void set_pedal_l_open(void);
void set_pedal_l_close(void);


#ifdef __cplusplus
}
#endif

#endif /*  */
