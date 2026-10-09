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

/* 0x28 普通通信开关。1=允许，0=关闭。应用层读取后自行开关报文。 */
volatile uint8 g_udsNormalCommEnable = 1u;

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
#ifdef UDS_PROJECT_FOR_APP
    /* 0x19 读DTC信息（调查表：默认/扩展会话 Y，编程会话 N）
     * 已做：01 按状态掩码报条数、02 按状态掩码报当前码、0A 报支持的码表
     * 暂不做：04 Snapshot、06 扩展数据（回 NRC 0x12） */
    {
        0x19u,
        BASIC_SESSION | EXTENDED_SESS,                 // 编程会话不支持
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,         // 物理+功能寻址
        NO_SECURITY,
        UDS_APP_ReadDTCInformation
    },
#endif
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
        SECURITY_LEVEL_1,                // 需要一级安全解锁
        UDS_APP_WriteDataIdent
    },
    /* 0x34 请求下载，Flash擦写前置服务 */
    {
        0x34u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        SECURITY_LEVEL_1,
        UDS_APP_ReqDownload
    },
    /* 0x36 传输升级数据包 */
    {
        0x36u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        SECURITY_LEVEL_1,
        UDS_APP_TransferProcess
    },
    /* 0x37 退出传输，校验程序CRC */
    {
        0x37u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        SECURITY_LEVEL_1,
        UDS_APP_FinalizeTransfer
    },
    /* 0x31 例程控制（擦除、校验、备份APP） */
    {
        0x31u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID,
        SECURITY_LEVEL_1,
        UDS_APP_HandleRoutineOperation
    },
    /* 0x11 ECU硬件/软件复位 */
    {
        0x11u,
        PROGRAM_SESS,
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,
        SECURITY_LEVEL_1,
        UDS_APP_PerformEcuReset
    },
