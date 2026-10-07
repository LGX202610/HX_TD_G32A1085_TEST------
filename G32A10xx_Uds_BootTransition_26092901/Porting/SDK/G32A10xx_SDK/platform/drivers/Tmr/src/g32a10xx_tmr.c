/*!
 * @file        g32a10xx_tmr.c
 *
 * @brief       This file contains all the functions for the TMR peripheral
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

#include "g32a10xx_tmr.h"
#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup TMR_Driver
  @{
*/

/** @defgroup  TMR_Functions Functions
  @{
*/

/*!
 * @brief     Reset the TMR peripheral registers to their default reset values
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    None
 */
void Tmr_Reset(const TMR_T* TMRx)
{
    if (TMRx == TMR1)
    {
        Rcm_EnableApb2PeriphReset(RCM_APB2_PERIPH_TMR1);
        Rcm_DisableApb2PeriphReset(RCM_APB2_PERIPH_TMR1);
    }
    else if (TMRx == TMR2)
    {
        Rcm_EnableApb1PeriphReset(RCM_APB1_PERIPH_TMR2);
        Rcm_DisableApb1PeriphReset(RCM_APB1_PERIPH_TMR2);
    }
    else if (TMRx == TMR3)
    {
        Rcm_EnableApb1PeriphReset(RCM_APB1_PERIPH_TMR3);
        Rcm_DisableApb1PeriphReset(RCM_APB1_PERIPH_TMR3);
    }
    else if (TMRx == TMR4)
    {
        Rcm_EnableApb1PeriphReset(RCM_APB1_PERIPH_TMR4);
        Rcm_DisableApb1PeriphReset(RCM_APB1_PERIPH_TMR4);
    }
    else if (TMRx == TMR6)
    {
        Rcm_EnableApb1PeriphReset(RCM_APB1_PERIPH_TMR6);
        Rcm_DisableApb1PeriphReset(RCM_APB1_PERIPH_TMR6);
    }
    else if (TMRx == TMR7)
    {
        Rcm_EnableApb1PeriphReset(RCM_APB1_PERIPH_TMR7);
        Rcm_DisableApb1PeriphReset(RCM_APB1_PERIPH_TMR7);
    }
    else if (TMRx == TMR8)
    {
        Rcm_EnableApb2PeriphReset(RCM_APB2_PERIPH_TMR8);
        Rcm_DisableApb2PeriphReset(RCM_APB2_PERIPH_TMR8);
    }
    else
    {
        /* nothing */
    }

}

/*!
 * @brief     Initializes the TMRx Time Base Unit peripheral according to
 *            the specified parameters in the Tmr_ConfigTimeBase
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     TimeBaseConfig: pointer to a Tmr_TimeBaseType structure that contains
 *            the configuration information for the specified TMR peripheral
 *
 * @retval    None
 */
void Tmr_ConfigTimeBase(TMR_T* TMRx, const Tmr_TimeBaseType* TimeBaseConfig)
{
    if ((TMRx != TMR6) && (TMRx != TMR7) && (TMRx != TMR8))
    {
        /** Select the Counter Mode */
        TMRx->CTRL1_R.CTRL1_B.CNTDIR = (uint8_t)TimeBaseConfig->counterMode;
        TMRx->CTRL1_R.CTRL1_B.CAMSEL = (uint32_t)((uint32_t)(TimeBaseConfig->counterMode) >> 1u);
        /** Set the clock division */
        TMRx->CTRL1_R.CTRL1_B.CLKDIV = (uint8_t)TimeBaseConfig->clockDivision;
    }
    else
    {
        /* nothing */
    }
    
    /** Set the Autoreload value */
    TMRx->AUTORLD_R.AUTORLD = TimeBaseConfig->period ;

    /** Set the Prescaler value */
    TMRx->PSC_R.PSC = TimeBaseConfig->div ;

    if (TMRx == TMR1) 
    {
        /** Set the Repetition Counter value */
        TMRx->REPCNT_R.REPCNT = TimeBaseConfig->repetitionCounter;
    }
    else
    {
        /* nothing */
    }

    /** Enable Update generation */
    TMRx->CEG_R.CEG_B.UEG = BIT_SET;
}

/*!
 * @brief     Fills each Tmr_ConfigTimeBaseStruct member with its default value
 *
 * @param     TimeBaseConfig: pointer to a Tmr_TimeBaseType structure that contains
 *            the configuration information for the specified TMR peripheral
 *
 * @retval    None
 */
void Tmr_ConfigTimeBaseStruct(Tmr_TimeBaseType* TimeBaseConfig)
{
    TimeBaseConfig->period = 0xFFFFFFFFu;
    TimeBaseConfig->div = 0x0000;
    TimeBaseConfig->clockDivision = TMR_CKD_DIV1;
    TimeBaseConfig->counterMode = TMR_COUNTER_MODE_UP;
    TimeBaseConfig->repetitionCounter = 0x0000;
}

/*!
 * @brief     Configures the TMRx Div
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     div: specifies the Div Register value
 *
 * @param     Mode: specifies the TMR Prescaler Reload Mode
 *
 * @retval    None
 */
void Tmr_ConfigDIV(TMR_T* TMRx, uint16_t Div, Tmr_PrescalerReloadType Mode)
{
    TMRx->PSC_R.PSC = Div;
    TMRx->CEG_R.CEG_B.UEG  =(uint8_t) Mode;
}

/*!
 * @brief      Specifies the TMRx Counter Mode to be used
 *
 * @param      TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param      Mode : specifies the Counter Mode to be used
 *
 * @retval     None
 */
void Tmr_ConfigCounterMode(TMR_T* TMRx, Tmr_CounterModeType Mode)
{
    TMRx->CTRL1_R.CTRL1_B.CNTDIR =(uint8_t) Mode;
    TMRx->CTRL1_R.CTRL1_B.CAMSEL = (uint32_t)((uint32_t)(Mode) >> 1u);
}

/*!
 * @brief     Sets the TMRx Counter Register value
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     counter: specifies the Counter register new value
 *
 * @retval    None
 */
void Tmr_SetCounter(TMR_T* TMRx, uint32_t Counter)
{
    TMRx->CNT_R.CNT = Counter;
}

/*!
 * @brief     Read the TMRx Counter value
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    Counter Register value.
 */
uint32_t Tmr_ReadCounter(const TMR_T* TMRx)
{
    return (uint32_t)TMRx->CNT_R.CNT;
}

/*!
 * @brief     Sets the AutoReload Register value
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     autoReload: autoReload register new value
 *
 * @retval    None
 */
void Tmr_SetAutoReload(TMR_T* TMRx, uint32_t AutoReload)
{
    TMRx->AUTORLD_R.AUTORLD = AutoReload;
}

/*!
 * @brief     Read the TMRx Div value
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    Div Register value.
 */
uint32_t Tmr_ReadDiv(const TMR_T* TMRx)
{
    return (uint32_t)TMRx->PSC_R.PSC;
}

/*!
 * @brief     Enable the No update Event
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    None
 */
void Tmr_EnableNGUpdate(TMR_T* TMRx)
{
    TMRx->CTRL1_R.CTRL1_B.UD = ENABLE;
}

/*!
 * @brief     Enable the No update Event
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    None
 */
void Tmr_DisableNGUpdate(TMR_T* TMRx)
{
    TMRx->CTRL1_R.CTRL1_B.UD = DISABLE;
}

/*!
 * @brief     Configures the Update Request Interrupt Source.
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     Source: Config the Update Source
 *
 * @retval    None
 */
void Tmr_ConfigUPdateRequest(TMR_T* TMRx, Tmr_UpdateSourceType Source)
{
    if (Source != TMR_UPDATE_SOURCE_GLOBAL)
    {
        TMRx->CTRL1_R.CTRL1_B.URSSEL = BIT_SET;
    }
    else
    {
        TMRx->CTRL1_R.CTRL1_B.URSSEL = BIT_RESET;
    }
}

/*!
 * @brief     Enables peripheral Preload register on AUTORLD
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    None
 */
void Tmr_EnableAUTOReload(TMR_T* TMRx)
{
    TMRx->CTRL1_R.CTRL1_B.ARPEN = ENABLE;
}

/*!
 * @brief     Disable peripheral Preload register on AUTORLD
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    None
 */
void Tmr_DisableAUTOReload(TMR_T* TMRx)
{
    TMRx->CTRL1_R.CTRL1_B.ARPEN = DISABLE;
}

/*!
 * @brief     Selects the One Pulse Mode
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     OPMode:Config OP Mode to be used
 *
 * @retval    None
 */
void Tmr_SelectOnePulseMode(TMR_T* TMRx, Tmr_OpmodeType OPMode)
{
    TMRx->CTRL1_R.CTRL1_B.SPMEN = (uint8_t)OPMode;
}

/*!
 * @brief     Sets the Clock Division value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     clockDivision: clock division value
 *
 * @retval    None
 */
void Tmr_SetClockDivision(TMR_T* TMRx, Tmr_CkdType ClockDivision)
{
    TMRx->CTRL1_R.CTRL1_B.CLKDIV = (uint8_t)ClockDivision;
}

/*!
 * @brief     Enable the specified TMR peripheral
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    None
 */
