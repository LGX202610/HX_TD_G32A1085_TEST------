/*******************************************************************************
* Project Name      : CAN/LIN Protocol Stack
* Platform          : Arm
* Revision Number   : V1.0
* Compiled Version  : G32A1xxx_01-June-25
*
* Copyright (C) 2025 Geehy Semiconductor
*
* You may not use this file except in compliance with the GEEHY COPYRIGHT NOTICE
* (GEEHY SOFTWARE PACKAGE LICENSE).
*
* The program is only for reference, which is distributed in the hope that it
* will be useful and instructional for customers to develop their software.
* Unless required by applicable law or agreed to in writing, the program is
* distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY
* KIND, either express or implied. See the GEEHY SOFTWARE PACKAGE LICENSE for
* the governing permissions and limitations under the License.
*
*******************************************************************************/
#ifndef FLS_APP_H_
#define FLS_APP_H_
#include "flash_hal.h"
#include "includes.h"
#include "CRC_hal.h"
/**************************************************************************
                    ENUMERATION DEFINITION
                    枚举类型定义
**************************************************************************/
/**
* @brief    three status of flash erase
*           Flash擦除操作三阶段状态枚举
*/
typedef enum
{
    BEGIN_STEP   = 0,  // 擦除启动阶段
    ERASING_STEP = 1,  // 擦除执行中阶段
    END_STEP     = 2   // 擦除完成结束阶段
} eraseFlashType;
/**
* @brief    four status of flash download
*           Flash程序下载四状态枚举
*/
typedef enum
{
    FLASH_DOWNLOAD_REQUEST       = 0,  // 发起下载请求状态
    FLASH_DOWNLOAD_TRANSFER      = 1,  // 数据传输下载状态
    FLASH_DOWNLOAD_EXIT_TRANSFER = 2,  // 退出数据传输阶段
    FLASH_DOWNLOAD_CHECKSUM      = 3   // 校验和校验阶段
} FlashDownloadType;
/**
* @brief    five status of flash operation
*           Flash底层运行五任务状态枚举
*/
typedef enum
{
    FLASH_IS_IN_IDLE     = 0,  // Flash空闲无操作
    FLASH_IS_IN_ERASING  = 1,  // Flash正在执行擦除
    FLASH_IS_IN_WRITING  = 2,  // Flash正在执行烧写
    FLASH_IS_IN_CHECK    = 3,  // Flash正在执行校验
    FLASH_IS_IN_WAITING  = 4   // Flash操作等待状态
} flashOperationType;
/**************************************************************************
                    MACRO DEFINITION
                    宏定义区
**************************************************************************/
/** the longest byte to check every time */
/** 单次CRC校验最大数据长度（字节） */
#ifndef LONGEST_LENGTH_TO_CHECK
#define LONGEST_LENGTH_TO_CHECK (100000u)
#endif
/** the longest save number */
/** 下载信息备份缓存最大存储份数 */
#define LONGEST_SAVE_NUMBER (3u)
/** flash operate size */
/** Flash单次操作字节大小（烧写/擦除块尺寸） */
#define FLASH_OPERATE_SIZE_BYTE (128u)
/** useless uds service id */
/** 无效UDS诊断服务ID标识 */
#define UDS_SERVICES_USELESS_ID (0xFFu)
/**************************************************************************
                    OTHER TYPE DEFINITION
                    其他自定义数据类型定义
**************************************************************************/
// 操作完成响应回调函数类型：入参为状态码
typedef void (*ResponseFuncType)(uint8);
// 延时请求回调函数类型：入参1服务ID、入参2子回调函数
typedef void (*RequestOtherTimeFuncType)(uint8, void (*)(uint8));
/**
* @brief    flash receive content parameter
*           LIN TP协议下Flash接收数据段参数结构体
*/
#ifdef ALLOW_LIN_TP
typedef struct
{
    uint32 flashRecContentBeginAddr; // 接收数据对应的Flash起始地址
    uint32 flashRecContentSize;      // 当前接收数据总长度
} flashRecContentParaType;
#endif
/**
* @brief    app information parameter
*           应用程序版本/烧写状态信息结构体
*/
typedef struct
{
    uint8 flashWriteOkFlag;         // 应用烧写完成标志位
    uint8 flashEraseOkFlag;         // 应用擦除完成标志位
    uint8 flashStructOkFlag;        // 本App信息结构体校验合法标志
    uint8 appNumber;                // 当前应用编号
    uint8 PrintBufferForFinger[17u];// 固件指纹校验缓存数组（17字节）
    uint32 appBeginAddrSize;        // 应用程序起始地址+分区总长度
    uint32 appPosition;             // 应用程序存储分区偏移地址
#ifdef APP_SINGLE_UPDATA
    uint32 appCrc;                  // 单包升级：应用程序CRC校验值
    uint32 appLen;                  // 单包升级：应用程序总字节长度
#endif
#ifdef ALLOW_LIN_TP
    // LIN协议下多份接收段参数备份缓存，最大LONGEST_SAVE_NUMBER份
    flashRecContentParaType flashRecContentParaBuffer[LONGEST_SAVE_NUMBER];
#endif
    uint32 crc;                     // 当前App信息结构体自身CRC校验码
} appInfoType;
/**
* @brief    flash download status parameter
*           Flash整机下载流程全局状态管理结构体
*/
typedef struct
{
    uint8 needToWriteBuffer[MAX_MOD_DATA_LEN]; // 待写入Flash数据缓存区
    uint8 VerifyFingerPrintWrittenFlag;        // 固件指纹已写入校验标志
    uint8 VerifyFDDownloadedFlag;              // 完整固件下载完成标志
    uint8 flashErrorStatus;                    // Flash操作错误状态码
#ifdef ALLOW_LIN_TP
    uint8 appErasedFlag;                       // LIN模式下应用分区擦除完成标志
#endif
    uint8 udsSerIDrequested;                   // 当前激活的UDS诊断服务ID
    uint32 startAddr;                          // 本次烧写目标Flash起始地址
    uint32 length;                             // 本次待烧写数据长度
    uint32 flashRecContentBeginAddr;            // 当前接收数据段Flash起始地址
    uint32 flashRecContentSize;                 // 当前接收数据段总长度
    uint32 receivedCRC;                        // 上位机下发的固件CRC校验值
#ifdef ALLOW_LIN_TP
    uint32 computeLinCRC;                      // LIN协议本地实时计算CRC值
#endif
    uint32 recContentSize;                     // 累计已接收固件字节总数
    FlashDownloadType flashDownloadPara;        // 当前下载流程阶段状态
    flashOperationType flashCurTask;            // Flash底层当前执行任务
#ifdef ALLOW_LIN_TP
    flashOperationType newIntterpTask;          // LIN中断触发的临时Flash任务
#endif
    ResponseFuncType taskEndFunc;               // Flash任务完成回调函数指针
    RequestOtherTimeFuncType requestOtherTimePara; // 延时操作回调函数指针
    appInfoType *appStatusInFlash;              // 指向Flash内存储的App信息结构体
    Flash_OperationAPIType FlashOperationAPI;    // Flash底层硬件操作API接口集合
} flashDLStatusType;
/**************************************************************************
                    FUNCTION DECLARATION
                    外部函数声明
**************************************************************************/
/**
 * @brief 记录固件指纹校验数据
 * @param addr 指纹数据源地址指针
 * @param space 指纹数据字节长度
 */
