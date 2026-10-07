#ifndef HAL_ADC_H
#define HAL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Define */



/* Enum */ 
//Õ®µ¿”≥…‰
typedef enum
{
    ADC_BUF_CH_VST 		= 0,
    ADC_BUF_CH_MOTOR_L	= 1,
    ADC_BUF_CH_MOTOR_R  = 2,
} AdcBufCh_t;


/* Struct */ 




void hal_adc_init(void);
void hal_adc_sleep(void);
void hal_adc_wakeup(void);

uint16_t hal_get_vst_adc_value(void);




#ifdef __cplusplus
}
#endif

#endif /*  */
