#ifndef DRV_ADC_H
#define DRV_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx_adc.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_misc.h"
#include "g32a10xx_gpio.h"


/* Define */
#define	ADC_CH_SIZE				3           //ADC通道数量

#define ADC_INPUT_CH10			ADC_CHANNEL_10      //ADC通道10
#define ADC_INPUT_CH11			ADC_CHANNEL_11      //ADC通道11
#define ADC_INPUT_CH12			ADC_CHANNEL_12      //ADC通道12

#define ADC_SAMPLE_TIME			ADC_SAMPLE_TIME_13_5    //ADC采样时间



/* Enum */ 



/* Struct */ 






void drv_adc_init(void);
void drv_adc_sleep(void);
void drv_adc_wakeup(void);

uint32_t drv_get_adc_dr_addr(void);
uint32_t drv_get_adc_buf_addr(void);
uint16_t drv_get_adc_buf(uint8_t idx);


#ifdef __cplusplus
}
#endif

#endif /*  */
