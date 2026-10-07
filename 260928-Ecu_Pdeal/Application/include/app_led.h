#ifndef APP_LED_H
#define APP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 实际灯数 */
#define APP_LED_CH_USED		2u

/* 通道号 */
#define LED_CH_1		0u
#define LED_CH_2		1u  






/*
 * 灯语。任意通道都可请求同一张表。
 * 枚举值必须与 app_led.c 中 s_led_tbl[] 下标一致。
 */
typedef enum
{
    /* 系统类 */
	APP_LED_IND_IDLE = 0,           /* 该灯灭 */
	APP_LED_IND_ON,	                /* 该灯常亮 */
	APP_LED_IND_NORMAL,             /* 慢闪，持续 */

	/* 业务类 */
	APP_LED_IND_VIN_LOW,            /* 欠压 */
	APP_LED_IND_VIN_HIGH,           /* 过压 */
	APP_LED_IND_PEDAL_PINCH,        /* 触发防夹 */
    APP_LED_IND_MOTOR_OC,           /* 电机开路 */
    APP_LED_IND_MOTOR_SC,           /* 电机短路 */
    APP_LED_IND_DRVER_FAULT,        /* 驱动器故障 */




	APP_LED_IND_FAULT			    /* 快速有限次闪，prio 最高 */
} AppLedIndicate_t;



typedef uint8_t AppLedChannel_t;

/* 初始化 */    
void app_led_init(void);
/* 休眠 */
void app_led_sleep(void);
/* 唤醒 */
void app_led_wakeup(void);	
/* 指示 */
void app_led_indicate(AppLedChannel_t ch, AppLedIndicate_t ind);
/* 清除指示 */
void app_led_clear(AppLedChannel_t ch, AppLedIndicate_t ind);


#ifdef __cplusplus
}
#endif

#endif