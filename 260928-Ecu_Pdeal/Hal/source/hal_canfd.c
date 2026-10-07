
/* Includes */
#include "hal_canfd.h"

#include "drv_canfd.h"
#include "drv_gpio.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>






/*************** CAN 数据实体 ***************/
static Can_RxFram g_can_rx_fram[CAN_RX_FRAME_ID_MAX] = {0};
static uint8_t g_can_rx_fram_len=0;



/**********************************************************
  * @brief	CAN 接收中断 钩子实现
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void hal_receive_can_rx_irq_hook(uint8_t hw_box, uint32_t id, uint32_t len, uint8_t *data)
{
	uint8_t i=0;

	for(i=0; i<g_can_rx_fram_len; i++)
	{
		/*************** 校验硬件邮箱号 ***************/
		if(hw_box == g_can_rx_fram[i].box)
		{
			/*************** 校验id ***************/
			if(id == g_can_rx_fram[i].id)
			{
				if(len > CAN_RX_FRAME_DATA_SIZE)
				{
					break;	// 
				}

				if((len > 0U) && (data != NULL))
				{
					memcpy(g_can_rx_fram[i].data, data, len);
				}

				g_can_rx_fram[i].len = len;
				
				/*************** 业务回调 ***************/
				if(g_can_rx_fram[i].callback != NULL)
				{
					g_can_rx_fram[i].callback(g_can_rx_fram[i].id, g_can_rx_fram[i].len, g_can_rx_fram[i].data);
				}
				
				break;
			}
		}
	}
}






/**********************************************************
  * @brief	 HAL层 CAN TX 完成中断 drv层的钩子实现
  * @param  
  * @retval 
  * @note		只释放对应邮箱
 **********************************************************/
static Hal_Can_TxIrqHookFunc_t hal_can_tx_irq_hook = NULL;
static void hal_receive_can_tx_irq_hook(uint8_t hw_box, uint32_t id, uint32_t len, uint8_t *data)
{
	(void)len;
	(void)data;

	/*************** 触发上层回调 ***************/
	if(hal_can_tx_irq_hook != NULL)
	{
		hal_can_tx_irq_hook(hw_box, id);
	}
}

/**********************************************************
* @brief	 HAL层 CAN TX 完成中断 钩子注册
* @param  
* @retval 
* @note		
**********************************************************/
void hal_register_can_tx_irq_hook(Hal_Can_TxIrqHookFunc_t hook_func)
{
	hal_can_tx_irq_hook = hook_func;
}




/**********************************************************
* @brief	HAL层 CAN 错误中断 钩子实现
* @param  
* @retval 
* @note		HAL 先释放全部 TX 占用，再转业务 BusOff
**********************************************************/
static Hal_Can_ErrorIrqHookFunc_t hal_can_error_irq_hook = NULL;
static void hal_receive_can_error_irq_hook(uint8_t hw_box, uint32_t id, uint32_t len, uint8_t *data)
{
	(void)hw_box;
	(void)id;
	(void)len;
	(void)data;
	
	if(hal_can_error_irq_hook != NULL)
	{
		hal_can_error_irq_hook();
	}
}

/**********************************************************
* @brief	 HAL层 CAN 错误中断 钩子注册
* @param  
* @retval 
* @note		
**********************************************************/
void hal_register_can_error_irq_hook(Hal_Can_ErrorIrqHookFunc_t hook_func)
{
	hal_can_error_irq_hook = hook_func;
}



/**********************************************************
  * @brief	唤醒中断 钩子实现
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static volatile bool flag_can_wakeup = false;	// 唤醒 CAN唤醒中断标志位
static void hal_receive_can_wakeup_irq_hook(void)
{
	flag_can_wakeup = true;
}


/**********************************************************
  * @brief	获取 CAN 唤醒中断标志位
  * @param
  * @retval
  * @note
 **********************************************************/
bool hal_can_get_wakeup_flag(void)
{
	return flag_can_wakeup;
}


