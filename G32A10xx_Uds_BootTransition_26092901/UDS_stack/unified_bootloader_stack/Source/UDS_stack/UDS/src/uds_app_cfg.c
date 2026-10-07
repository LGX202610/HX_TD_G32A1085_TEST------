#include "uds_app_cfg.h"
#include "uds_app.h"
#include "TP.h"
#include "fls_app.h"
#include "boot.h"
#include "watchdog_hal.h"
#include "uds_alg_hal.h"
#include "user_versions.h" //lu-
#include "uds_verify.h"
#include "flash_hal_Cfg.h"
#include "uds_dtc_nvm.h"

/* UDS协议标准时间配置表，存放P2、P2*等服务响应超时参数 */
const UDS_APP_tTimeInfoStructType g_udsTimeConfigTable =
{
    1u,
    3u,
    10000u,
    8000u,
    50u,
    2000u
};

/* UDS全局基础配置结构体：会话类型、寻址模式、安全等级、S3超时、安全锁定计时 */
static UDS_APP_tInfoStructType g_udsConfiguration =
{
    BASIC_SESSION,                // 默认基础会话
    FAILURE_REQUEST_ID,           // 默认寻址异常标记
    NO_SECURITY,                  // 默认无安全访问
    0u,                           // S3服务超时计数
    0u,                           // 安全锁定超时计数
    0u,                           // P2服务端响应超时计数
    0u,                           // P2*扩展响应超时计数
    0u,                           // P2*响应挂起标记
};


/* 宏函数：设置当前接收报文的寻址模式（物理/功能寻址） */
#define MACRO_SET_REQUEST_ID(requsetId) (g_udsConfiguration.iReqIdMode = (requsetId))

#ifdef UDS_PROJECT_FOR_BOOTLOADER
#define UDS_AUTH_PARAM_LEN (1322u)
static uint8 s_aAuthParam[UDS_AUTH_PARAM_LEN];
/* 0x31 6001 例程结果为 0x04 才允许后续 2E(F198/F199)/34/36/37/FF00 */
static uint8 s_versionCheckPassed = FALSE;
/* 0x31 6000 例程结果为 0x04 才允许做 6001 */
static uint8 s_authenticityPassed = FALSE;
/* 0x31 0202 例程结果为 0x04 才允许擦除内存 FF00 */
static uint8 s_flashDriverCrcPassed = FALSE;
/* 安全访问通过=TRUE，未通过=FALSE；31 01 6000 未通过时回 NRC33 */
static uint8 s_secAccessPassed = FALSE;
/* 2E F198 工具序列号 / 2E F199 编程日期：写入成功=TRUE；跳过则 0x34 NRC22 */
static uint8 s_toolSerialPassed = FALSE;
static uint8 s_reprogDatePassed = FALSE;
/* 本周期 31 FF01 已把头标有效：11 01 才写 SRAM 成功标志，避免 APP 误发 51 01 */
static uint8 s_programCycleSucceeded = FALSE;

static uint8 UDS_APP_RejectIfProgVersionLow(struct UDS_APP_ServiceInfoType *pSrv,
                                            UDS_APP_tLocalAppMsgType *pMsg)
{
    (void)pSrv;
    (void)pMsg;
    /* 过渡刷 Boot：不卡 6001/指纹/0202 等顺序前提 */
    return FALSE;
}

/* 过渡刷 Boot：不要求先写 F198/F199 */
static uint8 UDS_APP_RejectIfFingerprintNotWritten(struct UDS_APP_ServiceInfoType *pSrv,
                                                  UDS_APP_tLocalAppMsgType *pMsg)
{
    (void)pSrv;
    (void)pMsg;
    return FALSE;
}

/* 过渡刷 Boot：不要求先做 0202 */
static uint8 UDS_APP_RejectIfFlashDriverCrcFail(struct UDS_APP_ServiceInfoType *pSrv,
                                               UDS_APP_tLocalAppMsgType *pMsg)
{
    (void)pSrv;
    (void)pMsg;
    return FALSE;
}
#endif

/* 填充 0x10 正响应中的 P2(1ms) / P2*(10ms) */
static void UDS_APP_FillP2ServerParams(uint8 *pHiLo)
{
    tLocalTime p2Ms = g_udsTimeConfigTable.cP2Time;
    tLocalTime p2StarEnc = (tLocalTime)(g_udsTimeConfigTable.cP2StarTime / 10u);

    pHiLo[0u] = (uint8)((p2Ms >> 8u) & 0xFFu);
    pHiLo[1u] = (uint8)(p2Ms & 0xFFu);
    pHiLo[2u] = (uint8)((p2StarEnc >> 8u) & 0xFFu);
    pHiLo[3u] = (uint8)(p2StarEnc & 0xFFu);
}

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/* 使能延迟下线功能宏定义时，存储等待复位延时标记与计时值 */
#ifdef ALLOW_DELAY_TIME
UDS_APP_DelayTimeInfoType gs_structForTimeInformation = {FALSE, 0u};
#endif

/* 单分区升级宏：擦除备份区标志位全局外部变量 */
#ifdef APP_SINGLE_UPDATA
extern boolean backupEraseFlag;
#endif

/* Flash下载状态全局外部变量，记录擦写、校验状态 */
extern flashDLStatusType flashDLStatusSingle;

/* 双分区升级宏：APP程序擦除起始地址全局静态变量 */
#ifdef APP_BICLE_UPDATA
static uint32 gs_eraseStartAddress = 0;
#endif
#endif

/* 编译器兼容告警屏蔽，消除未使用函数、变量警告 */
#if defined(__ICCARM__)
    #pragma diag_suppress=Pe177
#elif defined(__GNUC__) && !defined(__ARMCC_VERSION)
    #pragma GCC diagnostic ignored "-Wunused-function"
#else
    #pragma clang diagnostic ignored "-Wunused-function"
#endif

/* 全局静态块编号，下载传输时记录当前数据块序号 */
static volatile uint8 uBlockNum = 0u;

/* UDS服务注册表：每条条目对应SID、支持会话、支持寻址、所需安全等级、处理函数 */
static const UDS_APP_localServiceType gs_astUDSService[] =
{
    /* 0x10 会话控制服务 */
    {
        0x10u,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS, // 默认/编程/扩展会话全部支持
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,       // 物理+功能寻址都支持
        NO_SECURITY,                                 // 无需安全解锁
        UDS_APP_ShiftSession                         // 会话切换处理函数
    },
    /* 0x14 清除DTC信息 */
    {
        0x14u,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS,  // 所有会话支持
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,         // 物理+功能寻址都支持
        NO_SECURITY,                                   // 无需安全解锁
        UDS_APP_ClearDTCInformation                    // 处理函数
    },
	 /* 0x22 通过标识符读数据 lu- */
    {
        0x22u,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS,  // 默认/编程/扩展会话全部支持
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,         // 物理+功能寻址都支持（0x22通常两者都支持）
        NO_SECURITY,                                   // 读取不需要安全解锁
        UDS_APP_ReadDataByIdentifier                   // DID读取处理函数
    },
    /* 0x28 通信控制服务 */
    {
        0x28u,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS,
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,
        NO_SECURITY,
        UDS_APP_CommunicationSetting
    },
    /* 0x85 DTC故障码开关控制 */
    {
        0x85u,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS,
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,
        NO_SECURITY,
        UDS_APP_CtrlDtcSetting
    },

#ifdef UDS_PROJECT_FOR_BOOTLOADER
    /* 0x27 安全访问（仅Bootloader升级工程生效） */
    {
        0x27u,
        PROGRAM_SESS,                    // 仅编程会话可用
        ALLOW_PHYSICAL_ID,               // 仅支持物理寻址
        NO_SECURITY,                     // 服务本身无需前置解锁
        UDS_APP_SecAccessFunc
    },
    /* 0x2E 写入DID（写指纹、配置参数） */
    {
        0x2Eu,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        NO_SECURITY,                     /* 过渡刷 Boot：不卡 27 */
        UDS_APP_WriteDataIdent
    },
    /* 0x34 请求下载，Flash擦写前置服务 */
    {
        0x34u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        NO_SECURITY,
        UDS_APP_ReqDownload
    },
    /* 0x36 传输升级数据包 */
    {
        0x36u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        NO_SECURITY,
        UDS_APP_TransferProcess
    },
    /* 0x37 退出传输，校验程序CRC */
    {
        0x37u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        NO_SECURITY,
        UDS_APP_FinalizeTransfer
    },
    /* 0x31 例程控制（擦除、校验、备份APP） */
    {
        0x31u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        NO_SECURITY,
        UDS_APP_HandleRoutineOperation
    },
    /* 0x11 ECU硬件/软件复位 */
    {
        0x11u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,
        NO_SECURITY,
        UDS_APP_PerformEcuReset
    },
#endif
    /* 0x3E TesterPresent诊断仪在线保活 */
    {
        0x3Eu,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS,
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,
        NO_SECURITY,
        UDS_APP_HandleTesterPresence
    },
};

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/* 例程控制：擦除Flash内存对应的子功能ID数组 31 01 FF 00 */
static const uint8 gs_arrayEraseMemRoutineID[] = {0x31u, 0x01u, 0xFFu, 0x00u};
// ★ 新增：0x6000 校验文件合法性 RID
static const uint8 gs_arrayCheckAuthenticityRID[] = {0x31u, 0x01u, 0x60u, 0x00u};

// ★ 新增：0x6001 版本校验 RID
static const uint8 gs_arrayCheckVersionRID[] = {0x31u, 0x01u, 0x60u, 0x01u};

// ★ 新增：0x0203 下载数据完整性检查 RID
static const uint8 gs_arrayCheckDataIntegrityRID[] = {0x31u, 0x01u, 0x02u, 0x03u};
#ifdef APP_SINGLE_UPDATA
/* 单分区升级：擦除备份分区例程ID */
static const uint8 gs_arrayEraseBackupMemRoutineID[] = {0x31u, 0x01u, 0xFFu, 0x02u};
/* 单分区升级：主程序拷贝至备份分区例程ID */
static const uint8 gs_arrayEraseCopyBackupMemRoutineID[] = {0x31u, 0x01u, 0xFFu, 0x03u};
#endif

/* This array identifies the checksum routine ID */ //CRC校验
static const uint8 gs_arrayCksumRoutineID[] = {0x31u, 0x01u, 0x02u, 0x02u};
/* This array identifies the check programming dependency ID */ //依赖性检查
static const uint8 gs_arrayCheckProgranDepenID[] = {0x31u, 0x01u, 0xffu, 0x01u};
/* 写入硬件指纹DID 2E F1 5A */
static const uint8 gs_arrayWriteFingerprintID[] = {0x2Eu, 0xF1u, 0x5Au};
/* ★ 新增：0xF198 工具序列号 DID */
static const uint8 gs_arrayWriteToolSerialDID[] = {0x2Eu, 0xF1u, 0x98u};
/* ★ 新增：0xF199 编程日期 DID */
static const uint8 gs_arrayWriteReprogDateDID[] = {0x2Eu, 0xF1u, 0x99u};
#endif

/**
 * @brief 会话控制0x10服务处理函数
 * @param pLocalSrv 当前匹配的UDS服务配置结构体指针
 * @param pLocalMsg 接收/发送报文缓存结构体指针
 * @desc 处理默认/编程/扩展会话切换，支持抑制正响应bit7
 */
