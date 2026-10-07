/* Includes */
#include "g32a10xx_eint.h"
#include <stddef.h>
#include "drv_eint.h"

#define EINT4_15_IRQ_HOOK_MAX 1

typedef struct
{
  uint32_t irq_line;
  Drv_Eint4_15_IrqHookFunc_t hook_func;
} Drv_Eint4_15_IrqHook_t;



/**********************************************************
  * @brief	drv EINT 初始化
  * @param
  * @retval
  * @note		
 **********************************************************/
void drv_eint_init(void)
{
	;
}



static Drv_Eint4_15_IrqHook_t drv_eint4_15_irq_hook[EINT4_15_IRQ_HOOK_MAX]  = {{0, NULL}};		// 预留钩子
static uint8_t drv_eint4_15_irq_hook_index = 0;       // 钩子索引
/**********************************************************
  * @brief	EINT4_15 钩子注册
  * @param  irq_line: 中断线
  * @param  hook_func: 钩子函数
  * @retval 
  * @note		
 **********************************************************/
void drv_register_eint4_15_irq_hook(uint32_t irq_line, Drv_Eint4_15_IrqHookFunc_t hook_func)
{
  if(hook_func == NULL)
  {
    return;
  }

  if(drv_eint4_15_irq_hook_index >= EINT4_15_IRQ_HOOK_MAX)
  {
    return;
  }

  if(irq_line > EINT_LINE15 || irq_line < EINT_LINE4)
  {
    return;
  }

  drv_eint4_15_irq_hook[drv_eint4_15_irq_hook_index].irq_line = irq_line;
  drv_eint4_15_irq_hook[drv_eint4_15_irq_hook_index].hook_func = hook_func; //注册钩子
  drv_eint4_15_irq_hook_index++;
}



/**********************************************************
  * @brief	EINT 中断
  * @param
  * @retval
  * @note		
 **********************************************************/


void drv_eint4_15_irq_handle(void)
{
  uint8_t index = 0;

  for(index=0; index<drv_eint4_15_irq_hook_index; index++)
  {
    if(Eint_ReadStatusFlag(drv_eint4_15_irq_hook[index].irq_line) == (uint8_t)SET)
    {
      Eint_ClearStatusFlag(drv_eint4_15_irq_hook[index].irq_line);
      drv_eint4_15_irq_hook[index].hook_func();
    }
  } 
}