/**********************************************************
  * @brief	清除 CAN 唤醒中断标志位
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_can_clear_wakeup_flag(void)
{
	flag_can_wakeup = false;
}


/**********************************************************
  * @brief	hal CANFD 发送数据
  * @param  邮箱号，帧id，帧类型，长度，数据
  * @retval 
  * @note		
 **********************************************************/
int hal_can_tx_fram(uint32_t box, uint32_t id, uint8_t frame_type, uint32_t len, uint8_t *data)
{
	int ret=0;
	
	if(box >= CAN_TX_BUF_ID_MAX || (frame_type!=CAN_FRAME_TYPE_STD && frame_type!=CAN_FRAME_TYPE_EXT))
	{
		return 0;
	}
	
	ret = drv_can_tx_fram(box, id, frame_type, len, data);
	
	return ret;
}



////发送队列，3级fifo
//Can_TxFram can_tx_fifo[3] = 
//{
//	[0]={
//		.box = 0,
//		.id = 0,
//		.len = 0,
//		.data = 0,
//	},
//	
//	[1]={
//		.box = 0,
//		.id = 0,
//		.len = 0,
//		.data = 0,
//	},
//	
//	[2]={
//		.box = 0,
//		.id = 0,
//		.len = 0,
//		.data = 0,
//	},
//};
//uint8_t can_tx_fifo_idx = 0;	//





/**********************************************************
  * @brief	CANFD 注册id和回调
  * @param  帧id，帧类型(标准帧/扩展帧)，回调函数
  * @retval 0失败，1成功
  * @note		
 **********************************************************/
uint8_t hal_register_can_rx_callback(uint32_t id, uint8_t frame_type, Hal_Can_RxCallback_t callback)
{
	int box=0;
	
	if(g_can_rx_fram_len>=CAN_RX_FRAME_ID_MAX || (frame_type!=CAN_FRAME_TYPE_STD && frame_type!=CAN_FRAME_TYPE_EXT))
	{
		return 0;
	}

	
	box = drv_alloc_can_rx_box(id, frame_type);

	if(box<0)	
	{
		return 0;
	}
	
	g_can_rx_fram[g_can_rx_fram_len].box = box;
	g_can_rx_fram[g_can_rx_fram_len].id = id;
	g_can_rx_fram[g_can_rx_fram_len].callback = callback;
	g_can_rx_fram[g_can_rx_fram_len].len = 0;
	memset(g_can_rx_fram[g_can_rx_fram_len].data, 0, CAN_RX_FRAME_DATA_SIZE);
	
	g_can_rx_fram_len++;
	
	return 1;
}









/**********************************************************
  * @brief	hal CANFD 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_can_init(void)
{	
	/*************** CAN 初始化 ***************/
	drv_can_init();
	
	/*************** 退出 standby ***************/
	drv_canstb_gpio_init();
	
	/*************** 钩子注册 ***************/
	drv_register_can_rx_irq_hook(hal_receive_can_rx_irq_hook);
	drv_register_can_tx_irq_hook(hal_receive_can_tx_irq_hook);
	drv_register_can_error_irq_hook(hal_receive_can_error_irq_hook);
	drv_register_can_wakeup_irq_hook(hal_receive_can_wakeup_irq_hook);
}


/**********************************************************
  * @brief	hal CANFD 启动
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_can_enable(void)
{
	drv_can_enable();
	drv_set_canstb_pin_low();
}


/**********************************************************
  * @brief	hal CAN sleep
  * @param
  * @retval
  * @note		
 **********************************************************/
void hal_can_sleep(void)
{
	drv_can_sleep();
	drv_set_canstb_pin_high();
}


/**********************************************************
  * @brief	hal CAN wakeup
  * @param
  * @retval
  * @note
 **********************************************************/
void hal_can_wakeup(void)
{
	drv_can_wakeup();
	drv_set_canstb_pin_low();
}


	

