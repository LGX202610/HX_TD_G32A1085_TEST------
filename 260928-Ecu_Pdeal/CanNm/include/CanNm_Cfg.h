#ifndef CANNM_CFG_H
#define CANNM_CFG_H

#include "CanNm_Types.h"




/***************************** CANNM 配置宏 *****************************/

/* 是否使能开发错误检测 */
#define CANNM_DEV_ERROR_DETECT            STD_OFF

/* 是否启用 BusOff 检测处理 */
#define CANNM_BUS_OFF_DETECTION_ENABLED   STD_OFF

/* 是否启用被动唤醒模式 */
#define CANNM_PASSIVE_MODE_ENABLED        STD_OFF

/* 是否启用 PN  */
#define CANNM_PN_ENABLED                  STD_ON





/***************************** NM PDU 配置宏 *****************************/

/* NM 报文使用的 CAN ID */
#define CANNM_CAN_ID											0x441u

/* 本地节点 ID */
#define CANNM_NODE_ID                     0x41u

/* NM PDU 长度 */
#define CANNM_PDU_LENGTH                  8u

/* 用户数据长度 */
#define CANNM_USER_DATA_LENGTH            6u

/* NM 报文中 SNI（Source Node Identifier）字节偏移 */
#define CANNM_PDU_SNI_BIT                 0u

/* NM 报文中 CBV（Control Bit Vector）字节偏移 */
#define CANNM_PDU_CBV_BIT                 1u

/* CBV中 重复报文请求状态位掩码 */
#define CANNM_REPEAT_MESSAGE_REQUEST_CBV_MASK   0x01u	// 零跑：CBV bit0：重复报文请求状态位掩码

/* CBV中 激活唤醒位掩码 */
#define CANNM_ACTIVE_WAKEUP_CBV_MASK            0x10u	// 零跑：CBV bit4：激活唤醒位位掩码

/* CBV中 部分网络信息位掩码 */
#define CANNM_CBV_PNI_MASK                     0x40u





/***************************** PNC 配置宏 *****************************/

/* PN 位向量在 NM PDU 中的起始字节 */
#define CANNM_PN_INFO_OFFSET              2u

/* PN 位向量字节数， OFFSET+LENGTH <= PDU_LENGTH */
#define CANNM_PN_INFO_LENGTH              1u

/* 挂载 PNC 簇个数 */
#define CANNM_PNC_NUM                     1u

/* PNC 句柄 */
#define CANNM_PNC_0_ID                    0u

/* PNC0 在 PN Info 内的字节下标 */
#define CANNM_PNC_0_BYTE_INDEX            0u

/* PNC0 位掩码 */
#define CANNM_PNC_0_BIT_MASK              0x02u

/* 本节点关心的 PNC 位为 1 */
#define CANNM_PNC_0_FILTER_MASK_BYTE        CANNM_PNC_0_BIT_MASK






/***************************** 定时器 配置宏 *****************************/

/* T_REPEAT_MESSAGE，单位 ms */
#define CANNM_REPEAT_MESSAGE_TIME_MS      1600u

/* T_NM_TIMEOUT，单位 ms */
#define CANNM_TIMEOUT_TIME_MS             2000u

/* T_WAIT_BUS_SLEEP，单位 ms */
#define CANNM_WAIT_BUS_SLEEP_TIME_MS      2000u

/* 快速发送周期（本地唤醒后 RMS 快速发送子状态），单位 ms */
#define CANNM_IMMEDIATE_NM_CYCLE_TIME_MS  20u

/* 网络模式 NM 报文发送周期，单位 ms */
#define CANNM_MSG_CYCLE_TIME_MS           500u

/* 快速发送周期，次数 */
#define CANNM_IMMEDIATE_NM_TRANSMISSIONS  10u

/* T_WakeUp：进入 RMS 后必须在此时间内发出第一帧 NM 报文 */
#define CANNM_T_WAKEUP_TIME_MS            200u

/* 发出第一帧NM报文后，在CANNM_T_STARTx_APPFRAME_TIME_MS内发送第一帧应用报文 */
#define CANNM_T_STARTx_APPFRAME_TIME_MS   20u

/* 发送周期偏移，单位 ms */
#define CANNM_MSG_CYCLE_OFFSET_MS         0u

/* CanNm_MainFunction 调用周期，单位 ms */
#define CANNM_MAIN_FUNCTION_PERIOD_MS     10u

/* 外部请求 ERA 复位时间（ms），对应 CanNmPnResetTime。收到关心的 PNC bit=1 时重装，超时清 ERA 对应位。本步仅入库，定时逻辑在后续步骤实现 */
#define CANNM_PN_RESET_TIME_MS            1500u

/* 作为其它PNC的激活节点，需要在CANNM_T_GWWAKEUP_TIME_MS时间内快发 */
#define CANNM_T_GWWAKEUP_TIME_MS					10u



#if (CANNM_PN_INFO_LENGTH > CANNM_PN_INFO_LENGTH_MAX)
#error "CANNM_PN_INFO_LENGTH must be <= CANNM_PN_INFO_LENGTH_MAX"
#endif
#if (CANNM_PNC_NUM > CANNM_PNC_NUM_MAX)
#error "CANNM_PNC_NUM must be <= CANNM_PNC_NUM_MAX"
#endif
#if ((CANNM_PN_INFO_OFFSET + CANNM_PN_INFO_LENGTH) > CANNM_PDU_LENGTH)
#error "PN Info range exceeds CANNM_PDU_LENGTH"
#endif









extern const CanNm_ConfigType CanNm_ConfigData;		//配置对象声明，在 CanNm_Lcfg.c 中定义

//extern const CanNm_UserDataFilterType CanNm_RxPncFilter[1];
extern const CanNm_UserDataFilterType CanNm_TxUserdDataFilter[2];

/* 标准 PN FilterMask 与 PNC 配置表，在 CanNm_Lcfg.c 中定义 */
extern const CanNm_PncType CanNm_PncList[CANNM_PNC_NUM];

#endif /* CANNM_CFG_H */
