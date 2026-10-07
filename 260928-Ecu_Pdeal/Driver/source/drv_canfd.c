/* Includes */
#include "g32a10xx_can.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_misc.h"
#include "g32a10xx_gpio.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "drv_canfd.h"
#include "drv_gpio.h"



/*************** CAN 结构体 ***************/
static Can_HandleType g_CanHandle = {0U};

/*************** 接收帧结构体 ***************/
static Can_RxFrameType g_can_rxframe = {0U};

/*************** 发送快照结构体 ***************/
typedef struct
{
	uint8_t  sending;		/* 发送中标记 */
	uint32_t id;			/* CAN ID，提交给上层 */
} Drv_Can_TxPendType;

static volatile Drv_Can_TxPendType g_can_tx_pend[CAN_TX_BUF_ID_MAX];



/*************** 接收滤波器 标准帧配置 ***************/
static Can_FilterConfigType g_can_rxFilter_std = {
	.address  = STD_FILTER_OFS,
	.idFormat = CAN_FRAME_STD_ID,
	.listSize = CAN_RX_STD_LIST_MAX,
	.nmFrame  = CAN_REJECT,
	.remFrame = CAN_REJECT_REMOTE_FRAME,
};


/*************** 接收滤波器 扩展帧配置 ***************/
static Can_FilterConfigType g_can_rxFilter_ext = {
	.address  = EXT_FILTER_OFS,
	.idFormat = CAN_FRAME_EXT_ID,
	.listSize = CAN_RX_EXT_LIST_MAX,
	.nmFrame  = CAN_REJECT,
	.remFrame = CAN_REJECT_REMOTE_FRAME,
};


/*************** 接收buffer 配置 ***************/
static Can_RxBufConfigType g_can_rxBuffer = {
	.address       = RX_BUFFER_OFS,
	.datafieldSize = CAN_DATA_SIZE_8,
//	.datafieldSize = CAN_DATA_SIZE_64,
};


/*************** 发送buffer 配置 ***************/
static Can_TxBufConfigType txBuffer = {
	.address       = TX_BUFFER_OFS,
	.dedicatedSize = CAN_TX_BUF_ID_MAX,		
	.fifoQueCnt    = 0,
	.datafieldSize = CAN_DATA_SIZE_8,
//	.datafieldSize = CAN_DATA_SIZE_64,
};


/*************** 硬件接收邮箱 列表 ***************/
static uint8_t rx_box_list[CAN_RX_BUF_ID_MAX]=
{
	CAN_RX_BUF_ID_0,
	CAN_RX_BUF_ID_1,
	CAN_RX_BUF_ID_2,
	CAN_RX_BUF_ID_3,
	CAN_RX_BUF_ID_4,
	CAN_RX_BUF_ID_5,
	CAN_RX_BUF_ID_6,
	CAN_RX_BUF_ID_7,
	CAN_RX_BUF_ID_8,
	CAN_RX_BUF_ID_9,
	CAN_RX_BUF_ID_10,
	CAN_RX_BUF_ID_11,
	CAN_RX_BUF_ID_12,
	CAN_RX_BUF_ID_13,
	CAN_RX_BUF_ID_14,
	CAN_RX_BUF_ID_15,
	CAN_RX_BUF_ID_16,
	CAN_RX_BUF_ID_17,
	CAN_RX_BUF_ID_18,
	CAN_RX_BUF_ID_19,
};
static uint8_t rx_box_list_len = 0;		//已使用邮箱数量
static uint8_t std_filter_cnt = 0;		//标准帧滤波器索引
static uint8_t ext_filter_cnt = 0;		//扩展帧滤波器索引



/*************** 硬件发送邮箱 列表 ***************/
static uint8_t tx_box_list[CAN_TX_BUF_ID_MAX]=
{
	CAN_TX_BUF_ID_0,
	CAN_TX_BUF_ID_1,
	CAN_TX_BUF_ID_2,
	CAN_TX_BUF_ID_3,
	CAN_TX_BUF_ID_4,
	CAN_TX_BUF_ID_5,
	CAN_TX_BUF_ID_6,
	CAN_TX_BUF_ID_7,
	CAN_TX_BUF_ID_8,
	CAN_TX_BUF_ID_9,
};
static uint8_t tx_box_list_len = 0;		//已使用邮箱数量





/**********************************************************
  * @brief	接收中断 钩子注册
	* @param  
  * @retval 
  * @note		
 **********************************************************/
