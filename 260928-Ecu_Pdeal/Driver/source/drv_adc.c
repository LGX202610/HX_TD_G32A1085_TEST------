
/* Includes */
#include "drv_adc.h"
#include "board.h"


static volatile uint16_t adcData[ADC_CH_SIZE];

// ADC 数据寄存器地址，供 DMA 外设端使用
uint32_t drv_get_adc_dr_addr(void)
{
	return ((uint32_t)ADC_BASE + 0x40u);
}

// 给DMA提供内存地址
uint32_t drv_get_adc_buf_addr(void)
{
	return (uint32_t)&adcData[0];
}

/**********************************************************
  * @brief	读取 adcData 数据
  * @param  
  * @retval 
  * @note		
 **********************************************************/
uint16_t drv_get_adc_buf(uint8_t idx)
{
	if(idx >= ADC_CH_SIZE)
	{
		return 0;
	}
	
	return adcData[idx];
}


/**********************************************************
  * @brief	drv ADC 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_adc_init(void)
{
	Gpio_ConfigType gpioConfig;
	Adc_ConfigType adcConfig;
	
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_ADC1);
	Rcm_EnableAhbPeriphClock(ADC_GPIO_PER_CLOCK);


	gpioConfig.pin = (uint16_t)ADC_CH10_GPIO_PIN | (uint16_t)ADC_CH11_GPIO_PIN | (uint16_t)ADC_CH12_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_AN;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	Gpio_Config(ADC_CH10_GPIO_PORT, &gpioConfig);
    

	/* ADC reset */
	Adc_Reset();

	adcConfig.resolution		= ADC_RESOLUTION_12B,
	adcConfig.scanDir			 	= ADC_SCAN_DIR_UPWARD,
	adcConfig.convMode			= ADC_CONVERSION_CONTINUOUS,
	adcConfig.dataAlign			= ADC_DATA_ALIGN_RIGHT,
	adcConfig.extTrigEdge		= ADC_EXT_TRIG_EDGE_NONE,
	adcConfig.extTrigConv		= ADC_EXT_TRIG_CONV_TRG0,
	
	Adc_Config(&adcConfig);
	

	/* ADC channel Convert configuration */
	Adc_ConfigChannel((uint32_t)ADC_INPUT_CH10, (uint8_t)ADC_SAMPLE_TIME_13_5);
	Adc_ConfigChannel((uint32_t)ADC_INPUT_CH11, (uint8_t)ADC_SAMPLE_TIME_13_5);
	Adc_ConfigChannel((uint32_t)ADC_INPUT_CH12, (uint8_t)ADC_SAMPLE_TIME_13_5);
//	Adc_ConfigChannel((uint32_t)ADC_INPUT_CH13, (uint8_t)ADC_SAMPLE_TIME_13_5);

	/* Calibration */
	Adc_ReadCalibrationFactor();

	/* Enable ADC DMA*/
	Adc_EnableDma();
	Adc_DmaRequestMode(ADC_DMA_MODE_CIRCULAR);
	
	/* Enable ADC */
	Adc_Enable();
	
	
	/* Wait until ADC is ready */
	while (0u == Adc_ReadStatusFlag(ADC_FLAG_ADRDY))
	{
			/* nothing */
	}
		
	/* ADC start conversion command */
	Adc_StartConversion();
}



/**********************************************************
  * @brief	drv ADC 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_adc_sleep(void)
{
	Adc_StopConversion();
	Adc_DisableDma();
	Adc_Disable();
	Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_ADC1);
}


/**********************************************************
  * @brief	drv ADC 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_adc_wakeup(void)
{
	uint32_t waittime = 0;

	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_ADC1);
	
	/* Calibration */
	Adc_ReadCalibrationFactor();

	/* Enable ADC DMA*/
	Adc_EnableDma();
	Adc_DmaRequestMode(ADC_DMA_MODE_CIRCULAR);
	
	/* Enable ADC */
	Adc_Enable();
	
	
	/* Wait until ADC is ready */
	while (0u == Adc_ReadStatusFlag(ADC_FLAG_ADRDY))
	{
		waittime++;
		if(waittime > 5000)
		{
			break;
		}
	}
		
	/* ADC start conversion command */
	Adc_StartConversion();
}
