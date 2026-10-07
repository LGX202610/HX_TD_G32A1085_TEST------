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

#ifndef TIMER_HAL_H
#define TIMER_HAL_H

#include "includes.h"

/**************************************************************************
                    FUNCTION DECLARATION
**************************************************************************/
void TIMER_HAL_Init(void);
uint32 TIMER_HAL_GETRandTimerCnt(void);
boolean TIMER_HAL_Modified100msTickCheck(void);
void TIMER_HAL_1msTask(void);
boolean TIMER_HAL_Modified1msTickCheck(void);

#endif
