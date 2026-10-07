/*!
 * @file        g32a10xx_aes256.h
 *
 * @brief       This file contains all the functions prototypes for the AES256 firmware library
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
#ifndef G32A10xx_AES256_H
#define G32A10xx_AES256_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup AES256_Driver
  @{
*/

/** @defgroup AES256_Macros Macros
  @{
*/

/* Register Bit Definitions */
#define  AES256_CTRL_START_POS         (0U)
#define  AES256_CTRL_START_MSK         (0x1UL << AES256_CTRL_START_POS)
#define  AES256_CTRL_KEY_INT_EN_POS    (1U)
#define  AES256_CTRL_KEY_INT_EN_MSK    (0x1UL << AES256_CTRL_KEY_INT_EN_POS)
#define  AES256_CTRL_DATA_INT_EN_POS   (2U)
#define  AES256_CTRL_DATA_INT_EN_MSK   (0x1UL << AES256_CTRL_DATA_INT_EN_POS)
#define  AES256_CTRL_BIG_ENDIAN_POS    (3U)
#define  AES256_CTRL_BIG_ENDIAN_MSK    (0x1UL << AES256_CTRL_BIG_ENDIAN_POS)
#define  AES256_CTRL_KEY_LEN_POS       (4U)
#define  AES256_CTRL_KEY_LEN_MSK       (0x3UL << AES256_CTRL_KEY_LEN_POS)
#define  AES256_CTRL_OPCODE_POS        (6U)
#define  AES256_CTRL_OPCODE_MSK        (0x3UL << AES256_CTRL_OPCODE_POS)
#define  AES256_CTRL_MODE_POS          (8U)
#define  AES256_CTRL_MODE_MSK          (0xFUL << AES256_CTRL_MODE_POS)
#define  AES256_CTRL_Valid_length_POS  (12U)
#define  AES256_CTRL_Valid_length_MSK  (0x3FUL << AES256_CTRL_Valid_length_POS)
#define  AES256_CTRL_SubKG_POS         (19U)
#define  AES256_CTRL_SubKG_MSK         (0x1UL << AES256_CTRL_SubKG_POS)

#define  AES256_STATE_BUSY_POS         (0U)
#define  AES256_STATE_BUSY_MSK         (0x1UL << AES256_STATE_BUSY_POS)
#define  AES256_STATE_KEY_INT_FLG_POS  (1U)
#define  AES256_STATE_KEY_INT_FLG_MSK  (0x1UL << AES256_STATE_KEY_INT_FLG_POS)
#define  AES256_STATE_DATA_INT_FLG_POS (2U)
#define  AES256_STATE_DATA_INT_FLG_MSK (0x1UL << AES256_STATE_DATA_INT_FLG_POS)

/* AES Modes */
#define AES256_MODE_ECB                (0x0UL << AES256_CTRL_MODE_POS)
#define AES256_MODE_CBC                (0x1UL << AES256_CTRL_MODE_POS)
#define AES256_MODE_CMAC               (0x2UL << AES256_CTRL_MODE_POS)
#define AES256_MODE_MPC                (0x3UL << AES256_CTRL_MODE_POS)
#define AES256_MODE_CTR                (0x4UL << AES256_CTRL_MODE_POS)

/* Key Sizes */
#define AES256_KEYSIZE_128B            (0x0UL << AES256_CTRL_KEY_LEN_POS)
#define AES256_KEYSIZE_192B            (0x1UL << AES256_CTRL_KEY_LEN_POS)
#define AES256_KEYSIZE_256B            (0x2UL << AES256_CTRL_KEY_LEN_POS)

/* Operation Codes */
#define AES256_OPERATINGMODE_ENCRYPT   (0x0UL << AES256_CTRL_OPCODE_POS)
#define AES256_OPERATINGMODE_DECRYPT   (0x1UL << AES256_CTRL_OPCODE_POS)
#define AES256_OPERATINGMODE_EXPAND    (0x2UL << AES256_CTRL_OPCODE_POS)

/* Error Codes */
#define  AES256_ERROR_NONE             0x00000000U
#define  AES256_ERROR_TIMEOUT          0x00000001U
#define  AES256_ERROR_ILLEGAL          0x00000002U
#define  AES256_ERROR_INVALID_PARAM    0x00000003U

/* Endianness */
#define AES256_LITTLE_ENDIAN           0x00000000U
#define AES256_BIG_ENDIAN              (0x1UL << AES256_CTRL_BIG_ENDIAN_POS)

/* Data Width Unit */
#define AES256_DATAWIDTHUNIT_WORD      0x00000000U
#define AES256_DATAWIDTHUNIT_BYTE      0x00000001U

/* Constants */
#define AES256_BLOCK_SIZE_BYTES        16U
#define AES256_BLOCK_SIZE_WORDS        4U

/**@} end of group AES256_Macros */


/** @defgroup AES256_Structures Structures
  @{
*/

typedef struct
{
    uint32_t keySize;                    //!< Key size: 128, 192 or 256 bit
    const uint32_t *keyPtr;              //!< Pointer to the key buffer
    const uint32_t *initVectPtr;         //!< Pointer to the Initialization Vector (IV) / Counter
    uint32_t mode;                       //!< AES Mode: ECB, CBC, CMAC, MPC, CTR
    uint32_t bigEndian;                  //!< Endianness: AES256_LITTLE_ENDIAN or AES256_BIG_ENDIAN
    uint32_t dataWidthUnit;              //!< Data width unit: WORD or BYTE
} Aes256_ConfigType;

typedef struct
{
    Aes256_ConfigType                 init;                  //!< Initialization configuration
    const uint32_t                    *inputPtr;             //!< Pointer to input buffer
    uint32_t                          *outputPtr;            //!< Pointer to output buffer
    const uint8_t                     *inputCmacPtr;         //!< Pointer to CMAC input buffer (uint8_t)
    volatile uint16_t                 headerCount;           //!< Header data counter (unused in current impl)
    volatile uint16_t                 inputCount;            //!< Input data counter (in words)
    volatile uint16_t                 outputCount;           //!< Output data counter (in words)
    uint32_t                          size;                  //!< Total data size (in bytes or words, based on DataWidthUnit)
    uint32_t                          phase;                 //!< Current processing phase (unused in current impl)
    volatile uint32_t                 errorCode;             //!< Last error code
    uint32_t                          version;               //!< IP Version (unused in current impl)
    uint32_t                          keyIvConfig;           //!< Key/IV configuration flag (unused in current impl)
    uint32_t                          sizesSum;              //!< Sum of payload lengths (unused in current impl)
} Aes256_HandleType;

/**@} end of group AES256_Structures */

/** @defgroup AES256_Functions Functions
  @{
*/

/* Initialization, Reset Set */
void Aes256_Reset(void);
void Aes256_Init(Aes256_HandleType *Aes256HandlePtr);
void Aes256_SetEndian(const Aes256_HandleType *Aes256HandlePtr);

/* Core Operations */
uint32_t Aes256_Encrypt(Aes256_HandleType *Aes256HandlePtr, const uint32_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout);
uint32_t Aes256_Decrypt(Aes256_HandleType *Aes256HandlePtr, const uint32_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout);
uint32_t Aes256_GenerateCmac(Aes256_HandleType *Aes256HandlePtr, const uint8_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout);
uint32_t Aes256_CalculateMpc(Aes256_HandleType *Aes256HandlePtr, const uint32_t *InputPtr, uint16_t Size, uint32_t *OutputPtr, uint32_t Timeout);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_AES256_H */

/**@} end of group AES256_Functions */
/**@} end of group AES256_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
