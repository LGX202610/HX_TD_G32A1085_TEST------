#ifndef CANNM_H
#define CANNM_H

#include "CanNm_Types.h"





/* =========================================================================
 * CanNm 对外 API 声明
 * ========================================================================= */
 
/**********************************************************
  * @brief	接收回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_RxIndication(PduIdType RxPduId, const PduInfoType *PduInfoPtr);
	
	
/**********************************************************
  * @brief	发送回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_TxConfirmation(PduIdType TxPduId);


/**********************************************************
  * @brief	CAN BusOff 回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_BusOffIndication(void);


/**********************************************************
  * @brief	CAN BusOff 恢复回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_BusOffRecoveryIndication(void);


/***********************************************************
  * @brief	CanNM 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_Init(const CanNm_ConfigType *ConfigPtr);


/***********************************************************
  * @brief	CanNM 
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_DeInit(void);


/***********************************************************
  * @brief	CanNM 远程PNC请求 
  * @param	PncId PNC实例 
  * @retval	
  * @note	
 **********************************************************/
Std_ReturnType CanNm_RemotePncRequest(PNCHandleType PncId);


/***********************************************************
  * @brief	CanNM 远程PNC释放 
  * @param	PncId PNC实例 
  * @retval	
  * @note		应由定时器超时调用释放
 **********************************************************/
Std_ReturnType CanNm_RemotePncRelease(PNCHandleType PncId);


/***********************************************************
  * @brief	CanNM 本地PNC请求
  * @param	PncId PNC实例 
  * @retval	
  * @note	
 **********************************************************/
Std_ReturnType CanNm_LocalPncRequest(PNCHandleType PncId);


/***********************************************************
  * @brief	CanNM 本地PNC释放
  * @param	PncId PNC实例 
  * @retval	
  * @note	
 **********************************************************/
Std_ReturnType CanNm_LocalPncRelease(PNCHandleType PncId);


/***********************************************************
  * @brief	CanNM 本地业务网络请求
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_LocalAppNetworkRequest(void);


/***********************************************************
  * @brief	CanNM 本地业务网络释放
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_LocalAppNetworkRelease(void);


/***********************************************************
  * @brief	CanNM 重复报文请求
  * @param  
  * @retval 
  * @note	若在 RMS、PBSM、BSM 中调用 CanNm_RepeatMessageRequest，CanNm 不得执行该服务，并返回 E_NOT_OK	
 **********************************************************/
void CanNm_RepeatMessageRequest(void);


/***********************************************************
  * @brief	CanNM 
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_PassiveStartUp(void);


/***********************************************************
  * @brief	CanNM 状态机心跳
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_MainFunc_Tick(void);


/***********************************************************
  * @brief	CanNM 状态机
  * @param   
  * @retval 
  * @note		
 **********************************************************/
void CanNm_MainFunction(void);


/***********************************************************
  * @brief	CanNM 获取报文 本地节点ID SNI
  * @param  
  * @retval 
  * @note		
 **********************************************************/
Std_ReturnType CanNm_GetNodeIdentifier(uint8_t *NodeIdPtr);


/***********************************************************
  * @brief	CanNM 写入报文 用户数据
  * @param  
  * @retval 
  * @note		
 **********************************************************/
Std_ReturnType CanNm_SetUserData(uint8_t Offset, uint8_t Length, const uint8_t *DataPtr);
	

/***********************************************************
  * @brief	CanNM 获取报文 用户数据
  * @param  
  * @retval 
  * @note		
 **********************************************************/
Std_ReturnType CanNm_GetUserData(uint8_t Offset, uint8_t Length, uint8_t *DataPtr);


/***********************************************************
  * @brief	CanNM 获取状态
  * @param  
  * @retval 
  * @note		
 **********************************************************/
CanNm_StateType CanNm_GetState(void);


/***********************************************************
  * @brief	CanNM 获取模式
  * @param  
  * @retval 
  * @note		
 **********************************************************/
CanNm_ModeType CanNm_GetMode(void);








extern const CanNm_ConfigType *CanNm_ConfigPtr;

 /***********************************************************
  * @brief	发送函数，硬件适配接口
  * @param  
  * @retval 
  * @note	该函数由用户实现，用于最终通过 CAN 外设发送一帧报文	
 **********************************************************/
extern Std_ReturnType CanNm_WriteCanFrame(uint32_t CanId, uint8_t Dlc, const uint8_t *DataPtr);



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
void CanNm_RxIndicationInternal(const PduInfoType *PduInfoPtr);


/***********************************************************
  * @brief	CAN 发送完成回调
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_TxConfirmationInternal(PduIdType TxPduId);


/***********************************************************
  * @brief	CAN BusOff 回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_BusOffIndicationInternal(void);


/***********************************************************
  * @brief	CAN BusOff 恢复回调入口
  * @param  
  * @retval 
  * @note	BusOff 恢复后清除 FlagBusOffDetected 标志
 **********************************************************/
void CanNm_BusOffRecoveryIndicationInternal(void);





#endif /* CANNM_H */


