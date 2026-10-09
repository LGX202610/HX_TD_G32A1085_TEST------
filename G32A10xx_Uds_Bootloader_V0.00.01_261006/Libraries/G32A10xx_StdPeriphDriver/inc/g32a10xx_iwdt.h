/*!
 * @file        g32a10xx_iwdt.h
 *
 * @brief       This file contains all the functions prototypes for the IWDT firmware library
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
#ifndef G32A10xx_IWDT_H
#define G32A10xx_IWDT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup IWDT_Driver
  @{
*/

/** @defgroup IWDT_Enumerations Enumerations
  @{
*/

/**
 * @brief   IWDT key definition
 */
typedef enum
{
    IWDT_KEY_REFRESH = ((uint16_t)0xAAAA), /*!< Value of  Reload Register reoload to the counter to prevent the IWDT from resetting */
    IWDT_KEY_ENABLE  = ((uint16_t)0xCCCC), /*!< Enable the IWDT then the counter starts to count down from the reset value */
    IWDT_KEY_ACCESS  = ((uint16_t)0x5555)  /*!< Rewrite the value of the Prescaler Register, Reload Register and Window Value Register */
} Iwdt_KeyType;

/**
 * @brief   IWDT divider
 */
typedef enum
{
    IWDT_DIV_4   = ((uint8_t)0x00), /*!< Prescaler divider 4 */
    IWDT_DIV_8   = ((uint8_t)0x01), /*!< Prescaler divider 8 */
    IWDT_DIV_16  = ((uint8_t)0x02), /*!< Prescaler divider 16 */
    IWDT_DIV_32  = ((uint8_t)0x03), /*!< Prescaler divider 32 */
    IWDT_DIV_64  = ((uint8_t)0x04), /*!< Prescaler divider 64 */
    IWDT_DIV_128 = ((uint8_t)0x05), /*!< Prescaler divider 128 */
    IWDT_DIV_256 = ((uint8_t)0x06)  /*!< Prescaler divider 256 */
} Iwdt_DivType;

/**
 * @brief   IWDT flag definition
 */
typedef enum
{
    IWDT_FLAG_DIVU = ((uint8_t)0X01), /*!< Watchdog prescaler value update */
    IWDT_FLAG_CNTU = ((uint8_t)0X02), /*!< Watchdog counter reload value update */
    IWDT_FLAG_WINU = ((uint8_t)0X04)  /*!< Watchdog counter window value update */
} Iwdt_FlagType;

/**@} end of group IWDT_Enumerations */

/** @defgroup IWDT_Functions Functions
  @{
*/

/* Enable IWDT */
void Iwdt_Enable(void);

/* Refresh IWDT */
void Iwdt_Refresh(void);

/* Window Value */
void Iwdt_ConfigWindowValue(uint16_t windowValue);

/* Set Counter reload */
void Iwdt_ConfigReload(uint16_t reload);

/* Write Access */
void Iwdt_EnableWriteAccess(void);
void Iwdt_DisableWriteAccess(void);

/* divider */
void Iwdt_ConfigDivider(Iwdt_DivType divvalue);

/* flag */
uint8_t Iwdt_ReadStatusFlag(uint8_t flag);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_IWDT_H */

/**@} end of group IWDT_Functions */
/**@} end of group IWDT_Driver */
/**@} end of group G32A10xx_StdPeriphDriver*/
