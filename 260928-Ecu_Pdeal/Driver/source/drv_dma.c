
/* Includes */
#include "drv_dma.h"



/**********************************************************
  * @brief	drv DMA ≥ı ºªØ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_dma_init(uint32_t per_addr, uint32_t mem_addr, uint32_t buf_size)
{
	Dma_ConfigType dmaConfig;
	
	/* Enable DMA clock */
	Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_DMA1);
	
	/* Set Peripheral Address */
	dmaConfig.peripheralAddress = per_addr;
	/* Set memory Address */
	dmaConfig.memoryAddress = mem_addr;
	/* read from peripheral */
	dmaConfig.direction = DMA_DIR_PERIPHERAL;
	/* size of buffer*/
	dmaConfig.bufferSize = buf_size;
	/* Disable Peripheral Address increase */
	dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
	/* Enable Memory Address increase */
	dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
	/* Set peripheral Data Size */
	dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATASIZE_HALFWORD;
	/* set memory Data Size */
	dmaConfig.memoryDataSize = DMA_MEMORY_DATASIZE_HALFWORD;
	/* Reset Circular Mode */
	dmaConfig.circular = DMA_CIRCULAR_ENABLE;
	/* Disable M2M */
	dmaConfig.memoryTomemory = DMA_M2M_DISABLE;
	/* set priority */
	dmaConfig.priority = DMA_PRIORITY_LEVEL_HIGHT;
	
	/* DMA Configure */
	Dma_Config(DMA_CHANNEL, &dmaConfig);
	
	/* Clear Transfer Complete interrupt flag */
	Dma_ClearIntFlag((uint32_t)DMA1_INT_FLAG_TF1);
	
	/* Enable DMA Interrupt*/
//	Dma_EnableInterrupt(DMA_CHANNEL, (uint32_t)DMA_INT_TFIE);
//	Nvic_EnableIrqRequest(DMA_CH1_IRQn, 2);

	Dma_Enable(DMA_CHANNEL);	
}



/**********************************************************
  * @brief	drv DMA –›√ﬂ
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_dma_sleep(void)
{
	/* Clear Transfer Complete interrupt flag */
	Dma_ClearIntFlag((uint32_t)DMA1_INT_FLAG_TF1);
	
	Dma_Disable(DMA_CHANNEL);
	
	/* Enable DMA clock */
	Rcm_DisableAhbPeriphClock(RCM_AHB_PERIPH_DMA1);
}

/**********************************************************
  * @brief	drv DMA ªΩ–—
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_dma_wakeup(void)
{
	/* Enable DMA clock */
	Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_DMA1);
	
	/* Clear Transfer Complete interrupt flag */
	Dma_ClearIntFlag((uint32_t)DMA1_INT_FLAG_TF1);
	
	Dma_Enable(DMA_CHANNEL);
}
