/*!
 * @file        g32a10xx_dbgmcu.h
 *
 * @brief       This file contains all the functions prototypes for the DBG firmware library
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
#ifndef G32A10xx_DBGMCU_H
#define G32A10xx_DBGMCU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup DBG_Driver
  @{
*/

/** @defgroup DBG_Enumerations Enumerations
  @{
*/

/**
 * @brief   MCU Debug mode in the low power mode behavior
 */
typedef enum
{
    DBG_MODE_STOP    = ((uint32_t)0x02), /*!< Keep debugger connection during STOP mode */
    DBG_MODE_STANDBY = ((uint32_t)0x04)  /*!< Keep debugger connection during STANDBY mode */
} Dbg_ModeType;

/**
 * @brief   MCU Debug mode in the APB1 peripheral behavior
 */
typedef enum
{
    DBG_APB1_PER_TMR2_STOP  = ((uint32_t)0x01),              /*!< TMR2 counter stopped when Core is halted */
    DBG_APB1_PER_TMR3_STOP  = ((uint32_t)0x02),              /*!< TMR3 counter stopped when Core is halted */
    DBG_APB1_PER_TMR6_STOP  = ((uint32_t)0x10),              /*!< TMR6 counter stopped when Core is halted */
    DBG_APB1_PER_TMR4_STOP = ((uint32_t)0x100),              /*!< TMR4 counter stopped when Core is halted */
    DBG_APB1_PER_RTC_STOP   = ((uint32_t)0x400),             /*!< RTC counter stopped when Core is halted */
    DBG_APB1_PER_IWDT_STOP  = ((uint32_t)0x1000)             /*!< IWDT stopped when Core is halted */
} Dbg_Apb1PerType;

/**
 * @brief   MCU Debug mode in the APB2 peripheral behavior
 */
typedef enum
{
    DBG_APB2_PER_TMR1_STOP   = ((uint32_t)0x00800), /*!< TMR1 counter stopped when Core is halted */
    DBG_APB2_PER_TMR7_STOP   = ((uint32_t)0x10000), /*!< TMR7 counter stopped when Core is halted */
    DBG_APB2_PER_TMR8_STOP   = ((uint32_t)0x20000)  /*!< TMR8 counter stopped when Core is halted */
} Dbg_Apb2PerType;

/**@} end of group DBG_Enumerations */

/** @defgroup DBG_Functions Functions
  @{
*/

/* Read MCU ID Code  */
uint32_t Dbg_ReadDevId(void);
uint32_t Dbg_ReadRevId(void);

/* Debug Mode */
void Dbg_EnableDebugMode(Dbg_ModeType Mode);
void Dbg_DisableDebugMode(Dbg_ModeType Mode);

/* APB1 peripheral */
void Dbg_EnableApb1Periph(Dbg_Apb1PerType Peripheral);
void Dbg_DisableApb1Periph(Dbg_Apb1PerType Peripheral);

/* APB2 peripheral */
void Dbg_EnableApb2Periph(Dbg_Apb2PerType Peripheral);
void Dbg_DisableApb2Periph(Dbg_Apb2PerType Peripheral);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_DBGMCU_H */

/**@} end of group DBG_Functions */
/**@} end of group DBG_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