void Tmr_Enable(TMR_T* TMRx)
{
    TMRx->CTRL1_R.CTRL1_B.CNTEN = ENABLE;
}

/*!
 * @brief     Disable the specified TMR peripheral
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @retval    None
 */
void Tmr_Disable(TMR_T* TMRx)
{
    TMRx->CTRL1_R.CTRL1_B.CNTEN = DISABLE;
}

/*!
 * @brief     Configures the: Break feature, dead time, Lock level, the OSSI
 *
 * @param     structure: pointer to a Tmr_BdtInitType structure that contains
 *            the BDT Register configuration  information for the TMR peripheral
 *
 * @retval    None
 */
void Tmr1_ConfigBDT(const Tmr_BdtInitType* Structure)
{
    TMR1->BDT_R.BDT = (uint32_t)(((uint32_t)Structure->automaticOutput) << 14) |
                      (((uint32_t)Structure->breakPolarity) << 13)   |
                      (((uint32_t)Structure->breakState) << 12)      |
                      (((uint32_t)Structure->RMOS_State) << 11)      |
                      (((uint32_t)Structure->IMOS_State) << 10)      |
                      (((uint32_t)Structure->lockLevel)  << 8)       |
                      ((uint32_t)Structure->deadTime);
}

/*!
 * @brief     Initialize the BDT timer with its default value.
 *
 * @param     structure: pointer to a Tmr_BdtInitType structure that contains
 *            the BDT Register configuration  information for the TMR peripheral
 *
 * @retval    None
 */
void Tmr_ConfigBDTStructInit(Tmr_BdtInitType* Structure)
{
    Structure->RMOS_State = TMR_RMOS_STATE_DISABLE;
    Structure->IMOS_State = TMR_IMOS_STATE_DISABLE;
    Structure->lockLevel = TMR_LOCK_LEVEL_OFF;
    Structure->deadTime = 0x00;
    Structure->breakState = TMR_BREAK_STATE_DISABLE;
    Structure->breakPolarity = TMR_BREAK_POLARITY_LOW;
    Structure->automaticOutput = TMR_AUTOMATIC_OUTPUT_DISABLE;
}

/*!
 * @brief     Enable TMRx PWM output.
 *
 * @retval    None
 */
void Tmr1_EnablePWMOutputs(void)
{
    TMR1->BDT_R.BDT_B.MOEN = ENABLE;
}

/*!
 * @brief     Disable TMRx PWM output.
 *
 * @retval    None
 */
void Tmr1_DisablePWMOutputs(void)
{
    TMR1->BDT_R.BDT_B.MOEN = DISABLE;
}

/*!
 * @brief     Configure Channel 1 according to parameters
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4 to select Timer
 *
 * @param     OCcongigStruct: Channel configuration structure
 *
 * @retval    None
 */
void Tmr_OC1Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct)
{

    /** Disable the Channel 1: Reset the CC1EN Bit */
    TMRx->CCEN_R.CCEN_B.CC1EN = BIT_RESET;

    /** Reset and Select the Output Compare Mode Bits */
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.CC1SEL = BIT_RESET;
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC1MOD = (uint8_t)OCcongigStruct->OC_Mode;

    /** Reset and Set the Output Polarity level */
    TMRx->CCEN_R.CCEN_B.CC1POL = (uint8_t)OCcongigStruct->OC_Polarity;

    /** Set the Output State */
    TMRx->CCEN_R.CCEN_B.CC1EN = (uint8_t)OCcongigStruct->OC_OutputState;

    if ((TMRx == TMR1) || (TMRx == TMR2) || (TMRx == TMR3)
            || (TMRx == TMR4))
    {
        /** Reset and Set the Output N Polarity level */
        TMRx->CCEN_R.CCEN_B.CC1NPOL = (uint8_t)OCcongigStruct->OC_NPolarity;

        /** Reset and Set the Output N State */
        TMRx->CCEN_R.CCEN_B.CC1NEN = (uint8_t)OCcongigStruct->OC_OutputNState;

        /** Reset the Output Compare and Output Compare N IDLE State */
        TMRx->CTRL2_R.CTRL2_B.OC1OIS = BIT_RESET;
        TMRx->CTRL2_R.CTRL2_B.OC1NOIS = BIT_RESET;

        /** Set the Output Idle state */
        TMRx->CTRL2_R.CTRL2_B.OC1OIS = (uint8_t)OCcongigStruct->OC_Idlestate;
        /** Set the Output N State */
        TMRx->CTRL2_R.CTRL2_B.OC1NOIS = (uint8_t)OCcongigStruct->OC_NIdlestate;
    }
    else
    {
        /* nothing */
    }

    /** Set the Capture Compare Register value */
    TMRx->CC1_R.CC1 = OCcongigStruct->Pulse;
}

/*!
 * @brief     Configure Channel 2 according to parameters
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCcongigStruct: Channel configuration structure
 *
 * @retval    None
 */
void Tmr_OC2Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct)
{

    /** Disable the Channel 2: Reset the CC2EN Bit */
    TMRx->CCEN_R.CCEN_B.CC2EN = BIT_RESET;

    /** Reset and Select the Output Compare Mode Bits */
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.CC2SEL = BIT_RESET;
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC2MOD = (uint8_t)OCcongigStruct->OC_Mode;

    /** Reset and Set the Output Polarity level */
    TMRx->CCEN_R.CCEN_B.CC2POL = BIT_RESET;
    TMRx->CCEN_R.CCEN_B.CC2POL = (uint8_t)OCcongigStruct->OC_Polarity;

    /** Set the Output State */
    TMRx->CCEN_R.CCEN_B.CC2EN = (uint8_t)OCcongigStruct->OC_OutputState;

    if (TMRx == TMR1)
    {
        /** Reset and Set the Output N Polarity level */
        TMRx->CCEN_R.CCEN_B.CC2NPOL = BIT_RESET;
        TMRx->CCEN_R.CCEN_B.CC2NPOL = (uint8_t)OCcongigStruct->OC_NPolarity;

        /** Reset and Set the Output N State */
        TMRx->CCEN_R.CCEN_B.CC2NEN = BIT_RESET;
        TMRx->CCEN_R.CCEN_B.CC2NEN = (uint8_t)OCcongigStruct->OC_OutputNState;

        /** Reset the Output Compare and Output Compare N IDLE State */
        TMRx->CTRL2_R.CTRL2_B.OC2OIS = BIT_RESET;
        TMRx->CTRL2_R.CTRL2_B.OC2NOIS = BIT_RESET;

        /** Set the Output Idle state */
        TMRx->CTRL2_R.CTRL2_B.OC2OIS = (uint8_t)OCcongigStruct->OC_Idlestate;
        /** Set the Output N State */
        TMRx->CTRL2_R.CTRL2_B.OC2NOIS = (uint8_t)OCcongigStruct->OC_NIdlestate;
    }
    else
    {
        /* nothing */
    }
    
    /** Set the Capture Compare Register value */
    TMRx->CC2_R.CC2 = OCcongigStruct->Pulse;
}

/*!
 * @brief     Configure Channel 3 according to parameters
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCcongigStruct: Channel configuration structure
 *
 * @retval    None
 *
 * @note
 */
void Tmr_OC3Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct)
{

    /** Disable the Channel 3: Reset the CC3EN Bit */
    TMRx->CCEN_R.CCEN_B.CC3EN = BIT_RESET;

    /** Reset and Select the Output Compare Mode Bits */
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.CC3SEL = BIT_RESET;
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC3MOD = (uint8_t)OCcongigStruct->OC_Mode;

    /** Reset and Set the Output Polarity level */
    TMRx->CCEN_R.CCEN_B.CC3POL = BIT_RESET;
    TMRx->CCEN_R.CCEN_B.CC3POL = (uint8_t)OCcongigStruct->OC_Polarity;

    /** Set the Output State */
    TMRx->CCEN_R.CCEN_B.CC3EN = (uint8_t)OCcongigStruct->OC_OutputState;

    if (TMRx == TMR1)
    {
        /** Reset and Set the Output N Polarity level */
        TMRx->CCEN_R.CCEN_B.CC3NPOL = BIT_RESET;
        TMRx->CCEN_R.CCEN_B.CC3NPOL = (uint8_t)OCcongigStruct->OC_NPolarity;

        /** Reset and Set the Output N State */
        TMRx->CCEN_R.CCEN_B.CC3NEN = BIT_RESET;
        TMRx->CCEN_R.CCEN_B.CC3NEN = (uint8_t)OCcongigStruct->OC_OutputNState;

        /** Reset the Output Compare and Output Compare N IDLE State */
        TMRx->CTRL2_R.CTRL2_B.OC3OIS = BIT_RESET;
        TMRx->CTRL2_R.CTRL2_B.OC3NOIS = BIT_RESET;

        /** Set the Output Idle state */
        TMRx->CTRL2_R.CTRL2_B.OC3OIS = (uint8_t)OCcongigStruct->OC_Idlestate;
        /** Set the Output N State */
        TMRx->CTRL2_R.CTRL2_B.OC3NOIS = (uint8_t)OCcongigStruct->OC_NIdlestate;
    }
    else
    {
        /* nothing */
    }

    /** Set the Capture Compare Register value */
    TMRx->CC3_R.CC3 = OCcongigStruct->Pulse;
}

