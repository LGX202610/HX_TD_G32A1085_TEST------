/*!
 * @file        g32a10xx_tmr.h
 *
 * @brief       This file contains all functions prototype and macros for the TMR peripheral
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

#ifndef G32A10xx_TMR_H
#define G32A10xx_TMR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup TMR_Driver
  @{
*/

/** @defgroup  TMR_Enumerations Enumerations
  @{
*/

/**
 * @brief   Counter_Mode
 */
typedef enum
{
    TMR_COUNTER_MODE_UP = 0,                /*!< Timer Up Counting Mode */
    TMR_COUNTER_MODE_DOWN = 1,              /*!< Timer Down Counting Mode */
    TMR_COUNTER_MODE_CENTERALIGNED1 = 2,    /*!< Timer Center Aligned Mode1 */
    TMR_COUNTER_MODE_CENTERALIGNED2 = 4,    /*!< Timer Center Aligned Mode2 */
    TMR_COUNTER_MODE_CENTERALIGNED3 = 6     /*!< Timer Center Aligned Mode3 */
} Tmr_CounterModeType;

/**
 * @brief   Clock_Division_CKD
 */
typedef enum
{
    TMR_CKD_DIV1 = 0,    /*!< TDTS = Tck_tim */
    TMR_CKD_DIV2 = 1,    /*!< TDTS = 2 * Tck_tim */
    TMR_CKD_DIV4 = 2     /*!< TDTS = 4 * Tck_tim */
} Tmr_CkdType;

/**
 * @brief   Prescaler_Reload_Mode
 */
typedef enum
{
    TMR_PRESCALER_RELOAD_UPDATA = 0,    /*!< The Prescaler reload at the update event */
    TMR_PRESCALER_RELOAD_IMMEDIATE = 1  /*!< The Prescaler reload immediately */
} Tmr_PrescalerReloadType;

/**
 * @brief    TMR UpdateSource
 */
typedef enum
{
    TMR_UPDATE_SOURCE_GLOBAL = 0,   /*!< Source of update is Counter overflow/underflow.
                                       - UEG bit of Control event generation register(CEG) is set.
                                       - Update generation through the slave mode controller. */
    TMR_UPDATE_SOURCE_REGULAR = 1   /*!< Source of update is Counter overflow/underflow */
} Tmr_UpdateSourceType;

/**
 * @brief    TMR OPMode
 */
typedef enum
{
    TMR_OPMODE_REPETITIVE = 0,  /*!< Enable repetitive pulse mode */
    TMR_OPMODE_SINGLE = 1       /*!< Enable single pulse mode */
} Tmr_OpmodeType;

/**
 * @brief TMR Specifies the Off-State selection used in Run mode
 */
typedef enum
{
    TMR_RMOS_STATE_DISABLE = 0,     /*!< Disable run mode off-state */
    TMR_RMOS_STATE_ENABLE = 1       /*!< Enable run mode off-state */
} Tmr_RmosStateType;

/**
 * @brief TMR Closed state configuration in idle mode
 */
typedef enum
{
    TMR_IMOS_STATE_DISABLE = 0,     /*!< Disable idle mode off-state */
    TMR_IMOS_STATE_ENABLE = 1       /*!< Enable idle mode off-state */
} Tmr_ImosStateType;

/**
 * @brief TMR Protect mode configuration values
 */
typedef enum
{
    TMR_LOCK_LEVEL_OFF = 0,  /*!< No lock write protection */
    TMR_LOCK_LEVEL_1 = 1,    /*!< Lock write protection level 1 */
    TMR_LOCK_LEVEL_2 = 2,    /*!< Lock write protection level 2 */
    TMR_LOCK_LEVEL_3 = 3     /*!< Lock write protection level 3 */
} Tmr_LockLevelType;

/**
 * @brief TMR break state
 */
typedef enum
{
    TMR_BREAK_STATE_DISABLE,  /*!< Disable brake function */
    TMR_BREAK_STATE_ENABLE    /*!< Enable brake function */
} Tmr_BreakStateType;

/**
 * @brief TMR Specifies the Break Input pin polarity.
 */
typedef enum
{
    TMR_BREAK_POLARITY_LOW,  /*!< BREAK low level valid */
    TMR_BREAK_POLARITY_HIGH  /*!< BREAK high level valid */
} Tmr_BreakPolarityType;

/**
 * @brief TMR Automatic Output feature is enable or disable
 */
typedef enum
{
    TMR_AUTOMATIC_OUTPUT_DISABLE,  /*!< Disable automatic output */
    TMR_AUTOMATIC_OUTPUT_ENABLE    /*!< Enable automatic output */
} Tmr_AutomaticOutputType;

/**
 * @brief TMR_Output_Compare_and_PWM_modes
 */
