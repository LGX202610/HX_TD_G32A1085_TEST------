/*!
 * @file        flash.c
 *
 * @brief       This file provides all the FLASH firmware functions
 *
 * @version     V1.0.1
 *
 * @date        2024-03-20
 *
 * @attention
 *
 *  Copyright (C) 2023-2024 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#include "flash.h"
#include "flash_hal_Cfg.h"
#include "devassert.h"
#include <string.h>

#define FLASH_DRV_BAK_SIZE  (FLS_DRV_END_ADDR - FLS_DRV_BEGIN_ADDR)

static uint8_t gs_flashDrvImageBak[FLASH_DRV_BAK_SIZE];
static uint8_t gs_flashDrvBakValid = 0u;

/*! @brief Configuration structure flashCfg_0 */
static const FLASH_USER_CONFIG_T Flash_InitConfig0 =
{
    .pflashBase  = FEATURE_FLASH_PF_START_ADDRESS,                        /* Base address of Program Flash block */
    .pflashSize  = FEATURE_FLASH_PF_BLOCK_SIZE,          /* Size of Program Flash block         */
    .dflashBase  = FEATURE_FLASH_DF_START_ADDRESS,       /* Base address of Data Flash block    */
    .eeramBase   = 0U, /* Base address of CfgRAM block       */
    /* If using callback, any code reachable from this function must not be placed in a Flash block targeted for a program/erase operation. */
    .callBack    = NULL_CALLBACK
};

/* Declare a FLASH configuration struct which initialized by InitFlash, and will be used by all flash operations */
static FLASH_SSD_CONFIG_T flashSSDConfig;

#ifdef USE_FLASH_DRIVER
//#pragma CONST_SEG FLASH_HEADER
const tFlashOptInfo g_stFlashOptInfo =
{
    FALSH_DRIVER_START,
    FALSH_DRIVER_END,
    NULL_PTR,//&InitFlash,
    &EraseFlashSector,
    &WriteFlash,
    NULL_PTR//&ReadFlashMemory
};
//#pragma CODE_SEG DEFAULT

const tFlashOptInfo *g_pstFlashOptInfo = &g_stFlashOptInfo;
#else
//#pragma CODE_SEG DEFAULT
/* Declare a Flash configuration struct which initialized by InitFlashAPI, and will be used all flash operation */
static tFlashOptInfo *g_pstFlashOptInfo = (void *)0;
#endif /* USE_FLASH_DRIVER */

#ifndef FLASH_SDK_USING
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_Init
 * Description   : Initializes Flash module by clearing status error bit
 * and reporting the memory configuration via SSD configuration structure.
 *
 * Implements    : FLASH_DRV_Init_Activity
 *END**************************************************************************/
static STATUS_T FLASH_DRV_Init(const FLASH_USER_CONFIG_T *const pUserConf,
                               FLASH_SSD_CONFIG_T *const pSSDConfig)
{
    DEV_ASSERT(pUserConf != NULL);
    DEV_ASSERT(pSSDConfig != NULL);
    STATUS_T ret = STATUS_SUCC;

    pSSDConfig->pflashBase = pUserConf->pflashBase;
    pSSDConfig->pflashSize = pUserConf->pflashSize;
    pSSDConfig->dflashBase = pUserConf->dflashBase;
    pSSDConfig->eeramBase = pUserConf->eeramBase;
    pSSDConfig->callBack = pUserConf->callBack;

    /* If size of D/E-Flash = 0 */
    pSSDConfig->dflashSize = 0U;
    pSSDConfig->eeeSize = 0U;

    return ret;
}
#endif /* FLASH_SDK_USING */

void InitFlash(void)
{
    FLASH_DRV_Init(&Flash_InitConfig0, &flashSSDConfig);
}

#ifdef APP_SINGLE_UPDATA
void SetFlashDriverPosition(uint32_t i_flashDriverAddr)
{
    g_pstFlashOptInfo = (tFlashOptInfo *)i_flashDriverAddr;
}
#endif

