#include "uds_app.h"
#include "TP.h"
#include "Boot.h"
#include "fls_app.h"
#include "uds_alg_hal.h"
#include "bootloader_debug.h"
#ifdef UDS_PROJECT_FOR_BOOTLOADER
/* 分段下载重传标记，收到重复报文置1，用于计数重传次数 */
static uint8 gs_udsMsgRetransFlag = 0x00;
/* 上一次处理的诊断服务SID，用于新会话重置下载状态 */
static uint8 gs_LastUDSServiceNum = 0x00;  
#ifdef ALLOW_DELAY_TIME
/* 跳转APP延时全局配置结构体外部引用 */
extern UDS_APP_DelayTimeInfoType gs_structForTimeInformation;
#endif
#endif
/* UDS分段下载报文重传计数全局变量，最大允许重传2次 */
uint8 g_udsMsgRetransCount = 0;
/**
 * @brief UDS模块初始化，上电复位调用
 */
void UDS_Init(void)
{
#ifdef UDS_PROJECT_FOR_BOOTLOADER
#ifdef ALLOW_DELAY_TIME
    // 初始化跳转APP延时计数值，转换配置毫秒为调度周期计数
    gs_structForTimeInformation.appDelay = UDS_APP_TIME_TO_COUNT(LONGEST_DELAY_TIME_MS);
#endif
#endif
}
/**
 * @brief UDS主循环任务，周期调用，完成报文接收、服务匹配、逻辑处理、报文发送
 */
