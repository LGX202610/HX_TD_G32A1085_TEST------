/*!
 * @file        g32a10xx_eint.c
 *
 * @brief       This file contains all the functions for the EINT peripheral
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
#include "g32a10xx_eint.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup EINT_Driver
  @{
*/

/** @defgroup EINT_Functions Functions
  @{
*/

/*!
 * @brief     Set the EINT peripheral registers to their default reset values
 *
 * @param     None
 *
 * @retval    None
 */
void Eint_Reset(void)
{
    EINT->IMASK_R.IMASK = EINT_INTMASK_RESET_VALUE;
    EINT->EMASK_R.EMASK = EINT_EVTMASK_RESET_VALUE;
    EINT->RTEN_R.RTEN   = EINT_RTSEL_RESET_VALUE;
    EINT->FTEN_R.FTEN   = EINT_FTSEL_RESET_VALUE;
    EINT->IPEND_R.IPEND = EINT_PEND_RESET_VALUE;
}

/*!
 * @brief       Configure the EINT
 *
 * @param       eintConfig: Pointer to Eint_ConfigType structure
 *
 * @retval      None
 */
void Eint_Config(const Eint_ConfigType* eintConfigPtr)
{
    if (eintConfigPtr->lineCmd == (uint8_t)DISABLE)
    {
        if (eintConfigPtr->mode == EINT_MODE_INTERRUPT)
        {
            EINT->IMASK_R.IMASK &= ~eintConfigPtr->line;
        }
        else if (eintConfigPtr->mode == EINT_MODE_EVENT)
        {
            EINT->EMASK_R.EMASK &= ~eintConfigPtr->line;
        }
        else
        {
            /* nothing */
        }
    }
    else
    {
        if (eintConfigPtr->mode == EINT_MODE_INTERRUPT)
        {
            EINT->IMASK_R.IMASK |= eintConfigPtr->line;
        }
        else if (eintConfigPtr->mode == EINT_MODE_EVENT)
        {
            EINT->EMASK_R.EMASK |= eintConfigPtr->line;
        }
        else
        {
            /* nothing */
        }

        if (eintConfigPtr->trigger == EINT_TRIGGER_RISING)
        {
            EINT->RTEN_R.RTEN |= eintConfigPtr->line;
        }
        else if (eintConfigPtr->trigger == EINT_TRIGGER_FALLING)
        {
            EINT->FTEN_R.FTEN |= eintConfigPtr->line;
        }
        else
        {
            EINT->RTEN_R.RTEN |= eintConfigPtr->line;
            EINT->FTEN_R.FTEN |= eintConfigPtr->line;
        }
    }
}

/*!
 * @brief       Fills each Eint_ConfigType member with its default value
 *
 * @param       eintConfig: Pointer to a Eint_ConfigType structure which will be initialized
 *
 * @retval      None
 */
void Eint_ConfigStructInit(Eint_ConfigType* eintConfigPtr)
{
    eintConfigPtr->line    = EINT_LINENONE;
    eintConfigPtr->mode    = EINT_MODE_INTERRUPT;
    eintConfigPtr->trigger = EINT_TRIGGER_FALLING;
    eintConfigPtr->lineCmd = DISABLE;
}

/*!
 * @brief     Select software interrupt on EINT line
 *
 * @param     line: specifies the EINT line on which the software interrupt
 *
 * @retval    None
 */
void Eint_SelectSwInterrupt(uint32_t line)
{
    EINT->SWINTE_R.SWINTE |= (uint32_t)line;
}

/*!
 * @brief    Read the specified EINT line flag
 *
 * @param    line: Select the EINT line
 *
 * @retval   status: The new state of flag (SET or RESET)
 */
uint8_t Eint_ReadStatusFlag(uint32_t line)
{
    uint8_t status = RESET;

    if ((EINT->IPEND_R.IPEND & line) != (uint32_t)RESET)
    {
        status = SET;
    }
    else
    {
        status = RESET;
    }

    return status;
}

/*!
 * @brief    Clears the EINT line pending bits
 *
 * @param    line: Select the EINT line
 *
 * @retval   None
 */
void Eint_ClearStatusFlag(uint32_t line)
{
    EINT->IPEND_R.IPEND = line;
}

/*!
 * @brief    Read the specified EINT line interrupt flag
 *
 * @param    line: Select the EINT line
 *
 * @retval   None
 */
uint8_t Eint_ReadIntFlag(uint32_t line)
{
    uint8_t status = RESET;
    uint32_t enablestatus = 0;

    enablestatus = EINT->IMASK_R.IMASK & line;

    if (((EINT->IPEND_R.IPEND & line) != ((uint32_t)RESET)) && (enablestatus != (uint32_t)RESET))
    {
        status = SET;
    }
    else
    {
        status = RESET;
    }

    return status;
}

/*!
 * @brief    Clears the EINT line pending bits
 *
 * @param    line: Select the EINT line
 *
 * @retval   None
 */
void Eint_ClearIntFlag(uint32_t line)
{
    EINT->IPEND_R.IPEND = line;
}

/**@} end of group EINT_Functions */
/**@} end of group EINT_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