typedef enum
{
    TMR_OC_MODE_TMRING     = 0x00, /*!< Frozen TMR output compare mode */
    TMR_OC_MODE_ACTIVE     = 0x01, /*!< Set output to high when matching */
    TMR_OC_MODE_INACTIVE   = 0x02, /*!< Set output to low when matching */
    TMR_OC_MODE_TOGGEL     = 0x03, /*!< Toggle output when matching */
    TMR_OC_MODE_LOWLEVEL   = 0x04, /*!< Force output to be low */
    TMR_OC_MODE_HIGHLEVEL  = 0x05, /*!< Force output to be high */
    TMR_OC_MODE_PWM1       = 0x06, /*!< PWM1 mode */
    TMR_OC_MODE_PWM2       = 0x07  /*!< PWM2 mode */
} Tmr_OcModeType;

/**
 * @brief TMR_Output_Compare_state
 */
typedef enum
{
    TMR_OUTPUT_STATE_DISABLE,   /*!< Disable output compare */
    TMR_OUTPUT_STATE_ENABLE     /*!< Enable output compare */
} Tmr_OcOutputStateType;

/**
 * @brief TMR_Output_Compare_N_state
 */
typedef enum
{
    TMR_OUTPUT_NSTATE_DISABLE,  /*!< Disable complementary output */
    TMR_OUTPUT_NSTATE_ENABLE    /*!< Enable complementary output */
} Tmr_OcOutputNstateType;

/**
 * @brief TMR_Output_Compare_Polarity
 */
typedef enum
{
    TMR_OC_POLARITY_HIGH,  /*!< Output Compare active high */
    TMR_OC_POLARITY_LOW    /*!< Output Compare active low */
} Tmr_OcPolarityType;

/**
 * @brief TMR_Output_Compare_N_Polarity
 */
typedef enum
{
    TMR_OC_NPOLARITY_HIGH,    /*!< Output Compare active high */
    TMR_OC_NPOLARITY_LOW      /*!< Output Compare active low */
} Tmr_OcNpolarityType;

/**
 * @brief TMR_Output_Compare_Idle_State
 */
typedef enum
{
    TMR_OCIDLESTATE_RESET,  /*!< Reset output compare idle state */
    TMR_OCIDLESTATE_SET     /*!< Set output compare idle state */
} Tmr_OcIdleStateType;

/**
 * @brief TMR_Output_Compare_N_Idle_State
 */
typedef enum
{
    TMR_OCNIDLESTATE_RESET,  /*!< Reset output complementary idle state */
    TMR_OCNIDLESTATE_SET     /*!< Set output complementary idle state */
} Tmr_OcNidleStateType;

/**
 * @brief TMR Input Capture Init structure definition
 */
typedef enum
{
    TMR_CHANNEL_1 = 0x0000,  /*!< Timer Channel 1 */
    TMR_CHANNEL_2 = 0x0004,  /*!< Timer Channel 2 */
    TMR_CHANNEL_3 = 0x0008,  /*!< Timer Channel 3 */
    TMR_CHANNEL_4 = 0x000C   /*!< Timer Channel 4 */
} Tmr_ChannelType;

/**
 * @brief    TMR ForcedAction
 */
typedef enum
{
    TMR_FORCEDACTION_INACTIVE = 0x04,  /*!< Force inactive level on OC1REF */
    TMR_FORCEDACTION_ACTIVE   = 0x05   /*!< Force active level on OC1REF */
} Tmr_ForcedActionType;

/**
 * @brief    TMR Output_Compare_Preload_State
 */
typedef enum
{
    TMR_OC_PRELOAD_DISABLE,  /*!< Disable preload */
    TMR_OC_PRELOAD_ENABLE    /*!< Enable preload */
} Tmr_OcPreloadType;

/**
 * @brief    TMR Output_Compare_Fast_State
 */
typedef enum
{
    TMR_OCFAST_DISABLE,  /*!< Disable fast output compare */
    TMR_OCFAST_ENABLE    /*!< Enable fast output compare */
} Tmr_OcFastType;

/**
 * @brief    TMR Output_Compare_Clear_State
 */
typedef enum
{
    TMR_OCCLER_DISABLE,  /*!< Disable output compare clear */
    TMR_OCCLER_ENABLE    /*!< Enable output compare clear */
} Tmr_OcclerType;

/**
 * @brief    TMR_OCReferenceClear Clear source
 */
typedef enum
{
    TMR_OCCS_ETRF,         /*!< Select ETRF as clear source */
    TMR_OCCS_OCREFCLR      /*!< Select OCREFCLR as clear source */
} Tmr_OccselType;

/**
 * @brief TMR Input_Capture_Polarity
 */
