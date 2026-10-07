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
*******************************************************************************/
#include "fls_app.h"         // Flash应用层接口
#include "watchdog_hal.h"    // 看门狗硬件驱动，擦写时分段喂狗防复位
#include "uds_app.h"         // UDS诊断业务层交互接口
#include "flash_hal_Cfg.h"   // Flash底层驱动配置文件
#include "uds_dtc_nvm.h"     // F1ED 刷写失败原因
/**************************************************************************
                    全局变量定义区
**************************************************************************/
#ifdef APP_BICLE_UPDATA
static uint32 doubleRenewDLAppInfoAddr = 0; // 双分区升级时APP头部信息起始地址缓存
#endif
// 主Flash下载全局状态，4字节对齐防止硬件访问异常
flashDLStatusType flashDLStatusSingle __attribute__((aligned(4)));
// 多段下载状态缓存数组，最大保存3段固件参数
static flashDLStatusType flashDLStatusBuffer[LONGEST_SAVE_NUMBER];
static uint8 reqTimeFlag = 0xFFu; // 繁忙延长超时标记 1=正常等待 2=操作失败重置
static eraseFlashType eraseFlashStatus = BEGIN_STEP; // 全局擦除阶段状态机
static uint8_t flashDLNeedOtherTimeSignal = 0; // LIN多段下载分段计数标记
static appInfoType appInfoVar; // 当前操作分区头部信息本地缓存
/* 本编程周期 RAM 标志：10 02 清零，不上电保存 */
static uint8 s_dataIntegrityPassed = FALSE;   // 本次 0x0203 已通过
static uint8 s_appDataWrittenThisCycle = FALSE; // 本次已向 APP PFlash 写入过
#ifdef ALLOW_LIN_TP
/** LIN CRC分段校验三阶段状态枚举 */
typedef enum
{
    CHECK_BEGIN_STEP = 0u, // CRC初始化阶段
    CHECKING_STEP    = 1u, // CRC分段计算阶段
    CHECK_END_STEP   = 2u  // CRC计算完成，对比校验
}checkCrcType;
static checkCrcType checkCrcStatus = CHECK_BEGIN_STEP; // LIN CRC全局状态
// LIN下载备份状态数组
flashDLStatusType flashDLStatusbackp[LONGEST_SAVE_NUMBER];
#endif
boolean backupEraseFlag = FALSE; // 备份分区擦除标记 TRUE=擦备份区 FALSE=擦主分区
static uint32 gs_routineEraseStart = 0u;   // 0x31 FF00 对齐后的擦除起始地址
static uint32 gs_routineEraseLen = 0u;     // 0x31 FF00 对齐后的擦除长度
static uint8 gs_routineEraseValid = FALSE; // TRUE=按例程下发范围擦，FALSE=擦整分区
static Flash_blockInformationType gs_reqEraseBlock; // 本次例程擦除使用的临时块描述
/**************************************************************************
                    功能宏定义区
**************************************************************************/
// 将计算出的CRC写入本地APP信息结构体
#define SetAppCrc(num) do{appInfoVar.crc = num;}while(0u)
    
#ifdef ALLOW_LIN_TP
// LIN模式计算APP头部结构体CRC（排除末尾4字节CRC自身）
    #define ProduceAppCrc(num) CRC_HAL_ComputeSingleCRC((uint8 *)&appInfoVar, sizeof(appInfoVar) - 0x04u, num)
#else
// CAN模式软件CRC计算APP头部信息
    #define ProduceAppCrc(num) CRC_HAL_CreatSw((uint8 *)&appInfoVar, sizeof(appInfoVar) - 0x04u, num)
#endif
// 计算并自动更新appInfoVar内CRC字段
#define ProduceAndSetAppCrc(num) do{ProduceAppCrc(num); SetAppCrc(*num);}while(0u)
/**************************************************************************
                    内部静态函数前置声明
**************************************************************************/
static uint8 ProcessEraseFlash(boolean *completeFlag); // Flash分段擦除主逻辑
static uint8 RecordData(const uint8 *addr, const uint8 space); // 缓存分段固件数据
static boolean CheckFlsDrvSwData(void); // 校验目标地址是否在合法APP分区内
static uint8 IsCurrentDownloadFlashDriver(void); // 本次 34 起始地址是否为 RAM Flash Driver
static uint8 InvalidateAppInfoPage(uint32 appBeginAddr); // 擦1页作废APP信息头
#ifdef UDS_PROJECT_FOR_BOOTLOADER
#ifdef APP_BICLE_UPDATA
static AppIdType GainDLApp(void); // 双分区模式获取当前待刷写分区ID
#endif
#endif
#ifdef ALLOW_LIN_TP
static boolean VerifyRecCrcRationality(void); // LIN模式对比本地CRC与诊断下发CRC
static boolean ProcessLinTpCheckSum(boolean *completeFlag); // LIN分段CRC计算逻辑
#endif
static void SetRequestLongerTimerFlag(uint8 flag); // 设置繁忙超时标记
static uint8 ProcessVerifyCrc(boolean *completeFlag); // 固件整体CRC校验入口
static uint8 ProcessWriteFlash(boolean *completeFlag); // 分段写入Flash主逻辑
static void RenewDLFlsInformation(uint32_t count, uint32_t beginAddress, uint32_t space); // 保存多段下载参数
/**************************************************************************
                    内部静态函数实现区
**************************************************************************/
/**
 * @brief Flash分段擦除处理函数，分时执行防止单次耗时过长触发S3超时
 * @param completeFlag 出参 TRUE=本次分段全部擦除完成 FALSE=还有扇区待擦
 * @return uint8 TRUE=擦除无故障 FALSE=擦除扇区失败
 */
