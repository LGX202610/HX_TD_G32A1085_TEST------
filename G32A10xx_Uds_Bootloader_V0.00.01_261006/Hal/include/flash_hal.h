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

#ifndef FLASH_HAL_H_
#define FLASH_HAL_H_

#include "flash_hal_Cfg.h"

/**************************************************************************
                    OTHER TYPE DEFINITION
**************************************************************************/
/** This typedef defines a pointer function for initializing the flash. */
typedef boolean (*tpfFlashSetup)(void);

/** This typedef defines a pointer function for releasing/de-initializing the flash. */
typedef void (*tpfFlashRelease)(void);

/** This typedef defines a pointer function for erasing sectors in flash. */
typedef boolean (*tpfSectorRemove)(const uint32 startAddr, const uint32 sectorCount);

/** This typedef defines a pointer function for programming/writing data to flash. */
typedef boolean (*tpfDataWriter)(const uint32 writeAddr, const uint8 * dataBuffer, const uint32 dataLength);

/** This typedef defines a pointer function for reading data from flash. */
typedef boolean (*tpfDataReader)(const uint32 readAddr, const uint32 readSize, uint8 * readBuffer);

/** This structure holds function pointers for flash operations. */
typedef struct
{
    tpfFlashSetup    pfFlashSetup;    //!< Pointer to function that initializes the flash
    tpfSectorRemove  pfSectorRemove;  //!< Pointer to function that erases flash sectors
    tpfDataWriter    pfDataWriter;    //!< Pointer to function that programs/writes data
    tpfDataReader    pfDataReader;    //!< Pointer to function that reads data from flash
    tpfFlashRelease  pfFlashRelease;  //!< Pointer to function that de-initializes the flash
} Flash_OperationAPIType;

/**************************************************************************
                    FUNCTION DECLARATION
**************************************************************************/
boolean FLASH_HAL_BindFlashAPI(Flash_OperationAPIType * pstFlashAPI);
void FLASH_HAL_ClearFmcErrorFlags(void);

#endif