static Drv_Can_RxIrqHookFunc_t drv_can_rx_irq_hook  = NULL;	
void drv_register_can_rx_irq_hook(Drv_Can_RxIrqHookFunc_t hook_func)
{	
	drv_can_rx_irq_hook = hook_func;
}

/**********************************************************
  * @brief	发送中断 钩子注册
	* @param  
  * @retval 
  * @note		
 **********************************************************/
static Drv_Can_TxIrqHookFunc_t drv_can_tx_irq_hook  = NULL;	
void drv_register_can_tx_irq_hook(Drv_Can_TxIrqHookFunc_t hook_func)
{	
	drv_can_tx_irq_hook = hook_func;
}

/**********************************************************
  * @brief	错误中断 钩子注册
	* @param  
  * @retval 
  * @note		
 **********************************************************/
static Drv_Can_ErrorIrqHookFunc_t drv_can_error_irq_hook  = NULL;	
void drv_register_can_error_irq_hook(Drv_Can_ErrorIrqHookFunc_t hook_func)
{	
	drv_can_error_irq_hook = hook_func;
}

/**********************************************************
  * @brief	唤醒中断 钩子注册
	* @param  
  * @retval 
  * @note		
 **********************************************************/
static Drv_Can_WakeupIrqHookFunc_t drv_can_wakeup_irq_hook  = NULL;	
void drv_register_can_wakeup_irq_hook(Drv_Can_WakeupIrqHookFunc_t hook_func)
{	
	drv_can_wakeup_irq_hook = hook_func;
}


/**********************************************************
  * @brief	drv CAN 硬件中断服务函数
  * @param
  * @retval
  * @note
 **********************************************************/
void drv_can_irq_handle(void)
{
	Can_TransferHandleIsr(CAN, s_canHandleArr[0]);
}


/**********************************************************
  * @brief	drv CAN 唤醒中断服务函数
  * @param
  * @retval
  * @note
 **********************************************************/
void drv_can_serm_irq_handle(void)
{
	if(Can_ReadErmStsFlag(1U << 2U) != 0U)
	{
		Can_ClearErmStsFlag(1U << 2U);
		
		if(drv_can_wakeup_irq_hook != NULL)
		{
			drv_can_wakeup_irq_hook();
		}
	}
//	hal_set_led2_state(_led_on);
}


/**********************************************************
  * @brief	drv CAN 中断服务函数
  * @param  
  * @retval 
  * @note		SDK提供的回调接口,仅接收邮箱的数据并传递，数据的处理不在这层
 **********************************************************/
static void Can_IntCallback(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_TransferStsType Status, uint32_t Result, uint8_t *InputParaPtr)
{
	uint8_t i = 0U;
	uint8_t box = (uint8_t)Result;
	
	(void)InputParaPtr;
	
	switch(Status)
	{
		
		/*************** 发送完成 ***************/
		case CAN_TX_IDLE:
			for(i = 0U; i < CAN_TX_BUF_ID_MAX; i++)
			{
				if((g_can_tx_pend[i].sending != 0U) && (HandlePtr->txBufSts[i] == (uint8_t)CAN_IDLE_STATE))
				{
					g_can_tx_pend[i].sending = 0U;
					
					if(drv_can_tx_irq_hook != NULL)
					{
						drv_can_tx_irq_hook(i, g_can_tx_pend[i].id, 0U, NULL);
					}
				}
			}
		break;
		
		
		/*************** 接收完成 ***************/
		case CAN_RX_IDLE:
			
			Can_ReceiveNonBlocking(ModulePtr, box, &g_CanHandle, &g_can_rxframe);	
		
			
			if(drv_can_rx_irq_hook != NULL)	
			{
				if(g_can_rxframe.Can_RxFrameHead0.xtd == CAN_FRAME_STD_ID)
				{
					drv_can_rx_irq_hook(box, (g_can_rxframe.Can_RxFrameHead0.id >> CAN_FRAME_STD_ID_SHIFT), g_CanGblFdDlcConvDb[g_can_rxframe.Can_RxFrameHead1.dlc], g_can_rxframe.data);
				}
				else if(g_can_rxframe.Can_RxFrameHead0.xtd == CAN_FRAME_EXT_ID)
				{
					drv_can_rx_irq_hook(box, (g_can_rxframe.Can_RxFrameHead0.id), g_CanGblFdDlcConvDb[g_can_rxframe.Can_RxFrameHead1.dlc], g_can_rxframe.data);
				}
				else
				{
					;	//失败处理
				}
			}
			
		break;
			
			
		/*************** BUS OFF ***************/
		case CAN_ERROR_STS:
			drv_can_tx_reset();
			
			if(drv_can_error_irq_hook != NULL)
			{
				drv_can_error_irq_hook(0U, 0U, 0U, NULL);
			}
		break;
		
		default:
			
		break;
	}
}