#endif
#ifdef UDS_PROJECT_FOR_APP
    /* APP 侧 0x11：刷写完成后诊断仪也会发，需回 51 01 再复位 */
    {
        0x11u,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS,
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,
        NO_SECURITY,
        UDS_APP_PerformEcuReset
    },
    /* 0x09 OBD 车辆信息（VIN：PID 0x02） */
    {
        0x09u,
        BASIC_SESSION | PROGRAM_SESS | EXTENDED_SESS,
        ALLOW_PHYSICAL_ID | ALLOW_FUNCTION_ID,
        NO_SECURITY,
        UDS_APP_ObdRequestVehicleInfo
    },
    /* 0x27 安全访问：扩展会话 Level1，供 2E 写 VIN */
    {
        0x27u,
        EXTENDED_SESS,
        ALLOW_PHYSICAL_ID,
        NO_SECURITY,
        UDS_APP_SecAccessFunc
    },
    /* 0x2E 写 DID：APP 仅 VIN(0xF190)，需 Level1 */
    {
        0x2Eu,
        EXTENDED_SESS,
        ALLOW_PHYSICAL_ID,
        SECURITY_LEVEL_1,
        UDS_APP_WriteVinIdent
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

#ifdef UDS_PROJECT_FOR_APP
/* DTC 状态字节：调查表只支持 bit0 TestFailed、bit3 ConfirmedDTC，合起来 0x09 */
#define DTC_STATUS_AVAIL_MASK   0x09u   /* 应答里的 StatusAvailabilityMask，告诉工具我们支持哪几位 */
#define DTC_STATUS_STORED       0x09u   /* 仓库里已置位的码，当前按“失败+已确认”回 */
#define DTC_FORMAT_ISO15031     0x00u   /* 01 应答第4字节：三字节 DTC 格式 */

/* 填一条 4 字节记录：DTC 高/中/低 + 状态 */
static void UDS_APP_FillDtcRecord(uint8 *pBuf, uint32 dtc, uint8 status)
{
    pBuf[0u] = (uint8)((dtc >> 16u) & 0xFFu);
    pBuf[1u] = (uint8)((dtc >> 8u) & 0xFFu);
    pBuf[2u] = (uint8)(dtc & 0xFFu);
    pBuf[3u] = status;
}

/**
 * @brief 0x19 ReadDTCInformation
 * 请求：
 *   19 01 StatusMask     —— 报匹配条数，应答 59 01 AvailMask Format CountH CountL
 *   19 02 StatusMask     —— 报当前已置位且匹配掩码的码，应答 59 02 AvailMask {DTC3字节+状态}*N
 *   19 0A                —— 报 ECU 支持的全部码（未置位状态 0x00），应答 59 0A AvailMask {DTC3字节+状态}*N
 *   19 04 / 19 06        —— 暂不支持，NRC 0x12
 * StatusMask：工具关心哪些状态位；码的状态与掩码按位与非 0 才报。0A 没有掩码。
 * 子功能 bit7=1 时抑制正响应（与 10/3E 相同）。
 */
static void UDS_APP_ReadDTCInformation(struct UDS_APP_ServiceInfoType *pLocalSrv,
                                       UDS_APP_tLocalAppMsgType *pLocalMsg)
{
    uint8 sub;
    uint8 mask = 0u;
    uint8 suppress;
    uint32 dtc_list[FAULT_DTC_MAX_COUNT];
    uint16 stored;
    uint16 match;
    uint16 i;
    uint16 n;
    uint16 max_report;

    ASSERT(NULL_PTR == pLocalMsg);
    ASSERT(NULL_PTR == pLocalSrv);

    /* 至少要有 SID + 子功能 */
    if (pLocalMsg->xDataMsgLength < 2u)
    {
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pLocalMsg);
        return;
    }

    sub = (uint8)(pLocalMsg->aDataBuf[1u] & 0x7Fu);      /* 真正的子功能号 */
    suppress = (uint8)(pLocalMsg->aDataBuf[1u] & 0x80u); /* bit7 抑制正响应 */

    if ((0x01u != sub) && (0x02u != sub) && (0x0Au != sub))
    {
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pLocalMsg);
        return;
    }

    /* 01/02 固定 3 字节（带 StatusMask）；0A 固定 2 字节（不带掩码） */
    if (0x0Au == sub)
    {
        if (pLocalMsg->xDataMsgLength != 2u)
        {
            UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pLocalMsg);
            return;
        }
    }
    else if (pLocalMsg->xDataMsgLength != 3u)
    {
        UDS_APP_AssignNegErrCode(pLocalSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pLocalMsg);
        return;
    }
    else
    {
        mask = pLocalMsg->aDataBuf[2u]; /* 先记下请求掩码，后面应答会覆盖 byte2 */
    }

    /* 单帧 PDU 64 字节：59 + 子功能 + AvailMask + 每条 4 字节，最多 15 条 */
    max_report = (uint16)((UDS_MAX_PDU_LEN - 3u) / 4u);

    if (0u != suppress)
    {
        pLocalMsg->xDataMsgLength = 0u; /* 不回复正响应 */
        return;
    }

    pLocalMsg->aDataBuf[0u] = (uint8)(pLocalSrv->serviceId + 0x40u); /* 0x59 */
    pLocalMsg->aDataBuf[1u] = sub;
    pLocalMsg->aDataBuf[2u] = DTC_STATUS_AVAIL_MASK;                /* 本 ECU 支持的状态位 */

    /* ---- 0A：固定码表，不管有没有故障都报 ---- */
    if (0x0Au == sub)
    {
        n = FaultInfo_GetSupportedDTCCount();
        if (n > max_report)
        {
            n = max_report;
        }
        for (i = 0u; i < n; i++)
        {
            uint32 dtc = FaultInfo_GetSupportedDTC(i);
            uint8 status = (FaultInfo_IsDTCSet(dtc) != 0u) ? DTC_STATUS_STORED : 0x00u;

            UDS_APP_FillDtcRecord(&pLocalMsg->aDataBuf[3u + (i * 4u)], dtc, status);
        }
        pLocalMsg->xDataMsgLength = (TP_LengthType)(3u + (n * 4u));
        UDSCFGDebugLog("0x19 0A count=%u\n", n);
        return;
    }

    /* ---- 01/02：只看当前仓库里已置位的码 ---- */
    stored = FaultInfo_GetDTCList(dtc_list, FAULT_DTC_MAX_COUNT);
    match = 0u;
    if ((DTC_STATUS_STORED & mask) != 0u) /* 0x09 与工具掩码有交集才算匹配 */
    {
        match = stored;
    }
    if (match > max_report)
    {
        match = max_report;
    }

    /* 01：只回条数，不回码本身 */
    if (0x01u == sub)
    {
        pLocalMsg->aDataBuf[3u] = DTC_FORMAT_ISO15031;
        pLocalMsg->aDataBuf[4u] = (uint8)((match >> 8u) & 0xFFu);
        pLocalMsg->aDataBuf[5u] = (uint8)(match & 0xFFu);
        pLocalMsg->xDataMsgLength = 6u;
        UDSCFGDebugLog("0x19 01 mask=0x%02X count=%u\n", mask, match);
        return;
    }

    /* 02：回匹配到的码列表 */
    for (i = 0u; i < match; i++)
    {
        UDS_APP_FillDtcRecord(&pLocalMsg->aDataBuf[3u + (i * 4u)], dtc_list[i], DTC_STATUS_STORED);
    }
    pLocalMsg->xDataMsgLength = (TP_LengthType)(3u + (match * 4u));
    UDSCFGDebugLog("0x19 02 mask=0x%02X count=%u\n", mask, match);
}
#endif

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
    uint8 ucCommType = 0u;
    ASSERT(NULL_PTR == pLocalMsg);
    ASSERT(NULL_PTR == pLocalSrv);

    ucLocalCmd = pLocalMsg->aDataBuf[1u]; // 通信控制子功能
    if (pLocalMsg->xDataMsgLength >= 3u)
    {
        ucCommType = pLocalMsg->aDataBuf[2u];
    }
    /* bit0=普通通信。只记标志，不在这里开关报文。0x80/0x83 是抑制正响应的同一条命令。 */
    if (0u != (ucCommType & 0x01u))
    {
        if ((0x00u == ucLocalCmd) || (0x80u == ucLocalCmd))
        {
            g_udsNormalCommEnable = 1u;
        }
        else if ((0x03u == ucLocalCmd) || (0x83u == ucLocalCmd))
        {
            g_udsNormalCommEnable = 0u;
        }
    }

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

