/*!
 * @file        g32a10xx_sms.c
 *
 * @brief       This file provides firmware functions to manage the following
 *              functionalities of the sms peripheral
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

#ifdef __cplusplus
extern "C"{
#endif

#include "g32a10xx_sms.h"

/** @addtogroup G32A10xx_StdPeriphDriver 
  @{
*/

/** @defgroup SMS_Driver
  @{
*/

/**********************************************************************************************************************
                                                   GLOBAL VARIABLES
***********************************************************************************************************************/
/** @defgroup SMS_Variable Variable
  @{
*/

/** The configuration data of the Sms module */
static const Sms_ConfigType *g_smsConfigPtr = NULL;

/**@} end of group SMS_Variable Variable */

/** @defgroup SMS_Variable Variable
  @{
*/

/** The fault shift bits array */
static const uint8_t g_smsStatBitPosition[SMS_FAULTS_TOTAL_COUNT] = {(uint8_t)SMS_STAT_RD_ERR_SIN_SHIFT, \
                                                                     (uint8_t)SMS_STAT_RD_ERR_DBL_SHIFT, \
                                                                     (uint8_t)SMS_STAT_RMW_ERR_SIN_SHIFT, \
                                                                     (uint8_t)SMS_STAT_RMW_ERR_DBL_SHIFT};

/**@} end of group SMS_Variable Variable */

/**********************************************************************************************************************
                                                    GLOBAL FUNCTION
***********************************************************************************************************************/

/** @defgroup SMS_Functions Functions
  @{
*/

/*!
* @brief          The function is to get errors 
*
* @param          FaultPoolPtr  Error pool where the error flags shall be stored
*                              The bit map of the faults:
*                                    Bit 0: RD_ERR_SIN_ID
*                                    Bit 1: RD_ERR_DBL_ID
*                                    Bit 2: RMW_ERR_SIN_ID
*                                    Bit 3: RMW_ERR_DBL_ID
*
* @retval         E_OK:     There is no pending fault.
*                 E_NOT_OK: There is at least one pending fault.
*/
uint8_t Sms_GetErrors(uint32_t *FaultPoolPtr)
{
    uint8_t circleCnt = 0;
    uint32_t regVal = 0;
    uint8_t returnVal = E_OK;

    *FaultPoolPtr = 0U;

    /* Get the ECC error flags */
    regVal = SMS->INTRRn_R.INTRRn;

    if((0U != (regVal & SMS_INTRRn_RD_ERR_DET_MASK)) || (0U != (regVal & SMS_INTRRn_RMW_ERR_DET_MASK)))
    {
        while(circleCnt < SMS_FAULTS_TOTAL_COUNT)
        {
            if (0U != (regVal & ((uint32_t)1U << g_smsStatBitPosition[circleCnt])))
            {
                *FaultPoolPtr |= (uint32_t)1U << circleCnt;
                returnVal = E_NOT_OK;
            }
            else
            {
              /* do nothing */
            }

            circleCnt++;
        }
    }
    else
    {
      /* do nothing */
    }

    return (returnVal);
}

/*!
* @brief          The function is to clear the fault flags
*
* @param          None
*
* @retval         E_OK: the is cleared successfully.
*                 E_NOT_OK: the fault is cleared failed.
*/
uint8_t Sms_ClearFault(void)
{
    uint8_t returnVal = E_NOT_OK;
    uint32_t regValTemp = 0;

    /* Clear the fault flag */ 
    SMS->INTRRn_R.INTRRn = 0U;
    regValTemp = SMS->INTRRn_R.INTRRn;

    if (SMS_NO_ERR == regValTemp)
    {
        returnVal = E_OK;
    }
    else
    {
        /* do nothing */
    }

    return returnVal;
}

