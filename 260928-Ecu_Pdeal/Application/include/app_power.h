#ifndef APP_POWER_H
#define APP_POWER_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes */
#include <stdint.h>
#include <stdbool.h>



/* Define */
/********************  ********************/
#define ECU_START_TIME									1000			//上电滤波时间，单位10ms
#define ECU_SLEEP_TIME									100			//休眠时间，单位10ms


/******************** 高低压保护 ********************/
#define VIN_PROTECT_ENABLE								1				//0无高低压保护，1高低压保护
#define LOW_VIN_PROTECT_THRESHOLD					(8.8f)				//低压保护阈值
#define LOW_VIN_RECOVER_THRESHOLD					(9.2f)				//低压恢复阈值
#define HIGH_VIN_PROTECT_THRESHOLD				    (16.2f)				//高压保护阈值
#define HIGH_VIN_RECOVER_THRESHOLD				    (15.8f)				//高压恢复阈值
#define VIN_DETECT_PERIOD								50				//母线电压检测周期，单位 10ms
#define VIN_PROTECT_PERIOD								3				//触发阈值持续几次周期


/* Enum */ 




/* Struct */ 

extern bool flag_ecu_start;
extern bool flag_ecu_sleep;
extern bool flag_eint_wakeup;

extern uint16_t vin_adc;
extern float vin_voltage;
extern bool flag_low_vin_protect;
extern bool flag_high_vin_protect;



void app_power_init(void);
void app_power_main(void);
void ecu_power_sleep_wakeup(void);
void app_power_keep_awake(void);
void app_power_sleep_inhibit(void);
void app_power_sleep_allow(void);



#ifdef __cplusplus
}
#endif

#endif /*  */

