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
#include "flash_hal.h"
#include "flash.h"

/**************************************************************************
                    本地静态函数声明区
**************************************************************************/
/*!
* @brief    Flash模块反初始化函数
* @retval   无
*/
static void FLASH_HAL_Deinit(void)
{
    // 打印当前函数名调试日志
    FlashDebugLog("\n %s\n", __func__);
}

/*!
* @brief    Flash模块初始化，加载底层Flash驱动接口
* @retval   boolean：TRUE初始化成功
*/
static boolean FLASH_HAL_Init(void)
{
    // 调用底层Flash API初始化接口
    InitFlashAPI();
    return TRUE;
}

/*!
* @brief    读取Flash指定地址数据到缓存
* @param[in]    u32Addr    Flash读取起始地址
* @param[in]    Len        需要读取的数据长度
* @param[in]    DataBuff   数据接收缓存指针
* @retval   boolean：TRUE读取成功，FALSE读取失败
*/
static boolean FLASH_HAL_ReadData(const uint32 u32Addr,
                                  const uint32 Len,
                                  uint8 *DataBuff)
{
    // 打印当前函数名调试日志
    FlashDebugLog("\n %s\n", __func__);
    return TRUE;
}

/*!
* @brief    Flash写入适配函数，自动补齐8字节对齐（极海Flash按8字节双字编程）
* @param[in]    u32Addr    Flash写入起始地址
* @param[in]    pu8Data    待写入数据缓冲区指针
* @param[in]    u32Length   待写入数据总长度
* @retval   boolean：TRUE写入成功，FALSE写入失败
*/
static boolean FLASH_HAL_ModifiedFlashWriteData(const uint32 u32Addr, const uint8 *pu8Data, const uint32 u32Length)
{
    uint8 u8Idx = 0u; // 循环索引
    uint32 u32TempLen = 0u; // 8字节整数倍数据长度
    // 临时8字节缓存，Flash空白默认0xFF
    uint8 au8Buff[8u] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu};
    uint8 u8AlignLen = 8u; // Flash编程对齐字节（双字8字节）
    boolean bRet = FALSE; // 写入返回状态标记

    DisableAllInterrupts(); // 关闭全局中断，防止写入过程被打断

    // 判断长度是否刚好8字节整数倍，无需补齐
    if (!(u32Length & (u8AlignLen - 1)))
    {
        // 底层Flash写入返回0代表成功
        if (0u == WriteFlash(u32Addr, pu8Data, u32Length))
        {
            bRet = TRUE;
        }
    }
    else
    {
        // 数据长度不足8字节/存在剩余字节，需要补齐0xFF
        if (!(u32Length > u8AlignLen))
        {
            // 总长度小于8字节，全部拷贝到临时缓存，剩余填充0xFF
            u8Idx = 0u;
            while (u8Idx < u32Length)
            {
                au8Buff[u8Idx] = pu8Data[u32TempLen + u8Idx];
                u8Idx++;
            }
            // 写入完整8字节双字
            if (0u == WriteFlash(u32Addr + u32TempLen, au8Buff, 8u))
            {
                bRet = TRUE;
            }
        }
        else
        {
            // 长度大于8字节，先写入前面完整8字节块
            u32TempLen = u32Length - (u32Length & (u8AlignLen - 1));
            if (0u == WriteFlash(u32Addr, pu8Data, u32TempLen))
            {
                bRet = TRUE;
            }
            else
            {
                bRet = FALSE;
            }

            // 前面块写入成功，处理末尾不足8字节剩余数据
            if (TRUE == bRet)
            {
                u8Idx = 0u;
                while (u8Idx < (u32Length & (u8AlignLen - 1)))
                {
                    au8Buff[u8Idx] = pu8Data[u32TempLen + u8Idx];
                    u8Idx++;
                }
                // 补齐后写入最后一个8字节块
                if (0u == WriteFlash(u32Addr + u32TempLen, au8Buff, 8u))
                {
                    bRet = TRUE;
                }
            }
        }
    }

    EnableAllInterrupts(); // 重新开启全局中断
    return bRet;
}

/*!
* @brief    Flash扇区擦除函数
* @param[in]    addrParam       擦除起始Flash地址
* @param[in]    sctCountParam   需要擦除的扇区数量
* @retval   boolean：TRUE擦除成功，FALSE擦除失败
*/
static boolean FLASH_HAL_RemoveSector(const uint32 addrParam, const uint32 sctCountParam)
{
    boolean apiReturnFlag = FALSE; // 擦除结果标记
    uint8 eraseRetVal = 0u; // 底层擦除接口返回值
    uint32 calcLen = 0u; // 总擦除字节长度 = 扇区数 * 单扇区大小

    calcLen = sctCountParam * MOD_SECTOR_SIZE;
    eraseRetVal = EraseFlashSector(addrParam, calcLen);
    if (0u != eraseRetVal)
    {
        apiReturnFlag = FALSE; // 底层返回非0，擦除失败
    }
    else
    {
        apiReturnFlag = TRUE; // 底层返回0，擦除成功
    }
    return apiReturnFlag;
}

/**************************************************************************
                    对外全局API函数区
**************************************************************************/
/*!
* @brief    清除 FMC 错误标志，避免上次 PG_ERR/对齐错误挡住后续擦写
* @note     STS 写1清：PEF(bit2) PAEF(bit3) WPEF(bit4) OCF(bit5) DBFIFLG(bit16)
*           与 g32a10xx_fls.c 中 Fmc_ClearStatusFlag 同一套寄存器
*/
void FLASH_HAL_ClearFmcErrorFlags(void)
{
    uint32_t guard = 0x00002000u;

    /* 等待 BUSY 结束；若已是 PE 则 Read 不会进 BUSY，直接清标志 */
    while (((FMC->STS_R.STS & 0x01u) != 0u) && (0u != guard))
    {
        guard--;
    }
    FMC->STS_R.STS = 0x0001003Cu;
}

/*!
* @brief    绑定Flash操作API到外部操作结构体，供升级模块调用
* @param[in]    pstFlashAPI  Flash操作接口结构体指针
* @retval   boolean：TRUE绑定成功，FALSE入参空指针失败
*/
boolean FLASH_HAL_BindFlashAPI(Flash_OperationAPIType * pstFlashAPI)
{
    boolean regSuccess = FALSE; // 注册成功标记
    if (NULL_PTR == pstFlashAPI)
    {
        regSuccess = FALSE; // 空指针直接返回失败
    }
    else
    {
        // 绑定初始化接口
        pstFlashAPI->pfFlashSetup      = FLASH_HAL_Init;
        // 绑定扇区擦除接口
        pstFlashAPI->pfSectorRemove    = FLASH_HAL_RemoveSector;
        // 绑定适配8字节对齐写入接口
        pstFlashAPI->pfDataWriter      = FLASH_HAL_ModifiedFlashWriteData;
        // 绑定Flash读取接口
        pstFlashAPI->pfDataReader      = FLASH_HAL_ReadData;
        // 绑定反初始化接口
        pstFlashAPI->pfFlashRelease    = FLASH_HAL_Deinit;
        regSuccess = TRUE;
    }
    return regSuccess;
}
