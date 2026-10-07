/*!
 * @file        g32a10xx_sha256.c
 *
 * @brief       This file provides all the SHA256 firmware functions
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
#include "g32a10xx_sha256.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup SHA256_Driver
  @{
*/

/** @defgroup SHA256_Macros Macros
  @{
*/

#define SHA256_LITTLE_TO_BIG_ENDIAN(value) \
    ( \
        ( (((value) & 0x000000FFU) << 24) | \
          (((value) & 0x0000FF00U) <<  8) | \
          (((value) & 0x00FF0000U) >>  8) | \
          (((value) & 0xFF000000U) >> 24) ) \
    )

/**@} end of group SHA256_Macros */

/** @defgroup SHA256_Functions Functions
  @{
*/

/*!
 * @brief     Resets the SHA256 peripheral registers to their default reset values.
 *
 * @param     None
 *
 * @retval    None
 */
void Sha256_Reset(void)
{
    SHA256->STATUS_R.STATUS = 0xFFFFFFFFU;
    SHA256->BLKCNT_R.BLKCNT = 0x00000001;
    SHA256->DILH_R.DILH = 0x00000000;
    SHA256->DILL_R.DILL = 0x00000000;
    SHA256->CTRL_R.CTRL = 0x00000000;
}

/*!
 * @brief     Get the SHA256 peripheral REV register value.
 *
 * @param     None
 *
 * @retval    SHA256 REV register value
 */
uint32_t Sha256_GetRevision(void)
{
    return (SHA256->REV_R.REV);
}

/*!
 * @brief     Initialize the SHA256 peripheral, next process InputPtr then
 *            read the computed output message
 *
 * @param     Init: SHA256 init structure
 *
 * @param     InputPtr: pointer to the input buffer (buffer to be hashed)
 *
 * @param     Size: length of the input buffer in bytes
 *
 * @param     OutputPtr: pointer to the computed output message
 *
 * @param     Timeout: Timeout value
 *
 * @retval    status
 */
uint32_t Sha256_Start(Sha256_InitType Init,  const uint8_t *const InputPtr, uint64_t Size,
                      uint8_t *OutputPtr,
                      uint32_t Timeout)
{
    uint32_t status = SHA256_ERROR_TIMEOUT, sha256_sts = 0;
    const uint32_t *pIV_tmp = NULL;
    const uint8_t *pInBuffer_tmp = NULL;  /* input data address, input parameter of HASH_WriteData()         */
    uint64_t Size_tmp = 0; /* input data size (in bytes), input parameter of HASH_WriteData() */
    uint32_t time = Timeout;
    uint8_t i = 0;
    const volatile uint32_t *dout = &SHA256->DOUT_H0_R.DOUT_H0;

    if ((InputPtr == NULL) || (OutputPtr == NULL) || (Size == 0U))
    {
        status =  SHA256_ERROR_INVALID_PARAM;
    }
    else
    {
        pIV_tmp = Init.ivPtr;
        /* pInBuffer_tmp and Size_tmp are initialized to be used afterwards as
        input parameters of HASH_WriteData() */
        pInBuffer_tmp = (const uint8_t *)InputPtr;   /* pInBuffer_tmp is set to the input data address */
        Size_tmp = Size;             /* Size_tmp contains the input data size in bytes */

        SHA256->CTRL_R.CTRL = (SHA256_HASH_TYPE << SHA256_CTRL_TYPE_POS) |
                                ((uint32_t)Init.iniMode << SHA256_CTRL_INI_MODE_POS) |
                                ((uint32_t)Init.padMode << SHA256_CTRL_PAD_MODE_POS);
        SHA256->BLKCNT_R.BLKCNT = SHA256_BLKCNT_BLKN_DEFAULT << SHA256_BLKCNT_BLKN_POS;

        if (Init.padMode == SHA256_PAD_MODE)
        {
            SHA256->DILH_R.DILH = (uint32_t)(((Size << 3) >> 32) & 0xFFFFFFFFU);
            SHA256->DILL_R.DILL = (uint32_t) ((Size << 3) & 0xFFFFFFFFU);
        }
        else
        {
            /* nothing */
        }

        if (Init.iniMode == SHA256_NONE_INI_MODE)
        {
            Sha256_WriteDigest(pIV_tmp);
        }
        else
        {
            /* nothing */
        }

        Sha256_WriteData(pInBuffer_tmp, Size_tmp);

        (SHA256->CTRL_R.CTRL) |= (SHA256_CTRL_OP_START_MSK);

        while(time > 0U)
        {
            sha256_sts = READ_REG(SHA256->STATUS_R.STATUS);
            if (((sha256_sts & SHA256_STATUS_BUSY_MSK) == 0U) && ((sha256_sts & SHA256_STATUS_OP_DONE_MSK) != 0U))
            {
                (SHA256->STATUS_R.STATUS) &= ~(SHA256_STATUS_OP_DONE_MSK);
                dout = &SHA256->DOUT_H0_R.DOUT_H0;
                for (i = 0; i < 8U; ++i)
                {
                    uint32_t word = dout[i];
                    OutputPtr[(i * 4U) + 0U] = (uint8_t)(word >> 0);
                    OutputPtr[(i * 4U) + 1U] = (uint8_t)(word >> 8);
                    OutputPtr[(i * 4U) + 2U] = (uint8_t)(word >> 16);
                    OutputPtr[(i * 4U) + 3U] = (uint8_t)(word >> 24);
                }
                status = SHA256_ERROR_NONE;
                break;
            }
            else
            {
                status = SHA256_ERROR_TIMEOUT;
            }
            time--;
        }
    }
    return status;
}