#ifdef UDS_PROJECT_FOR_APP
/* APP Level1：扩展会话种子密钥算法，与 Boot 扩展会话一致 */
static uint32 UDS_APP_SeedKeyExtLevel1(uint32 seed)
{
    return ((((seed >> 4U) ^ seed) << 3U) ^ seed);
}

/**
 * @brief 0x27 安全访问（APP 仅 Level1：01 请求种子 / 02 发送密钥）
 */
static void UDS_APP_SecAccessFunc(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8 ucLocalSubFunc = 0u;
    static uint8 s_aLocalSeed[SA_ALGORITHM_SEED_SIZE] = {0u};
    boolean bLocalRst = FALSE;
    uint32 seed;
    uint32 local_key;
    uint32 remote_key;

    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);

    if (pMsg->xDataMsgLength < 2u)
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
        return;
    }

    ucLocalSubFunc = pMsg->aDataBuf[1u];
    if (0x01u == ucLocalSubFunc)
    {
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
        bLocalRst = UDS_ALG_HAL_MyReplacedFunc(SA_ALGORITHM_SEED_SIZE, s_aLocalSeed);
        if (FALSE == bLocalRst)
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_KEY, pMsg);
        }
        else
        {
            (void)memcpy(&pMsg->aDataBuf[2u], s_aLocalSeed, SA_ALGORITHM_SEED_SIZE);
            pMsg->xDataMsgLength = 2u + SA_ALGORITHM_SEED_SIZE;
            UDSCFGDebugLog("\r\n0x27 0x01 APP seed ok\r\n");
        }
    }
    else if (0x02u == ucLocalSubFunc)
    {
        if (pMsg->xDataMsgLength < (2u + SA_ALGORITHM_SEED_SIZE))
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
            return;
        }
        seed = ((uint32)s_aLocalSeed[0] << 24U) |
               ((uint32)s_aLocalSeed[1] << 16U) |
               ((uint32)s_aLocalSeed[2] << 8U) |
               ((uint32)s_aLocalSeed[3]);
        local_key = UDS_APP_SeedKeyExtLevel1(seed);
        remote_key = ((uint32)pMsg->aDataBuf[2] << 24U) |
                     ((uint32)pMsg->aDataBuf[3] << 16U) |
                     ((uint32)pMsg->aDataBuf[4] << 8U) |
                     ((uint32)pMsg->aDataBuf[5]);
        if (local_key != remote_key)
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_KEY, pMsg);
        }
        else
        {
            pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
            pMsg->xDataMsgLength = 2u;
            (void)memset(s_aLocalSeed, 0x01u, sizeof(s_aLocalSeed));
            UDS_APP_AssignSecurityLevel(SECURITY_LEVEL_1);
            UDSCFGDebugLog("0x27 APP Level1 unlock ok\r\n");
        }
    }
    else
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pMsg);
    }
}