typedef enum
{
    TMR_IC_POLARITY_RISING   = 0x00,  /*!< Rising edge */
    TMR_IC_POLARITY_FALLING  = 0x02,  /*!< Falling edge */
    TMR_IC_POLARITY_BOTHEDGE = 0x0A   /*!< Both rising and falling edge */
} Tmr_IcPolarityType;

/**
 * @brief TMR Input_Capture_Selection
 */
typedef enum
{
    TMR_IC_SELECTION_DIRECT_TI   = 0x01,  /*!< Input capture mapping in TI1 */
    TMR_IC_SELECTION_INDIRECT_TI = 0x02,  /*!< Input capture mapping in TI2 */
    TMR_IC_SELECTION_TRC         = 0x03   /*!< Input capture mapping in TRC */
} Tmr_IcSelectionType;

/**
 * @brief TMR_Input_Capture_Prescaler
 */
typedef enum
{
    TMR_ICPSC_DIV1 = 0x00,   /*!< No prescaler */
    TMR_ICPSC_DIV2 = 0x01,   /*!< Capture is done once every 2 events */
    TMR_ICPSC_DIV4 = 0x02,   /*!< capture is done once every 4 events */
    TMR_ICPSC_DIV8 = 0x03    /*!< capture is done once every 8 events */
} Tmr_IcPrescalerType;

/**
 * @brief    TMR_interrupt_sources
 */
typedef enum
{
    TMR_INT_UPDATE = 0x0001,  /*!< Timer update Interrupt source */
    TMR_INT_CH1    = 0x0002,  /*!< Timer Capture Compare 1 Interrupt source */
    TMR_INT_CH2    = 0x0004,  /*!< Timer Capture Compare 2 Interrupt source */
    TMR_INT_CH3    = 0x0008,  /*!< Timer Capture Compare 3 Interrupt source */
    TMR_INT_CH4    = 0x0010,  /*!< Timer Capture Compare 4 Interrupt source */
    TMR_INT_CCU    = 0x0020,  /*!< Timer Commutation Interrupt */
    TMR_INT_TRG    = 0x0040,  /*!< Timer Trigger Interrupt source */
    TMR_INT_BRK    = 0x0080   /*!< Timer Break Interrupt source */
} Tmr_IntType;

/**
 * @brief    TMR_event_sources
 */
typedef enum
{
    TMR_EVENT_UPDATE = 0x0001,  /*!< Timer update Interrupt source */
    TMR_EVENT_CH1    = 0x0002,  /*!< Timer Capture Compare 1 Event source */
    TMR_EVENT_CH2    = 0x0004,  /*!< Timer Capture Compare 2 Event source */
    TMR_EVENT_CH3    = 0x0008,  /*!< Timer Capture Compare 3 Event source */
    TMR_EVENT_CH4    = 0x0010,  /*!< Timer Capture Compare 4 Event source */
    TMR_EVENT_CCU    = 0x0020,  /*!< Timer Commutation Event source */
    TMR_EVENT_TRG    = 0x0040,  /*!< Timer Trigger Event source */
    TMR_EVENT_BRK    = 0x0080   /*!< Timer Break Event source */
} Tmr_EventType;

/**
 * @brief    TMR_interrupt_flag
 */
typedef enum
{
    TMR_INT_FLAG_UPDATE = 0x0001,  /*!< Timer update Interrupt source */
    TMR_INT_FLAG_CH1    = 0x0002,  /*!< Timer Capture Compare 1 Interrupt source */
    TMR_INT_FLAG_CH2    = 0x0004,  /*!< Timer Capture Compare 2 Interrupt source */
    TMR_INT_FLAG_CH3    = 0x0008,  /*!< Timer Capture Compare 3 Interrupt source */
    TMR_INT_FLAG_CH4    = 0x0010,  /*!< Timer Capture Compare 4 Interrupt source */
    TMR_INT_FLAG_CCU    = 0x0020,  /*!< Timer Commutation Interrupt source */
    TMR_INT_FLAG_TRG    = 0x0040,  /*!< Timer Trigger Interrupt source */
    TMR_INT_FLAG_BRK    = 0x0080   /*!< Timer Break Interrupt source */
} Tmr_IntFlagType;

/**
 * @brief    TMR Flag
 */
