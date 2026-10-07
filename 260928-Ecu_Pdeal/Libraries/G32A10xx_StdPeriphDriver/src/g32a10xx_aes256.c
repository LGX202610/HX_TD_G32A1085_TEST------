/*!
 * @file        g32a10xx_aes256.c
 *
 * @brief       This file provides all the AES256 firmware functions
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
#include "g32a10xx_aes256.h"
#include <string.h>

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup AES256_Driver
  @{
*/

/** @defgroup AES256_Functions Functions
  @{
*/

/*!
 * @brief     Waits until the AES256 BUSY flag is reset or timeout occurs.
 *
 * @param     Timeout: Maximum time to wait
 *
 * @retval    AES256_ERROR_NONE if successful, AES256_ERROR_TIMEOUT otherwise
 */
static uint32_t Aes256_WaitOnBusy(uint32_t Timeout)
{
    uint32_t state = AES256_ERROR_NONE;
    uint32_t timeCount = Timeout;

    while (timeCount > 0U)
    {
        if ((AES256->STATE_R.STATE & AES256_STATE_BUSY_MSK) != 0U)
        {
            state = AES256_ERROR_TIMEOUT;
        }
        else
        {
            state = AES256_ERROR_NONE;
            break;
        }
        timeCount--;
    }
    return state;
}

/*!
 * @brief     Reads a block (4 words) from the AES256 Output.
 *
 * @param     Aes256HandlePtr: Pointer to handle
 *
 * @retval    None
 */
static void Aes256_ReadOutput(Aes256_HandleType *Aes256HandlePtr)
{
    uint32_t const volatile *pDataOut = &(AES256->DATAOUT_0_R.DATAOUT_0);
    uint32_t idx = 0;
    uint32_t i = 0U;
    /* Copy to output buffer */
    for (i = 0U; i < AES256_BLOCK_SIZE_WORDS; i++)
    {
        idx = Aes256HandlePtr->outputCount;
        Aes256HandlePtr->outputPtr[idx] = pDataOut[i];
        Aes256HandlePtr->outputCount = Aes256HandlePtr->outputCount + 1U;
    }
}

/*!
 * @brief     Writes a block (up to 4 words) to the AES256 Input.
 *
 * @param     Aes256HandlePtr: Pointer to handle
 *
 * @retval    None
 */
static void Aes256_WriteInput(Aes256_HandleType *Aes256HandlePtr)
{
    volatile uint16_t *inCountPtr = &Aes256HandlePtr->inputCount;
    uint32_t maxWords = Aes256HandlePtr->size >> 2U;
    volatile uint32_t *pDataIn = &(AES256->DATAIN_0_R.DATAIN_0);
    uint32_t i = 0U;

    /* Copy to input buffer */
    for (i = 0U; (i < AES256_BLOCK_SIZE_WORDS) && (*inCountPtr < maxWords); i++)
    {
        pDataIn[i] = Aes256HandlePtr->inputPtr[*inCountPtr];

        (*inCountPtr) = (*inCountPtr) + 1U;
    }
}

/*!
 * @brief     Writes zero (up to 4 words) to the AES256 Input.
 *
 * @param     None
 *
 * @retval    None
 */
static void Aes256_ClearInput(void)
{
    volatile uint32_t *pDataIn = &(AES256->DATAIN_0_R.DATAIN_0);
    uint32_t i = 0U;

    /* Copy to input buffer */
    for (i = 0U; i < AES256_BLOCK_SIZE_WORDS; i++)
    {
        pDataIn[i] = 0;
    }
}

/*!
 * @brief     Writes a 16-byte block to AES256 Input FIFO from uint8_t buffer (for CMAC).
 *
 * @param     Aes256HandlePtr: Pointer to handle (Unused in this specific optimized logic but kept for signature)
 * @param     InputPtr:    Pointer to uint8_t input buffer
 * @param     offset: Offset in bytes from InputPtr
 *
 * @retval    None
 */