static void UDS_APP_ShiftSession(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg)
{
    uint8 ucLocalSubFunc = 0u; // 子功能号 01/02/03
    ASSERT(NULL_PTR == pLocalMsg); // 空指针断言，调试捕获非法入参
    ASSERT(NULL_PTR == pLocalSrv);

    UDS_APP_StartP2ServerTimer(); // 启动P2响应计时，约束0x10首次响应时限

    ucLocalSubFunc = pLocalMsg->aDataBuf[1u]; // 提取子功能字节

    // 填充标准正响应SID+0x40
    pLocalMsg->aDataBuf[0u] = pLocalSrv->serviceId + 0x40u;
    pLocalMsg->aDataBuf[1u] = ucLocalSubFunc;
    UDS_APP_FillP2ServerParams(&pLocalMsg->aDataBuf[2u]); /* P2=50ms, P2*=2000ms(10ms分辨率) */
    pLocalMsg->xDataMsgLength = 6u; // 默认响应长度6字节

    // 01/81：基础默认会话
    if ((0x01u == ucLocalSubFunc) || (0x81u == ucLocalSubFunc))
    {
        SetCurrentSession(BASIC_SESSION); // 切换至默认会话
        UDSCFGDebugLog("inter BASIC_SESSION\r\n");
        if (0x81u != ucLocalSubFunc)
        {
            /* bit7=0 需要回复正响应 */
        }
        else
        {
            pLocalMsg->xDataMsgLength = 0u; // bit7置1，抑制响应
        }
    }
    // 02/82：编程会话（升级模式）
    else if ((0x02u == ucLocalSubFunc) || (0x82u == ucLocalSubFunc))
    {
        SetCurrentSession(PROGRAM_SESS);
        UDSCFGDebugLog("inter PROGRAM_SESS\r\n");
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        /* 本编程会话开始：清周期 RAM 标志。不放 34，避免 Driver/APP 各清一次 */
        ResetProgrammingCycleFlags();
        /* 过渡刷 Boot：顺序前提全部视为已过 */
        s_versionCheckPassed = TRUE;
        s_authenticityPassed = TRUE;
        s_flashDriverCrcPassed = TRUE;
        s_secAccessPassed = TRUE;
        s_toolSerialPassed = TRUE;
        s_reprogDatePassed = TRUE;
        s_programCycleSucceeded = FALSE;
#endif
        if (0x82u != ucLocalSubFunc)
        {
            /* 正常回复响应 */
        }
        else
        {
            pLocalMsg->xDataMsgLength = 0u; // 抑制响应
        }
#ifdef UDS_PROJECT_FOR_APP
        //SetSlaveMcuInBootloader();
#endif
        RestartS3Server(); // 重置S3会话超时计时器
#ifdef UDS_PROJECT_FOR_APP
        BootloaderAccepteReq(); // APP工程下发进入boot请求
        pLocalMsg->pfTxMsgCb = &UDS_APP_PerformMcuReset; // 发送完成后复位
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_SERVICE_BUSY, pLocalMsg);
        UDS_APP_StartP2StarServerTimer(); // 0x78响应挂起，启动P2*扩展计时
        UDSCFGDebugLog("App Request Enter bootloader mode!\r\n");
#endif
    }
    // 03/83：扩展会话
    else if ((0x03u == ucLocalSubFunc) || (0x83u == ucLocalSubFunc))
    {
        SetCurrentSession(EXTENDED_SESS);
        UDSCFGDebugLog("inter EXTENDED_SESS\r\n");
        if (0x83u != ucLocalSubFunc)
        {
            /* 正常回复响应 */
        }
        else
        {
            pLocalMsg->xDataMsgLength = 0u; // 抑制响应
        }
        RestartS3Server(); // 刷新会话保活计时
    }
    // 非法子功能，返回不支持子功能负响应
    else
    {
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pLocalMsg);
    }
}

/**
 * @brief 获取当前S3会话剩余倒计时
 * @return 剩余计时计数值
 */
static tLocalTime UDS_APP_AcquireS3ServerCountdown(void)
{
    return (g_udsConfiguration.iS3Time);
}

/**
 * @brief S3倒计时递减指定时长
 * @param dSubtract 需要减去的计时单位
 */
static void UDS_APP_DecrementS3ServerCountdown(tLocalTime dSubtract)
{
    g_udsConfiguration.iS3Time -= dSubtract;
}

/**
 * @brief 获取安全访问锁定剩余计时
 * @return 安全锁剩余计数
 */
static tLocalTime UDS_APP_AcquireSecurityLockCountdown(void)
{
    return (g_udsConfiguration.iSecLockTime);
}

/**
 * @brief 安全锁定计时器递减
 * @param dSubtract 递减数值
 */
static void UDS_APP_DecrementSecurityLockCountdown(tLocalTime dSubtract)
{
    g_udsConfiguration.iSecLockTime -= dSubtract;
}

/**
 * @brief 获取P2服务端响应剩余倒计时
 * @return 剩余计时计数值
 */
static tLocalTime UDS_APP_AcquireP2ServerCountdown(void)
{
    return (g_udsConfiguration.iP2Time);
}

/**
 * @brief P2服务端响应倒计时递减
 * @param dSubtract 递减数值
 */
static void UDS_APP_DecrementP2ServerCountdown(tLocalTime dSubtract)
{
    g_udsConfiguration.iP2Time -= dSubtract;
}

/**
 * @brief 获取P2*扩展响应剩余倒计时
 * @return 剩余计时计数值
 */
static tLocalTime UDS_APP_AcquireP2StarServerCountdown(void)
{
    return (g_udsConfiguration.iP2StarTime);
}

/**
 * @brief P2*扩展响应倒计时递减
 * @param dSubtract 递减数值
 */
static void UDS_APP_DecrementP2StarServerCountdown(tLocalTime dSubtract)
{
    g_udsConfiguration.iP2StarTime -= dSubtract;
}

/**
 * @brief 启动P2服务端响应计时器，收到0x10请求时调用，约束首次响应时限
 */
void UDS_APP_StartP2ServerTimer(void)
{
    g_udsConfiguration.iP2Time = UDS_APP_TIME_TO_COUNT(g_udsTimeConfigTable.cP2Time);
}

/**
 * @brief 启动P2*扩展响应计时器，发送0x78响应挂起时调用，并置挂起标记
 */
void UDS_APP_StartP2StarServerTimer(void)
{
    g_udsConfiguration.iP2StarTime = UDS_APP_TIME_TO_COUNT(g_udsTimeConfigTable.cP2StarTime);
    g_udsConfiguration.iP2StarPending = TRUE;
}

/**
 * @brief 停止P2*计时器并清除响应挂起标记，最终响应完成后调用
 */
void UDS_APP_StopP2StarServerTimer(void)
{
    g_udsConfiguration.iP2StarTime = 0u;
    g_udsConfiguration.iP2StarPending = FALSE;
}

/**
 * @brief 判断P2服务端响应是否超时
 * @return TRUE超时 / FALSE未超时
 */
uint8 UDS_APP_CheckP2ServerIsTimeout(void)
{
    return (0u == g_udsConfiguration.iP2Time) ? TRUE : FALSE;
}

/**
 * @brief 判断P2*扩展响应是否超时（仅响应挂起状态有效）
 * @return TRUE超时 / FALSE未超时
 */
uint8 UDS_APP_CheckP2StarServerIsTimeout(void)
{
    return ((TRUE == g_udsConfiguration.iP2StarPending) &&
            (0u == g_udsConfiguration.iP2StarTime)) ? TRUE : FALSE;
}

/**
 * @brief 查询当前是否处于P2*响应挂起状态
 * @return TRUE挂起 / FALSE未挂起
 */
uint8 UDS_APP_IsP2StarPending(void)
{
    return (g_udsConfiguration.iP2StarPending);
}

static void UDS_APP_PerformClearDtcNvm(uint8 state);
static void UDS_APP_FlushFaultNvmAfterTx(uint8 state);
#ifdef UDS_PROJECT_FOR_BOOTLOADER
static void UDS_APP_PerformAuthenticity(uint8 state);
static void UDS_APP_PerformDataIntegrity(uint8 state);
static void UDS_APP_SendRoutineCtrlResult(uint8 ridH, uint8 ridL, uint8 status, void (*pfCb)(uint8));
#endif

/**
 * @brief 0x14 清除DTC信息服务处理函数
 * @param pLocalSrv 服务配置指针
 * @param pLocalMsg 报文缓存指针
 * @desc 清除ECU中存储的DTC故障码信息
 *       只支持 0xFFFFFF（所有组别）
 */
static void UDS_APP_ClearDTCInformation(struct UDS_APP_ServiceInfoType *pLocalSrv, 
                                         UDS_APP_tLocalAppMsgType *pLocalMsg)
{
    uint32_t dtc_group = 0u;
    
    ASSERT(NULL_PTR == pLocalMsg);
    ASSERT(NULL_PTR == pLocalSrv);
    
    UDSCFGDebugLog("0x14: Clear DTC Information\n");
    
    // 1. 检查消息长度（至少4字节：SID + 3字节DTC组别）
    if (pLocalMsg->xDataMsgLength < 4u)
    {
        UDSCFGDebugLog("0x14: Invalid length %d, expected >= 4\n", pLocalMsg->xDataMsgLength);
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pLocalMsg);
        return;
    }
    
    // 2. 提取DTC组别（3字节）
    dtc_group = (uint32_t)pLocalMsg->aDataBuf[1u] << 16u |
                (uint32_t)pLocalMsg->aDataBuf[2u] << 8u  |
                (uint32_t)pLocalMsg->aDataBuf[3u];
    
    UDSCFGDebugLog("0x14: DTC Group = 0x%06X\n", dtc_group);
    
    // 3. 只支持 0xFFFFFF（所有组别）
    if (dtc_group != 0xFFFFFFu)
    {
        UDSCFGDebugLog("0x14: Unsupported DTC group 0x%06X (only 0xFFFFFF supported)\n", dtc_group);
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_REQUEST_OUT_OF_RANGE, pLocalMsg);
        return;
    }
    
    // 4. 只清 RAM 中的 DTC，F1ED 等保留；先回 78，发送完成后再写 DFlash 并回 54
    FaultInfo_ClearAllDTC();
    UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_SERVICE_BUSY, pLocalMsg);
    pLocalMsg->pfTxMsgCb = &UDS_APP_PerformClearDtcNvm;
    UDS_APP_StartP2StarServerTimer();
    UDSCFGDebugLog("0x14: DTC RAM cleared, NRC 0x78 then NVM\n");
}

static void UDS_APP_PerformClearDtcNvm(uint8 state)
{
    uint8 aBuf[8u] = {0u};

    if (E_TX_MSG_OK != state) {
        return;
    }
    FaultInfo_Flush();
    UDS_APP_StopP2StarServerTimer();
    aBuf[0u] = 0x54u;
    (void)TP_DataTransferQueueFrame(TP_GetTransportTxID(), NULL_PTR, 1u, aBuf);
    UDSCFGDebugLog("0x14: All DTCs cleared, 0x54 sent\n");
}

static void UDS_APP_FlushFaultNvmAfterTx(uint8 state)
{
    if (E_TX_MSG_OK == state) {
        FaultInfo_Flush();
        Did_Flush();
    }
}

// 0x22服务
static void UDS_APP_ReadDataByIdentifier(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg)
{
	uint16_t did;                           // 数据标识符
	uint8_t ucLoca_did_h;
	uint8_t ucLoca_did_l;
    uint8_t nrc = 0x00;                     // 否定响应码
    int ret;

	ASSERT(NULL_PTR == pLocalMsg);
    ASSERT(NULL_PTR == pLocalSrv);

	if (pLocalMsg->xDataMsgLength < 3u) 
	{
		UDSCFGDebugLog("0x22 error len , NRC 0x13\r\n");
        // 消息长度错误 → NRC 0x13
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pLocalMsg);
        return;
    }
	// 提取 DID（数据标识符）
	did = (uint16_t)((pLocalMsg->aDataBuf[1] << 8) | pLocalMsg->aDataBuf[2]);
	ucLoca_did_h = pLocalMsg->aDataBuf[1];
	ucLoca_did_l = pLocalMsg->aDataBuf[2];
	
	pLocalMsg->aDataBuf[0u] = pLocalSrv->serviceId + 0x40u;
	pLocalMsg->aDataBuf[1u] = ucLoca_did_h;
	pLocalMsg->aDataBuf[2u] = ucLoca_did_l;

    UDSCFGDebugLog("0x22 read 0x%04X\r\n",did);

	ret = Did_Read(did,&pLocalMsg->aDataBuf[3],&pLocalMsg->xDataMsgLength);
	if (ret != 0) {
        // DID不支持 → NRC 0x31 (RequestOutOfRange)
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_REQUEST_OUT_OF_RANGE, pLocalMsg);
        return;
    }
	pLocalMsg->xDataMsgLength = pLocalMsg->xDataMsgLength+3;
	UDSCFGDebugLog("0x22 read 0x%04X OK\r\n",did);
