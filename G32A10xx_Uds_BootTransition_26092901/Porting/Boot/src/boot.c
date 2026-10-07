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

#include "boot.h"
#include "includes.h"
#include "fls_app.h"
#include "uds_app.h"

#ifdef APP_SINGLE_UPDATA
    #include "flash.h"
    #include "CRC_hal.h"
#endif

/*******************************************************************************
                        LOCAL FUNCTION DECLARATIONS
*******************************************************************************/
#ifdef UDS_PROJECT_FOR_BOOTLOADER

#ifdef APP_SINGLE_UPDATA

static boolean Boot_IsBackupAPPValid(void);

static boolean Boot_EraseBadApp(void);

static void Boot_InitFlashRamAPI(void);

static boolean Boot_WriteFlash(uint32 startAddr, uint8 *needToWriteBuffer);

boolean Boot_WriteBackupToApp(void);

boolean Flash_CheckAppCrcAfterCopy(void);

static boolean Boot_CopyBackupApp(void);

#endif

#endif

/*******************************************************************************
                               LOCAL FUNCTIONS
*******************************************************************************/
#ifdef UDS_PROJECT_FOR_BOOTLOADER

#ifdef APP_SINGLE_UPDATA
/**
 * @brief  The Flash driver, put it in .ramcode section to avoid the g_RamDriverArray is overwrite by the app
 *         When the Bootloader starts,the g_RamDriverArray will being relocated to the RAM.
 */