static void Aes256_WriteBlockCmac(const Aes256_HandleType *Aes256HandlePtr, const uint8_t *InputPtr, uint32_t Offset)
{
    uint8_t block[AES256_BLOCK_SIZE_BYTES] = {0};
    uint32_t remaining = Aes256HandlePtr->size - Offset;
    uint32_t copySize = (remaining >= AES256_BLOCK_SIZE_BYTES) ? AES256_BLOCK_SIZE_BYTES : remaining;
    volatile uint32_t *pDataIn = &(AES256->DATAIN_0_R.DATAIN_0);
    uint32_t i = 0U;

    (void)memcpy(block, &InputPtr[Offset], copySize);

    for (i = 0U; i < (AES256_BLOCK_SIZE_BYTES >> 2U); i++)
    {
        uint32_t wordIndex = i << 2U; // i * 4
        pDataIn[i] = ((uint32_t)block[wordIndex]     << 24) |
                     ((uint32_t)block[wordIndex + 1U] << 16) |
                     ((uint32_t)block[wordIndex + 2U] << 8)  |
                     ((uint32_t)block[wordIndex + 3U]);
    }
}

/*!
 * @brief     Sets the IV registers.
 *
 * @param     Aes256HandlePtr: Pointer to handle
 *
 * @retval    None
 */
