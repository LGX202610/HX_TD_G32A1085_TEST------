#include "CanNm_Types.h"
#include "CanNm.h"




//内部函数声明，在 CanNm.c 中实现
extern void CanNm_RxIndicationInternal(const PduInfoType *PduInfoPtr);
extern void CanNm_TxConfirmationInternal(PduIdType TxPduId);
extern void CanNm_BusOffIndicationInternal(void);
extern void CanNm_BusOffRecoveryIndicationInternal(void);





/**********************************************************
  * @brief	接收回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_RxIndication(PduIdType RxPduId, const PduInfoType *PduInfoPtr)
{
	(void)RxPduId; /* 单通道不检查 PduId */

	if(PduInfoPtr == NULL_PTR)
	{
		return;
	}

	CanNm_RxIndicationInternal(PduInfoPtr);
}


/**********************************************************
  * @brief	发送回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_TxConfirmation(PduIdType TxPduId)
{
	CanNm_TxConfirmationInternal(TxPduId);
}


/**********************************************************
  * @brief	CAN BusOff 回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_BusOffIndication(void)
{
	CanNm_BusOffIndicationInternal();
}


/**********************************************************
  * @brief	CAN BusOff 恢复回调入口
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void CanNm_BusOffRecoveryIndication(void)
{
	CanNm_BusOffRecoveryIndicationInternal();
}