typedef enum
{
    TMR_FLAG_UPDATE  = 0x0001,  /*!< Timer update Flag */
    TMR_FLAG_CH1     = 0x0002,  /*!< Timer Capture Compare 1 Flag */
    TMR_FLAG_CH2     = 0x0004,  /*!< Timer Capture Compare 2 Flag */
    TMR_FLAG_CH3     = 0x0008,  /*!< Timer Capture Compare 3 Flag */
    TMR_FLAG_CH4     = 0x0010,  /*!< Timer Capture Compare 4 Flag */
    TMR_FLAG_CCU     = 0x0020,  /*!< Timer Commutation Flag */
    TMR_FLAG_TRG     = 0x0040,  /*!< Timer Trigger Flag */
    TMR_FLAG_BRK     = 0x0080,  /*!< Timer Break Flag (Only for TMR1 and TMR8) */
    TMR_FLAG_CH1OC   = 0x0200,  /*!< Timer Capture Compare 1 Repetition Flag */
    TMR_FLAG_CH2OC   = 0x0400,  /*!< Timer Capture Compare 2 Repetition Flag */
    TMR_FLAG_CH3OC   = 0x0800,  /*!< Timer Capture Compare 3 Repetition Flag */
    TMR_FLAG_CH4OC   = 0x1000   /*!< Timer Capture Compare 4 Repetition Flag */
} Tmr_FlagType;

/**
 * @brief    TMR DMA Base Address
 */
typedef enum
{
    TMR_DMABASE_CTRL1   = 0x0000,  /*!< TMR CTRL1 DMA base address setup */
    TMR_DMABASE_CTRL2   = 0x0001,  /*!< TMR CTRL2 DMA base address setup */
    TMR_DMABASE_SMCTRL  = 0x0002,  /*!< TMR SMCTRL DMA base address setup */
    TMR_DMABASE_DIEN    = 0x0003,  /*!< TMR DIEN DMA base address setup */
    TMR_DMABASE_STS     = 0x0004,  /*!< TMR STS DMA base address setup */
    TMR_DMABASE_CEG     = 0x0005,  /*!< TMR CEG DMA base address setup */
    TMR_DMABASE_CCM1    = 0x0006,  /*!< TMR CCM1 DMA base address setup */
    TMR_DMABASE_CCM2    = 0x0007,  /*!< TMR CCM2 DMA base address setup */
    TMR_DMABASE_CHCTRL  = 0x0008,  /*!< TMR CHCTRL DMA base address setup */
    TMR_DMABASE_CNT     = 0x0009,  /*!< TMR CNT DMA base address setup */
    TMR_DMABASE_DIV     = 0x000A,  /*!< TMR DIV DMA base address setup */
    TMR_DMABASE_AUTORLD = 0x000B,  /*!< TMR AUTORLD DMA base address setup */
    TMR_DMABASE_REPCNT  = 0x000C,  /*!< TMR REPCNT DMA base address setup */
    TMR_DMABASE_CH1CC   = 0x000D,  /*!< TMR CH1CC DMA base address setup */
    TMR_DMABASE_CH2CC   = 0x000E,  /*!< TMR CH2CC DMA base address setup */
    TMR_DMABASE_CH3CC   = 0x000F,  /*!< TMR CH3CC DMA base address setup */
    TMR_DMABASE_CH4CC   = 0x0010,  /*!< TMR CH4CC DMA base address setup */
    TMR_DMABASE_BDT     = 0x0011,  /*!< TMR BDT DMA base address setup */
    TMR_DMABASE_DMAB    = 0x0012   /*!< TMR DMAB DMA base address setup */
} Tmr_DmaBaseAdderssType;

/**
 * @brief    TMR DMA Burst Lenght
 */
typedef enum
{
    TMR_DMA_BURSTLENGHT_1TRANSFER   = 0x0000,  /*!< Select TMR DMA burst Length 1 */
    TMR_DMA_BURSTLENGHT_2TRANSFERS  = 0x0100,  /*!< Select TMR DMA burst Length 2 */
    TMR_DMA_BURSTLENGHT_3TRANSFERS  = 0x0200,  /*!< Select TMR DMA burst Length 3 */
    TMR_DMA_BURSTLENGHT_4TRANSFERS  = 0x0300,  /*!< Select TMR DMA burst Length 4 */
    TMR_DMA_BURSTLENGHT_5TRANSFERS  = 0x0400,  /*!< Select TMR DMA burst Length 5 */
    TMR_DMA_BURSTLENGHT_6TRANSFERS  = 0x0500,  /*!< Select TMR DMA burst Length 6 */
    TMR_DMA_BURSTLENGHT_7TRANSFERS  = 0x0600,  /*!< Select TMR DMA burst Length 7 */
    TMR_DMA_BURSTLENGHT_8TRANSFERS  = 0x0700,  /*!< Select TMR DMA burst Length 8 */
    TMR_DMA_BURSTLENGHT_9TRANSFERS  = 0x0800,  /*!< Select TMR DMA burst Length 9 */
    TMR_DMA_BURSTLENGHT_10TRANSFERS = 0x0900,  /*!< Select TMR DMA burst Length 10 */
    TMR_DMA_BURSTLENGHT_11TRANSFERS = 0x0A00,  /*!< Select TMR DMA burst Length 11 */
    TMR_DMA_BURSTLENGHT_12TRANSFERS = 0x0B00,  /*!< Select TMR DMA burst Length 12 */
    TMR_DMA_BURSTLENGHT_13TRANSFERS = 0x0C00,  /*!< Select TMR DMA burst Length 13 */
    TMR_DMA_BURSTLENGHT_14TRANSFERS = 0x0D00,  /*!< Select TMR DMA burst Length 14 */
    TMR_DMA_BURSTLENGHT_15TRANSFERS = 0x0E00,  /*!< Select TMR DMA burst Length 15 */
    TMR_DMA_BURSTLENGHT_16TRANSFERS = 0x0F00,  /*!< Select TMR DMA burst Length 16 */
    TMR_DMA_BURSTLENGHT_17TRANSFERS = 0x1000,  /*!< Select TMR DMA burst Length 17 */
    TMR_DMA_BURSTLENGHT_18TRANSFERS = 0x1100   /*!< Select TMR DMA burst Length 18 */
} Tmr_DmaBaseLenghtType;

