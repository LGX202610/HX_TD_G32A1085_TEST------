/* Includes */
#include "app_canfd.h"
#include "hal_canfd.h"

#include "app_timer.h"

#include "CanNm.h"
#include "CanNm_Cfg.h"
#include "CanNm_Types.h"
#include "uds_port.h"
#include <string.h>



typedef struct
{
	App_CanCyclicPduCfg_t	cfg;		// 配置
	uint16_t				tick;		// 定时器计数
	uint8_t					pending;	// 发送标志
	uint8_t					hold_tx;	// 1=已组包，邮箱忙重发不再 pack
	uint8_t					tx_data[CAN_TX_FRAME_DATA_SIZE];
} App_CanCyclicPduInst_t;


static uint8_t s_cyclic_comm_enable = 1u;	/* UDS 0x28 通信控制可挂接此开关 */





static uint8_t s_can_alive_510 = 0u;	// 生命信号
#define CAN_510_ALIVE_BYTE			(1u)
#define CAN_510_ALIVE_MAX			(15u)
/**********************************************************
  * @brief	0x510 周期报文组包
  * @param  data   数据指针
  * @param  dlc    长度
  * @retval 
  * @note	
 **********************************************************/
static void can_510_pack(uint8_t *data, uint8_t dlc)
{
	uint8_t i = 0u;

	if((data == NULL) || (dlc == 0u) || (dlc > CAN_TX_FRAME_DATA_SIZE))
	{
		return;
	}

	for(i = 0u; i < dlc; i++)
	{
		data[i] = 0u;
	}

	/*************** 生命信号填充 ***************/
	data[CAN_510_ALIVE_BYTE] = (uint8_t)((data[CAN_510_ALIVE_BYTE] & 0xF0u) | (s_can_alive_510 & 0x0Fu));

	/*************** 生命信号步进 ***************/
	s_can_alive_510 = (s_can_alive_510 + 1u) % (CAN_510_ALIVE_MAX+1);
}



static uint8_t s_can_alive_511 = 0u;	// 生命信号
#define CAN_511_ALIVE_BYTE			(1u)
#define CAN_511_ALIVE_MAX			(15u)
/**********************************************************
  * @brief	0x511 周期报文组包
  * @param  data   数据指针
  * @param  dlc    长度
  * @retval 
  * @note	
 **********************************************************/
static void can_511_pack(uint8_t *data, uint8_t dlc)
{
	uint8_t i = 0u;

	if((data == NULL) || (dlc == 0u) || (dlc > CAN_TX_FRAME_DATA_SIZE))
	{
		return;
	}

	for(i = 0u; i < dlc; i++)
	{
		data[i] = 0u;
	}

	/*************** 生命信号填充 ***************/
	data[CAN_511_ALIVE_BYTE] = (uint8_t)((data[CAN_511_ALIVE_BYTE] & 0xF0u) | (s_can_alive_511 & 0x0Fu));

	/*************** 生命信号步进 ***************/
	s_can_alive_511 = (s_can_alive_511 + 1u) % (CAN_511_ALIVE_MAX+1);
}



/**********************************************************
  * @brief	0x650 周期报文组包
  * @param  data   数据指针
  * @param  dlc    长度
  * @retval 
  * @note	
 **********************************************************/
static void can_650_pack(uint8_t *data, uint8_t dlc)
{
	(void)data;
	(void)dlc;
}


/**********************************************************
  * @brief	0x62D 周期报文组包
  * @param  data   数据指针
  * @param  dlc    长度
  * @retval 
  * @note	
 **********************************************************/
static void can_62D_pack(uint8_t *data, uint8_t dlc)
{
	(void)data;
	(void)dlc;
}


