/*!
 * @file        g32a10xx_sha256.h
 *
 * @brief       This file contains all the functions prototypes for the SHA256 firmware library
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
#ifndef G32A10xx_SHA256_H
#define G32A10xx_SHA256_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup SHA256_Driver
  @{
*/

/** @defgroup SHA256_Macros Macros
  @{
*/
#define SHA256_HASH_TYPE               (0x2UL)
#define SHA256_BLKCNT_BLKN_DEFAULT     (0x1UL)

#define SHA256_CTRL_OP_START_POS      (0U)
#define SHA256_CTRL_OP_START_MSK      (0x1UL << SHA256_CTRL_OP_START_POS) //!< 0x00000001
#define SHA256_CTRL_IE_POS            (1U)
#define SHA256_CTRL_IE_MSK            (0x1UL << SHA256_CTRL_IE_POS)       //!< 0x00000002
#define SHA256_CTRL_TYPE_POS          (2U)
#define SHA256_CTRL_TYPE_MSK          (0xFUL << SHA256_CTRL_TYPE_POS)     //!< 0x0000003C
#define SHA256_CTRL_INI_MODE_POS      (6U)
#define SHA256_CTRL_INI_MODE_MSK      (0x1UL << SHA256_CTRL_INI_MODE_POS) //!< 0x00000040
#define SHA256_CTRL_PAD_MODE_POS      (7U)
#define SHA256_CTRL_PAD_MODE_MSK      (0x1UL << SHA256_CTRL_PAD_MODE_POS) //!< 0x00000080

#define SHA256_STATUS_OP_DONE_POS     (0U)
#define SHA256_STATUS_OP_DONE_MSK     (0x1UL << SHA256_STATUS_OP_DONE_POS)//!< 0x00000001
#define SHA256_STATUS_BUSY_POS        (1U)
#define SHA256_STATUS_BUSY_MSK        (0x1UL << SHA256_STATUS_BUSY_POS)   //!< 0x00000002
#define SHA256_STATUS_INTR_POS        (2U)
#define SHA256_STATUS_INTR_MSK        (0x1UL << SHA256_STATUS_INTR_POS)   //!< 0x00000004

#define SHA256_BLKCNT_BLKN_POS        (0U)
#define SHA256_BLKCNT_BLKN_MSK        (0x3FUL << SHA256_BLKCNT_BLKN_POS)  //!< 0x0000003F

#define SHA256_ERROR_NONE             0x00000000U
#define SHA256_ERROR_TIMEOUT          0x00000001U
#define SHA256_ERROR_ILLEGAL          0x00000002U
#define SHA256_ERROR_INVALID_PARAM    0x00000003U

/**@} end of group SHA256_Macros */

/** @defgroup SHA256_Enumerations Enumerations
  @{
*/

/*!
 * @brief Endian Type
 */
typedef enum
{
    ENDIAN_LITTLE,
    ENDIAN_BIG
} Sha256_BigEndianType;

/*!
 * @brief Sha256 Ini Mode
 */
typedef enum
{
    SHA256_INI_MODE = 0U,
    SHA256_NONE_INI_MODE = 1U
}Sha256_IniModeType;

/*!
 * @brief Sha256 Pad Mode
 */
typedef enum
{
    SHA256_PAD_MODE = 0U,
    SHA256_NONE_PAD_MODE = 1U
}Sha256_PadModeType;

/**@} end of group SHA256_Enumerations */

/** @defgroup SHA256_Structures Structures
  @{
*/

/*!
 * @brief Sha256 init structure
 */
typedef struct
{
    Sha256_IniModeType iniMode;        //!< The key is used only in HMAC operation.
    Sha256_PadModeType padMode;        //!< The key is used only in HMAC operation.
    const uint32_t *ivPtr;             //!< The key is used only in HMAC operation.
} Sha256_InitType;

/**@} end of group SHA256_Structures */


/** @defgroup SHA256_Functions Functions
  @{
*/

/* Reset SHA256 */
void Sha256_Reset(void);

/* Get SHA256 Revision*/
uint32_t Sha256_GetRevision(void);

/* Get Hash */
uint32_t Sha256_Start(Sha256_InitType Init,  const uint8_t *const InputPtr, uint64_t Size,
                      uint8_t *OutputPtr,
                      uint32_t Timeout);

/* Write SHA256 Digest*/
void Sha256_WriteDigest(const uint32_t Iv[8]);

/* Write SHA256 DIN*/
void Sha256_WriteData(const uint8_t *const InputPtr, uint64_t Size);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_SHA256_H */

/**@} end of group SHA256_Functions */
/**@} end of group SHA256_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
