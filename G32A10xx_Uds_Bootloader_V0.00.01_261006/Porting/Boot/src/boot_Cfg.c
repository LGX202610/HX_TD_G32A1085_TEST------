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

#include "boot_Cfg.h"
#include "flash_hal_cfg.h"
#include "fls_app.h"
#include "CRC_hal.h"

/**************************************************************************
                    OTHER TYPE DEFINITION
**************************************************************************/
/**
 * @brief   定义Boot启动信息结构体类型
 * @note    该结构体用于APP与Bootloader之间交互升级状态信息，
 *          结构体整体长度必须满足4字节对齐要求，避免非对齐内存访问故障。
 */
typedef struct
{
    uint8 bootInfoDataLength;          //!< 启动信息数据长度，注意：长度必须4字节对齐
    uint8 bootloaderReqEnterFlag;      //!< 请求进入Bootloader的标志位
    uint8 appDownloadSuccFlag;         //!< APP固件下载成功标志位
    uint32 infoBeginAddress;           //!< 启动信息数据存储起始地址
    uint32 bootloaderReqEnterAddress;  //!< 请求进入Bootloader标志对应的存储地址
    uint32 appDownloadSuccAddress;     //!< APP下载成功标志对应的存储地址
} bootInfoType;
/**************************************************************************
                    GLOBAL VARIABLE
**************************************************************************/
/**
 * @brief   Initilize boot Infomation content.
 */
static const bootInfoType gs_bootInformation =
{
    16u,
    0x5Au,
    0xA5u,
    INFO_BEGIN_ADDR,
    REQ_ENTER_BL_ADDR,
    APP_DL_SUC_ADDR,
};

/*******************************************************************************
                        LOCAL FUNCTION DECLARATIONS
*******************************************************************************/
static uint16 GainInfomationCrcValue(void);

static void SetInfomationCrcValue(void);

static boolean VerifyInfomationCorrect(void);

/**************************************************************************
                         MACRO DEFINITION
**************************************************************************/
/** Get information CRC value*/
#define GetInformationCRCValue() (*(uint16 *)(gs_bootInformation.infoBeginAddress + 0x0E))

/** Set information CRC value*/
#define SetInformationCRCValue(num) ((*(uint16 *)(gs_bootInformation.infoBeginAddress + 0x0E)) = (uint16)(num))

/*******************************************************************************
                               LOCAL FUNCTIONS
*******************************************************************************/
/*!
* @brief         Gain Infomation Crc Value.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] crcVal  -CRC value
*
* @retval        CRC value
*/
static uint16 GainInfomationCrcValue(void)
{
    uint32 crcVal = 0u;

    /** calculate CRC value*/
    CRC_HAL_CreatSw((const uint8 *)gs_bootInformation.infoBeginAddress, gs_bootInformation.bootInfoDataLength - 2u, &crcVal);

    return (uint16)crcVal;
}

/*!
* @brief         Set Crc Value.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
static void SetInfomationCrcValue(void)
{
    uint16 crcVal = 0u;

    crcVal = GainInfomationCrcValue();

    SetInformationCRCValue(crcVal);
}

/*!
* @brief         Verify whether Infomation Correct or not.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: TRUE, FALSE
*/
static boolean VerifyInfomationCorrect(void)
{
    uint16 crcVal = GainInfomationCrcValue();

    uint16 crcTemp = GetInformationCRCValue();

    boolean ret = (crcTemp == crcVal) ? TRUE : FALSE;

    return ret;
}

