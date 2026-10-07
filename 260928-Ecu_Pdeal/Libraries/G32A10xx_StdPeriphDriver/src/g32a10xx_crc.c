/*!
 * @file        g32a10xx_crc.c
 *
 * @brief       This file provides all the CRC firmware functions
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
#include "g32a10xx_crc.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup CRC_Driver
  @{
*/

/** @defgroup CRC_Functions Functions
  @{
*/

/*!
 * @brief     Resets the CRC peripheral registers to their default reset values.
 *
 * @param     None
 *
 * @retval    None
 */
void Crc_Reset(void)
{
    CRC->DATA_R.DATA = 0xFFFFFFFFU;
    CRC->INDATA_R.INDATA = 0x00;
    CRC->INITVAL_R.INITVAL = 0xFFFFFFFFU;
    CRC->CTRL_R.CTRL = 0x00000000;
}

/*!
 * @brief     Reset CRC data register (DATA)
 *
 * @param     None
 *
 * @retval    None
 */
void Crc_ResetData(void)
{
    CRC->CTRL_R.CTRL_B.RST = BIT_SET;
}

/*!
 * @brief     Selects the reverse operation to be performed on input data
 *
 * @param     RevInData:   Reverse input data
 *                         The parameter can be one of following values:
 *                         @arg CRC_REVERSE_INPUT_DATA_NO:   Bit order not affected
 *                         @arg CRC_REVERSE_INPUT_DATA_8B:   Bit reversal done by byte
 *                         @arg CRC_REVERSE_INPUT_DATA_16B:  Bit reversal done by half-word
 *                         @arg CRC_REVERSE_INPUT_DATA_32B:  Bit reversal done by word
 *
 * @retval    None
 */
void Crc_SelectReverseInputData(Crc_ReverseInputDataType RevInData)
{
    CRC->CTRL_R.CTRL_B.REVI = (uint8_t)RevInData;
}

/*!
 * @brief     Enable the reverse operation on output data
 *
 * @param     None
 *
 * @retval    None
 */
void Crc_EnableReverseOutputData(void)
{
    CRC->CTRL_R.CTRL_B.REVO = BIT_SET;
}

/*!
 * @brief     Disable the reverse operation on output data
 *
 * @param     None
 *
 * @retval    None
 */
void Crc_DisableReverseOutputData(void)
{
    CRC->CTRL_R.CTRL_B.REVO = BIT_RESET;
}

/*!
 * @brief     Initializes the INITVAL register.
 *
 * @param     InitValue: Programmable initial CRC value
 *
 * @retval    None
 */
void Crc_WriteInitRegister(uint32_t InitValue)
{
    CRC->INITVAL_R.INITVAL = InitValue;
}

/*!
 * @brief     Calculate a 32-bit CRC for a given data word (32 bits)
 *
 * @param     Data: data word(32-bit) to compute its CRC
 *
 * @retval    32-bit CRC
 */
uint32_t Crc_CalculateCrc(uint32_t Data)
{
    CRC->DATA_R.DATA = Data;

    return (CRC->DATA_R.DATA);
}

/*!
 * @brief     Computes the 32-bit CRC of a given buffer of data word(32-bit)
 *
 * @param     Buffer: Pointer to the buffer containing the data to be computed
 *
 * @param     BufferLength: buffer length
 *
 * @retval    32-bit CRC
 */
uint32_t Crc_CalculateBlockCrc(const uint32_t Buffer[], uint32_t BufferLength)
{
    uint32_t index = 0;

    for (index = 0; index < BufferLength; index++)
    {
        CRC->DATA_R.DATA = Buffer[index];
    }

    return (CRC->DATA_R.DATA);
}

/*!
 * @brief     Returns the current CRC value
 *
 * @param     None
 *
 * @retval    32-bit CRC
 */
uint32_t Crc_ReadCrc(void)
{
    return (CRC->DATA_R.DATA);
}

/*!
 * @brief     Stores a 8-bit data in the Independent Data(INDATA) register
 *
 * @param     IDValue: 8-bit value to be stored in the INDATA register
 *
 * @retval    None
 */
void Crc_WriteIdRegister(uint8_t IdValue)
{
    CRC->INDATA_R.INDATA = IdValue;
}

/*!
 * @brief      Returns a 8-bit data stored in the Independent Data(INDATA) register
 *
 * @param      None
 *
 * @retval     8-bit value of the INDATA register
 */
uint8_t Crc_ReadIdRegister(void)
{
    return ((uint8_t)(CRC->INDATA_R.INDATA));
}

/**@} end of group CRC_Functions */
/**@} end of group CRC_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
