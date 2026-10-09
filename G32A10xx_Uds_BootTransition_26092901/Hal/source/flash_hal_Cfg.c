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
#include "flash_hal_Cfg.h"

/**************************************************************************
                    全局配置变量定义区
**************************************************************************/
/* 多核分区配置，当前工程单核MCU，预留多核适配 */
#if (MCU_CORE_NUMBER >= 1u)
/* 主程序A区、主程序B区、镜像A、镜像B、重映射地址 */
static const Flash_coreInformationType gs_multiCoreConfig[MCU_CORE_NUMBER] =
{
    {
        0x1000000u, 0x1200000u, 0xA000000u, 0xA200000u, 0x2000000u
    },
};
#endif

/* Flash驱动固件存储分区起始/结束地址 */
const Flash_blockInformationType g_flashDriverConfig[] =
{
    {FLS_DRV_BEGIN_ADDR, FLS_DRV_END_ADDR},
};

/* APP主分区A 起始、结束地址（单分区/双分区主程序运行区） */
const Flash_blockInformationType g_blocklogicalA[] =
{
    {APP_A_BEGIN_ADDR, APP_A_END_ADDR},    /* Block logical A 主运行分区 */
};
/* APP A分区数量 */
const uint32 g_logicalNumA = sizeof(g_blocklogicalA) / sizeof(g_blocklogicalA[0u]);

#ifdef APP_BICLE_UPDATA
/* 双分区升级：备用程序B分区地址 */
const Flash_blockInformationType g_blocklogicalB[] =
{
    {APP_B_BEGIN_ADDR, APP_B_END_ADDR},    /* Block logical B 备用升级分区 */
};
/* B分区数量 */
const uint32 g_logicalNumB = sizeof(g_blocklogicalB) / sizeof(g_blocklogicalB[0u]);
#endif

#ifdef APP_SINGLE_UPDATA
/* 单分区升级：A程序备份分区，升级时临时存放旧APP */
const Flash_blockInformationType g_blocklogicalABackup[] =
{
    {APP_A_BACKUP_BEGIN_ADDR, APP_A_BACKUP_END_ADDR},    /* Block logical A Backup 备份分区 */
};
/* 备份分区数量 */
const uint32 g_logicalNumABackup = sizeof(g_blocklogicalABackup) / sizeof(g_blocklogicalABackup[0u]);
#endif

