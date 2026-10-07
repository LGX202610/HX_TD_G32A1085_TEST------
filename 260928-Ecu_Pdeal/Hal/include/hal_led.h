#ifndef HAL_LED_H
#define HAL_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */



/* Define */



 /* Enum */ 
/***************  ***************/
typedef enum           
{
	_led_off			=	0,			//
	_led_on				=	1,			//	
	_led_toggle 	=	2,			//	
}LedState;	


/* Struct */ 


void hal_led_init(void);
void hal_led_sleep(void);
void hal_led_wakeup(void);
void hal_set_led1_state(LedState led);
void hal_set_led2_state(LedState led);

	


#ifdef __cplusplus
}
#endif

#endif /*  */