static uint8 ProcessEraseFlash(boolean *completeFlag)
{
    static uint8 ret = TRUE;                // 擦除操作结果缓存
    static uint32 appFlashTime = 0u;        // 单次循环可擦除扇区计时上限
    static uint32 eraseSectorNumber = 0u;   // 当前已擦除扇区总数
    static AppIdType appConclude = APP_USELESS_ID; // 当前待擦除分区ID
    static Flash_blockInformationType *bloInfo = NULL_PTR; // 分区扇区配置信息指针
    uint32 space = 0u;                      // 单块分区总字节长度
    uint32 sectorId = 0u;                   // 当前块包含扇区数量
    uint32 earseBeginAddr = 0u;             // 当前扇区擦除起始地址
    uint32 allSectorNumber = 0u;            // 当前分区总扇区数量
    uint32 curNumberEraseSectoe = 0u;       // 单次循环待擦除扇区计数
    uint32 accessLargestSectorNumber = 0u;  // 单次最大可擦除扇区限制
    uint32 infoAddr = 0u;                   // 信息头页地址，擦APP前先作废
    uint32 infoLen = 0u;
    tCrc crcValue = 0u;                     // 擦除完成后APP头部CRC缓存
    // 单次最大允许擦除扇区 = S3水位超时毫秒 / 单扇区擦除耗时  MOD_MAX_ERASE_SECTOR_MS=擦除1个扇区所消耗的时间 ms
    const uint32 LargestEraseSector = UDS_GetUDSS3WatermarkTimerMs() / MOD_MAX_ERASE_SECTOR_MS;
    ASSERT(NULL_PTR == completeFlag);
    // 固件未下载完成禁止执行擦除
    if (TRUE != flashDLStatusSingle.VerifyFDDownloadedFlag)
    {
        *completeFlag = TRUE;
        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_ERASE_FAIL;
        return FALSE;
    }
    *completeFlag = FALSE;
    switch (eraseFlashStatus)
    {
        case BEGIN_STEP:
#ifdef UDS_PROJECT_FOR_BOOTLOADER
    #ifdef APP_BICLE_UPDATA
            appConclude = GainDLApp(); // 双分区获取目标擦除分区
    #else
            appConclude = GainPreviousAppId(backupEraseFlag); // 单备份分区获取目标
    #endif
#else
            appConclude = GainPreviousAppId(backupEraseFlag);
#endif
            bloInfo = NULL_PTR;
            appFlashTime = 0u;
            eraseSectorNumber = 0u;
            ret = TRUE;
            FlashDebugLog("Erase BEGIN_STEP 1\r\n");
            // 读取分区扇区配置信息，初始化擦除计时上限
            if (TRUE == FLASH_HAL_InspectFlashConfiguration(appConclude, &bloInfo, &appFlashTime))
            {
                FlashDebugLog("Erase BEGIN_STEP 2\r\n");
                /* 0x31 FF00：按上位机地址/长度擦 Boot 区 */
                if ((FALSE == backupEraseFlag) && (TRUE == gs_routineEraseValid) && (0u != gs_routineEraseLen))
                {
                    gs_reqEraseBlock.startLogAddr = gs_routineEraseStart;
                    gs_reqEraseBlock.endLogAddr = gs_routineEraseStart + gs_routineEraseLen;
                    bloInfo = &gs_reqEraseBlock;
                    appFlashTime = 1u;
                    FlashDebugLog("Erase range 0x%x - 0x%x\r\n",
                                  gs_reqEraseBlock.startLogAddr,
                                  gs_reqEraseBlock.endLogAddr);
                }
                /* 擦 APP 前先作废信息头；擦 Boot / 标定不要动过渡程序自己的 APP 头 */
                if ((FALSE == backupEraseFlag) &&
                    (gs_routineEraseStart >= APP_A_BEGIN_ADDR) &&
                    (gs_routineEraseStart < APP_A_END_ADDR))
                {
                    infoAddr = APP_A_BEGIN_ADDR;
                    infoLen = 0u;
                    (void)FLASH_HAL_GetDetailOfAPP(appConclude, &infoAddr, &infoLen);
                    if (TRUE != InvalidateAppInfoPage(infoAddr))
                    {
                        FlashDebugLog("Erase: invalidate app info fail\r\n");
                        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_ERASE_FAIL;
                        ret = FALSE;
                        *completeFlag = TRUE;
                        break;
                    }
                    appInfoVar.flashWriteOkFlag = FALSE;
                    appInfoVar.flashEraseOkFlag = FALSE;
                    appInfoVar.flashStructOkFlag = FALSE;
                    ProduceAndSetAppCrc(&crcValue);
                    FlashDebugLog("Erase: app info invalidated\r\n");
                }
                eraseFlashStatus = ERASING_STEP;
            }
            break;
        case ERASING_STEP:
            FlashDebugLog("Erase ERASING_STEP\r\n");
            /* 按当前块描述计算扇区数，FF00时仅为请求范围而非整片APP */
            if ((NULL_PTR != bloInfo) && (bloInfo->endLogAddr > bloInfo->startLogAddr))
            {
                allSectorNumber = FLASH_HAL_CalcSectorCount(bloInfo->startLogAddr,
                                                            bloInfo->endLogAddr - bloInfo->startLogAddr);
            }
            else
            {
                allSectorNumber = FLASH_HAL_CalcFlashSec(appConclude);
            }
            // 分区总扇区小于单次最大擦除量，一次性擦完
            if (allSectorNumber <= LargestEraseSector)
            {
                for( ; appFlashTime != 0; appFlashTime--)
                {
                    space = bloInfo->endLogAddr - bloInfo->startLogAddr;
                    WATCHDOG_HAL_Fed(); // 擦除循环喂狗防止硬件复位
                    sectorId = FLASH_HAL_CalcSectorCount(bloInfo->startLogAddr, space);  //获取扇区数量
                    if (NULL_PTR == flashDLStatusSingle.FlashOperationAPI.pfSectorRemove)
                    {
                        ret = FALSE; // 底层擦除API未绑定
                    }
                    else
                    {
                        curNumberEraseSectoe = sectorId;
                        earseBeginAddr = bloInfo->startLogAddr;
                        // 循环擦除当前块所有扇区
                        while (curNumberEraseSectoe)
                        {
                            WATCHDOG_HAL_Fed();
                            DisableAllInterrupts(); // 关闭中断防止擦除被打断
                            ret = flashDLStatusSingle.FlashOperationAPI.pfSectorRemove(earseBeginAddr, 1u);
                            EnableAllInterrupts();
                            curNumberEraseSectoe--;
                            if (TRUE != ret)
                            {
                                break; // 单个扇区擦除失败直接退出
                            }
                            earseBeginAddr += MOD_SECTOR_SIZE;
                        }
                        
                    }
                    if (TRUE != ret)
                    {
                        break;
                    }
                    eraseSectorNumber += sectorId;
                    bloInfo++; // 切换下一块分区配置
                }
            }
            else
            {
                // 分区扇区量大，分段擦除，每次擦LargestEraseSector个扇区后退出循环
                while ((eraseSectorNumber < allSectorNumber) && (0u != appFlashTime))
                {
                    WATCHDOG_HAL_Fed();
                    // 根据扇区序号获取对应擦除起始地址
                    if (TRUE != FLASH_HAL_BOASecNumToAddr(appConclude, eraseSectorNumber, &earseBeginAddr))
                    {
                        ret = FALSE;
                        break;
                    }
                    if ((earseBeginAddr >= bloInfo->startLogAddr) && (earseBeginAddr < bloInfo->endLogAddr))
                    {
                        space = bloInfo->endLogAddr - earseBeginAddr;
                    }
                    else
                    {
                        ret = FALSE;
                        break;
                    }
#ifdef ALLOW_LIN_TP
                    accessLargestSectorNumber = LargestEraseSector - (eraseSectorNumber % LargestEraseSector);
#else
                    earseBeginAddr = bloInfo->startLogAddr;
                    accessLargestSectorNumber = LargestEraseSector - (eraseSectorNumber % LargestEraseSector);
#endif
                    sectorId = FLASH_HAL_CalcSectorCount(earseBeginAddr, space);
                    if (sectorId > LargestEraseSector)
                    {
                        sectorId = (sectorId > accessLargestSectorNumber) ? accessLargestSectorNumber : LargestEraseSector;
                    }
                    else
                    {
                        if (sectorId <= accessLargestSectorNumber)
                        {
                            appFlashTime--;
                            bloInfo++;
                        }
                        sectorId = (sectorId > accessLargestSectorNumber) ? accessLargestSectorNumber : sectorId;
                    }
                    if (NULL_PTR == flashDLStatusSingle.FlashOperationAPI.pfSectorRemove)
                    {
                        ret = FALSE;
                    }
                    else
                    {
                        curNumberEraseSectoe = sectorId;
                        while (curNumberEraseSectoe)
                        {
                            WATCHDOG_HAL_Fed();
                            DisableAllInterrupts();
                            ret = flashDLStatusSingle.FlashOperationAPI.pfSectorRemove(earseBeginAddr, 1u);
                            EnableAllInterrupts();
                            curNumberEraseSectoe--;
                            if (TRUE != ret)
                            {
                                FlashDebugLog("Flash erased failed and Remind sectors:0x%x\r\n", curNumberEraseSectoe);
                                break;
                            }
                            earseBeginAddr += MOD_SECTOR_SIZE;
                        }
                    }
                    if (TRUE != ret)
                    {
                        break;
                    }
                    eraseSectorNumber += sectorId;
                    // 达到单次擦除上限，标记未完成，切等待状态回复繁忙
                    if(0u == (eraseSectorNumber % LargestEraseSector))
                    {
                        if(eraseSectorNumber < allSectorNumber)
                        {
                            *completeFlag = FALSE;
                            break;
                        }
                    }
                }
            }
            // 分段擦除未完成，切换等待任务，触发0x7F 78繁忙响应
            if ((FALSE == *completeFlag) && (TRUE == ret) && (eraseSectorNumber < allSectorNumber))
            {
#ifdef ALLOW_LIN_TP
                flashDLStatusSingle.newIntterpTask = FLASH_IS_IN_ERASING;
#endif
                flashDLStatusSingle.flashCurTask = FLASH_IS_IN_WAITING;
                if (NULL_PTR != flashDLStatusSingle.requestOtherTimePara)
                {
                    flashDLStatusSingle.requestOtherTimePara(flashDLStatusSingle.udsSerIDrequested, SetRequestLongerTimerFlag);
                }
            }
            else
            {
                // 全部扇区擦除完成，更新分区擦除标记，重新计算APP头部CRC
                if ((TRUE == ret) && (eraseSectorNumber == allSectorNumber))
                {
                    FlashDebugLog("Flash erase ok appInfoVar.flashEraseOkFlag\r\n");
                    appInfoVar.flashEraseOkFlag = TRUE;
                    appInfoVar.flashWriteOkFlag = FALSE;
                    appInfoVar.flashStructOkFlag = TRUE;
#ifdef ALLOW_LIN_TP
                    FlashDebugLog("Flash set app type erased\r\n");
                    if((APP_A_ID == appConclude)
#ifdef APP_BICLE_UPDATA
                    || (APP_B_ID == appConclude)
#endif
                      )
                    {
                        flashDLStatusSingle.appErasedFlag = 1u << appConclude;
                    }
#endif
                }
                else
                {
                    // 擦除失败，清空所有合法标记
                    FlashDebugLog("Flash erase fail\r\n");
                    FaultInfo_GetPtr()->f1ed |= F1ED_BIT_ERASE_FAIL;
                    appInfoVar.flashEraseOkFlag = FALSE;
                    appInfoVar.flashWriteOkFlag = FALSE;
                    appInfoVar.flashStructOkFlag = TRUE;
#ifdef ALLOW_LIN_TP
                    FlashDebugLog("Flash clear app type erased!!! ret:0x%x; eraseSectorNumber:0x%x; allSectorNumber:0x%x:\r\n", ret, eraseSectorNumber, allSectorNumber);
                    if((APP_A_ID == appConclude)
#ifdef APP_BICLE_UPDATA
                    || (APP_B_ID == appConclude)
#endif
                      )
                    {
                        flashDLStatusSingle.appErasedFlag &= ~(1u << appConclude);
                    }
                    else
                    {
                        flashDLStatusSingle.appErasedFlag = 0u;
                    }
#endif
                }
                ProduceAndSetAppCrc(&crcValue);
                eraseSectorNumber = 0u;
                eraseFlashStatus = END_STEP;
            }
            break;
        case END_STEP:
            FlashDebugLog("Erase END_STEP\r\n");
            WATCHDOG_HAL_Fed();
            bloInfo = NULL_PTR;
            appFlashTime = 0u;
            eraseSectorNumber = 0u;
            *completeFlag = TRUE; // 完整擦除流程结束
            eraseFlashStatus = BEGIN_STEP; // 重置擦除状态机等待下一次擦除指令
            break;
        default:
            eraseFlashStatus = BEGIN_STEP;
            break;
    }
    return ret;
}
/**
 * @brief 将0x36分段接收数据缓存到全局写入缓冲区，超出长度返回失败
 * @param addr 分段数据源指针
 * @param space 分段数据有效长度
 * @return TRUE=缓存成功 FALSE=数据过长溢出
 */
static uint8 RecordData(const uint8 *addr, const uint8 space)
{
    boolean ret = FALSE;
    ASSERT(NULL_PTR == addr);
    if( space <= MAX_MOD_DATA_LEN )
    {
        Momory_Copy_Function(flashDLStatusSingle.needToWriteBuffer, addr, space);
        flashDLStatusSingle.recContentSize = space;
        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }
    return ret;
}
/**
 * @brief 校验目标写入地址是否落在合法APP分区Flash区间内
 * @return TRUE=地址合法可写入 FALSE=地址越界禁止刷写
 */
