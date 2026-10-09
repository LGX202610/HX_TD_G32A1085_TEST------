/*!
* @file     system_g32a10xx.h
*
* @version  1.0.0
*
* @brief    This file contains the system clock configuration for G32A10XX devices
*
* @attention
*
* Project Name      : AUTOSAR MCAL
*
* Platform          : Arm
*
* Autosar Standard  : Classic Platform 4.4.0
*
* Compiled Version  : G32A10xx_MCAL_1_0_0_01-Otc-15
*
* Copyright (C) 2025 Geehy Semiconductor
*
* You may not use this file except in compliance with the
* GEEHY COPYRIGHT NOTICE (Geehy Semiconductor Software License Agreement).
*
* The program is only for reference, which is distributed in the hope
* that it will be usefull and instructional for customers to develop
* their software. Unless required by applicable law or agreed to in
* writing, the program is distributed on an "AS IS" BASIS, WITHOUT
* ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
* See the Geehy Semiconductor Software License Agreement for the governing permissions
* and limitations under the License.
*/

#ifndef SYSTEM_G32A10xx_H
#define SYSTEM_G32A10xx_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup G32A10xx_StdPeriphDriver 
  @{
*/


/** @defgroup System_Variables Variables
  @{
  */

/* System Clock Frequency (Core Clock) */
extern uint32_t SystemCoreClock;

/**@} end of group System_Variables */

/** @defgroup System_Functions_Functions
  @{
*/

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

/**@} end of group System_Functions_Functions */

#ifdef __cplusplus
}
#endif

#endif

/**@} end of group G32A10xx_StdPeriphDriver  */
