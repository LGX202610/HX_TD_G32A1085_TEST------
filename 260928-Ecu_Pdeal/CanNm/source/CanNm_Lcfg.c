#include "CanNm_Cfg.h"


//const CanNm_UserDataFilterType CanNm_RxPncFilter[1] = {
//	{0x02u, 0x02u},   /* Byte2 bit1：有无唤醒需求 */
//};

const CanNm_UserDataFilterType CanNm_TxUserdDataFilter[2] = {
	{0x07u, 0x01u},   /* Byte7 bit0：重复报文请求状态 */
	{0x07u, 0x02u},   /* Byte7 bit1-bit7：VIU 唤醒原因 */ 
};



// PNC 配置表，
const CanNm_PncType CanNm_PncList[CANNM_PNC_NUM] = {
	{
		.PncId     = CANNM_PNC_0_ID,					// 零跑，单PNC
		.ByteIndex = CANNM_PNC_0_BYTE_INDEX, 	// 零跑，bit17	
		.BitMask   = CANNM_PNC_0_BIT_MASK			// 零跑，bit17，有无唤醒需求
	}
};



// CANNM
const CanNm_ConfigType CanNm_ConfigData = {
    .CanId                	= CANNM_CAN_ID,
    .NodeId               	= CANNM_NODE_ID,

		.RepeatMessageTime_Value  	= CANNM_REPEAT_MESSAGE_TIME_MS,
    .NmTimeoutTime_Value        = CANNM_TIMEOUT_TIME_MS,
    .WaitBusSleepTime_Value   	= CANNM_WAIT_BUS_SLEEP_TIME_MS,
		.ImmediateNmCycleTime_Value = CANNM_IMMEDIATE_NM_CYCLE_TIME_MS,
		.MsgCycleTime_Value					= CANNM_MSG_CYCLE_TIME_MS,
		.ImmediateNmTrans_Cnt	      = CANNM_IMMEDIATE_NM_TRANSMISSIONS,
    .WakeupTimeout_Value      	= CANNM_T_WAKEUP_TIME_MS,
		.MsgCycleOffset_Value     	= CANNM_MSG_CYCLE_OFFSET_MS,
    .MainFunctionPeriod_Value 	= CANNM_MAIN_FUNCTION_PERIOD_MS,
	
    .PduLength            	= CANNM_PDU_LENGTH,
    .UserDataLength       	= CANNM_USER_DATA_LENGTH,
	
    .BusOffDetectionCfg     = CANNM_BUS_OFF_DETECTION_ENABLED,
    .PassiveModeCfg   	    = CANNM_PASSIVE_MODE_ENABLED,
		
		.PnEnabled					= ((CANNM_PN_ENABLED == STD_ON) ? TRUE : FALSE),
//		.RxPncFilter				= CanNm_RxPncFilter,
		.TxUserDataFilter			= CanNm_TxUserdDataFilter,
		
		.PnInfoOffset               = CANNM_PN_INFO_OFFSET,
		.PnInfoLength               = CANNM_PN_INFO_LENGTH,
		.PnResetTime                = CANNM_PN_RESET_TIME_MS,
		.PncCount                   = CANNM_PNC_NUM,
		
		.PncList                    = CanNm_PncList,
		
    .InitialUserData      	= {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}
};
