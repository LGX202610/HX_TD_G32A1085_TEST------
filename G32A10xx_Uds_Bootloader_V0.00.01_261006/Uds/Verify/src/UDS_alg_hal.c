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

#include "UDS_alg_hal.h"
#include "timer_hal.h"
#include "AES.h"
#include "ZLGKey.h"

/**************************************************************************
                    LOCAL VARIABLE
**************************************************************************/
#ifdef EN_AES_SA_ALGORITHM_SW
static const uint8 gs_aKey[] =
{ 0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, 0x08u, 0x09u, 0x0au, 0x0bu, 0x0cu, 0x0du, 0x0eu, 0x0fu };
#endif

#if defined (EN_AES_SA_ALGORITHM_SW) || defined (ALLOW_ZLG_ZXDOC_SA_ALGORITHM)
/* Here is not init, because this used for software random */
static uint32 gs_UDSSwTimerTick;
#endif

/**************************************************************************
                    GLOBAL FUNCTION
**************************************************************************/
/*!
 * @brief UDS software timer tick.
 */
void UDS_ALG_HAL_AddSWTimerTickCnt(void)
{
#if defined (EN_AES_SA_ALGORITHM_SW) || defined (ALLOW_ZLG_ZXDOC_SA_ALGORITHM)
    gs_UDSSwTimerTick++;
#endif
}


/*!
 * @brief To UDS get random data. This function returns get random data status.
 *
 * @param[in]   needLen     need random data len
 * @param[out]  pOutBuf     point random data buff
 *
 * @return get random data status.
 */
boolean UDS_ALG_HAL_MyReplacedFunc(const uint32 needLen, uint8 *pOutBuf)
{
    /* Local return flag */
    boolean bResult = TRUE;
    /* Local counter */
    uint8 cCounter = 0u;
    /* Temporary pointer */
    uint8 *pTempPtr = NULL_PTR;
    /* Random value container */
    uint32 uRandVal = (uint32)&cCounter;

    bResult = ((0u == needLen) || (NULL_PTR == pOutBuf)) ? FALSE : TRUE;

#if defined (EN_AES_SA_ALGORITHM_SW) || defined (ALLOW_ZLG_ZXDOC_SA_ALGORITHM)
    /* Obtain random from timer */
    uRandVal = TIMER_HAL_GETRandTimerCnt();
    uRandVal |= (gs_UDSSwTimerTick << 16u);
    Memory_Set_Seed(uRandVal);

    /* Invert if condition to check bResult */
    if (!(!(bResult))) 
    {
        pTempPtr = (uint8 *)&uRandVal;

        /* Change for to while loop */
        cCounter = 0u;
        while (cCounter < needLen)
        {
            /* Invert if condition */
            if (!(((cCounter & 0x03u) != 0x03u)))
            {
                uRandVal = Memory_Generate_Next_Value();
            }

            pOutBuf[cCounter] = pTempPtr[cCounter & 0x03u];
            cCounter++;
        }
    }

#endif

    /* Return final result */
    return bResult;
}


/*!
 * @brief To UDS decrypt data.This function returns decrypt data status.
 *
 * @param[in] pDeCipherData     point ciphertext
 * @param[in] pDataLength       point ciphertext data lenght
 * @param[out]  pDataBuffer     point plaintext
 *
 * @return decrypt data status.
 */
boolean UDS_ALG_HAL_DataDecryptionMethod(const uint8 *pDeCipherData, const uint32 pDataLength, uint8 *pDataBuffer)
{
#ifdef EN_AES_SA_ALGORITHM_SW
    AES_aesDecryptCore((sint8 *)pDeCipherData, pDataLength, (sint8 *)&gs_aKey[0], (sint8 *)pDataBuffer);
#endif
#ifdef ALLOW_ZLG_ZXDOC_SA_ALGORITHM
    /* Simple Security Access Algorithm Decryption function */
    ZLG_MyModifiedKey((sint8 *)pDeCipherData, pDataLength, (sint8 *)pDataBuffer);
#endif

    return FALSE;
}

/*!
 * @brief To UDS encrypt data. This function returns encrypt data status.
 *
 * @param[in] pDataIn       point plaintext
 * @param[in] dataLength    point plaintext data lenght
 * @param[out]  pDataOut    point ciphertext
 *
 * @return encrypt data status.
 */
boolean UDS_ALG_HAL_DataEncryptionProcess(const uint8 *pDataIn, const uint32 dataLength, uint8 *pDataOut)
{
#ifdef EN_AES_SA_ALGORITHM_SW
    /* Call the external AES function with renamed parameters. */
    AES_MyProcessAES((sint8 *)pDataIn, dataLength, (sint8 *)&gs_aKey[0], (sint8 *)pDataOut);
#endif

    return FALSE;
}


