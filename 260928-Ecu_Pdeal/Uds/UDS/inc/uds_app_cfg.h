#ifndef UDS_APP_CFG_H_
#define UDS_APP_CFG_H_
#include "includes.h"
#include "TP.h"


/* Loader程序类型定义，固定0x0F代表Bootloader固件标识 */
#define LOADER_CODE_TYPE (0x0Fu)
/* Bootloader软件版本号数组：类型+主版本+次版本+修订号 */
#define LOADER_SOFT_VERSION {LOADER_CODE_TYPE, 0x01, 0x0, 0x00}
/* Bootloader硬件版本号数组：类型+主版本+次版本+修订号 */
#define LOADER_HARD_VERSION {LOADER_CODE_TYPE, 0x01, 0x0, 0x00}

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/* 请求下载服务 内存起始地址长度，4字节32位地址 */
#define DOWNLOAD_ADDR_SIZE (4u)
/* 请求下载服务 单次数据长度字段字节数，4字节长度 */
#define DOWNLOAD_DATA_SIZE (4u)
#endif
/* 无效请求ID标记，未识别物理/功能寻址时赋值 */
#define FAILURE_REQUEST_ID (0u)
/* 位0置1：支持物理寻址诊断请求 */
#define ALLOW_PHYSICAL_ID (1u << 0u)
/* 位1置1：支持功能寻址诊断请求 */
#define ALLOW_FUNCTION_ID (1u << 1u)
/* S3会话超时水位百分比，到达该值可提前处理超时预警逻辑 */
#ifndef S3_TIMER_WATERMARK_PERCENTAGE
#define S3_TIMER_WATERMARK_PERCENTAGE (90u)
#endif
/* 配置合法性校验：水位值必须大于0且小于100 */
#if (S3_TIMER_WATERMARK_PERCENTAGE <= 0) || (S3_TIMER_WATERMARK_PERCENTAGE >= 100)
#error "S3_TIMER_WATERMARK_PERCENTAGE should config (0, 100]"
#endif
/* 否定响应SID固定值0x7F，所有错误回复报文首字节 */
#define NEGATIVE_RSP_ID (0x7Fu)
/* 默认会话标识位，上电初始会话模式 */
#define BASIC_SESSION (1u << 0u)
/* 编程会话标识位，固件刷写专用会话 */
#define PROGRAM_SESS (1u << 1u)
/* 扩展诊断会话标识位，普通深度诊断使用 */
#define EXTENDED_SESS (1u << 2u)
/* 无安全解锁等级，默认上电状态 */
#define NO_SECURITY (1u << 0u)
/* 安全等级1：解锁基础刷写功能，继承无安全权限 */
#define SECURITY_LEVEL_1 ((1 << 1u) | NO_SECURITY)
/* 安全等级2：更高权限，继承等级1所有权限 */
#define SECURITY_LEVEL_2 ((1u << 2u) | SECURITY_LEVEL_1)
/* 时间转换宏：将毫秒时间转换为UDS周期计数（毫秒/调度周期） */
#define UDS_APP_TIME_TO_COUNT(xTime) ((xTime) / g_udsTimeConfigTable.cPeriod)
/* 否定响应NRC错误码定义 */
/* 0x11：当前会话/寻址模式不支持该诊断服务 */
#define E_NRC_SERVICE_NOT_SUPPORTED               0x11
/* 0x12：服务子功能不支持 */
#define E_NRC_SUBFUNCTION_NOT_SUPPORTED           0x12
/* 0x13：报文长度、数据格式不符合规范 */
#define E_NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT    0x13
/* 0x22：当前硬件/软件条件不满足执行该服务 */
#define E_NRC_CONDITIONS_NOT_CORRECT              0x22
/* 0x24：诊断服务调用顺序错误（如未34直接发36） */
#define E_NRC_REQUEST_SEQUENCE_ERROR              0x24
/* 0x31：请求地址、长度超出合法范围 */
#define E_NRC_REQUEST_OUT_OF_RANGE                0x31
/* 0x33：当前解锁等级不满足该服务所需安全访问等级 */
#define E_NRC_SECURITY_ACCESS_DENIED              0x33
/* 0x35：安全校验密钥错误，解锁失败 */
#define E_NRC_INVALID_KEY                         0x35
/* 0x72：一般编程错误 */
#define E_NRC_GENERAL_PROGRAMMING_FAILURE         0x72
/* 0x78：服务繁忙，需要客户端延长超时等待（长耗时Flash擦写/校验） */
#define E_NRC_SERVICE_BUSY                        0x78
/* 本地计时变量类型，16位无符号整型存储计数值 */
typedef uint16 tLocalTime;
/**
* @brief    UDS本地收发报文存储结构体
*/
typedef struct
{
    TP_UdsIdType xUdsMsgId;  //!< CAN TP层报文ID（物理/功能请求ID、回复ID）
    TP_LengthType xDataMsgLength;  //!< 当前报文有效数据长度
    uint8 aDataBuf[UDS_MAX_PDU_LEN];  //!< UDS报文数据缓冲区，容纳0x31 0x6000的1322字节加密文件
    void (*pfTxMsgCb)(uint8);  //!< 报文发送完成回调函数指针（长耗时操作异步处理）
} UDS_APP_tLocalAppMsgType;
/**
* @brief    UDS诊断服务配置信息结构体，注册所有支持服务的权限与处理函数
*/
typedef struct UDS_APP_ServiceInfoType
{
    uint8 serviceId;  //!< 诊断服务SID，如0x10会话控制、0x3E测试仪在线、0x34请求下载
    uint8 sessionMode;  //!< 允许调用该服务的会话模式掩码（默认/编程/扩展）
    uint8 supportReqType;  //!< 支持的寻址类型掩码（物理/功能寻址）
    uint8 requestLevel;  //!< 调用该服务需要的安全解锁等级
    void (*pfServiceHandler)(struct UDS_APP_ServiceInfoType *, UDS_APP_tLocalAppMsgType *);  //!< 服务处理函数入口指针
} UDS_APP_localServiceType;
#ifdef UDS_PROJECT_FOR_BOOTLOADER
/**
* @brief    固件下载参数结构体，存储34服务下发的起始地址与总数据长度
*/
typedef struct
{
    uint32 uStartAddress;  //!< 待烧写Flash起始物理地址
    uint32 uDataLength;  //!< 待烧写固件总字节长度
} UDS_APP_downloadDataStructType;
/**
* @brief    例程控制0x31支持的操作枚举，区分擦除、校验、备份拷贝等任务
*/
typedef enum
{
    ERASE_MEMORY_CONTROL,      // 0xFF00 擦除内存
    CHECKSUM_CONTROL,          // 0x0202 CRC校验
    DEPENDENCY_CONTROL,        // 0xFF01 依赖性检查
    CHECK_AUTHENTICITY,        // ★ 新增：0x6000 校验文件合法性
    CHECK_VERSION,             // ★ 新增：0x6001 版本校验
    CHECK_DATA_INTEGRITY,      // ★ 新增：0x0203 下载数据完整性检查
#ifdef APP_SINGLE_UPDATA
    ERASE_BACKUP_CONTROL,  //!< 备份分区Flash擦除例程（双分区升级）
    COPY_BACKUP_CONTROL  //!< 主分区APP拷贝至备份分区例程
#endif
} UDS_APP_checkRoutineCtrlType;

