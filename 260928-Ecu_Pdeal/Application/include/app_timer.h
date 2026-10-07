#ifndef APP_TIMER_H
#define APP_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes */
#include <stdint.h>
#include <stddef.h>



/* Define */
#define TIMER_1MS_CALLBACK_MAX	3							//允许注册的最大数量
#define TIMER_10MS_CALLBACK_MAX	15						//允许注册的最大数量


 /* Enum */ 


typedef enum
{
    TIMER_1MS_CALLBACK_TYPE = 0,   // 1ms 回调
    TIMER_10MS_CALLBACK_TYPE = 1   // 10ms 回调
}App_Timer_CallbackType_t;




/* Struct */ 


typedef void (*App_Timer_Callback_t)(void);


void app_timer_init(void);
int app_register_timer_callback(App_Timer_Callback_t callback, App_Timer_CallbackType_t callback_type);

	


#ifdef __cplusplus
}
#endif

#endif /*  */
