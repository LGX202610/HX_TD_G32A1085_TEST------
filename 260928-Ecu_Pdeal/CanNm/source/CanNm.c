#include "CanNm_Types.h"
#include "CanNm_Cfg.h"
#include "CanNm.h"

#include <stdio.h>




CanNm_RuntimeType CanNm_Runtime;		//单实例状态
CanNm_PncRuntimeType CanNm_PncRuntime;		//单实例状态
const CanNm_ConfigType *CanNm_ConfigPtr = NULL_PTR;		//单实例配置




/* =========================================================================
 * 内部私有函数声明
 * ========================================================================= */
static boolean CanNm_PncIsIraActive(void);
static boolean CanNm_PncIsEraActive(void);
static boolean CanNm_PncIsEiraActive(void);
static void CanNm_PncUpdateEira(void);
static uint8_t CanNm_PncFindIndex(PNCHandleType PncId);
static void CanNm_PncResetRuntime(void);
static void CanNm_UpdateNwRequestFlags(void);
static void CanNm_PncRxPnInfo(const uint8_t *PnInfoPtr, uint8_t PnInfoLength);
static void CanNm_PncTxPnInfo(uint8_t *NmPduPtr, uint8_t PduLength);
static Std_ReturnType CanNm_TransmitNmPdu(void);
static void CanNm_EnterState(CanNm_StateType State);
static void CanNm_DecrementTimer(uint32_t *TimerPtr, uint32_t ElapsedMs);
static void CanNm_IncrementTimer(uint32_t *TimerPtr, uint32_t ElapsedMs);
static void CanNm_ResetRuntimeDefaults(void);
 
 
 
 
 
 


/* =========================================================================
 * CanNm 对外 API 实现
 * ========================================================================= */

/***********************************************************
  * @brief	CanNM 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_Init(const CanNm_ConfigType *ConfigPtr)
{
	uint8_t i;

	if(ConfigPtr == NULL_PTR)
	{
		return;
	}

	CanNm_ConfigPtr = ConfigPtr;

	CanNm_ResetRuntimeDefaults();

	CanNm_Runtime.TxUserDataLength = ConfigPtr->UserDataLength;

	for(i=0u; i<ConfigPtr->UserDataLength; i++)
	{
		if((2u + i) < 8u)
		{
			CanNm_Runtime.TxUserData[2u + i] = ConfigPtr->InitialUserData[i];
		}
	}

	CanNm_UpdateNwRequestFlags();

	CanNm_EnterState(CANNM_STATE_BUS_SLEEP);
}



/***********************************************************
  * @brief	CanNM 
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_DeInit(void)
{
	if(CanNm_ConfigPtr != NULL_PTR)
	{
		CanNm_EnterState(CANNM_STATE_BUS_SLEEP);
	}
	else
	{
		CanNm_Runtime.State = CANNM_STATE_BUS_SLEEP;
		CanNm_Runtime.Mode = CANNM_MODE_BUS_SLEEP;
	}

	CanNm_ResetRuntimeDefaults();
	CanNm_ConfigPtr = NULL_PTR;
}



/***********************************************************
  * @brief	CanNM 远程PNC请求 
  * @param	PncId PNC实例 
  * @retval	
  * @note	
 **********************************************************/
Std_ReturnType CanNm_RemotePncRequest(PNCHandleType PncId)
{
	uint8_t pncIndex;
	uint8_t byteIndex;
	uint8_t bitMask;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(CanNm_ConfigPtr->PnEnabled != TRUE)
	{
		return E_NOT_OK;
	}

	pncIndex = CanNm_PncFindIndex(PncId);
	if(pncIndex == CANNM_PNC_NUM_MAX)
	{
		return E_NOT_OK;
	}

	// 获取偏移字节和位掩码
	byteIndex = CanNm_ConfigPtr->PncList[pncIndex].ByteIndex;
	bitMask = CanNm_ConfigPtr->PncList[pncIndex].BitMask;

	// 获取PN位向量长度
	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}

	if(byteIndex >= pnInfoLength)
	{
		return E_NOT_OK;
	}

	// 设置外部请求
	CanNm_PncRuntime.Era[byteIndex] |= bitMask;
	// 重置定时器
	CanNm_PncRuntime.PnResetTimerMs[pncIndex] = CanNm_ConfigPtr->PnResetTime;
	// 更新EIRA
	CanNm_PncUpdateEira();
	// 更新网络请求标志
	CanNm_UpdateNwRequestFlags();

	return E_OK;
}



/***********************************************************
  * @brief	CanNM 远程PNC释放 
  * @param	PncId PNC实例 
  * @retval	
  * @note		应由定时器超时调用释放
 **********************************************************/
Std_ReturnType CanNm_RemotePncRelease(PNCHandleType PncId)
{
	uint8_t pncIndex;
	uint8_t byteIndex;
	uint8_t bitMask;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(CanNm_ConfigPtr->PnEnabled != TRUE)
	{
		return E_NOT_OK;
	}

	pncIndex = CanNm_PncFindIndex(PncId);
	if(pncIndex == CANNM_PNC_NUM_MAX)
	{
		return E_NOT_OK;
	}

	// 获取偏移字节和位掩码
	byteIndex = CanNm_ConfigPtr->PncList[pncIndex].ByteIndex;
	bitMask = CanNm_ConfigPtr->PncList[pncIndex].BitMask;
	// 获取PN位向量长度
	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}
	if(byteIndex >= pnInfoLength)
	{
		return E_NOT_OK;
	}

	// 清除外部请求
	CanNm_PncRuntime.Era[byteIndex] &= (uint8_t)(~bitMask);
	// 重置定时器
	CanNm_PncRuntime.PnResetTimerMs[pncIndex] = 0u;
	// 更新EIRA
	CanNm_PncUpdateEira();
	// 更新网络请求标志
	CanNm_UpdateNwRequestFlags();

	return E_OK;
}



/***********************************************************
  * @brief	CanNM 本地PNC请求
  * @param	PncId PNC实例 
  * @retval	
  * @note	
 **********************************************************/
Std_ReturnType CanNm_LocalPncRequest(PNCHandleType PncId)
{
	uint8_t pncIndex;
	uint8_t byteIndex;
	uint8_t bitMask;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(CanNm_ConfigPtr->PnEnabled != TRUE)
	{
		return E_NOT_OK;
	}

	pncIndex = CanNm_PncFindIndex(PncId);
	if(pncIndex == CANNM_PNC_NUM_MAX)
	{
		return E_NOT_OK;
	}

	// 获取偏移字节和位掩码
	byteIndex = CanNm_ConfigPtr->PncList[pncIndex].ByteIndex;
	bitMask = CanNm_ConfigPtr->PncList[pncIndex].BitMask;
	// 获取PN位向量长度
	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}
	if(byteIndex >= pnInfoLength)
	{
		return E_NOT_OK;
	}

	// 设置内部请求
	CanNm_PncRuntime.Ira[byteIndex] |= bitMask;
	// 更新EIRA
	CanNm_PncUpdateEira();
	// 更新网络请求标志
	CanNm_UpdateNwRequestFlags();

	return E_OK;
}



/***********************************************************
  * @brief	CanNM 本地PNC释放
  * @param	PncId PNC实例 
  * @retval	
  * @note	
 **********************************************************/
