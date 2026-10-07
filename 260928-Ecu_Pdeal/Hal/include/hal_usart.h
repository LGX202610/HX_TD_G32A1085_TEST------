#ifndef HAL_USART_H
#define HAL_USART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Define */



/* Enum */ 



/* Struct */ 


void hal_usart1_init(void);
void hal_usart_log(const uint8_t *dat);
void hal_usart1_sleep(void);
void hal_usart1_wakeup(void);


#ifdef __cplusplus
}
#endif

#endif /*  */
