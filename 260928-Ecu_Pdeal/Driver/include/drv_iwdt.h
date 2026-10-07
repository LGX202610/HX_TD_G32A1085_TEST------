#ifndef DRV_IWDT_H
#define DRV_IWDT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx_iwdt.h"
#include "g32a10xx_rcm.h"


/* Define */




/* Enum */ 




/* Struct */ 





void drv_iwdt_init(void);
void drv_iwdt_set_run_mode(void);
void drv_iwdt_set_sleep_mode(void);
void drv_iwdt_refresh(void);



#ifdef __cplusplus
}
#endif

#endif /*  */