/*!
* @brief          The function is to init the Sms moudle.
*
* @param          SmsConfigPtr       The point of configuration parameter.
*
* @retval         E_OK:Initialize successfully.
*                 E_NOT_OK:Initialize failed.
*
* @note          This function additionally clear all injected errors.
*/
uint8_t Sms_Init(const Sms_ConfigType *SmsConfigPtr)
{
    g_smsConfigPtr = SmsConfigPtr;

    /* Clear the error intrrupt flags */
    SMS->INTRRn_R.INTRRn = 0xFFFFFFFFU;

    if((uint8_t)ENABLE == SmsConfigPtr->smsEccEn)
    {
        SMS->ECC_CFG_R.ECC_CFG_B.ECC_EN = ENABLE;
    }
    else
    {
        SMS->ECC_CFG_R.ECC_CFG_B.ECC_EN = DISABLE;
    }

    if((uint8_t)ENABLE == SmsConfigPtr->smsRdCorEn)
    {
        SMS->ECC_CFG_R.ECC_CFG_B.RD_COR_EN = ENABLE;
    }
    else
    {
        SMS->ECC_CFG_R.ECC_CFG_B.RD_COR_EN = DISABLE;
    }

    if((uint8_t)ENABLE == SmsConfigPtr->smsRmwCorEn)
    {
        SMS->ECC_CFG_R.ECC_CFG_B.RMW_COR_EN = ENABLE;
    }
    else
    {
        SMS->ECC_CFG_R.ECC_CFG_B.RMW_COR_EN = DISABLE;
    }

    return E_OK;
}

/*!
* @brief          The function is to inject a fault.
*
* @param          InjectBitErrType       The fault id to be injected.
*/
void Sms_InjectFault(Sms_InjectBitErrType InjectBitErrType)
{
    SuspendAllInterrupts();

    /* Clear the INSERT_ERR bit region of the SMS_ECC_CFG register */
    SMS->ECC_CFG_R.ECC_CFG_B.INSERT_ERR = 0U;

    switch(InjectBitErrType)
    {
        case SMS_INJECT_SINGLE_BIT_ERROR:
            SMS->ECC_CFG_R.ECC_CFG_B.INSERT_ERR = (uint8_t)SMS_INJECT_SINGLE_BIT_ERROR;
            break;
        case SMS_INJECT_DOUBLE_BIT_ERROR:
            SMS->ECC_CFG_R.ECC_CFG_B.INSERT_ERR = (uint8_t)SMS_INJECT_DOUBLE_BIT_ERROR;
            break;
        case SMS_INJECT_TRIPLE_BIT_ERROR:
            SMS->ECC_CFG_R.ECC_CFG_B.INSERT_ERR = (uint8_t)SMS_INJECT_TRIPLE_BIT_ERROR;
            break;
        default:
            /* Do nothing */
            break;
    }

    /* Write the value to the address */
    (*(volatile uint32_t*)(g_smsConfigPtr->smsInjectErrAddr)) = 0xFFFFFFFFU;

    /* Read the value of the address to inject an error:RD type */
    *(volatile uint32_t*)(g_smsConfigPtr->smsInjectErrAddr);
    /* Byte operation to trigger RMW operation with an error: RMW type */
    (*(volatile uint8_t*)(g_smsConfigPtr->smsInjectErrAddr)) = 1U;

    /* Disable the error injection */
    SMS->ECC_CFG_R.ECC_CFG_B.INSERT_ERR = (uint32_t)SMS_INJECT_NO_ERROR;

    ResumeAllInterrupts();
}

/*!
* @brief          The function is to get the error address.
*
* @param         None
*
* @retval        The address where the error occurred.
*/
uint32_t Sms_GetErrorAddr(void)
{
    uint32_t errorAddr = 0;

    errorAddr = SMS->ECC_LOGn_R.ECC_LOGn;
    errorAddr = (((errorAddr >> SMS_ECC_LOGn_ADDR_SHIFT) * 4U ) + SMS_RAM_START_ADDRESS);

    return (errorAddr);
}

/*!
* @brief         The function is to get the error bit position.
*
* @param         FaultId: The fault id:
*                             0: RD_ERR_SIN_ID
*                             1: RD_ERR_DBL_ID
*                             2: RMW_ERR_SIN_ID
*                             3: RMW_ERR_DBL_ID
*
* @retval        The bit position of the fault id in the status register.
*/
uint32_t Sms_GetErrorBitPosition(uint8_t FaultId)
{
    return (g_smsStatBitPosition[FaultId]);
}

/**@} end of grou SMS_Functions Functions */

#ifdef __cplusplus
}
#endif

/**@} end of group SMS_Driver */
/**@} end of group G32A10xx_StdPeriphDriver  */