static boolean CheckFlsDrvSwData(void)
{
    boolean ret = FALSE;
    uint32 beginAddr = 0u;
    uint32 rearAddr = 0u;
    // 读取APP分区起始与结束地址
    ret = FLASH_HAL_ReadDriverRange(&beginAddr, &rearAddr);
    if(flashDLStatusSingle.startAddr >= beginAddr)
    {
        if((flashDLStatusSingle.startAddr + flashDLStatusSingle.length) < rearAddr)
        {
            if(TRUE == ret)
            {
                ret = TRUE;
            }
            else
            {
                ret = FALSE;
            }
        }
        else
        {
            ret = FALSE;
        }
    }
    else
    {
        ret = FALSE;
    }
    return ret;
}

/* 用 0x34 记录的起始地址判断，37 时 startAddr 已被写指针推走，不能再用 CheckFlsDrvSwData */
static uint8 IsCurrentDownloadFlashDriver(void)
{
    uint32 beginAddr = 0u;
    uint32 rearAddr = 0u;
    uint32 addr = flashDLStatusSingle.flashRecContentBeginAddr;

    if (TRUE != FLASH_HAL_ReadDriverRange(&beginAddr, &rearAddr))
    {
        return FALSE;
    }
    if ((addr >= beginAddr) && (addr < rearAddr))
    {
        return TRUE;
    }
    return FALSE;
}
#ifdef UDS_PROJECT_FOR_BOOTLOADER
#ifdef APP_BICLE_UPDATA
/**
 * @brief 双分区模式根据擦除起始地址判断当前待刷写分区ID
 * @return APP_A_ID / APP_B_ID / APP_USELESS_ID
 */
static AppIdType GainDLApp(void)
{
    uint32 appABeginAddr = 0u;
    uint32 appABlockSpace = 0u;
    AppIdType app = APP_USELESS_ID;
    uint32 appBBeginAddr = 0u;
    uint32 appBBlockSpace = 0u;
    // 读取A/B分区头部起始地址与分区大小
    FLASH_HAL_GetDetailOfAPP(APP_A_ID, &appABeginAddr, &appABlockSpace);
    FLASH_HAL_GetDetailOfAPP(APP_B_ID, &appBBeginAddr, &appBBlockSpace);
    doubleRenewDLAppInfoAddr = UDS_APP_RetrieveEraseStartAddress() - APP_INFORMATION_SIZE;
    // 匹配擦除地址对应分区
    app = (doubleRenewDLAppInfoAddr == appABeginAddr) ? APP_A_ID : APP_USELESS_ID;
    app = (doubleRenewDLAppInfoAddr == appBBeginAddr) ? APP_B_ID : app;
    return app;
}
#endif
#endif
#ifdef ALLOW_LIN_TP
/**
 * @brief LIN模式对比诊断下发CRC与本地计算CRC是否一致
 * @return TRUE=CRC匹配固件完整 FALSE=校验失败
 */
static boolean VerifyRecCrcRationality(void)
{
    boolean returnResult = FALSE;
#ifndef DebugBootloader_NOTCRC
    returnResult = (flashDLStatusSingle.receivedCRC == flashDLStatusSingle.computeLinCRC) ? TRUE : FALSE;
    FlashDebugLog("flashDLStatusSingle.receivedCRC:0x%x; flashDLStatusSingle.computeLinCRC:0x%x\r\n",
                    flashDLStatusSingle.receivedCRC, flashDLStatusSingle.computeLinCRC);
#else
    returnResult = TRUE; // 调试宏开启时跳过CRC校验
#endif
    return returnResult;
}
/**
 * @brief LIN TP多段固件分段CRC计算主逻辑，分时运算
 * @param completeFlag 出参 TRUE=全部计算完成 FALSE=分段等待下一次调度
 * @return TRUE=运算无异常 FALSE=运算出错
 */
static boolean ProcessLinTpCheckSum(boolean *completeFlag)
{
    uint8 sucFlag = FALSE;
    uint8 count = 0;
    uint32 calDataBeginAddr = 0u;
    uint32 calcrcSize = 0u;
    int checkAgain = 1;
    static uint32 crcSize = 0u;
    static tCrc crcValue = 0u;
    boolean finishFlag = FALSE;
    checkCrcType phase = CHECK_BEGIN_STEP;
#ifdef APP_SINGLE_UPDATA
    static uint32 s_calCRCDataLenTotal = 0u; // 双备份模式固件总长度缓存
#endif
    WATCHDOG_HAL_Fed();
    *completeFlag = FALSE;
    sucFlag = TRUE;
    phase = checkCrcStatus;
    if(phase == CHECK_END_STEP)
    {
        // CRC全部计算完成，停止CRC硬件/软件运算
        CRC_HAL_StopSWCrc(&crcValue);
        flashDLStatusSingle.computeLinCRC = crcValue;
        finishFlag = TRUE;
        FlashDebugLog("Final Crc value:0x%x\r\n", crcValue);
        flashDLNeedOtherTimeSignal = 0;
        count = 0;
        // 清空多段下载缓存数组
        while(count < LONGEST_SAVE_NUMBER)
        {
            flashDLStatusBuffer[count].flashRecContentSize = 0;
            flashDLStatusBuffer[count].flashRecContentBeginAddr = 0;
            count++;
        }
#ifdef APP_SINGLE_UPDATA
            flashDLStatusSingle.appStatusInFlash->appCrc = crcValue;
            flashDLStatusSingle.appStatusInFlash->appLen = s_calCRCDataLenTotal;
#endif
        checkCrcStatus = CHECK_BEGIN_STEP;
    }
    else if(phase == CHECK_BEGIN_STEP)
    {
        // CRC初始化阶段，清零CRC寄存器，计算总校验长度
        crcSize = 0;
        CRC_HAL_InitSWCrc(&crcValue);
        checkCrcStatus = CHECKING_STEP;
        if(flashDLNeedOtherTimeSignal <= 1)
        {
            crcSize = flashDLStatusSingle.flashRecContentSize;
        }
        else
        {
            count = 0;
            while(count < LONGEST_SAVE_NUMBER)
            {
                crcSize  = crcSize + (flashDLStatusBuffer[count].flashRecContentSize);
                count++;
            }
        }
#ifdef APP_SINGLE_UPDATA
        s_calCRCDataLenTotal = crcSize;
#endif
    }
    else if(phase == CHECKING_STEP)
    {
        // 分段循环计算CRC，单次不超限长读取长度
        while (checkAgain) 
        {
            calcrcSize = (crcSize < LONGEST_LENGTH_TO_CHECK) ? crcSize : LONGEST_LENGTH_TO_CHECK;
            switch (flashDLNeedOtherTimeSignal)
            {
                case 0u:
                case 1u:
                    calDataBeginAddr = flashDLStatusSingle.flashRecContentBeginAddr;
                    break;
            
                default:
                    calDataBeginAddr = flashDLStatusBuffer[0].flashRecContentBeginAddr;
                    calcrcSize = flashDLStatusBuffer[0].flashRecContentSize;
                    flashDLNeedOtherTimeSignal--;
                    break;
            }
            CRC_HAL_CreatSw((const uint8 *)calDataBeginAddr, calcrcSize, &crcValue);
            FlashDebugLog("Crc calculate calDataBeginAddr:0x%x; calcrcSize:0x%x; Crc Result:0x%x\r\n", calDataBeginAddr, calcrcSize, crcValue);
            
            if (crcSize == flashDLStatusSingle.flashRecContentSize) 
            {
                checkCrcStatus = CHECK_END_STEP;
                checkAgain = 0;
            } 
            else if (crcSize > flashDLStatusSingle.flashRecContentSize) 
            {
                crcSize -= flashDLStatusBuffer[0].flashRecContentSize;
            } 
            else 
            {
                // 单次运算到达上限，标记等待，回复繁忙给诊断仪
                flashDLStatusSingle.newIntterpTask = FLASH_IS_IN_CHECK;
                flashDLStatusSingle.flashCurTask = FLASH_IS_IN_WAITING;
                if(NULL_PTR != flashDLStatusSingle.requestOtherTimePara)
                {
                    flashDLStatusSingle.requestOtherTimePara(flashDLStatusSingle.udsSerIDrequested, SetRequestLongerTimerFlag);
                }
                checkAgain = 0;
            }
        }
    }
    else
    {
        checkCrcStatus = CHECK_BEGIN_STEP;
        sucFlag = FALSE;
        finishFlag = TRUE;
    }
    WATCHDOG_HAL_Fed();
    *completeFlag = finishFlag;
    if(TRUE == sucFlag)
    {
        /** CRC分段运算无异常 */
    }
    else
    {
        FlashDebugLog("%s: checksum job failed!\n", __func__);
    }
    return sucFlag;
}
#endif
/**
 * @brief 设置繁忙超时标记，1=正常等待 2=操作失败重置下载流程
 */
static void SetRequestLongerTimerFlag(uint8 flag)
{
    reqTimeFlag = (flag == 0u) ? 1 : 2;
}
/**
 * @brief 固件CRC校验总入口，区分CAN/LIN两种校验逻辑
 * @param completeFlag 出参 TRUE=校验全部完成 FALSE=分段等待
 * @return TRUE=校验无底层故障 FALSE=地址非法/校验失败
 */