//	for(uint8_t i=0;i<13; i++)
//	{
//		UDSCFGDebugLog("0x%x ",pLocalMsg->aDataBuf[i]);
//	}
	
}
/**
 * @brief 0x85 DTC故障码开关控制服务处理
 * @param pLocalSrv 服务配置指针
 * @param pLocalMsg 报文缓存指针
 */
static void UDS_APP_CtrlDtcSetting(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg)
{
    uint8 ucLocalSubFunc = 0u;
    ASSERT(NULL_PTR == pLocalMsg);
    ASSERT(NULL_PTR == pLocalSrv);
	
    ucLocalSubFunc = pLocalMsg->aDataBuf[1u]; // 提取子功能01/02

    if((0x01u == ucLocalSubFunc))// 开启故障码检测
    {
        FaultInfo_SetDtcSetting(1u);
        pLocalMsg->aDataBuf[0u] = pLocalSrv->serviceId + 0x40u;
        pLocalMsg->aDataBuf[1u] = ucLocalSubFunc;
        pLocalMsg->xDataMsgLength = 2u;
        UDSCFGDebugLog("\r\n0x85 0x01 opend DTC\r\n");
    }
    else if (0x02u == ucLocalSubFunc)// 关闭故障码检测
    {
        FaultInfo_SetDtcSetting(0u);
        pLocalMsg->aDataBuf[0u] = pLocalSrv->serviceId + 0x40u;
        pLocalMsg->aDataBuf[1u] = ucLocalSubFunc;
        pLocalMsg->xDataMsgLength = 2u;
        UDSCFGDebugLog("\r\n0x85 0x02 close DTC\r\n");
    }
    // bit7置1抑制响应
    else if ((0x81u == ucLocalSubFunc) || (0x82u == ucLocalSubFunc))
    {
        FaultInfo_SetDtcSetting((0x81u == ucLocalSubFunc) ? 1u : 0u);
        pLocalMsg->xDataMsgLength = 0u;
    }
    // 非法子功能返回负响应
    else
    {
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pLocalMsg);
    }
}

/**
 * @brief 0x28 通信控制服务处理（收发开关）
 * @param pLocalSrv 服务配置
 * @param pLocalMsg 报文缓存
 */
static void UDS_APP_CommunicationSetting(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg)
{
    uint8 ucLocalCmd = 0u;
    ASSERT(NULL_PTR == pLocalMsg);
    ASSERT(NULL_PTR == pLocalSrv);

    ucLocalCmd = pLocalMsg->aDataBuf[1u]; // 通信控制子功能

    if (0x00u == ucLocalCmd)//00 允许收发
    {
        pLocalMsg->aDataBuf[0u] = pLocalSrv->serviceId + 0x40u;
        pLocalMsg->aDataBuf[1u] = ucLocalCmd;
        pLocalMsg->xDataMsgLength = 2u;
        UDSCFGDebugLog("\r\n0x28 0x00 enable TX\r\n");
    }
    else if (0x03u == ucLocalCmd)//03 关闭收发
    {
        pLocalMsg->aDataBuf[0u] = pLocalSrv->serviceId + 0x40u;
        pLocalMsg->aDataBuf[1u] = ucLocalCmd;
        pLocalMsg->xDataMsgLength = 2u;
        UDSCFGDebugLog("\r\n0x28 0x03 disable TX\r\n");
    }
    // 0x80/0x83 抑制正响应
    else if ((0x80u == ucLocalCmd) || (0x83u == ucLocalCmd))
    {
        pLocalMsg->aDataBuf[0u] = 0u;
        pLocalMsg->xDataMsgLength = 0u;
    }
    // 其余子功能不支持
    else
    {
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pLocalMsg);
    }
}

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/**
 * @brief 0x27 安全访问服务（种子密钥）
 * @param pSrv 服务配置指针
 * @param pMsg 报文缓存指针
 */
static void UDS_APP_SecAccessFunc(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 ucLocalSubFunc = 0u;
    static uint8 s_aLocalSeed[SA_ALGORITHM_SEED_SIZE] = {0u}; // 缓存随机种子
    boolean bLocalRst = FALSE;
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);

    ucLocalSubFunc = pMsg->aDataBuf[1u];

    // 子功能01 请求种子
    if (0x01u == ucLocalSubFunc || 0x11u == ucLocalSubFunc)
    {
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
        // 调用加密HAL生成随机种子
        bLocalRst = UDS_ALG_HAL_MyReplacedFunc(SA_ALGORITHM_SEED_SIZE, s_aLocalSeed);
        if (FALSE == bLocalRst)
        {
            // 种子生成失败，返回密钥无效
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_KEY, pMsg);
        }
        else
        {
            // 种子拷贝至响应报文
            UDS_APP_DuplicateMemory(s_aLocalSeed, SA_ALGORITHM_SEED_SIZE, &pMsg->aDataBuf[2u]);
            pMsg->xDataMsgLength = 2u + SA_ALGORITHM_SEED_SIZE;
            UDSCFGDebugLog("\r\n0x27 0x%x create ok seed\r\n",ucLocalSubFunc);
        }
    }
    // 子功能02 发送密钥校验解锁
    else if (0x02u == ucLocalSubFunc || 0x12u == ucLocalSubFunc)
    {
        // 解密比对传入密钥与种子是否匹配
        if (FALSE == UDS_APP_ValidateKey(&pMsg->aDataBuf[2u], s_aLocalSeed, SA_ALGORITHM_SEED_SIZE))
        {
            /* 密钥与种子不匹配：未通过，并清掉已解锁等级 */
            s_secAccessPassed = FALSE;
            UDS_APP_AssignSecurityLevel(NO_SECURITY);
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_KEY, pMsg);
            UDSCFGDebugLog("0x27 key mismatch, sec access not passed\r\n");
        }
        else
        {
            // 校验通过，解锁一级安全等级
            pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
            pMsg->xDataMsgLength = 2u;
            UDS_APP_FillMemory(0x1u, sizeof(s_aLocalSeed), s_aLocalSeed); // 清空种子缓存
            s_secAccessPassed = TRUE;
            UDS_APP_AssignSecurityLevel(SECURITY_LEVEL_1);
        }
    }
    else
    {
        /* 非法子功能无处理 */
    }
}

/**
 * @brief 0x2E 写入DID，主要用于写入工具序列号
 * @param pSrv 服务配置
 * @param pMsg 收发报文缓存
 */
static void UDS_APP_WriteDataIdent(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);

    /* VIN / F1EF：Boot 不支持 2E 写入 */
    if ((pMsg->xDataMsgLength >= 3u) &&
        (0xF1u == pMsg->aDataBuf[1u]) &&
        ((0x90u == pMsg->aDataBuf[2u]) || (0xEFu == pMsg->aDataBuf[2u])))
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_OUT_OF_RANGE, pMsg);
        return;
    }

    // 判断是否为指纹专属DID
    if (TRUE == UDS_APP_ValidateWriteFingerprint(pMsg))
    {
         // 读取指纹数据并写入非易失存储
        RecordFingerPrint(&pMsg->aDataBuf[3u], (pMsg->xDataMsgLength - 3u));
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
        pMsg->aDataBuf[1u] = 0xF1u;
        pMsg->aDataBuf[2u] = 0x5Au;
        pMsg->xDataMsgLength = 3u;  
        
        return;
    }
    
    // 判断是否为写入工具序列号 0xF198
    if (TRUE == UDS_APP_CheckWriteToolSerial(pMsg))
    {
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        if (TRUE == UDS_APP_RejectIfProgVersionLow(pSrv, pMsg))
        {
            return;
        }
#endif
        // 工具序列号从 aDataBuf[3] 开始，16字节 ASCII
        uint8_t data_len = pMsg->xDataMsgLength - 3u;
        
        // 校验长度：必须是16字节
        if (data_len != 16u) {
            UDSCFGDebugLog("0x2E 0xF198: Invalid length %d, expected 16\n", data_len);
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
            return;
        }
        
        // 调用 FlashInfo 写入工具序列号
        int ret = Did_Write(0xF198, &pMsg->aDataBuf[3u], data_len);
        if (ret != 0) {
            UDSCFGDebugLog("0x2E 0xF198: Write failed, ret=%d\n", ret);
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
            return;
        }
        
        // 肯定响应：6E F1 98
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;  // 0x6E
        pMsg->aDataBuf[1u] = 0xF1u;
        pMsg->aDataBuf[2u] = 0x98u;
        pMsg->xDataMsgLength = 3u;
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        s_toolSerialPassed = TRUE;
#endif
        
        UDSCFGDebugLog("0x2E: Write Tool Serial success\n");
        return;
    }
    // 3. 判断是否为写入编程日期 0xF199
    // ================================================================
    if (TRUE == UDS_APP_CheckWriteReprogDate(pMsg))
    {
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        if (TRUE == UDS_APP_RejectIfProgVersionLow(pSrv, pMsg))
        {
            return;
        }
#endif
        // 编程日期从 aDataBuf[3] 开始，4字节 BCD
        uint8_t data_len = pMsg->xDataMsgLength - 3u;
        
        // 校验长度：必须是4字节
        if (data_len != 4u) {
            UDSCFGDebugLog("0x2E 0xF199: Invalid length %d, expected 4\n", data_len);
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
            return;
        }
        
        //调用 FlashInfo 写入编程日期
        int ret = Did_Write(0xF199, &pMsg->aDataBuf[3u], data_len);
        if (ret != 0) {
            UDSCFGDebugLog("0x2E 0xF199: Write failed, ret=%d\n", ret);
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
            return;
        }
        
        // 肯定响应：6E F1 99
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;  // 0x6E
        pMsg->aDataBuf[1u] = 0xF1u;
        pMsg->aDataBuf[2u] = 0x99u;
        pMsg->xDataMsgLength = 3u;
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        s_reprogDatePassed = TRUE;
#endif
        
        UDSCFGDebugLog("0x2E: Write Reprogramming Date success\n");
        return;
    }

    // 4. 不支持的DID
    UDSCFGDebugLog("0x2E: Unsupported DID\n");

    UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pMsg);
}

/* 全局下载信息结构体：存储起始地址、程序总长度 */
UDS_APP_downloadDataStructType gs_stDowloadDataInfo = {0u, 0u};
static uint8 gs_RxBlockNum = 0u; // 当前接收数据包块号

/**
 * @brief 0x34 请求下载，解析升级地址与长度，校验合法性
 * @param pSrv 服务配置
 * @param pMsg 报文缓存
 */
static void UDS_APP_ReqDownload(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);
    uint8 ucIdx = 0u;
    uint8 ucRetFlag = TRUE;

#ifdef UDS_PROJECT_FOR_BOOTLOADER
    if (TRUE == UDS_APP_RejectIfProgVersionLow(pSrv, pMsg))
    {
        return;
    }