void UDS_MainFunction(void)
{
    /* 服务数组遍历索引 */
    uint8 ucSerIdx = 0u;
    /* 当前接收报文的诊断服务SID */
    uint8 ucSerNum = 0u;
    /* 本地UDS报文缓存结构体，单次处理一帧报文 */
    UDS_APP_tLocalAppMsgType stLocalMsg = {0u, 0u, {0u}, NULL_PTR};
    /* 标记是否在服务注册表匹配到对应SID */
    uint8 bServiceFound = FALSE;
    /* 系统支持的诊断服务总数量 */
    uint8 ucSupSerItem = 0u;
    /* 服务注册表数组指针 */
    UDS_APP_localServiceType *pLocalUDSService = NULL_PTR;
    
#if defined (EN_AES_SA_ALGORITHM_SW) || defined (ALLOW_ZLG_ZXDOC_SA_ALGORITHM)
    // 安全算法软件计时滴答累加
    UDS_ALG_HAL_AddSWTimerTickCnt();
#endif
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        // 判断S3会话是否超时
        if (TRUE != UDS_APP_CheckS3ServerIsTimeout())
        {
            /* S3未超时，无操作 */
        }
        else
        {
            // S3超时分支：无待烧写数据 或 重传计数达到上限
            if((0u == gs_stDowloadDataInfo.uDataLength) || (g_udsMsgRetransCount == 2))
            {
                // 会话切回默认模式，清空安全解锁权限
                SetCurrentSession(BASIC_SESSION);
                UDS_APP_AssignSecurityLevel(NO_SECURITY);
                // 重置下载地址、长度、块号等全局下载参数
                PrepareDLInformation();
                // 清空重传计数
                g_udsMsgRetransCount = 0;
            }
            else if((0u != gs_stDowloadDataInfo.uDataLength) && (g_udsMsgRetransCount < 2))
            {
                // 仍有固件待烧写且重传未达上限，刷新S3计时器，标记存在重传报文
                RestartS3Server();
                gs_udsMsgRetransFlag = 1;
            }
        }
#else
        // APP工程S3超时逻辑，无重传计数相关处理
        if (TRUE != UDS_APP_CheckS3ServerIsTimeout())
        {
            /* S3未超时，无操作 */
        }
        else
        {
            // S3超时切回默认会话，清空安全权限与下载参数
            SetCurrentSession(BASIC_SESSION);
            UDS_APP_AssignSecurityLevel(NO_SECURITY);
            PrepareDLInformation();
        }
#endif
    // 调用TP层接口读取一帧完整UDS报文
    if (TRUE != TP_DataTransferRetrieveFrame(&stLocalMsg.xUdsMsgId,
                                             &stLocalMsg.xDataMsgLength,
                                             stLocalMsg.aDataBuf))
    {
        // 无新报文，直接退出本次主循环
        return;
    }
    else
    {
	//APPDebugLog("RX ID 0x%x len %d\r\n",stLocalMsg.xUdsMsgId,stLocalMsg.xDataMsgLength);
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        // 标记周期收到有效UDS报文，复位跳转APP延时计时
        UDS_APP_MarkRxUdsMessage(TRUE);
        if(1 != gs_udsMsgRetransFlag)
        {
            /* 无重复报文标记，不更新重传计数 */
        }
        else
        {
            // 清除重传标记，重传计数+1并打印调试日志
            gs_udsMsgRetransFlag = 0;
            g_udsMsgRetransCount++;
            APPDebugLog("Retrans +1 !!!\r\n");
        }
#endif
        // 非默认会话下，收到任意有效报文刷新S3超时计时器
        if (TRUE == UDS_APP_CheckIfDefaultSession())
        {
            /* 默认会话不刷新S3计时器 */
        }
        else
        {
            RestartS3Server();
        }
         
        // 记录当前报文的寻址ID（物理/功能ID）
        UDS_APP_StoreRxIdType(stLocalMsg.xUdsMsgId);
    }
    // 获取全部注册服务数组首地址与服务总数
    pLocalUDSService = UDS_APP_ObtainUdsSrvConfig(&ucSupSerItem);
    // 报文首字节为诊断服务SID
    ucSerNum = stLocalMsg.aDataBuf[0u];
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        // 判断场景：存在重传计数且当前不是36分段服务 / 上一帧是36且当前是10会话切换，重置全部下载状态
        if(((ucSerNum != 0x36) && (g_udsMsgRetransCount != 0)) || ((gs_LastUDSServiceNum == 0x36) && (ucSerNum == 0x10)))
        {
            // 切回默认会话，清空安全、下载参数、重传计数
            SetCurrentSession(BASIC_SESSION);
            UDS_APP_AssignSecurityLevel(NO_SECURITY);
            PrepareDLInformation();
            g_udsMsgRetransCount = 0;
            APPDebugLog("Start a new trans!!!\r\n");
        }
        // 更新上一次服务SID缓存
        gs_LastUDSServiceNum = ucSerNum;
#endif
    // 遍历注册表匹配SID
    for (; (ucSerIdx < ucSupSerItem) && (NULL_PTR != pLocalUDSService); ucSerIdx++)
    {
        if (ucSerNum != pLocalUDSService[ucSerIdx].serviceId)
        {
            /* SID不匹配，继续遍历 */
        }
        else
        {
            // 匹配到对应服务SID
            bServiceFound = TRUE;
            // 校验当前报文寻址模式是否被该服务允许
            if (TRUE == UDS_APP_CheckRxIdValidity(pLocalUDSService[ucSerIdx].supportReqType))
            {
                /* 寻址合法，继续校验会话 */
            }
            else
            {
                // 寻址不支持，组装0x11否定响应并跳出遍历
                UDS_APP_AssignNegErrCode(stLocalMsg.aDataBuf[0u], E_NRC_SERVICE_NOT_SUPPORTED, &stLocalMsg);
                break;
            }
            // 校验当前会话模式是否允许调用该服务
            if (TRUE == UDS_APP_CheckSessionReq(pLocalUDSService[ucSerIdx].sessionMode))
            {
                /* 会话合法，继续校验安全等级 */
            }
            else
            {
                // 会话不支持，组装0x11否定响应并跳出遍历
                UDS_APP_AssignNegErrCode(stLocalMsg.aDataBuf[0u], E_NRC_SERVICE_NOT_SUPPORTED, &stLocalMsg);
                break;
            }
            // 校验当前解锁安全等级是否满足服务要求
            if (TRUE == UDS_APP_IsSecurityLevelValid(pLocalUDSService[ucSerIdx].requestLevel))
            {
                /* 安全权限合法，准备执行服务处理函数 */
            }
            else
            {
                // 安全等级不足，组装0x33否定响应并跳出遍历
                UDS_APP_AssignNegErrCode(stLocalMsg.aDataBuf[0u], E_NRC_SECURITY_ACCESS_DENIED, &stLocalMsg);
                break;
            }
            // 初始化报文发送完成回调为空
            stLocalMsg.pfTxMsgCb = NULL_PTR;
            // 判断服务处理函数指针是否有效
            if (NULL_PTR == pLocalUDSService[ucSerIdx].pfServiceHandler)
            {
                // 无处理函数，回复服务不支持否定响应
                UDS_APP_AssignNegErrCode(stLocalMsg.aDataBuf[0u], E_NRC_SERVICE_NOT_SUPPORTED, &stLocalMsg);
            }
            else
            {
                // 调用对应服务处理函数，填充正向/否定响应报文
                pLocalUDSService[ucSerIdx].pfServiceHandler(
                    (UDS_APP_localServiceType *)&pLocalUDSService[ucSerIdx],
                    &stLocalMsg);
            }
            // 匹配完成，跳出服务遍历循环
            break;
        }
    }
    if (TRUE == bServiceFound)
    {
        /* 匹配到服务，无需额外报错 */
    }
    else
    {
        // 注册表无对应SID，回复0x11服务不支持否定响应
        UDS_APP_AssignNegErrCode(stLocalMsg.aDataBuf[0u], E_NRC_SERVICE_NOT_SUPPORTED, &stLocalMsg);
    }
    // 报文长度为0代表无需回复，否则调用TP层发送响应报文
    if (0u == stLocalMsg.xDataMsgLength)
    {
        // 无响应报文，不发送
    }
    else
    {
        // 获取TP发送ID，将响应报文入队发送
        stLocalMsg.xUdsMsgId = TP_GetTransportTxID();
        (void)TP_DataTransferQueueFrame(stLocalMsg.xUdsMsgId,
                                     stLocalMsg.pfTxMsgCb,
                                     stLocalMsg.xDataMsgLength,
                                     stLocalMsg.aDataBuf);
    }
}