/**
 * @brief 0x2E 写 VIN（0xF190），仅 APP + Level1
 */
static void UDS_APP_WriteVinIdent(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint16_t did;
    uint8_t data_len;
    int ret;

    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);

    if (pMsg->xDataMsgLength < 3u)
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
        return;
    }

    did = (uint16_t)(((uint16_t)pMsg->aDataBuf[1u] << 8) | pMsg->aDataBuf[2u]);
    if (did != 0xF190u)
    {
        UDSCFGDebugLog("0x2E APP unsupported DID 0x%04X\r\n", did);
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_OUT_OF_RANGE, pMsg);
        return;
    }

    data_len = (uint8_t)(pMsg->xDataMsgLength - 3u);
    if (data_len != LEN_VIN)
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
        return;
    }

    ret = Did_Write(0xF190u, &pMsg->aDataBuf[3u], data_len);
    if (ret == -2)
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
        return;
    }
    if (ret != 0)
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
        return;
    }

    pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
    pMsg->aDataBuf[1u] = 0xF1u;
    pMsg->aDataBuf[2u] = 0x90u;
    pMsg->xDataMsgLength = 3u;
    UDSCFGDebugLog("0x2E F190 VIN write ok\r\n");
}

/**
 * @brief OBD Mode 09：PID 00 支持位图，PID 02 读 VIN
 */