Std_ReturnType CanNm_LocalPncRelease(PNCHandleType PncId)
{
	uint8_t pncIndex;
	uint8_t byteIndex;
	uint8_t bitMask;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(CanNm_ConfigPtr->PnEnabled != TRUE)
	{
		return E_NOT_OK;
	}

	pncIndex = CanNm_PncFindIndex(PncId);
	if(pncIndex == CANNM_PNC_NUM_MAX)
	{
		return E_NOT_OK;
	}

	// 获取偏移字节和位掩码
	byteIndex = CanNm_ConfigPtr->PncList[pncIndex].ByteIndex;
	bitMask = CanNm_ConfigPtr->PncList[pncIndex].BitMask;
	// 获取PN位向量长度
	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}
	if(byteIndex >= pnInfoLength)
	{
		return E_NOT_OK;
	}

	// 清除内部请求
	CanNm_PncRuntime.Ira[byteIndex] &= (uint8_t)(~bitMask);
	// 更新EIRA
	CanNm_PncUpdateEira();
	// 更新网络请求标志
	CanNm_UpdateNwRequestFlags();

	return E_OK;
}



/***********************************************************
  * @brief	CanNM 本地业务网络请求
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_LocalAppNetworkRequest(void)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if(CanNm_Runtime.State==CANNM_STATE_BUS_SLEEP || CanNm_Runtime.State==CANNM_STATE_PREPARE_BUS_SLEEP)
	{
		CanNm_Runtime.FlagActiveWakeup = TRUE;	// 主动唤醒置位
	}

	// 如果启用PN，且需要作为其它PNC的激活节点，需要在CANNM_T_GWWAKEUP_TIME_MS时间内快发，
	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		;		//本节点不作为其它PNC激活节点，不实现
	}

	// 置位本地业务网络请求标志
	CanNm_Runtime.FlagLocalAppNWRequest = TRUE;
	// 更新网络请求标志
	CanNm_UpdateNwRequestFlags();
}



/***********************************************************
  * @brief	CanNM 本地业务网络释放
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_LocalAppNetworkRelease(void)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	// 清除本地业务网络请求标志
	CanNm_Runtime.FlagLocalAppNWRequest = FALSE;
	// 更新网络请求标志
	CanNm_UpdateNwRequestFlags();
}



/***********************************************************
  * @brief	CanNM 重复报文请求
  * @param  
  * @retval 
  * @note	若在 RMS、PBSM、BSM 中调用 CanNm_RepeatMessageRequest，CanNm 不得执行该服务，并返回 E_NOT_OK	
 **********************************************************/
void CanNm_RepeatMessageRequest(void)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if((CanNm_Runtime.State != CANNM_STATE_NORMAL_OPERATION) && (CanNm_Runtime.State != CANNM_STATE_READY_SLEEP))
	{
		return;
	}

	CanNm_Runtime.FlagLocalRMRequest = TRUE;
}



/***********************************************************
  * @brief	CanNM 
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_PassiveStartUp(void)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if(CanNm_ConfigPtr->PassiveModeCfg == FALSE)
	{
		return;
	}

	/* 被动启动：参与 NM 状态机，但抑制 NM 发送，直到 NetworkRequest */
	CanNm_Runtime.FlagPassiveMode = TRUE;

	if((CanNm_Runtime.State==CANNM_STATE_BUS_SLEEP) || (CanNm_Runtime.State==CANNM_STATE_PREPARE_BUS_SLEEP))
	{
		CanNm_Runtime.WakeupReason = CANNM_WAKEUP_REASON_REMOTE;	// 被动启动，远程唤醒
		CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);
	}
}



/***********************************************************
  * @brief	CanNM 状态机心跳
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static volatile uint32_t time_cannm_mainfunc_tick = 0;		//CanNm_MainFunction的心跳
void CanNm_MainFunc_Tick(void)
{
	time_cannm_mainfunc_tick++;
	
	if(time_cannm_mainfunc_tick>1000u)
	{
		time_cannm_mainfunc_tick=1000u;
		
		/* 考虑超时是否强制调用 */
		//
	}
}