static uint8 ProcessVerifyCrc(boolean *completeFlag)
{
#ifdef ALLOW_LIN_TP
    boolean ret = CheckFlsDrvSwData();
    if((TRUE == ret) || ((FALSE == CheckFlsDrvSwData()) && (TRUE == flashDLStatusSingle.VerifyFDDownloadedFlag)))
    {
        ret = ProcessLinTpCheckSum(completeFlag);
    }
    else
    {
        *completeFlag = TRUE;
        ret = FALSE;
    }
    if (*completeFlag != TRUE) {
        return TRUE;
    }
    // LIN模式校验本地CRC与诊断下发CRC
    if (VerifyRecCrcRationality() != TRUE) {
        FlashDebugLog("%s: CRC failed!\n", __func__);
        return FALSE;
    }
    if (CheckFlsDrvSwData() != TRUE) {
        return TRUE;
    }
    // CRC校验通过，绑定Flash底层操作API
    flashDLStatusSingle.VerifyFDDownloadedFlag = TRUE;
    if (FLASH_HAL_BindFlashAPI(&flashDLStatusSingle.FlashOperationAPI) != TRUE) {
        flashDLStatusSingle.VerifyFDDownloadedFlag = FALSE;
        return TRUE;
    }
    if (flashDLStatusSingle.FlashOperationAPI.pfFlashSetup != NULL_PTR) {
        flashDLStatusSingle.FlashOperationAPI.pfFlashSetup();
    }
    return TRUE;
#else
    tCrc crcValue = 0u;
#ifdef APP_SINGLE_UPDATA
    uint32 appDataLen = 0;
#endif
    WATCHDOG_HAL_Fed();
    // 合法分区地址，硬件CRC整包计算
    if ((TRUE == CheckFlsDrvSwData()) )
    {
        CRC_HAL_CreatHw((const uint8 *)flashDLStatusSingle.flashRecContentBeginAddr, flashDLStatusSingle.flashRecContentSize, &crcValue);
        FlashDebugLog("CRC_HAL_CreatHw ok! crcValue:0x%x\r\n",crcValue);
    }
    else if (TRUE == flashDLStatusSingle.VerifyFDDownloadedFlag)
    {
        // 备份分区场景分段计算
        FlashDebugLog("VerifyFDDownloadedFlag ok 1\r\n");
        CRC_HAL_CreatHw((const uint8 *)flashDLStatusSingle.flashRecContentBeginAddr, flashDLStatusSingle.flashRecContentSize, &crcValue);
        flashDLStatusSingle.flashRecContentBeginAddr += flashDLStatusSingle.flashRecContentSize;
#ifdef APP_SINGLE_UPDATA
        appDataLen = flashDLStatusSingle.flashRecContentSize;
#endif
        flashDLStatusSingle.flashRecContentSize = 0u;
    }
    else
    {
        /* 无固件数据无需校验 */
        FlashDebugLog("No firmware data to verify!\r\n");
    }
    WATCHDOG_HAL_Fed();
#ifdef DebugBootloader_NOTCRC
    if (1)
#else
    // 对比本地计算CRC与诊断仪下发CRC
    if (flashDLStatusSingle.receivedCRC == crcValue)
#endif
    {
        FlashDebugLog("receivedCRC = crcValue\r\n");
        if ((TRUE == CheckFlsDrvSwData()))
        {
            flashDLStatusSingle.VerifyFDDownloadedFlag = TRUE;
            FlashDebugLog("VerifyFDDownloadedFlag ok 2\r\n");
            if (TRUE != FLASH_HAL_BindFlashAPI(&flashDLStatusSingle.FlashOperationAPI))
            {
                flashDLStatusSingle.VerifyFDDownloadedFlag = FALSE;
                FlashDebugLog("VerifyFDDownloadedFlag fail 1\r\n");
            }
            else
            {
                if (NULL_PTR != flashDLStatusSingle.FlashOperationAPI.pfFlashSetup)
                {
                    flashDLStatusSingle.FlashOperationAPI.pfFlashSetup();
                }
            }
        }
#ifdef APP_SINGLE_UPDATA
        else
        {
            flashDLStatusSingle.appStatusInFlash->appCrc = crcValue;
            flashDLStatusSingle.appStatusInFlash->appLen = appDataLen;
        }
#endif
        return TRUE;
    }
    FlashDebugLog("crcValue fail 3\r\n");
    
    return FALSE;
#endif
}
/**
 * @brief 分段写入Flash主逻辑，按硬件128字节对齐写入，不足补齐0xFF
 * @param completeFlag 出参 TRUE=当前分段写入完成
 * @return TRUE=写入无故障 FALSE=写入失败
 */
static uint8 ProcessWriteFlash(boolean *completeFlag)
{
    uint8 ret = FALSE;
    uint8 counter = 0u;
    uint8 alignCount = 0u;
    uint32 crcValue = 0u;
    // 固件未下载完成禁止写入
    if (TRUE != flashDLStatusSingle.VerifyFDDownloadedFlag)
    {
        FlashDebugLog("VerifyFDDownloadedFlag fail 4\r\n");
        FaultInfo_GetPtr()->f1ed |= F1ED_BIT_DOWNLOAD_FAIL;
        *completeFlag = TRUE;
        return FALSE;
    }
    ret = TRUE;
    // 循环按硬件最小写入单元分段刷写
    while (flashDLStatusSingle.recContentSize >= FLASH_OPERATE_SIZE_BYTE)
    {
        ProduceAppCrc(&crcValue);
        // 写入前校验APP头部CRC合法性
        if (TRUE != (((appInfoVar.crc) == crcValue) ? TRUE : FALSE))
        {
            ret = FALSE;
            FlashDebugLog("appInfoVar.crc fail \r\n");
            break;
        }
        // 分区擦除、结构体标记全部合法才允许写入
        if ((TRUE == appInfoVar.flashEraseOkFlag) && (TRUE == appInfoVar.flashStructOkFlag))
        {
            WATCHDOG_HAL_Fed();
            if (NULL_PTR != flashDLStatusSingle.FlashOperationAPI.pfDataWriter)
            {
                DisableAllInterrupts();
                ret = flashDLStatusSingle.FlashOperationAPI.pfDataWriter
                      (
                        flashDLStatusSingle.startAddr,
                        &flashDLStatusSingle.needToWriteBuffer[counter * FLASH_OPERATE_SIZE_BYTE],
                        FLASH_OPERATE_SIZE_BYTE
                      );
                EnableAllInterrupts();
            }
            else
            {
                ret = FALSE;
            }
            if (TRUE == ret)
            {
                // 更新剩余长度、写入起始地址、分段索引
                flashDLStatusSingle.length -= FLASH_OPERATE_SIZE_BYTE;
                flashDLStatusSingle.recContentSize -= FLASH_OPERATE_SIZE_BYTE;
                flashDLStatusSingle.startAddr += FLASH_OPERATE_SIZE_BYTE;
                counter++;
            }
            else
            {
                ret = FALSE;
                break;
            }
        }
        else
        {
            ret = FALSE;
            break;
        }
    }
    // 处理末尾不足128字节的数据，补齐0xFF对齐硬件写入单元
    if ((0u != flashDLStatusSingle.recContentSize) && (TRUE == ret))
    {
        alignCount = (uint8)(flashDLStatusSingle.recContentSize & 0x07u);
        alignCount = (~alignCount + 1u) & 0x07u;
        Momory_Fill_Function((void *)&flashDLStatusSingle.needToWriteBuffer[counter * FLASH_OPERATE_SIZE_BYTE + flashDLStatusSingle.recContentSize], 0xFFu, alignCount);
        flashDLStatusSingle.recContentSize = flashDLStatusSingle.recContentSize + alignCount;
        if (NULL_PTR != flashDLStatusSingle.FlashOperationAPI.pfDataWriter)
        {
            DisableAllInterrupts();
            ret =  flashDLStatusSingle.FlashOperationAPI.pfDataWriter
                (  flashDLStatusSingle.startAddr,
                  &flashDLStatusSingle.needToWriteBuffer[counter * FLASH_OPERATE_SIZE_BYTE],
                   flashDLStatusSingle.recContentSize
                );
            EnableAllInterrupts();
        }
        else
        {
            ret = FALSE;
        }
        if (TRUE == ret)
        {
            flashDLStatusSingle.length -= (flashDLStatusSingle.recContentSize - alignCount);
            flashDLStatusSingle.startAddr += flashDLStatusSingle.recContentSize;
            flashDLStatusSingle.recContentSize = 0;
            counter++;
        }
    }
    if (TRUE == ret)
    {
        /* 仅记录本周期已写过 APP，写成功标志改到 0x37 传输完成再置 */
        s_appDataWrittenThisCycle = TRUE;
        *completeFlag = TRUE;
        return TRUE;
    }
    *completeFlag = TRUE;
    FaultInfo_GetPtr()->f1ed |= F1ED_BIT_DOWNLOAD_FAIL;
    // 写入失败此处可重置下载流程，工程屏蔽
    //PrepareDLInformation();
    return FALSE;
}
/**
 * @brief LIN多段下载参数缓存写入全局数组
 * @param count 缓存数组索引0~2
 * @param beginAddress 分段固件起始地址
 * @param space 分段固件长度
 */
static void RenewDLFlsInformation(uint32_t count, uint32_t beginAddress, uint32_t space)
{
    (void)flashDLStatusBuffer;
    if (count < LONGEST_SAVE_NUMBER) 
    {
        flashDLStatusBuffer[count].flashRecContentBeginAddr = beginAddress;
        flashDLStatusBuffer[count].flashRecContentSize = space;
#ifdef ALLOW_LIN_TP
        flashDLStatusbackp[count].flashRecContentBeginAddr = beginAddress;
        flashDLStatusbackp[count].flashRecContentSize = space;
#endif
        flashDLStatusSingle.startAddr = beginAddress;
        flashDLStatusSingle.length = space;
        flashDLStatusSingle.flashRecContentBeginAddr = beginAddress;
        flashDLStatusSingle.flashRecContentSize = space;
        flashDLNeedOtherTimeSignal++; // 分段计数自增
    }
}
/**************************************************************************
                    全局对外函数实现区
**************************************************************************/
/**
 * @brief 保存诊断下发指纹到APP信息区并刷新结构体CRC
 */
void RecordFingerPrint(const uint8 *addr, const uint8 space)
{
    uint8 size = 0u;
    tCrc crcVal = 0u;
    ASSERT(NULL_PTR == addr);
    // 指纹最大存储17字节，超长截断
    size = (space > 17u) ? 17u : space;
    Momory_Copy_Function((void *) flashDLStatusSingle.appStatusInFlash->PrintBufferForFinger, (const void *)addr, size);
    ProduceAndSetAppCrc(&crcVal);
}
/**
 * @brief 更新UDS下载流程阶段状态机
 */
void SetNextDLPara(const FlashDownloadType num)
{
    flashDLStatusSingle.flashDownloadPara = num;
}
/**
 * @brief 读取最新合法APP分区头部，校验结构体CRC是否合法
 * @return TRUE=分区可用 FALSE=损坏/空分区
 */
