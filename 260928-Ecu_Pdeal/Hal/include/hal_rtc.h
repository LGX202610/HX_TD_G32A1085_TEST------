#ifndef HAL_RTC_H
#define HAL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdbool.h>



/* Define */





/* Enum */ 




/* Struct */ 


void hal_rtc_init(void);
void hal_rtc_reset_time(void);
bool hal_rtc_get_wakeup_flag(void);
void hal_rtc_clear_wakeup_flag(void);
void hal_rtc_enable_wakeup_alarm(void);
void hal_rtc_disable_wakeup_alarm(void);


#ifdef __cplusplus
}
#endif

#endif /*  */