/*!
 * @brief     Configure Channel 4 according to parameters
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCcongigStruct: Channel configuration structure
 *
 * @retval    None
 */
void Tmr_OC4Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct)
{

    /** Disable the Channel 4: Reset the CC4EN Bit */
    TMRx->CCEN_R.CCEN_B.CC4EN = BIT_RESET;

    /** Reset and Select the Output Compare Mode Bits */
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.CC4SEL = BIT_RESET;
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC4MOD = (uint8_t)OCcongigStruct->OC_Mode;

    /** Reset and Set the Output Polarity level */
    TMRx->CCEN_R.CCEN_B.CC4POL = BIT_RESET;
    TMRx->CCEN_R.CCEN_B.CC4POL = (uint8_t)OCcongigStruct->OC_Polarity;

    /** Set the Output State */
    TMRx->CCEN_R.CCEN_B.CC4EN = (uint8_t)OCcongigStruct->OC_OutputState;

    if (TMRx == TMR1)
    {
        /** Reset the Output Compare and Output Compare IDLE State */
        TMRx->CTRL2_R.CTRL2_B.OC4OIS = BIT_RESET;

        /** Set the Output Idle state */
        TMRx->CTRL2_R.CTRL2_B.OC4OIS = (uint8_t)OCcongigStruct->OC_Idlestate;
    }
    else
    {
        /* nothing */
    }

    /** Set the Capture Compare Register value */
    TMRx->CC4_R.CC4 = OCcongigStruct->Pulse;
}

/*!
 * @brief     Initialize the OC timer with its default value.
 *
 * @param     OCcongigStruct: Channel configuration structure
 *
 * @retval    None
 */
void Tmr_OCConfigStructInit(Tmr_OcConfigType* OCcongigStruct)
{
    /** Set the default configuration */
    OCcongigStruct->OC_Mode = TMR_OC_MODE_TMRING;
    OCcongigStruct->OC_OutputState = TMR_OUTPUT_STATE_DISABLE;
    OCcongigStruct->OC_OutputNState = TMR_OUTPUT_NSTATE_DISABLE;
    OCcongigStruct->Pulse = 0x0000;
    OCcongigStruct->OC_Polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct->OC_NPolarity = TMR_OC_NPOLARITY_HIGH;
    OCcongigStruct->OC_Idlestate = TMR_OCIDLESTATE_RESET;
    OCcongigStruct->OC_NIdlestate = TMR_OCNIDLESTATE_RESET;
}

/*!
 * @brief     Selects the Output Compare Mode.
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Channel: specifies the TMR Channel
 *                    This parameter can be one of the following values:
 *                     @arg TMR_CHANNEL_1
 *                     @arg TMR_CHANNEL_2
 *                     @arg TMR_CHANNEL_3
 *                     @arg TMR_CHANNEL_4
 *
 * @param     OCMode: specifies the TMR Output Compare Mode
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_MODE_TMRING
 *                     @arg TMR_OC_MODE_ACTIVE
 *                     @arg TMR_OC_MODE_INACTIVE
 *                     @arg TMR_OC_MODE_LOWLEVEL
  *                    @arg TMR_OC_MODE_HIGHLEVEL
 *                     @arg TMR_OC_MODE_PWM1
 *                     @arg TMR_OC_MODE_PWM2

 * @retval    None
 */
void Tmr_SelectOCxMode(TMR_T* TMRx, Tmr_ChannelType Channel, Tmr_OcModeType Mode)
{
    TMRx->CCEN_R.CCEN &=  (uint32_t)((uint32_t)BIT_RESET << (uint32_t)Channel);

    if (Channel == TMR_CHANNEL_1)
    {
        TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC1MOD = (uint8_t)Mode;
    }
    else if (Channel == TMR_CHANNEL_2)
    {
        TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC2MOD = (uint8_t)Mode;
    }
    else if (Channel == TMR_CHANNEL_3)
    {
        TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC3MOD = (uint8_t)Mode;
    }
    else if (Channel == TMR_CHANNEL_4)
    {
        TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC4MOD = (uint8_t)Mode;
    }
    else
    {
        /* nothing */
    }
}

/*!
 * @brief     Sets the Capture Compare1 Register value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Compare: specifies the Capture Compare1 register new value
 *
 * @retval    None
 */
void Tmr_SetCompare1(TMR_T* TMRx, uint32_t Compare)
{
    TMRx->CC1_R.CC1 = Compare;
}

/*!
 * @brief     Sets the Capture Compare2 Register value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Compare: specifies the Capture Compare1 register new value
 *
 * @retval    None
 */
void Tmr_SetCompare2(TMR_T* TMRx, uint32_t Compare)
{
    TMRx->CC2_R.CC2 = Compare;
}

/*!
 * @brief     Sets the Capture Compare3 Register value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Compare: specifies the Capture Compare1 register new value
 *
 * @retval    None
 */
void Tmr_SetCompare3(TMR_T* TMRx, uint32_t Compare)
{
    TMRx->CC3_R.CC3 = Compare;
}

/*!
 * @brief     Sets the Capture Compare4 Register value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Compare: specifies the Capture Compare1 register new value
 *
 * @retval    None
 */
void Tmr_SetCompare4(TMR_T* TMRx, uint32_t Compare)
{
    TMRx->CC4_R.CC4 = Compare;
}

/*!
 * @brief     Forces the output 1 waveform to active or inactive level
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Action: forced Action to be set to the output waveform
 *                  This parameter can be one of the following values:
 *                     @arg TMR_FORCEDACTION_INACTIVE
 *                     @arg TMR_FORCEDACTION_ACTIVE
 * @retval    None
 */
void Tmr_ForcedOC1Config(TMR_T* TMRx, Tmr_ForcedActionType Action)
{
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC1MOD = (uint8_t)Action;
}

/*!
 * @brief     Forces the output 2 waveform to active or inactive level
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Action: forced Action to be set to the output waveform
 *                  This parameter can be one of the following values:
 *                     @arg TMR_FORCEDACTION_INACTIVE
 *                     @arg TMR_FORCEDACTION_ACTIVE
 * @retval    None
 */
void Tmr_ForcedOC2Config(TMR_T* TMRx, Tmr_ForcedActionType Action)
{
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC2MOD = (uint8_t)Action;
}

/*!
 * @brief     Forces the output 3 waveform to active or inactive level
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Action: forced Action to be set to the output waveform
 *                  This parameter can be one of the following values:
 *                     @arg TMR_FORCEDACTION_INACTIVE
 *                     @arg TMR_FORCEDACTION_ACTIVE
 * @retval    None
 */
void Tmr_ForcedOC3Config(TMR_T* TMRx, Tmr_ForcedActionType Action)
{
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC3MOD = (uint8_t)Action;
}

/*!
 * @brief     Forces the output 4 waveform to active or inactive level
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Action: forced Action to be set to the output waveform
 *                  This parameter can be one of the following values:
 *                     @arg TMR_FORCEDACTION_INACTIVE
 *                     @arg TMR_FORCEDACTION_ACTIVE
 *
 * @retval    None
 */
void Tmr_ForcedOC4Config(TMR_T* TMRx, Tmr_ForcedActionType Action)
{
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC4MOD = (uint8_t)Action;
}

/*!
 * @brief     Sets Capture Compare Preload Control bit
 *
 * @retval    None
 */
void Tmr1_EnableCCPreload(void)
{
    TMR1->CTRL2_R.CTRL2_B.CCPEN = ENABLE;
}

/*!
 * @brief     Resets Capture Compare Preload Control bit
 *
 * @retval    None
 */
void Tmr1_DisableCCPreload(void)
{
    TMR1->CTRL2_R.CTRL2_B.CCPEN = DISABLE;
}

/*!
 * @brief     Enables or disables the peripheral Preload register on CC1
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPreload: new state of the TMRx peripheral Preload register
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OC_PRELOAD_DISABLE
 *                     @arg TMR_OC_PRELOAD_ENABLE
 * @retval    None
 */
void Tmr_OC1PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload)
{
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC1PEN = (uint8_t)OCPreload;
}

/*!
 * @brief     Enables or disables the peripheral Preload register on CC2
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPreload: new state of the TMRx peripheral Preload register
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OC_PRELOAD_DISABLE
 *                     @arg TMR_OC_PRELOAD_ENABLE
 * @retval    None
 */
void Tmr_OC2PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload)
{
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC2PEN = (uint8_t)OCPreload;
}

/*!
 * @brief     Enables or disables the peripheral Preload register on CC3
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPreload: new state of the TMRx peripheral Preload register
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OC_PRELOAD_DISABLE
 *                     @arg TMR_OC_PRELOAD_ENABLE
 * @retval    None
 */
void Tmr_OC3PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload)
{
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC3PEN = (uint8_t)OCPreload;
}

/*!
 * @brief     Enables or disables the peripheral Preload register on CC4
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPreload: new state of the TMRx peripheral Preload register
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OC_PRELOAD_DISABLE
 *                     @arg TMR_OC_PRELOAD_ENABLE
 * @retval    None
 */
void Tmr_OC4PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload)
{
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC4PEN = (uint8_t)OCPreload;
}

/*!
 * @brief     Configures the Output Compare 1 Fast feature
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCFast: new state of the Output Compare Fast Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCFAST_DISABLE
 *                     @arg TMR_OCFAST_ENABLE
 * @retval    None
 */

