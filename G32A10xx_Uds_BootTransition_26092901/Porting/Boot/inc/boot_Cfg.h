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

#ifndef BOOT_CFG_H_
#define BOOT_CFG_H_

#include "includes.h"

/**************************************************************************
                      FUNCTION DECLARATION
**************************************************************************/
#ifdef UDS_PROJECT_FOR_APP
/** Bootloader Accepte Request */
void BootloaderAccepteReq(void);

/** Clear Flag For Download App Ok */
void ClearFlagForDownloadAppOk(void);

/** Verify whether Download App is Ok */
boolean VerifyDownloadAppOk(void);
#endif

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/** Clear Power On Flags when detecte */
void ClearPowerOnFlags(void);

/** Set Successful Flag For Download App */
void SetSuccFlagForDownloadApp(void);

/** Execute Jump To App Operation */
void ExecuteJumpToAppOperation(const uint32 addr);

/** Clear Bootloader Request Enter Flag */
void ClearBootloaderReqEnterFlag(void);

/** Verify whether Bootloader Request Enter or not */
boolean VerifyBootloaderReqEnter(void);

/** Process Multi-Core Application */
void ProcessMultiCoreApp(void);

/** Verify whether Power On event happend */
boolean VerifyDueToPowerOnReset(void);
boolean VerifyDueToWdtReset(void);
#endif

#endif


