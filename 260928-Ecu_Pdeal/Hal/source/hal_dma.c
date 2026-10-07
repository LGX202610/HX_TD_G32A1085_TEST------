
/* Includes */
#include "hal_dma.h"
#include "drv_dma.h"
#include "drv_adc.h"




/**********************************************************
  * @brief	hal DMA ≥ı ºªØ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_dma_init(void)
{
	drv_dma_init(drv_get_adc_dr_addr(), drv_get_adc_buf_addr(), ADC_CH_SIZE);
}


/**********************************************************
  * @brief	hal DMA –›√ﬂ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_dma_sleep(void)
{
	drv_dma_sleep();
}


/**********************************************************
  * @brief	hal DMA ªΩ–—
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_dma_wakeup(void)
{
	drv_dma_wakeup();
}






