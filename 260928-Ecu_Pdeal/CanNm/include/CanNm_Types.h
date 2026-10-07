#ifndef CANNM_TYPES_H
#define CANNM_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include "g32a10xx.h"
/* =========================================================================
 * 基础类型定义
 * ========================================================================= */
#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

#ifndef STD_ON
#define STD_ON  1u
#endif

#ifndef STD_OFF
#define STD_OFF 0u
#endif

#ifndef E_OK
#define E_OK 0u
#endif

#ifndef E_NOT_OK
#define E_NOT_OK 1u
#endif

typedef uint8_t Std_ReturnType;


#ifndef BOOLEAN_DEFINED
typedef uint8_t boolean;
#define BOOLEAN_DEFINED
#endif

//#ifndef TRUE
//#define TRUE  1u
//#endif

//#ifndef FALSE
//#define FALSE 0u
//#endif



typedef uint16_t PduIdType;
typedef uint8_t  PduLengthType;
typedef uint8_t  NetworkHandleType;

/* AUTOSAR：PNC 句柄类型，用于标识一个 Partial Network Cluster */
typedef uint8_t  PNCHandleType;




/************** PDU信息结构 **************/
typedef struct {
    uint8_t *SduDataPtr;      /* 指向数据缓冲区 */
    uint8_t *MetaDataPtr;     /* 元数据指针，通常不用，可置为 NULL_PTR */
    PduLengthType SduLength;  /* 数据长度 */
} PduInfoType;


/************** USERDATA 过滤表项结构 **************/
typedef struct {
	uint8_t ByteOffset;	// 字节偏移
  uint8_t Mask;		    // 掩码
} CanNm_UserDataFilterType;


/***********************************************************
 * @brief  单个 PNC 在 PN 位向量中占用哪 1 bit
 * @note   ByteIndex 相对 PN Info 起点。PDU 绝对字节号 = PnInfoOffset + ByteIndex。
 **********************************************************/
typedef struct {
    PNCHandleType PncId;  /* 应用使用的 PNC 句柄，本 ECU 内唯一 */
    uint8_t ByteIndex;    /* PN Info 内的字节下标，0 .. PnInfoLength-1 */
    uint8_t BitMask;      /* 该字节内的 bit，例如 0x02 表示 bit1 */
} CanNm_PncType;


/***********************************************************
 * @brief  单个 PNC 是否仍需要通信（对齐 ComM PNC 状态名）
 **********************************************************/
typedef enum {
    PNC_NO_COMMUNICATION = 0, /* IRA、ERA 对应位均为 0 */
    PNC_PREPARE_SLEEP,        /* 内部已释放，睡眠准备过渡 */
    PNC_READY_SLEEP,          /* 内部已释放，等待 ERA 超时 */
    PNC_REQUESTED             /* IRA 或 ERA 对应位为 1 */
} CanNm_PncStateType;



#define CANNM_PN_INFO_LENGTH_MAX    1      //PN Info 长度最大值，假设长度为6，>=CANNM_PN_INFO_LENGTH
#define CANNM_PNC_NUM_MAX           1      //PNC 数量最大值，假设数量为8，>=CANNM_PNC_NUM
/***********************************************************
 * @brief  PNC 运行时数据（通道级一份）
 * @note   IRA/ERA/EIRA 的布局与 NM PDU 中 PN Info 位向量一致。
 *         有效长度为配置项 PnInfoLength，其余 MAX 尾部字节保持 0。
 **********************************************************/
typedef struct {
    /* Internal Request Array：本 ECU 应用通过 RequestPnc 置位的内部请求 */
    uint8_t Ira[CANNM_PN_INFO_LENGTH_MAX];
    /* External Request Array：总线上其它节点 NM 中、经 FilterMask 过滤后的外部请求 */
    uint8_t Era[CANNM_PN_INFO_LENGTH_MAX];
    /* External Internal Request Array：EIRA = IRA OR ERA，决定该簇是否仍需通信 */
    uint8_t Eira[CANNM_PN_INFO_LENGTH_MAX];
    /* 每个已配置 PNC 的当前状态，下标与 PncList[] 一致 */
    CanNm_PncStateType PncState[CANNM_PNC_NUM_MAX];
    /* 每个 PNC 的 ERA 复位剩余时间（ms）。收到对应 bit=1 时重装 PnResetTime，减到 0 则清 ERA 该位 */
    uint32_t PnResetTimerMs[CANNM_PNC_NUM_MAX];
} CanNm_PncRuntimeType;


/************** 网络管理状态机状态 **************/
typedef enum {
    CANNM_STATE_BUS_SLEEP = 0,       /* 总线休眠模式 */
    CANNM_STATE_PREPARE_BUS_SLEEP,   /* 准备总线休眠模式 */
    CANNM_STATE_REPEAT_MESSAGE,      /* 网络模式：重复报文状态 */
    CANNM_STATE_NORMAL_OPERATION,    /* 网络模式：正常运行状态 */
    CANNM_STATE_READY_SLEEP          /* 网络模式：准备休眠状态 */
} CanNm_StateType;