/***********************************************************
  * @brief	CanNM 状态机
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void CanNm_MainFunction(void)
{
	uint8_t i=0u;
	uint8_t byteIndex=0u;
	uint8_t bitMask=0u;
	uint8_t pncCount=0u;
	boolean eraSet=FALSE;
	uint32_t ElapsedMs = 0;
	

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	
	ElapsedMs = time_cannm_mainfunc_tick;
	if(ElapsedMs < CanNm_ConfigPtr->MainFunctionPeriod_Value)
	{
		return;
	}


	/*************************** PNC 周期处理 ***************************/
	if((CanNm_ConfigPtr->PnEnabled == TRUE) && (CanNm_ConfigPtr->PncList != NULL_PTR))
	{
		pncCount = CanNm_ConfigPtr->PncCount;
		if(pncCount > CANNM_PNC_NUM_MAX)
		{
			pncCount = CANNM_PNC_NUM_MAX;
		}

		for(i = 0u; i < pncCount; i++)
		{
			// 获取偏移字节和位掩码
			byteIndex = CanNm_ConfigPtr->PncList[i].ByteIndex;
			bitMask = CanNm_ConfigPtr->PncList[i].BitMask;

			// 是否有外部请求
			eraSet = ((CanNm_PncRuntime.Era[byteIndex] & bitMask) != 0u) ? TRUE : FALSE;

			if(eraSet == FALSE)
			{
				CanNm_PncRuntime.PnResetTimerMs[i] = 0u;	// 重置定时器
			}
			else
			{
				CanNm_DecrementTimer(&CanNm_PncRuntime.PnResetTimerMs[i], ElapsedMs);
				if(CanNm_PncRuntime.PnResetTimerMs[i] == 0u)
				{
					/* 超时清该簇 ERA，并重算 FlagEraActive / FlagRemoteNWRequest */
					(void)CanNm_RemotePncRelease(CanNm_ConfigPtr->PncList[i].PncId);
				}
			}
		}
	}

	
	// 进入状态机前再检测一次标志
	CanNm_UpdateNwRequestFlags();
	

	/*************************** 状态机 ***************************/
	switch(CanNm_Runtime.State)
	{
		
		/*************************** BSM 总线休眠模式 ***************************/
		case CANNM_STATE_BUS_SLEEP:
		
				/************** 本地唤醒/网络请求 **************/
				if(CanNm_Runtime.FlagLocalNWRequest==TRUE)
				{
					CanNm_Runtime.WakeupReason = CANNM_WAKEUP_REASON_LOCAL;	// 本地唤醒优先于远程
					
					CanNm_Runtime.FlagActiveWakeup = TRUE;									// CBV 主动唤醒，发送确认后清除
					CanNm_Runtime.FlagImmTransmitState = TRUE;							// 快速发送子状态
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);						// 零跑NM02，本地唤醒，BSM跳转RMS ITS
					
					break;
				}
				
				/************** 远程唤醒/网络请求 **************/
				if(CanNm_Runtime.FlagRemoteNWRequest==TRUE)
				{
					CanNm_Runtime.FlagImmTransmitState = FALSE;				// 正常发送子状态
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);			// 零跑NM03，远程唤醒，BSM跳转RMS NTS
					
					break;
				}
			
		break;
		
		
			


		/*************************** PBSM 预休眠模式 ***************************/
		case CANNM_STATE_PREPARE_BUS_SLEEP:
			
				/************** WaitBusSleepTimerMs 定时器超时 **************/
				CanNm_DecrementTimer(&CanNm_Runtime.WaitBusSleepTimerMs, ElapsedMs);
				if(CanNm_Runtime.WaitBusSleepTimerMs == 0u)
				{
					printf("超时 WaitBusSleepTimer\r\n");
					
					if(CanNm_Runtime.FlagLocalNWRequest==FALSE && CanNm_Runtime.FlagRemoteNWRequest==FALSE)
					{
						CanNm_EnterState(CANNM_STATE_BUS_SLEEP);				// 零跑NM17，WaitBusSleepTimerMs超时，PBSM跳转BSM
					}
					
					break;
				}
								
				
				/************** 本地网络请求 **************/
				if(CanNm_Runtime.FlagLocalNWRequest == TRUE)
				{
					CanNm_Runtime.WakeupReason = CANNM_WAKEUP_REASON_LOCAL;
					
					CanNm_Runtime.FlagActiveWakeup = TRUE;
					CanNm_Runtime.FlagImmTransmitState = TRUE;				// 快速发送子状态
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);			// 零跑NM16，本地唤醒请求，PBSM跳转RMS ITS
				}
				
				
				/************** 远程网络请求 **************/
				if(CanNm_Runtime.FlagRemoteNWRequest == TRUE)
				{
					// 零跑，PBSM收到NM报文或被网络请求，默认进入RMS
					CanNm_Runtime.FlagImmTransmitState = FALSE;				// 正常发送子状态
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);			// 零跑NM15，远程唤醒，PBSM跳转RMS NTS			
				}
				
			
		break;
			

			
		
				
		/*************************** RMS 重复报文状态 ***************************/
		case CANNM_STATE_REPEAT_MESSAGE:
			
				/************** NmTimeoutTimerMs 定时器超时 **************/
				CanNm_DecrementTimer(&CanNm_Runtime.NmTimeoutTimerMs, ElapsedMs);
				if(CanNm_Runtime.NmTimeoutTimerMs == 0u)
				{
					printf("超时 TimeoutTimer\r\n");
					
					/* 关 PN：超时即远程消失。开 PN 远程只跟 ERA/PnReset */
					if(CanNm_ConfigPtr->PnEnabled != TRUE)
					{
						CanNm_Runtime.FlagRemoteNWRequest = FALSE;
					}
					CanNm_Runtime.NmTimeoutTimerMs = CanNm_ConfigPtr->NmTimeoutTime_Value;		// 零跑NM05，RMS NmTimeOut超时重置
				}
				
				
				/************** RepeatMessageTimerMs 定时器超时 **************/
				CanNm_DecrementTimer(&CanNm_Runtime.RepeatMessageTimerMs, ElapsedMs);
				if(CanNm_Runtime.RepeatMessageTimerMs == 0u)
				{
					printf("超时 RepeatMessageTimer\r\n");

					// 仍有网络请求，进入 NOS
					if(CanNm_Runtime.FlagLocalNWRequest==TRUE || CanNm_Runtime.FlagRemoteNWRequest==TRUE)
					{					
						CanNm_EnterState(CANNM_STATE_NORMAL_OPERATION);		// 零跑NM06，RMS跳转NOS
					}
					// 网络已释放，进入 RSS
					else
					{
						CanNm_EnterState(CANNM_STATE_READY_SLEEP);			// 零跑NM07,RMS跳转RSS
					}
					break;
				}
				
				
				/************** MsgCycleTimerMs 定时器超时 **************/			
				CanNm_IncrementTimer(&CanNm_Runtime.MsgCycleTimerMs, ElapsedMs);
				if(CanNm_Runtime.MsgCycleTimerMs >= CanNm_Runtime.CurrentCycleTimeMs)
				{
					CanNm_Runtime.MsgCycleTimerMs = 0u;
					printf("超时 MsgCycleTimer\r\n");

					printf(" RMS MsgCycleTime发\r\n");
					(void)CanNm_TransmitNmPdu();
										
					// RMS下，发送NM报文后，重置 NM_TIMEOUT
					CanNm_Runtime.NmTimeoutTimerMs = CanNm_ConfigPtr->NmTimeoutTime_Value;

					// 快速发送子状态下递减计数器
					if(CanNm_Runtime.ImmediateNmTransLeft > 0u)
					{
						CanNm_Runtime.ImmediateNmTransLeft--;

						// 发送完成，进入正常发送周期
						if(CanNm_Runtime.ImmediateNmTransLeft == 0u)
						{
							CanNm_Runtime.CurrentCycleTimeMs = CanNm_ConfigPtr->MsgCycleTime_Value;		// 零跑NM04 ITS跳转NTS
						}
					}
					else
					{
						CanNm_Runtime.CurrentCycleTimeMs = CanNm_ConfigPtr->MsgCycleTime_Value;
					}
				}
				
				
				/************** 本地RMR请求 **************/
				if(CanNm_Runtime.FlagLocalRMRequest == TRUE)
				{
					// 零跑规范没有定义此行为，原autosar标准，重启NmRepeatMessageTime，切换至ITS
	//				CanNm_Runtime.RepeatMessageTimerMs = CanNm_ConfigPtr->RepeatMessageTime_Value;
	//				CanNm_Runtime.ImmediateNmTransLeft = CanNm_ConfigPtr->ImmediateNmTrans_Cnt;
	//				CanNm_Runtime.CurrentCycleTimeMs = CanNm_ConfigPtr->ImmediateNmCycleTime_Value;
				}
				
				
				/************** 远程RMR请求 **************/
				if(CanNm_Runtime.FlagRemoteRMRequest == TRUE)
				{				
					// 零跑规范没有定义此行为，原autosar标准，重启NmRepeatMessageTime，不切换子状态
	//				CanNm_Runtime.RepeatMessageTimerMs = CanNm_ConfigPtr->RepeatMessageTime_Value;
				}
				
				
				/************** 本地网络请求 **************/
				if(CanNm_Runtime.FlagLocalNWRequest == TRUE)
				{
					// 零跑规范没有定义此行为，原autosar标准，无状态跳转
				}
				
			
				/************** 远程网络请求 **************/
				if(CanNm_Runtime.FlagRemoteNWRequest == TRUE)
				{
					// 零跑规范仅要求重置NmTimeOut，已在接收回调中完成
				}
			
		break;

			
			
		
		/*************************** NOS 常规运行状态 ***************************/
		case CANNM_STATE_NORMAL_OPERATION:
			
				/************** NmTimeoutTimerMs 定时器超时 **************/
				CanNm_DecrementTimer(&CanNm_Runtime.NmTimeoutTimerMs, ElapsedMs);
				if(CanNm_Runtime.NmTimeoutTimerMs == 0u)
				{
					printf("超时 TimeoutTimer\r\n");

					if(CanNm_ConfigPtr->PnEnabled != TRUE)
					{
						CanNm_Runtime.FlagRemoteNWRequest = FALSE;
					}
					CanNm_Runtime.NmTimeoutTimerMs = CanNm_ConfigPtr->NmTimeoutTime_Value;	// 零跑NM09，NmTimeOut超时重置
				}

				/************** MsgCycleTimerMs 定时器超时 **************/
				CanNm_IncrementTimer(&CanNm_Runtime.MsgCycleTimerMs, ElapsedMs);
				if(CanNm_Runtime.MsgCycleTimerMs >= CanNm_ConfigPtr->MsgCycleTime_Value)
				{
					printf("超时 MsgCycleTimer\r\n");
					CanNm_Runtime.MsgCycleTimerMs = 0u;
					
					if((CanNm_Runtime.FlagPassiveMode == FALSE) || (CanNm_Runtime.FlagLocalNWRequest == TRUE))
					{
						printf(" NOS MsgCycleTime发\r\n");
						(void)CanNm_TransmitNmPdu();

						// NOS下，发送NM报文后，重置 NM_TIMEOUT
						CanNm_Runtime.NmTimeoutTimerMs = CanNm_ConfigPtr->NmTimeoutTime_Value;			
					}
				}

				/************** 本地RMR请求 **************/
				if(CanNm_Runtime.FlagLocalRMRequest == TRUE)
				{
					CanNm_Runtime.FlagImmTransmitState = TRUE;				// 快速发送子状态
					CanNm_Runtime.FlagRepeatMessageRequest = TRUE;		// NM报文RMR置1
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);			// 零跑NM08，本地RMR请求，NOS跳转RMS ITS
					
					break;
				}
				
				/************** 远程RMR请求 **************/
				if(CanNm_Runtime.FlagRemoteRMRequest == TRUE)
				{
					CanNm_Runtime.FlagImmTransmitState = FALSE;				// 正常发送子状态
					CanNm_Runtime.FlagRepeatMessageRequest = FALSE;		// NM报文RMR置0
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);			// 零跑NM08，远程RMR请求，NOS跳转RMS NTS
					
					break;
				}
				
				/************** 本地网络请求 **************/
				if(CanNm_Runtime.FlagLocalNWRequest == TRUE)
				{
					// 零跑规范没有定义此行为，原autosar标准，无状态跳转
				}
				
				/************** 远程网络请求 **************/
				if(CanNm_Runtime.FlagRemoteNWRequest == TRUE)
				{
					// 零跑规范仅要求重置NmTimeOut，已在接收回调中完成
				}
				
				/************** 网络释放 **************/
				if(CanNm_Runtime.FlagLocalNWRequest==FALSE && CanNm_Runtime.FlagRemoteNWRequest==FALSE)
				{
					CanNm_EnterState(CANNM_STATE_READY_SLEEP);				// 零跑NM10，网络释放，NOS跳转RSS
					
					break;
				}
			
		break;

	
		

			
		/*************************** RSS 准备睡眠状态 ***************************/
		case CANNM_STATE_READY_SLEEP:
			
				/************** NmTimeoutTimerMs 定时器超时 **************/
				CanNm_DecrementTimer(&CanNm_Runtime.NmTimeoutTimerMs, ElapsedMs);
				if(CanNm_Runtime.NmTimeoutTimerMs == 0u)
				{				
					if(CanNm_ConfigPtr->PnEnabled != TRUE)
					{
						CanNm_Runtime.FlagRemoteNWRequest = FALSE;
					}
					
					if(CanNm_Runtime.FlagLocalNWRequest==FALSE && CanNm_Runtime.FlagRemoteNWRequest==FALSE)
					{
						printf("超时 TimeoutTimer\r\n");
						CanNm_EnterState(CANNM_STATE_PREPARE_BUS_SLEEP);	// 零跑NM14，NmTimeOut超时，网络已释放，RSS跳转PBSM
					}
					else
					{
						/* PN 下 ERA 仍在：远程 PNC 未释放，重装超时，继续留在 RSS */
						CanNm_Runtime.NmTimeoutTimerMs = CanNm_ConfigPtr->NmTimeoutTime_Value;
					}
				}
				
				
				/************** 本地RMR请求 **************/
				if(CanNm_Runtime.FlagLocalRMRequest == TRUE)
				{
					CanNm_Runtime.FlagImmTransmitState = TRUE;				// 快速发送子状态
					CanNm_Runtime.FlagRepeatMessageRequest = TRUE;		// NM报文RMR置1
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);			// 零跑NM12，本地RMR请求，RSS跳转RMS
					
					break;
				}

				/************** 远程RMR请求 **************/
				if(CanNm_Runtime.FlagRemoteRMRequest == TRUE)
				{
					CanNm_Runtime.FlagImmTransmitState = FALSE;				// 正常发送子状态
					CanNm_Runtime.FlagRepeatMessageRequest = FALSE;		// NM报文RMR置0
					CanNm_EnterState(CANNM_STATE_REPEAT_MESSAGE);			// 零跑NM12，远程RMR请求，RSS跳转RMS
					
					break;
				}
				
				/************** 本地网络请求 **************/
				if(CanNm_Runtime.FlagLocalNWRequest == TRUE)
				{
					CanNm_EnterState(CANNM_STATE_NORMAL_OPERATION);		// 零跑NM11，本地唤醒请求，RSS跳转NOS
					
					break;
				}
				
				/************** 远程网络请求 **************/
				if(CanNm_Runtime.FlagRemoteNWRequest == TRUE)
				{
					// 零跑规范仅要求重置NmTimeOut，已在接收回调中完成，原autosar标准，RSS跳转至NOS
				}

		break;

			
			
			
		default:
			
		break;
	}

	time_cannm_mainfunc_tick = 0;	//心跳清除
}