/**********************************************************
  * @brief	drv CANFD 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_can_init(void)
{		
	Gpio_ConfigType gpioConfig;
	Can_ConfigType	canfdConfig;
	
	/*************** 时钟 ***************/
	Rcm_ConfigCanClk(RCM_CANCLK_PLLCLK);
	Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_CAN);
	Rcm_EnableAhbPeriphClock(CAN_GPIO_PER_CLOCK);
	
	
	/*************** GPIO ***************/
	Gpio_ConfigPinAF(CAN_RX_GPIO_PORT, CAN_RX_PIN_SOURCE, CAN_RX_GPIO_AF);
	Gpio_ConfigPinAF(CAN_TX_GPIO_PORT, CAN_TX_PIN_SOURCE, CAN_TX_GPIO_AF);
	
	gpioConfig.mode = GPIO_MODE_AF;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pin = (uint16_t)CAN_RX_GPIO_PIN; 
	Gpio_Config(CAN_RX_GPIO_PORT, &gpioConfig);
	
	gpioConfig.pupd = GPIO_PUPD_PD;
	gpioConfig.pin = (uint16_t)CAN_TX_GPIO_PIN; 
	Gpio_Config(CAN_TX_GPIO_PORT, &gpioConfig);

	
	/*
		Tq = seg1+seg2+3
		clk = 64MHz
		波特率 = clk/Tq
		seg1[0x00-0x1F]	seg2[0x00-0x0F]
	*/
	/*************** 波特率、FD、时序等CAN内核初始化 ***************/
	canfdConfig.loopBackInterEn                    = DISABLE,
	canfdConfig.loopBackExtEn                      = DISABLE,
	canfdConfig.busMonEn                           = DISABLE,	
#if(CANFD_USED==1)					
	canfdConfig.arbBaudrate                        = CAN_ARB_BAUDRATE,
	canfdConfig.dataBaudrate                       = CAN_DATA_BAUDRATE,
	canfdConfig.canfdNorEn                         = DISABLE,
	canfdConfig.canfdBrsEn                         = DISABLE,
	canfdConfig.baudrateConfig.clkPsc              = 1U,
	canfdConfig.baudrateConfig.phaseSeg1           = 0x31U,
	canfdConfig.baudrateConfig.phaseSeg2           = 0x0CU,
	canfdConfig.baudrateConfig.resyncJumpWidth     = 0x0CU,
	canfdConfig.baudrateConfig.dataClkPsc          = 1U,
	canfdConfig.baudrateConfig.dataPhaseSeg1       = 0x31U,
	canfdConfig.baudrateConfig.dataPhaseSeg2       = 0x0CU,
	canfdConfig.baudrateConfig.dataResyncJumpWidth = 0x0CU,
#elif(CANFD_USED==2)
	canfdConfig.arbBaudrate                        = CANFD_ARB_BAUDRATE,
	canfdConfig.dataBaudrate                       = CANFD_DATA_BAUDRATE,
	canfdConfig.canfdNorEn                         = ENABLE,
	canfdConfig.canfdBrsEn                         = ENABLE,
	canfdConfig.baudrateConfig.clkPsc              = 1U,
	canfdConfig.baudrateConfig.phaseSeg1           = 0x31U,
	canfdConfig.baudrateConfig.phaseSeg2           = 0x0CU,
	canfdConfig.baudrateConfig.resyncJumpWidth     = 0x0CU,
	canfdConfig.baudrateConfig.dataClkPsc          = 1U,
	canfdConfig.baudrateConfig.dataPhaseSeg1       = 0x0CU,
	canfdConfig.baudrateConfig.dataPhaseSeg2       = 0x04U,
	canfdConfig.baudrateConfig.dataResyncJumpWidth = 0x04U,