/**
* @brief    全局下载参数结构体外部声明，存储34下发的刷写地址长度
*/
extern UDS_APP_downloadDataStructType gs_stDowloadDataInfo;
#endif
/**
* @brief    UDS时间配置表结构体，固定编译期配置S3、安全锁定、调度周期参数
*/
typedef struct
{
    uint8 cPeriod;  //!< UDS主函数调度周期（单位ms）
    uint8 cSecRequestCount;  //!< 安全访问密码错误最大重试次数，超限锁定
    tLocalTime cLockTime;  //!< 密码错误后安全锁定倒计时计数
    tLocalTime cS3Time;  //!< S3会话最大超时总计数值
    tLocalTime cP2Time;  //!< P2服务端响应最大超时（单位ms）
    tLocalTime cP2StarTime;  //!< P2*扩展响应超时（0x78响应挂起后，单位ms）
} UDS_APP_tTimeInfoStructType;
/**
* @brief    UDS运行时状态结构体，实时记录当前会话、寻址、安全等级、计时器
*/
typedef struct
{
    uint8 iSessionMode;  //!< 当前激活会话模式掩码
    uint8 iReqIdMode;  //!< 当前报文使用的寻址类型（物理/功能/无效）
    uint8 iSecLevel;  //!< 当前解锁安全等级
    tLocalTime iS3Time;  //!< S3会话剩余超时计数
    tLocalTime iSecLockTime;  //!< 安全锁定剩余倒计时计数
    tLocalTime iP2Time;  //!< P2服务端响应剩余倒计时计数
    tLocalTime iP2StarTime;  //!< P2*扩展响应剩余倒计时计数
    uint8 iP2StarPending;  //!< P2*响应挂起标记（已发0x78等待最终响应）
} UDS_APP_tInfoStructType;
/**
* @brief    UDS固定时间配置表全局常量外部声明
*/
extern const UDS_APP_tTimeInfoStructType g_udsTimeConfigTable;
#ifdef UDS_PROJECT_FOR_BOOTLOADER
#ifdef ALLOW_DELAY_TIME
/**
* @brief    Boot跳转APP延时控制结构体，无诊断报文后倒计时跳转应用
*/
typedef struct
{
    boolean isReceivedMsg;  //!< 周期内是否收到有效UDS报文标记
    uint32 appDelay;  //!< 跳转APP剩余延时计数值
} UDS_APP_DelayTimeInfoType;
#endif
#endif
#ifdef UDS_PROJECT_FOR_BOOTLOADER
// 内存拷贝内部工具函数：源缓冲区拷贝指定长度至目标缓冲区
static void UDS_APP_DuplicateMemory(const void *pSrc, const uint8 uLen, void *pDst);
// 内存填充工具函数：用指定字节值填充缓冲区
static void UDS_APP_FillMemory(const uint8 uVal, const uint16 uSize, void *pBuf);
// 校验0x31例程控制ID是否匹配当前操作类型
static uint8 UDS_APP_ValidateRoutineCtrlId(const UDS_APP_checkRoutineCtrlType eRoutineCtrl,
                                           const UDS_APP_tLocalAppMsgType *pMsg);