void Tmr_OC1FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast)
{
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC1FEN = (uint8_t)OCFast;
}

/*!
 * @brief     Configures the Output Compare 2 Fast feature
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCFast: new state of the Output Compare Fast Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCFAST_DISABLE
 *                     @arg TMR_OCFAST_ENABLE
 * @retval    None
 */
void Tmr_OC2FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast)
{
    TMRx->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC2FEN = (uint8_t)OCFast;
}

/*!
 * @brief     Configures the Output Compare 3 Fast feature
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCFast: new state of the Output Compare Fast Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCFAST_DISABLE
 *                     @arg TMR_OCFAST_ENABLE
 * @retval    None
 */
void Tmr_OC3FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast)
{
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC3FEN = (uint8_t)OCFast;
}

/*!
 * @brief     Configures the Output Compare 4 Fast feature
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCFast: new state of the Output Compare Fast Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCFAST_DISABLE
 *                     @arg TMR_OCFAST_ENABLE
 * @retval    None
 */
void Tmr_OC4FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast)
{
    TMRx->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC4FEN = (uint8_t)OCFast;
}

/*!
 * @brief     Clears or safeguards the OCREF1 signal on an external Event
 *
 * @param     OCCler: new state of the Output Compare Clear Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCCLER_DISABLE
 *                     @arg TMR_OCCLER_ENABLE
 * @retval    None
 */
void Tmr1_ClearOC1Ref(Tmr_OcclerType OCCler)
{
    TMR1->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC1CEN = (uint8_t)OCCler;
}

/*!
 * @brief     Clears or safeguards the OCREF2 signal on an external Event
 *
 * @param     OCCler: new state of the Output Compare Clear Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCCLER_DISABLE
 *                     @arg TMR_OCCLER_ENABLE
 * @retval    None
 */
void Tmr1_ClearOC2Ref(Tmr_OcclerType OCCler)
{
    TMR1->CCM1R.CCM1_OUTPUT_R.CCM1_OUTPUT_B.OC2CEN = (uint8_t)OCCler;
}

/*!
 * @brief     Clears or safeguards the OCREF3 signal on an external Event
 *
 * @param     OCCler: new state of the Output Compare Clear Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCCLER_DISABLE
 *                     @arg TMR_OCCLER_ENABLE
 * @retval    None
 */
void Tmr1_ClearOC3Ref(Tmr_OcclerType OCCler)
{
    TMR1->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC3CEN = (uint8_t)OCCler;
}

/*!
 * @brief     Clears or safeguards the OCREF4 signal on an external Event
 *
 * @param     OCCler: new state of the Output Compare Clear Enable Bit
 *                  This parameter can be one of the following values:
 *                     @arg TMR_OCCLER_DISABLE
 *                     @arg TMR_OCCLER_ENABLE
 * @retval    None
 */
void Tmr1_ClearOC4Ref(Tmr_OcclerType OCCler)
{
    TMR1->CCM2R.CCM2_OUTPUT_R.CCM2_OUTPUT_B.OC4CEN = (uint8_t)OCCler;
}

/*!
 * @brief     Configures the  Channel 1 Polarity
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPolarity: specifies the OC1 Polarity
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_POLARITY_HIGH
 *                     @arg TMR_OC_POLARITY_LOW
 * @retval    None
 */
void Tmr_OC1PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity)
{
    TMRx->CCEN_R.CCEN_B.CC1POL = (uint8_t)OCPolarity;
}

/*!
 * @brief     Configures the  Channel 1N Polarity
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCNPolarity: specifies the OC1 NPolarity
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_NPOLARITY_HIGH
 *                     @arg TMR_OC_NPOLARITY_LOW
 * @retval    None
 */
void Tmr_OC1NPolarityConfig(TMR_T* TMRx, Tmr_OcNpolarityType OCNPolarity)
{
    TMRx->CCEN_R.CCEN_B.CC1NPOL = (uint8_t)OCNPolarity;
}

/*!
 * @brief     Configures the  Channel 2 Polarity
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPolarity: specifies the OC2 Polarity
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_POLARITY_HIGH
 *                     @arg TMR_OC_POLARITY_LOW
 * @retval    None
 */
void Tmr_OC2PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity)
{
    TMRx->CCEN_R.CCEN_B.CC2POL = (uint8_t)OCPolarity;
}

/*!
 * @brief     Configures the  Channel 2N Polarity
 *
 * @param     TMRx: where x can be 1 to select the TMR peripheral
 *
 * @param     OCNPolarity: specifies the OC2 NPolarity
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_NPOLARITY_HIGH
 *                     @arg TMR_OC_NPOLARITY_LOW
 * @retval    None
 */
void Tmr_OC2NPolarityConfig(TMR_T* TMRx, Tmr_OcNpolarityType OCNPolarity)
{
    TMRx->CCEN_R.CCEN_B.CC2NPOL = (uint8_t)OCNPolarity;
}

/*!
 * @brief     Configures the  Channel 3 Polarity
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPolarity: specifies the OC3 Polarity
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_POLARITY_HIGH
 *                     @arg TMR_OC_POLARITY_LOW
 * @retval    None
 */
void Tmr_OC3PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity)
{
    TMRx->CCEN_R.CCEN_B.CC3POL = (uint8_t)OCPolarity;
}

/*!
 * @brief     Configures the  Channel 3N Polarity
 *
 * @param     TMRx: where x can be 1 to select the TMR peripheral
 *
 * @param     OCNPolarity: specifies the OC3 NPolarity
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_NPOLARITY_HIGH
 *                     @arg TMR_OC_NPOLARITY_LOW
 * @retval    None
 */
void Tmr_OC3NPolarityConfig(TMR_T* TMRx, Tmr_OcNpolarityType OCNPolarity)
{
    TMRx->CCEN_R.CCEN_B.CC3NPOL = (uint8_t)OCNPolarity;
}

/*!
 * @brief     Configures the  Channel 4 Polarity
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     OCPolarity: specifies the OC4 Polarity
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OC_POLARITY_HIGH
 *                     @arg TMR_OC_POLARITY_LOW
 * @retval    None
 */
void Tmr_OC4PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity)
{
    TMRx->CCEN_R.CCEN_B.CC4POL = (uint8_t)OCPolarity;
}

/*!
 * @brief     Selects the OCReference Clear Source
 *
 * @param     OCReferenceClear: specifies the OCReference Clear Source
 *                    This parameter can be one of the following values:
 *                     @arg TMR_OCCS_ETRF
 *                     @arg TMR_OCCS_OCREFCLR
 * @retval    None
 */
void Tmr1_SelectOCREFClear(Tmr_OccselType OCReferenceClear)
{
    TMR1->SMCTRL_R.SMCTRL_B.OCCSEL = (uint8_t)OCReferenceClear;
}

/*!
 * @brief     Enables the Capture Compare Channel x
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Channel: TMR Channel
 *
 * @retval    None
 */
void Tmr_EnableCCxChannel(TMR_T* TMRx, Tmr_ChannelType Channel)
{
    TMRx->CCEN_R.CCEN |= (uint32_t)((uint32_t)BIT_SET << (uint32_t)Channel);
}

/*!
 * @brief     Disables the Capture Compare Channel x
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Channel: TMR Channel
 *
 * @retval    None
 */
void Tmr_DisableCCxChannel(TMR_T* TMRx, Tmr_ChannelType Channel)
{
    TMRx->CCEN_R.CCEN &= ~((uint32_t)((uint32_t)BIT_SET << (uint32_t)Channel));
}

/*!
 * @brief     Enables the Capture Compare Channel xN.
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Channel: TMR Channel
 *
 * @retval    None
 */
void Tmr_EnableCCxNChannel(TMR_T* TMRx, Tmr_ChannelType Channel)
{
    TMRx->CCEN_R.CCEN |= (uint32_t)0x04u << (uint32_t)Channel;
}

/*!
 * @brief     Disables the Capture Compare Channel xN
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Channel: TMR Channel
 *
 * @retval    None
 */
void Tmr_DisableCCxNChannel(TMR_T* TMRx, Tmr_ChannelType Channel)
{
    TMRx->CCEN_R.CCEN &= ~((uint32_t)0x04u << (uint32_t)Channel);
}

/*!
 * @brief     Enable Selects the TMR peripheral Commutation Event
 *
 * @retval    None
 */

void Tmr1_EnableSelectCOM(void)
{
    TMR1->CTRL2_R.CTRL2_B.CCUSEL = ENABLE;
}
/*!
 * @brief     Disable Selects the TMR peripheral Commutation Event
 *
 * @retval    None
 */
void Tmr1_DisableSelectCOM(void)
{
    TMR1->CTRL2_R.CTRL2_B.CCUSEL = DISABLE;
}

/*!
 * @brief     Configure the TI1 as Input.
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     ICpolarity: The Input Polarity.
 *
 * @param     ICselection: specifies the Input to be used.
 *
 * @param     ICfilter: Specifies the Input Capture Filter
 *
 * @retval    None
 */
