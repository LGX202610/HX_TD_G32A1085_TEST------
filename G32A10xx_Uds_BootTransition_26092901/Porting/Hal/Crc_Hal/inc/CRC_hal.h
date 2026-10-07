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

#ifndef CRC_HAL_H
#define CRC_HAL_H

#include "includes.h"

/**************************************************************************
                    MACRO DEFINITION
**************************************************************************/
#define CRC_SEED_INIT_VALUE 0xFFFF

/**************************************************************************
                    OTHER TYPE DEFINITION
**************************************************************************/
typedef uint32 tCrc;

/**************************************************************************
                    FUNCTION DECLARATION
**************************************************************************/
boolean CRC_HAL_Init(void);
void CRC_HAL_CreatHw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc);
void CRC_HAL_CreatSw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc);

uint32_t crc32_calc(const uint8_t* data, uint32_t len);

void crc32_test(void); //É¾³ý


#endif



