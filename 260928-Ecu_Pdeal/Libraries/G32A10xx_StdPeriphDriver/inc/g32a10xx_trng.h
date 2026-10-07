/*!
 * @file        g32a10xx_trng.h
 *
 * @brief       This file contains all the functions prototypes for the TRNG firmware library
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
#ifndef G32A10xx_TRNG_H
#define G32A10xx_TRNG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup TRNG_Driver
  @{
*/

/** @defgroup TRNG_Macros Macros
  @{
*/
#define  TRNG_CTRL_TRNGEN_POS        (2U)
#define  TRNG_CTRL_TRNGEN_MSK        (0x1UL << TRNG_CTRL_TRNGEN_POS)   //!< 0x00000004
#define  TRNG_CTRL_INTEN_POS         (3U)
#define  TRNG_CTRL_INTEN_MSK         (0x1UL << TRNG_CTRL_INTEN_POS)    //!< 0x00000008

#define  TRNG_STS_DATARDY_POS        (0U)
#define  TRNG_STS_DATARDY_MSK        (0x1UL << TRNG_STS_DATARDY_POS)   //!< 0x00000001
#define  TRNG_STS_FSCSTS_POS         (2U)
#define  TRNG_STS_FSCSTS_MSK         (0x1UL << TRNG_STS_FSCSTS_POS)    //!< 0x00000004
#define  TRNG_STS_FSINT_POS          (6U)
#define  TRNG_STS_FSINT_MSK          (0x1UL << TRNG_STS_FSINT_POS)     //!< 0x00000040

#define  TRNG_ERROR_NONE             0x00000000U                       //!< No error
#define  TRNG_ERROR_FSCSTS           0x00000001U                       //!< FSCSTS error
#define  TRNG_ERROR_FSINT            0x00000002U                       //!< FSINT error
#define  TRNG_ERROR_TIMEOUT          0x00000003U                       //!< Timeout error
#define  TRNG_ERROR_ILLEGAL          0x00000004U                       //!< Illegal error
#define  TRNG_ERROR_INVALID_PARAM    0x00000005U                       //!< Invalid error

/**@} end of group TRNG_Macros */

/** @defgroup TRNG_Functions Functions
  @{
*/

/* Reset TRNG */
void Trng_Reset(void);

/* Enable TRNG */
void Trng_Enable(void);

/* Generates a 32-bit true random number */
uint32_t Trng_GenerateRandomNumber(uint32_t *Random32bitPtr);

/* Generates a 32-bit true random number in interrupt mode */
void Trng_GenerateRandomNumber_It(void);

/* Returns generated random number in polling mode */
uint32_t Trng_GetRandomNumber(void);

/* Returns a 32-bit random number with interrupt enabled */
uint32_t Trng_GetRandomNumber_It(void);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_TRNG_H */

/**@} end of group TRNG_Functions */
/**@} end of group TRNG_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