#endif

    // 校验报文最小长度 0x34 0x00 0x44 +4个地址数据 +4地址长度
    if (pMsg->xDataMsgLength >= (DOWNLOAD_ADDR_SIZE + DOWNLOAD_DATA_SIZE + 1u + 2u))
    {
    }
    else
    {
        ucRetFlag = FALSE;
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
    }

    if (TRUE == ucRetFlag)
    {
        // 大端转32位Flash起始地址
        gs_stDowloadDataInfo.uStartAddress = 0u;
        ucIdx = 0u;
        while (ucIdx < DOWNLOAD_ADDR_SIZE)  //地址4个字节长度 左移成32位
        {
            gs_stDowloadDataInfo.uStartAddress <<= 8u;
            gs_stDowloadDataInfo.uStartAddress |= pMsg->aDataBuf[ucIdx + 3u];
            ucIdx++;
        }
        // 大端转程序总长度
        gs_stDowloadDataInfo.uDataLength = 0u;
        ucIdx = 0u;
        while (ucIdx < DOWNLOAD_DATA_SIZE)
        {
            gs_stDowloadDataInfo.uDataLength <<= 8u;
            gs_stDowloadDataInfo.uDataLength |= pMsg->aDataBuf[ucIdx + 7u];
            ucIdx++;
        }
    }

    /* 起始地址、整段长度须落在同一白名单窗口，否则 NRC31 */
    if (((TRUE != UDS_APP_CheckDownloadAddress(gs_stDowloadDataInfo.uStartAddress)) ||
            (TRUE != UDS_APP_CheckDownloadLength(gs_stDowloadDataInfo.uDataLength))) && (TRUE == ucRetFlag))
    {
        UDSCFGDebugLog("0x34 CheckDownloadAddress,false\r\n");
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_OUT_OF_RANGE, pMsg);
        ucRetFlag = FALSE;
    }

    if (TRUE != ucRetFlag)
    {
        FlashDebugLog("\n ReqDownload false\n");
        PrepareDLInformation(); // 重置下载状态
        SetNextDLPara(FLASH_DOWNLOAD_REQUEST);
    }
    else
    {
        UDSCFGDebugLog("0x34 inter FLASH_DOWNLOAD_TRANSFER,\r\n");
        SetNextDLPara(FLASH_DOWNLOAD_TRANSFER); // 进入数据传输阶段
        RecordDLInformation(gs_stDowloadDataInfo.uStartAddress, gs_stDowloadDataInfo.uDataLength);
        // 回复块大小、STmin
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
        pMsg->aDataBuf[1u] = 0x20u;
        pMsg->aDataBuf[2u] = (uint8)((DOWNLOAD_MAX_BLOCK_LEN >> 8u) & 0xFFu);
        pMsg->aDataBuf[3u] = (uint8)(DOWNLOAD_MAX_BLOCK_LEN & 0xFFu);
        pMsg->xDataMsgLength = 4u;
        gs_RxBlockNum = 1u; // 初始块号从1开始
    }
}

/**
 * @brief 0x36 数据传输，单块Flash写入、块号校验
 * @param pSrv 服务配置
 * @param pMsg 报文缓存
 */
static void UDS_APP_TransferProcess(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 bFlag = TRUE;
    uint8 uPrevBlockNum = 0u;
    uint8 nrc = 0x00;  // 新增：记录错误码
	static uint8_t resele=0;
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);
#ifdef UDS_PROJECT_FOR_BOOTLOADER
    if (TRUE == UDS_APP_RejectIfProgVersionLow(pSrv, pMsg))
    {
        return;
    }
#endif
		
	if(resele==0){resele=1;FlashDebugLog("0x36 inter\r\n");}	
	
	
    uBlockNum = pMsg->aDataBuf[1u]; // 当前收到块号
    uPrevBlockNum = (uint8)(gs_RxBlockNum - 1u);  //gs_RxBlockNum=我想要的块 的块号

    // 步骤1：校验当前是否处于传输状态
    if ((FLASH_DOWNLOAD_TRANSFER == GainCurDLPara()) || (TRUE != bFlag))
    {
    }
    else
    {
        bFlag = FALSE;
        nrc = E_NRC_REQUEST_SEQUENCE_ERROR;
        //UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_SEQUENCE_ERROR, pMsg);
    }

#ifdef UDS_PROJECT_FOR_BOOTLOADER
    /* 超过 0x34 报的最大块长：NRC13，不写、不复位，传输继续 */
    if ((TRUE == bFlag) && (pMsg->xDataMsgLength > DOWNLOAD_MAX_BLOCK_LEN))
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
        FlashDebugLog("0x36 NRC13 len %u > 0x%x, keep transfer\r\n",
                      (uint32)pMsg->xDataMsgLength, DOWNLOAD_MAX_BLOCK_LEN);
        return;
    }
#endif

    // 步骤2：校验块号连续性
    if ((gs_RxBlockNum == uBlockNum) || (TRUE != bFlag)) // gs_RxBlockNum=我想要的块的块号=uBlockNum当前进来的块号,块号正确什么都不执行
    {
    }
    else if (uPrevBlockNum == uBlockNum)
    {
        /* 与上一块号相同：NRC73，不写 Flash、不复位下载，仍等待期望块 */
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_WRONG_BLOCK_SEQUENCE_COUNTER, pMsg);
        FlashDebugLog("0x36 NRC73 duplicate block %u, keep transfer\r\n", uBlockNum);
        return;
    }
    else
    {
        bFlag = FALSE;
        nrc = E_NRC_WRONG_BLOCK_SEQUENCE_COUNTER;
        FlashDebugLog("0x36 download fail block cross \r\n");
    }

    // 步骤3：状态正常则写入Flash
    if (TRUE == bFlag)
    {
        if (TRUE == ProcessProgramRegion(gs_stDowloadDataInfo.uStartAddress,
                                         &pMsg->aDataBuf[2u],
                                         (pMsg->xDataMsgLength - 2u)))
        {
            gs_RxBlockNum++; // 下一块期待序号
        }
        else
        {
            bFlag = FALSE;
            nrc = E_NRC_CONDITIONS_NOT_CORRECT;
            FlashDebugLog("download fail the %d num\r\n",gs_RxBlockNum);
            //UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
        }
        // 更新写入地址与剩余长度
        if (TRUE == bFlag)
        {
            gs_stDowloadDataInfo.uStartAddress += (pMsg->xDataMsgLength - 2u);
            gs_stDowloadDataInfo.uDataLength -= (pMsg->xDataMsgLength - 2u);
        }
    }

    // 步骤4：判断全部数据是否接收完毕
    if ((0u != gs_stDowloadDataInfo.uDataLength) || (TRUE != bFlag))
    {
    }
    else
    {
        gs_RxBlockNum = 0u;
        SetNextDLPara(FLASH_DOWNLOAD_EXIT_TRANSFER); // 进入校验阶段
        FlashDebugLog("0x36 DOWNLOAD finish \r\n");
    }

    // 步骤5：正常返回块号，失败重置下载流程  发送响应（正响应 或 负响应)  
    if (TRUE != bFlag)
    {
        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_DOWNLOAD_FAIL;
        UDS_APP_AssignNegErrCode(pSrv->serviceId, nrc, pMsg);
        PrepareDLInformation();
        SetNextDLPara(FLASH_DOWNLOAD_REQUEST);
        gs_RxBlockNum = 0u;
        pMsg->pfTxMsgCb = &UDS_APP_FlushFaultNvmAfterTx;
        FlashDebugLog("0x36 download TRANSFER fail \r\n");
    }
    else
    {
        // 正响应（0x76 + 块号）
        if(FALSE == flashDLStatusSingle.VerifyFDDownloadedFlag)
        {
            pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
            pMsg->aDataBuf[1u] = uBlockNum;
            pMsg->xDataMsgLength = 2u;
        }
        else
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SERVICE_BUSY, pMsg);
            UDS_APP_StartP2StarServerTimer();
        }
    }
}

/**
 * @brief 0x37 退出传输，触发APP CRC校验
 * @param pSrv 服务配置
 * @param pMsg 报文缓存
 */
static void UDS_APP_FinalizeTransfer(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 bResult = TRUE;
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);
#ifdef UDS_PROJECT_FOR_BOOTLOADER
    if (TRUE == UDS_APP_RejectIfProgVersionLow(pSrv, pMsg))
    {
        return;
    }
#endif

    // 校验当前阶段是否为传输完成
    if (FLASH_DOWNLOAD_EXIT_TRANSFER == GainCurDLPara())
    {
        FlashDebugLog("0x37 EXIT_TRANSFER In crc\r\n");
        if (TRUE != FinalizeAppDownloadTransfer())
        {
            FaultInfo_GetPtr()->f1ed |= F1ED_BIT_DOWNLOAD_FAIL;
            bResult = FALSE;
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
            pMsg->pfTxMsgCb = &UDS_APP_FlushFaultNvmAfterTx;
        }
    }
    else
    {
        FlashDebugLog("0x37 EXIT_TRANSFER in crc fail\r\n");
        bResult = FALSE;
        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_DOWNLOAD_FAIL;
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_SEQUENCE_ERROR, pMsg);
        pMsg->pfTxMsgCb = &UDS_APP_FlushFaultNvmAfterTx;
    }

    if (TRUE != bResult)
    {
        PrepareDLInformation(); // 重置下载信息
        FlashDebugLog("0x37 EXIT_TRANSFER reset\r\n");
        return;
    }
    else
    {
        SetNextDLPara(FLASH_DOWNLOAD_CHECKSUM); // 进入校验步骤
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
        pMsg->xDataMsgLength = 1u;
        FlashDebugLog("0x37 EXIT_TRANSFER in crc finish\r\n");
    }
}

#ifdef APP_BICLE_UPDATA
/**
 * @brief 获取APP擦除起始地址（双分区升级）
 * @return Flash擦除首地址
 */
uint32_t UDS_APP_RetrieveEraseStartAddress(void)
{
    return gs_eraseStartAddress;
}
#endif

/**
 * @brief 0x31 例程控制：擦除、备份、CRC校验、版本校验
 * @param pSrv 服务配置
 * @param pMsg 报文缓存
 */
static void UDS_APP_HandleRoutineOperation(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 bFlag = FALSE;
    uint32 ulCrcVal = 0u;
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);
    RestartS3Server(); // 刷新会话计时

    // 判断是否为擦除内存例程 0xFF00
    if (TRUE == UDS_APP_CheckErasingMemory(pMsg))
    {
        uint32 eraseStart = 0u;
        uint32 eraseLen = 0u;
        uint8 eraseNrc;

#ifdef UDS_PROJECT_FOR_BOOTLOADER
        if (TRUE == UDS_APP_RejectIfProgVersionLow(pSrv, pMsg))
        {
            return;
        }
        /* 0202 失败或未做：擦除入口直接 NRC22，不进 78/擦除 */
        if (TRUE == UDS_APP_RejectIfFlashDriverCrcFail(pSrv, pMsg))
        {
            return;
        }
#endif

        UDSCFGDebugLog("inter 0x31 0xFF00 ErasingMemory\r\n");
        eraseNrc = UDS_APP_ParseAndCheckEraseRange(pMsg, &eraseStart, &eraseLen);
        if (0u != eraseNrc)
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, eraseNrc, pMsg);
            return;
        }
        RecordEraseMemoryRange(eraseStart, eraseLen);
        FaultInfo_ResetF1ED(); /* 擦除前清 F1ED，周期 RAM 标志已在 10 02 清 */
        FaultInfo_IncProgramAttempt();
        UDSCFGDebugLog("0x31 FF00 erase 0x%x len 0x%x\r\n", eraseStart, eraseLen);
#ifdef APP_BICLE_UPDATA
        gs_eraseStartAddress = eraseStart;
#endif
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SERVICE_BUSY, pMsg);
        pMsg->pfTxMsgCb = &UDS_APP_PerformFlashErase;
        UDS_APP_StartP2StarServerTimer();
        return;
    }

#ifdef APP_SINGLE_UPDATA
    if (TRUE == UDS_APP_CheckErasingBackupMemory(pMsg))
    {
        //UDSCFGDebugLog("inter 0x31 0xFF01\r\n");
        // 擦除备份分区例程
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SERVICE_BUSY, pMsg);
        pMsg->pfTxMsgCb = &UDS_APP_PerformBackupFlashErase;
        UDS_APP_StartP2StarServerTimer();
        return;
    }
