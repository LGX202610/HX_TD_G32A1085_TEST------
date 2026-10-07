#ifndef HAL_BUZZER_H
#define HAL_BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */




/* Define */



 /* Enum */ 
/***************  ***************/
typedef enum           
{
	_buzzer_off			=	0,			//
	_buzzer_on			=	1,			//	
	_buzzer_toggle 	=	2,			//	
}BuzzerState;	


/* Struct */ 



void hal_buzzer_init(void);
void hal_buzzer_sleep(void);
void hal_set_buzzer_state(BuzzerState buzzer);

	


#ifdef __cplusplus
}
#endif

#endif /*  */