/***********************************************************
  * @brief	CanNM 获取报文 本地节点ID SNI
  * @param  
  * @retval 
  * @note		
 **********************************************************/
Std_ReturnType CanNm_GetNodeIdentifier(uint8_t *NodeIdPtr)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(NodeIdPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	*NodeIdPtr = CanNm_ConfigPtr->NodeId;
	
	return E_OK;
}



/***********************************************************
  * @brief	CanNM 写入报文 用户数据
  * @param  
  * @retval 
  * @note		
 **********************************************************/
Std_ReturnType CanNm_SetUserData(uint8_t Offset, uint8_t Length, const uint8_t *DataPtr)
{
  uint8_t i;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(DataPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	/* 检查范围是否合法 */
	if(((uint16_t)Offset+(uint16_t)Length) > (uint16_t)CanNm_ConfigPtr->UserDataLength)
	{
		return E_NOT_OK;
	}

	/* 用户数据从 PDU Byte2 开始 */
	for(i=0u; i<Length; i++)
	{
		CanNm_Runtime.TxUserData[2u+Offset+i] = DataPtr[i];
	}

	return E_OK;
}


/***********************************************************
  * @brief	CanNM 获取报文 用户数据
  * @param  
  * @retval 
  * @note		
 **********************************************************/
Std_ReturnType CanNm_GetUserData(uint8_t Offset, uint8_t Length, uint8_t *DataPtr)
{
	uint8_t i;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(DataPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(((uint16_t)Offset+(uint16_t)Length) > (uint16_t)CanNm_ConfigPtr->UserDataLength)
	{
		return E_NOT_OK;
	}

	for(i=0u; i<Length; i++)
	{
		DataPtr[i] = CanNm_Runtime.TxUserData[2u+Offset+i];
	}

	return E_OK;
}


/***********************************************************
  * @brief	CanNM 获取状态
  * @param  
  * @retval 
  * @note		
 **********************************************************/
CanNm_StateType CanNm_GetState(void)
{
	return CanNm_Runtime.State;
}


/***********************************************************
  * @brief	CanNM 获取模式
  * @param  
  * @retval 
  * @note		
 **********************************************************/
CanNm_ModeType CanNm_GetMode(void)
{
	return CanNm_Runtime.Mode;
}











/* =========================================================================
 * 内部共享函数
 * 说明：
 * 底层 CAN 驱动回调入口
 * 这些函数由 CanNm 框架提供，用户应在底层, CAN 接收/发送/错误回调中调用，不应直接调用
 * ========================================================================= */

/***********************************************************
  * @brief	CAN接收回调入口
  * @param  
  * @retval 
  * @note		收到 NM 报文时调用
 **********************************************************/
void CanNm_RxIndicationInternal(const PduInfoType *PduInfoPtr)
{
	uint8_t i=0u;
	uint8_t cbv=0u;
	uint8_t nodeId=0u;
	uint8_t pncByte=0u;

	boolean flagValidPNI = FALSE;
	boolean flag_RemoteRMRequest = FALSE;
	boolean flagValidRemoteNm = FALSE;

	
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if(PduInfoPtr == NULL_PTR)
	{
		return;
	}

	if(PduInfoPtr->SduDataPtr == NULL_PTR)
	{
		return;
	}

	if(PduInfoPtr->SduLength < 2u)
	{
		return;
	}
	
	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		if(CanNm_ConfigPtr->PncCount == 0u || CanNm_ConfigPtr->PncList == NULL_PTR)
		{
			return;
		}
	}


	/*************************** nodeid ***************************/
	nodeId = PduInfoPtr->SduDataPtr[CANNM_PDU_SNI_BIT];
	(void)nodeId;	//不做检查



	/*************************** cbv ***************************/
	cbv = PduInfoPtr->SduDataPtr[CANNM_PDU_CBV_BIT];

	// bit0 重复报文请求位，远程RMR请求
	flag_RemoteRMRequest = ((cbv & CANNM_REPEAT_MESSAGE_REQUEST_CBV_MASK) == CANNM_REPEAT_MESSAGE_REQUEST_CBV_MASK) ? TRUE : FALSE;

	// bit6 网络指示位，PN 启用时 PNI 须置 1
	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		if((cbv & CANNM_CBV_PNI_MASK) != CANNM_CBV_PNI_MASK)
		{
			// return;		// 零跑，PNI 须置 1
			flagValidPNI = FALSE;
		}
		else
		{
			flagValidPNI = TRUE;
		}
	}

	

	/*************************** user data ***************************/
	// PNC实现，bit17，零跑，有无唤醒需求
	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		if(flagValidPNI == TRUE)
		{
			CanNm_PncRxPnInfo(&PduInfoPtr->SduDataPtr[CanNm_ConfigPtr->PnInfoOffset], CanNm_ConfigPtr->PnInfoLength);

			/* 只认本节点关心的 ERA，不把任意 NM 当成远程请求 */
			if(CanNm_Runtime.FlagEraActive == TRUE)
			{
				flagValidRemoteNm = TRUE;
			}
		}
	}
	else
	{
		flagValidRemoteNm = TRUE;
	}

	// 其它bit实现


	
	/*************************** 有效NM报文处理 ***************************/
	if(flagValidRemoteNm == TRUE)
	{
		// 远程RMR请求 
		if(flag_RemoteRMRequest == TRUE)
		{
			CanNm_Runtime.FlagRemoteRMRequest = TRUE;	// 不做赋值，避免没被MainFunction消费的RMR请求被覆盖
		}

		/* 关 PN：远程请求直接置位。开 PN 已由 PncRxPnInfo 按 ERA 写入 FlagRemoteNWRequest */
		if(CanNm_ConfigPtr->PnEnabled != TRUE)
		{
			CanNm_Runtime.FlagRemoteNWRequest = TRUE;
		}

		// 唤醒源：远程唤醒，仅BSM/PBSM 下收到有效 NM 报文才标记
		if((CanNm_Runtime.Mode == CANNM_MODE_BUS_SLEEP) || (CanNm_Runtime.Mode == CANNM_MODE_PREPARE_BUS_SLEEP))
		{
			if(CanNm_Runtime.WakeupReason != CANNM_WAKEUP_REASON_LOCAL)	// 本地唤醒优先，不覆盖
			{
				CanNm_Runtime.WakeupReason = CANNM_WAKEUP_REASON_REMOTE;	// 远程唤醒
			}
		}

		// 零跑NM13，网络模式下收到NM重置NmTimeOut
		if(CanNm_Runtime.Mode == CANNM_MODE_NETWORK)
		{
			CanNm_Runtime.NmTimeoutTimerMs = CanNm_ConfigPtr->NmTimeoutTime_Value;
		}
	}
}





