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

#ifndef FLASH_HAL_CFG_H
#define FLASH_HAL_CFG_H

#include "includes.h"

/**************************************************************************
                    OTHER TYPE DEFINITION
**************************************************************************/
/* Define a 32-bit logical address */
typedef uint32 tModLogicalAddr;

/**
* @brief    This structure contains core address information for images and remapping
*/
typedef struct
{
    uint32 primAStartAddr;    //!< Start address of image A
    uint32 primBStartAddr;    //!< Start address of image B
    uint32 mirrorAStartAddr;  //!< Mirror address corresponding to image A
    uint32 mirrorBStartAddr;  //!< Mirror address corresponding to image B
    uint32 remapAppAddr;      //!< Address used for remapping the application
} Flash_coreInformationType;

/**
* @brief    This structure describes the start and end logical addresses of a block
*/
typedef struct
{
    tModLogicalAddr startLogAddr; //!< Block start address in logical space
    tModLogicalAddr endLogAddr; //!< Block end address in logical space
} Flash_blockInformationType;

/**************************************************************************
                    MACRO DEFINITION
**************************************************************************/
/* Define max length of the data buffer for flash operations */
#define MAX_MOD_DATA_LEN (200u)

/* Define a sector size in bytes */
#define MOD_SECTOR_SIZE (512)

/* Configure whether to enable writing reset handler into flash */
#define EN_MOD_RESET_HANDLER (FALSE)

/* Define max time (in ms) required to erase flash sector under specific MCU type */
#if (defined MCU_VENDOR_ID) && (MCU_VENDOR_ID == MCU_G32A10xx)
#define MOD_MAX_ERASE_SECTOR_MS (13u)
#endif

/* Define vector table and reset handler offsets for a specific MCU type */
#if (defined MCU_VENDOR_ID) && (MCU_VENDOR_ID == MCU_G32A10xx)
/* Vector table offset from primary block addresses */
#define MOD_VECTOR_TABLE_OFFSET   (0x200u)
/* Offset from the vector table top to the reset handler */
#define MOD_RESET_HANDLER_OFFSET  (4u)
/* Length (in bytes) of the reset handlers address */
#define MOD_RESET_HANDLER_ADDR_LEN (4u)
#endif

/**************************************************************************
                    FUNCTION DECLARATION
**************************************************************************/
boolean FLASH_HAL_BOASecNumToAddr(const AppIdType rType, 
                                  const uint32 rSectorNo, 
                                  uint32 *pOutAddr);

uint32 FLASH_HAL_CalcFlashSec(const AppIdType paramApp);

boolean FLASH_HAL_AcquireMultiRemapInfoAddress(const AppIdType iLocalAppType, 
                                               const uint32 iLocalCoreNo, 
                                               uint32 *pOutMirrorAddr);

boolean FLASH_HAL_UpdateMultiCoreRemapAddr(const AppIdType p_appCategory, 
                                           const uint32 p_coreIndex, 
                                           uint32 *p_remapResult);

void FLASH_HAL_GetResetHandlerDetails(boolean *pIsEnableResetHandler, 
                                      uint32 *pRstHandlerOffset, 
                                      uint32 *pRstHandlerLength);

boolean FLASH_HAL_ReadDriverRange(uint32 *pOutDrvStart, uint32 *pOutDrvEnd);

uint32 FLASH_HAL_CalcSectorCount(const uint32 p_initAddr, 
                                 const uint32 p_dataLen);

boolean FLASH_HAL_GetDetailOfAPP(const AppIdType paramAppType, 
                                 uint32 *outStartAddress, 
                                 uint32 *outBlockLen);

boolean FLASH_HAL_InspectFlashConfiguration(const AppIdType iLocalAppType,
                                            Flash_blockInformationType **pOutBlockInfo,
                                            uint32 *pOutItemCount);

boolean FLASH_HAL_VerifyAppFlashConfiguration(void);

#endif