/*!
 * @brief     Write the SHA256 peripheral DIGEST register value.
 *
 * @param     iv[8]: array to write IV register 8 times
 *
 * @retval    None
 */
void Sha256_WriteDigest(const uint32_t *Iv)
{
    uint8_t i = 0;

    if (Iv == NULL)
    {
        /* nothing */
    }
    else
    {
        for (i = 0; i < 8U; i++)
        {
            SHA256->DIGEST_R.DIGEST = SHA256_LITTLE_TO_BIG_ENDIAN(Iv[i]);
        }
    }
}

/*!
 * @brief     Write the SHA256 peripheral DIN register value.
 *
 * @param     InputPtr: pointer to the input buffer (buffer to be hashed)
 *
 * @param     Size: length of the input buffer in bytes
 *
 * @retval    None
 */
void Sha256_WriteData(const uint8_t *const InputPtr, uint64_t Size)
{
    uint64_t word_count = 0;
    uint32_t remainder = 0;
    uint32_t temp = 0;
    uint64_t i = 0;
    uint64_t offset = 0U;  /* byte offset */

    /* Define pointer for word-wise access to avoid array indexing overhead */
    const uint8_t *ptr = InputPtr;

    if ((InputPtr == NULL) || (Size == 0U))
    {
        /* nothing */
    }
    else
    {
        /* Calculate number of full 32-bit words */
        word_count = Size >> 2; /* Equivalent to len / 4 */

        /* Process full 32-bit words */
        for (i = 0; i < word_count; i++)
        {
            /* Manual construction to handle potentially unaligned source data safely on M0+ */
            temp =  (uint32_t)ptr[offset] |
                   ((uint32_t)ptr[offset + 1U] << 8)  |
                   ((uint32_t)ptr[offset + 2U] << 16) |
                   ((uint32_t)ptr[offset + 3U] << 24);

            SHA256->DIN_R.DIN = temp;
            offset += 4U;
        }

        /* Process remaining bytes (0 to 3 bytes) */
        remainder = (uint32_t)(Size & 0x03U); /* Equivalent to len % 4 */

        if (remainder > 0U)
        {
            temp = 0U;
    
            /* ptr[offset] now points to first remaining byte */
            switch (remainder)
            {
                case 3:
                    temp |= ((uint32_t)ptr[offset + 2U] << 16);
                    temp |= ((uint32_t)ptr[offset + 1U] << 8);
                    temp |=  (uint32_t)ptr[offset];
                    break;
                case 2:
                    temp |= ((uint32_t)ptr[offset + 1U] << 8);
                    temp |=  (uint32_t)ptr[offset];
                    break;
                case 1:
                    temp |=  (uint32_t)ptr[offset];
                    break;
                default:
                    {
                        /* nothing */
                    }
                    break;
            }
            SHA256->DIN_R.DIN = temp;
        }
        else
        {
            /* nothing */
        }
    }
}

/**@} end of group SHA256_Functions */
/**@} end of group SHA256_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