/*******************************************************************************
                               GLOBAL FUNCTIONS
*******************************************************************************/
#ifdef UDS_PROJECT_FOR_APP
/*!
* @brief         Bootloader Accepte Request.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void BootloaderAccepteReq(void)
{
    *((uint8 *)gs_bootInformation.bootloaderReqEnterAddress) = gs_bootInformation.bootloaderReqEnterFlag;

    SetInfomationCrcValue();
}

/*!
* @brief         Clear Flag For Download App Ok.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void ClearFlagForDownloadAppOk(void)
{
    *((uint8 *)gs_bootInformation.appDownloadSuccAddress) = 0u;

    SetInfomationCrcValue();
}

/*!
* @brief         Verify whether Download App is Ok.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: TRUE, FALSE
*/
boolean VerifyDownloadAppOk(void)
{
    boolean ret = FALSE;

    if(  (VerifyInfomationCorrect() == TRUE) &&
         (*((uint8 *)gs_bootInformation.appDownloadSuccAddress) == gs_bootInformation.appDownloadSuccFlag)  )
    {
        ret = TRUE;
    }

    return ret;
}
#endif

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/*!
* @brief         Clear Power On Flags when detecte.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void ClearPowerOnFlags(void)
{
    uint8 count = 0u;

    /** Clear RAM with 4 bytes for ECC */
    while(count < (gs_bootInformation.bootInfoDataLength >> 2u))
    {
        *((uint32 *)gs_bootInformation.infoBeginAddress + count) = 0u;

        count++;
    }

    SetInfomationCrcValue();
}

/*!
* @brief         Set Successful Flag For Download App.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void SetSuccFlagForDownloadApp(void)
{
    *((uint8 *)gs_bootInformation.appDownloadSuccAddress) = gs_bootInformation.appDownloadSuccFlag;

    SetInfomationCrcValue();
}

/*!
* @brief         Execute Jump To App Operation.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
typedef void (*AppAddrType)(void);

AppAddrType GoToAppAddress = NULL;

void ExecuteJumpToAppOperation(const uint32 addr)
{
     GoToAppAddress = (AppAddrType)(addr);
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*)(APP_A_BEGIN_ADDR + 0x200));
    (GoToAppAddress)();
}

/*!
* @brief         Clear Bootloader Request Enter Flag.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void ClearBootloaderReqEnterFlag(void)
{
    *((uint8 *)gs_bootInformation.bootloaderReqEnterAddress) = 0u;

    SetInfomationCrcValue();
}

/*!
* @brief         Verify whether Bootloader Request Enter or not.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: TRUE, FALSE
*/
boolean VerifyBootloaderReqEnter(void)
{
    boolean ret = FALSE;

    if(  (VerifyInfomationCorrect() == TRUE) &&
         (*((uint8 *)gs_bootInformation.bootloaderReqEnterAddress) == gs_bootInformation.bootloaderReqEnterFlag)  )
    {
         ret = TRUE;
    }

    return ret;
}

/*!
* @brief         Process Multi-Core Application.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void ProcessMultiCoreApp(void)
{
    uint32 count = 0u;
    uint32 appVirtualAddress = 0u;
    uint32 appAllocateAddress = 0u;
    AppIdType appContent = GainNewestAPPId();

    if (MCU_CORE_NUMBER > 0u)
    {
        while(count < MCU_CORE_NUMBER)
        {
            if(FLASH_HAL_AcquireMultiRemapInfoAddress(appContent, count, &appVirtualAddress) == TRUE)
            {
                if(FLASH_HAL_UpdateMultiCoreRemapAddr(appContent, count, &appAllocateAddress) == TRUE)
                {
                    /** do nothing */
                }
            }

            count++;
        }
    }
}

/*!
* @brief         Verify whether Power On event happend.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: TRUE, FALSE
*/
boolean VerifyDueToPowerOnReset(void)
{
    boolean ret = FALSE;
    uint8_t resetRegVal = 0;

    /** It must be executed, otherwise the APP cannot be flashed again after being flashed once. */
    resetRegVal = Rcm_ReadStatusFlag(RCM_FLAG_PWRRST);

    if(SET == resetRegVal)
    {
        ret = TRUE;
    }

    return ret;
}

boolean VerifyDueToWdtReset(void)
{
    boolean ret = FALSE;
    uint8_t resetRegVal = 0;

    /** It must be executed, otherwise the APP cannot be flashed again after being flashed once. */
    resetRegVal = Rcm_ReadStatusFlag(RCM_FLAG_IWDTRST);

    if(SET == resetRegVal)
    {
        ret = TRUE;
    }

    return ret;
}

#endif