uint8 VerifyAppInfoFromFlsRationality(void)
{
    tCrc crcValue = 0u;
    uint32 begin = 0u;
    uint32 space = 0u;
    boolean ret = FALSE;
    // 获取当前最新APP分区起始地址与分区大小
    ret = FLASH_HAL_GetDetailOfAPP(GainNewestAPPId(), &begin, &space);
	FlashDebugLog("BOOT begin= 0x%08x ,space=0x%x\r\n",begin,space);
	FlashDebugLog("BOOT sizeof(appInfoType)= 0x%x\r\n",sizeof(appInfoType));
    if(sizeof(appInfoType) <= space)
    {
        if(TRUE == ret)
        {
    #ifdef ALLOW_LIN_TP
        #ifdef EN_APP_INFO_DATA_IN_NONE_FLASH
            ret = FLASH_HAL_ReadAPPInfoData(begin, (sizeof(appInfoType)), &appInfoVar);
        #else
            appInfoVar = *(appInfoType*)begin;
            ret = TRUE;
        #endif
    #else
            appInfoVar = *(appInfoType *)begin;
    #endif
        }
    }
    //4. 重新计算appInfoVar头部的CRC（排除结构体末尾4字节crc成员本身）
    ProduceAppCrc(&crcValue);
	FlashDebugLog("BOOT appInfoVar.crc= 0x%08x ,crcValue=0x%x\r\n",(appInfoVar.crc),crcValue);
	//5. 对比：Flash头部里面保存的crc 和 现场重新算出来的crc是否相等
    return (((appInfoVar.crc) == crcValue) ? TRUE : FALSE);
}
/**
 * @brief 重置所有下载、擦除、CRC全局状态，初始化刷写流程
 */
void PrepareDLInformation(void)
{
    flashDLStatusSingle.VerifyFingerPrintWrittenFlag = FALSE;
    if (TRUE == flashDLStatusSingle.VerifyFDDownloadedFlag)
    {
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        RamFDErase(); // 清空RAM固件缓存
#endif
        flashDLStatusSingle.VerifyFDDownloadedFlag = FALSE;
    }
    flashDLNeedOtherTimeSignal = 0;
    SetNextDLPara(FLASH_DOWNLOAD_REQUEST);
    SetCurFlsTaskPara(FLASH_IS_IN_IDLE, NULL_PTR, UDS_SERVICES_USELESS_ID, NULL_PTR);
    flashDLStatusSingle.appStatusInFlash = &appInfoVar;
    Momory_Fill_Function(&flashDLStatusSingle.FlashOperationAPI, 0x0u, sizeof(Flash_OperationAPIType));
    Momory_Fill_Function(&appInfoVar, 0xFFu, sizeof(appInfoType));
    flashDLNeedOtherTimeSignal = 0;
}
#ifdef APP_SINGLE_UPDATA
/**
 * @brief 校验备份分区APP CRC与主分区是否一致
 * @return TRUE=备份完整可用 FALSE=备份损坏
 */
boolean VerifyAppBackupResult(void)
{
    static tCrc backupAppCrc = 0u;
    uint8 result = FALSE;
    tCrc backupAppInfoCrc = 0u;
#ifdef ALLOW_LIN_TP
    CRC_HAL_ComputeSingleCRC((const uint8 *)(APP_A_BACKUP_BEGIN_ADDR), sizeof(appInfoType) - 4, &backupAppInfoCrc);
#else
    CRC_HAL_CreatHw((const uint8 *)(APP_A_BACKUP_BEGIN_ADDR), sizeof(appInfoType) - 4, &backupAppInfoCrc);
#endif
    // 先校验备份分区头部信息CRC合法
    if(((appInfoType*)(APP_A_BACKUP_BEGIN_ADDR))->crc == backupAppInfoCrc)
    {
#ifdef ALLOW_LIN_TP
        CRC_HAL_InitSWCrc(&backupAppCrc);
        for(uint8 Index = 0;Index < LONGEST_SAVE_NUMBER;Index++)
        {
            if(0 != flashDLStatusbackp[Index].flashRecContentBeginAddr)
            {
                CRC_HAL_CreatSw((const uint8 *)(APP_A_BACKUP_BEGIN_ADDR + (flashDLStatusbackp[Index].flashRecContentBeginAddr - APP_A_BEGIN_ADDR)),
                        flashDLStatusbackp[Index].flashRecContentSize, &backupAppCrc);
                FlashDebugLog("Crc callback startCalDataAddr:0x%lx; calCRCDataLen:0x%lx; Crc Result:0x%lx\r\n",
                                APP_A_BACKUP_BEGIN_ADDR + (flashDLStatusbackp[Index].flashRecContentBeginAddr - APP_A_BEGIN_ADDR)
                              , flashDLStatusbackp[Index].flashRecContentSize
                              , backupAppCrc);
            }
        }
        CRC_HAL_StopSWCrc(&backupAppCrc);
        FlashDebugLog("callback Final Crc value:0x%lx\r\n", backupAppCrc);
#else
        // 硬件CRC计算备份分区完整固件
        CRC_HAL_CreatHw((const uint8 *)(APP_A_BACKUP_BEGIN_ADDR + APP_INFORMATION_SIZE), flashDLStatusSingle.appStatusInFlash->appLen, &backupAppCrc);
#endif
        // 主分区CRC与备份分区CRC完全匹配则备份有效
        if((flashDLStatusSingle.appStatusInFlash->appCrc == backupAppCrc) &&
           ((appInfoType*)(APP_A_BACKUP_BEGIN_ADDR))->appCrc == backupAppCrc)
        {
            FlashDebugLog("Backup App is Valid when OTA!!!\r\n");
            result = TRUE;
        }
    }
    return result;
}
/**
 * @brief 将主分区完整APP拷贝至备份分区（单备份升级）
 * @return TRUE=拷贝成功 FALSE=写入失败
 */
boolean AppBackupInFls(void)
{
    uint8 ret = TRUE;
    uint8 count = 0;
    uint32 allCopyNumber = 0;
    uint32 haveCopiedNumber = 0;
    boolean completeFlag = FALSE;
    uint8 *sourcePosition = NULL;
#ifdef ALLOW_LIN_TP
    uint8 counterCnt = 0;
    allCopyNumber = APP_INFORMATION_SIZE;
    flashDLStatusSingle.startAddr = APP_A_BACKUP_BEGIN_ADDR;
    sourcePosition = (uint8*)(APP_A_BEGIN_ADDR);
    // 先拷贝APP头部信息区
    for( ; haveCopiedNumber < allCopyNumber; haveCopiedNumber += FLASH_OPERATE_SIZE_BYTE)
    {
        for( ; count < FLASH_OPERATE_SIZE_BYTE; count++)
        {
            flashDLStatusSingle.needToWriteBuffer[count] = *sourcePosition++;
            flashDLStatusSingle.recContentSize++;
        }
        count = 0;
        if(!ProcessWriteFlash(&completeFlag))
        {
            FlashDebugLog("App Backup write failed!!!\r\n");
            ret = FALSE;
            break;
        }
    }
    // 循环拷贝多段固件至备份区
    for( ; counterCnt < LONGEST_SAVE_NUMBER; counterCnt++)
    {
        if(0 != flashDLStatusbackp[counterCnt].flashRecContentSize)
        {
            haveCopiedNumber = 0;
            count = 0;
            flashDLStatusSingle.recContentSize = 0;
            allCopyNumber = flashDLStatusbackp[counterCnt].flashRecContentSize;
            flashDLStatusSingle.startAddr = APP_A_BACKUP_BEGIN_ADDR + (flashDLStatusbackp[counterCnt].flashRecContentBeginAddr - APP_A_BEGIN_ADDR);
            sourcePosition = (uint8*)(flashDLStatusbackp[counterCnt].flashRecContentBeginAddr);
            for( ; haveCopiedNumber < allCopyNumber; haveCopiedNumber += FLASH_OPERATE_SIZE_BYTE)
            {
                for( ; count < FLASH_OPERATE_SIZE_BYTE; count++)
                {
                    flashDLStatusSingle.needToWriteBuffer[count] = *sourcePosition++;
                    flashDLStatusSingle.recContentSize++;
                }
                count = 0;
                if(!ProcessWriteFlash(&completeFlag))
                {
                    FlashDebugLog("App Backup write failed!!!\r\n");
                    ret = FALSE;
                    break;
                }
            }
        }
    }
#else
    // CAN模式直接拷贝头部+完整固件
    allCopyNumber = flashDLStatusSingle.appStatusInFlash->appLen + APP_INFORMATION_SIZE;
    flashDLStatusSingle.startAddr = APP_A_BACKUP_BEGIN_ADDR;
    sourcePosition = (uint8*)(APP_A_BEGIN_ADDR);
    for( ; haveCopiedNumber < allCopyNumber; haveCopiedNumber += FLASH_OPERATE_SIZE_BYTE)
    {
        for( ; count < FLASH_OPERATE_SIZE_BYTE; count++)
        {
            flashDLStatusSingle.needToWriteBuffer[count] = *sourcePosition++;
            flashDLStatusSingle.recContentSize++;
        }
        count = 0;
        if(!ProcessWriteFlash(&completeFlag))
        {
            FlashDebugLog("App Backup write failed!!!\r\n");
            ret = FALSE;
            break;
        }
    }
#endif
    backupEraseFlag = FALSE; // 备份拷贝完成清空擦除标记
    return ret;
}
#endif
/**
 * @brief 保存0x34下发的固件起始地址与总长度至多段缓存数组
 * 场景：UDS 0x34 RequestDownload，支持多段下载（多段固件烧录）。
整车 ECU 下载，不是只刷 1 段 Flash；可能要同时刷：App 段、Calibration 标定段、Data 数据段，会连续下发多次0x34。
 */
