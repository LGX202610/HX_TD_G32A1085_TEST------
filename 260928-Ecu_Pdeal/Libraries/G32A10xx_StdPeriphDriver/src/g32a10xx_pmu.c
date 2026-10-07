/*!
 * @file        g32a10xx_pmu.c
 *
 * @brief       This file contains all the functions for the PMU peripheral
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

#include "g32a10xx_pmu.h"
#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup PMU_Driver
  @{
*/

/** @defgroup  PMU_Functions Functions
  @{
*/

/*!
 * @brief   Resets the PWR peripheral registers to their default reset values
 *
 * @param   None
 *
 * @retval  None
 */
void Pmu_Reset(void)
{
    Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_PMU);
    Rcm_DisableApb1PeriphClock(RCM_APB1_PERIPH_PMU);
}

/*!
 * @brief   Enables access to the Backup domain registers
 *
 * @param   None
 *
 * @retval  None
 */
void Pmu_EnableBackupAccess(void)
{
    PMU->CTRL_R.CTRL_B.BPWEN = BIT_SET;
}

/*!
 * @brief   Disables access to the Backup domain registers
 *
 * @param   None
 *
 * @retval  None
 */
void Pmu_DisableBackupAccess(void)
{
    PMU->CTRL_R.CTRL_B.BPWEN = BIT_RESET;
}

/*!
 * @brief   Enters Sleep mode
 *
 * @param   entry :specifies if SLEEP mode in entered with WFI or WFE instruction
 *                 This parameter can be one of the following values:
 *                 @arg PMU_SLEEPENTRY_WFI: enter SLEEP mode with WFI instruction
 *                 @arg PMU_SLEEPENTRY_WFE: enter SLEEP mode with WFE instruction
 *
 * @retval  None
 */
void Pmu_EnterSleepMode(Pmu_SleepEntryType entry)
{
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);

    if (entry == PMU_SLEEPENTRY_WFI)
    {
        __WFI();
    }
    else
    {
        __SEV();
        __WFE();
        __WFE();
    }
}

/*!
 * @brief   Enters STOP mode
 *
 * @param   regulator: specifies the regulator state in STOP mode
 *                     This parameter can be one of the following values:
 *                     @arg PMU_REGULATOR_ON: STOP mode with regulator ON
 *                     @arg PMU_REGULATOR_LowPower: STOP mode with regulator in low power mode
 *
 * @param   entry:     specifies if STOP mode in entered with WFI or WFE instruction
 *                     This parameter can be one of the following values:
 *                     @arg PMU_STOPENTRY_WFI: enter STOP mode with WFI instruction
 *                     @arg PMU_STOPENTRY_WFE: enter STOP mode with WFE instruction
 *                     @arg PMU_STOPENTRY_SLEEPONEXIT: enter STOP mode with SLEEPONEXIT instruction
 *
 * @retval  None
 */
void Pmu_EnterStopMode(Pmu_RegulatorType regulator, Pmu_StopEntryType entry)
{
    PMU->CTRL_R.CTRL_B.PDDSCFG = BIT_RESET;

    PMU->CTRL_R.CTRL_B.LPDSCFG = (uint32_t)regulator;

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    switch (entry)
    {
        case PMU_STOPENTRY_WFI:

            __WFI();
            SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
            break;

        case  PMU_STOPENTRY_WFE:
            __WFE();
            __WFE();
            SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
            break;

        case PMU_STOPENTRY_SLEEPONEXIT:
            SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
            break;

        default:
            {
                /* nothing */
            }
            break;
    }
}

/*!
 * @brief   Enters STANDBY mode
 *
 * @param   None
 *
 * @retval  None
 */
void Pmu_EnterStandbyMode(void)
{
    PMU->CTRL_R.CTRL_B.PDDSCFG = BIT_SET;

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    __WFI();
}

/*!
 * @brief   Checks whether the specified PMU flag is set or not
 *
 * @param   flag: specifies the flag to check
 *                This parameter can be one of the following values:
 *                @arg PMU_FLAG_WUPF: Wake Up flag
 *                @arg PMU_FLAG_STDBYF: StandBy flag
 *                @arg PMU_FLAG_PVDOF: PVD output flag
 *                @arg PMU_FLAG_VREFINTF: VREFINT flag
 *
 * @retval  The new state of PMU_FLAG (SET or RESET)
 */
uint8_t Pmu_ReadStatusFlag(Pmu_FlagType flag)
{
    uint8_t bit;

    if ((PMU->CSTS_R.CSTS & (uint32_t)flag) != (uint32_t)RESET)
    {
        bit = SET;
    }
    else
    {
        bit = RESET;
    }

    /** Return the flag status */
    return bit;
}

/*!
 * @brief   Clears the PWR's pending flags
 *
 * @param   flag: specifies the flag to clear
 *                This parameter can be one of the following values:
 *                @arg PMU_FLAG_WUPF: Wake Up flag
 *                @arg PMU_FLAG_STDBYF: StandBy flag
 *
 * @retval  None
 */
void Pmu_ClearStatusFlag(uint8_t flag)
{
    if (flag == (uint8_t)PMU_FLAG_WUPF)
    {
        PMU->CTRL_R.CTRL_B.WUFLGCLR = BIT_SET;
    }
    else if (flag == (uint8_t)PMU_FLAG_STDBYF)
    {
        PMU->CTRL_R.CTRL_B.SBFLGCLR = BIT_SET;
    }
    else
    {
        /* nothing */
    }
}

/**@} end of group PMU_Functions*/
/**@} end of group PMU_Driver */
/**@} end of group G32A10xx_StdPeriphDriver*/
