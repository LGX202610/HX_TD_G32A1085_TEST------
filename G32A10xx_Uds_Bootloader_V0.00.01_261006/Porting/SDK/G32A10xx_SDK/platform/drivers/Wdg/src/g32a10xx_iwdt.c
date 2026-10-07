/*!
 * @file        g32a10xx_iwdt.c
 *
 * @brief       This file contains all the functions for the IWDT peripheral
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

#include "g32a10xx_iwdt.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup IWDT_Driver
  @{
*/

/** @defgroup IWDT_Functions Functions
  @{
*/

/*!
 * @brief       Enable IWDT
 *
 * @param       None
 *
 * @retval      None
 */
void Iwdt_Enable(void)
{
    IWDT->KEY_R.KEY = (uint32_t)IWDT_KEY_ENABLE;
}

/*!
 * @brief       Enable write access to Divider and Counter Reload registers
 *
 * @param       None
 *
 * @retval      None
 */
void Iwdt_EnableWriteAccess(void)
{
    IWDT->KEY_R.KEY = (uint32_t)IWDT_KEY_ACCESS;
}

/*!
 * @brief       Disable write access to Divider and Counter Reload registers
 *
 * @param       None
 *
 * @retval      None
 */
void Iwdt_DisableWriteAccess(void)
{
    IWDT->KEY_R.KEY = 0;
}

/*!
 * @brief       Refresh IWDT
 *
 * @param       None
 *
 * @retval      None
 */
void Iwdt_Refresh(void)
{
    IWDT->KEY_R.KEY = (uint32_t)IWDT_KEY_REFRESH;
}

/*!
 * @brief       Divider configuration
 *
 * @param       divvalue: Specifies the divider
 *                   The parameter can be one of following values:
 *                      @arg IWDT_DIV_4:    Prescaler divider 4
 *                      @arg IWDT_DIV_8:    Prescaler divider 8
 *                      @arg IWDT_DIV_16:   Prescaler divider 16
 *                      @arg IWDT_DIV_32:   Prescaler divider 32
 *                      @arg IWDT_DIV_64:   Prescaler divider 64
 *                      @arg IWDT_DIV_128:  Prescaler divider 128
 *                      @arg IWDT_DIV_256:  Prescaler divider 256
 *
 * @retval      None
 */
void Iwdt_ConfigDivider(Iwdt_DivType divvalue)
{
    IWDT->PSC_R.PSC = (uint32_t)divvalue;
}

/*!
 * @brief       Set counter reload value
 *
 * @param       reload: Specifies the reload value
 *
 * @retval      None
 */
void Iwdt_ConfigReload(uint16_t reload)
{
    IWDT->CNTRLD_R.CNTRLD = (uint32_t)reload;
}

/*!
 * @brief       Set counter reload value
 *
 * @param       reload: Specifies the reload value
 *
 * @retval      None
 */
void Iwdt_ConfigWindowValue(uint16_t windowValue)
{
    IWDT->WIN_R.WIN = (uint32_t)windowValue;
}
/*!
 * @brief       Read the specified IWDT flag
 *
 * @param       flag: Specifies the flag to read
 *              The parameter can be one of following values:
 *                 @arg IWDT_FLAG_DIVU:  Watchdog prescaler value update
 *                 @arg IWDT_FLAG_CNTU:  Watchdog counter reload value update
 *                 @arg IWDT_FLAG_WINU:  Watchdog counter window value update
 *
 * @retval      status of IWDT_FLAG (SET or RESET)
 */
uint8_t Iwdt_ReadStatusFlag(uint8_t flag)
{
    uint8_t bitStatus = RESET;

    if ((IWDT->STS_R.STS & flag) != (uint32_t)RESET)
    {
        bitStatus = SET;
    }
    else
    {
        bitStatus = RESET;
    }

    return bitStatus;
}

/**@} end of group IWDT_Functions*/
/**@} end of group IWDT_Driver */
/**@} end of group G32A10xx_StdPeriphDriver*/