/**
 * @brief    TMR DMA Soueces
 */
typedef enum
{
    TMR_DMA_UPDATE    = 0x0100,  /*!< TMR update DMA souces */
    TMR_DMA_CH1       = 0x0200,  /*!< TMR Capture Compare 1 DMA souces */
    TMR_DMA_CH2       = 0x0400,  /*!< TMR Capture Compare 2 DMA souces */
    TMR_DMA_CH3       = 0x0800,  /*!< TMR Capture Compare 3 DMA souces */
    TMR_DMA_CH4       = 0x1000,  /*!< TMR Capture Compare 4 DMA souces */
    TMR_DMA_CCU       = 0x2000,  /*!< TMR Commutation DMA souces */
    TMR_DMA_TRG       = 0x4000   /*!< TMR Trigger DMA souces */
} Tmr_DmaSoucesType;

/**
 * @brief    TMR Internal_Trigger_Selection
 */
typedef enum
{
    TMR_TS_ITR0     = 0x00,  /*!< Internal Trigger 0 */
    TMR_TS_ITR1     = 0x01,  /*!< Internal Trigger 1 */
    TMR_TS_ITR2     = 0x02,  /*!< Internal Trigger 2 */
    TMR_TS_ITR3     = 0x03,  /*!< Internal Trigger 3 */
    TMR_TS_TI1F_ED  = 0x04,  /*!< TI1 Edge Detector */
    TMR_TS_TI1FP1   = 0x05,  /*!< Filtered Timer Input 1 */
    TMR_TS_TI2FP2   = 0x06,  /*!< Filtered Timer Input 2 */
    TMR_TS_ETRF     = 0x07   /*!< External Trigger input */
} Tmr_InputTriggerSourceType;

/**
 * @brief    TMR  The external Trigger Prescaler.
 */
typedef enum
{
    TMR_ExtTRGPSC_OFF   = 0x00,  /*!< ETRP Prescaler OFF */
    TMR_EXTTRGPSC_DIV2  = 0x01,  /*!< ETRP frequency divided by 2 */
    TMR_EXTTRGPSC_DIV4  = 0x02,  /*!< ETRP frequency divided by 4 */
    TMR_EXTTRGPSC_DIV8  = 0x03   /*!< ETRP frequency divided by 8 */
} Tmr_ExttrgPrescalerType;

/**
 * @brief    TMR External_Trigger_Polarity
 */
typedef enum
{
    TMR_EXTTRGPOLARITY_INVERTED      = 0x01,  /*!< Active low or falling edge active */
    TMR_EXTTGRPOLARITY_NONINVERTED   = 0x00   /*!< Active high or rising edge active */
} Tmr_ExttrgPolarityType;

/**
 * @brief    TMR OPMode
 */
typedef enum
{
    TMR_TRGOSOURCE_RESET,   /*!< Select reset signal as TRGO source  */
    TMR_TRGOSOURCE_ENABLE,  /*!< Select enable signal as TRGO source */
    TMR_TRGOSOURCE_UPDATE,  /*!< Select update signal as TRGO source */
    TMR_TRGOSOURCE_OC1,     /*!< Select OC1 signal as TRGO source */
    TMR_TRGOSOURCE_OC1REF,  /*!< Select OC1REF signal as TRGO source */
    TMR_TRGOSOURCE_OC2REF,  /*!< Select OC2REF signal as TRGO source */
    TMR_TRGOSOURCE_OC3REF,  /*!< Select OC3REF signal as TRGO source */
    TMR_TRGOSOURCE_OC4REF   /*!< Select OC4REF signal as TRGO source */
} Tmr_TrgosourceType;

/**
 * @brief    TMR OPMode
 */