/***********************************************************
  * @brief	CAN 发送完成回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_TxConfirmationInternal(PduIdType TxPduId)
{
	(void)TxPduId;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	/* 单通道，直接清除发送等待标志 */
	CanNm_Runtime.FlagTxPending = FALSE;
}



/***********************************************************
  * @brief	CAN BusOff 回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_BusOffIndicationInternal(void)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if(CanNm_ConfigPtr->BusOffDetectionCfg == FALSE)
	{
		return;
	}

	CanNm_Runtime.FlagBusOffDetected = TRUE;

	// BusOff 后进入 BSM；BusOffDetected 不在 EnterState(BSM) 中清除，保持至 BusOff 恢复 
	CanNm_EnterState(CANNM_STATE_BUS_SLEEP);
}



/***********************************************************
  * @brief	CAN BusOff 恢复回调入口
  * @param  
  * @retval 
  * @note	BusOff 恢复后清除 FlagBusOffDetected 标志
 **********************************************************/
void CanNm_BusOffRecoveryIndicationInternal(void)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if(CanNm_ConfigPtr->BusOffDetectionCfg == FALSE)
	{
		return;
	}

	CanNm_Runtime.FlagBusOffDetected = FALSE;
}








/* =========================================================================
 * 内部私有函数实现
 * ========================================================================= */

/***********************************************************
  * @brief	判断 IRA 是否仍有内部 PNC 请求。
  * @param	
  * @retval	
  * @note	
 **********************************************************/
static boolean CanNm_PncIsIraActive(void)
{
	uint8_t i;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return FALSE;
	}

	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}

	for(i = 0u; i < pnInfoLength; i++)
	{
		if(CanNm_PncRuntime.Ira[i] != 0u)
		{
			return TRUE;
		}
	}

	return FALSE;
}




/***********************************************************
  * @brief	判断 ERA 是否仍有外部 PNC 请求。
  * @param	
  * @retval	
  * @note	
 **********************************************************/
static boolean CanNm_PncIsEraActive(void)
{
	uint8_t i;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return FALSE;
	}

	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}

	for(i = 0u; i < pnInfoLength; i++)
	{
		if(CanNm_PncRuntime.Era[i] != 0u)
		{
			return TRUE;
		}
	}

	return FALSE;
}




/***********************************************************
  * @brief	判断本通道是否仍有 PNC 需要保持通信。
  * @param	
  * @retval	
  * @note	
 **********************************************************/
static boolean CanNm_PncIsEiraActive(void)
{
	uint8_t i;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return FALSE;
	}

	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}

	for(i = 0u; i < pnInfoLength; i++)
	{
		if(CanNm_PncRuntime.Eira[i] != 0u)
		{
			return TRUE;
		}
	}

	return FALSE;
}




/***********************************************************
  * @brief	更新EIRA
  * @param	
  * @retval	
  * @note	
 **********************************************************/
static void CanNm_PncUpdateEira(void)
{
	uint8_t i;
	uint8_t pnInfoLength;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;
	if(pnInfoLength > CANNM_PN_INFO_LENGTH_MAX)
	{
		pnInfoLength = CANNM_PN_INFO_LENGTH_MAX;
	}

	for(i=0u; i<pnInfoLength; i++)
	{
		CanNm_PncRuntime.Eira[i] = (uint8_t)(CanNm_PncRuntime.Ira[i] | CanNm_PncRuntime.Era[i]);
	}
}




/***********************************************************
  * @brief	按实例索引 PncList 下标
  * @param	
  * @retval	
  * @note		0 .. CANNM_PNC_NUM_MAX-1 有效；CANNM_PNC_NUM_MAX 表示未找到
 **********************************************************/
