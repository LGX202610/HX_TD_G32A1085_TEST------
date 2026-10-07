#ifndef HAL_IWDT_H
#define HAL_IWDT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */



/* Define */




/* Enum */

/**
 * IWDT 工作模式: 运行短超时 / 休眠长超时
 */
typedef enum
{
	HAL_IWDT_MODE_RUN = 0,	/* 运行模式, 短超时 */
	HAL_IWDT_MODE_SLEEP		/* 休眠模式, 长超时 */
} Hal_IwdtModeType;



/* Struct */ 





void hal_iwdt_init(void);
void hal_iwdt_set_mode(Hal_IwdtModeType mode);
void hal_iwdt_refresh(void);



#ifdef __cplusplus
}
#endif

#endif /*  */