static void TI1Config(TMR_T* TMRx, uint16_t ICpolarity, uint16_t ICselection, uint16_t ICfilter)
{
    uint16_t tmpchctrl = 0;

    /** Disable the Channel 1: Reset the CC1EN Bit */
    TMRx->CCEN_R.CCEN_B.CC1EN = BIT_RESET;

    /** Select the Input and set the Filter */
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC1SEL = BIT_RESET;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC1F = BIT_RESET;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC1SEL = ICselection;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC1F = ICfilter;

    /** Select the Polarity */
    tmpchctrl = (uint16_t)TMRx->CCEN_R.CCEN;
    tmpchctrl &= (uint16_t)~((uint16_t)TMR_IC_POLARITY_BOTHEDGE);
    tmpchctrl |= ICpolarity;
    TMRx->CCEN_R.CCEN = tmpchctrl;

    /** Set the CC1EN Bit */
    TMRx->CCEN_R.CCEN_B.CC1EN = BIT_SET;
}

/*!
 * @brief     Configure the TI2 as Input
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     ICpolarity: The Input Polarity.
 *
 * @param     ICselection: specifies the Input to be used.
 *
 * @param     ICfilter: Specifies the Input Capture Filter
 *
 * @retval    None
 */
static void TI2Config(TMR_T* TMRx, uint16_t ICpolarity, uint16_t ICselection, uint16_t ICfilter)
{
    uint16_t tmpchctrl = 0;

    /** Disable the Channel 2: Reset the CC2EN Bit */
    TMRx->CCEN_R.CCEN_B.CC2EN = BIT_RESET;

    /** Select the Input and set the Filter */
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC2SEL = BIT_RESET;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC2F = BIT_RESET;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC2SEL = ICselection;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC2F = ICfilter;

    /** Select the Polarity */
    tmpchctrl = (uint16_t)TMRx->CCEN_R.CCEN;
    tmpchctrl &= (uint16_t)~((uint16_t)TMR_IC_POLARITY_BOTHEDGE << 4);
    tmpchctrl |= (uint16_t)(ICpolarity << 4);
    TMRx->CCEN_R.CCEN = tmpchctrl;

    /** Set the CC2EN Bit */
    TMRx->CCEN_R.CCEN_B.CC2EN = BIT_SET;
}

/*!
 * @brief     Configure the TI3 as Input.
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     ICpolarity: The Input Polarity.
 *
 * @param     ICselection: specifies the Input to be used.
 *
 * @param     ICfilter: Specifies the Input Capture Filter
 *
 * @retval    None
 */
static void TI3Config(TMR_T* TMRx, uint16_t ICpolarity, uint16_t ICselection, uint16_t ICfilter)
{
    uint16_t tmpchctrl = 0;

    /** Disable the Channel 3: Reset the CC3EN Bit */
    TMRx->CCEN_R.CCEN_B.CC3EN = BIT_RESET;

    /** Select the Input and set the Filter */
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.CC3SEL = BIT_RESET;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC3F = BIT_RESET;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.CC3SEL = ICselection;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC3F = ICfilter;

    /** Select the Polarity */
    tmpchctrl = (uint16_t)TMRx->CCEN_R.CCEN;
    tmpchctrl &= (uint16_t)~((uint16_t)TMR_IC_POLARITY_BOTHEDGE << 8);
    tmpchctrl |= (uint16_t)(ICpolarity << 8);
    TMRx->CCEN_R.CCEN = tmpchctrl;

    /** Set the CC3EN Bit */
    TMRx->CCEN_R.CCEN_B.CC3EN = BIT_SET;
}

/*!
 * @brief     Configure the TI4 as Input
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     ICpolarity: The Input Polarity.
 *
 * @param     ICselection: specifies the Input to be used.
 *
 * @param     ICfilter: Specifies the Input Capture Filter
 *
 * @retval    None
 */
static void TI4Config(TMR_T* TMRx, uint16_t ICpolarity, uint16_t ICselection, uint16_t ICfilter)
{
    uint16_t tmpchctrl = 0;

    /** Disable the Channel 4: Reset the CC4EN Bit */
    TMRx->CCEN_R.CCEN_B.CC4EN = BIT_RESET;

    /** Select the Input and set the Filter */
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.CC4SEL = BIT_RESET;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC4F = BIT_RESET;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.CC4SEL = ICselection;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC4F = ICfilter;

    /** Select the Polarity */
    tmpchctrl = (uint16_t)TMRx->CCEN_R.CCEN;
    tmpchctrl &= (uint16_t)~((uint16_t)TMR_IC_POLARITY_BOTHEDGE << 12);
    tmpchctrl |= (uint16_t)(ICpolarity << 12);
    TMRx->CCEN_R.CCEN = tmpchctrl;

    /** Set the CC4EN Bit */
    TMRx->CCEN_R.CCEN_B.CC4EN = BIT_SET;
}

/*!
 * @brief     Configure Peripheral equipment
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     ICconfigstruct: pointer to a Tmr_IcConfigType structure
 *
 * @retval    None
 */
void Tmr_ICConfig(TMR_T* TMRx, const Tmr_IcConfigType* ICconfigstruct)
{
    if (ICconfigstruct->channel == TMR_CHANNEL_1)
    {
        /** TI1 Configuration */
        TI1Config(TMRx, (uint16_t)ICconfigstruct->ICpolarity, (uint16_t)ICconfigstruct->ICselection, (uint16_t)ICconfigstruct->ICfilter);
        Tmr_SetIC1Prescal(TMRx, ICconfigstruct->ICprescaler);
    }
    else if (ICconfigstruct->channel == TMR_CHANNEL_2)
    {
        /** TI2 Configuration */
        TI2Config(TMRx, (uint16_t)ICconfigstruct->ICpolarity, (uint16_t)ICconfigstruct->ICselection, (uint16_t)ICconfigstruct->ICfilter);
        Tmr_SetIC2Prescal(TMRx, ICconfigstruct->ICprescaler);
    }
    else if (ICconfigstruct->channel == TMR_CHANNEL_3)
    {
        /** TI3 Configuration */
        TI3Config(TMRx, (uint16_t)ICconfigstruct->ICpolarity, (uint16_t)ICconfigstruct->ICselection, (uint16_t)ICconfigstruct->ICfilter);
        Tmr_SetIC3Prescal(TMRx, ICconfigstruct->ICprescaler);
    }
    else if (ICconfigstruct->channel == TMR_CHANNEL_4)
    {
        /** TI4 Configuration */
        TI4Config(TMRx, (uint16_t)ICconfigstruct->ICpolarity, (uint16_t)ICconfigstruct->ICselection, (uint16_t)ICconfigstruct->ICfilter);
        Tmr_SetIC4Prescal(TMRx, ICconfigstruct->ICprescaler);
    }
    else
    {
        /* nothing */
    }
}

/*!
 * @brief     Initialize the IC timer with its default value.
 *
 * @param     ICconfigstruct: pointer to a Tmr_IcConfigType structure
 *
 * @retval    None
 */
void Tmr_ICConfigStructInit(Tmr_IcConfigType* ICconfigstruct)
{
    ICconfigstruct->channel = TMR_CHANNEL_1;
    ICconfigstruct->ICpolarity = TMR_IC_POLARITY_RISING;
    ICconfigstruct->ICselection = TMR_IC_SELECTION_DIRECT_TI;
    ICconfigstruct->ICprescaler = TMR_ICPSC_DIV1;
    ICconfigstruct->ICfilter = 0x00;
}

/*!
 * @brief     Config of PWM output
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     ICconfigstruct: pointer to a Tmr_IcConfigType structure
 *
 * @retval    None
 */
void Tmr_PWMConfig(TMR_T* TMRx, const Tmr_IcConfigType* ICconfigstruct)
{
    uint16_t icpolarity = (uint16_t)TMR_IC_POLARITY_RISING;
    uint16_t icselection = (uint16_t)TMR_IC_SELECTION_DIRECT_TI;

    /** Select the Opposite Input Polarity */
    if (ICconfigstruct->ICpolarity == TMR_IC_POLARITY_RISING)
    {
        icpolarity = (uint16_t)TMR_IC_POLARITY_FALLING;
    }
    else
    {
        icpolarity = (uint16_t)TMR_IC_POLARITY_RISING;
    }

    /** Select the Opposite Input */
    if (ICconfigstruct->ICselection == TMR_IC_SELECTION_DIRECT_TI)
    {
        icselection = (uint16_t)TMR_IC_SELECTION_INDIRECT_TI;
    }
    else
    {
        icselection = (uint16_t)TMR_IC_SELECTION_DIRECT_TI;
    }

    if (ICconfigstruct->channel == TMR_CHANNEL_1)
    {
        /** TI1 Configuration */
        TI1Config(TMRx, (uint16_t)ICconfigstruct->ICpolarity, (uint16_t)ICconfigstruct->ICselection, (uint16_t)ICconfigstruct->ICfilter);
        /** Set the Input Capture Prescaler value */
        Tmr_SetIC1Prescal(TMRx, ICconfigstruct->ICprescaler);
        /** TI2 Configuration */
        TI2Config(TMRx, icpolarity, icselection, ICconfigstruct->ICfilter);
        /** Set the Input Capture Prescaler value */
        Tmr_SetIC2Prescal(TMRx, ICconfigstruct->ICprescaler);
    }
    else
    {
        /** TI2 Configuration */
        TI2Config(TMRx, (uint16_t)ICconfigstruct->ICpolarity, (uint16_t)ICconfigstruct->ICselection, (uint16_t)ICconfigstruct->ICfilter);
        /** Set the Input Capture Prescaler value */
        Tmr_SetIC2Prescal(TMRx, ICconfigstruct->ICprescaler);
        /** TI1 Configuration */
        TI1Config(TMRx, icpolarity, icselection, ICconfigstruct->ICfilter);
        /** Set the Input Capture Prescaler value */
        Tmr_SetIC1Prescal(TMRx, ICconfigstruct->ICprescaler);
    }
}