static uint8_t CanNm_PncFindIndex(PNCHandleType PncId)
{
	uint8_t i;
	uint8_t pncCount;

	if((CanNm_ConfigPtr == NULL_PTR) || (CanNm_ConfigPtr->PncList == NULL_PTR))
	{
		return CANNM_PNC_NUM_MAX;
	}

	pncCount = CanNm_ConfigPtr->PncCount;
	if(pncCount > CANNM_PNC_NUM_MAX)
	{
		pncCount = CANNM_PNC_NUM_MAX;
	}

	for(i=0u; i<pncCount; i++)
	{
		if(CanNm_ConfigPtr->PncList[i].PncId == PncId)
		{
			return i;
		}
	}

	return CANNM_PNC_NUM_MAX;
}




/***********************************************************
  * @brief	清零 PNC 位向量与 PnReset 定时器
  * @param	
  * @retval	
  * @note		
 **********************************************************/
static void CanNm_PncResetRuntime(void)
{
	uint8_t i;

	for(i = 0u; i < CANNM_PN_INFO_LENGTH_MAX; i++)
	{
		CanNm_PncRuntime.Ira[i]  = 0u;
		CanNm_PncRuntime.Era[i]  = 0u;
		CanNm_PncRuntime.Eira[i] = 0u;
	}

	for(i = 0u; i < CANNM_PNC_NUM_MAX; i++)
	{
		CanNm_PncRuntime.PncState[i]       = PNC_NO_COMMUNICATION;
		CanNm_PncRuntime.PnResetTimerMs[i] = 0u;
	}
}




/***********************************************************
  * @brief	合成请求标志位
  * @param	
  * @param	
  * @retval	
  * @note	
 **********************************************************/
static void CanNm_UpdateNwRequestFlags(void)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		CanNm_Runtime.FlagIraActive = CanNm_PncIsIraActive();
		CanNm_Runtime.FlagEraActive = CanNm_PncIsEraActive();
		CanNm_Runtime.FlagRemoteNWRequest = CanNm_Runtime.FlagEraActive;
	}
	else
	{
		CanNm_Runtime.FlagIraActive = FALSE;
		CanNm_Runtime.FlagEraActive = FALSE;
	}

	CanNm_Runtime.FlagLocalNWRequest =
		((CanNm_Runtime.FlagLocalAppNWRequest == TRUE) || (CanNm_Runtime.FlagIraActive == TRUE)) ? TRUE : FALSE;
	// CanNm_Runtime.FlagRemoteNWRequest =
	// 	((CanNm_Runtime.FlagRemoteAppNWRequest == TRUE) || (CanNm_Runtime.FlagEraActive == TRUE)) ? TRUE : FALSE;
}




/***********************************************************
  * @brief	接收端解析PN位向量
  * @param	PnInfoPtr，PN_Info；
  * @param	PnInfoLength，长度
  * @retval	
  * @note	
 **********************************************************/
static void CanNm_PncRxPnInfo(const uint8_t *PnInfoPtr, uint8_t PnInfoLength)
{
	uint8_t i=0;
	
	uint8_t bitMask=0;
	uint8_t rxPnByte=0;
	uint8_t byteIndex=0;
	
	if(PnInfoPtr == NULL_PTR)
	{
		return;	
	}

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}

	if(CanNm_ConfigPtr->PncList == NULL_PTR)
	{
		return;
	}
		
	// 遍历PNC实例
	for(i=0u; i<CanNm_ConfigPtr->PncCount; i++)
	{
		byteIndex = CanNm_ConfigPtr->PncList[i].ByteIndex;
		bitMask = CanNm_ConfigPtr->PncList[i].BitMask;
		
		if(byteIndex >= PnInfoLength)
		{
			continue;		
		}
		
		// PN位向量所在byte
		rxPnByte = PnInfoPtr[byteIndex];
		
		// PNC请求位判断
		if((rxPnByte & bitMask) == bitMask)
		{
			CanNm_PncRuntime.Era[byteIndex] |= bitMask;		// 外部请求
			CanNm_PncRuntime.PnResetTimerMs[i] = CanNm_ConfigPtr->PnResetTime;	// 重置定时器
			
			CanNm_PncUpdateEira();		// 更新EIRA
		}
		/* 收到该位为 0 不立刻清 ERA，等 PnReset 超时 */
	}

	/* 立即重算 FlagEraActive / FlagRemoteNWRequest */
	CanNm_UpdateNwRequestFlags();
}




/***********************************************************
  * @brief	CanNm PDU PN Info 构造
  * @param  NmPduPtr，PDU 指针
  * @param  PduLength，PDU 长度
  * @retval 
  * @note		
 **********************************************************/
static void CanNm_PncTxPnInfo(uint8_t *NmPduPtr, uint8_t PduLength)
{
	uint8_t i=0u;

	uint8_t pncCount=0;
	uint8_t pnInfoOffset=0;
	uint8_t pnInfoLength=0;
	
	uint8_t bitMask=0;
	uint8_t byteIndex=0;
	uint8_t	pduIndex=0;
	
	pncCount = CanNm_ConfigPtr->PncCount;			// 挂载的PNC数量
	pnInfoOffset = CanNm_ConfigPtr->PnInfoOffset;	// PN Info 起点
	pnInfoLength = CanNm_ConfigPtr->PnInfoLength;	// PN Info 长度

	if(pnInfoOffset+pnInfoLength>PduLength)
	{
		return;		// 越界
	}

	for(i=0u; i<pncCount; i++)
	{
		byteIndex = CanNm_ConfigPtr->PncList[i].ByteIndex;
		bitMask = CanNm_ConfigPtr->PncList[i].BitMask;

		pduIndex = (uint8_t)(pnInfoOffset + byteIndex);		// PDU 的绝对下标

		if(pduIndex>=PduLength)	
		{
			continue;		//越界
		}

		NmPduPtr[pduIndex] &= (uint8_t)(~bitMask);			// 先清除

		if((CanNm_PncRuntime.Ira[byteIndex] & bitMask) != 0u)
		{
			NmPduPtr[pduIndex] |= bitMask;					// 再置位
		}
	}	
}




