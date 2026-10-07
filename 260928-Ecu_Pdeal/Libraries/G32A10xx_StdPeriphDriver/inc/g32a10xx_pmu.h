/*!
 * @file        g32a10xx_pmu.h
 *
 * @brief       This file contains all functions prototype and macros for the PMU peripheral
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

#ifndef G32A10xx_PMU_H
#define G32A10xx_PMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup PMU_Driver
  @{
*/

/** @defgroup PMU_Enumerations Enumerations
  @{
*/

/**
 * @brief   Sleep mode entry
 */
typedef enum
{
    PMU_SLEEPENTRY_WFI = 0x00,  /*!< enter SLEEP mode with WFI instruction */
    PMU_SLEEPENTRY_WFE = 0x01   /*!< enter SLEEP mode with WFE instruction */
} Pmu_SleepEntryType;

/**
 * @brief   Regulator state is Sleep/Stop mode
 */
typedef enum
{
    PMU_REGULATOR_ON = 0x00,         /*!< STOP mode with regulator ON */
    PMU_REGULATOR_LowPower = 0x01    /*!< STOP mode with regulator in low power mode */
} Pmu_RegulatorType;

/**
 * @brief   Stop mode entry
 */
typedef enum
{
    PMU_STOPENTRY_WFI = 0x00,             /*!< Enter STOP mode with WFI instruction */
    PMU_STOPENTRY_WFE = 0x01,             /*!< Enter STOP mode with WFE instruction */
    PMU_STOPENTRY_SLEEPONEXIT = 0x02      /*!< Enter STOP mode with SLEEPONEXIT instruction */
} Pmu_StopEntryType;

/**
 * @brief   Flag
 */
typedef enum
{
    PMU_FLAG_WUPF      = 0x01,  /*!< Wake Up flag */
    PMU_FLAG_STDBYF    = 0x02,  /*!< StandBy flag */
    PMU_FLAG_PVDOF     = 0x04,
    PMU_FLAG_VREFINTF  = 0x08   /*!<VREFINT flag */
} Pmu_FlagType;

/**@} end of group PMU_Enumerations*/

/** @defgroup PMU_Functions Functions
  @{
*/

/** Function used to set the PMU configuration to the default reset state */
void Pmu_Reset(void);

/** Backup Domain Access function */
void Pmu_EnableBackupAccess(void);
void Pmu_DisableBackupAccess(void);

/** Low Power modes configuration functions */
void Pmu_EnterSleepMode(Pmu_SleepEntryType entry);
void Pmu_EnterStopMode(Pmu_RegulatorType regulator, Pmu_StopEntryType entry);
void Pmu_EnterStandbyMode(void);

/** Flags management functions */
uint8_t Pmu_ReadStatusFlag(Pmu_FlagType flag);
void Pmu_ClearStatusFlag(uint8_t flag);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_PMU_H */

/**@} end of group PMU_Functions */
/**@} end of group PMU_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