void RecordDLInformation(const uint32 beginAddress, const uint32 space)
{
    if(flashDLNeedOtherTimeSignal == 0)
    {
        RenewDLFlsInformation(0, beginAddress, space);
    }
    else if (flashDLNeedOtherTimeSignal == 1)
    {
        RenewDLFlsInformation(1, beginAddress, space);
    }
    else if (flashDLNeedOtherTimeSignal == 2)
    {
        RenewDLFlsInformation(2, beginAddress, space);
    }
    else
    {
        /** 超出最大缓存条数不保存 */
    }
}
void RecordEraseMemoryRange(const uint32 startAddr, const uint32 length)
{
    gs_routineEraseStart = startAddr;
    gs_routineEraseLen = length;
    gs_routineEraseValid = TRUE;
}
boolean GainEraseMemoryRange(uint32 *startAddr, uint32 *length)
{
    if ((NULL_PTR == startAddr) || (NULL_PTR == length) || (TRUE != gs_routineEraseValid))
    {
        return FALSE;
    }
    *startAddr = gs_routineEraseStart;
    *length = gs_routineEraseLen;
    return TRUE;
}
void ClearEraseMemoryRange(void)
{
    gs_routineEraseStart = 0u;
    gs_routineEraseLen = 0u;
    gs_routineEraseValid = FALSE;
}
/**
 * @brief 清空RAM内固件下载缓存区
 */
void RamFDErase(void)
{
    boolean ret = FALSE;
    uint32 beginAddr = 0u;
    uint32 endAddr = 0u;
    ret = FLASH_HAL_ReadDriverRange(&beginAddr, &endAddr);
    if (TRUE == ret)
    {
        Momory_Fill_Function((void *)beginAddr, 0x0u, endAddr - beginAddr);
    }
}
/**
 * @brief Flash后台任务调度总入口，UDS主循环周期调用，执行擦/写/CRC分时操作
 */
void FlashTaskRunLogic(void)
{
    boolean taskCompleteFlag = FALSE;
    flashOperationType flashTaskNow = flashDLStatusSingle.flashCurTask;
    if(flashTaskNow == FLASH_IS_IN_ERASING)
    {
        taskCompleteFlag = FALSE;
        flashDLStatusSingle.flashErrorStatus = ProcessEraseFlash(&taskCompleteFlag);
    }
    else if (flashTaskNow == FLASH_IS_IN_WRITING)
    {
        taskCompleteFlag = TRUE;
        flashDLStatusSingle.flashErrorStatus = ProcessWriteFlash(&taskCompleteFlag);
    }
    else if (flashTaskNow == FLASH_IS_IN_CHECK)
    {
        taskCompleteFlag = TRUE;
        flashDLStatusSingle.flashErrorStatus = ProcessVerifyCrc(&taskCompleteFlag);
    }
    else if (flashTaskNow == FLASH_IS_IN_WAITING)
    {
        // 分段等待任务，根据超时标记恢复任务或重置下载
        switch (reqTimeFlag)
        {
            case 1u:
                reqTimeFlag = 0xFFu;
#ifdef ALLOW_LIN_TP
                flashDLStatusSingle.flashCurTask = flashDLStatusSingle.newIntterpTask;
#else
                flashDLStatusSingle.flashCurTask = FLASH_IS_IN_ERASING;
#endif
                break;
            case 2u:
                reqTimeFlag = 0xFFu;
                eraseFlashStatus = BEGIN_STEP;
                PrepareDLInformation();
                SetCurFlsTaskPara(FLASH_IS_IN_IDLE, NULL_PTR, UDS_SERVICES_USELESS_ID, NULL_PTR);
                break;
            default:
                break;
        }
    }
    else
    {
        /** Flash空闲无操作 */
    }
    // 分段任务全部执行完成，调用操作完成回调回复诊断仪
    if (TRUE == taskCompleteFlag)
    {
        if(NULL_PTR != flashDLStatusSingle.taskEndFunc)
        {
            if(FLASH_IS_IN_IDLE != flashTaskNow)
            {
                (flashDLStatusSingle.taskEndFunc)(flashDLStatusSingle.flashErrorStatus);
                flashDLStatusSingle.taskEndFunc = NULL_PTR;
            }
        }
        // 操作失败重置全部下载状态
        if(flashDLStatusSingle.flashErrorStatus != TRUE)
        {
            if((FLASH_IS_IN_ERASING == flashTaskNow) ||
               (FLASH_IS_IN_WRITING == flashTaskNow) ||
               (FLASH_IS_IN_CHECK == flashTaskNow))
            {
                PrepareDLInformation();
            }
        }
        SetCurFlsTaskPara(FLASH_IS_IN_IDLE, NULL_PTR, UDS_SERVICES_USELESS_ID, NULL_PTR);
    }
}
/**
 * @brief 遍历A/B分区，返回当前版本最新、CRC合法的APP分区ID
 */
AppIdType GainNewestAPPId(void)
{
#ifdef APP_BICLE_UPDATA
    uint32 appABeginAddr = 0u;
    uint32 appABloSpace = 0u;
    appInfoType appAInformation;
    appInfoType appBInformation;
    uint32 appBBeginAddr = 0u;
    uint32 appBBloSpace = 0u;
#endif
#ifndef APP_BICLE_UPDATA
    return APP_A_ID; // 单分区固定返回A分区
#else
    FLASH_HAL_GetDetailOfAPP(APP_A_ID, &appABeginAddr, &appABloSpace);
    FLASH_HAL_GetDetailOfAPP(APP_B_ID, &appBBeginAddr, &appBBloSpace);
    appAInformation = *(appInfoType *)appABeginAddr;
    appBInformation = *(appInfoType *)appBBeginAddr;
    return (VerifyNewestAppInfo(&appAInformation, &appBInformation));
#endif
}
/**
 * @brief 保存诊断仪下发的预期固件CRC值
 */
void RecordRecCrcValue(uint32 num)
{
    flashDLStatusSingle.receivedCRC = (tCrc)num;
}
/**
 * @brief 校验当前APP分区擦除、写入、结构体标记是否全部正常
 * @return TRUE=分区完整可运行 FALSE=分区损坏
 */
uint8 FlashAppRationality(void)
{
/*
读取全局 RAM 变量`appInfoVar`里面**三个升级状态标志位**，
判断上一次固件下载（UDS 刷写）流程是不是完整顺利跑完，
APP 镜像是否标记为合法可用。
> 前置条件：前面已经调用过 `VerifyAppInfoFromFlsRationality()`，
已经把 Flash 上的`appInfoType`头部加载到全局 RAM `appInfoVar`
*/
    boolean ret = FALSE;
    if(TRUE == appInfoVar.flashWriteOkFlag)
    {
        if(TRUE == appInfoVar.flashEraseOkFlag)
        {
            if(TRUE == appInfoVar.flashStructOkFlag)
            {
                ret = TRUE;
            }
        }
    }
    return ret;
}
/**
 * @brief 上电初始化Flash下载全套状态缓存
 */
void PrepareFlsForApp(void)
{
#ifdef ALLOW_LIN_TP
    uint8 count = 0;
#endif
    flashDLStatusSingle.VerifyFingerPrintWrittenFlag = FALSE;
#ifdef UDS_PROJECT_FOR_BOOTLOADER
    RamFDErase();
#endif
    flashDLStatusSingle.VerifyFDDownloadedFlag = FALSE;
    SetNextDLPara(FLASH_DOWNLOAD_REQUEST);
    SetCurFlsTaskPara(FLASH_IS_IN_IDLE, NULL_PTR, UDS_SERVICES_USELESS_ID, NULL_PTR);
    flashDLStatusSingle.appStatusInFlash = &appInfoVar;
    Momory_Fill_Function(&flashDLStatusSingle.FlashOperationAPI, 0x0u, sizeof(Flash_OperationAPIType));
    Momory_Fill_Function(&appInfoVar, 0xFFu, sizeof(appInfoType));
    flashDLNeedOtherTimeSignal = 0;
#ifdef ALLOW_LIN_TP
    count = 0;
    while(count < LONGEST_SAVE_NUMBER)
    {
        flashDLStatusBuffer[count].flashRecContentBeginAddr = 0;
        flashDLStatusBuffer[count].flashRecContentSize = 0;
        flashDLStatusbackp[count].flashRecContentBeginAddr = 0;
        flashDLStatusbackp[count].flashRecContentSize = 0;
        count++;
    }
#endif
}
/**
 * @brief 取 APP Reset 入口：读向量表 [1]，不使用信息头里缓存的 appPosition
 * @note  分区首地址 + 0x200 为向量表，+4 为 Reset_Handler（Thumb 地址最低位为 1）
 *        APP 重编导致 Reset 搬家不影响；只有改 APP_A_BEGIN_ADDR / 向量偏移才要同步改 Boot
 */
uint32 GainAppPositionAddr(void)
{
	//原old return appInfoVar.appPosition;
    uint32 begin = APP_A_BEGIN_ADDR;
    uint32 space = 0u;

    if (TRUE != FLASH_HAL_GetDetailOfAPP(GainNewestAPPId(), &begin, &space))
    {
        begin = APP_A_BEGIN_ADDR;
    }

    return *((uint32 *)(begin + MOD_VECTOR_TABLE_OFFSET + MOD_RESET_HANDLER_OFFSET));
}
#ifdef APP_BICLE_UPDATA
/**
 * @brief 对比A/B分区头部CRC合法性，返回有效更新分区ID
 */
