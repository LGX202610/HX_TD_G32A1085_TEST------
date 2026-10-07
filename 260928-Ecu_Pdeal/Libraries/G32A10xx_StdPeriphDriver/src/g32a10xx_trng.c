/*!
 * @file        g32a10xx_trng.c
 *
 * @brief       This file provides all the TRNG firmware functions
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

/* include */
#include "g32a10xx_trng.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup TRNG_Driver
  @{
*/

/** @defgroup TRNG_Macros Macros
  @{
*/
#define TRNG_TIMEOUT_VALUE     2U

/**@} end of group TRNG_Macros */

/** @defgroup TRNG_Functions Functions
  @{
*/

/*!
 * @brief     Resets the TRNG peripheral registers to their default reset values.
 *
 * @param     None
 *
 * @retval    None
 */
void Trng_Reset(void)
{
    TRNG->STS_R.STS = 0xFFFFFFFFU;
    TRNG->CTRL_R.CTRL = 0;
}

/*!
 * @brief     Enable the TRNG.
 *
 * @param     None
 *
 * @retval    None
 */
void Trng_Enable(void)
{
    TRNG->CTRL_R.CTRL |= TRNG_CTRL_TRNGEN_MSK;
}

/*!
 * @brief     Generates a 32-bit true random number
 *
 * @param     Random32bitPtr: pointer to generated true random number variable if successful
 *
 * @retval    status
 */
uint32_t Trng_GenerateRandomNumber(uint32_t *Random32bitPtr)
{
    uint32_t wait_times = TRNG_TIMEOUT_VALUE, status = TRNG_ERROR_TIMEOUT, trng_sts = 0;

    if (Random32bitPtr == NULL)
    {
        status = TRNG_ERROR_INVALID_PARAM;
    }
    else
    {
        while(wait_times > 0U)
        {
            trng_sts = READ_REG(TRNG->STS_R.STS);

            if ((trng_sts & TRNG_STS_DATARDY_MSK) != 0U)
            {
                *Random32bitPtr = TRNG->DATA_R.DATA;
                status = TRNG_ERROR_NONE;
            }
            else if ((trng_sts & TRNG_STS_FSCSTS_MSK) != 0U)
            {
                status = TRNG_ERROR_FSCSTS;
            }
            else if ((trng_sts & TRNG_STS_FSINT_MSK) != 0U)
            {
                status = TRNG_ERROR_FSINT;
            }
            else
            {
                status = TRNG_ERROR_TIMEOUT;
            }

            if((status == TRNG_ERROR_NONE) || (status == TRNG_ERROR_FSCSTS) || (status == TRNG_ERROR_FSINT))
            {
                break;
            }
            else
            {
                /* nothing */
            }

            wait_times--;
        }
    }

    return status;
}

/*!
 * @brief     Generates a 32-bit true random number in interrupt mode
 *
 * @param     None
 *
 * @retval    None
 */
void Trng_GenerateRandomNumber_It(void)
{
    TRNG->CTRL_R.CTRL |= TRNG_CTRL_INTEN_MSK;
}

/*!
 * @brief     Returns generated random number in polling mode
 *
 * @param     None
 *
 * @retval    Random value
 */
uint32_t Trng_GetRandomNumber(void)
{
    uint32_t random32bit = 0, val = 0;

    if (Trng_GenerateRandomNumber(&random32bit) == TRNG_ERROR_NONE)
    {
        val = random32bit;
    }
    else
    {
        val = 0U;
    }
    return val;
}

/*!
 * @brief     Returns a 32-bit random number with interrupt enabled
 *
 * @param     None
 *
 * @retval    Random value
 */
uint32_t Trng_GetRandomNumber_It(void)
{
    uint32_t random32bit = 0U;

    random32bit = TRNG->DATA_R.DATA;
    TRNG->CTRL_R.CTRL |= TRNG_CTRL_INTEN_MSK;

    return random32bit;
}

/**@} end of group TRNG_Functions */
/**@} end of group TRNG_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