/* Init Flash API g_pstFlashOptInfo pointer */
void InitFlashAPI(void)
{
    uint32 *tmp = NULL;
    uint32 flashDriverStartAdd = 0;
    uint32 flashDriverEndAdd = 0;
    uint32 drvSize = 0;

    FLASH_HAL_ReadDriverRange(&flashDriverStartAdd, &flashDriverEndAdd);
    tmp = (uint32 *)flashDriverStartAdd;

    drvSize = flashDriverEndAdd - flashDriverStartAdd;
    if (drvSize > FLASH_DRV_BAK_SIZE)
    {
        drvSize = FLASH_DRV_BAK_SIZE;
    }
    (void)memcpy(gs_flashDrvImageBak, (const void *)flashDriverStartAdd, drvSize);
    gs_flashDrvBakValid = 1u;

    for (uint32 i = 0; i < sizeof(tFlashOptInfo) / 4; i++)
    {
        tmp[i] += (uint32) flashDriverStartAdd;
    }

    g_pstFlashOptInfo = (tFlashOptInfo *)flashDriverStartAdd;
}

const uint8_t *Flash_GetUnrelocatedDriver(void)
{
    return (0u != gs_flashDrvBakValid) ? gs_flashDrvImageBak : NULL;
}

uint32_t Flash_GetUnrelocatedDriverMaxSize(void)
{
    return FLASH_DRV_BAK_SIZE;
}

unsigned char EraseFlashSector(const unsigned long i_ulLogicalAddr,
                               const unsigned long i_ulEraseLen)
{
    STATUS_T ret; /* Store the driver APIs return code */
    unsigned long i_ulStartVerifyAddr = i_ulLogicalAddr;

    ret = g_pstFlashOptInfo->FLASH_EraseSector(i_ulLogicalAddr, FLASH_ERASE_KEY);

    if(STATUS_SUCC == ret)
    {
        ret = g_pstFlashOptInfo->FLASH_VerifySection(i_ulStartVerifyAddr);
    }
    else
    {
        /* do nothing */
    }

    return ret;
}

unsigned char WriteFlash(const uint32_t i_xStartAddr,
                         const void *i_pvDataBuf,
                         const unsigned short i_usDataLen)
{
    STATUS_T ret; /* Store the driver APIs return code */

    ret = g_pstFlashOptInfo->FLASH_Program(i_xStartAddr, i_usDataLen, i_pvDataBuf);

    if(STATUS_SUCC == ret)
    {
        ret = g_pstFlashOptInfo->FLASH_ProgramCheck(i_xStartAddr);
    }
    else
    {
        /* do nothing */
    }

    return ret;
}

/* read a byte from flash. Read data address must be global address. */
unsigned char ReadFlashByte(const unsigned long i_ulGloabalAddress)
{
    unsigned char  ucReadvalue;
    /* From global address get values */
    ucReadvalue = (*((unsigned long *)i_ulGloabalAddress));
    return ucReadvalue;
}

/********************************************************
**  read data from current page flash.
**  Parameter :
**      @   i_ulLogicalAddr : Local address
**      @   i_ulLength : Data length need to be read
**      @   o_pucDataBuf : Data buffer used to store the read buffer
*********************************************************/
void ReadFlashMemory(const unsigned long i_ulLogicalAddr,
                     const unsigned long i_ulLength,
                     unsigned char *o_pucDataBuf)
{
    unsigned long ulGlobalAddr;
    unsigned long ulIndex = 0u;
    ulGlobalAddr = i_ulLogicalAddr;

    for (ulIndex = 0u; ulIndex < i_ulLength; ulIndex++)
    {
        o_pucDataBuf[ulIndex] = ReadFlashByte(ulGlobalAddr);
        ulGlobalAddr++;
    }
}