/***********************************************************
  * @brief	发送一个 NM PDU
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static Std_ReturnType CanNm_TransmitNmPdu(void)
{
	Std_ReturnType ret;
	uint8_t cbv = 0u;
	uint8_t i;

	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return E_NOT_OK;
	}

	if(CanNm_ConfigPtr->PduLength > 8u)
	{
		return E_NOT_OK;
	}

	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		if(CanNm_ConfigPtr->PncCount == 0u || CanNm_ConfigPtr->PncList == NULL_PTR)
		{
			return E_NOT_OK;
		}
	}

	if((CanNm_Runtime.FlagPassiveMode == TRUE) && (CanNm_Runtime.FlagLocalNWRequest == FALSE))
	{
		return E_NOT_OK;	// 被动模式且无网络请求，抑制发送NM报文
	}

	if((CanNm_ConfigPtr->BusOffDetectionCfg == TRUE) && (CanNm_Runtime.FlagBusOffDetected == TRUE))
	{
		return E_NOT_OK;	// BusOff 期间抑制 NM 发送
	}

	if(CanNm_Runtime.FlagTxPending == TRUE)
	{
		return E_NOT_OK;
	}
	
	
	/*************************** 填充 NODEID ***************************/
	CanNm_Runtime.TxUserData[CANNM_PDU_SNI_BIT] = CanNm_ConfigPtr->NodeId;
	
	
	
	/*************************** 构造 CBV ***************************/
	// bit0 重复报文请求位，
	if(CanNm_Runtime.FlagRepeatMessageRequest == TRUE)
	{
		cbv |= CANNM_REPEAT_MESSAGE_REQUEST_CBV_MASK;
	}

	// PN关闭请求位，零跑未定义
	
	// NM协调器休眠就绪位，零跑未定义
	
	// bit4 激活唤醒位
	if(CanNm_Runtime.FlagActiveWakeup == TRUE)
	{
		cbv |= CANNM_ACTIVE_WAKEUP_CBV_MASK;
		// CanNm_Runtime.FlagActiveWakeup = FALSE;		// 作为事件标记，而不是持续状态。在发送成功后清除
	}

	// bit6 网络指示位
	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		cbv |= CANNM_CBV_PNI_MASK;		
	}

	CanNm_Runtime.TxUserData[CANNM_PDU_CBV_BIT] = cbv;


	/*************************** 构造 USERDATA ***************************/
	
	/***************************  PNC位图 ***************************/
	CanNm_PncTxPnInfo(CanNm_Runtime.TxUserData, CanNm_ConfigPtr->PduLength);

	/*************************** 其它OEM数据 ***************************/
	if(CanNm_ConfigPtr->PnEnabled == TRUE)
	{
		// bit56 重复报文请求状态
		if(CanNm_Runtime.State == CANNM_STATE_REPEAT_MESSAGE)
		{
			CanNm_Runtime.TxUserData[CanNm_ConfigPtr->TxUserDataFilter[0].ByteOffset] |= CanNm_ConfigPtr->TxUserDataFilter[0].Mask;
		}
		else 
		{
			CanNm_Runtime.TxUserData[CanNm_ConfigPtr->TxUserDataFilter[0].ByteOffset] &= (uint8_t)(~CanNm_ConfigPtr->TxUserDataFilter[0].Mask);
		}

		// bit57 VIU唤醒
		if(CanNm_Runtime.WakeupReason == CANNM_WAKEUP_REASON_LOCAL)
		{
			CanNm_Runtime.TxUserData[CanNm_ConfigPtr->TxUserDataFilter[1].ByteOffset] |= CanNm_ConfigPtr->TxUserDataFilter[1].Mask;
		}
		else 
		{
			CanNm_Runtime.TxUserData[CanNm_ConfigPtr->TxUserDataFilter[1].ByteOffset] &= (uint8_t)(~CanNm_ConfigPtr->TxUserDataFilter[1].Mask);
		}
	}


	// 唤醒源信息已在首帧 NM 中编码完毕，发送后清除
	if(CanNm_Runtime.WakeupReason != CANNM_WAKEUP_REASON_NONE)
	{
		CanNm_Runtime.WakeupReason = CANNM_WAKEUP_REASON_NONE;
	}

	
	/*************************** 发送 ***************************/
	CanNm_Runtime.FlagTxPending = TRUE;	// 标记发送等待
	ret = CanNm_WriteCanFrame(CanNm_ConfigPtr->CanId, CanNm_ConfigPtr->PduLength, CanNm_Runtime.TxUserData);

	
	// 发送失败处理
	if(ret != E_OK)
	{
		CanNm_Runtime.FlagTxPending = FALSE;
	}
	
	// 发送完成处理
	else
	{
		// CanNm_Runtime.FlagTxPending = FALSE;	// 异步发送，发送完成中断回调里清除
		CanNm_Runtime.FlagActiveWakeup = FALSE;	// 发送后清除主动唤醒标志
	}
	
	
	return ret;
}




/***********************************************************
  * @brief	状态切换及初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void CanNm_EnterState(CanNm_StateType State)
{
	if(CanNm_ConfigPtr == NULL_PTR)
	{
		return;
	}
	
//	CanNm_Runtime.State = State;
	
	
	switch(State)
	{
		/*************************** BSM 总线睡眠模式 ***************************/	
		case CANNM_STATE_BUS_SLEEP:
			
			// 总线睡眠模式
			CanNm_Runtime.Mode = CANNM_MODE_BUS_SLEEP;
			CanNm_Runtime.State = CANNM_STATE_BUS_SLEEP;
		
			// 标志位 
			CanNm_Runtime.FlagImmTransmitState			= FALSE;
			CanNm_Runtime.FlagRepeatMessageRequest	= FALSE;
		
			CanNm_Runtime.FlagLocalRMRequest		= FALSE;
			CanNm_Runtime.FlagRemoteRMRequest		= FALSE;
		
			CanNm_Runtime.FlagActiveWakeup			= FALSE;
			CanNm_Runtime.FlagPassiveMode			= FALSE;
			CanNm_Runtime.FlagTxPending				= FALSE;
			CanNm_Runtime.WakeupReason				= CANNM_WAKEUP_REASON_NONE;
		
			// 定时器 
			CanNm_Runtime.RepeatMessageTimerMs		= 0u;
			CanNm_Runtime.NmTimeoutTimerMs			= 0u;
			CanNm_Runtime.WaitBusSleepTimerMs		= 0u;
			CanNm_Runtime.MsgCycleTimerMs			= 0u;
			CanNm_Runtime.CurrentCycleTimeMs		= 0u;
			CanNm_Runtime.ImmediateNmTransLeft		= 0u;
			
			printf("进入 BSM\r\n");
		break;

		

		/*************************** PBSM 预睡眠模式 ***************************/	
		case CANNM_STATE_PREPARE_BUS_SLEEP:
			
			// 预睡眠模式 
			CanNm_Runtime.Mode = CANNM_MODE_PREPARE_BUS_SLEEP;
			CanNm_Runtime.State = CANNM_STATE_PREPARE_BUS_SLEEP;
			
			// 标志位
			CanNm_Runtime.FlagImmTransmitState		= FALSE;
			CanNm_Runtime.FlagRepeatMessageRequest	= FALSE;
		
			CanNm_Runtime.FlagLocalRMRequest 		= FALSE;
			CanNm_Runtime.FlagRemoteRMRequest		= FALSE;
		
			CanNm_Runtime.FlagActiveWakeup			= FALSE;
			CanNm_Runtime.FlagPassiveMode			= FALSE;
			CanNm_Runtime.FlagTxPending				= FALSE;
			CanNm_Runtime.FlagBusOffDetected		= FALSE;
			CanNm_Runtime.WakeupReason				= CANNM_WAKEUP_REASON_NONE;
		
			// 定时器
			CanNm_Runtime.RepeatMessageTimerMs		= 0u;
			CanNm_Runtime.NmTimeoutTimerMs			= 0u;
			CanNm_Runtime.MsgCycleTimerMs			= 0u;
			CanNm_Runtime.CurrentCycleTimeMs		= 0u;
			CanNm_Runtime.ImmediateNmTransLeft		= 0u;
			CanNm_Runtime.WaitBusSleepTimerMs		= CanNm_ConfigPtr->WaitBusSleepTime_Value;
		
			printf("进入 PBSM\r\n");
		break;

		

		/*************************** RMS 重复报文状态 ***************************/
		case CANNM_STATE_REPEAT_MESSAGE:
			
			// 网络模式  重复报文状态
			CanNm_Runtime.Mode = CANNM_MODE_NETWORK;
			CanNm_Runtime.State = CANNM_STATE_REPEAT_MESSAGE;
		
			// 标志位
//			CanNm_Runtime.FlagImmTransmitState		= FALSE;	// 进入快速发送子状态后取消置位，离开RMS取消置位	
//			CanNm_Runtime.FlagRepeatMessageRequest	= TRUE;		// 是否发送RMR取决于进入RMS时的状态，在离开RMS后才取消置位
		
			CanNm_Runtime.FlagLocalRMRequest		= FALSE;
			CanNm_Runtime.FlagRemoteRMRequest		= FALSE;
		
			// CanNm_Runtime.FlagActiveWakeup			= FALSE;		// 离开NWM模式后才清除主动唤醒标志
			CanNm_Runtime.FlagTxPending				= FALSE;
			CanNm_Runtime.FlagBusOffDetected		= FALSE;
		
			// 定时器 
			CanNm_Runtime.RepeatMessageTimerMs		= CanNm_ConfigPtr->RepeatMessageTime_Value;
			CanNm_Runtime.NmTimeoutTimerMs			= CanNm_ConfigPtr->NmTimeoutTime_Value;
