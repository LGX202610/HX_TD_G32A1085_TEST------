#ifndef DRV_DMA_H
#define DRV_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx_dma.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_misc.h"

/* Define */
#define DMA_CHANNEL             DMA1_CHANNEL_1		// DMA通道1


 /* Enum */ 



/* Struct */ 




void drv_dma_init(uint32_t per_addr, uint32_t mem_addr, uint32_t buf_size);
void drv_dma_sleep(void);
void drv_dma_wakeup(void);


#ifdef __cplusplus
}
#endif

#endif /*  */