#endif

	/*************** 初始化 ***************/
	Can_Init(CAN, &canfdConfig);
	
	
	/*************** 创建中断句柄 ***************/
	Can_CreateHandle(CAN, &g_CanHandle, &Can_IntCallback, NULL);

	
	/*************** 初始化专用RAM ***************/
	memset((uint32_t *)CAN_SRAM_BASE, 0, CAN_SRAM_SIZE);
	
	
	/*************** 配置 接收滤波器 ***************/
	Can_SetFilterConfig(CAN, &g_can_rxFilter_std);
	Can_SetFilterConfig(CAN, &g_can_rxFilter_ext);


	/*************** 配置 接收滤波规则 ***************/

	
	
	/*************** 配置 RX Buffer ***************/
	Can_SetRxBufferConfig(CAN, &g_can_rxBuffer);


	/*************** 配置 TX Buffer ***************/
	Can_SetTxBufConfig(CAN, &txBuffer);

	
	/*************** 关闭自动重传 ***************/
	Can_SetAutomaticRetrans(CAN, (uint8_t)TRUE);

	/*************** 使能CAN唤醒中断 ***************/
	Can_EnableErmInt(1U << 3U);

	/*************** 使能CAN唤醒中断 ***************/
	Nvic_EnableIrqRequest(CAN_SMS_IT_IRQn, 0x0e);
	
	/*************** 同步到总线 ***************/
//	Can_EnterNorMode(CAN);


	/*************** 挂载接收缓存 ***************/
//	Can_ReceiveNonBlocking(CAN, CAN_RX_BUF_ID_0, &g_CanHandle, &g_can_rxframe);
//	Can_ReceiveNonBlocking(CAN, CAN_RX_BUF_ID_1, &g_CanHandle, &g_can_rxframe);
}



/**********************************************************
  * @brief	drv CAN 休眠配置
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_can_sleep(void)
{
	/*************** 关闭CAN，软件发送占用一并释放 ***************/
	drv_can_tx_reset();
	Can_EnterInitMode(CAN);
	 
//	Rcm_DisableApb1PeriphClock(RCM_APB1_PERIPH_CAN);
}
 
 
 /**********************************************************
   * @brief	drv CAN 唤醒配置
   * @param  
   * @retval 
   * @note		
  **********************************************************/
void drv_can_wakeup(void)
{
//	Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_CAN);

	/*************** 同步到总线 ***************/
	Can_EnterNorMode(CAN);
	
	drv_set_canstb_pin_low();
}
 

 



/**********************************************************
  * @brief	drv CANFD 接收滤波规则配置
  * @param  帧id，帧类型(标准帧/扩展帧)
  * @retval 硬件邮箱号
  * @note		
 **********************************************************/
int drv_alloc_can_rx_box(uint32_t filter_id, uint8_t frame_type)
{
	uint8_t box_id;
	
	Can_StdFilterEleConfigType stdFilter = {0};
	Can_ExtFilterEleConfigType extFilter = {0};
	
	if((rx_box_list_len>=CAN_RX_BUF_ID_MAX) || ((frame_type != CAN_FRAME_STD_ID) && (frame_type != CAN_FRAME_EXT_ID)))
	{
		return -1;
	}
	
	box_id = rx_box_list[rx_box_list_len];
	
	if(frame_type == CAN_FRAME_STD_ID)
	{
		if(std_filter_cnt >= CAN_RX_STD_LIST_MAX)
		{
			return -1;
		}
		
		stdFilter.sfec = (uint8_t)CAN_FILTER_STORE_IN_RX_BUF;
		stdFilter.sft   = (uint8_t)CAN_FILTER_DUAL_MODE;
		stdFilter.sfid1 = filter_id;
		stdFilter.sfid2 = box_id;
		Can_SetStdFilterEle(&g_can_rxFilter_std, &stdFilter, std_filter_cnt);
		std_filter_cnt++;
	}

	else if(frame_type == CAN_FRAME_EXT_ID)
	{
		if(ext_filter_cnt >= CAN_RX_EXT_LIST_MAX)
		{
			return -1;
		}
		
		extFilter.efec = (uint8_t)CAN_FILTER_STORE_IN_RX_BUF;
		extFilter.eft   = (uint8_t)CAN_FILTER_DUAL_MODE;
		extFilter.efid1 = filter_id;
		extFilter.efid2 = box_id;
		Can_SetExtFilterEle(&g_can_rxFilter_ext, &extFilter, ext_filter_cnt);
		ext_filter_cnt++;
	}
	
	rx_box_list_len++;
	return box_id;
}


