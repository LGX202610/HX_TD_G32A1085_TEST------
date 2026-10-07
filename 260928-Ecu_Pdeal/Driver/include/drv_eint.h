#ifndef DRV_EINT_H
#define DRV_EINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Define */
#define DRV_EINT_LINE11  ((uint32_t)0x00000800U)


/* Enum */



/* Struct */
typedef void (*Drv_Eint4_15_IrqHookFunc_t)(void);


void drv_eint_init(void);
void drv_eint4_15_irq_handle(void);
void drv_register_eint4_15_irq_hook(uint32_t irq_line, Drv_Eint4_15_IrqHookFunc_t hook_func);


#ifdef __cplusplus
}
#endif

#endif /*  */
