#ifndef APP_MOTOR_H
#define APP_MOTOR_H



#ifdef __cplusplus
extern "C" {
#endif





/* Includes */
#include <stdint.h>
#include <stdbool.h>
#include "motor_types.h"


/*
 当前motor组件成员对外可见，约定仅通过api操作组件，而不能直接操作成员
 可考虑做成不透明指针实例，当前调试阶段保留
*/



/* Struct */
/*************** 电机运行参数 ***************/
typedef struct
{
	MotorDir_t		dir;					// 转向
	float					pwmduty;			// 占空比
	float					voltage;			// 驱动电压
	uint16_t			adc;					// 电流ADC值

	uint8_t				hall_value;		// 当前霍尔电平		
	uint16_t			hall_pos;			// 霍尔位置		
	uint16_t			hall_cnt;			// 霍尔跳变计数

} MotorParam_t;





/*************** 电机运行状态 ***************/
typedef struct
{
	uint16_t time_run;				// 运行时间	
	uint16_t time_current_pinch;	// 电流防夹计时
	uint16_t time_current_stall;	// 电流堵转计时
	uint16_t time_short_current;	// 短路计时
	uint16_t time_hall_pinch;		// 霍尔防夹计时
	uint16_t time_hall_stall;		// 霍尔堵转计时
	uint16_t time_hall_lose;		// 霍尔丢失计时

	bool flag_ovortime;				// 运行超时
	bool flag_current_pinch;		// 电流防夹
	bool flag_current_stall;		// 电流堵转
	bool flag_short_current;		// 电流短路

	bool flag_hall_pinch;			// 霍尔防夹
	bool flag_hall_stall;			// 霍尔堵转
	bool flag_hall_lose;			// 霍尔丢失

} MotorState_t;





/*************** 电机保护阈值 ***************/
typedef struct
{
	uint16_t pinch_current;		// 电流防夹阈值
	uint16_t stall_current;		// 电流堵转阈值
	uint16_t short_current;		// 短路电流阈值
	uint16_t pinch_time;			// 电流防夹持续时间
	uint16_t stall_time;			// 电流堵转持续时间
	uint16_t short_time;			// 短路持续时间
	uint16_t hall_pinch_time;		// 霍尔防夹持续时间
	uint16_t hall_stall_time;		// 霍尔堵转持续时间
	uint16_t hall_lose_time;		// 霍尔丢失持续时间
	uint16_t run_time_max;			// 运行超时阈值，

} MotorThreshold_t;





/*************** 电机功能使能 ***************/
typedef struct
{
	bool current_pinch;			// 电流防夹使能
	bool current_stall;			// 电流堵转使能
	bool current_short;			// 短路电流使能

	bool hall_pinch;			// 霍尔防夹使能
	bool hall_stall;			// 霍尔堵转使能

} MotorFuncEn_t;





/*************** 电机实例 ***************/
typedef struct
{
	MotorParam_t					param;			// 运行参数，组件 tick/驱动 更新
	MotorState_t					state;			// 保护状态，组件 tick 更新
	MotorThreshold_t			threshold;	// 保护阈值，外部配置
	MotorFuncEn_t					func_en;		// 保护使能，外部配置
	MotorInstId_t					id;					// 硬件实例 ID
	
} MotorInst_t;






void motor_init(void);
void motor_sleep(void);
void motor_wakeup(void);
void motor_hall_power_on(void);
void motor_hall_power_off(void);

void motor_inst_init(MotorInst_t *motor, MotorInstId_t id);
void motor_set_dir_vol(MotorInst_t *motor, MotorDir_t dir, float vol, float vin);

void motor_hall_tick(MotorInst_t *motor);
void motor_current_tick(MotorInst_t *motor);
void motor_state_tick(MotorInst_t *motor);


/******************************************** 电机阈值类API ********************************************/
void motor_set_threshold(MotorInst_t *motor, const MotorThreshold_t *th);
void motor_set_pinch_current_th(MotorInst_t *motor, uint16_t adc);
void motor_set_pinch_time_th(MotorInst_t *motor, uint16_t time);
void motor_set_stall_current_th(MotorInst_t *motor, uint16_t adc);
void motor_set_stall_time_th(MotorInst_t *motor, uint16_t time);
void motor_set_short_current_th(MotorInst_t *motor, uint16_t adc);
void motor_set_short_time_th(MotorInst_t *motor, uint16_t time);
void motor_set_hall_pinch_time_th(MotorInst_t *motor, uint16_t time);
void motor_set_hall_stall_time_th(MotorInst_t *motor, uint16_t time);
void motor_set_hall_lose_time_th(MotorInst_t *motor, uint16_t time);
void motor_set_run_timeout_th(MotorInst_t *motor, uint16_t ten_ms);


/******************************************** 电机功能类API ********************************************/
void motor_set_current_pinch_en(MotorInst_t *motor, bool en);
void motor_set_current_stall_en(MotorInst_t *motor, bool en);
void motor_set_current_short_en(MotorInst_t *motor, bool en);
void motor_set_hall_pinch_en(MotorInst_t *motor, bool en);
void motor_set_hall_stall_en(MotorInst_t *motor, bool en);
void motor_set_hall_lose_en(MotorInst_t *motor, bool en);
void motor_set_run_timeout_en(MotorInst_t *motor, bool en);


/******************************************** 电机状态类API ********************************************/
MotorDir_t motor_get_dir(const MotorInst_t *motor);
float motor_get_pwmduty(const MotorInst_t *motor);
float motor_get_voltage(const MotorInst_t *motor);
uint16_t motor_get_adc(const MotorInst_t *motor);
void motor_set_hall_pos(MotorInst_t *motor, uint16_t pos);
uint8_t motor_get_hall(const MotorInst_t *motor);
uint16_t motor_get_hall_pos(const MotorInst_t *motor);
uint16_t motor_get_hall_cnt(const MotorInst_t *motor);
uint16_t motor_get_time_run(const MotorInst_t *motor);
uint16_t motor_get_time_current_pinch(const MotorInst_t *motor);
uint16_t motor_get_time_current_stall(const MotorInst_t *motor);
uint16_t motor_get_time_short_current(const MotorInst_t *motor);
uint16_t motor_get_time_hall_pinch(const MotorInst_t *motor);
uint16_t motor_get_time_hall_stall(const MotorInst_t *motor);
uint16_t motor_get_time_hall_lose(const MotorInst_t *motor);
bool motor_get_flag_ovortime(const MotorInst_t *motor);
bool motor_get_flag_current_pinch(const MotorInst_t *motor);
bool motor_get_flag_current_stall(const MotorInst_t *motor);
bool motor_get_flag_short_current(const MotorInst_t *motor);
bool motor_get_flag_hall_pinch(const MotorInst_t *motor);
bool motor_get_flag_hall_stall(const MotorInst_t *motor);
bool motor_get_flag_hall_lose(const MotorInst_t *motor);

void motor_start_run_timer(MotorInst_t *motor);
void motor_stop_run_timer(MotorInst_t *motor);
void motor_clear_state(MotorInst_t *motor);
void motor_clear_hall_travel(MotorInst_t *motor);

#ifdef __cplusplus

}

#endif



#endif /* APP_MOTOR_H */