typedef enum
{
    TMR_SLAVEMODE_RESET     = 0x04,  /*!< Reset mode */
    TMR_SLAVEMODE_GATED     = 0x05,  /*!< Gated mode */
    TMR_SLAVEMODE_TRIGGER   = 0x06,  /*!< Trigger mode */
    TMR_SLAVEMODE_EXTERNALL = 0x07   /*!< External 1 mode */
} Tmr_SlavemodeType;

/**
 * @brief    TMR Encoder_Mode
 */
typedef enum
{
    TMR_ENCODER_MODE_TI1      = 0x01,  /*!< Encoder mode 1 */
    TMR_ENCODER_MODE_TI2      = 0x02,  /*!< Encoder mode 2 */
    TMR_ENCODER_MODE_TI12     = 0x03   /*!< Encoder mode 3 */
} Tmr_EncoderModeType;

/**
 * @brief    TMR Remap Select
 */
typedef enum
{
    TMR_REMAP_GPIO      = 0x00,  /*!< TMR input is connected to GPIO */
    TMR_REMAP_RTC_CLK   = 0x01,  /*!< TMR input is connected to RTC clock */
    TMR_REMAP_HSEDiv32  = 0x02,  /*!< TMR input is connected to HSE clock/32 */
    TMR_REMAP_MCO       = 0x03   /*!< TMR input is connected to MCO */
} Tmr_RemapType;

/**@} end of group TMR_Enumerations*/

/** @defgroup TMR_Structures Stuctures
  @{
*/

/**
  * @brief  TMR Time Base Init structure definition
  * @note   This sturcture is used with all TMRx.
  */
typedef struct
{
    uint16_t                div;                /*!< This must between 0x0000 and 0xFFFF */
    Tmr_CounterModeType     counterMode;        /*!< TMR counter mode selection */
    uint32_t                period;             /*!< This must between 0x0000 and 0xFFFF */
    Tmr_CkdType             clockDivision;      /*!< TMR clock division selection */
    uint8_t                 repetitionCounter;  /*!< This must between 0x00 and 0xFF, only for TMR1 and TMR8. */
} Tmr_TimeBaseType;

/**
 * @brief    TMR BDT structure definition
 */
typedef struct
{
    Tmr_RmosStateType        RMOS_State;       /*!< TMR Specifies the Off-State selection used in Run mode selection */
    Tmr_ImosStateType        IMOS_State;       /*!< TMR Closed state configuration in idle mode selection */
    Tmr_LockLevelType        lockLevel;        /*!< TMR Protect mode configuration values selection */
    uint8_t                  deadTime;         /*!< Setup dead time */
    Tmr_BreakStateType       breakState;       /*!< Setup TMR BRK state */
    Tmr_BreakPolarityType    breakPolarity;    /*!< Setup TMR BRK polarity */
    Tmr_AutomaticOutputType  automaticOutput;  /*!< Setup break input pin polarity */
} Tmr_BdtInitType;
/**
 * @brief    TMR Config struct definition
 */
typedef struct
{
    Tmr_OcModeType           OC_Mode;            /*!< Specifies the TMR mode. */

    Tmr_OcOutputStateType    OC_OutputState;     /*!< Specifies the TMR Output Compare state. */

    Tmr_OcOutputNstateType   OC_OutputNState;    /*!< Specifies the TMR complementary Output Compare state.  @note This parameter is valid only for TMR1 and TMR8. */

    Tmr_OcPolarityType       OC_Polarity;        /*!<  Specifies the output polarity. */

    Tmr_OcNpolarityType      OC_NPolarity;       /*!<  Specifies the complementary output polarity.  @note This parameter is valid only for TMR1 and TMR8. */

    Tmr_OcIdleStateType      OC_Idlestate;       /*!<  Specifies the TMR Output Compare pin state during Idle state. @note This parameter is valid only for TMR1 and TMR8. */

    Tmr_OcNidleStateType     OC_NIdlestate;      /*!<  Specifies the TMR Output Compare pin state during Idle state. @note This parameter is valid only for TMR1 and TMR8. */

    uint16_t                 Pulse;              /*!< Specifies the pulse value to be loaded into the Capture Compare Register. */

} Tmr_OcConfigType;

/**
 * @brief    TMR Input Capture Config struct definition
 */
typedef struct
{
    Tmr_ChannelType         channel;            /*!<  Specifies the TMR channel. */

    Tmr_IcPolarityType      ICpolarity;         /*!< Specifies the active edge of the input signal. */

    Tmr_IcSelectionType     ICselection;        /*!<  Specifies the input. */

    Tmr_IcPrescalerType     ICprescaler;        /*!<  Specifies the Input Capture Prescaler. */

    uint16_t                ICfilter;           /*!< Specifies the input capture filter. */

} Tmr_IcConfigType;

/**@} end of group TMR_Structures*/