void RecordFingerPrint(const uint8 *addr, const uint8 space);

/**
 * @brief 设置下一阶段下载流程状态
 * @param num 目标下载状态枚举值
 */
void SetNextDLPara(const FlashDownloadType num);

/**
 * @brief 校验Flash中存储的App信息合法性
 * @retval 校验结果状态码
 */
uint8 VerifyAppInfoFromFlsRationality(void);

/**
 * @brief 初始化下载前置信息，准备烧写环境
 */
void PrepareDLInformation(void);

#ifdef APP_SINGLE_UPDATA
/**
 * @brief 校验单包升级备份固件完整性
 * @retval true=校验通过 false=校验失败
 */
boolean VerifyAppBackupResult(void);
/**
 * @brief 将当前运行App备份至备份分区Flash
 * @retval true=备份成功 false=备份失败
 */
boolean AppBackupInFls(void);
#endif

/**
 * @brief 记录本次下载的起始地址与数据长度
 * @param beginAddress 烧写起始Flash地址
 * @param space 待烧写数据总字节长度
 */
void RecordDLInformation(const uint32 beginAddress, const uint32 space);
void RecordEraseMemoryRange(const uint32 startAddr, const uint32 length);
boolean GainEraseMemoryRange(uint32 *startAddr, uint32 *length);
void ClearEraseMemoryRange(void);

/**
 * @brief 内存中模拟擦除操作（RAM缓存清空）
 */
void RamFDErase(void);

/**
 * @brief Flash后台任务主运行逻辑，轮询处理擦除/烧写/校验
 */
void FlashTaskRunLogic(void);

/**
 * @brief 获取Flash内最新版本App编号ID
 * @retval AppIdType 最新应用ID
 */
AppIdType GainNewestAPPId(void);

/**
 * @brief 保存上位机下发的固件CRC校验值
 * @param num CRC校验码数值
 */
