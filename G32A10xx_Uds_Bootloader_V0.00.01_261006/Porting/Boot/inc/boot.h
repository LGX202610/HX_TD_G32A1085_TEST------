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

#ifndef BOOT_H_
#define BOOT_H_

#include "boot_Cfg.h"

/**************************************************************************
                    FUNCTION DECLARATION
**************************************************************************/
#if defined (__cplusplus)
extern "C" {
#endif

#ifdef UDS_PROJECT_FOR_APP
/** Verify App Status After Downlaod */
boolean VerifyAppStatusAfterDownlaod(void);
#endif

#ifdef UDS_PROJECT_FOR_BOOTLOADER
/** Request bootloader mode check */
boolean VerifyBootloaderModeOnRequest(void);

/** Judge whether jump to app */
void JudgeJumpToApp(void);
#endif

#if defined (__cplusplus)
}
#endif

#endif

