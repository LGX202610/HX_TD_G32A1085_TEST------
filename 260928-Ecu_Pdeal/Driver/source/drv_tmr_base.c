
/* Includes */
#include "drv_tmr_base.h"


/**********************************************************
  * @brief	drv 定时器 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_tmr6_base_init(void)
{
	Tmr_TimeBaseType tmr_BaseConfig;
	
	/* Enable Clock */
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
	Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_TMR6);
	
	tmr_BaseConfig.clockDivision = TMR_CKD_DIV1;
	tmr_BaseConfig.counterMode = TMR_COUNTER_MODE_UP;
	tmr_BaseConfig.div = (64-1);
	tmr_BaseConfig.period = (1000-1);
	tmr_BaseConfig.repetitionCounter = 0;
	
	Tmr_ConfigTimeBase(TMR6, &tmr_BaseConfig);
	
	/* Enable update interrupt*/
	Tmr_EnableInterrupt(TMR6, (uint16_t)TMR_INT_UPDATE);
	Nvic_EnableIrqRequest(TMR6_IRQn, 2);

	/* Enable TMR6 */
	Tmr_Enable(TMR6);
}


/**********************************************************
  * @brief	drv 定时器 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_tmr6_base_sleep(void)
{
	Tmr_Disable(TMR6);
	Tmr_ClearIntFlag(TMR6, (uint16_t)TMR_INT_FLAG_UPDATE);
	NVIC_ClearPendingIRQ(TMR6_IRQn);
	Nvic_DisableIrqRequest(TMR6_IRQn);
	Rcm_DisableApb1PeriphClock(RCM_APB1_PERIPH_TMR6);
}

/**********************************************************
  * @brief	drv 定时器 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_tmr6_base_wakeup(void)
{
	Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_TMR6);
	Tmr_ClearIntFlag(TMR6, (uint16_t)TMR_INT_FLAG_UPDATE);
	NVIC_ClearPendingIRQ(TMR6_IRQn);
	Nvic_EnableIrqRequest(TMR6_IRQn, 2);
	Tmr_Enable(TMR6);
}


static Drv_Tmr6_IrqHookFunc_t drv_timer6_irq_hook  = NULL;		//预留钩子
/**********************************************************
  * @brief	钩子注册
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_register_tmr6_irq_hook(Drv_Tmr6_IrqHookFunc_t hook_func)
{
	drv_timer6_irq_hook = hook_func;
}


/**********************************************************
  * @brief	drv trm6 硬件中断服务函数
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_tmr6_irq_handle(void)
{	
	if(Tmr_ReadIntFlag(TMR6, TMR_INT_FLAG_UPDATE) == (uint16_t)SET)
	{		
		if(drv_timer6_irq_hook != NULL)	drv_timer6_irq_hook();
			
		Tmr_ClearIntFlag(TMR6, (uint16_t)TMR_INT_FLAG_UPDATE);
	}
}