/*!
 * @brief     Read Input Capture 1 value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @retval    Capture Compare 1 Register value
 */
uint16_t Tmr_ReadCaputer1(const TMR_T* TMRx)
{
    return (uint16_t)TMRx->CC1_R.CC1;
}

/*!
 * @brief     Read Input Capture 2 value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @retval    Capture Compare 2 Register value
 */
uint16_t Tmr_ReadCaputer2(const TMR_T* TMRx)
{
    return (uint16_t)TMRx->CC2_R.CC2;
}

/*!
 * @brief     Read Input Capture 3 value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @retval    Capture Compare 3 Register value
 */
uint16_t Tmr_ReadCaputer3(const TMR_T* TMRx)
{
    return (uint16_t)TMRx->CC3_R.CC3;
}

/*!
 * @brief     Read Input Capture 4 value
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @retval    Capture Compare 4 Register value
 */
uint16_t Tmr_ReadCaputer4(const TMR_T* TMRx)
{
    return (uint16_t)TMRx->CC4_R.CC4;
}

/*!
 * @brief     Sets the TMRx Input Capture 1 Prescaler
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Prescaler: specifies the Input Capture 1 Prescaler new value
 *
 * @retval    None
 */
void Tmr_SetIC1Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler)
{
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC1PSC = BIT_RESET;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC1PSC = (uint8_t)Prescaler;
}
/*!
 * @brief     Sets the TMRx Input Capture 2 Prescaler
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Prescaler: specifies the Input Capture 2 Prescaler new value
 *
 * @retval    None
 */
void Tmr_SetIC2Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler)
{
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC2PSC = BIT_RESET;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.IC2PSC = (uint8_t)Prescaler;
}

/*!
 * @brief     Sets the TMRx Input Capture 3 Prescaler
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Prescaler: specifies the Input Capture 3 Prescaler new value
 *
 * @retval    None
 */
void Tmr_SetIC3Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler)
{
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC3PSC = BIT_RESET;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC3PSC = (uint8_t)Prescaler;
}

/*!
 * @brief     Sets the TMRx Input Capture 4 Prescaler
 *
 * @param     TMRx: x can be can be 1, 2, 3 and 4 to select Timer
 *
 * @param     Prescaler: specifies the Input Capture 4 Prescaler new value
 *
 * @retval    None
 */
void Tmr_SetIC4Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler)
{
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC4PSC = BIT_RESET;
    TMRx->CCM2R.CCM2_INPUT_R.CCM2_INPUT_B.IC4PSC = (uint8_t)Prescaler;
}

/*!
 * @brief     Enable intterupts
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     Interrupt: specifies the TMR interrupts sources
 *                     The parameter can be any combination of following values:
 *                     @arg TMR_INT_UPDATE: TMR update Interrupt Source
 *                     @arg TMR_INT_CH1:    TMR Capture Compare 1 Interrupt Source
 *                     @arg TMR_INT_CH2:    TMR Capture Compare 2 Interrupt Source
 *                     @arg TMR_INT_CH3:    TMR Capture Compare 3 Interrupt Source
 *                     @arg TMR_INT_CH4:    TMR Capture Compare 4 Interrupt Source
 *                     @arg TMR_INT_CCU:    TMR Commutation Interrupt Source
 *                     @arg TMR_INT_TRG:    TMR Trigger Interrupt Source
 *                     @arg TMR_INT_BRK:    TMR Break Interrupt Source
 *
 * @retval    None
 */
void Tmr_EnableInterrupt(TMR_T* TMRx, uint16_t Interrupt)
{
    TMRx->DIEN_R.DIEN |= Interrupt;
}

/*!
 * @brief     Disable intterupts
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     Interrupt: specifies the TMR interrupts sources
 *                     The parameter can be any combination of following values:
 *                     @arg TMR_INT_UPDATE: TMR update Interrupt Source
 *                     @arg TMR_INT_CH1:    TMR Capture Compare 1 Interrupt Source
 *                     @arg TMR_INT_CH2:    TMR Capture Compare 2 Interrupt Source
 *                     @arg TMR_INT_CH3:    TMR Capture Compare 3 Interrupt Source
 *                     @arg TMR_INT_CH4:    TMR Capture Compare 4 Interrupt Source
 *                     @arg TMR_INT_CCU:    TMR Commutation Interrupt Source
 *                     @arg TMR_INT_TRG:    TMR Trigger Interrupt Source
 *                     @arg TMR_INT_BRK:    TMR Break Interrupt Source
 *
 * @retval    None
 */
void Tmr_DisableInterrupt(TMR_T* TMRx, uint16_t Interrupt)
{
    TMRx->DIEN_R.DIEN &= ~(uint32_t)Interrupt;
}

/*!
 * @brief     Configures the TMRx Event to be generate by software
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer
 *
 * @param     Event:   specifies the TMR generate Event
 *                     The parameter can be any combination of following values:
 *                     @arg TMR_EVENT_UPDATE: TMR update Interrupt Source
 *                     @arg TMR_EVENT_CH1:    TMR Capture Compare 1 Interrupt Source
 *                     @arg TMR_EVENT_CH2:    TMR Capture Compare 2 Interrupt Source
 *                     @arg TMR_EVENT_CH3:    TMR Capture Compare 3 Interrupt Source
 *                     @arg TMR_EVENT_CH4:    TMR Capture Compare 4 Interrupt Source
 *                     @arg TMR_EVENT_CCU:    TMR Commutation Interrupt Source
 *                     @arg TMR_EVENT_TRG:    TMR Trigger Interrupt Source
 *                     @arg TMR_EVENT_BRK:    TMR Break Interrupt Source
 *
 * @retval    None
 *            TMR6 only TMR_EVENT_UPDATE
 *            TMR_EVENT_CCU and TMR_EVENT_BRK are used only with TMR1
 */
void Tmr_GenerateEvent(TMR_T* TMRx, uint16_t Event)
{
    TMRx->CEG_R.CEG |= Event;
}

/*!
 * @brief     Check whether the Flag is set or reset
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer.
 *
 * @param     Flag: specifies the TMR Flag
 *                     The parameter can be one of following values:
 *                     @arg TMR_FLAG_UPDATA:  TMR update Flag
 *                     @arg TMR_FLAG_CH1:     TMR Capture Compare 1 Flag
 *                     @arg TMR_FLAG_CH2:     TMR Capture Compare 2 Flag
 *                     @arg TMR_FLAG_CH3:     TMR Capture Compare 3 Flag
 *                     @arg TMR_FLAG_CH4:     TMR Capture Compare 4 Flag
 *                     @arg TMR_FLAG_CCU:     TMR Commutation Flag
 *                     @arg TMR_FLAG_TRG:     TMR Trigger Flag
 *                     @arg TMR_FLAG_BRK:     TMR Break Flag
 *                     @arg TMR_FLAG_CH1OC:   TMR Capture Compare 1 overcapture Flag
 *                     @arg TMR_FLAG_CH2OC:   TMR Capture Compare 2 overcapture Flag
 *                     @arg TMR_FLAG_CH3OC:   TMR Capture Compare 3 overcapture Flag
 *                     @arg TMR_FLAG_CH4OC:   TMR Capture Compare 4 overcapture Flag
 *
 * @retval    The new state of the Flag is SET or RESET
 *
 * @note      TMR2, TMR3 and TMR4 can have only TMR_FLAG_UPDATA, TMR_FLAG_CH1, TMR_FLAG_CH2 and TMR_FLAG_TRG
 *            TMR6, TMR7 and TMR8 can TMR_FLAG_UPDATA
 *            TMR_FLAG_BRK is used only with TMR1.
 *            TMR_FLAG_CCU is used only with TMR1, TMR2, TMR3 and TMR4
 */
uint16_t Tmr_ReadStatusFlag(const TMR_T* TMRx, Tmr_FlagType Flag)
{
    uint16_t tempReturn = RESET;
    
    if ((TMRx->STS_R.STS & (uint32_t)Flag) != (uint32_t)RESET)
    {
        tempReturn = SET;
    }
    else
    {
        tempReturn = RESET;
    }

    return tempReturn;
}

/*!
 * @brief     Clears the TMR's pending flags
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer.
 *
 * @param     Flag: specifies the TMR Flag
 *                     The parameter can be any combination of following values:
 *                     @arg TMR_FLAG_UPDATA:  TMR update Flag
 *                     @arg TMR_FLAG_CH1:     TMR Capture Compare 1 Flag
 *                     @arg TMR_FLAG_CH2:     TMR Capture Compare 2 Flag
 *                     @arg TMR_FLAG_CH3:     TMR Capture Compare 3 Flag
 *                     @arg TMR_FLAG_CH4:     TMR Capture Compare 4 Flag
 *                     @arg TMR_FLAG_CCU:     TMR Commutation Flag
 *                     @arg TMR_FLAG_TRG:     TMR Trigger Flag
 *                     @arg TMR_FLAG_BRK:     TMR Break Flag
 *                     @arg TMR_FLAG_CH1OC:   TMR Capture Compare 1 overcapture Flag
 *                     @arg TMR_FLAG_CH2OC:   TMR Capture Compare 2 overcapture Flag
 *                     @arg TMR_FLAG_CH3OC:   TMR Capture Compare 3 overcapture Flag
 *                     @arg TMR_FLAG_CH4OC:   TMR Capture Compare 4 overcapture Flag
 *
 * @retval    None
 */
