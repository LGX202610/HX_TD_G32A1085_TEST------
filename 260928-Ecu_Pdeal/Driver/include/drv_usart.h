#ifndef DRV_USART_H
#define DRV_USART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx_usart.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_misc.h"
#include "g32a10xx_gpio.h"


/* Define */




/* Enum */ 



/* Struct */ 



typedef void (*Drv_Usart1_RxIrqHookFunc_t)(uint8_t byte);

void drv_usart1_init(void);
void drv_usart1_sleep(void);
void drv_usart1_wakeup(void);
void drv_usart1_write(const uint8_t *data);
void drv_register_usart1_rx_irq_hook(Drv_Usart1_RxIrqHookFunc_t hook_func);


#ifdef __cplusplus
}
#endif

#endif /*  */