#define APP_CAN_CYCLIC_PDU_MAX 4
static App_CanCyclicPduInst_t s_cyclic_pdu_list[APP_CAN_CYCLIC_PDU_MAX] =
{
	{
		.cfg =
		{
			.id = CAN_TX_FRAME_ID_510,
			.box = CAN_TX_BUF_ID_2,
			.frame_type = CAN_FRAME_TYPE_STD,
			.dlc = 8u,
			.period_10ms = 10u,
			.offset_10ms = 0u,
			.enable = 1u,
			.pack = can_510_pack,
		},
		.tick = 0u,
		.pending = 0u,
		.hold_tx = 0u,
	},

	{
		.cfg =
		{
			.id = CAN_TX_FRAME_ID_511,
			.box = CAN_TX_BUF_ID_3,
			.frame_type = CAN_FRAME_TYPE_STD,
			.dlc = 8u,
			.period_10ms = 10u,
			.offset_10ms = 2u,
			.enable = 1u,
			.pack = can_511_pack,
		},
		.tick = 0u,
		.pending = 0u,
		.hold_tx = 0u,
	},

	{
		.cfg =
		{
			.id = CAN_TX_FRAME_ID_650,
			.box = CAN_TX_BUF_ID_4,
			.frame_type = CAN_FRAME_TYPE_STD,
			.dlc = 8u,
			.period_10ms = 100u,
			.offset_10ms = 4u,
			.enable = 1u,
			.pack = can_650_pack,
		},
		.tick = 0u,
		.pending = 0u,
		.hold_tx = 0u,
	},

	{
		.cfg =
		{
			.id = CAN_TX_FRAME_ID_62D,
			.box = CAN_TX_BUF_ID_5,
			.frame_type = CAN_FRAME_TYPE_STD,
			.dlc = 8u,
			.period_10ms = 200u,
			.offset_10ms = 6u,
			.enable = 1u,
			.pack = can_62D_pack,
		},
		.tick = 0u,
		.pending = 0u,
		.hold_tx = 0u,
	},
};


static void app_can_cyclic_reset_all_alive(void)
{
	;
}


/**********************************************************
  * @brief	周期发送是否允许
  * @param  
  * @retval 
  * @note	
 **********************************************************/
static uint8_t app_can_cyclic_is_tx_allowed(void)
{
	if(s_cyclic_comm_enable == 0u)
	{
		return 0u;
	}

	if(CanNm_GetMode() != CANNM_MODE_NETWORK)
	{
		return 0u;
	}

	return 1u;
}


/**********************************************************
  * @brief	周期发送 10ms 定时器回调
  * @param  
  * @retval 
  * @note	
 **********************************************************/
static void app_can_cyclic_tick_10ms(void)
{
	uint8_t i = 0u;

	for(i = 0u; i < APP_CAN_CYCLIC_PDU_MAX; i++)
	{
		App_CanCyclicPduInst_t *pdu = &s_cyclic_pdu_list[i];

		if((pdu->cfg.enable == 0u) || (pdu->cfg.period_10ms == 0u) || (pdu->cfg.pack == NULL))
		{
			continue;
		}

		pdu->tick++;

		if(pdu->tick >= pdu->cfg.period_10ms)
		{
			pdu->tick = 0u;
			pdu->pending = 1u;
		}
	}
}


/**********************************************************
  * @brief	周期发送初始化
  * @param  
  * @retval 
  * @note	
 **********************************************************/
void app_can_cyclic_init(void)
{
	uint8_t i = 0u;

	app_can_cyclic_reset_all_alive();

	for(i = 0u; i < APP_CAN_CYCLIC_PDU_MAX; i++)
	{
		App_CanCyclicPduInst_t *pdu = &s_cyclic_pdu_list[i];

		if(pdu->cfg.period_10ms == 0u)
		{
			pdu->tick = 0u;
		}
		else
		{
			pdu->tick = (uint16_t)(pdu->cfg.offset_10ms % pdu->cfg.period_10ms);
		}

		pdu->pending = 0u;
		pdu->hold_tx = 0u;
	}
}


/**********************************************************
  * @brief	周期发送设置使能
  * @param  
  * @retval 
  * @note	
 **********************************************************/
void app_can_cyclic_set_enable(uint8_t index, uint8_t enable)
{
	if(index >= APP_CAN_CYCLIC_PDU_MAX)
	{
		return;
	}

	s_cyclic_pdu_list[index].cfg.enable = (enable != 0u) ? 1u : 0u;
	s_cyclic_pdu_list[index].tick = 0u;
	s_cyclic_pdu_list[index].pending = 0u;
	s_cyclic_pdu_list[index].hold_tx = 0u;
}


/**********************************************************
  * @brief	周期发送主函数
  * @param  
  * @retval 
  * @note	
 **********************************************************/
