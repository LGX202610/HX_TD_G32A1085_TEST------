#ifndef DRV_TMR_PWM_H
#define DRV_TMR_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Define */




 /* Enum */ 



/* Struct */ 






void drv_tmr_pwm_init(void);
void drv_tmr_pwm_sleep(void);
void drv_tmr_pwm_wakeup(void);
void drv_set_tmr1_ch_comp(uint8_t ch, uint16_t comp);
void drv_enable_tmr1_pwm_output(void);
void drv_disable_tmr1_pwm_output(void);


#ifdef __cplusplus
}
#endif

#endif /*  */