/** @defgroup  TMR_Functions Functions
  @{
*/
/* TimeBase management */
void Tmr_Reset(const TMR_T* TMRx);
void Tmr_ConfigTimeBase(TMR_T* TMRx, const Tmr_TimeBaseType* TimeBaseConfig);
void Tmr_ConfigTimeBaseStruct(Tmr_TimeBaseType* TimeBaseConfig);
void Tmr_ConfigDIV(TMR_T* TMRx, uint16_t Div, Tmr_PrescalerReloadType Mode);
void Tmr_ConfigCounterMode(TMR_T* TMRx, Tmr_CounterModeType Mode);
void Tmr_SetCounter(TMR_T* TMRx, uint32_t Counter);
void Tmr_SetAutoReload(TMR_T* TMRx, uint32_t AutoReload);
uint32_t Tmr_ReadCounter(const TMR_T* TMRx);
uint32_t Tmr_ReadDiv(const TMR_T* TMRx);
void Tmr_EnableNGUpdate(TMR_T* TMRx);
void Tmr_DisableNGUpdate(TMR_T* TMRx);
void Tmr_ConfigUPdateRequest(TMR_T* TMRx, Tmr_UpdateSourceType Source);
void Tmr_SetClockDivision(TMR_T* TMRx, Tmr_CkdType ClockDivision);
void Tmr_Enable(TMR_T* TMRx);
void Tmr_Disable(TMR_T* TMRx);
void Tmr1_ConfigBDT(const Tmr_BdtInitType* Structure);
void Tmr_ConfigBDTStructInit(Tmr_BdtInitType* Structure);

void Tmr1_EnablePWMOutputs(void);
void Tmr1_DisablePWMOutputs(void);

void Tmr_OC1Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct);
void Tmr_OC2Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct);
void Tmr_OC3Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct);
void Tmr_OC4Config(TMR_T* TMRx, const Tmr_OcConfigType* OCcongigStruct);
void Tmr_OCConfigStructInit(Tmr_OcConfigType* OCcongigStruct);

void Tmr_SelectOCxMode(TMR_T* TMRx, Tmr_ChannelType Channel, Tmr_OcModeType Mode);
void Tmr_SelectSlaveMode(TMR_T* TMRx, Tmr_SlavemodeType Mode);
void Tmr_SelectOnePulseMode(TMR_T* TMRx, Tmr_OpmodeType OPMode);

void Tmr_SetCompare1(TMR_T* TMRx, uint32_t Compare);
void Tmr_SetCompare2(TMR_T* TMRx, uint32_t Compare);
void Tmr_SetCompare3(TMR_T* TMRx, uint32_t Compare);
void Tmr_SetCompare4(TMR_T* TMRx, uint32_t Compare);

void Tmr_ForcedOC1Config(TMR_T* TMRx, Tmr_ForcedActionType Action);
void Tmr_ForcedOC2Config(TMR_T* TMRx, Tmr_ForcedActionType Action);
void Tmr_ForcedOC3Config(TMR_T* TMRx, Tmr_ForcedActionType Action);
void Tmr_ForcedOC4Config(TMR_T* TMRx, Tmr_ForcedActionType Action);

void Tmr1_EnableCCPreload(void);
void Tmr1_DisableCCPreload(void);

void Tmr_OC1PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload);
void Tmr_OC2PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload);
void Tmr_OC3PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload);
void Tmr_OC4PreloadConfig(TMR_T* TMRx, Tmr_OcPreloadType OCPreload);

void Tmr_OC1FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast);
void Tmr_OC2FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast);
void Tmr_OC3FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast);
void Tmr_OC4FastConfit(TMR_T* TMRx, Tmr_OcFastType OCFast);

void Tmr1_ClearOC1Ref(Tmr_OcclerType OCCler);
void Tmr1_ClearOC2Ref(Tmr_OcclerType OCCler);
void Tmr1_ClearOC3Ref(Tmr_OcclerType OCCler);
void Tmr1_ClearOC4Ref(Tmr_OcclerType OCCler);

void Tmr_OC1PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity);
void Tmr_OC1NPolarityConfig(TMR_T* TMRx, Tmr_OcNpolarityType OCNPolarity);
void Tmr_OC2PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity);
void Tmr_OC2NPolarityConfig(TMR_T* TMRx, Tmr_OcNpolarityType OCNPolarity);
void Tmr_OC3PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity);
void Tmr_OC3NPolarityConfig(TMR_T* TMRx, Tmr_OcNpolarityType OCNPolarity);
void Tmr_OC4PolarityConfig(TMR_T* TMRx, Tmr_OcPolarityType OCPolarity);

void Tmr1_SelectOCREFClear(Tmr_OccselType OCReferenceClear);