static AppIdType VerifyNewestAppInfo(const appInfoType *AApp, const appInfoType *BApp)
{
#ifdef APP_BICLE_UPDATA
    uint32 crc = 0u;
    AppIdType appId = APP_A_ID;
    boolean appARationality = FALSE;
    boolean appBRationality = FALSE;
#endif
    ASSERT(NULL_PTR == AApp);
    ASSERT(NULL_PTR == BApp);
#ifndef APP_BICLE_UPDATA
    return APP_A_ID;
#else
    crc = 0u;
    #ifdef ALLOW_LIN_TP
        CRC_HAL_ComputeSingleCRC((const uint8 *)AApp, sizeof(appInfoType) - 4u, &crc);
    #else
        CRC_HAL_CreatSw((const uint8 *)AApp, sizeof(appInfoType) - 4u, &crc);
    #endif
    appARationality = (crc == AApp->crc) ? TRUE : appARationality;
    crc = 0u;
    #ifdef ALLOW_LIN_TP
        CRC_HAL_ComputeSingleCRC((const uint8 *)BApp, sizeof(appInfoType) - 4u, &crc);  
    #else
        CRC_HAL_CreatSw((const uint8 *)BApp, sizeof(appInfoType) - 4u, &crc);
    #endif
    appBRationality = (crc == BApp->crc) ? TRUE : appBRationality;
    // 分区单一合法直接返回该分区
    if ((TRUE == appARationality) && (TRUE != appBRationality))
    {
        appId = APP_A_ID;
    }
    else if ((TRUE != appARationality) && (TRUE == appBRationality))
    {
        appId = APP_B_ID;
    }
    else if ((TRUE != appARationality) && (TRUE != appBRationality))
    {
        appId = APP_A_ID; // 双分区损坏默认启动A
    }
    else
    {
        // 双分区都合法，对比版本号取新分区
        appId = VerifyNewestApp(AApp, BApp);
    }
    return appId;
#endif
}
/**
 * @brief 对比两个合法APP的版本号，返回版本更新分区
 */
static AppIdType VerifyNewestApp(const appInfoType *AApp, const appInfoType *BApp)
{
    uint8 Acount = 0u;
    uint8 Bcount = 0u;
    uint8 subtraction = 0u;
    AppIdType appId = APP_A_ID;
    ASSERT(NULL_PTR == AApp);
    ASSERT(NULL_PTR == BApp);
    Acount = AApp->appNumber;
    Bcount = BApp->appNumber;
    if(Acount > Bcount)
    {
        subtraction = Acount - Bcount;
    }
    else
    {
        subtraction = Bcount - Acount;
    }
    switch (subtraction)
    {
        case 1U:
            appId = (Acount > Bcount) ? APP_A_ID : APP_B_ID;
            break;
        case 0xFEu:
             appId = (Acount < Bcount) ? APP_A_ID : APP_B_ID;
            break;
        default:
            // 版本号0xFF代表空分区
            if ((0xFFu == Acount) && (0xFFu != Bcount))
            {
                appId = APP_B_ID;
            }
            else if ((0xFFu != Acount) && (0xFFu == Bcount))
            {
                appId = APP_A_ID;
            }
            else if ((0xFFu == Acount) && (0xFFu == Bcount))
            {
                appId = APP_USELESS_ID;
            }
            else
            {
                appId = APP_A_ID;
            }
            break;
    }
    return appId;
}
#endif
/**
 * @brief 设置当前后台Flash任务、完成回调、绑定UDS服务、繁忙延长超时回调
 */
void SetCurFlsTaskPara(const flashOperationType task,
                       const ResponseFuncType func1,
                       const uint8 id,
                       const RequestOtherTimeFuncType func2)
{
    flashDLStatusSingle.flashCurTask = task;
    flashDLStatusSingle.taskEndFunc = func1;
    flashDLStatusSingle.udsSerIDrequested = id;
    flashDLStatusSingle.requestOtherTimePara = func2;
}
/**
 * @brief 获取上一次升级使用的分区ID，用于备份/主分区切换擦写
 * @param flag TRUE=取备份分区 FALSE=取主分区
 */
AppIdType GainPreviousAppId(boolean flag)
{
#ifdef APP_BICLE_UPDATA
    uint32 appAInfoBeginAddr = 0u;
    uint32 appBInfoBeginAddr = 0u;
    
    uint32 appABloSpace = 0u;
    uint32 appBBloSpace = 0u;
    
    AppIdType previousAppId = APP_A_ID;
    AppIdType newestAPPId = APP_A_ID;

    appInfoType appAInformation;
    appInfoType appBInformation;
#endif

#ifndef APP_BICLE_UPDATA
#ifdef APP_SINGLE_UPDATA
    if(flag)
    {
        return APP_A_BACKUP_ID;
    }
    else
#endif
    {
        return APP_A_ID;
    }

#else
    FLASH_HAL_GetDetailOfAPP(APP_A_ID, &appAInfoBeginAddr, &appABloSpace);
    FLASH_HAL_GetDetailOfAPP(APP_B_ID, &appBInfoBeginAddr, &appBBloSpace);

    appAInformation = *(appInfoType *)appAInfoBeginAddr;
    appBInformation = *(appInfoType *)appBInfoBeginAddr;

    newestAPPId = VerifyNewestAppInfo(&appAInformation, &appBInformation);

    switch (newestAPPId)
    {
        case APP_A_ID:
            previousAppId = APP_B_ID;
            break;

        default:
            previousAppId = APP_A_ID;
            break;
    }

    return previousAppId;
#endif
}

uint8 ProcessProgramRegion(const uint32 addr, const uint8 *des, const uint32 space)
{
    uint8 length = (uint8)space;
    uint8 ret = TRUE;

    ASSERT(NULL_PTR == des);
    ret = TRUE;

    ret = (FLASH_DOWNLOAD_TRANSFER != GainCurDLPara()) ? FALSE : TRUE;
    //将数据拷贝到RAM缓冲区
    ret = (TRUE != RecordData(des, length)) ? FALSE : ret;

    if (TRUE != ret)
    {
        PrepareDLInformation();
    }
    else
    {
        // 如果下载完成标志位为真，并且检查Flash驱动数据为假，则设置当前Flash任务为写Flash确认，错误状态为真，意思是flash下载好了
        if ((TRUE == flashDLStatusSingle.VerifyFDDownloadedFlag) && (FALSE == CheckFlsDrvSwData()))
        {
            SetCurFlsTaskPara(FLASH_IS_IN_WRITING, &UDS_APP_WriteFlashAck, UDS_SERVICES_USELESS_ID, NULL_PTR);
            flashDLStatusSingle.flashErrorStatus = TRUE;
        }
        else
        {
            // 如果检查Flash驱动数据为真，则将数据从 des 拷贝到 addr
            if (TRUE == CheckFlsDrvSwData())
            { //FLASH DRIVER  数据从 des 拷贝到 addr，然后设置当前Flash任务为空闲
                Momory_Copy_Function((void *)addr, (void *)des, length);
            }

            SetCurFlsTaskPara(FLASH_IS_IN_IDLE, NULL_PTR, UDS_SERVICES_USELESS_ID, NULL_PTR);
        }
    }

    return ret;
}

/**
 * @brief 擦 1 个扇区作废 APP 信息头（全 0xFF，标志不再等于 TRUE）
 * @note  必须用 RAM 驱动；第 2 参是扇区个数 1，不是字节数
 */
static uint8 InvalidateAppInfoPage(uint32 appBeginAddr)
{
    boolean eraseOk = FALSE;

    if (NULL_PTR == flashDLStatusSingle.FlashOperationAPI.pfSectorRemove)
    {
        return FALSE;
    }
    if (0u != (appBeginAddr & ((uint32)MOD_SECTOR_SIZE - 1u)))
    {
        FlashDebugLog("InvalidateAppInfo: addr not page aligned 0x%x\r\n", appBeginAddr);
        return FALSE;
    }

    FLASH_HAL_ClearFmcErrorFlags();
    WATCHDOG_HAL_Fed();
    DisableAllInterrupts();
    eraseOk = flashDLStatusSingle.FlashOperationAPI.pfSectorRemove(appBeginAddr, 1u);
    EnableAllInterrupts();
    if (TRUE != eraseOk)
    {
        FlashDebugLog("InvalidateAppInfo: erase fail 0x%x\r\n", appBeginAddr);
    }
    return eraseOk;
}

/**
 * @brief 0x31 0x0203 失败：清 RAM 标志并再擦信息头，避免 FF01 把坏镜像标成可跳转
 */
void MarkAppDownloadIntegrityFailed(void)
{
    tCrc crcValue = 0u;

    s_dataIntegrityPassed = FALSE;
    appInfoVar.flashWriteOkFlag = FALSE;
    appInfoVar.flashEraseOkFlag = FALSE;
    appInfoVar.flashStructOkFlag = FALSE;
    ProduceAndSetAppCrc(&crcValue);
    /* 过渡刷 Boot 失败时不要擦自己的 APP 头，否则本程序也跳不回来 */
    FlashDebugLog("0x0203 fail: keep transition app header\r\n");
}

void ResetProgrammingCycleFlags(void)
{
    tCrc crcValue = 0u;

    s_dataIntegrityPassed = FALSE;
    s_appDataWrittenThisCycle = FALSE;
    appInfoVar.flashWriteOkFlag = FALSE;
    appInfoVar.flashEraseOkFlag = FALSE;
    appInfoVar.flashStructOkFlag = FALSE;
    ProduceAndSetAppCrc(&crcValue);
}

uint8 IsFlashEraseCompleted(void)
{
    return (TRUE == appInfoVar.flashEraseOkFlag) ? TRUE : FALSE;
}

void MarkAppDownloadIntegrityPassed(void)
{
    s_dataIntegrityPassed = TRUE;
}

uint8 IsAppDownloadIntegrityPassed(void)
{
    return s_dataIntegrityPassed;
}

uint8 FinalizeAppDownloadTransfer(void)
{
    tCrc crcValue = 0u;

    /* Driver → RAM 的 37：不查擦写标志、不置写成功，后面走 0202 */
    if (TRUE == IsCurrentDownloadFlashDriver())
    {
        return TRUE;
    }
    /* APP PFlash 的 37 */
    if ((TRUE != s_appDataWrittenThisCycle) ||
        (TRUE != appInfoVar.flashEraseOkFlag) ||
        (TRUE != appInfoVar.flashStructOkFlag))
    {
        FlashDebugLog("0x37 APP: writeOk not set w=%u e=%u s=%u\r\n",
                      s_appDataWrittenThisCycle,
                      appInfoVar.flashEraseOkFlag,
                      appInfoVar.flashStructOkFlag);
        return FALSE;
    }

    appInfoVar.flashWriteOkFlag = TRUE;
    ProduceAndSetAppCrc(&crcValue);
    FlashDebugLog("0x37 APP: flashWriteOkFlag set\r\n");
    return TRUE;
}

