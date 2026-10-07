#ifndef DRV_TMR_BASE_H
#define DRV_TMR_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx_tmr.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_misc.h"


/* Define */ 




/* Enum */ 




/* Struct */ 



typedef void (*Drv_Tmr6_IrqHookFunc_t)(void);


void drv_tmr6_base_init(void);
void drv_tmr6_base_sleep(void);
void drv_tmr6_base_wakeup(void);
void drv_tmr6_irq_handle(void);
void drv_register_tmr6_irq_hook(Drv_Tmr6_IrqHookFunc_t hook_func);


#ifdef __cplusplus
}
#endif

#endif /*  */
