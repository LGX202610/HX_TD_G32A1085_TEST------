#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Define */



 /* Enum */ 



/* Struct */ 




void drv_led_gpio_init(void);
void drv_set_led1_pin_high(void);
void drv_set_led1_pin_low(void);
void drv_set_led1_pin_toggle(void);
void drv_set_led2_pin_high(void);
void drv_set_led2_pin_low(void);
void drv_set_led2_pin_toggle(void);


void drv_buzzer_gpio_init(void);
void drv_set_buzzer_pin_high(void);
void drv_set_buzzer_pin_low(void);
void drv_set_buzzer_pin_toggle(void);


void drv_con_power_gpio_init(void);
void drv_set_con_vst_pin_high(void);
void drv_set_con_vst_pin_low(void);
void drv_set_con_vst_pin_toggle(void);

void drv_canstb_gpio_init(void);
void drv_set_canstb_pin_high(void);
void drv_set_canstb_pin_low(void);


void drv_bdc_driver_gpio_init(void);
uint8_t drv_get_bdc_nfault_pin(void);
void drv_set_bdc_nslp_pin_high(void);
void drv_set_bdc_nslp_pin_low(void);
void drv_set_bdc_brake_pin_high(void);
void drv_set_bdc_brake_pin_low(void);


void drv_hall_gpio_init(void);
uint8_t drv_get_hall_1_pin(void);
uint8_t drv_get_hall_2_pin(void);


void drv_gpio_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /*  */