void app_can_cyclic_main(void)
{
	uint8_t i = 0u;
	CanNm_ModeType nm_mode = CanNm_GetMode();
	static CanNm_ModeType s_cyclic_last_nm_mode = CANNM_MODE_BUS_SLEEP;


	/*************** 周期发送模式切换 ***************/
	if((s_cyclic_last_nm_mode != CANNM_MODE_NETWORK) && (nm_mode == CANNM_MODE_NETWORK))
	{
		for(i = 0u; i < APP_CAN_CYCLIC_PDU_MAX; i++)
		{
			s_cyclic_pdu_list[i].hold_tx = 0u;
		}
		app_can_cyclic_reset_all_alive();
	}
	s_cyclic_last_nm_mode = nm_mode;

	/*************** 周期发送是否允许 ***************/
	if(app_can_cyclic_is_tx_allowed() == 0u)
	{
		return;
	}

	/*************** 周期发送主循环 ***************/
	for(i = 0u; i < APP_CAN_CYCLIC_PDU_MAX; i++)
	{
		App_CanCyclicPduInst_t *pdu = &s_cyclic_pdu_list[i];

		/*************** 周期发送条件判断 ***************/
		if((pdu->pending == 0u) || (pdu->cfg.enable == 0u) || (pdu->cfg.pack == NULL))
		{
			continue;
		}

		/*************** 周期发送清零 ***************/
		pdu->pending = 0u;

		/*************** 周期发送长度判断 ***************/
		if((pdu->cfg.dlc == 0u) || (pdu->cfg.dlc > CAN_TX_FRAME_DATA_SIZE))
		{
			continue;
		}

		/*************** 周期发送组包 ***************/
		if(pdu->hold_tx == 0u)
		{
			memset(pdu->tx_data, 0, sizeof(pdu->tx_data));
			pdu->cfg.pack(pdu->tx_data, pdu->cfg.dlc);
			pdu->hold_tx = 1u;
		}

		/*************** 周期发送发送 ***************/
		if(1 == app_can_tx_fram(pdu->cfg.box, pdu->cfg.id, pdu->cfg.frame_type, pdu->cfg.dlc, pdu->tx_data))
		{
			pdu->hold_tx = 0u;
		}
		else
		{
			/* 邮箱忙：重发同一帧，不再 pack，生命信号不重复 +1 */
			pdu->pending = 1u;
		}
	}
}

/**********************************************************
  * @brief	CanNM 发送函数实现
  * @param  
  * @retval 
  * @note	占用 CAN_TX_BUF_ID_0
 **********************************************************/
Std_ReturnType CanNm_WriteCanFrame(uint32_t CanId, uint8_t Dlc, const uint8_t *DataPtr)
{
	if ((DataPtr == NULL))
	{
		return E_NOT_OK;
	}
	
	if(1 == app_can_tx_fram(CAN_TX_BUF_ID_0, CanId, CAN_FRAME_TYPE_STD, Dlc, DataPtr))
	{
		return E_OK;
	}
	
	else
	{
		return E_NOT_OK;
	}
}




/**********************************************************
  * @brief	CANFD 发送函数
  * @param  
  * @retval 
  * @note		
 **********************************************************/
int app_can_tx_fram(uint32_t box, uint32_t id, uint8_t frame_type, uint32_t len, uint8_t *data)
{
	int ret = 0;

	if ((data == NULL) || ((frame_type != CAN_FRAME_TYPE_STD) && (frame_type != CAN_FRAME_TYPE_EXT)))
	{
		return 0;	
	}
	
	ret = hal_can_tx_fram(box, id, frame_type, len, data);
	
	return ret;
}



/**********************************************************
  * @brief	CAN TX 完成中断
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void app_can_tx_irq_hook(uint8_t box, uint32_t id)
{
	(void)box;
	 
	if(id == CANNM_CAN_ID)
	{
		CanNm_TxConfirmation(0);
	}
}
 
/**********************************************************
   * @brief	CAN 错误中断
   * @param  
   * @retval 
   * @note		
**********************************************************/
static void app_can_error_irq_hook(void)
{
	CanNm_BusOffIndication();
}



