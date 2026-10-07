/*!
 * @file        g32a10xx_eint.h
 *
 * @brief       This file contains all the functions prototypes for the EINT firmware library
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
#ifndef G32A10xx_EINT_H
#define G32A10xx_EINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup EINT_Driver
  @{
*/

/** @defgroup EINT_Macros Macros
  @{
*/

#define EINT_LINENONE                  ((uint32_t)0x00000)       /*!< clear EINT line config */
#define EINT_INTMASK_RESET_VALUE       ((uint32_t)(0x0F940000))
#define EINT_EVTMASK_RESET_VALUE       ((uint32_t)(0x00000000))  /*!< EINT Event MASK register reset value */
#define EINT_RTSEL_RESET_VALUE         ((uint32_t)(0x00000000))  /*!< EINT Rising Trigger enable register reset value */
#define EINT_FTSEL_RESET_VALUE         ((uint32_t)(0x00000000))  /*!< EINT Falling Trigger enable register reset value */
#define EINT_PEND_RESET_VALUE          ((uint32_t)(0x00000000))  /*!< EINT Interrupt Pending register reset value */

/**@} end of group EINT_Macros */

/** @defgroup EINT_Enumerations Enumerations
  @{
*/

/*!
 * @brief   EINT mode
 */
typedef enum
{
    EINT_MODE_INTERRUPT    = ((uint8_t)0x00), /*!< EINT mode select interrupt */
    EINT_MODE_EVENT        = ((uint8_t)0x01)  /*!< EINT mode select event */
} Eint_ModeType;

/*!
 * @brief   EINT trigger mode
 */
typedef enum
{
    EINT_TRIGGER_RISING    = ((uint8_t)0x00), /*!< EINT trigger select rising edge */
    EINT_TRIGGER_FALLING   = ((uint8_t)0x01), /*!< EINT trigger select falling edge */
    EINT_TRIGGER_ALL       = ((uint8_t)0x02)  /*!< EINT trigger select rising and falling edge */
} Eint_TriggerType;

/*!
 * @brief   EINT line
 */
typedef enum
{
    EINT_LINE0   = ((uint32_t)0x00000001),  /*!< External interrupt line 0 */
    EINT_LINE1   = ((uint32_t)0x00000002),  /*!< External interrupt line 1 */
    EINT_LINE2   = ((uint32_t)0x00000004),  /*!< External interrupt line 2 */
    EINT_LINE3   = ((uint32_t)0x00000008),  /*!< External interrupt line 3 */
    EINT_LINE4   = ((uint32_t)0x00000010),  /*!< External interrupt line 4 */
    EINT_LINE5   = ((uint32_t)0x00000020),  /*!< External interrupt line 5 */
    EINT_LINE6   = ((uint32_t)0x00000040),  /*!< External interrupt line 6 */
    EINT_LINE7   = ((uint32_t)0x00000080),  /*!< External interrupt line 7 */
    EINT_LINE8   = ((uint32_t)0x00000100),  /*!< External interrupt line 8 */
    EINT_LINE9   = ((uint32_t)0x00000200),  /*!< External interrupt line 9 */
    EINT_LINE10  = ((uint32_t)0x00000400),  /*!< External interrupt line 10 */
    EINT_LINE11  = ((uint32_t)0x00000800),  /*!< External interrupt line 11 */
    EINT_LINE12  = ((uint32_t)0x00001000),  /*!< External interrupt line 12 */
    EINT_LINE13  = ((uint32_t)0x00002000),  /*!< External interrupt line 13 */
    EINT_LINE14  = ((uint32_t)0x00004000),  /*!< External interrupt line 14 */
    EINT_LINE15  = ((uint32_t)0x00008000),  /*!< External interrupt line 15 */
    EINT_LINE17  = ((uint32_t)0x00020000),  /*!< External interrupt line 17 Connected to the RTC Alarm event */
    EINT_LINE19  = ((uint32_t)0x00080000)   /*!< External interrupt line 19 Connected to the RTC Tamper and TimeStamp events */
} Eint_LineType;

/**@} end of group EINT_Enumerations */

/** @defgroup EINT_Structures Structures
  @{
*/

/*!
 * @brief   EINT Config struct definition
 */
typedef struct
{
    uint32_t          line;        /*!< Specifies the new state of the selected EINT lines. */
    Eint_ModeType     mode;        /*!< Specifies the mode for the EINT lines. */
    Eint_TriggerType  trigger;     /*!< Specifies the trigger signal active edge for the EINT lines. */
    uint8_t           lineCmd;     /*!< Specifies the EINT lines to be enabled or disabled. */
} Eint_ConfigType;

/**@} end of group EINT_Structures */

/** @defgroup EINT_Functions Functions
  @{
*/

/* Reset and configuration */
void Eint_Reset(void);
void Eint_Config(const Eint_ConfigType* eintConfigPtr);
void Eint_ConfigStructInit(Eint_ConfigType* eintConfigPtr);

/* Interrupt */
void Eint_SelectSwInterrupt(uint32_t line);

/* Flag */
uint8_t Eint_ReadStatusFlag(uint32_t line);
void    Eint_ClearStatusFlag(uint32_t line);
uint8_t Eint_ReadIntFlag(uint32_t line);
void    Eint_ClearIntFlag(uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_EINT_H */

/**@} end of group EINT_Functions */
/**@} end of group EINT_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