const uint8 g_flashDriverRAM[] __attribute__((section(".ramcode"))) =
{
    0x11, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x00, 0xDB, 0x00, 0x00, 0x00,
    0x10, 0xB5, 0x04, 0x46, 0x07, 0x48, 0x81, 0x42, 0x08, 0xD1, 0x00, 0xF0, 0x61, 0xF8, 0x20, 0x46,
    0x00, 0xF0, 0x96, 0xF8, 0x04, 0x46, 0x00, 0xF0, 0x67, 0xF8, 0x00, 0xE0, 0x02, 0x24, 0x20, 0x46,
    0x10, 0xBD, 0xC0, 0x46, 0x5A, 0xA5, 0x5A, 0xA5, 0x00, 0x20, 0x70, 0x47, 0xF0, 0xB5, 0x95, 0xB0,
    0x00, 0x2A, 0x47, 0xD0, 0x14, 0x46, 0x05, 0x46, 0x00, 0x20, 0x05, 0xAA, 0xFF, 0x27, 0x17, 0x54,
    0x40, 0x1C, 0x40, 0x28, 0xF9, 0xD1, 0x3F, 0x20, 0x08, 0x40, 0x8E, 0x09, 0x04, 0x90, 0x00, 0x28,
    0x00, 0xD0, 0x76, 0x1C, 0x00, 0xF0, 0x3C, 0xF8, 0x00, 0x2E, 0x2D, 0xD0, 0x01, 0x96, 0x70, 0x1E,
    0x02, 0x90, 0x00, 0x26, 0x40, 0x21, 0x02, 0x98, 0x86, 0x42, 0x04, 0x98, 0x00, 0xD0, 0x08, 0x46,
    0x03, 0x95, 0x04, 0x9A, 0x00, 0x2A, 0x00, 0xD1, 0x08, 0x46, 0x00, 0x28, 0x08, 0xD0, 0x00, 0x21,
    0x0A, 0x46, 0x63, 0x5C, 0x05, 0xAD, 0x6B, 0x54, 0x52, 0x1C, 0xD1, 0xB2, 0x88, 0x42, 0xF8, 0xD8,
    0x03, 0x21, 0x05, 0xAA, 0x03, 0x9D, 0x28, 0x46, 0x00, 0xF0, 0x78, 0xF8, 0x00, 0x22, 0x05, 0xA9,
    0x8F, 0x54, 0x52, 0x1C, 0x40, 0x2A, 0xFA, 0xD1, 0x00, 0x28, 0x06, 0xD1, 0x76, 0x1C, 0x40, 0x34,
    0x40, 0x35, 0x01, 0x98, 0x86, 0x42, 0xD5, 0xD1, 0x00, 0x20, 0x04, 0x46, 0x00, 0xF0, 0x14, 0xF8,
    0x20, 0x46, 0x00, 0xE0, 0x02, 0x20, 0x15, 0xB0, 0xF0, 0xBD, 0x00, 0x20, 0x70, 0x47, 0x00, 0x00,
    0x02, 0x48, 0x03, 0x49, 0x01, 0x60, 0x03, 0x49, 0x01, 0x60, 0x70, 0x47, 0x04, 0x20, 0x02, 0x40,
    0x23, 0x01, 0x67, 0x45, 0xAB, 0x89, 0xEF, 0xCD, 0x02, 0x48, 0x01, 0x68, 0x80, 0x22, 0x0A, 0x43,
    0x02, 0x60, 0x70, 0x47, 0x10, 0x20, 0x02, 0x40, 0x08, 0x48, 0x00, 0x68, 0x41, 0x07, 0x07, 0xD4,
    0xC1, 0x06, 0x07, 0xD4, 0xC1, 0x07, 0x07, 0xD1, 0x81, 0x0B, 0x04, 0x20, 0x08, 0x40, 0x70, 0x47,
    0x02, 0x20, 0x70, 0x47, 0x03, 0x20, 0x70, 0x47, 0x01, 0x20, 0x70, 0x47, 0x0C, 0x20, 0x02, 0x40,
    0xB0, 0xB5, 0x01, 0x21, 0x0C, 0x1A, 0x65, 0x1C, 0xFF, 0xF7, 0xE6, 0xFF, 0x01, 0x28, 0x02, 0xD1,
    0x00, 0x2C, 0x2C, 0x46, 0xF7, 0xD1, 0x01, 0x2D, 0x00, 0xD1, 0x05, 0x20, 0xB0, 0xBD, 0xC0, 0x46,
    0xF0, 0xB5, 0x81, 0xB0, 0x04, 0x46, 0x01, 0x20, 0x40, 0x02, 0xA1, 0x0C, 0x81, 0x42, 0x17, 0xD1,
    0x0B, 0x20, 0x05, 0x04, 0x28, 0x46, 0xFF, 0xF7, 0xE3, 0xFF, 0x00, 0x28, 0x11, 0xD1, 0x0A, 0x4E,
    0x30, 0x68, 0x02, 0x27, 0x38, 0x43, 0x30, 0x60, 0x74, 0x60, 0x30, 0x68, 0x40, 0x21, 0x01, 0x43,
    0x31, 0x60, 0x28, 0x46, 0xFF, 0xF7, 0xD4, 0xFF, 0x31, 0x68, 0xB9, 0x43, 0x31, 0x60, 0x00, 0xE0,
    0x02, 0x20, 0x01, 0xB0, 0xF0, 0xBD, 0xC0, 0x46, 0x10, 0x20, 0x02, 0x40, 0xF0, 0xB5, 0x83, 0xB0,
    0x15, 0x46, 0x0E, 0x46, 0x02, 0x90, 0x01, 0x27, 0x7C, 0x03, 0x20, 0x46, 0xFF, 0xF7, 0xC0, 0xFF,
    0x00, 0x28, 0x27, 0xD1, 0x01, 0x94, 0x14, 0x4C, 0x20, 0x68, 0x38, 0x43, 0x20, 0x60, 0x20, 0x68,
    0x12, 0x49, 0x08, 0x40, 0xB1, 0x07, 0x89, 0x0A, 0x40, 0x18, 0x20, 0x60, 0x60, 0x68, 0x02, 0x98,
    0x60, 0x60, 0xB0, 0x00, 0x0E, 0xA1, 0x08, 0x58, 0x21, 0x46, 0x30, 0x31, 0x00, 0x22, 0x08, 0xCD,
    0x08, 0xC1, 0x52, 0x1C, 0x82, 0x42, 0xFA, 0xD3, 0x20, 0x68, 0x40, 0x21, 0x01, 0x43, 0x21, 0x60,
    0x01, 0x98, 0xFF, 0xF7, 0x9D, 0xFF, 0x21, 0x68, 0x04, 0x4A, 0x11, 0x40, 0x21, 0x60, 0x21, 0x68,
    0xB9, 0x43, 0x21, 0x60, 0x03, 0xB0, 0xF0, 0xBD, 0x10, 0x20, 0x02, 0x40, 0xFF, 0xFF, 0xCF, 0xFF,
    0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};

/** define flash operation */
static Flash_OperationAPIType gs_stFlashOperateAPI;