//			CanNm_Runtime.MsgCycleTimerMs		= 0u;
			CanNm_Runtime.WaitBusSleepTimerMs		= 0u;


			// 快速发送子状态
			if(CanNm_Runtime.FlagImmTransmitState == TRUE)
			{
				CanNm_Runtime.FlagImmTransmitState = FALSE;
				
				printf("进入 RMS 快速发送子状态\r\n");
				
				CanNm_Runtime.ImmediateNmTransLeft = CanNm_ConfigPtr->ImmediateNmTrans_Cnt;
				CanNm_Runtime.CurrentCycleTimeMs = CanNm_ConfigPtr->ImmediateNmCycleTime_Value;
			}
			// 正常发送子状态
			else
			{				
				printf("进入 RMS 正常发送子状态\r\n");
				
				CanNm_Runtime.ImmediateNmTransLeft = 0u;
				CanNm_Runtime.CurrentCycleTimeMs = CanNm_ConfigPtr->MsgCycleTime_Value;
			}

			
			// 零跑要求简易实现，立即发送第一帧 NM 报文（T_WakeUp 0-200ms）/
			CanNm_Runtime.MsgCycleTimerMs = 0u;		
			printf(" RMS立即发\r\n");
			(void)CanNm_TransmitNmPdu();
							
		break;

		

		/*************************** NOS 常规运行状态 ***************************/
		case CANNM_STATE_NORMAL_OPERATION:
			
			// 网络模式  常规运行状态 
			CanNm_Runtime.Mode = CANNM_MODE_NETWORK;
			CanNm_Runtime.State = CANNM_STATE_NORMAL_OPERATION;
		
			// 标志位 
			CanNm_Runtime.FlagImmTransmitState		= FALSE;
			CanNm_Runtime.FlagRepeatMessageRequest	= FALSE;
		
			CanNm_Runtime.FlagLocalRMRequest		= FALSE;
			CanNm_Runtime.FlagRemoteRMRequest		= FALSE;
		
			// CanNm_Runtime.FlagActiveWakeup			= FALSE;		// 离开NWM模式后才清除主动唤醒标志
			CanNm_Runtime.FlagTxPending				= FALSE;
			CanNm_Runtime.FlagBusOffDetected		= FALSE;
		
			// 定时器 
			CanNm_Runtime.MsgCycleTimerMs			= CanNm_ConfigPtr->MsgCycleOffset_Value;
			CanNm_Runtime.CurrentCycleTimeMs		= CanNm_ConfigPtr->MsgCycleTime_Value;
//			CanNm_Runtime.NmTimeoutTimerMs			= CanNm_ConfigPtr->NmTimeoutTime_Value;
			CanNm_Runtime.RepeatMessageTimerMs		= 0u;
			CanNm_Runtime.WaitBusSleepTimerMs		= 0u;	
			CanNm_Runtime.ImmediateNmTransLeft		= 0u;
		
			printf("进入 NOS\r\n");
		break;

		
		
		
		/*************************** RSS 准备休眠状态 ***************************/
		case CANNM_STATE_READY_SLEEP:
			
			// 网络模式  准备休眠状态 
			CanNm_Runtime.Mode = CANNM_MODE_NETWORK;
			CanNm_Runtime.State = CANNM_STATE_READY_SLEEP;
			
			// 标志位 
			CanNm_Runtime.FlagImmTransmitState		= FALSE;
			CanNm_Runtime.FlagRepeatMessageRequest	= FALSE;
		
			CanNm_Runtime.FlagLocalRMRequest		= FALSE;
			CanNm_Runtime.FlagRemoteRMRequest		= FALSE;
		
			// CanNm_Runtime.FlagActiveWakeup			= FALSE;		// 离开NWM模式后才清除主动唤醒标志
			CanNm_Runtime.FlagTxPending				= FALSE;
			CanNm_Runtime.FlagBusOffDetected		= FALSE;
		
			// 定时器 
			CanNm_Runtime.NmTimeoutTimerMs			= CanNm_ConfigPtr->NmTimeoutTime_Value;	
			CanNm_Runtime.RepeatMessageTimerMs		= 0u;
			CanNm_Runtime.WaitBusSleepTimerMs		= 0u;
			CanNm_Runtime.MsgCycleTimerMs			= 0u;
			CanNm_Runtime.CurrentCycleTimeMs		= 0u;
			CanNm_Runtime.ImmediateNmTransLeft		= 0u;
		
			printf("进入 RSS\r\n");
		break;

		
		default:
		break;
	}
}




/***********************************************************
  * @brief	CanNM 递增定时器
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void CanNm_IncrementTimer(uint32_t *TimerPtr, uint32_t ElapsedMs)
{
	*TimerPtr += ElapsedMs;
}




/***********************************************************
  * @brief	CanNM 递减定时器
  * @param  
  * @retval 
  * @note		递减，不下溢
 **********************************************************/
static void CanNm_DecrementTimer(uint32_t *TimerPtr, uint32_t ElapsedMs)
{
	if(*TimerPtr > ElapsedMs)
	{
		*TimerPtr -= ElapsedMs;
	}
	else
	{
		*TimerPtr = 0u;
	}
}




/***********************************************************
  * @brief	运行时成员恢复缺省值
  * @param  
  * @retval 
  * @note		
 **********************************************************/ 
static void CanNm_ResetRuntimeDefaults(void)
{
	uint8_t i;

	CanNm_Runtime.WakeupReason							= CANNM_WAKEUP_REASON_NONE;
	CanNm_Runtime.FlagImmTransmitState			= FALSE;
	CanNm_Runtime.FlagRepeatMessageRequest	= FALSE;
	CanNm_Runtime.FlagLocalRMRequest				= FALSE;
	CanNm_Runtime.FlagRemoteRMRequest				= FALSE;
	CanNm_Runtime.FlagLocalAppNWRequest			= FALSE;
	// CanNm_Runtime.FlagRemoteAppNWRequest		= FALSE;
	CanNm_Runtime.FlagIraActive							= FALSE;
	CanNm_Runtime.FlagEraActive							= FALSE;
	CanNm_Runtime.FlagLocalNWRequest				= FALSE;
	CanNm_Runtime.FlagRemoteNWRequest				= FALSE;
	CanNm_Runtime.FlagPassiveMode						= FALSE;
	CanNm_Runtime.FlagActiveWakeup					= FALSE;
	CanNm_Runtime.FlagTxPending							= FALSE;
	CanNm_Runtime.FlagBusOffDetected				= FALSE;

	CanNm_Runtime.RepeatMessageTimerMs			= 0u;
	CanNm_Runtime.NmTimeoutTimerMs					= 0u;
	CanNm_Runtime.WaitBusSleepTimerMs				= 0u;
	CanNm_Runtime.MsgCycleTimerMs						= 0u;
	CanNm_Runtime.CurrentCycleTimeMs				= 0u;
	CanNm_Runtime.ImmediateNmTransLeft			= 0u;
	CanNm_Runtime.TxUserDataLength					= 0u;

	for(i = 0u; i < 8u; i++)
	{
		CanNm_Runtime.TxUserData[i] = 0u;
	}

	CanNm_PncResetRuntime();
}










