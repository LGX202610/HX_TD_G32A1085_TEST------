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

#include "watchdog_hal.h"

/**************************************************************************
                    GLOBAL FUNCTION
**************************************************************************/
void WATCHDOG_HAL_SystemReset(void)
{
    /* set IWDT Write Access */
    Iwdt_EnableWriteAccess();
    
    while(SET == Iwdt_ReadStatusFlag(IWDT_FLAG_CNTU));
    Iwdt_ConfigReload(0U);
    Iwdt_Refresh();
}

/* Wdg 2.5s timeout */
void WATCHDOG_HAL_Init(void)
{
    /* clear IWDTRST Flag*/
    if (Rcm_ReadStatusFlag(RCM_FLAG_IWDTRST) != RESET)
    {
        Rcm_ClearStatusFlag();
    }
    
    /* set IWDT Write Access */
    Iwdt_EnableWriteAccess();

    /* set IWDT Divider*/
    Iwdt_ConfigDivider(IWDT_DIV_64);

    /* set IWDT Reloader*/
    Iwdt_ConfigReload(40000 / 32);

    /* Refresh*/
    Iwdt_Refresh();

    /* Enable IWDT*/
    Iwdt_Enable();
}

void WATCHDOG_HAL_Fed(void)
{
    Iwdt_Refresh();
}

