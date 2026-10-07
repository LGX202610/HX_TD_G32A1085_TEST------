/* Includes */
#include "drv_iwdt.h"


/* LSI 约 32kHz 时: Tout = DIV * RLR / 32000 (s) */
#define IWDT_RUN_DIV        IWDT_DIV_256
#define IWDT_RUN_RELOAD     ((uint16_t)625)	/* 约 5s */

#define IWDT_SLEEP_DIV      IWDT_DIV_256
#define IWDT_SLEEP_RELOAD   ((uint16_t)3750)	/* 约 30s */



/**********************************************************
  * @brief	下发预分频和重装值
  * @param	div		IWDT 预分频
  * @param	reload	计数重装值
  * @retval
  * @note	  须等 STS 更新完成后再喂狗, 新超时才生效
 **********************************************************/
static void drv_iwdt_apply(Iwdt_DivType div, uint16_t reload)
{
	uint32_t timeout = 0xFFFF;
	
	/* set IWDT Write Access */
	Iwdt_EnableWriteAccess();

	/* set IWDT Divider*/
	Iwdt_ConfigDivider(div);

	while ((IWDT->STS_R.STS != 0U) && (timeout > 0U))
	{
			timeout = timeout - 1U;
	}

	/* set IWDT Reloader*/
	Iwdt_ConfigReload(reload);

	timeout = 0xFFFF;
	while ((IWDT->STS_R.STS != 0U) && (timeout > 0U))
	{
			timeout = timeout - 1U;
	}

	/* Refresh*/
	Iwdt_Refresh();
}


/**********************************************************
  * @brief	drv 看门狗 初始化
  * @param
  * @retval
  * @note	  启动 IWDT 并进入运行模式短超时
 **********************************************************/
void drv_iwdt_init(void)
{
	/* clear IWDTRST Flag*/
	if(Rcm_ReadStatusFlag(RCM_FLAG_IWDTRST) != (uint16_t)RESET)
	{
			Rcm_ClearStatusFlag();
	}
	else
	{
			/* nothing */
	}

	/* Enable IWDT*/
	Iwdt_Enable();

	drv_iwdt_apply(IWDT_RUN_DIV, IWDT_RUN_RELOAD);
}


/**********************************************************
  * @brief	配置运行模式短超时
  * @param
  * @retval
  * @note	  约 1s
 **********************************************************/
void drv_iwdt_set_run_mode(void)
{
	drv_iwdt_apply(IWDT_RUN_DIV, IWDT_RUN_RELOAD);
}


/**********************************************************
  * @brief	配置休眠模式长超时
  * @param
  * @retval
  * @note	约 12s
 **********************************************************/
void drv_iwdt_set_sleep_mode(void)
{
	drv_iwdt_apply(IWDT_SLEEP_DIV, IWDT_SLEEP_RELOAD);
}


/**********************************************************
  * @brief	drv 看门狗 刷新
  * @param
  * @retval
  * @note
 **********************************************************/
void drv_iwdt_refresh(void)
{
	Iwdt_Refresh();
}
