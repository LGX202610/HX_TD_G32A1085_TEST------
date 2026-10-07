#ifndef DRV_RTC_H
#define DRV_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Define */




/* Enum */ 



/* Struct */ 

typedef void (*Drv_Rtc_IrqHookFunc_t)(void);


void drv_rtc_init(void);
void drv_rtc_reset_time(void);
void drv_rtc_config_alarm(uint8_t seconds);
void drv_rtc_enable_wakeup_alarm(void);
void drv_rtc_disable_wakeup_alarm(void);
void drv_rtc_irq_handle(void);
void drv_register_rtc_irq_hook(Drv_Rtc_IrqHookFunc_t hook_func);



#ifdef __cplusplus
}
#endif

#endif /*  */
