/* Includes */
#include "app_timer.h"
#include "hal_timer.h"



static uint8_t timer_1ms_callback_num = 0;		//已注册的数量
static App_Timer_Callback_t timer_1ms_callback_list[TIMER_1MS_CALLBACK_MAX] = {NULL};		//1ms定时器列表


static uint8_t timer_10ms_callback_num = 0;		//已注册的数量
static App_Timer_Callback_t timer_10ms_callback_list[TIMER_10MS_CALLBACK_MAX] = {NULL};	//10ms定时器列表



/**********************************************************
  * @brief	定时器 钩子实现
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void app_receive_timer_irq_hook(void)
{
	uint8_t i=0;
	static uint16_t timer_10ms=0;
	
	/*************** 1ms中断 ***************/
	for(i=0; i<timer_1ms_callback_num; i++)
	{
		if(timer_1ms_callback_list[i] != NULL)
		{
			timer_1ms_callback_list[i]();  
		}
	}
	
	/*************** 10ms中断 ***************/
	if(++timer_10ms>=10)
	{
		timer_10ms=0;
		
		for(i=0; i<timer_10ms_callback_num; i++)
		{
			if(timer_10ms_callback_list[i] != NULL)
			{
				timer_10ms_callback_list[i]();  
			}
		}
	}
}


/**********************************************************
  * @brief	定时器 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_timer_init(void)
{
	hal_register_timer_irq_hook(app_receive_timer_irq_hook);
	hal_timer_init();
}




/**********************************************************
  * @brief	定时器 注册
  * @param  
  * @retval 
  * @note		1成功，-1空函数，-2超出最大注册数
 **********************************************************/
int app_register_timer_callback(App_Timer_Callback_t callback, App_Timer_CallbackType_t callback_type)
{
	uint8_t i=0;
	
	if(callback == NULL) 
	{
		return -1;
	}
	
	if(callback_type == TIMER_1MS_CALLBACK_TYPE)
	{
		if(timer_1ms_callback_num>=TIMER_1MS_CALLBACK_MAX)
		{
			return -2;
		}
		else
		{
			for(i=0; i<TIMER_1MS_CALLBACK_MAX; i++)
			{
				if(timer_1ms_callback_list[i] == NULL)
				{
					timer_1ms_callback_num++;
					timer_1ms_callback_list[i] = callback;
					break;
				}
			}
		}
	}
	else if(callback_type == TIMER_10MS_CALLBACK_TYPE)
	{
		if(timer_10ms_callback_num>=TIMER_10MS_CALLBACK_MAX)
		{
			return -2;
		}
		else
		{
			for(i=0; i<TIMER_10MS_CALLBACK_MAX; i++)
			{
				if(timer_10ms_callback_list[i] == NULL)
				{
					timer_10ms_callback_num++;
					timer_10ms_callback_list[i] = callback;
					break;
				}
			}
		}
	}
	
	return 1;
}