/************** 网络管理模式 **************/
typedef enum {
    CANNM_MODE_BUS_SLEEP = 0,        /* 总线休眠模式 */
    CANNM_MODE_PREPARE_BUS_SLEEP,    /* 准备总线休眠模式 */
    CANNM_MODE_NETWORK               /* 网络模式 */
} CanNm_ModeType;


/************** 唤醒源 **************/
typedef enum {
    CANNM_WAKEUP_REASON_NONE = 0,
    CANNM_WAKEUP_REASON_LOCAL,   /* 本地唤醒 */
    CANNM_WAKEUP_REASON_REMOTE   /* 远程唤醒 */
} CanNm_WakeupReasonType;






/*************************** CanNm 配置结构 ***************************/
typedef struct {
    uint32_t CanId;               			//CANNM报文ID 
    uint8_t  NodeId;              			//CANNM本地节点ID
	
		uint16_t RepeatMessageTime_Value; 			//RMS定时器
		uint16_t NmTimeoutTime_Value;       		//总线监控定时器
		uint16_t WaitBusSleepTime_Value;  			//预睡眠等待定时器	
		uint16_t ImmediateNmCycleTime_Value; 		//快速发送状态下，NM报文发送周期	 
		uint16_t MsgCycleTime_Value;    				//正常发送子状态或NOS状态下，NM报文发送周期  
		uint16_t ImmediateNmTrans_Cnt;					//快速发送子状态下，以周期时间ImmediateNmCycleTime_Value发送的NM报文数量
		uint16_t WakeupTimeout_Value;         	//进入 RMS 后必须在此时间内发出第一帧 NM 报文 */
    uint16_t MsgCycleOffset_Value;    			//发送周期偏移
    uint16_t MainFunctionPeriod_Value;			//CanNm_MainFunction 调用周期

    uint8_t  PduLength;           /* NM PDU 长度，通常为 8 */
    uint8_t  UserDataLength;      /* 用户数据长度，从 PDU Byte2 开始 */

    boolean  BusOffDetectionCfg; /* 是否启用 BusOff 检测处理 */
    boolean  PassiveModeCfg;     // 是否允许被动唤醒，CanNm_PassiveStartUp
		

    const CanNm_UserDataFilterType *TxUserDataFilter; /* 发送侧 UserData */

		boolean		PnEnabled;							// 是否启用 PN
    uint8_t		PnInfoOffset;						// PN位向量在 NM PDU 的起始字节 
    uint8_t		PnInfoLength;						// PN位向量字节数  
    uint16_t	PnResetTime;						// ERA 复位时间（ms）   
    uint8_t		PncCount;								// PNC 个数     
    const CanNm_PncType *PncList;			// PNC 列表

    uint8_t  InitialUserData[8];  // 初始用户数据，实际有效长度为 UserDataLength
} CanNm_ConfigType;



/*************************** CanNm 运行时状态 ***************************/
typedef struct {
    CanNm_ModeType  				Mode;             // 当前模式 
    CanNm_StateType 				State;            // 当前状态机状态    
		CanNm_WakeupReasonType  WakeupReason;			// 唤醒源

    boolean FlagRepeatMessageRequest;			    // 重复报文请求位置位标志，CBV bit0
    boolean FlagImmTransmitState;           	// 快速发送子状态标志，CBV bit1 对应快速发送子状态
    boolean FlagPassiveMode;                  // 被动唤醒标志，直至 NetworkRequest 或进入 BSM/PBSM 清除，CBV bit2 对应被动唤醒标志
    boolean FlagActiveWakeup;                 // 主动唤醒（事件）标志，CBV bit4

    boolean FlagTxPending;                    // 发送确认等待标志
    boolean FlagBusOffDetected;               // BusOff （锁存）标志位
	
    boolean FlagLocalRMRequest;   						// 本地重复报文请求  
		boolean FlagRemoteRMRequest;							// 远程重复报文请求
	
		boolean FlagLocalAppNWRequest;            // 本地业务网络请求
		// boolean FlagRemoteAppNWRequest;           // 远程业务网络请求
    boolean FlagIraActive;                    // 本地IRA激活标志
    boolean FlagEraActive;                    // 远程ERA激活标志

    boolean FlagLocalNWRequest;               // 本地网络请求
		boolean FlagRemoteNWRequest;              // 远程网络请求
    
	
    uint32_t RepeatMessageTimerMs;         // T_REPEAT_MESSAGE 定时器
    uint32_t NmTimeoutTimerMs;             // T_NM_TIMEOUT 定时器 
    uint32_t WaitBusSleepTimerMs;          // T_WAIT_BUS_SLEEP 定时器 
    uint32_t MsgCycleTimerMs;              // NM 报文发送周期定时器 
		uint16_t CurrentCycleTimeMs;           // 当前使用的发送周期（快速或正常） 
		uint16_t ImmediateNmTransLeft;				 // 剩余快速报文计数器 

    uint8_t  TxUserData[8];                // 发送 NM PDU 数据缓冲区
    uint8_t  TxUserDataLength;             // 用户数据长度
		
} CanNm_RuntimeType;


extern CanNm_PncRuntimeType CanNm_PncRuntime;


#endif /* CANNM_TYPES_H */