static void UDS_APP_ObdRequestVehicleInfo(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg)
{
    uint8_t pid;
    TP_LengthType vin_len = 0u;
    int ret;

    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);

    if (pMsg->xDataMsgLength < 2u)
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT, pMsg);
        return;
    }

    pid = pMsg->aDataBuf[1u];
    if (0x00u == pid)
    {
        /* PID 02 支持：第一字节 bit6 = 0x40 */
        pMsg->aDataBuf[0u] = (uint8)(pSrv->serviceId + 0x40u);
        pMsg->aDataBuf[1u] = 0x00u;
        pMsg->aDataBuf[2u] = 0x40u;
        pMsg->aDataBuf[3u] = 0x00u;
        pMsg->aDataBuf[4u] = 0x00u;
        pMsg->aDataBuf[5u] = 0x00u;
        pMsg->xDataMsgLength = 6u;
        return;
    }
    if (0x02u != pid)
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_OUT_OF_RANGE, pMsg);
        return;
    }

    ret = Did_Read(0xF190u, &pMsg->aDataBuf[3u], &vin_len);
    if ((ret != 0) || (vin_len != LEN_VIN))
    {
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_CONDITIONS_NOT_CORRECT, pMsg);
        return;
    }

    pMsg->aDataBuf[0u] = (uint8)(pSrv->serviceId + 0x40u); /* 0x49 */
    pMsg->aDataBuf[1u] = 0x02u;
    pMsg->aDataBuf[2u] = 0x01u; /* number of data items */
    pMsg->xDataMsgLength = 3u + LEN_VIN;
    UDSCFGDebugLog("0x09 02 VIN read ok\r\n");
}
#endif

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
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_INVALID_KEY, pMsg);
        }
        else
        {
            // 校验通过，解锁一级安全等级
            pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
            pMsg->xDataMsgLength = 2u;
            UDS_APP_FillMemory(0x1u, sizeof(s_aLocalSeed), s_aLocalSeed); // 清空种子缓存
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

    // ★ 先检查超时
    // if (TRUE == UDS_APP_CheckAndHandleP2Timeout(pSrv, pMsg, pSrv->serviceId)) {
    //     return;
    // }

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
        
        UDSCFGDebugLog("0x2E: Write Tool Serial success\n");
        return;
    }
    // 3. 判断是否为写入编程日期 0xF199
    // ================================================================
    if (TRUE == UDS_APP_CheckWriteReprogDate(pMsg))
    {
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

    // 校验地址、长度是否在合法Flash区间 -----后续需要添加
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
        pMsg->aDataBuf[2u] = 0x00u;
        pMsg->aDataBuf[3u] = 0x82u;
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
    uint8 bDuplicateBlock = FALSE;  //重复：bDuplicateBlock=TRUE 不重复：bDuplicateBlock=FALSE
    uint8 uPrevBlockNum = 0u;
    ASSERT(NULL_PTR == pMsg);
    ASSERT(NULL_PTR == pSrv);
    uint8 nrc = 0x00;  // 新增：记录错误码

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

    // 步骤2：校验块号连续性
    if ((gs_RxBlockNum == uBlockNum) || (TRUE != bFlag)) // gs_RxBlockNum=我想要的块的块号=uBlockNum当前进来的块号,块号正确什么都不执行
    {
    }
    else if (uPrevBlockNum == uBlockNum)
    {
        bDuplicateBlock = TRUE; // 重复块无需重写
    }
    else
    {
        bFlag = FALSE;
        nrc = E_NRC_REQUEST_SEQUENCE_ERROR;
        FlashDebugLog("0x36 download fail block cross \r\n");
        //UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_SEQUENCE_ERROR, pMsg);
    }

    // 步骤3：无重复、状态正常则写入Flash
    if ((TRUE == bFlag) && (TRUE != bDuplicateBlock))
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
        UDS_APP_AssignNegErrCode(pSrv->serviceId, nrc, pMsg);
        PrepareDLInformation();
        SetNextDLPara(FLASH_DOWNLOAD_REQUEST);
        gs_RxBlockNum = 0u;
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

    // 校验当前阶段是否为传输完成
    if (FLASH_DOWNLOAD_EXIT_TRANSFER == GainCurDLPara())
    {
        FlashDebugLog("0x37 EXIT_TRANSFER In crc\r\n");
    }
    else
    {
        FlashDebugLog("0x37 EXIT_TRANSFER in crc fail\r\n");
        bResult = FALSE;
        UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_REQUEST_SEQUENCE_ERROR, pMsg);
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

        UDSCFGDebugLog("inter 0x31 0xFF00 ErasingMemory\r\n");
        eraseNrc = UDS_APP_ParseAndCheckEraseRange(pMsg, &eraseStart, &eraseLen);
        if (0u != eraseNrc)
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, eraseNrc, pMsg);
            return;
        }
        RecordEraseMemoryRange(eraseStart, eraseLen);
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
        // 版本校验逻辑：比较上位机发来的版本号与当前ECU版本号
        // 版本信息在 aDataBuf[4] 开始（共10字节）
        // TODO: 实现版本比对逻辑
        uint8_t compare_result = UDS_APP_CompareVersion();
        
        pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;  // 0x71
        pMsg->aDataBuf[1u] = 0x01u;
        pMsg->aDataBuf[2u] = 0x60u;
        pMsg->aDataBuf[3u] = 0x01u;
        if (compare_result == 0) {
            pMsg->aDataBuf[4u] = 0x04u;  // 检查成功
        } else {
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

        (void)WriteAppInfoToFls();
        bFlag = UDS_APP_DoCheckProgramDependency();
        if (TRUE == bFlag)
        {
            pMsg->aDataBuf[0u] = pSrv->serviceId + 0x40u;
            pMsg->xDataMsgLength = 4u;
            FaultInfo_OnProgramSuccess();
            (void)Did_Upd_Sw_Ver(UDS_Verify_GetSavedVersion());
            pMsg->pfTxMsgCb = &UDS_APP_FlushFaultNvmAfterTx;
        }
        else
        {
            UDS_APP_AssignNegErrCode(pSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pMsg);
        }
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
    RamFDErase(); // 擦除RAM临时升级标记
    SetSuccFlagForDownloadApp(); // 设置升级成功标志
    pstMsg->pfTxMsgCb = &UDS_APP_PerformMcuReset; // 发送完成执行复位
    UDS_APP_AssignNegErrCode(pstSrv->serviceId, E_NRC_SERVICE_BUSY, pstMsg);
    UDS_APP_StartP2StarServerTimer();
}
#else
/**
 * @brief APP 侧 0x11：回 51 xx 后再复位；81/83 抑制正响应则立即复位
 */
