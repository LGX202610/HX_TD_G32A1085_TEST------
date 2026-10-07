/*!
 * @file        g32a10xx_crc.h
 *
 * @brief       This file contains all the functions prototypes for the CRC firmware library
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

/* Define to prevent recursive inclusion */
#ifndef G32A10xx_CRC_H
#define G32A10xx_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup CRC_Driver
  @{
*/

/** @defgroup CRC_Enumerations Enumerations
  @{
*/

/**
 * @brief   CRC Reverse Input Data
 */
typedef enum
{
    CRC_REVERSE_INPUT_DATA_NO   = ((uint8_t)0x00), /*!< Bit order not affected */
    CRC_REVERSE_INPUT_DATA_8B   = ((uint8_t)0x01), /*!< Bit reversal done by byte */
    CRC_REVERSE_INPUT_DATA_16B  = ((uint8_t)0x02), /*!< Bit reversal done by half-word */
    CRC_REVERSE_INPUT_DATA_32B  = ((uint8_t)0x03)  /*!< Bit reversal done by word */
} Crc_ReverseInputDataType;

/**
 * @brief   CRC Polynomial Size
 */
typedef enum
{
    CRC_POLYNOMIAL_SIZE_7   = ((uint8_t)0x03), /*!< 7-bit polynomial for CRC calculation */
    CRC_POLYNOMIAL_SIZE_8   = ((uint8_t)0x02), /*!< 8-bit polynomial for CRC calculation */
    CRC_POLYNOMIAL_SIZE_16  = ((uint8_t)0x01), /*!< 16-bit polynomial for CRC calculation */
    CRC_POLYNOMIAL_SIZE_32  = ((uint8_t)0x00)  /*!< 32-bit polynomial for CRC calculation */
} Crc_PolynomialSiseType;

/**@} end of group CRC_Enumerations*/


/** @defgroup CRC_Functions Functions
  @{
*/

/* Reset CRC */
void Crc_Reset(void);

/* Reset DATA */
void Crc_ResetData(void);

/* Performed on input data */
void Crc_SelectReverseInputData(Crc_ReverseInputDataType RevInData);

/* Enable and Disable Reverse Output Data */
void Crc_EnableReverseOutputData(void);
void Crc_DisableReverseOutputData(void);

/* Write INITVAL register */
void Crc_WriteInitRegister(uint32_t InitValue);

/* Calculate CRC */
uint32_t Crc_CalculateCrc(uint32_t Data);
uint32_t Crc_CalculateBlockCrc(const uint32_t Buffer[], uint32_t BufferLength);

/* Read CRC */
uint32_t Crc_ReadCrc(void);

/* Independent Data(ID) */
void Crc_WriteIdRegister(uint8_t IdValue);
uint8_t Crc_ReadIdRegister(void);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_CRC_H */

/**@} end of group CRC_Functions */
/**@} end of group CRC_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