#endif

    //3. 判断是否为校验文件合法性例程 0x6000
    if (TRUE == UDS_APP_CheckAuthenticity(pMsg))
    {
        UDSCFGDebugLog("inter 0x31 0x6000\r\n");
        if (pMsg->xDataMsgLength != 1326U) {
            UDSCFGDebugLog("singinfo len != 1326 =%d\r\n", pMsg->xDataMsgLength);
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
            return;
        }
        Momory_Copy_Function(s_aAuthParam, &pMsg->aDataBuf[4u], UDS_AUTH_PARAM_LEN);
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SERVICE_BUSY, pMsg);
        pMsg->pfTxMsgCb = &UDS_APP_PerformAuthenticity;
        UDS_APP_StartP2StarServerTimer();
        return;
    }

    //4. 判断是否为版本校验例程 0x6001
    if (TRUE == UDS_APP_CheckVersion(pMsg))
    {
        UDSCFGDebugLog("inter 0x31 0x6001\r\n");
        uint8_t compare_result = UDS_APP_CompareVersion();
        
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;  // 0x71
        pMsg->aDataBuf[1u] = 0x01u;
        pMsg->aDataBuf[2u] = 0x60u;
        pMsg->aDataBuf[3u] = 0x01u;
        if (compare_result == 0) {
#ifdef UDS_PROJECT_FOR_BOOTLOADER
            s_versionCheckPassed = TRUE;
#endif
            pMsg->aDataBuf[4u] = 0x04u;  // 检查成功
        } else {
#ifdef UDS_PROJECT_FOR_BOOTLOADER
            s_versionCheckPassed = FALSE;
#endif
            pMsg->aDataBuf[4u] = 0x05u;  // 版本过低
        }
        pMsg->xDataMsgLength = 5u;
        return;
    }

    //5. 判断是否为下载数据完整性检查例程 0x0203
    if (TRUE == UDS_APP_CheckDataIntegrity(pMsg))
    {
        UDSCFGDebugLog("inter 0x31 0x0203\r\n");
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SERVICE_BUSY, pMsg);
        pMsg->pfTxMsgCb = &UDS_APP_PerformDataIntegrity;
        UDS_APP_StartP2StarServerTimer();
        return;
    }

    // 6.判断是否为CRC校验例程 0x0202 ,检查和激活次级引导加载程序
    if (TRUE == UDS_APP_CheckSummationRoutine(pMsg))
    {
        // 读取下发CRC校验值
        ulCrcVal = pMsg->aDataBuf[4u];
        ulCrcVal = (ulCrcVal << 8u) | pMsg->aDataBuf[5u];
//#ifdef USE_ECU_BUS_PRO
        ulCrcVal = (ulCrcVal << 8u) | pMsg->aDataBuf[6u];
        ulCrcVal = (ulCrcVal << 8u) | pMsg->aDataBuf[7u];
//#endif
        RecordRecCrcValue(ulCrcVal);
        UDSCFGDebugLog("inter 0x31 0x0202 crcValue:0x%x\r\n",ulCrcVal);
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SERVICE_BUSY, pMsg);
        pMsg->pfTxMsgCb = &UDS_APP_PerformChecksum;
        UDS_APP_StartP2StarServerTimer();
        return;
    }

    //判断是否为依赖性检查例程 0xFF01
    if (TRUE == UDS_APP_CheckProgramDependency(pMsg)) 
    {
        UDSCFGDebugLog("inter 0x31 0xFF01\r\n");
        // 软硬件依赖校验
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;  // 0x71
        pMsg->aDataBuf[1u] = 0x01u;
        pMsg->aDataBuf[2u] = 0xFFu;
        pMsg->aDataBuf[3u] = 0x01u;

        /* 过渡刷 Boot：不卡 0203；擦写成功后再作废 APP 头 */
        if (TRUE != FlashAppRationality())
        {
            UDSCFGDebugLog("0xFF01 boot program flags fail\r\n");
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
            return;
        }
        if (TRUE != InvalidateAppHeaderAfterBootOk())
        {
            UDSCFGDebugLog("0xFF01 invalidate app header fail\r\n");
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
            return;
        }
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
        pMsg->xDataMsgLength = 4u;
        s_programCycleSucceeded = TRUE;
        return;
    }

#ifdef APP_SINGLE_UPDATA
    if (TRUE != UDS_APP_CheckCopyBackupMemory(pMsg))
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pMsg);
    }
    else
    {
        // 拷贝主程序到备份分区
        bFlag = AppBackupInFls();
        if (TRUE == bFlag)
        {
            bFlag = VerifyAppBackupResult();
        }
        if (TRUE == bFlag)
        {
            pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
            pMsg->xDataMsgLength = 4u;
        }
        else
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pMsg);
        }
    }
    return;
#endif

    UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pMsg);
        
}

/**
 * @brief 0x11 ECU复位处理函数，复位前保存升级标志
 * @param pstSrv 服务配置
 * @param pstMsg 报文缓存
 */
static void UDS_APP_PerformEcuReset(struct UDS_APP_ServiceInfoType *pstSrv, UDS_APP_tLocalAppMsgType *pstMsg)
{
    ASSERT(NULL_PTR == pstMsg);
    ASSERT(NULL_PTR == pstSrv);
	UDSCFGDebugLog("0x11 clear ram flag, cycle=%d no 0xA5\r\n", s_programCycleSucceeded);
    RamFDErase(); // 擦除RAM临时升级标记
    /* 过渡刷 Boot 成功后复位进新 Boot，不置 APP 的 0xA5 */
    pstMsg->pfTxMsgCb = &UDS_APP_PerformMcuReset; // 发送完成执行复位
    UDS_APP_AssignNegErrCode(pstSrv->serviceId, E_NRC_SERVICE_BUSY, pstMsg);
    UDS_APP_StartP2StarServerTimer();
}
#endif

/**
 * @brief 0x3E Tester Present诊断仪在线保活
 * @param pSrv 服务配置
 * @param pMsg 报文缓存
 */
static void UDS_APP_HandleTesterPresence(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 bSubFunc = 0u;
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);

    bSubFunc = pMsg->aDataBuf[1u];
    // 子功能00：正常在线响应
    if (bSubFunc != 0x00u)
    {
        if (bSubFunc != 0x80u)
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pMsg);
        }
        else
        {
            pMsg->xDataMsgLength = 0u; // bit7抑制响应
        }
    }
    else
    {
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
        pMsg->aDataBuf[1u] = bSubFunc;
        pMsg->xDataMsgLength = 2u;
    }
}

/**
 * @brief 复位MCU底层函数，操作寄存器触发系统复位
 * @param uStatus 报文发送完成状态
 */
static void UDS_APP_PerformMcuReset(uint8 uStatus)
{
    if (E_TX_MSG_OK != uStatus)
    {
    }
    else
    {
        // 操作极海G32A1085 RCC外设寄存器开启软件复位
        *((uint32_t*)0x40021024) = 1 << 24;
        WATCHDOG_HAL_SystemReset(); // 看门狗复位兜底
        while (1)
        {
            /* 死循环等待复位生效 */
        }
    }
}

/**
 * @brief 获取全局UDS服务注册表入口与服务总数
 * @param pServiceItem 出参：返回服务条目数量
 * @return 服务配置数组首地址指针
 */
UDS_APP_localServiceType *UDS_APP_ObtainUdsSrvConfig(uint8 *pServiceItem)
{
    ASSERT(NULL_PTR == pServiceItem);
    *pServiceItem = sizeof(gs_astUDSService) / sizeof(gs_astUDSService[0u]);
    return (UDS_APP_localServiceType *) &gs_astUDSService[0u];
}

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/**
 * @brief 标记是否收到诊断完整报文（延时下线功能）
 * @param bSetVal 布尔标记
 */
void UDS_APP_MarkRxUdsMessage(const boolean bSetVal)
{
#ifdef ALLOW_DELAY_TIME
    gs_structForTimeInformation.isReceivedMsg = bSetVal ? TRUE : FALSE;
#endif
}

/**
 * @brief 查询当前是否收到诊断报文
 * @return TRUE收到 / FALSE无
 */
boolean UDS_APP_HasRxUdsMessage(void)
{
#ifdef ALLOW_DELAY_TIME
    return gs_structForTimeInformation.isReceivedMsg;
#else
    return TRUE;
#endif
}
#endif

/**
 * @brief 填充UDS标准负响应报文
 * @param uServNum 当前服务SID
 * @param uErrCode 标准NRC错误码
 * @param pMsg 报文缓存指针
 */
void UDS_APP_AssignNegErrCode(const uint8 uServNum,
                              const uint8 uErrCode,
                              UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    pMsg->aDataBuf[0u] = NEGATIVE_RSP_ID; // 负响应固定0x7F
    pMsg->aDataBuf[1u] = uServNum;        // 对应服务号
    pMsg->aDataBuf[2u] = uErrCode;        // 错误码
    pMsg->xDataMsgLength = 3u;
}

/**
 * @brief 判断当前会话是否为默认基础会话
 * @return TRUE=默认会话 / FALSE=编程/扩展会话
 */
uint8 UDS_APP_CheckIfDefaultSession(void)
{
    return (BASIC_SESSION == g_udsConfiguration.iSessionMode) ? TRUE : FALSE;
}

uint8 UDS_APP_CheckIfProgramSession(void)
{
    return (PROGRAM_SESS == g_udsConfiguration.iSessionMode) ? TRUE : FALSE;
}

/**
 * @brief 判断S3会话超时计时器是否归零
 * @return TRUE超时 / FALSE未超时
 */
uint8 UDS_APP_CheckS3ServerIsTimeout(void)
{
    return (0u == g_udsConfiguration.iS3Time) ? TRUE : FALSE;
}

/**
 * @brief 校验当前会话是否支持对应服务
 * @param uSesMode 待校验会话掩码
 * @return 匹配返回TRUE
 */
uint8 UDS_APP_CheckSessionReq(uint8 uSesMode)
{
    return ((uSesMode & g_udsConfiguration.iSessionMode) == g_udsConfiguration.iSessionMode) ? TRUE : FALSE;
}

/**
 * @brief 存储当前接收报文寻址类型（物理/功能）
 * @param uReqID 底层TP层接收CAN ID
 */
void UDS_APP_StoreRxIdType(const uint32 uReqID)
{
    if (uReqID == TP_GetTransportRxPhyID())
    {
        MACRO_SET_REQUEST_ID(ALLOW_PHYSICAL_ID);
    }
    else if (uReqID == TP_GetTransportRxFunID())
    {
        MACRO_SET_REQUEST_ID(ALLOW_FUNCTION_ID);
    }
    else
    {
        MACRO_SET_REQUEST_ID(FAILURE_REQUEST_ID);
    }
}

/**
 * @brief 校验当前寻址模式是否匹配服务支持类型
 * @param uIdMode 服务允许寻址掩码
 * @return 匹配返回TRUE
 */
uint8 UDS_APP_CheckRxIdValidity(uint8 uIdMode)
{
    return ((uIdMode & g_udsConfiguration.iReqIdMode) == g_udsConfiguration.iReqIdMode) ? TRUE : FALSE;
}

/**
 * @brief 设置当前全局安全访问等级
 * @param uSecLevel 安全等级枚举值
 */
void UDS_APP_AssignSecurityLevel(const uint8 uSecLevel)
{
    g_udsConfiguration.iSecLevel = uSecLevel;
}

/**
 * @brief 校验当前安全等级是否满足服务要求
 * @param uSecLev 服务所需安全掩码
 * @return 满足返回TRUE
 */
uint8 UDS_APP_IsSecurityLevelValid(uint8 uSecLev)
{
    /* uSecLev：服务所需等级；iSecLevel：当前已解锁等级。当前必须覆盖所需。 */
    return ((g_udsConfiguration.iSecLevel & uSecLev) == uSecLev) ? TRUE : FALSE;
}

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/**
 * @brief 内存拷贝封装函数，底层调用memcpy
 * @param pSrc 源地址
 * @param uLen 拷贝长度
 * @param pDst 目标地址
 */