// 校验当前0x31报文是否为Flash擦除指令
static uint8 UDS_APP_CheckErasingMemory(const UDS_APP_tLocalAppMsgType *pMsg);
// 解析0x31 FF00的ALFI/地址/长度，校验APP区间并对齐扇区；返回0成功，否则为NRC
static uint8 UDS_APP_ParseAndCheckEraseRange(const UDS_APP_tLocalAppMsgType *pMsg,
                                             uint32 *pAlignedStart,
                                             uint32 *pAlignedLen);
#ifdef APP_SINGLE_UPDATA
// 校验0x31报文是否为备份分区擦除指令
static uint8 UDS_APP_CheckErasingBackupMemory(const UDS_APP_tLocalAppMsgType *pMsg);
// 校验0x31报文是否为主APP拷贝至备份分区指令
static uint8 UDS_APP_CheckCopyBackupMemory(const UDS_APP_tLocalAppMsgType *pMsg);
#endif
// 校验0x31报文是否为固件CRC校验指令
static uint8 UDS_APP_CheckSummationRoutine(const UDS_APP_tLocalAppMsgType *pMsg);
// 校验0x31报文是否为固件依赖版本校验指令
static uint8 UDS_APP_CheckProgramDependency(const UDS_APP_tLocalAppMsgType *pMsg);
// 校验0x2E写入指纹ID报文合法性
static uint8 UDS_APP_ValidateWriteFingerprint(const UDS_APP_tLocalAppMsgType *pMsg);
// 校验34服务下发的起始烧写地址是否合法
static uint8 UDS_APP_CheckDownloadAddress(const uint32 uAddr);
// 校验34服务下发的总烧写长度是否合法
static uint8 UDS_APP_CheckDownloadLength(const uint32 uLen);
// 安全访问0x27校验客户端上传密钥与种子匹配
static uint8 UDS_APP_ValidateKey(const uint8 *pKeyIn,
                                 const uint8 *pSeed,
                                 const uint8 keyLength);