static void Aes256_SetIv(const Aes256_HandleType *Aes256HandlePtr)
{
    volatile uint32_t *pIV = &(AES256->IV_0_R.IV_0);
    uint32_t i = 0U;

    if (Aes256HandlePtr->init.initVectPtr != NULL)
    {
        for (i = 0U; i < 4U; i++)
        {
            pIV[i] = Aes256HandlePtr->init.initVectPtr[i];
        }
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief     Set the encryption/decryption key.
 *
 * @param     Aes256HandlePtr: Pointer to handle
 *
 * @retval    None
 */
static void Aes256_SetKey(const Aes256_HandleType *Aes256HandlePtr)
{
    uint32_t keyWords = 0;
    volatile uint32_t *pKeyReg = NULL;
    uint32_t i = 0U;

    if ((Aes256HandlePtr == NULL) || (Aes256HandlePtr->init.keyPtr == NULL))
    {
        /* do nothing */
    }
    else
    {
        switch (Aes256HandlePtr->init.keySize)
        {
            case (uint32_t)AES256_KEYSIZE_256B:
                keyWords = 8U;  /* 256 bit / 32 = 8 words */
                break;
            case (uint32_t)AES256_KEYSIZE_192B:
                keyWords = 6U;  /* 192 bit / 32 = 6 words */
                break;
            case (uint32_t)AES256_KEYSIZE_128B:
            default:
                keyWords = 4U;  /* 128 bit / 32 = 4 words */
                break;
        }

        pKeyReg = &(AES256->KEY_0_R.KEY_0);

        for (i = 0U; i < keyWords; i++)
        {
            pKeyReg[i] = Aes256HandlePtr->init.keyPtr[i];
        }
    }
}

/*!
 * @brief     Common processing loop for ECB, CBC, CTR modes.
 *
 * @param     Aes256HandlePtr: Pointer to handle
 * @param     Timeout: Timeout value
 *
 * @retval    AES256_ERROR_NONE or AES256_ERROR_TIMEOUT
 */
static uint32_t Aes256_ProcessCommon(Aes256_HandleType *Aes256HandlePtr, uint32_t Timeout)
{
    uint32_t status = AES256_ERROR_NONE;
    /* inputCount and outputCount track words processed */
    uint16_t totalWords = (Aes256HandlePtr->init.dataWidthUnit == AES256_DATAWIDTHUNIT_WORD) ?
                    (uint16_t)((Aes256HandlePtr->size) >> 2) : (uint16_t)(Aes256HandlePtr->size);

    while (Aes256HandlePtr->inputCount < totalWords)
    {
        /* Write Input Block */
        Aes256_WriteInput(Aes256HandlePtr);

        /* Start Processing */
        AES256->CTRL_R.CTRL |= AES256_CTRL_START_MSK;

        /* Wait for completion */
        status = Aes256_WaitOnBusy(Timeout);
        if (status != AES256_ERROR_NONE)
        {
            Aes256HandlePtr->errorCode = status;
            break;
        }
        else
        {
            /* do nothing */
        }

        /* Read Output Block */
        Aes256_ReadOutput(Aes256HandlePtr);
    }

    return status;
}

/*!
 * @brief     Resets the AES256 peripheral registers to their default reset values.
 *
 * @param     None
 *
 * @retval    None
 */
void Aes256_Reset(void)
{
    AES256->CTRL_R.CTRL = 0x00000000;
    AES256->STATE_R.STATE = 0x00000000;
}

/*!
 * @brief     Initialize the AES256 peripheral
 *
 * @param     Aes256HandlePtr: Pointer to handle
 *
 * @retval    None
 */
void Aes256_Init(Aes256_HandleType *Aes256HandlePtr)
{
    if (Aes256HandlePtr == NULL)
    {
        /* do nothing */
    }
    else
    {
        AES256->CTRL_R.CTRL = Aes256HandlePtr->init.bigEndian | Aes256HandlePtr->init.keySize | Aes256HandlePtr->init.mode;
        Aes256HandlePtr->errorCode = AES256_ERROR_NONE;
    }
}

/*!
 * @brief     Set the Endian.
 *
 * @param     Aes256HandlePtr: Pointer to handle
 *
 * @retval    None
 */
void Aes256_SetEndian(const Aes256_HandleType *Aes256HandlePtr)
{
    if (Aes256HandlePtr->init.bigEndian == AES256_BIG_ENDIAN)
    {
        AES256->CTRL_R.CTRL |= AES256_CTRL_BIG_ENDIAN_MSK;
    }
    else
    {
        AES256->CTRL_R.CTRL &= ~AES256_CTRL_BIG_ENDIAN_MSK;
    }
}

/*!
 * @brief     AES256 Encryption mode.
 *
 * @param     Aes256HandlePtr: pointer to a Aes256_HandleTypeDef structure that contains
 *            the configuration information for AES256 module
 *
 * @param     InputPtr: Pointer to the input buffer (plaintext)
 *
 * @param     Size: Length of the plaintext buffer either in word or in byte, according to DataWidthUnit
 *
 * @param     OutputPtr: Pointer to the output buffer(ciphertext)
 *
 * @param     Timeout: Specify Timeout value
 *
 * @retval    status
 */
uint32_t Aes256_Encrypt(Aes256_HandleType *Aes256HandlePtr, const uint32_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout)
{
    uint32_t status = AES256_ERROR_NONE;
    uint32_t currentMode = AES256->CTRL_R.CTRL & AES256_CTRL_MODE_MSK;

    if ((Aes256HandlePtr == NULL) || (InputPtr == NULL) || (OutputPtr == NULL) || (Size == 0U))
    {
        status =  AES256_ERROR_INVALID_PARAM;
    }
    else if (Aes256HandlePtr->init.keyPtr == NULL)
    {
        Aes256HandlePtr->errorCode = AES256_ERROR_INVALID_PARAM;
        status =  AES256_ERROR_INVALID_PARAM;
    }
    else if ((currentMode != AES256_MODE_ECB) && (currentMode != AES256_MODE_CBC) && (currentMode != AES256_MODE_CTR))
    {
        Aes256HandlePtr->errorCode = AES256_ERROR_ILLEGAL;
        status = AES256_ERROR_ILLEGAL;
    }
    else
    {
        /* Initialize Handle */
        Aes256HandlePtr->inputPtr = InputPtr;
        Aes256HandlePtr->outputPtr = OutputPtr;
        Aes256HandlePtr->inputCount = 0U;
        Aes256HandlePtr->outputCount = 0U;
        Aes256HandlePtr->errorCode = AES256_ERROR_NONE;

        /* Calculate Size in Bytes */
        Aes256HandlePtr->size = (Aes256HandlePtr->init.dataWidthUnit == AES256_DATAWIDTHUNIT_WORD) ?
                        ((uint32_t)Size << 2U) : (uint32_t)Size;

        /* Set Operation Mode to Encrypt */
        AES256->CTRL_R.CTRL = (AES256->CTRL_R.CTRL & ~AES256_CTRL_OPCODE_MSK) | AES256_OPERATINGMODE_ENCRYPT;

        /* Set Key and IV */
        Aes256_SetKey(Aes256HandlePtr);
        if (Aes256HandlePtr->init.mode != AES256_MODE_ECB)
        {
            Aes256_SetIv(Aes256HandlePtr);
        }
        else
        {
            /* do nothing */
        }

        /* Process Data */
        status = Aes256_ProcessCommon(Aes256HandlePtr, Timeout);
        Aes256HandlePtr->errorCode = status;
    }
    return status;
}

/*!
 * @brief     AES256 Decryption mode.
 *
 * @param     Aes256HandlePtr: pointer to a Aes256_HandleTypeDef structure that contains
 *            the configuration information for AES256 module
 *
 * @param     InputPtr: Pointer to the input buffer (ciphertex)
 *
 * @param     Size: Length of the plaintext buffer either in word or in byte, according to DataWidthUnit
 *
 * @param     OutputPtr: Pointer to the output buffer(plaintext)
 *
 * @param     Timeout: Specify Timeout value
 *
 * @retval    status
 */
uint32_t Aes256_Decrypt(Aes256_HandleType *Aes256HandlePtr, const uint32_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout)
{
    uint32_t status = AES256_ERROR_NONE;
    uint32_t currentMode = AES256->CTRL_R.CTRL & AES256_CTRL_MODE_MSK;

    if ((Aes256HandlePtr == NULL) || (InputPtr == NULL) || (OutputPtr == NULL) || (Size == 0U))
    {
        status = AES256_ERROR_INVALID_PARAM;
    }
    else if (Aes256HandlePtr->init.keyPtr == NULL)
    {
        Aes256HandlePtr->errorCode = AES256_ERROR_INVALID_PARAM;
        status = AES256_ERROR_INVALID_PARAM;
    }
    else if ((currentMode != AES256_MODE_ECB) && (currentMode != AES256_MODE_CBC) && (currentMode != AES256_MODE_CTR))
    {
        Aes256HandlePtr->errorCode = AES256_ERROR_ILLEGAL;
        status = AES256_ERROR_ILLEGAL;
    }
    else
    {
        /* Initialize Handle */
        Aes256HandlePtr->inputPtr = InputPtr;
        Aes256HandlePtr->outputPtr = OutputPtr;
        Aes256HandlePtr->inputCount = 0U;
        Aes256HandlePtr->outputCount = 0U;
        Aes256HandlePtr->errorCode = AES256_ERROR_NONE;

        /* Calculate Size in Bytes */
        Aes256HandlePtr->size = (Aes256HandlePtr->init.dataWidthUnit == AES256_DATAWIDTHUNIT_WORD) ?
                        ((uint32_t)Size << 2U) : (uint32_t)Size;

        /* Key Preparation for CBC/ECB Decryption */
        if (currentMode != AES256_MODE_CTR)
        {
            /* Set Mode to Expand Key */
            AES256->CTRL_R.CTRL = (AES256->CTRL_R.CTRL & ~AES256_CTRL_OPCODE_MSK) | AES256_OPERATINGMODE_EXPAND;
            Aes256_SetKey(Aes256HandlePtr);

            /* Start Key Expansion */
            AES256->CTRL_R.CTRL |= AES256_CTRL_START_MSK;

            /* Wait for completion */
            status = Aes256_WaitOnBusy(Timeout);
            if (status != AES256_ERROR_NONE)
            {
                Aes256HandlePtr->errorCode = status;
            }
            else
            {
                /* Switch back to Decrypt Mode */
                AES256->CTRL_R.CTRL = (AES256->CTRL_R.CTRL & ~AES256_CTRL_OPCODE_MSK) | AES256_OPERATINGMODE_DECRYPT;
            }
        }
        else
        {
            /* CTR mode doesn't need key expansion for decryption */
            AES256->CTRL_R.CTRL = (AES256->CTRL_R.CTRL & ~AES256_CTRL_OPCODE_MSK) | AES256_OPERATINGMODE_DECRYPT;
            Aes256_SetKey(Aes256HandlePtr);
        }

        if (status != AES256_ERROR_NONE)
        {
            /* do nothing */
        }
        else
        {
            /* Set IV */
            if (Aes256HandlePtr->init.mode != AES256_MODE_ECB)
            {
                Aes256_SetIv(Aes256HandlePtr);
            }
            else
            {
                /* do nothing */
            }
            /* Process Data */
            status = Aes256_ProcessCommon(Aes256HandlePtr, Timeout);
            Aes256HandlePtr->errorCode = status;
        }
    }
    return status;
}

/*!
 * @brief     AES256 Generate CMAC.
 *
 * @param     Aes256HandlePtr: pointer to a Aes256_HandleTypeDef structure that contains
 *            the configuration information for AES256 module
 *
 * @param     InputPtr: Pointer to the input buffer (plaintext)
 *
 * @param     Size: Length of the plaintext buffer either in word or in byte, according to DataWidthUnit
 *
 * @param     OutputPtr: Pointer to the output buffer(ciphertex)
 *
 * @param     Timeout: Specify Timeout value
 *
 * @retval    status
 */
uint32_t Aes256_GenerateCmac(Aes256_HandleType *Aes256HandlePtr, const uint8_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout)
{
    uint32_t status = AES256_ERROR_NONE;
    uint32_t blkNum = 0, remainBytes = 0, processedBytes = 0;
    uint32_t dataLengthBits = 0;

    if ((Aes256HandlePtr == NULL) || (InputPtr == NULL) || (OutputPtr == NULL) || (Size == 0U))
    {
        status = AES256_ERROR_INVALID_PARAM;
    }
    else if (Aes256HandlePtr->init.keyPtr == NULL)
    {
        Aes256HandlePtr->errorCode = AES256_ERROR_INVALID_PARAM;
        status = AES256_ERROR_INVALID_PARAM;
    }
    else if ((AES256->CTRL_R.CTRL & AES256_CTRL_MODE_MSK) != AES256_MODE_CMAC)
    {
        Aes256HandlePtr->errorCode = AES256_ERROR_ILLEGAL;
        status = AES256_ERROR_ILLEGAL;
    }
    else
    {
        /* Initialize Handle */
        Aes256HandlePtr->inputCmacPtr = InputPtr;
        Aes256HandlePtr->outputPtr = OutputPtr;
        Aes256HandlePtr->size = Size; /* Size in bytes for CMAC */
        Aes256HandlePtr->inputCount = 0U;
        Aes256HandlePtr->outputCount = 0U;
        Aes256HandlePtr->errorCode = AES256_ERROR_NONE;

        dataLengthBits = (uint32_t)Size << 3U;
        blkNum = ((uint32_t)Size + AES256_BLOCK_SIZE_BYTES - (uint32_t)1U) / AES256_BLOCK_SIZE_BYTES;
        remainBytes = Size - ((blkNum - 1U) * AES256_BLOCK_SIZE_BYTES);

        /* Set Key */
        Aes256_SetKey(Aes256HandlePtr);
        /* Set IV */
        Aes256_SetIv(Aes256HandlePtr);

        /* Process full blocks except the last one */
        if (blkNum > 1U)
        {
            uint32_t fullBlocksBytes = (blkNum - 1U) * AES256_BLOCK_SIZE_BYTES;
            while (processedBytes < fullBlocksBytes)
            {
                Aes256_WriteBlockCmac(Aes256HandlePtr, InputPtr, processedBytes);
                processedBytes += AES256_BLOCK_SIZE_BYTES;

                AES256->CTRL_R.CTRL |= AES256_CTRL_START_MSK;
                status = Aes256_WaitOnBusy(Timeout);
                if (status != AES256_ERROR_NONE)
                {
                    Aes256HandlePtr->errorCode = status;
                    break;
                }
                else
                {
                    /* do nothing */
                }
            }
            Aes256_ClearInput();
        }
        else
        {
            /* do nothing */
        }
        if (status != AES256_ERROR_NONE)
        {
            /* do nothing */
        }
        else
        {

            /* Prepare and process the last block */
            /* change Mode to ECB*/
            AES256->CTRL_R.CTRL = (AES256->CTRL_R.CTRL & ~AES256_CTRL_MODE_MSK) | AES256_MODE_ECB;

            /* Set Valid Length for the last block */
            if (remainBytes < AES256_BLOCK_SIZE_BYTES)
            {
                uint32_t validLenBits = dataLengthBits - ((blkNum - 1U) * AES256_BLOCK_SIZE_BYTES * 8U);
                AES256->CTRL_R.CTRL = (AES256->CTRL_R.CTRL & ~AES256_CTRL_Valid_length_MSK) |
                               ((validLenBits << AES256_CTRL_Valid_length_POS) & AES256_CTRL_Valid_length_MSK);
            }
            else
            {
                 /* Clear Valid Length if full block */
                 AES256->CTRL_R.CTRL &= ~AES256_CTRL_Valid_length_MSK;
            }

            /* Enable SubKey Generation if required by HW for CMAC finalization */
            AES256->CTRL_R.CTRL |= AES256_CTRL_SubKG_MSK;

            /* Start Processing */
            AES256->CTRL_R.CTRL |= AES256_CTRL_START_MSK;

            status = Aes256_WaitOnBusy(Timeout);

            if (status != AES256_ERROR_NONE)
            {
                Aes256HandlePtr->errorCode = status;
            }
            else
            {
                /* Disable SubKey Generation */
                AES256->CTRL_R.CTRL &= ~AES256_CTRL_SubKG_MSK;

                /* Turn back to Mode of the configuration */
                AES256->CTRL_R.CTRL = (AES256->CTRL_R.CTRL & ~AES256_CTRL_MODE_MSK) | AES256_MODE_CMAC;

                /* Write the last block */
                Aes256_WriteBlockCmac(Aes256HandlePtr, InputPtr, processedBytes);

                /* Start Final Processing */
                AES256->CTRL_R.CTRL |= AES256_CTRL_START_MSK;

                status = Aes256_WaitOnBusy(Timeout);

                if (status != AES256_ERROR_NONE)
                {
                    Aes256HandlePtr->errorCode = status;
                }
                else
                {
                    /* Read the CMAC result (usually 4 words / 16 bytes) */
                    /* Reset counters for reading result */
                    Aes256HandlePtr->outputCount = 0U;
                    /* The result size is typically one block (4 words) */
                    Aes256HandlePtr->size = AES256_BLOCK_SIZE_BYTES;

                    Aes256_ReadOutput(Aes256HandlePtr);

                    Aes256HandlePtr->errorCode = AES256_ERROR_NONE;
                    status = AES256_ERROR_NONE;
                }
            }
        }
    }
    return status;
}

/*!
 * @brief     AES256 Calculate MPC.
 *
 * @param     Aes256HandlePtr: pointer to a Aes256_HandleTypeDef structure that contains
 *            the configuration information for AES256 module
 *
 * @param     InputPtr: Pointer to the input buffer (plaintext)
 *
 * @param     Size: Length of the plaintext buffer either in word or in byte, according to DataWidthUnit
 *
 * @param     OutputPtr: Pointer to the output buffer(ciphertex)
 *
 * @param     Timeout: Specify Timeout value
 *
 * @retval    status
 */
uint32_t Aes256_CalculateMpc(Aes256_HandleType *Aes256HandlePtr, const uint32_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout)
{
    uint32_t status = AES256_ERROR_NONE;
    uint16_t totalWords = 0;

    if ((Aes256HandlePtr == NULL) || (InputPtr == NULL) || (OutputPtr == NULL) || (Size == 0U))
    {
        status = AES256_ERROR_INVALID_PARAM;
    }
    else if ((AES256->CTRL_R.CTRL & AES256_CTRL_MODE_MSK) != AES256_MODE_MPC)
    {
        Aes256HandlePtr->errorCode = AES256_ERROR_ILLEGAL;
        status = AES256_ERROR_ILLEGAL;
    }
    else
    {
        /* Initialize Handle */
        Aes256HandlePtr->inputPtr = InputPtr;
        Aes256HandlePtr->outputPtr = OutputPtr;
        Aes256HandlePtr->inputCount = 0U;
        Aes256HandlePtr->outputCount = 0U;
        Aes256HandlePtr->errorCode = AES256_ERROR_NONE;

        /* Calculate Size in Bytes */
        Aes256HandlePtr->size = (Aes256HandlePtr->init.dataWidthUnit == AES256_DATAWIDTHUNIT_WORD) ?
                        ((uint32_t)Size << 2U) : (uint32_t)Size;

        totalWords = (uint16_t)(Aes256HandlePtr->size >> 2U);

        /* Process Input Blocks (MPC might not produce output immediately per block) */
        while (Aes256HandlePtr->inputCount < totalWords)
        {
            Aes256_WriteInput(Aes256HandlePtr);

            AES256->CTRL_R.CTRL |= AES256_CTRL_START_MSK;

            status = Aes256_WaitOnBusy(Timeout);
            if (status != AES256_ERROR_NONE)
            {
                Aes256HandlePtr->errorCode = status;
                break;
            }
            else
            {
                /* do nothing */
            }
        }
        if (status != AES256_ERROR_NONE)
        {
            Aes256HandlePtr->errorCode = status;
        }
        else
        {
            /* Read any remaining output data */
            /* Reset output count if MPC accumulates results differently,
               but assuming standard word count for output buffer fill */
            Aes256HandlePtr->outputCount = 0U;
            while (Aes256HandlePtr->outputCount < totalWords)
            {
                 /* Check if data is available? HW usually guarantees after Busy clears if output expected */
                 Aes256_ReadOutput(Aes256HandlePtr);
            }

            Aes256HandlePtr->errorCode = AES256_ERROR_NONE;
            status = AES256_ERROR_NONE;
        }
    }
    return status;
}

/**@} end of group AES256_Functions */
/**@} end of group AES256_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