static void UDS_APP_DuplicateMemory(const void *pSrc, const uint8 uLen, void *pDst)
{
    ASSERT(NULL_PTR == pSrc);
    ASSERT(NULL_PTR == pDst);
    Momory_Copy_Function(pDst, pSrc, uLen);
}

/**
 * @brief 内存填充封装，底层memset
 * @param uVal 填充字节
 * @param uSize 填充长度
 * @param pBuf 目标缓存
 */
static void UDS_APP_FillMemory(const uint8 uVal, const uint16 uSize, void *pBuf)
{
    ASSERT(NULL_PTR == pBuf);
    Momory_Fill_Function(pBuf, uVal, uSize);
}

//Level-1 进入扩展会话用：
static uint32 SeedKeyEXTSupplier(uint32 seed)
{
    uint32 key;
    key = 0U;
    key = ((((seed >> 4U) ^ seed) << 3U) ^ seed);
    return key;
}
//Level-3 进入编程会话用：
static uint32 SeedKeyPROGSupplier (uint32 seed) 
{
    const uint32 key[4U] = {0x4fe87269U, 0x6bc361d8U,0x9b127d51U, 0x5ba41903U}; /* 128
    bits */
    uint32 y = ((seed<<24U)&0xff000000U) + ((seed<<8U)&0xff0000U) +
    ((seed>>8U)&0xff00U) + ((seed>>24U)&0xffU); /* swap byte order */
    uint32 z = 0U, sum = 0U; /* y = LOW_PART, z = HIGH_PART */
    uint8 n = 64U; /* number of iterations */
    while (n > 0U) { /* encrypt */
    y += (((z << 4U) ^ (z >> 5U)) + z) ^ (sum + key[sum & 3U]);
    sum += 0x8f750a1dU;
    z += (((y << 4U) ^ (y >> 5U)) + y) ^ (sum + key[(sum >> 11U) & 3U]);
    n--;
    }
    return ((z<<24U)&0xff000000U) + ((z<<8U)&0xff0000U) + ((z>>8U)&0xff00U) +
    ((z>>24U)&0xffU); /* swap byte order */
}
/**
 * @brief 安全密钥校验：解密收到的密钥并比对种子
 * @param pKeyIn 诊断下发密钥
 * @param pSeed 本地生成种子
 * @param keyLength 密钥长度
 * @return 匹配TRUE，不匹配FALSE
 */
