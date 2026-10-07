#ifndef HAL_MOTOR_H
#define HAL_MOTOR_H


#ifdef __cplusplus
extern "C" {
#endif




/* Includes */
#include "stdbool.h"
#include <stdint.h>
#include "motor_types.h"





/* Define */







/* Enum */








void hal_motor_init(void);
void hal_motor_sleep(void);
void hal_motor_wakeup(void);

void hal_hall_power_on(void);
void hal_hall_power_off(void);



void hal_motor_set_output(MotorInstId_t id, MotorDir_t dir, uint16_t pwm_comp);
uint8_t hal_motor_get_hall(MotorInstId_t id);
uint16_t hal_motor_get_current_adc(MotorInstId_t id);





#ifdef __cplusplus

}

#endif



#endif /* HAL_MOTOR_H */