/**********************************************************
  * @brief	drv CAN 使能
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_can_enable(void)
{
	uint8_t i = 0U;
	uint32_t wait_cnt = 0U;
	
	/*************** 同步到总线 ***************/
	Can_EnterNorMode(CAN);
	
	/*************** 等待进入nor ***************/
	wait_cnt = 0U;
	while ((uint8_t)BIT_SET == CAN->CCCR_R.CCCR_B.INIT)
	{
		wait_cnt++;
		if(wait_cnt > 100000U)
		{
			break;
		}
	}
	
	/*************** 总线集成（约 11 个隐性位）后再挂接收 ***************/
	wait_cnt = 0U;
	while(wait_cnt < 5000U)
	{
		wait_cnt++;
	}
	
	/*************** 挂载接收缓存 ***************/
	for(i=0; i<rx_box_list_len; i++)
	{
		Can_ReceiveNonBlocking(CAN, rx_box_list[i], &g_CanHandle, &g_can_rxframe);
	}
}



/**********************************************************
  * @brief	drv CAN 失能
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_can_disable(void)
{
	uint8_t i=0;
	  
	/*************** 释放接收缓存 ***************/
	for(i=0; i<rx_box_list_len; i++)
	{
		Can_FinishReceiveBuf(CAN, &g_CanHandle, rx_box_list[i]);
	}

	/*************** 关闭CAN，软件发送占用一并释放 ***************/
	drv_can_tx_reset();
	Can_EnterInitMode(CAN);
}

 




/**********************************************************
  * @brief	释放全部 TX 软件占用
  * @param
  * @retval
  * @note		
 **********************************************************/
void drv_can_tx_reset(void)
{
	uint8_t i = 0U;
	
	for(i = 0U; i < CAN_TX_BUF_ID_MAX; i++)
	{
		g_can_tx_pend[i].sending = 0U;
		g_can_tx_pend[i].id = 0U;
	}
}


/**********************************************************
  * @brief	drv CAN 发送
  * @param	邮箱号，帧id，帧类型，长度，数据
  * @retval 1成功，0失败
  * @note	只占用本 box。库按该 TxBufId 的 TXBRP/txBufSts 判忙，其它邮箱仍可装载
 **********************************************************/
int drv_can_tx_fram(uint32_t box, uint32_t id, uint8_t frame_type, uint32_t len, uint8_t *data)
{
	uint8_t dlc = 0U;
	Can_TxFrameType txframe;
	Can_BufTransInfoType txinfo;
	
	if((box >= CAN_TX_BUF_ID_MAX) || (len > CAN_DATA_SIZE) || (data == NULL) || ((frame_type != CAN_FRAME_STD_ID) && (frame_type != CAN_FRAME_EXT_ID)))
	{
		return 0;
	}
	
	memset(&txframe, 0, sizeof(txframe));
	memset(&txinfo, 0, sizeof(txinfo));
	
	dlc = Can_ConvertLenByteToDlc(len);
	
	txframe.Can_TxFrameHead0.xtd  = (uint8_t)frame_type;
	txframe.Can_TxFrameHead0.rtr  = (uint8_t)CAN_DATA_FRAME;
	txframe.Can_TxFrameHead1.fdf  = 0U;
	txframe.Can_TxFrameHead1.brs  = 0U;
	txframe.Can_TxFrameHead0.id   = (frame_type == CAN_FRAME_STD_ID) ? (uint32_t)(id << CAN_FRAME_STD_ID_SHIFT) : (uint32_t)(id);
	txframe.data = data;
	txframe.dataSize = len;
	txframe.Can_TxFrameHead1.dlc = dlc;
	
	txinfo.txFramePtr = &txframe;
	txinfo.bufferIdx = (Can_TxBufferIdType)box;
	
	
	/*************** 先记下本邮箱这次发送，再交给硬件 ***************/
	g_can_tx_pend[box].id = id;
	g_can_tx_pend[box].sending = 1U;
	
	/*************** 失败后释放 ***************/
	if(CAN_API_FAIL == Can_SendNonBlocking(CAN, &g_CanHandle, &txinfo))
	{
		g_can_tx_pend[box].sending = 0U;
		g_can_tx_pend[box].id = 0U;
		return 0;
	}
	
	return 1;
}