uint8 InvalidateAppHeaderAfterBootOk(void)
{
    return InvalidateAppInfoPage(APP_A_BEGIN_ADDR);
}

uint8 IsAppResetHandlerValid(uint32 resetAddr)
{
    uint32 dest;

    /* 全 0 / 全 F */
    if ((0u == resetAddr) || (0xFFFFFFFFu == resetAddr))
    {
        return FALSE;
    }
    /* Cortex-M Thumb 入口必须为奇数 */
    if (0u == (resetAddr & 1u))
    {
        return FALSE;
    }
    dest = resetAddr & 0xFFFFFFFEu;
    /* APP 代码区：[信息头之后, APP 结束) */
    if ((dest < (APP_A_BEGIN_ADDR + APP_INFORMATION_SIZE)) ||
        (dest >= APP_A_END_ADDR))
    {
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief 写 APP 信息头：先擦 1 个扇区再编程
 * @note  PFlash 必须走已下载到 RAM 的驱动，不能调 Fmc_ErasePage（Boot 跑在 PFlash）
 *        pfSectorRemove 第 2 参是扇区个数，1 扇区 = MOD_SECTOR_SIZE(512) = APP_INFORMATION_SIZE
 *        pfDataWriter 内部按 8 字节（FMC_LEN_64BIT）补齐，与 RAM 驱动 FLASH_Program 一致
 */
static uint8 WriteAppInfoPage(uint32 appBeginAddr, appInfoType *appInfoStruct)
{
    boolean eraseOk = FALSE;
    uint8 writeOk = FALSE;
    uint32 off = 0u;
    /* 按整页 512B 写，与擦除扇区、FLASH_OPERATE_SIZE_BYTE(128) 对齐 */
    static uint8 pageBuf[APP_INFORMATION_SIZE] __attribute__((aligned(4)));

    if ((NULL_PTR == flashDLStatusSingle.FlashOperationAPI.pfSectorRemove) ||
        (NULL_PTR == flashDLStatusSingle.FlashOperationAPI.pfDataWriter) ||
        (NULL_PTR == appInfoStruct))
    {
        return FALSE;
    }
    /* 信息头必须落在扇区首地址，结构体不能跨到下一页向量表 */
    if ((0u != (appBeginAddr & ((uint32)MOD_SECTOR_SIZE - 1u))) ||
        (sizeof(appInfoType) > APP_INFORMATION_SIZE))
    {
        FlashDebugLog("WriteAppInfo: addr/size mismatch 0x%x %u\r\n",
                      appBeginAddr, (uint32)sizeof(appInfoType));
        return FALSE;
    }

    FLASH_HAL_ClearFmcErrorFlags();
    WATCHDOG_HAL_Fed();
    DisableAllInterrupts();
    /* 第 2 参是扇区个数，与 0x31 FF00 擦 APP 相同：每次 1 页 512B */
    eraseOk = flashDLStatusSingle.FlashOperationAPI.pfSectorRemove(appBeginAddr, 1u);
    EnableAllInterrupts();
    if (TRUE != eraseOk)
    {
        FlashDebugLog("WriteAppInfo: erase page fail 0x%x\r\n", appBeginAddr);
        return FALSE;
    }

    Momory_Fill_Function(pageBuf, 0xFFu, APP_INFORMATION_SIZE);
    Momory_Copy_Function(pageBuf, (uint8 *)appInfoStruct, sizeof(appInfoType));
    writeOk = TRUE;
    for (off = 0u; off < APP_INFORMATION_SIZE; off += FLASH_OPERATE_SIZE_BYTE)
    {
        WATCHDOG_HAL_Fed();
        writeOk = flashDLStatusSingle.FlashOperationAPI.pfDataWriter(
                      appBeginAddr + off, &pageBuf[off], FLASH_OPERATE_SIZE_BYTE);
        if (TRUE != writeOk)
        {
            FlashDebugLog("WriteAppInfo: program fail 0x%x\r\n", appBeginAddr + off);
            break;
        }
    }
    return writeOk;
}

uint8 WriteAppInfoToFls(void)
{
    uint8 ret = FALSE;
    uint32 size = 0u;
    uint32 offset = 0u;
    uint32 crcValue = 0u;
    uint32 appInfoSize = 0u;
    uint32 appBeginAddr = 0u;
    uint32 nextAppInforSize = 0u;
    uint32 nextAppInforBeginAddr = 0u;
    appInfoType *appInfoStruct = NULL_PTR;
    boolean writeEnable = FALSE;
#ifdef ALLOW_LIN_TP
    uint8 count = 0;
#endif

    ProduceAndSetAppCrc(&crcValue);

#ifdef APP_BICLE_UPDATA
    ret = TRUE;
    appBeginAddr = doubleRenewDLAppInfoAddr;
#else
    ret = FLASH_HAL_GetDetailOfAPP(GainPreviousAppId(backupEraseFlag), &appBeginAddr, &appInfoSize);
#endif

    switch (ret)
    {
        case TRUE:
            appInfoStruct = &appInfoVar;
            FLASH_HAL_GetResetHandlerDetails(&writeEnable, &offset, &size);
    
            if (TRUE == FLASH_HAL_GetDetailOfAPP(GainNewestAPPId(), &nextAppInforBeginAddr, &nextAppInforSize))
            {
                appInfoStruct->appNumber = (((appInfoType *)nextAppInforBeginAddr)->appNumber) + 1u;
                appInfoStruct->appNumber = (0xFFu == appInfoStruct->appNumber) ? (0u):(appInfoStruct->appNumber);
#ifdef ALLOW_LIN_TP
                count = 0;
                while(count < LONGEST_SAVE_NUMBER)
                {
                    appInfoStruct->flashRecContentParaBuffer[count].flashRecContentSize = flashDLStatusbackp[count].flashRecContentSize;
                    appInfoStruct->flashRecContentParaBuffer[count].flashRecContentBeginAddr = flashDLStatusbackp[count].flashRecContentBeginAddr;
                    count++;
                }
#endif
                appInfoVar.appBeginAddrSize = size;
                appInfoVar.appPosition = *((uint32 *)(appBeginAddr + offset));
    
                crcValue = 0u;
                ProduceAndSetAppCrc(&crcValue);
            }
            /* 仅当本次擦写都成功、0203 已通过、Reset 合法才把头标成有效 */
            if ((TRUE != appInfoVar.flashWriteOkFlag) ||
                (TRUE != appInfoVar.flashEraseOkFlag) ||
                (TRUE != appInfoVar.flashStructOkFlag))
            {
                FlashDebugLog("WriteAppInfo: flags not ready w=%u e=%u s=%u\r\n",
                              appInfoVar.flashWriteOkFlag,
                              appInfoVar.flashEraseOkFlag,
                              appInfoVar.flashStructOkFlag);
                ret = FALSE;
                break;
            }
            if (TRUE != IsAppDownloadIntegrityPassed())
            {
                FlashDebugLog("WriteAppInfo: 0x0203 not passed\r\n");
                ret = FALSE;
                break;
            }
            {
                uint32 resetAddr = GainAppPositionAddr();
                if (TRUE != IsAppResetHandlerValid(resetAddr))
                {
                    FlashDebugLog("WriteAppInfo: reset addr invalid 0x%x\r\n", resetAddr);
                    ret = FALSE;
                    break;
                }
            }
#ifdef ALLOW_LIN_TP
            if(NULL_PTR == appInfoStruct)
            {
                ret = FALSE;
            }
            else
            {
                ret = FALSE;
              #ifndef EN_APP_INFO_DATA_IN_NONE_FLASH
                if(NULL_PTR == flashDLStatusSingle.FlashOperationAPI.pfDataWriter)
                {
                    /** do nothing */
                }
                else
                {
                    ret = WriteAppInfoPage(appBeginAddr, appInfoStruct);
                }
              #else
                ret = FLASH_HAL_WriteAPPInfoData(appBeginAddr,(uint8 *)appInfoStruct,sizeof(appInfoType));
              #endif
            }
#else
            if ((NULL_PTR == flashDLStatusSingle.FlashOperationAPI.pfDataWriter) || (NULL_PTR == appInfoStruct))
            {
                ret = FALSE;
            }
            else
            {
                ret = WriteAppInfoPage(appBeginAddr, appInfoStruct);
            }
#endif
            if (TRUE == ret)
            {
                FlashDebugLog("WriteAppInfo: marked valid, reset=0x%x\r\n",
                              appInfoVar.appPosition);
            }
            break;

        default:
            /** do nothing */
            break;
    }

    (void)appInfoSize;

    return ret;
}

FlashDownloadType GainCurDLPara(void)
{
    return flashDLStatusSingle.flashDownloadPara;
}

/* ================================================================
 * 获取已下载分段数量
 * ================================================================ */
uint32_t UDS_APP_GetDownloadSegmentCount(void)
{
    return flashDLNeedOtherTimeSignal;
}
/* ================================================================
 * 获取指定分段的起始地址和长度
 * ================================================================ */
boolean UDS_APP_GetDownloadSegmentInfo(uint32_t index, uint32_t* pStartAddr, uint32_t* pLength)
{
    if (pStartAddr == NULL || pLength == NULL) {
        return FALSE;
    }
    
    if (index >= flashDLNeedOtherTimeSignal) {
        return FALSE;
    }
    
    *pStartAddr = flashDLStatusBuffer[index].flashRecContentBeginAddr;
    *pLength = flashDLStatusBuffer[index].flashRecContentSize;
    
    return TRUE;
}