void Tmr_ClearStatusFlag(TMR_T* TMRx, uint16_t Flag)
{
    TMRx->STS_R.STS = (uint16_t)(~Flag);
}

/*!
 * @brief     Check whether the TMR Interrupt Flag is set or reset
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer.
 *
 * @param     Flag: specifies the TMR interrupts Flag
 *                     The parameter can be one of following values:
 *                     @arg TMR_INT_FLAG_UPDATE: TMR update Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH1:    TMR Capture Compare 1 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH2:    TMR Capture Compare 2 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH3:    TMR Capture Compare 3 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH4:    TMR Capture Compare 4 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CCU:    TMR Commutation Interrupt Flag
 *                     @arg TMR_INT_FLAG_TRG:    TMR Trigger Interrupt Flag
 *                     @arg TMR_INT_FLAG_BRK:    TMR Break Interrupt Flag
 *
 * @retval    The new state of the INT Flag is SET or RESET
 *
 * @note      TMR2, TMR3 and TMR4 can have only TMR_FLAG_UPDATA, TMR_FLAG_CH1, TMR_FLAG_CH2 and TMR_FLAG_TRG
 *            TMR6, TMR7 and TMR8 can TMR_FLAG_UPDATA
 *            TMR_FLAG_BRK is used only with TMR1.
 *            TMR_FLAG_CCU is used only with TMR1, TMR2, TMR3 and TMR4
 */
uint16_t Tmr_ReadIntFlag(const TMR_T* TMRx, Tmr_IntFlagType Flag)
{
    uint16_t tempReturn = RESET;
    
    if (((TMRx->STS_R.STS & (uint32_t)Flag) != (uint32_t)RESET) && ((TMRx->DIEN_R.DIEN & (uint32_t)Flag) != (uint32_t)RESET))
    {
        tempReturn = SET;
    }
    else
    {
        tempReturn = RESET;
    }
    
    return tempReturn;
}

/*!
 * @brief     Clears the TMR's Interrupt pending bits
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer.
 *
 * @param     Flag: specifies the TMR interrupts Flag
 *                     The parameter can be any combination of following values:
 *                     @arg TMR_INT_FLAG_UPDATE: TMR update Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH1:    TMR Capture Compare 1 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH2:    TMR Capture Compare 2 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH3:    TMR Capture Compare 3 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CH4:    TMR Capture Compare 4 Interrupt Flag
 *                     @arg TMR_INT_FLAG_CCU:    TMR Commutation Interrupt Flag
 *                     @arg TMR_INT_FLAG_TRG:    TMR Trigger Interrupt Flag
 *                     @arg TMR_INT_FLAG_BRK:    TMR Break Interrupt Flag
 *
 * @retval    None
 */
void Tmr_ClearIntFlag(TMR_T* TMRx, uint16_t Flag)
{
    TMRx->STS_R.STS = (uint16_t)(~Flag);
}

/*!
 * @brief     Configures the TMRx's DMA interface.
 *
 * @param     TMRx: x can be can be 1, 2, 3, and 4 to select Timer
 *
 * @param     Address: DMA Base Address
 *
 * @param     Lenght: DMA Burst length
 *
 * @retval    None
 */
void Tmr_ConfigDMA(TMR_T* TMRx, Tmr_DmaBaseAdderssType Address, Tmr_DmaBaseLenghtType Lenght)
{
    TMRx->DCTRL_R.DCTRL = (uint32_t)Address | (uint32_t)Lenght;
}

/*!
 * @brief     Enable TMRx Requests
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer.
 *
 * @param     Souces: specifies the TMR DMA Souces
 *                     The parameter can be any combination of following values:
 *                     @arg TMR_DMA_UPDATE: TMR update DMA Souces
 *                     @arg TMR_DMA_CH1:    TMR Capture Compare 1 DMA Souces
 *                     @arg TMR_DMA_CH2:    TMR Capture Compare 2 DMA Souces
 *                     @arg TMR_DMA_CH3:    TMR Capture Compare 3 DMA Souces
 *                     @arg TMR_DMA_CH4:    TMR Capture Compare 4 DMA Souces
 *                     @arg TMR_DMA_CCU:    TMR Commutation DMA Souces
 *                     @arg TMR_DMA_TRG:    TMR Trigger DMA Souces
 *
 * @retval    None
 */
void Tmr_EnableDMASoure(TMR_T* TMRx, uint16_t Souces)
{
    TMRx->DIEN_R.DIEN |= Souces;
}

/*!
 * @brief     Disable TMRx Requests
 *
 * @param     TMRx: x can be can be 1, 2, 3, 4, 6, 7 and 8 to select Timer.
 *
 * @param     Souces: specifies the TMR DMA Souces
 *                     The parameter can be any combination of following values:
 *                     @arg TMR_DMA_UPDATE: TMR update DMA Souces
 *                     @arg TMR_DMA_CH1:    TMR Capture Compare 1 DMA Souces
 *                     @arg TMR_DMA_CH2:    TMR Capture Compare 2 DMA Souces
 *                     @arg TMR_DMA_CH3:    TMR Capture Compare 3 DMA Souces
 *                     @arg TMR_DMA_CH4:    TMR Capture Compare 4 DMA Souces
 *                     @arg TMR_DMA_CCU:    TMR Commutation DMA Souces
 *                     @arg TMR_DMA_TRG:    TMR Trigger DMA Souces
 *
 * @retval    None
 */
void Tmr_DisableDMASoure(TMR_T* TMRx, uint16_t Souces)
{
    TMRx->DIEN_R.DIEN &= ~(uint32_t)Souces;
}

/*!
 * @brief     Enable Capture Compare DMA Source
 *
 * @param     TMRx: x can be can be 1, 2, 3, and 4 to select Timer
 *
 * @retval    None
 */
void Tmr_EnableCCDMA(TMR_T* TMRx)
{
    TMRx->CTRL2_R.CTRL2_B.CCDSEL = ENABLE;
}

/*!
 * @brief     Disable Capture Compare DMA Source
 *
 * @param     TMRx: x can be can be 1, 2, 3, and 4 to select Timer
 *
 * @retval    None
 */
void Tmr_DisableCCDMA(TMR_T* TMRx)
{
    TMRx->CTRL2_R.CTRL2_B.CCDSEL = DISABLE;
}

/*!
 * @brief     Configures the TMRx internal Clock
 *
 * @param     TMRx: x can be can be 1, 2, 3, and 4 to select Timer
 *
 * @retval    None
 */
void Tmr_ConfigInternalClock(TMR_T* TMRx)
{
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = DISABLE;
}

/*!
 * @brief     Configures the TMRx Internal Trigger as External Clock
 *
 * @param     TMRx: x can be can be 1, 2, 3, and 4 to select Timer
 *
 * @param     Input: specifies the TMR trigger Souces
 *                     The parameter can be one of following values:
 *                     @arg TMR_TS_ITR0:    TMR Internal Trigger 0
 *                     @arg TMR_TS_ITR1:    TMR Internal Trigger 1
 *                     @arg TMR_TS_ITR2:    TMR Internal Trigger 2
 *                     @arg TMR_TS_ITR3:    TMR Internal Trigger 3
 *
 * @retval    None
 */
void Tmr_ConfigITRxExternalClock(TMR_T* TMRx, Tmr_InputTriggerSourceType Input)
{
    Tmr_SelectInputTrigger(TMRx, Input);
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = 0x07;
}

/*!
 * @brief     Configures the TMRx  Trigger as External Clock
 *
 * @param     TMRx: x can be can be 1, 2, 3, and 4 to select Timer
 *
 * @param     Input: specifies the TMR trigger Souces
 *                     The parameter can be one of following values:
 *                     @arg TMR_TS_TI1F_ED:  TI1 Edge Detector
 *                     @arg TMR_TS_TI1FP1:   Filtered Timer Input 1
 *                     @arg TMR_TS_TI2FP2:   Filtered Timer Input 2
 *
 * @param     ICpolarity: specifies the TMR IC Polarity
 *                     The parameter can be one of following values:
 *                     @arg TMR_IC_POLARITY_RISING:  TMR IC Polarity rising
 *                     @arg TMR_IC_POLARITY_FALLING: TMR IC Polarity falling
 *
 * @param     ICfilter:specifies the Filter value.This parameter must be a value between 0x00 and 0x0F.
 *
 * @retval    None
 */