static void UDS_APP_PerformEcuReset(struct UDS_APP_ServiceInfoType *pstSrv, UDS_APP_tLocalAppMsgType *pstMsg)
{
    uint8 ucSubFunc;

    ASSERT(NULL_PTR == pstMsg);
    ASSERT(NULL_PTR == pstSrv);

    ucSubFunc = pstMsg->aDataBuf[1u];
    if ((0x01u == ucSubFunc) || (0x03u == ucSubFunc))
    {
        pstMsg->aDataBuf[0u] = pstSrv->serviceId + 0x40u;
        pstMsg->aDataBuf[1u] = ucSubFunc;
        pstMsg->xDataMsgLength = 2u;
        pstMsg->pfTxMsgCb = &UDS_APP_PerformMcuReset;
    }
    else if ((0x81u == ucSubFunc) || (0x83u == ucSubFunc))
    {
        pstMsg->xDataMsgLength = 0u;
        UDS_APP_PerformMcuReset(E_TX_MSG_OK);
    }
    else
    {
        UDS_APP_AssignNegErrCode(pstSrv->serviceId, E_NRC_SUBFUNCTION_NOT_SUPPORTED, pstMsg);
    }
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
    uint8_t local_version[10] = {0};
    int result;
    uint8_t * new_version = NULL;
    uint8_t local_trim[10] = {0};
    uint8_t new_trim[10] = {0};
	uint8_t i, j;
	
    new_version = UDS_Verify_GetSavedVersion();

    // 1. 获取当前ECU软件版本号（从 DID 0xF189 读取）
    //    实际应从 FlashInfo_Get() 或 NVM 中读取
	ECU_Information_t* info = Did_Info_Get();
    if (NULL_PTR != info) {
        memcpy(local_version, info->sw_version, 10);
    } else {
        // 如果无法获取，默认允许刷写（保守策略）
        UDSCFGDebugLog("Version: Cannot get local version, allow update\n");
        return 0;
    }

    UDSCFGDebugLog("Version: Local=[%.10s], new_version=[%.10s]\n", 
                   local_version, new_version);

	//    去除两个版本号末尾的空格和空字符
    //    复制有效字符（遇到空格或'\0'停止）
    for (i = 0, j = 0; i < 10 && local_version[i] != ' ' && local_version[i] != '\0'; i++, j++) {
        local_trim[j] = local_version[i];
    }
   
    for (i = 0, j = 0; i < 10 && new_version[i] != ' ' && new_version[i] != '\0'; i++, j++) {
        new_trim[j] = new_version[i];
    }
	
    // 2. 版本比对（ASCII字符串比较）
    //    规则：如果远程版本号低于本地版本号，拒绝刷写
    result = memcmp(new_trim, local_trim, 10);

    if (result < 0) {
        // 远程版本低于本地版本 → 拒绝
        UDSCFGDebugLog("Version: Remote is LOWER than Local, reject!\n");
        return -1;  // 版本过低
    } else if (result == 0) {
        // 版本相同 → 允许（重刷同版本）
        UDSCFGDebugLog("Version: Same version, allow re-flash\n");
        return 0;
    } else {
        // 远程版本高于本地版本 → 允许升级
        UDSCFGDebugLog("Version: Remote is HIGHER than Local, allow update\n");
        return 0;
    }
}

/**
 * @brief 校验例程控制ID是否匹配预设数组
 * @param eRoutineCtrl 例程类型枚举
 * @param pMsg 接收报文缓存
 * @return ID匹配返回TRUE
 */
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
 * @brief 解析 31 01 FF 00 44 + 地址4B + 长度4B，校验不得擦Boot，并按512字节扇区对齐
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
    /* 原始区间必须完全落在APP，Boot及APP外一律拒绝 */
    if ((addr < APP_A_BEGIN_ADDR) || (addr >= APP_A_END_ADDR) ||
        ((addr + size) > APP_A_END_ADDR))
    {
        return E_NRC_REQUEST_OUT_OF_RANGE;
    }

    alignedStart = addr & (~sectorMask);
    end = addr + size;
    alignedEnd = (end + sectorMask) & (~sectorMask);

    if (alignedStart < APP_A_BEGIN_ADDR)
    {
        alignedStart = APP_A_BEGIN_ADDR;
    }
    if (alignedEnd > APP_A_END_ADDR)
    {
        alignedEnd = APP_A_END_ADDR;
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

/**
 * @brief 校验下载起始地址是否在合法Flash区间
 */
static uint8 UDS_APP_CheckDownloadAddress(const uint32 uAddr)
{
    return TRUE;
}

/**
 * @brief 校验下载程序长度不超过Flash分区大小
 */
static uint8 UDS_APP_CheckDownloadLength(const uint32 uLen)
{
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
        UDSCFGDebugLog("singinfo OK OK OK OK OK OK\r\n");
    }
    else if (-1 == result)
    {
        status = 0x05u;
        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_VERIFY_FAIL;
        pfCb = &UDS_APP_FlushFaultNvmAfterTx;
        UDSCFGDebugLog("singinfo fail %d\r\n", result);
    }
    else
    {
        status = 0x06u;
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
        status = 0x04u;
    }
    else
    {
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
        aBuf[uLen] = 5u;
    }
    else
    {
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