/**********************************************************
  * @brief	CanNM 回调
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_can_func_0(uint32_t id, uint32_t len, uint8_t *data)
{
	PduInfoType PduInfo;
	
	PduInfo.SduDataPtr = data;   
	PduInfo.MetaDataPtr = NULL_PTR;
	PduInfo.SduLength = len;    
	
	/* CanNm 接收回调 */
  CanNm_RxIndication(0u, &PduInfo);
}


/**********************************************************
  * @brief	
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_can_func_1(uint32_t id, uint32_t len, uint8_t *data)
{
	(void)id;
	(void)len;
	(void)data;
}


/**********************************************************
  * @brief	 
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_can_func_2(uint32_t id, uint32_t len, uint8_t *data)
{
	(void)id;
	(void)len;
	(void)data;
}


/**********************************************************
  * @brief	
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_can_func_3(uint32_t id, uint32_t len, uint8_t *data)
{
//	// 左前
//	if(((data[5] & 0x03) == 0x01) || ((data[5] & 0x03) == 0x02))
//	{
//		set_pedal_l_open();
//	}
//	else
//	{
//		set_pedal_l_close();
//	}
//	
//	// 左后
//	if((data[5] & 0x02) == 0x02)
//	{
//		set_pedal_l_open();
//	}
//	else
//	{
//		set_pedal_l_close();
//	}
}


/**********************************************************
  * @brief	
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_can_func_4(uint32_t id, uint32_t len, uint8_t *data)
{
	(void)id;
	(void)len;
	(void)data;
}





// uint8_t can_tx_buf0[8] = {1,2,3,4,5,6,7,8};
// uint8_t can_tx_buf1[8] = {1,1,1,1,1,1,1,1};
// uint8_t can_tx_buf2[8] = {5,5,5,5,5,5,5,5};
/**********************************************************
  * @brief	CANFD 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_can_init(void)
{
	/*************** CAN 初始化 ***************/
	hal_can_init();
	
	/*************** CAN TX 完成中断 ***************/
	hal_register_can_tx_irq_hook(app_can_tx_irq_hook);
	
	/*************** CAN 错误中断 ***************/
	hal_register_can_error_irq_hook(app_can_error_irq_hook);
	
	
	/*************** 接收回调 ***************/
	if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_0, CAN_FRAME_TYPE_STD, app_can_func_0))
	{
		;	//失败处理
	}
	
	if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_1, CAN_FRAME_TYPE_STD, app_can_func_1))
	{
		;	//失败处理
	}
	
	if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_2, CAN_FRAME_TYPE_EXT, uds_can_rx_callback))	
	{
		;	//UDS physical
	}

	if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_3, CAN_FRAME_TYPE_EXT, uds_can_rx_callback))
	{
		;	//UDS functional
	}

	if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_4, CAN_FRAME_TYPE_EXT, uds_can_rx_callback))
	{
		;	//UDS response
	}
	
	// if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_5, app_can_func_2))
	// {
	// 	;	//失败处理
	// }
	
	// if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_6, app_can_func_3))
	// {
	// 	;	//失败处理
	// }
	
	// if(0 == hal_register_can_rx_callback(CAN_RX_FRAME_ID_7, app_can_func_4))
	// {
	// 	;	//失败处理
	// }
	
	
	app_register_timer_callback(CanNm_MainFunc_Tick, TIMER_1MS_CALLBACK_TYPE);
	app_register_timer_callback(app_can_cyclic_tick_10ms, TIMER_10MS_CALLBACK_TYPE);

	app_can_cyclic_init();
	
	CanNm_Init(&CanNm_ConfigData);
	hal_can_enable();
	
	

//	hal_can_tx_fram(CAN_TX_BUF_ID_0, CAN_TX_FRAME_ID_0, 8, can_tx_buf0);
}




// /**********************************************************
//   * @brief	CANFD 休眠
//   * @param  
//   * @retval 
//   * @note		
//  **********************************************************/
// void app_can_sleep(void)
// {
// 	hal_can_sleep();
// }

// /**********************************************************
//   * @brief	CANFD 唤醒
//   * @param  
//   * @retval 
//   * @note		
//  **********************************************************/
// void app_can_wakeup(void)
// {
// 	hal_can_wakeup();
// }