void RecordRecCrcValue(uint32 num);

/**
 * @brief 整机Flash应用分区合法性整体校验
 * @retval 校验状态码
 */
uint8 FlashAppRationality(void);

/**
 * @brief 擦除、初始化App存储Flash分区
 */
void PrepareFlsForApp(void);

/**
 * @brief 获取当前应用程序存储分区起始地址
 * @retval 32位Flash物理地址
 */
uint32 GainAppPositionAddr(void);

#ifdef APP_BICLE_UPDATA
/**
 * @brief 对比两个App信息结构体，返回更新版本ID（双分区升级专用）
 * @param AApp 分区A应用信息指针
 * @param BApp 分区B应用信息指针
 * @retval 较新App的ID
 */
static AppIdType VerifyNewestAppInfo(const appInfoType *AApp, const appInfoType *BApp);
/**
 * @brief 对比两个App实体，判定最新版本（双分区升级专用）
 * @param AApp 分区A应用信息指针
 * @param BApp 分区B应用信息指针
 * @retval 较新App的ID
 */
static AppIdType VerifyNewestApp(const appInfoType *AApp, const appInfoType *BApp);
#endif

/**
 * @brief 设置当前Flash运行任务参数、回调函数、服务ID
 * @param task Flash底层任务状态枚举
 * @param func1 任务完成响应回调
 * @param id 当前UDS服务ID
 * @param func2 延时请求回调函数
 */
void SetCurFlsTaskPara(const flashOperationType task,
                       const ResponseFuncType func1,
                       const uint8 id,
                       const RequestOtherTimeFuncType func2);

/**
 * @brief 获取上一版本App编号ID
 * @param flag 查询控制标志位
 * @retval 旧版本AppID
 */
AppIdType GainPreviousAppId(boolean flag);

/**
 * @brief 执行Flash分区烧写：将缓存数据写入指定Flash地址
 * @param addr Flash目标起始地址
 * @param des 待烧写数据缓存指针
 * @param space 烧写字节长度
 * @retval 操作执行状态码
 */
uint8 ProcessProgramRegion(const uint32 addr, const uint8 *des, const uint32 space);

/**
 * @brief 将当前App状态信息结构体写入Flash存储区
 * @retval 写入操作状态码
 */
uint8 WriteAppInfoToFls(void);

/**
 * @brief 0x31 0x0203 失败：作废信息头并清标志，禁止随后 FF01 标有效、禁止跳 APP
 */
void MarkAppDownloadIntegrityFailed(void);

/**
 * @brief 0x10 02 进编程会话：清本周期 RAM 标志（完整性/已写 APP/擦写完成）
 */
void ResetProgrammingCycleFlags(void);

/**
 * @brief 本周期 0x31 FF00 是否已擦除成功
 */
uint8 IsFlashEraseCompleted(void);

/**
 * @brief 0x31 0x0203 通过
 */
void MarkAppDownloadIntegrityPassed(void);

/**
 * @brief 本次 0x0203 是否已通过
 */
uint8 IsAppDownloadIntegrityPassed(void);

/**
 * @brief APP 的 0x37：已擦且已写过 APP 才置 flashWriteOkFlag。Driver 的 37 直接返回 TRUE
 */
uint8 FinalizeAppDownloadTransfer(void);

/**
 * @brief 新 Boot 刷写成功后作废过渡程序自己的 APP 信息头，复位后新 Boot 不再跳过渡区
 */
uint8 InvalidateAppHeaderAfterBootOk(void);

/**
 * @brief Reset 入口是否可跳：非 0、非全 F、奇数(Thumb)、落在 APP 代码区
 */
uint8 IsAppResetHandlerValid(uint32 resetAddr);

/**
 * @brief 获取当前下载流程所处阶段状态
 * @retval FlashDownloadType 当前下载状态枚举
 */
FlashDownloadType GainCurDLPara(void);

/**
 * @brief 获取已下载分段数量 -lu
 * @return 分段数量
 */
uint32_t UDS_APP_GetDownloadSegmentCount(void);

/**
 * @brief 获取指定分段的起始地址和长度
 * @param index 分段索引
 * @param pStartAddr 输出：起始地址
 * @param pLength 输出：长度
 * @return TRUE=成功，FALSE=索引无效
 */
boolean UDS_APP_GetDownloadSegmentInfo(uint32_t index, uint32_t* pStartAddr, uint32_t* pLength);


#endif

#ifdef ALLOW_LIN_TP
// LIN TP协议：全局下载状态备份缓存数组，支持多份存储
extern flashDLStatusType gs_stFlashDownloadInfobackp[LONGEST_SAVE_NUMBER];
#endif