/**************************************************************************
                    全局配置接口函数区
**************************************************************************/
/*!
* @brief    扇区号转换为对应Flash物理起始地址
* @param[in]    rType       分区类型：A主分区/B分区/备份分区
* @param[in]    rSectorNo   输入扇区编号
* @param[out]   pOutAddr    输出对应扇区首地址
* @retval   boolean：TRUE转换成功，FALSE扇区号越界
*/
boolean FLASH_HAL_BOASecNumToAddr(const AppIdType rType, const uint32 rSectorNo, uint32 *pOutAddr)
{
    uint32 u32AddrTemp = 0u; // 临时地址计算变量
    const uint32 u32AddrMask = (MOD_SECTOR_SIZE) - 1u; // 扇区大小掩码，取扇区内偏移
    uint32 u32TempSec = 0u; // 遍历临时扇区计数
    uint32 u32Idx = 0u; // 循环索引
    uint32 u32TotalSec = 0u; // 当前分区总扇区数量
    uint32 u32BlockCount = 0u; // 当前分区配置块数量
    Flash_blockInformationType *pLocalBlock = NULL_PTR; // 当前分区配置结构体指针
    boolean bRetVal = FALSE; // 函数返回结果标记

    // 校验分区配置是否合法，获取分区配置指针与块数量
    if (FALSE == FLASH_HAL_InspectFlashConfiguration(rType, &pLocalBlock, &u32BlockCount))
    {
        // 分区类型不存在，直接返回失败
    }
    else
    {
        // 计算该分区总扇区数
        u32TotalSec = FLASH_HAL_CalcFlashSec(rType);
        // 输入扇区号大于等于总扇区，越界返回false
        if (rSectorNo >= u32TotalSec)
        {
            bRetVal = FALSE;
        }
        else
        {
            // 遍历分区所有存储块
            for (u32Idx = 0u; u32Idx < u32BlockCount; u32Idx++)
            {
                // 清除地址低字节偏移，对齐扇区首地址
                u32AddrTemp = pLocalBlock[u32Idx].startLogAddr & u32AddrMask;
                if (!u32AddrTemp)
                {
                    // 起始地址刚好扇区对齐，扇区计数不变
                }
                else
                {
                    u32TempSec += 1u;
                }
                // 修正块起始地址为扇区对齐地址
                u32AddrTemp = pLocalBlock[u32Idx].startLogAddr - u32AddrTemp;
                // 按扇区步长遍历整块地址
                for ( ; u32AddrTemp < pLocalBlock[u32Idx].endLogAddr; u32AddrTemp += MOD_SECTOR_SIZE)
                {
                    // 匹配到目标扇区号，赋值输出地址
                    if (u32TempSec == rSectorNo)
                    {
                        *pOutAddr = u32AddrTemp;
                        bRetVal = TRUE;
                        break;
                    }
                    u32TempSec++;
                }
                // 找到扇区直接跳出外层循环
                if (FALSE == bRetVal)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }
    return bRetVal;
}

/*!
* @brief    根据分区类型计算该分区总扇区数量
* @param[in]    paramApp 分区类型APP_A/APP_B/APP_A_BACKUP
* @retval   uint32 分区总扇区个数
*/
uint32 FLASH_HAL_CalcFlashSec(const AppIdType paramApp)
{
    uint32 u32TotalSectorNum = 0u; // 总扇区计数
    Flash_blockInformationType *pBlockData = NULL_PTR; // 分区配置指针
    uint32 u32ItemCount = 0u; // 分区块数量
    uint32 u32FlashLen = 0u; // 单块Flash总字节长度
    uint32 u32Index = 0u; // 循环索引

    // 获取分区配置信息
    if (FALSE == FLASH_HAL_InspectFlashConfiguration(paramApp, &pBlockData, &u32ItemCount))
    {
        // 无合法分区，扇区总数0
    }
    else
    {
        // 遍历分区所有存储块，累加扇区
        while (u32Index < u32ItemCount)
        {
            // 单块总字节 = 结束地址 - 起始地址
            u32FlashLen = pBlockData[u32Index].endLogAddr - pBlockData[u32Index].startLogAddr;
            // 计算当前块占用扇区并累加总数量
            u32TotalSectorNum += FLASH_HAL_CalcSectorCount(pBlockData[u32Index].startLogAddr, u32FlashLen);
            u32Index++;
        }
    }
    return u32TotalSectorNum;
}

/*!
* @brief    获取Flash驱动固件分区首尾地址
* @param[out]    pOutDrvStart 驱动区起始地址
* @param[out]    pOutDrvEnd   驱动区结束地址
* @retval   boolean 固定返回TRUE
*/
boolean FLASH_HAL_ReadDriverRange(uint32 *pOutDrvStart, uint32 *pOutDrvEnd)
{
    ASSERT(NULL_PTR == pOutDrvStart);
    ASSERT(NULL_PTR == pOutDrvEnd);
    // 读取全局驱动配置数组首项起始地址
    *pOutDrvStart = g_flashDriverConfig[0u].startLogAddr;
    // 读取全局驱动配置数组首项结束地址
    *pOutDrvEnd   = g_flashDriverConfig[0u].endLogAddr;
    return TRUE;
}

/*!
* @brief    获取复位中断向量配置参数：使能标记、偏移、长度
* @param[out]    pIsEnableResetHandler 复位向量使能
* @param[out]    pRstHandlerOffset     复位向量偏移地址
* @param[out]    pRstHandlerLength     复位向量占用长度
* @retval   无
*/
void FLASH_HAL_GetResetHandlerDetails(boolean *pIsEnableResetHandler, 
                                      uint32 *pRstHandlerOffset, 
                                      uint32 *pRstHandlerLength)
{
    ASSERT(NULL_PTR == pIsEnableResetHandler);
    ASSERT(NULL_PTR == pRstHandlerOffset);
    ASSERT(NULL_PTR == pRstHandlerLength);
    // 复位处理函数全局使能宏赋值
    *pIsEnableResetHandler = EN_MOD_RESET_HANDLER;
    // 向量表基地址 + 复位函数偏移 = 实际复位地址
    *pRstHandlerOffset = (MOD_VECTOR_TABLE_OFFSET + MOD_RESET_HANDLER_OFFSET);
    // 复位向量存储字节长度
    *pRstHandlerLength = MOD_RESET_HANDLER_ADDR_LEN;
}

/*!
* @brief    根据分区ID获取分区起始地址与总字节长度
* @param[in]    paramAppType 分区类型A/B/备份
* @param[out]   outStartAddress 分区首地址
* @param[out]   outBlockLen     分区总字节长度
* @retval   boolean TRUE找到对应分区，FALSE无匹配分区
*/
boolean FLASH_HAL_GetDetailOfAPP(const AppIdType paramAppType, uint32 *outStartAddress, uint32 *outBlockLen)
{
    boolean result = FALSE;
    
    if (APP_A_ID == paramAppType)
    {
        // 主运行A分区
        *outStartAddress = g_blocklogicalA[0u].startLogAddr;
        *outBlockLen = g_blocklogicalA[0u].endLogAddr - g_blocklogicalA[0u].startLogAddr;
        result = TRUE;
    }
#ifdef APP_SINGLE_UPDATA
    else if(APP_A_BACKUP_ID == paramAppType)
    {
        // 单分区升级备份分区
        *outStartAddress = g_blocklogicalABackup[0u].startLogAddr;
        *outBlockLen = g_blocklogicalABackup[0u].endLogAddr - g_blocklogicalABackup[0u];
        result = TRUE;
    }
#endif
    else
    {
#ifdef APP_BICLE_UPDATA
        if (APP_B_ID == paramAppType)
        {
            // 双分区备用B分区
            *outStartAddress = g_blocklogicalB[0u].startLogAddr;
            *outBlockLen = g_blocklogicalB[0u].endLogAddr - g_blocklogicalB[0u].startLogAddr;
            result = TRUE;
        }
#endif
    }
    return result;
}

/*!
* @brief    根据分区ID获取分区配置结构体指针与块总数
* @param[in]    iLocalAppType 分区ID
* @param[out]   pOutBlockInfo 分区配置数组指针
* @param[out]   pOutItemCount 分区存储块数量
* @retval   boolean TRUE存在该分区配置
*/
boolean FLASH_HAL_InspectFlashConfiguration(const AppIdType iLocalAppType,
                                            Flash_blockInformationType **pOutBlockInfo,
                                            uint32 *pOutItemCount)
{
    boolean bFuncResult = FALSE;
    // 判断是否为主运行A分区
    if (APP_A_ID != iLocalAppType)
    {
#ifdef APP_SINGLE_UPDATA
        // 判断是否为A备份分区
        if (APP_A_BACKUP_ID == iLocalAppType)
        {
            *pOutBlockInfo = (Flash_blockInformationType *)g_blocklogicalABackup;
            *pOutItemCount = g_logicalNumABackup;
            bFuncResult = TRUE;
        }
#endif
#ifdef APP_BICLE_UPDATA
        // 判断是否为备用B分区
        if (APP_B_ID == iLocalAppType)
        {
            *pOutBlockInfo = (Flash_blockInformationType *)g_blocklogicalB;
            *pOutItemCount = g_logicalNumB;
            bFuncResult = TRUE;
        }
#endif
    }
    else
    {
        // 主A分区配置赋值
        *pOutBlockInfo = (Flash_blockInformationType *)g_blocklogicalA;
        *pOutItemCount = g_logicalNumA;
        bFuncResult = TRUE;
    }
    return bFuncResult;
}

/*!
* @brief    根据起始地址+长度计算占用多少Flash扇区
* @param[in]    p_initAddr 分区起始地址
* @param[in]    p_dataLen  分区总字节长度
* @retval   uint32 占用扇区总数
*/
uint32 FLASH_HAL_CalcSectorCount(const uint32 p_initAddr, const uint32 p_dataLen)
{
    uint32 tempSectorCount = 0u; // 扇区计数
    const uint32 localMask = (MOD_SECTOR_SIZE) - 1u; // 扇区掩码
    uint32 tempFlashAddr = 0u;

    // 取出起始地址在扇区内的偏移量
    tempFlashAddr = (p_initAddr & localMask);
    // 分区长度大于单个扇区
    if (p_dataLen > MOD_SECTOR_SIZE)
    {
        // 先整除得到完整扇区
        tempSectorCount = p_dataLen / MOD_SECTOR_SIZE;
        // 存在剩余字节，额外加1个扇区
        if (0u != (p_dataLen & localMask))
        {
            tempSectorCount += 1u;
        }
        // 起始地址非扇区对齐，跨扇区边界再多占用1扇区
        if ((0u != tempFlashAddr) && (tempFlashAddr != ((tempFlashAddr + p_dataLen) & localMask)))
        {
            tempSectorCount += 1u;
        }
    }
    else
    {
        // 长度小于等于1个扇区
        tempFlashAddr += p_dataLen;
        // 起始偏移+长度超出当前扇区，占用2扇区，否则1扇区
        if (!(tempFlashAddr <= MOD_SECTOR_SIZE))
        {
            tempSectorCount = 2u;
        }
        else
        {
            tempSectorCount = 1u;
        }
    }
    return tempSectorCount;
}

/*!
* @brief    多核场景：获取APP重映射地址
* @param[in]    p_appCategory 分区ID
* @param[in]    p_coreIndex   核心编号
* @param[out]   p_remapResult 重映射地址输出
* @retval   boolean TRUE匹配核心与分区，输出地址有效
*/
boolean FLASH_HAL_UpdateMultiCoreRemapAddr(const AppIdType p_appCategory, const uint32 p_coreIndex, uint32 *p_remapResult)
{
    boolean r_status = FALSE;
#if (MCU_CORE_NUMBER >= 1)
    // 主A分区+合法核心号
    if (!(APP_A_ID != p_appCategory || p_coreIndex >= MCU_CORE_NUMBER))
    {
        *p_remapResult = gs_multiCoreConfig[p_coreIndex].remapAppAddr;
        r_status = TRUE;
    }
    else
    {
#ifdef APP_BICLE_UPDATA
        // 双分区B分区合法核心号
        if ((APP_B_ID == p_appCategory) && (p_coreIndex < MCU_CORE_NUMBER))
        {
            *p_remapResult = gs_multiCoreConfig[p_coreIndex].remapAppAddr;
            r_status = TRUE;
        }
#endif
    }
#endif
    return r_status;
}

/*!
* @brief    获取分区镜像存储地址（多核镜像备份区）
* @param[in]    iLocalAppType 分区ID
* @param[in]    iLocalCoreNo  核心编号
* @param[out]   pOutMirrorAddr 镜像地址输出
* @retval   boolean TRUE地址有效
*/
boolean FLASH_HAL_AcquireMultiRemapInfoAddress(const AppIdType iLocalAppType, 
                                               const uint32 iLocalCoreNo, 
                                               uint32 *pOutMirrorAddr)
{
    boolean bAcquisitionSuccess = FALSE;
#if (MCU_CORE_NUMBER >= 1U)
    // A分区镜像地址
    if ((iLocalAppType == APP_A_ID) && (iLocalCoreNo < MCU_CORE_NUMBER))
    {
        *pOutMirrorAddr = gs_multiCoreConfig[iLocalCoreNo].mirrorAStartAddr;
        bAcquisitionSuccess = TRUE;
    }
#ifdef APP_BICLE_UPDATA
    // B分区镜像地址
    if ((iLocalAppType == APP_B_ID) && (iLocalCoreNo < MCU_CORE_NUMBER))
    {
        *pOutMirrorAddr = gs_multiCoreConfig[iLocalCoreNo].mirrorBStartAddr;
        bAcquisitionSuccess = TRUE;
    }
#endif
#endif
    return bAcquisitionSuccess;
}

/*!
* @brief    校验所有APP分区起始/结束地址是否扇区对齐
* @retval   boolean 全部对齐返回TRUE，存在不对齐地址返回FALSE
*/
boolean FLASH_HAL_VerifyAppFlashConfiguration(void)
{
    Flash_blockInformationType *pLocalBlock = NULL_PTR;
    uint32 remainingItems = 0u;
    const uint32 localAddrMask = MOD_SECTOR_SIZE - 1u;

    // 校验A分区地址对齐
    if (TRUE != FLASH_HAL_InspectFlashConfiguration(APP_A_ID, &pLocalBlock, &remainingItems))
    {
    }
    else
    {
        for (; remainingItems > 0u; remainingItems--)
        {
            // 起始/结束地址必须是扇区大小整数倍
            if (((pLocalBlock->startLogAddr & localAddrMask) != 0u) ||
                ((pLocalBlock->endLogAddr & localAddrMask)   != 0u))
            {
                return FALSE;
            }
            pLocalBlock++;
        }
    }
#ifdef APP_BICLE_UPDATA
    // 双分区模式校验B分区
    if (TRUE != FLASH_HAL_InspectFlashConfiguration(APP_B_ID, &pLocalBlock, &remainingItems))
    {
    }
    else
    {
        for (; remainingItems > 0u; remainingItems--)
        {
            if (((pLocalBlock->startLogAddr & localAddrMask) != 0u) ||
                ((pLocalBlock->endLogAddr & localAddrMask)   != 0u))
            {
                return FALSE;
            }
            pLocalBlock++;
        }
    }
#endif
    // 所有分区地址均扇区对齐
    return TRUE;
}
