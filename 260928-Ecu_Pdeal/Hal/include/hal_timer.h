#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */





/* Define */





/* Enum */ 




/* Struct */ 

typedef void (*Hal_Timer_IrqHookFunc_t)(void);


void hal_timer_init(void);
void hal_timer_sleep(void);
void hal_timer_wakeup(void);
void hal_register_timer_irq_hook(Hal_Timer_IrqHookFunc_t hook_func);


#ifdef __cplusplus
}
#endif

#endif /*  */