/*!
* @brief         Check whether backup App is valid or not.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
static boolean Boot_IsBackupAPPValid(void)
{
    boolean bResult = FALSE;
    tCrc backupAppInfoCrc = 0u;
    tCrc backupAppCrc = 0u;

    /** calculate the hardware CRC value to check whether it is valid */
    CRC_HAL_CreatHw((const uint8 *)(APP_A_BACKUP_BEGIN_ADDR), sizeof(appInfoType) - 4, &backupAppInfoCrc);

    /** check the result */
    if(((appInfoType*)(APP_A_BACKUP_BEGIN_ADDR))->crc == backupAppInfoCrc)
    {
        /** Get the backup App CRC except the APP info(512Bytes) */
        CRC_HAL_CreatHw((const uint8 *)(APP_A_BACKUP_BEGIN_ADDR + APP_INFORMATION_SIZE),
                        ((appInfoType*)(APP_A_BACKUP_BEGIN_ADDR))->appLen, &backupAppCrc);
        /** check whether the Backup APP is valid or not */
        if(((appInfoType*)(APP_A_BACKUP_BEGIN_ADDR))->appCrc  == backupAppCrc)
        {
            bResult = TRUE;
        }
    }

    return bResult;
}

/*!
* @brief         Erase invalid app on flash.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
static boolean Boot_EraseBadApp(void)
{
    uint8 s_result = FALSE;
    uint32 eraseFlashLen = 0u;
    Flash_blockInformationType *s_pAppFlashMemoryInfo = NULL_PTR;
    uint32 s_appFlashItem = 0u;
    uint32 sectorNo = 0u;
    uint32 eraseFlashStartAddr = 0u;
    uint32 eraseSectorNoTmp = 0u;

    /** Get invalid APP flash config information */
   if (TRUE == FLASH_HAL_InspectFlashConfiguration(APP_A_ID, &s_pAppFlashMemoryInfo, &s_appFlashItem))
    {
        /** One time erase all flash sectors */
        while (s_appFlashItem)
        {
            eraseFlashLen = s_pAppFlashMemoryInfo->endLogAddr - s_pAppFlashMemoryInfo->startLogAddr;

            sectorNo = FLASH_HAL_CalcSectorCount(s_pAppFlashMemoryInfo->startLogAddr, eraseFlashLen);

            /** Disable all interrupts */
            DisableAllInterrupts();
            eraseSectorNoTmp = sectorNo;
            eraseFlashStartAddr = s_pAppFlashMemoryInfo->startLogAddr;

            /** Erase a sector once because for watch dog */
            while (eraseSectorNoTmp)
            {
                /** Do erase flash */
                if( gs_stFlashOperateAPI.pfSectorRemove != NULL_PTR)
                {
                    s_result = gs_stFlashOperateAPI.pfSectorRemove(eraseFlashStartAddr, 1u);
                }

                eraseSectorNoTmp--;

                if (TRUE != s_result)
                {
                    break;
                }

                eraseFlashStartAddr += MOD_SECTOR_SIZE;
            }

            /** Enable all all interrupts */
            EnableAllInterrupts();

            if (TRUE != s_result)
            {
                break;
            }

            s_appFlashItem--;
            s_pAppFlashMemoryInfo++;
        }
    }

    return s_result;
}

/*!
* @brief         Initialize ram flash.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
static void Boot_InitFlashRamAPI(void)
{
    /** Avoid to be optimized by the MDK */
    volatile uint32_t *tmp = NULL;
    uint32_t flashDriverStartAdd = 0;

    flashDriverStartAdd = (uint32)g_flashDriverRAM;
    tmp = (uint32 *)flashDriverStartAdd;

    for (uint32_t i = 0; i < sizeof(tFlashOptInfo) / 4; i++)
    {
        tmp[i] += (uint32_t) flashDriverStartAdd;
    }

    SetFlashDriverPosition((uint32_t)flashDriverStartAdd);
}

/*!
* @brief         flash write.
*
* @param[in]     beginAddr              -the address begin to write
                 targetBufferToWrite    -the source data buffer
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
static boolean Boot_WriteFlash(uint32 startAddr, uint8 *needToWriteBuffer)
{
    uint8 result = TRUE;

    /** Write data in flash */
    if (NULL_PTR != gs_stFlashOperateAPI.pfDataWriter)
    {
        DisableAllInterrupts();
        result = gs_stFlashOperateAPI.pfDataWriter(startAddr, &needToWriteBuffer[0], FLASH_OPERATE_SIZE_BYTE);
        EnableAllInterrupts();
    }
    else
    {
        result = FALSE;
    }

    return result;
}

/*!
* @brief         backup app copy operation.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
static boolean Boot_CopyBackupApp(void)
{
    uint8 s_result = FALSE;

    /** Init the Flash */
    InitFlash();
    Boot_InitFlashRamAPI();
    FLASH_HAL_BindFlashAPI(&gs_stFlashOperateAPI);

    /** Erase the App region */
    s_result = Boot_EraseBadApp();

    if(TRUE == s_result)
    {
        s_result = Boot_WriteBackupToApp();
    }

    if(TRUE == s_result)
    {
        s_result = Flash_CheckAppCrcAfterCopy();
    }

    return s_result;

}