// CRC校验完成异步回调，回复0x31正向响应
static void UDS_APP_PerformChecksum(uint8 bStatus);
// Flash擦除完成异步回调，回复0x31正向/否定响应
static void UDS_APP_PerformFlashErase(uint8 state);
#ifdef APP_SINGLE_UPDATA
// 备份分区擦除异步回调
static void UDS_APP_PerformBackupFlashErase(uint8 state);
#endif
// 执行固件依赖合法性校验，返回校验结果
static uint8 UDS_APP_DoCheckProgramDependency(void);
// 组装并发送CRC校验结果响应报文
static void UDS_APP_RespondCksumResult(uint8 bFlag);
// 组装并发送Flash擦除完成响应报文
static void UDS_APP_EraseFlashAck(uint8 bFlag);
// 触发长耗时操作E_NRC_SERVICE_BUSY否定响应，注册异步完成回调
static void UDS_APP_TriggerExtendedTime(const uint8 uService, void (*pfCB)(uint8));
#endif
// 执行MCU软件复位，发送复位响应后触发重启
static void UDS_APP_PerformMcuReset(uint8 uStatus);
// 0x10会话控制服务处理函数，切换默认/编程/扩展会话
static void UDS_APP_ShiftSession(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg);
// 0x22 读取数据信息，硬件 软件版本号
static void UDS_APP_ReadDataByIdentifier(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg);
// 0x85 DTC故障码设置控制服务处理函数
static void UDS_APP_CtrlDtcSetting(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg);
// 0x28通信模式控制服务处理函数（启用/禁用收发）
static void UDS_APP_CommunicationSetting(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg);
/* 0x14 清除DTC信息服务处理函数 */
static void UDS_APP_ClearDTCInformation(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg);
#ifdef UDS_PROJECT_FOR_APP
/* 0x19 读DTC信息：01 条数、02 当前列表、0A 支持的码表 */
static void UDS_APP_ReadDTCInformation(struct UDS_APP_ServiceInfoType *pLocalSrv, UDS_APP_tLocalAppMsgType *pLocalMsg);
#endif
#ifdef UDS_PROJECT_FOR_APP
static void UDS_APP_SecAccessFunc(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
static void UDS_APP_WriteVinIdent(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
static void UDS_APP_ObdRequestVehicleInfo(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
#endif
#ifdef UDS_PROJECT_FOR_BOOTLOADER
// 0x27安全解锁服务处理函数，生成种子/校验密钥解锁权限
static void UDS_APP_SecAccessFunc(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
// 0x2E通过标识符写入数据服务，写入升级指纹信息
static void UDS_APP_WriteDataIdent(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
// 0x34请求下载服务，解析烧写起始地址与总长度
static void UDS_APP_ReqDownload(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
// 0x36传输数据服务，分段接收固件并写入Flash
static void UDS_APP_TransferProcess(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
// 0x37退出传输服务，结束分段下载流程，进入校验阶段
static void UDS_APP_FinalizeTransfer(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
// 0x31例程控制服务处理函数，分发擦除/校验/备份拷贝任务
static void UDS_APP_HandleRoutineOperation(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
#endif
// 0x11 ECU复位：boot 里标记升级成功；APP 里回 51 01 后复位
static void UDS_APP_PerformEcuReset(struct UDS_APP_ServiceInfoType *pstSrv, UDS_APP_tLocalAppMsgType *pstMsg);
// 0x3E测试仪在线服务，刷新S3会话超时计时器
static void UDS_APP_HandleTesterPresence(struct UDS_APP_ServiceInfoType *pSrv, UDS_APP_tLocalAppMsgType *pMsg);
// 0x36分段烧写完成异步响应发送函数
void UDS_APP_WriteFlashAck(uint8 bFlag);
// 设置全局当前会话模式
void SetCurrentSession(const uint8 i_SerSessionMode);
uint8 UDS_APP_GetF186Session(void); /* 0xF186：01默认/02编程/03扩展 */
// 判断当前是否为默认会话，返回TRUE/FALSE
uint8 UDS_APP_CheckIfDefaultSession(void);
// 判断S3会话是否已经超时
uint8 UDS_APP_CheckS3ServerIsTimeout(void);
// 重置S3会话超时计时器为配置最大值
void RestartS3Server(void);
// 启动P2服务端响应计时器（收到0x10诊断请求时调用）
void UDS_APP_StartP2ServerTimer(void);
// 启动P2*扩展响应计时器（发送0x78响应挂起时调用）
void UDS_APP_StartP2StarServerTimer(void);
// 停止P2*计时器并清除响应挂起标记（最终响应完成后调用）
void UDS_APP_StopP2StarServerTimer(void);
// 判断P2服务端响应是否已超时
uint8 UDS_APP_CheckP2ServerIsTimeout(void);
// 判断P2*扩展响应是否已超时
uint8 UDS_APP_CheckP2StarServerIsTimeout(void);
// 判断当前是否处于P2*响应挂起状态
uint8 UDS_APP_IsP2StarPending(void);
// 校验当前会话是否允许执行目标服务
uint8 UDS_APP_CheckSessionReq(uint8 uSesMode);
// 根据接收报文ID记录当前寻址类型（物理/功能）
void UDS_APP_StoreRxIdType(const uint32 uReqID);
// 校验当前寻址模式是否允许执行目标服务
uint8 UDS_APP_CheckRxIdValidity(uint8 uIdMode);
// 设置全局当前安全解锁等级
void UDS_APP_AssignSecurityLevel(const uint8 uSecLevel);
// 校验当前安全等级是否满足服务调用要求
uint8 UDS_APP_IsSecurityLevelValid(uint8 uSecLev);
// 获取全部注册UDS服务配置数组首地址与服务总数
UDS_APP_localServiceType *UDS_APP_ObtainUdsSrvConfig(uint8 *pServiceItem);
#ifdef UDS_PROJECT_FOR_BOOTLOADER
// 标记周期内是否收到UDS报文，用于跳转APP延时逻辑
void UDS_APP_MarkRxUdsMessage(const boolean bSetVal);
// 查询周期内是否收到有效UDS报文
boolean UDS_APP_HasRxUdsMessage(void);
#endif
#ifdef APP_BICLE_UPDATA
// 获取全局擦除Flash起始地址（多分区升级使用）
uint32_t UDS_APP_RetrieveEraseStartAddress(void);
#endif
// 组装否定响应报文，填充0x7F+SID+NRC错误码
void UDS_APP_AssignNegErrCode(const uint8 uServNum,
                              const uint8 uErrCode,
                              UDS_APP_tLocalAppMsgType *pMsg);
// 系统1ms周期滴答处理，递减S3计时器、安全锁定计时器、跳转延时计时器
void UDS_APP_HandleSystemTicks(void);
// 计算S3超时水位阈值毫秒值（S3总时长*90%）
uint32 UDS_GetUDSS3WatermarkTimerMs(void);
// APP主动发送诊断报文请求进入Bootloader模式
boolean UDS_APP_SendMsgToHost(void);

/* 0x28 普通通信：1=允许收发，0=关闭。上电默认允许。
 * 只记录请求，不开关报文。应用层读这个标志去处理开关报文。 */
extern volatile uint8 g_udsNormalCommEnable;

// 0x3E新增函数声明（如果需要在外部调用）
static uint8 UDS_APP_CheckAuthenticity(const UDS_APP_tLocalAppMsgType *pMsg);
static uint8 UDS_APP_CheckVersion(const UDS_APP_tLocalAppMsgType *pMsg);
static uint8 UDS_APP_CheckDataIntegrity(const UDS_APP_tLocalAppMsgType *pMsg);

/* 0x2E校验函数声明（如果需要外部调用） */
static uint8 UDS_APP_CheckWriteToolSerial(const UDS_APP_tLocalAppMsgType *pMsg);
static uint8 UDS_APP_CheckWriteReprogDate(const UDS_APP_tLocalAppMsgType *pMsg);

/**
 * @brief 版本号比对
 * @param pRemoteVersion 上位机发来的版本信息（10字节）
 * @return 0=允许刷写，-1=版本过低拒绝
 */
int UDS_APP_CompareVersion();

#endif