#if 0
static uint8 UDS_APP_ValidateKey(const uint8 *pKeyIn,
                                const uint8 *pSeed,
                                const uint8 keyLength)
{
    uint8 bIdx = 0u;
    uint8 decrypted[SA_ALGORITHM_SEED_SIZE] = {0u};
    ASSERT(NULL_PTR == pKeyIn);
    ASSERT(NULL_PTR == pSeed);
    UDS_ALG_HAL_DataDecryptionMethod(pKeyIn, keyLength, decrypted);
    for (bIdx = 0u; bIdx < SA_ALGORITHM_SEED_SIZE; bIdx++)
    {
        if (decrypted[bIdx] == pSeed[bIdx])
        {
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}
#endif
//pKeyIn:上位机进来的  pSeed:本地生成的种子
static uint8 UDS_APP_ValidateKey(const uint8 *pKeyIn,
                                 const uint8 *pSeed,
                                 const uint8 keyLength)
{
    uint32 seed = 0U;
    uint32 local_key = 0U;
    uint32 remote_key = 0U;
    uint8 current_session;

    // 参数检查
    if ((NULL_PTR == pKeyIn) || (NULL_PTR == pSeed) || (keyLength != 4U)) {
        return FALSE;
    }

    // 获取当前会话
    current_session = g_udsConfiguration.iSessionMode;

    // 组装种子值（大端 → uint32）    
    seed = (uint32)pSeed[0] << 24U |
           (uint32)pSeed[1] << 16U |
           (uint32)pSeed[2] << 8U |
           (uint32)pSeed[3];

    // ★ 根据当前会话选择算法
    if (EXTENDED_SESS == current_session) {
        // Level 1：扩展会话（0x27 0x01/0x02）
        local_key = SeedKeyEXTSupplier(seed);
        UDSCFGDebugLog("Security: Level 1 (EXT), seed=0x%08X, local_key=0x%08X\r\n",
                       seed, local_key);
    } else if (PROGRAM_SESS == current_session) {
        // Level 3：编程会话（0x27 0x11/0x12）
        local_key = SeedKeyPROGSupplier(seed);
        UDSCFGDebugLog("Security: Level 3 (PROG), seed=0x%08X, local_key=0x%08X\r\n",
                       seed, local_key);
    } else {
        // 默认会话不支持安全访问
        UDSCFGDebugLog("Security: Invalid session 0x%02X for security access\r\n",
                       current_session);
        return FALSE;
    }

    // 组装上位机发来的密钥
    remote_key = (uint32)pKeyIn[0] << 24U |
                 (uint32)pKeyIn[1] << 16U |
                 (uint32)pKeyIn[2] << 8U |
                 (uint32)pKeyIn[3];

    UDSCFGDebugLog("Security: remote_key=0x%08X\r\n", remote_key);

    // ★ 比对密钥
    if (local_key == remote_key) {
        UDSCFGDebugLog("Security: Key match! Unlock success.\r\n");
        return TRUE;
    } else {
        UDSCFGDebugLog("Security: Key mismatch! Unlock failed.\r\n");
        return FALSE;
    }
}

// ================================================================
// 函数名   : UDS_APP_CompareVersion
// 功能     : 版本号比对，判断是否允许刷写
// 参数     : pRemoteVersion - 上位机发来的版本信息（10字节）
// 返回     : 0=版本有效（允许刷写），-1=版本过低（拒绝刷写）
// 说明     ：参照《软件下载规范》7.2.5节
//           ECU内部记录的版本号应和 DID 0xF189 的值一致
// ================================================================
int UDS_APP_CompareVersion()
{
    /* 过渡程序刷 Boot，不做 F189 版本门禁 */
    return 0;
}

static uint8 UDS_APP_ValidateRoutineCtrlId(const UDS_APP_checkRoutineCtrlType eRoutineCtrl,
                                           const UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 uCount = 0u;
    uint8 *pRtCtrlId = NULL_PTR;
    uint8 uIdx = 0u;
    ASSERT(NULL_PTR == pMsg);

    if (ERASE_MEMORY_CONTROL == eRoutineCtrl)
    {
        pRtCtrlId = (uint8 *)&gs_arrayEraseMemRoutineID[0u];
        uCount = (uint8)sizeof(gs_arrayEraseMemRoutineID);
    }
    else if (CHECKSUM_CONTROL == eRoutineCtrl)
    {
        pRtCtrlId = (uint8 *)&gs_arrayCksumRoutineID[0u];
        uCount = (uint8)sizeof(gs_arrayCksumRoutineID);
    }
    else if (DEPENDENCY_CONTROL == eRoutineCtrl)
    {
        pRtCtrlId = (uint8 *)&gs_arrayCheckProgranDepenID[0u];
        uCount = (uint8)sizeof(gs_arrayCheckProgranDepenID);
    }
    else if(CHECK_AUTHENTICITY == eRoutineCtrl) // ★ 新增：校验文件合法性 0x6000
    {
        pRtCtrlId = (uint8 *)&gs_arrayCheckAuthenticityRID[0u];
        uCount = (uint8)sizeof(gs_arrayCheckAuthenticityRID);
    }
    else if(CHECK_VERSION == eRoutineCtrl)// ★ 新增：版本校验 0x6001
    {
        pRtCtrlId = (uint8 *)&gs_arrayCheckVersionRID[0u];
        uCount = (uint8)sizeof(gs_arrayCheckVersionRID);
    }
    else if(CHECK_DATA_INTEGRITY == eRoutineCtrl)  // ★ 新增：下载数据完整性检查 0x0203
    {
        pRtCtrlId = (uint8 *)&gs_arrayCheckDataIntegrityRID[0u];
        uCount = (uint8)sizeof(gs_arrayCheckDataIntegrityRID);
    }

#ifdef APP_SINGLE_UPDATA
    else if (ERASE_BACKUP_CONTROL == eRoutineCtrl)
    {
        pRtCtrlId = (uint8 *)&gs_arrayEraseBackupMemRoutineID[0u];
        uCount = (uint8)sizeof(gs_arrayEraseBackupMemRoutineID);
    }
    else if (COPY_BACKUP_CONTROL == eRoutineCtrl)
    {
        pRtCtrlId = (uint8 *)&gs_arrayEraseCopyBackupMemRoutineID[0u];
        uCount = (uint8)sizeof(gs_arrayEraseCopyBackupMemRoutineID);
    }
#endif
    else
    {
        return FALSE;
    }

    if ((NULL_PTR != pRtCtrlId) && (pMsg->xDataMsgLength >= uCount))
    {
        for (uIdx = 0u; uIdx < uCount; uIdx++)
        {
            if (pMsg->aDataBuf[uIdx] == pRtCtrlId[uIdx])
            {
            }
            else
            {
                return FALSE;
            }
        }
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief 解析 31 01 FF 00 44 + 地址4B + 长度4B，区间须整段落在 Boot 区，并按512字节扇区对齐
 * @return 0=通过，非0=NRC
 */
static uint8 UDS_APP_ParseAndCheckEraseRange(const UDS_APP_tLocalAppMsgType *pMsg,
                                             uint32 *pAlignedStart,
                                             uint32 *pAlignedLen)
{
    uint32 addr;
    uint32 size;
    uint32 end;
    uint32 alignedStart;
    uint32 alignedEnd;
    uint32 winBegin = 0u;
    uint32 winEnd = 0u;
    const uint32 sectorMask = ((uint32)MOD_SECTOR_SIZE) - 1u;

    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pAlignedStart);
    ASSERT(NULL_PTR == pAlignedLen);

    if (pMsg->xDataMsgLength < 13u)
    {
        return E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT;
    }
    if (0x44u != pMsg->aDataBuf[4u])
    {
        return E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT;
    }

    addr = ((uint32)pMsg->aDataBuf[5u] << 24)
         | ((uint32)pMsg->aDataBuf[6u] << 16)
         | ((uint32)pMsg->aDataBuf[7u] << 8)
         | ((uint32)pMsg->aDataBuf[8u]);
    size = ((uint32)pMsg->aDataBuf[9u] << 24)
         | ((uint32)pMsg->aDataBuf[10u] << 16)
         | ((uint32)pMsg->aDataBuf[11u] << 8)
         | ((uint32)pMsg->aDataBuf[12u]);

    if ((0u == size) || ((addr + size) < addr))
    {
        return E_NRC_REQUEST_OUT_OF_RANGE;
    }

    end = addr + size;
    /* 过渡程序：整段须落在 Boot 区，禁止擦自己所在的 APP */
    if ((addr >= BOOT_BEGIN_ADDR) && (addr < BOOT_END_ADDR) && (end <= BOOT_END_ADDR))
    {
        winBegin = BOOT_BEGIN_ADDR;
        winEnd = BOOT_END_ADDR;
    }
    else
    {
        return E_NRC_REQUEST_OUT_OF_RANGE;
    }

    alignedStart = addr & (~sectorMask);
    alignedEnd = (end + sectorMask) & (~sectorMask);

    if (alignedStart < winBegin)
    {
        alignedStart = winBegin;
    }
    if (alignedEnd > winEnd)
    {
        alignedEnd = winEnd;
    }
    if (alignedEnd <= alignedStart)
    {
        return E_NRC_REQUEST_OUT_OF_RANGE;
    }

    *pAlignedStart = alignedStart;
    *pAlignedLen = alignedEnd - alignedStart;
    return 0u;
}

/**
 * @brief 校验当前例程是否为Flash擦除
 * @param pMsg 接收报文
 * @return ID匹配返回TRUE
 */
static uint8 UDS_APP_CheckErasingMemory(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(ERASE_MEMORY_CONTROL, pMsg);
}
// 新增：判断是否为校验文件合法性例程 0x6000
static uint8 UDS_APP_CheckAuthenticity(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(CHECK_AUTHENTICITY, pMsg);
}

// 新增：判断是否为版本校验例程 0x6001
static uint8 UDS_APP_CheckVersion(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(CHECK_VERSION, pMsg);
}

// 新增：判断是否为下载数据完整性检查例程 0x0203
static uint8 UDS_APP_CheckDataIntegrity(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(CHECK_DATA_INTEGRITY, pMsg);
}

#ifdef APP_SINGLE_UPDATA
/**
 * @brief 校验擦除备份分区例程ID
 */
static uint8 UDS_APP_CheckErasingBackupMemory(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(ERASE_BACKUP_CONTROL, pMsg);
}

/**
 * @brief 校验拷贝APP至备份分区例程ID
 */
static uint8 UDS_APP_CheckCopyBackupMemory(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(COPY_BACKUP_CONTROL, pMsg);
}
#endif

/**
 * @brief 校验CRC校验例程ID
 */
static uint8 UDS_APP_CheckSummationRoutine(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(CHECKSUM_CONTROL, pMsg);
}

/**
 * @brief 校验软硬件依赖检查例程ID
 */
static uint8 UDS_APP_CheckProgramDependency(const UDS_APP_tLocalAppMsgType *pMsg)
{
    ASSERT(NULL_PTR == pMsg);
    return UDS_APP_ValidateRoutineCtrlId(DEPENDENCY_CONTROL, pMsg);
}

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
#pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(__ICCARM__)
#pragma diag_suppress=Pe177
#else
#pragma clang diagnostic ignored "-Wunused-function"
#endif
/**
 * @brief 校验2E写指纹DID是否匹配
 * @param pMsg 接收报文
 * @return DID完全匹配返回TRUE
 */
static uint8 UDS_APP_ValidateWriteFingerprint(const UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 uCnt = 0u;
    uint8 uFpLen = 0u;
    ASSERT(NULL_PTR == pMsg);
    uFpLen = (uint8)sizeof(gs_arrayWriteFingerprintID);
    if (pMsg->xDataMsgLength >= uFpLen)
    {
        for (uCnt = 0u; uCnt < uFpLen; uCnt++)
        {
            if (pMsg->aDataBuf[uCnt] == gs_arrayWriteFingerprintID[uCnt])
            {
            }
            else
            {
                return FALSE;
            }
        }
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}
/**
 * @brief 0x2E校验是否为写入工具序列号 0xF198
 * @param pMsg 接收报文
 * @return TRUE匹配 / FALSE不匹配
 */
static uint8 UDS_APP_CheckWriteToolSerial(const UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 uCount = 0u;
    uint8 uIdx = 0u;
    const uint8 *pDidId = NULL_PTR;
    
    ASSERT(NULL_PTR == pMsg);
    
    pDidId = (uint8 *)&gs_arrayWriteToolSerialDID[0u];
    uCount = (uint8)sizeof(gs_arrayWriteToolSerialDID);
    
    if (pMsg->xDataMsgLength < uCount) {
        return FALSE;
    }
    
    for (uIdx = 0u; uIdx < uCount; uIdx++) {
        if (pMsg->aDataBuf[uIdx] != pDidId[uIdx]) {
            return FALSE;
        }
    }
    return TRUE;
}
/**
 * @brief 0x2E校验是否为写入编程日期 0xF199
 * @param pMsg 接收报文
 * @return TRUE匹配 / FALSE不匹配
 */
static uint8 UDS_APP_CheckWriteReprogDate(const UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 uCount = 0u;
    uint8 uIdx = 0u;
    const uint8 *pDidId = NULL_PTR;
    
    ASSERT(NULL_PTR == pMsg);
    
    pDidId = (uint8 *)&gs_arrayWriteReprogDateDID[0u];
    uCount = (uint8)sizeof(gs_arrayWriteReprogDateDID);
    
    if (pMsg->xDataMsgLength < uCount) {
        return FALSE;
    }
    
    for (uIdx = 0u; uIdx < uCount; uIdx++) {
        if (pMsg->aDataBuf[uIdx] != pDidId[uIdx]) {
            return FALSE;
        }
    }
    return TRUE;
}

/* 0x34 下载白名单：[begin, end)，end 不含。标定仅占位，暂无独立擦写流程 */
typedef struct
{
    uint32 begin;
    uint32 end;
} UdsDownloadWindowType;

static const UdsDownloadWindowType s_downloadAddrWhitelist[] =
{
    {FLS_DRV_BEGIN_ADDR, FLS_DRV_END_ADDR}, /* Flash 驱动 RAM */
    {BOOT_BEGIN_ADDR, BOOT_END_ADDR}        /* 过渡程序刷 Boot，不含信息头 */
};

/**
 * @brief 起始地址落在哪一个白名单窗口
 * @return TRUE=命中，可选带回该窗口起止
 */
static uint8 UDS_APP_FindDownloadWindow(const uint32 uAddr, uint32 *pBegin, uint32 *pEnd)
{
    uint32 i;
    uint32 n = (uint32)(sizeof(s_downloadAddrWhitelist) / sizeof(s_downloadAddrWhitelist[0]));

    for (i = 0u; i < n; i++)
    {
        if ((uAddr >= s_downloadAddrWhitelist[i].begin) &&
            (uAddr < s_downloadAddrWhitelist[i].end))
        {
            if (NULL_PTR != pBegin)
            {
                *pBegin = s_downloadAddrWhitelist[i].begin;
            }
            if (NULL_PTR != pEnd)
            {
                *pEnd = s_downloadAddrWhitelist[i].end;
            }
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * @brief 校验下载起始地址是否落在白名单窗口内
 */
static uint8 UDS_APP_CheckDownloadAddress(const uint32 uAddr)
{
    return UDS_APP_FindDownloadWindow(uAddr, NULL_PTR, NULL_PTR);
}

/**
 * @brief 整段 [start, start+len) 须落在起始地址所在的同一白名单窗口
 */
static uint8 UDS_APP_CheckDownloadLength(const uint32 uLen)
{
    uint32 winBegin = 0u;
    uint32 winEnd = 0u;
    uint32 addr;
    uint32 rangeEnd;

    if (0u == uLen)
    {
        return FALSE;
    }
    addr = gs_stDowloadDataInfo.uStartAddress;
    if ((addr + uLen) < addr)
    {
        return FALSE;
    }
    if (TRUE != UDS_APP_FindDownloadWindow(addr, &winBegin, &winEnd))
    {
        return FALSE;
    }
    (void)winBegin;
    rangeEnd = addr + uLen;
    if (rangeEnd > winEnd)
    {
        return FALSE;
    }
    return TRUE;
}

static void UDS_APP_SendRoutineCtrlResult(uint8 ridH, uint8 ridL, uint8 status, void (*pfCb)(uint8))
{
    uint8 aBuf[8u] = {0u};

    UDS_APP_StopP2StarServerTimer();
    aBuf[0u] = 0x71u;
    aBuf[1u] = 0x01u;
    aBuf[2u] = ridH;
    aBuf[3u] = ridL;
    aBuf[4u] = status;
    (void)TP_DataTransferQueueFrame(TP_GetTransportTxID(), pfCb, 5u, aBuf);
}

static void UDS_APP_PerformAuthenticity(uint8 state)
{
    int result;
    uint8 status;
    void (*pfCb)(uint8) = NULL_PTR;

    if (E_TX_MSG_OK != state)
    {
        return;
    }
    WATCHDOG_HAL_Fed();
    result = UDS_Verify_CheckFileValidity(s_aAuthParam);
    if (0 == result)
    {
        status = 0x04u;
        s_authenticityPassed = TRUE;
        UDSCFGDebugLog("singinfo OK OK OK OK OK OK\r\n");
    }
    else if (-1 == result)
    {
        status = 0x05u;
        s_authenticityPassed = FALSE;
        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_VERIFY_FAIL;
        pfCb = &UDS_APP_FlushFaultNvmAfterTx;
        UDSCFGDebugLog("singinfo fail %d\r\n", result);
    }
    else
    {
        status = 0x06u;
        s_authenticityPassed = FALSE;
        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_VERIFY_FAIL;
        pfCb = &UDS_APP_FlushFaultNvmAfterTx;
    }
    UDS_APP_SendRoutineCtrlResult(0x60u, 0x00u, status, pfCb);
}

static void UDS_APP_PerformDataIntegrity(uint8 state)
{
    int result;
    uint8 status;
    void (*pfCb)(uint8) = NULL_PTR;

    if (E_TX_MSG_OK != state)
    {
        return;
    }
    WATCHDOG_HAL_Fed();
    result = UDS_Verify_CheckDownloadedData();
    if (0 == result)
    {
        MarkAppDownloadIntegrityPassed();
        status = 0x04u;
    }
    else
    {
        /* 完整性失败视为刷写失败：作废信息头，不允许随后跳 APP */
        MarkAppDownloadIntegrityFailed();
        status = 0x05u;
        pfCb = &UDS_APP_FlushFaultNvmAfterTx;
    }
    UDS_APP_SendRoutineCtrlResult(0x02u, 0x03u, status, pfCb);
}

/* Flash操作超时回调函数指针类型 */
typedef void (*tpfFlashOperateMoreTimecallback)(uint8);
static tpfFlashOperateMoreTimecallback gs_pfFlashOperateMoreTimecallback = NULL_PTR;

/**
 * @brief 执行Flash操作超时注册的回调
 * @param bStat 报文发送状态
 */
static void UDS_APP_TriggerExtraTimeCallback(uint8 bStat)
{
    if (E_TX_MSG_OK != bStat)
    {
    }
    else
    {
        RestartS3Server();
    }
    if (NULL_PTR == gs_pfFlashOperateMoreTimecallback)
    {
    }
    else
    {
        gs_pfFlashOperateMoreTimecallback(bStat);
        gs_pfFlashOperateMoreTimecallback = NULL_PTR;
    }
}

/**
 * @brief 注册Flash长操作（擦除/校验）异步回调，返回78等待响应
 * @param uService 当前服务SID
 * @param pfCB 操作完成回调函数
 */
static void UDS_APP_TriggerExtendedTime(const uint8 uService, void (*pfCB)(uint8))
{
    UDS_APP_tLocalAppMsgType tMsgBox = {0};
    ASSERT(NULL_PTR == pfCB);
    tMsgBox.xUdsMsgId = TP_GetTransportTxID();
    UDS_APP_AssignNegErrCode(uService, E_NRC_SERVICE_BUSY, &tMsgBox);
    tMsgBox.pfTxMsgCb = &UDS_APP_TriggerExtraTimeCallback;
    UDS_APP_StartP2StarServerTimer();

    /* Store the provided callback in the global pointer */
    gs_pfFlashOperateMoreTimecallback = pfCB;

    /* Queue the frame for data transfer */
    (void)TP_DataTransferQueueFrame(tMsgBox.xUdsMsgId, 
                                    tMsgBox.pfTxMsgCb,
                                    tMsgBox.xDataMsgLength, 
                                    tMsgBox.aDataBuf);
}

/**
 * @brief APP校验完成回调，回复CRC校验结果
 * @param bStatus 校验成功/失败标记
 */
static void UDS_APP_PerformChecksum(uint8 bStatus)
{
    if (E_TX_MSG_OK != bStatus)
    {
    }
    else
    {
        SetCurFlsTaskPara(FLASH_IS_IN_CHECK, &UDS_APP_RespondCksumResult, 0x31u, &UDS_APP_TriggerExtendedTime);
    }
}

/**
 * @brief 0x31组装CRC校验完成正响应并发送
 * @param bFlag 校验结果TRUE=成功
 */
static void UDS_APP_RespondCksumResult(uint8 bFlag)
{
    uint8 aBuf[8u] = {0u};
    uint8 uLoop = 0u;
    uint8 uLen = 0u;
    TP_UdsIdType uTxId = 0u;
    UDS_APP_StopP2StarServerTimer();
    uLen = (uint8)((uint8)sizeof(gs_arrayCksumRoutineID) / (uint8)sizeof(gs_arrayCksumRoutineID[0u]));
    aBuf[0u] = (uint8)(gs_arrayCksumRoutineID[0u] + 0x40u);
    while (uLoop < (uLen - 1u))
    {
        aBuf[uLoop + 1u] = gs_arrayCksumRoutineID[uLoop + 1u];
        uLoop++;
    }
    if (TRUE != bFlag)
    {
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        s_flashDriverCrcPassed = FALSE; /* 0202 失败：标记，后续 FF00 回 NRC22 */
#endif
        aBuf[uLen] = 5u;
    }
    else
    {
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        s_flashDriverCrcPassed = TRUE; /* 0202 成功（结果 0x04） */
#endif
        aBuf[uLen] = 4u;
    }
    uLen++;
    uTxId = TP_GetTransportTxID();
    (void)TP_DataTransferQueueFrame(uTxId, NULL_PTR, uLen, aBuf);
}

/**
 * @brief Flash擦除完成响应报文组装发送
 * @param bFlag 擦除成功标记
 */
static void UDS_APP_EraseFlashAck(uint8 bFlag)
{
    uint8 uCnt = 0u;
    uint8 aBuf[8u] = {0u};
    uint8 uLen = 0u;
    TP_UdsIdType uTx = 0u;
    UDS_APP_StopP2StarServerTimer();
    if (TRUE != bFlag)
    {
        aBuf[0u] = NEGATIVE_RSP_ID;
        aBuf[1u] = 0x31;
        aBuf[2u] = E_NRC_CONDITIONS_NOT_CORRECT;
        uLen = 3u;
        SetCurrentSession(BASIC_SESSION);
        UDS_APP_AssignSecurityLevel(NO_SECURITY);
        PrepareDLInformation();
        g_udsMsgRetransCount = 0;
        (void)TP_DataTransferQueueFrame(TP_GetTransportTxID(), &UDS_APP_FlushFaultNvmAfterTx, uLen, aBuf);
        return;
    }
    else
    {
#ifdef APP_SINGLE_UPDATA
        if (TRUE == backupEraseFlag)
        {
            uLen = (uint8)((uint8)sizeof(gs_arrayEraseBackupMemRoutineID) / (uint8)sizeof(gs_arrayEraseBackupMemRoutineID[0u]));
            aBuf[0u] = (uint8)(gs_arrayEraseBackupMemRoutineID[0u] + 0x40u);
        }
        else
#endif
        {
            uLen = (uint8)((uint8)sizeof(gs_arrayEraseMemRoutineID) / (uint8)sizeof(gs_arrayEraseMemRoutineID[0u]));
            aBuf[0u] = (uint8)(gs_arrayEraseMemRoutineID[0u] + 0x40u);
        }
        while (uCnt < (uint8)(uLen - 1u))
        {
#ifdef APP_SINGLE_UPDATA
            if (TRUE == backupEraseFlag)
            {
                aBuf[uCnt + 1u] = gs_arrayEraseBackupMemRoutineID[uCnt + 1u];
            }
            else
#endif
            {
                aBuf[uCnt + 1u] = gs_arrayEraseMemRoutineID[uCnt + 1u];
            }
            uCnt++;
        }
    
        /* Invert the if condition for the status flag */
        /* Indicate success */
        aBuf[uLen] = 4u;  //擦除成功 -lu
    
        /* Increase the data length by one to include the last byte */
        uLen++;
    }
    uTx = TP_GetTransportTxID();
    (void)TP_DataTransferQueueFrame(uTx, NULL_PTR, uLen, aBuf);
}

/**
 * @brief 擦除Flash异步回调，触发等待响应
 * @param state 发送状态
 */
static void UDS_APP_PerformFlashErase(uint8 state)
{
    if (E_TX_MSG_OK != state)
    {
    }
    else
    {
        FaultInfo_Flush(); /* 本周期首次擦除次数 0x0201 */
        SetCurFlsTaskPara(FLASH_IS_IN_ERASING, &UDS_APP_EraseFlashAck, 0x31, &UDS_APP_TriggerExtendedTime);
    }
}

#ifdef APP_SINGLE_UPDATA
/**
 * @brief 擦除备份分区异步回调
 */
static void UDS_APP_PerformBackupFlashErase(uint8 state)
{
    if (E_TX_MSG_OK != state)
    {
    }
    else
    {
        backupEraseFlag = TRUE;
        SetCurFlsTaskPara(FLASH_IS_IN_ERASING, &UDS_APP_EraseFlashAck, 0x31, &UDS_APP_TriggerExtendedTime);
    }
}
#endif

/**
 * @brief 校验APP主程序与备份分区版本匹配逻辑
 * @return 软硬件版本兼容返回TRUE
 */
static uint8 UDS_APP_DoCheckProgramDependency(void)
{
    uint8 bResult = FALSE;
    if (TRUE != VerifyAppInfoFromFlsRationality())
    {
        bResult = FALSE;
    }
    else
    {
        if (TRUE == FlashAppRationality())
        {
            bResult = TRUE;
        }
        else
        {
            bResult = FALSE;
        }
    }
    return bResult;
}

/**
 * @brief 发送复位报文完成回调，切回默认会话并解除安全锁
 * @param bStatus 报文发送状态
 */
static void UDS_APP_ConfirmTxMessage(uint8 bStatus)
{
    if (E_TX_MSG_OK != bStatus)
    {
    }
    else
    {
        SetCurrentSession(PROGRAM_SESS);
        UDS_APP_AssignSecurityLevel(NO_SECURITY);
        RestartS3Server();
    }
}
#endif

/**
 * @brief 0x36数据传输完成后组装正响应报文
 * @param bFlag 写入Flash成功标记
 */
void UDS_APP_WriteFlashAck(uint8 bFlag)
{
    uint8 uCnt = 0u;
    uint8 aBuf[8u] = {0u};
    uint8 uLen = 0u;
    TP_UdsIdType uTx = 0u;
    UDS_APP_StopP2StarServerTimer();
    if (TRUE != bFlag)
    {
        aBuf[0u] = NEGATIVE_RSP_ID;
        aBuf[1u] = 0x36;
        aBuf[2u] = E_NRC_CONDITIONS_NOT_CORRECT;
        uLen = 3u;
        SetCurrentSession(BASIC_SESSION);
        UDS_APP_AssignSecurityLevel(NO_SECURITY);
        PrepareDLInformation();
        g_udsMsgRetransCount = 0;
        (void)TP_DataTransferQueueFrame(TP_GetTransportTxID(), &UDS_APP_FlushFaultNvmAfterTx, uLen, aBuf);
        return;
    }
    else
    {
        aBuf[0] = 0x36 + 0x40u;
        aBuf[1] = uBlockNum;
        uLen = 2u;
    }
    uTx = TP_GetTransportTxID();
    (void)TP_DataTransferQueueFrame(uTx, NULL_PTR, uLen, aBuf);
}

/**
 * @brief APP主动下发请求进入Bootloader的10 02报文
 * @return 报文入队成功TRUE
 */
boolean UDS_APP_SendMsgToHost(void)
{
    UDS_APP_tLocalAppMsgType tMsgInfo = {0u, 0u, {0u}, NULL_PTR};
    boolean bResult = FALSE;
    tMsgInfo.xUdsMsgId = TP_GetTransportTxID();
    tMsgInfo.xDataMsgLength = 2u;
#ifdef UDS_PROJECT_FOR_BOOTLOADER
    /* Boot工程发送0x50 0x02响应（含 P2 / P2*） */
    tMsgInfo.aDataBuf[0u] = 0x50u;
    tMsgInfo.aDataBuf[1u] = 0x02u;
    UDS_APP_FillP2ServerParams(&tMsgInfo.aDataBuf[2u]);
    tMsgInfo.xDataMsgLength = 6u;
    tMsgInfo.pfTxMsgCb = UDS_APP_ConfirmTxMessage;
#endif
#ifdef UDS_PROJECT_FOR_APP
    /* APP工程发送进入boot请求 */
    tMsgInfo.aDataBuf[0u] = 0x51u;
    tMsgInfo.aDataBuf[1u] = 0x01u;
    tMsgInfo.pfTxMsgCb = NULL_PTR;
#endif
    bResult = TP_DataTransferQueueFrame(tMsgInfo.xUdsMsgId,
                                        tMsgInfo.pfTxMsgCb,
                                        tMsgInfo.xDataMsgLength,
                                        tMsgInfo.aDataBuf);
    return bResult;
}

/**
 * @brief UDS系统1ms周期调度函数，递减S3、安全锁定、P2、P2*计时器
 */
void UDS_APP_HandleSystemTicks(void)
{
    // S3会话超时计时自减
    if (!UDS_APP_AcquireS3ServerCountdown())
    {
    }
    else
    {
        UDS_APP_DecrementS3ServerCountdown(1u);
    }
    // 安全访问锁定计时自减
    if (!UDS_APP_AcquireSecurityLockCountdown())
    {
    }
    else
    {
        UDS_APP_DecrementSecurityLockCountdown(1u);
    }
    // P2服务端响应计时自减
    if (!UDS_APP_AcquireP2ServerCountdown())
    {
    }
    else
    {
        UDS_APP_DecrementP2ServerCountdown(1u);
    }
    // P2*扩展响应计时自减
    if (!UDS_APP_AcquireP2StarServerCountdown())
    {
    }
    else
    {
        UDS_APP_DecrementP2StarServerCountdown(1u);
    }
#ifdef UDS_PROJECT_FOR_BOOTLOADER
#ifdef ALLOW_DELAY_TIME
    // 延时下线倒计时
    if (TRUE == UDS_APP_HasRxUdsMessage())
    {
    }
    else
    {
        if (gs_structForTimeInformation.appDelay)
        {
            gs_structForTimeInformation.appDelay--;
        }
        else
        {
            UDS_APP_PerformMcuReset(E_TX_MSG_OK);
        }
    }
#endif
#endif
}

/**
 * @brief 获取S3超时水位阈值（S3总时长百分比）
 * @return 毫秒数，用于提前判定会话即将超时
 */
uint32 UDS_GetUDSS3WatermarkTimerMs(void)
{
    const uint32 watermarkTimerMs = (g_udsTimeConfigTable.cS3Time * S3_TIMER_WATERMARK_PERCENTAGE) / 100u;
    return (uint32)watermarkTimerMs;
}

/**
 * @brief 重置S3会话超时计时器，每次收到诊断报文调用
 */
void RestartS3Server(void)
{
    g_udsConfiguration.iS3Time = UDS_APP_TIME_TO_COUNT(g_udsTimeConfigTable.cS3Time);
}

/**
 * @brief 设置当前全局会话类型
 * @param i_SerSessionMode BASIC/PROGRAM/EXTENDED
 */
void SetCurrentSession(const uint8 i_SerSessionMode)
{
    g_udsConfiguration.iSessionMode = i_SerSessionMode;
    /* 规范 7.17：切回默认会话后恢复 DTC 状态位更新 */
    if (BASIC_SESSION == i_SerSessionMode) {
        FaultInfo_SetDtcSetting(1u);
    }
}

uint8 UDS_APP_GetF186Session(void)
{
    if (PROGRAM_SESS == g_udsConfiguration.iSessionMode)
    {
        return 0x02u;
    }
    if (EXTENDED_SESS == g_udsConfiguration.iSessionMode)
    {
        return 0x03u;
    }
    return 0x01u;
}