void Tmr_ConfigTIxExternalClock(TMR_T* TMRx, Tmr_InputTriggerSourceType Input,
                                Tmr_IcPolarityType ICpolarity, uint16_t ICfilter)
{
    if (Input == TMR_TS_TI2FP2)
    {
        TI2Config(TMRx, (uint16_t)ICpolarity, (uint16_t)TMR_IC_SELECTION_DIRECT_TI, (uint16_t)ICfilter);
    }
    else
    {
        TI1Config(TMRx, (uint16_t)ICpolarity, (uint16_t)TMR_IC_SELECTION_DIRECT_TI, (uint16_t)ICfilter);
    }

    Tmr_SelectInputTrigger(TMRx, Input);
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = 0x07;
}

/*!
 * @brief     Configures the External clock Mode1
 *
 * @param     TMRx: x can be can be 1, 2, 3, and 4 to select Timer
 *
 * @param     Prescaler: The external Trigger Prescaler.
 *
 * @param     Polarity: The external Trigger Polarity.
 *
 * @param     Filter: External Trigger Filter.
 *
 * @retval    None
 */
void Tmr_ConfigExternalClockMode1(TMR_T* TMRx, Tmr_ExttrgPrescalerType Prescaler,
                                  Tmr_ExttrgPolarityType Polarity, uint16_t Filter)
{
    Tmr1_ConfigExternalTrigger(Prescaler, Polarity, Filter);
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = BIT_RESET;
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = 0x07;
    TMRx->SMCTRL_R.SMCTRL_B.TRGSEL = 0x07;
}

/*!
 * @brief     Configures the External clock Mode2
 *
 * @param     Prescaler: The external Trigger Prescaler
 *
 * @param     Polarity: The external Trigger Polarity
 *
 * @param     Filter: External Trigger Filter
 *
 * @retval    None
 */
void Tmr1_ConfigExternalClockMode2(Tmr_ExttrgPrescalerType Prescaler,
                                  Tmr_ExttrgPolarityType Polarity, uint16_t Filter)
{
    Tmr1_ConfigExternalTrigger(Prescaler, Polarity, Filter);
    TMR1->SMCTRL_R.SMCTRL_B.ECEN = ENABLE;
}

/*!
 * @brief     Selects the Input Trigger Source
 *
 * @param     TMRx: where x can be 1, 2, 3 and 4 select the TMR peripheral
 *
 * @param     Input: specifies the TMR trigger Souces
 *                     The parameter can be one of following values:
 *                     @arg TMR_TS_ITR0:     TMR Internal Trigger 0
 *                     @arg TMR_TS_ITR1:     TMR Internal Trigger 1
 *                     @arg TMR_TS_ITR2:     TMR Internal Trigger 2
 *                     @arg TMR_TS_ITR3:     TMR Internal Trigger 3
 *                     @arg TMR_TS_TI1F_ED:  TI1 Edge Detector
 *                     @arg TMR_TS_TI1FP1:   Filtered Timer Input 1
 *                     @arg TMR_TS_TI2FP2:   Filtered Timer Input 2
 *                     @arg TMR_TS_ETRF:     External Trigger Input
 *
 * @retval    None
 */
void Tmr_SelectInputTrigger(TMR_T* TMRx, Tmr_InputTriggerSourceType Input)
{
    TMRx->SMCTRL_R.SMCTRL_B.TRGSEL = BIT_RESET;
    TMRx->SMCTRL_R.SMCTRL_B.TRGSEL = (uint8_t)Input;
}

/*!
 * @brief     Selects the Trigger Output Mode.
 *
 * @param     TMRx: where x can be 1, 2, 3 and 4 select the TMR peripheral
 *
 * @param     Source: specifies the TMR trigger Souces
 *                    The parameter can be one of following values:
 *                    For all TMR:
 *                        @arg TMR_TRGOSOURCE_RESET
 *                        @arg TMR_TRGOSOURCE_ENABLE
 *                        @arg TMR_TRGOSOURCE_UPDATE
                      For all TMR except TMR6 and TMR7
 *                        @arg TMR_TRGOSOURCE_OC1,
 *                        @arg TMR_TRGOSOURCE_OC1REF
 *                        @arg TMR_TRGOSOURCE_OC2REF
 *                        @arg TMR_TRGOSOURCE_OC3REF
 *                        @arg TMR_TRGOSOURCE_OC4REF
 *
 * @retval    None
 */
void Tmr_SelectOutputTrigger(TMR_T* TMRx, Tmr_TrgosourceType Source)
{
    TMRx->CTRL2_R.CTRL2_B.MMSEL = (uint8_t)Source;
}

/*!
 * @brief     Selects the Slave Mode.
 *
 * @param     TMRx: where x can be 1, 2, 3 and 4 select the TMR peripheral
 *
 * @param     Mode: Tmr_SlavemodeType
 *
 * @retval    None
 */
void Tmr_SelectSlaveMode(TMR_T* TMRx, Tmr_SlavemodeType Mode)
{
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = (uint8_t)Mode;
}

/*!
 * @brief     Enable the Master Slave Mode
 *
 * @param     TMRx: where x can be 1, 2, 3 and 4 select the TMR peripheral
 *
 * @retval    None
 */
void Tmr_EnableMasterSlaveMode(TMR_T* TMRx)
{
    TMRx->SMCTRL_R.SMCTRL_B.MSMEN = ENABLE ;
}

/*!
 * @brief     Disable the Master Slave Mode
 *
 * @param     TMRx: where x can be 1, 2, 3 and 4 select the TMR peripheral
 *
 * @retval    None
 */
void Tmr_DisableMasterSlaveMode(TMR_T* TMRx)
{
    TMRx->SMCTRL_R.SMCTRL_B.MSMEN = DISABLE ;
}

/*!
 * @brief     Configures the TMRx External Trigger (ETR)
 *
 * @param     Prescaler: The external Trigger Prescaler
 *
 * @param     Polarity: The external Trigger Polarity
 *
 * @param     Filter: External Trigger Filter
 *
 * @retval    None
 */
void Tmr1_ConfigExternalTrigger(Tmr_ExttrgPrescalerType Prescaler,
                               Tmr_ExttrgPolarityType Polarity, uint16_t Filter)
{
    TMR1->SMCTRL_R.SMCTRL &= 0x00FFu;
    TMR1->SMCTRL_R.SMCTRL_B.ETPCFG = (uint8_t)Prescaler;
    TMR1->SMCTRL_R.SMCTRL_B.ETPOL = (uint8_t)Polarity;
    TMR1->SMCTRL_R.SMCTRL_B.ETFCFG = Filter;
}

/*!
 * @brief     Configures the Encoder Interface
 *
 * @param     TMRx: where x can be 1, 2, 3 and 4 select the TMR peripheral
 *
 * @param     EncodeMode: specifies the Encoder Mode
 *
 * @param     IC1Polarity: specifies the IC1 Polarity
 *
 * @param     IC2Polarity: specifies the IC2 Polarity
 *
 * @retval    None
 */
void Tmr_ConfigEncodeInterface(TMR_T* TMRx, Tmr_EncoderModeType EncodeMode, Tmr_IcPolarityType IC1Polarity,
                               Tmr_IcPolarityType IC2Polarity)
{
    /** Set the encoder Mode */
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = BIT_RESET;
    TMRx->SMCTRL_R.SMCTRL_B.SMFSEL = (uint8_t)EncodeMode;

    /** Select the Capture Compare 1 and the Capture Compare 2 as Input */
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC1SEL = BIT_RESET ;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC2SEL = BIT_RESET ;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC1SEL = BIT_SET ;
    TMRx->CCM1R.CCM1_INPUT_R.CCM1_INPUT_B.CC2SEL = BIT_SET ;

    /** Set the TI1 and the TI2 Polarities */
    TMRx->CCEN_R.CCEN &= ~((uint32_t)TMR_IC_POLARITY_BOTHEDGE) & ~((uint32_t)TMR_IC_POLARITY_BOTHEDGE << 4u);
    TMRx->CCEN_R.CCEN |= ((uint32_t)IC1Polarity | ((uint32_t)IC2Polarity << 4u));
}

/*!
 * @brief     Enables Hall sensor interface.
 *
 * @param     TMRx: where x can be 1, 3 select the TMR peripheral
 *
 * @retval    None
 */
void Tmr_EnableHallSensor(TMR_T* TMRx)
{
    TMRx->CTRL2_R.CTRL2_B.TI1SEL = ENABLE;
}

/*!
 * @brief     Disable Hall sensor interface.
 *
 * @param     TMRx: where x can be 1, 2, 3 and 4 select the TMR peripheral
 *
 * @retval    None
 */
void Tmr_DisableHallSensor(TMR_T* TMRx)
{
    TMRx->CTRL2_R.CTRL2_B.TI1SEL = DISABLE;
}

/*!
 * @brief     Configures the TMR3 Remapping Input Capabilities.
 *
 * @param     Remap: specifies the TMR Input reampping Source
 *                    The parameter can be one of following values:
 *                        @arg TMR_REMAP_GPIO
 *                        @arg TMR_REMAP_RTC_CLK
 *                        @arg TMR_REMAP_HSEDiv32
 *                        @arg TMR_REMAP_MCO
 *
 * @retval    None
 */
void Tmr3_ConfigRemap(Tmr_RemapType Remap)
{
    TMR3->OPT_R.OPT_B.RMPSEL = (uint8_t)Remap;
}

/**@} end of group TMR_Functions*/
/**@} end of group TMR_Driver */
/**@} end of group G32A10xx_StdPeriphDriver*/