/*!
* @brief         Write flash App application from Backup App by bootloader when the App is invalid.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
boolean Boot_WriteBackupToApp(void)
{
    uint8 result = TRUE;
    uint8 readCnt = 0;
    uint32 totalCopyCnt = 0;
    uint32 curCopyCnt = 0;
    uint8 *srcAddr = NULL;
    uint32 desAddr = 0;
    uint8 tempBuff[150] = {0};

    /** Source address */
    srcAddr = (uint8*)(APP_A_BACKUP_BEGIN_ADDR);

    /** Copy size:Backup APP size + Backup APP info size */
    totalCopyCnt = ((appInfoType*)(APP_A_BACKUP_BEGIN_ADDR))->appLen + APP_INFORMATION_SIZE;
    /** Destinied start adress */
    desAddr = APP_A_BEGIN_ADDR;

    while(curCopyCnt < totalCopyCnt)
    {
        while(readCnt < FLASH_OPERATE_SIZE_BYTE)
        {
            tempBuff[readCnt++] = *srcAddr++;
        }

        readCnt = 0;

        if(!Boot_WriteFlash(desAddr, tempBuff))
        {
            result = FALSE;
            break;
        }

        curCopyCnt += FLASH_OPERATE_SIZE_BYTE;
        desAddr += FLASH_OPERATE_SIZE_BYTE;
    }

    return result;
}

/*!
* @brief         Check the App CRC after copy successfully.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
boolean Flash_CheckAppCrcAfterCopy(void)
{
    tCrc appInfoCrc = 0u;
    tCrc appCrc = 0u;
    uint8 result = FALSE;

    /** Confirm the APP info valid */
    CRC_HAL_CreatHw((const uint8 *)(APP_A_BEGIN_ADDR), sizeof(appInfoType) - 4, &appInfoCrc);

    if(((appInfoType*)(APP_A_BEGIN_ADDR))->crc == appInfoCrc)
    {
        /** Calculate the App CRC except the APP info(512Bytes) */
        CRC_HAL_CreatHw((const uint8 *)(APP_A_BEGIN_ADDR + APP_INFORMATION_SIZE),
                        ((appInfoType*)(APP_A_BEGIN_ADDR))->appLen, &appCrc);

        /** Calcultated CRC of App should equal APP CRC and Backup APP CRC in flash */
        if((((appInfoType*)(APP_A_BEGIN_ADDR))->appCrc == appCrc) &&
            ((appInfoType*)(APP_A_BACKUP_BEGIN_ADDR))->appCrc == appCrc)
        {
            result = TRUE;
        }
    }

    return result;
}

#endif

/*!
* @brief         Request bootloader mode check.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
boolean VerifyBootloaderModeOnRequest(void)
{
    boolean retResult = FALSE;
    boolean checkResult = FALSE;

    /** check Bootloader Request Enter */
    checkResult = VerifyBootloaderReqEnter();

    if (checkResult == TRUE)
    {
        ClearBootloaderReqEnterFlag();

        retResult = (UDS_APP_SendMsgToHost() == TRUE) ? TRUE : FALSE;

        if (retResult == TRUE)
        {
            APPDebugLog("\nEnter bootloader mode successfully!\r\n");
        }
        else
        {
            APPDebugLog("\nEnter bootloader mode and transmit confirm message failed!\r\n");
        }
    }

    return retResult;
}

/*!
* @brief         Judge whether jump to app.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void JudgeJumpToApp(void)
{
    /* 过渡程序链在 APP 区，禁止再跳转 */
    return;
}

#endif

#ifdef UDS_PROJECT_FOR_APP
/*!
* @brief         Verify App Status After Downlaod.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        boolean: true, false
*/
boolean VerifyAppStatusAfterDownlaod(void)
{
    boolean retResult = FALSE;
    boolean checkResult = FALSE;

    checkResult = VerifyDownloadAppOk();

    if (checkResult == TRUE)
    {
        ClearFlagForDownloadAppOk();

        retResult = (UDS_APP_SendMsgToHost() == TRUE) ? TRUE : FALSE;

        if (retResult == TRUE)
        {
            APPDebugLog("\nDownlaod APP successfully!\n");
        }
        else
        {
            APPDebugLog("\nEnter APP mode and transmit confirm message failed!\n");
        }
    }

    return retResult;
}

#endif