void Tmr_EnableCCxChannel(TMR_T* TMRx, Tmr_ChannelType Channel);
void Tmr_DisableCCxChannel(TMR_T* TMRx, Tmr_ChannelType Channel);
void Tmr_EnableCCxNChannel(TMR_T* TMRx, Tmr_ChannelType Channel);
void Tmr_DisableCCxNChannel(TMR_T* TMRx, Tmr_ChannelType Channel);

void Tmr_EnableAUTOReload(TMR_T* TMRx);
void Tmr_DisableAUTOReload(TMR_T* TMRx);
void Tmr1_EnableSelectCOM(void);
void Tmr1_DisableSelectCOM(void);

void Tmr_ICConfig(TMR_T* TMRx, const Tmr_IcConfigType* ICconfigstruct);
void Tmr_ICConfigStructInit(Tmr_IcConfigType* ICconfigstruct);

void Tmr_PWMConfig(TMR_T* TMRx, const Tmr_IcConfigType* ICconfigstruct);

uint16_t Tmr_ReadCaputer1(const TMR_T* TMRx);
uint16_t Tmr_ReadCaputer2(const TMR_T* TMRx);
uint16_t Tmr_ReadCaputer3(const TMR_T* TMRx);
uint16_t Tmr_ReadCaputer4(const TMR_T* TMRx);

void Tmr_SetIC1Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler);
void Tmr_SetIC2Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler);
void Tmr_SetIC3Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler);
void Tmr_SetIC4Prescal(TMR_T* TMRx, Tmr_IcPrescalerType Prescaler);

/* Interrupts and Event management functions */
void Tmr_EnableInterrupt(TMR_T* TMRx, uint16_t Interrupt);
void Tmr_DisableInterrupt(TMR_T* TMRx, uint16_t Interrupt);
void Tmr_GenerateEvent(TMR_T* TMRx, uint16_t Event);

uint16_t Tmr_ReadStatusFlag(const TMR_T* TMRx, Tmr_FlagType Flag);
void Tmr_ClearStatusFlag(TMR_T* TMRx, uint16_t Flag);
uint16_t Tmr_ReadIntFlag(const TMR_T* TMRx,  Tmr_IntFlagType Flag);
void Tmr_ClearIntFlag(TMR_T* TMRx,  uint16_t Flag);

void Tmr_ConfigDMA(TMR_T* TMRx, Tmr_DmaBaseAdderssType Address, Tmr_DmaBaseLenghtType Lenght);
void Tmr_EnableDMASoure(TMR_T* TMRx, uint16_t Souces);
void Tmr_DisableDMASoure(TMR_T* TMRx, uint16_t Souces);
void Tmr_EnableCCDMA(TMR_T* TMRx);
void Tmr_DisableCCDMA(TMR_T* TMRx);

/* Clocks management */
void Tmr_ConfigInternalClock(TMR_T* TMRx);
void Tmr_ConfigITRxExternalClock(TMR_T* TMRx, Tmr_InputTriggerSourceType Input);
void Tmr_ConfigTIxExternalClock(TMR_T* TMRx, Tmr_InputTriggerSourceType Input,
                                Tmr_IcPolarityType ICpolarity, uint16_t ICfilter);
void Tmr_ConfigExternalClockMode1(TMR_T* TMRx, Tmr_ExttrgPrescalerType Prescaler,
                                  Tmr_ExttrgPolarityType Polarity, uint16_t Filter);
void Tmr1_ConfigExternalClockMode2(Tmr_ExttrgPrescalerType Prescaler,
                                  Tmr_ExttrgPolarityType Polarity, uint16_t Filter);
/* Synchronization management */
void Tmr_SelectInputTrigger(TMR_T* TMRx, Tmr_InputTriggerSourceType Input);
void Tmr_SelectOutputTrigger(TMR_T* TMRx, Tmr_TrgosourceType Source);
void Tmr_EnableMasterSlaveMode(TMR_T* TMRx);
void Tmr_DisableMasterSlaveMode(TMR_T* TMRx);
void Tmr1_ConfigExternalTrigger(Tmr_ExttrgPrescalerType Prescaler,
                               Tmr_ExttrgPolarityType Polarity, uint16_t Filter);

/* Specific interface management */
void Tmr_ConfigEncodeInterface(TMR_T* TMRx, Tmr_EncoderModeType EncodeMode, Tmr_IcPolarityType IC1Polarity,
                               Tmr_IcPolarityType IC2Polarity);
void Tmr_EnableHallSensor(TMR_T* TMRx);
void Tmr_DisableHallSensor(TMR_T* TMRx);

/* Specific remapping management */
void Tmr3_ConfigRemap(Tmr_RemapType Remap);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_TMR_H */

/**@} end of group TMR_Functions */
/**@} end of group TMR_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
