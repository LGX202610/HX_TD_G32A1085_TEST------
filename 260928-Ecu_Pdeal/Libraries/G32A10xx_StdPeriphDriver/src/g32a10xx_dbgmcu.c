/*!
 * @file        g32a10xx_dbgmcu.c
 *
 * @brief       This file provides all the DBG firmware functions
 *
 * @version     V1.0.0
 *
 * @date        2026-02-25
 *
 * @attention
 *
 *  Copyright (C) 2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (Geehy Semiconductor Software License Agreement).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the Geehy Semiconductor Software License Agreement for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "g32a10xx_dbgmcu.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup DBG_Driver
  @{
*/

/** @defgroup DBG_Functions Functions
  @{
*/

/*!
 * @brief     Read Device Identifier
 *
 * @param     None
 *
 * @retval    The value of the Device Identifier
 */
uint32_t Dbg_ReadDevId(void)
{
    return ((uint32_t)DBG->IDCODE_R.IDCODE_B.EQR);
}

/*!
 * @brief     Read Revision Identifier
 *
 * @param     None
 *
 * @retval    The value of the Revision Identifier
 */
uint32_t Dbg_ReadRevId(void)
{
    return ((uint32_t)DBG->IDCODE_R.IDCODE_B.WVR);
}

/*!
 * @brief     Enable Debug Mode
 *
 * @param     Mode: specifies the low power mode.
 *                  The parameter can be combination of following values:
 *                  @arg DBG_MODE_STOP:    Keep debugger connection during STOP mode
 *                  @arg DBG_MODE_STANDBY: Keep debugger connection during STANDBY mode
 * @retval    None
 */
void Dbg_EnableDebugMode(Dbg_ModeType Mode)
{
    DBG->CFG_R.CFG |= (uint32_t)Mode;
}

/*!
 * @brief     Disable Debug Mode
 *
 * @param     Mode: specifies the low power mode.
 *                  The parameter can be combination of following values:
 *                  @arg DBG_MODE_STOP:    Keep debugger connection during STOP mode
 *                  @arg DBG_MODE_STANDBY: Keep debugger connection during STANDBY mode
 * @retval    None
 */
void Dbg_DisableDebugMode(Dbg_ModeType Mode)
{
    DBG->CFG_R.CFG &= (~(uint32_t)Mode);
}

/*!
 * @brief     Enable APB1 peripheral in Debug mode.
 *
 * @param     Peripheral: Specifies the APB1 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB1_PER_TMR2_STOP:    TMR2  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR3_STOP:    TMR3  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR6_STOPP:   TMR6 counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR4_STOP     TMR4 counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_RTC_STOP :    RTC stopped when Core is halted
 *                        @arg DBG_APB1_PER_IWDT_STOP:    IWDT stopped when Core is halted
 * @retval      None
 */
void Dbg_EnableApb1Periph(Dbg_Apb1PerType Peripheral)
{
    DBG->APB1F_R.APB1F |= (uint32_t)Peripheral;
}

/*!
 * @brief     Disable APB1 peripheral in Debug mode.
 *
 * @param     Peripheral: Specifies the APB1 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB1_PER_TMR2_STOP:    TMR2  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR3_STOP:    TMR3  counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR6_STOPP:   TMR6 counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_TMR4_STOP     TMR4 counter stopped when Core is halted
 *                        @arg DBG_APB1_PER_RTC_STOP :    RTC stopped when Core is halted
 *                        @arg DBG_APB1_PER_IWDT_STOP:    IWDT stopped when Core is halted
 * @retval      None
 */
void Dbg_DisableApb1Periph(Dbg_Apb1PerType Peripheral)
{
    DBG->APB1F_R.APB1F &= (~(uint32_t)Peripheral);
}

/*!
 * @brief     Enable APB2 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB2 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB2_PER_TMR1_STOP:    TMR1  counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR7_STOP:    TMR7  counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR8_STOP:    TMR8  counter stopped when Core is halted
 * @retval      None
 */
void Dbg_EnableApb2Periph(Dbg_Apb2PerType Peripheral)
{
    DBG->APB2F_R.APB2F |= (uint32_t)Peripheral;
}

/*!
 * @brief     Disable APB2 peripheral in Debug mode.
 *
 * @param     Peripheral: Specifies the APB2 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBG_APB2_PER_TMR1_STOP:    TMR1  counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR7_STOP:    TMR7  counter stopped when Core is halted
 *                        @arg DBG_APB2_PER_TMR8_STOP:    TMR8  counter stopped when Core is halted
 *
 * @retval    None
 */
void Dbg_DisableApb2Periph(Dbg_Apb2PerType Peripheral)
{
    DBG->APB2F_R.APB2F &= (~(uint32_t)Peripheral);
}

/**@} end of group DBG_Functions */
/**@} end of group DBG_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
