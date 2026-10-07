/*!
 * @file        g32a10xx_adc.h
 *
 * @brief       This file contains all the functions prototypes for the ADC firmware library
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

/*
* MISRA-C:2012 compliance checks
*
*       g32a10xx_adc_h_MISRA_REF1
*          Breaks the required Dir-1.1 of MISRA 2012 guidelines,
*          Any implementation-defined behaviour on which the output of the program depends shall be documented and understood
*
*       g32a10xx_adc_h_MISRA_REF2
           Breaks the required Rule-7.2 of MISRA 2012 guidelines,
*          A "u" or "U" suffix shall be applied to all integer constants that are represented in an unsigned type
*/

/* Define to prevent recursive inclusion */
#ifndef G32A10xx_ADC_H
#define G32A10xx_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup ADC_Driver
  @{
*/

/** @defgroup ADC_Macros Macros
  @{
*/

/* ADC_channels */
#define  ADC_CHANNEL_TEMPSENSOR   ((uint32_t)ADC_CHANNEL_16)  /*!< ADC TempSensor Channel definition */
#define  ADC_CHANNEL_VREFINT      ((uint32_t)ADC_CHANNEL_17)  /*!< ADC Vrefint Channel definition */
#define  ADC_CHANNEL_VBAT         ((uint32_t)ADC_CHANNEL_18)  /*!< ADC Vbat Channel definition */

/* ADC CFG mask */
#define CFG1_CLEAR_MASK           ((uint32_t)0xFFFFD203)

/* Calibration time out */
#define CALIBRATION_TIMEOUT       ((uint32_t)0x0000F000)

/**@} end of group ADC_Macros */

/** @defgroup ADC_Enumerations Enumerations
  @{
*/

/**
 * @brief   ADC conversion mode
 */
typedef enum
{
    ADC_CONVERSION_SINGLE        = ((uint8_t)0),  /*!< Single conversion mode */
    ADC_CONVERSION_CONTINUOUS    = ((uint8_t)1)   /*!< Continuous conversion mode */
} Adc_ConversionType;

/**
 * @brief    ADC Jitter
 */
typedef enum
{
    ADC_JITTER_PCLKDIV2   = ((uint8_t)0x01),         /*!< ADC clocked by PCLK div2 */
    ADC_JITTER_PCLKDIV4   = ((uint8_t)0x02)          /*!< ADC clocked by PCLK div4 */
} Adc_JitterType;

/**
 * @brief    ADC clock mode
 */
typedef enum
{
    ADC_CLOCK_MODE_ASYNCLK      = ((uint8_t)0x00),   /*!< ADC Asynchronous clock mode */
    ADC_CLOCK_MODE_SYNCLKDIV2   = ((uint8_t)0x01),   /*!< Synchronous clock mode divided by 2 */
    ADC_CLOCK_MODE_SYNCLKDIV4   = ((uint8_t)0x02)    /*!< Synchronous clock mode divided by 4 */
} Adc_ClockModeType;

/**
 * @brief    ADC data resolution
 */
typedef enum
{
    ADC_RESOLUTION_12B   = ((uint8_t)0x00),     /*!< ADC Resolution is 12 bits */
    ADC_RESOLUTION_10B   = ((uint8_t)0x01),     /*!< ADC Resolution is 10 bits */
    ADC_RESOLUTION_8B    = ((uint8_t)0x02),     /*!< ADC Resolution is 8 bits */
    ADC_RESOLUTION_6B    = ((uint8_t)0x03)      /*!< ADC Resolution is 6 bits */
} Adc_ResolutionType;

/**
 * @brief   ADC data alignment
 */
typedef enum
{
    ADC_DATA_ALIGN_RIGHT    = ((uint8_t)0), /*!< Data alignment right */
    ADC_DATA_ALIGN_LEFT     = ((uint8_t)1)  /*!< Data alignment left */
} Adc_DataAlignType;

/**
 * @brief   ADC scan sequence direction
 */
typedef enum
{
    ADC_SCAN_DIR_UPWARD     = ((uint8_t)0),     /*!< from CHSEL0 to CHSEL17 */
    ADC_SCAN_DIR_BACKWARD   = ((uint8_t)1)      /*!< from CHSEL17 to CHSEL0 */
} Adc_ScanDirType;

/**
 * @brief   ADC DMA Mode
 */
typedef enum
{
    ADC_DMA_MODE_ONESHOUT   = ((uint8_t)0),     /*!< ADC DMA Mode Select one shot */
    ADC_DMA_MODE_CIRCULAR   = ((uint8_t)1)      /*!< ADC DMA Mode Select circular */
} Adc_DmaModeType;

/**
 * @brief   ADC external conversion trigger edge selectio
 */
typedef enum
{
    ADC_EXT_TRIG_EDGE_NONE     = ((uint8_t)0x00),   /*!< ADC External Trigger Conversion mode disabled */
    ADC_EXT_TRIG_EDGE_RISING   = ((uint8_t)0x01),   /*!< ADC External Trigger Conversion mode rising edge */
    ADC_EXT_TRIG_EDGE_FALLING  = ((uint8_t)0x02),   /*!< ADC External Trigger Conversion mode falling edge */
    ADC_EXT_TRIG_EDGE_ALL      = ((uint8_t)0x03)    /*!< ADC External Trigger Conversion mode rising and falling edges */
} Adc_ExtTrigEdgeType;

/**
 * @brief   ADC external trigger sources selection
 */
typedef enum
{
    ADC_EXT_TRIG_CONV_TRG0   = ((uint8_t)0x00),     /*!< ADC External Trigger Conversion timer1 TRG0 */
    ADC_EXT_TRIG_CONV_TRG1   = ((uint8_t)0x01),     /*!< ADC External Trigger Conversion timer1 CC4 */
    ADC_EXT_TRIG_CONV_TRG2   = ((uint8_t)0x02),     /*!< ADC External Trigger Conversion timer2 TRGO */
    ADC_EXT_TRIG_CONV_TRG3   = ((uint8_t)0x03),     /*!< ADC External Trigger Conversion timer3 TRG0 */
    ADC_EXT_TRIG_CONV_TRG4   = ((uint8_t)0x04)      /*!< ADC External Trigger Conversion timer4 TRG0 */
} Adc_ExtTrigConvType;

/**
 * @brief   ADC analog watchdog channel selection
 */
typedef enum
{
    ADC_ANALG_WDT_CHANNEL_0    = ((uint8_t)0x00),   /*!< AWD Channel 0 */
    ADC_ANALG_WDT_CHANNEL_1    = ((uint8_t)0x01),   /*!< AWD Channel 1 */
    ADC_ANALG_WDT_CHANNEL_2    = ((uint8_t)0x02),   /*!< AWD Channel 2 */
    ADC_ANALG_WDT_CHANNEL_3    = ((uint8_t)0x03),   /*!< AWD Channel 3 */
    ADC_ANALG_WDT_CHANNEL_4    = ((uint8_t)0x04),   /*!< AWD Channel 4 */
    ADC_ANALG_WDT_CHANNEL_5    = ((uint8_t)0x05),   /*!< AWD Channel 5 */
    ADC_ANALG_WDT_CHANNEL_6    = ((uint8_t)0x06),   /*!< AWD Channel 6 */
    ADC_ANALG_WDT_CHANNEL_7    = ((uint8_t)0x07),   /*!< AWD Channel 7 */
    ADC_ANALG_WDT_CHANNEL_8    = ((uint8_t)0x08),   /*!< AWD Channel 8 */
    ADC_ANALG_WDT_CHANNEL_9    = ((uint8_t)0x09),   /*!< AWD Channel 9 */
    ADC_ANALG_WDT_CHANNEL_10   = ((uint8_t)0x0A),   /*!< AWD Channel 10 */
    ADC_ANALG_WDT_CHANNEL_11   = ((uint8_t)0x0B),   /*!< AWD Channel 11 */
    ADC_ANALG_WDT_CHANNEL_12   = ((uint8_t)0x0C),   /*!< AWD Channel 12 */
    ADC_ANALG_WDT_CHANNEL_13   = ((uint8_t)0x0D),   /*!< AWD Channel 13 */
    ADC_ANALG_WDT_CHANNEL_14   = ((uint8_t)0x0E),   /*!< AWD Channel 14 */
    ADC_ANALG_WDT_CHANNEL_15   = ((uint8_t)0x0F),   /*!< AWD Channel 15 */
    ADC_ANALG_WDT_CHANNEL_16   = ((uint8_t)0x10),   /*!< AWD Channel 16 */
    ADC_ANALG_WDT_CHANNEL_17   = ((uint8_t)0x11),   /*!< AWD Channel 17 */
    ADC_ANALG_WDT_CHANNEL_18   = ((uint8_t)0x12)    /*!< AWD Channel 18 */
} Adc_AnalgWdtChannelType;

/**
 * @brief   ADC sampling times
 */
typedef enum
{
    ADC_SAMPLE_TIME_1_5     = ((uint8_t)0x00),  /*!< 1.5   ADC clock cycles */
    ADC_SAMPLE_TIME_7_5     = ((uint8_t)0x01),  /*!< 7.5   ADC clock cycles */
    ADC_SAMPLE_TIME_13_5    = ((uint8_t)0x02),  /*!< 13.5  ADC clock cycles */
    ADC_SAMPLE_TIME_28_5    = ((uint8_t)0x03),  /*!< 28.5  ADC clock cycles */
    ADC_SAMPLE_TIME_41_5    = ((uint8_t)0x04),  /*!< 41.5  ADC clock cycles */
    ADC_SAMPLE_TIME_55_5    = ((uint8_t)0x05),  /*!< 55.5  ADC clock cycles */
    ADC_SAMPLE_TIME_71_5    = ((uint8_t)0x06),  /*!< 71.5  ADC clock cycles */
    ADC_SAMPLE_TIME_239_5   = ((uint8_t)0x07)   /*!< 239.5 ADC clock cycles */
} Adc_SampleTimeType;

/**
 * @brief   ADC channel selection
 */
typedef enum
{
    ADC_CHANNEL_0    = ((uint32_t)0x00000001),  /*!< ADC Channel 0 */
    ADC_CHANNEL_1    = ((uint32_t)0x00000002),  /*!< ADC Channel 1 */
    ADC_CHANNEL_2    = ((uint32_t)0x00000004),  /*!< ADC Channel 2 */
    ADC_CHANNEL_3    = ((uint32_t)0x00000008),  /*!< ADC Channel 3 */
    ADC_CHANNEL_4    = ((uint32_t)0x00000010),  /*!< ADC Channel 4 */
    ADC_CHANNEL_5    = ((uint32_t)0x00000020),  /*!< ADC Channel 5 */
    ADC_CHANNEL_6    = ((uint32_t)0x00000040),  /*!< ADC Channel 6 */
    ADC_CHANNEL_7    = ((uint32_t)0x00000080),  /*!< ADC Channel 7 */
    ADC_CHANNEL_8    = ((uint32_t)0x00000100),  /*!< ADC Channel 8 */
    ADC_CHANNEL_9    = ((uint32_t)0x00000200),  /*!< ADC Channel 9 */
    ADC_CHANNEL_10   = ((uint32_t)0x00000400),  /*!< ADC Channel 10 */
    ADC_CHANNEL_11   = ((uint32_t)0x00000800),  /*!< ADC Channel 11 */
    ADC_CHANNEL_12   = ((uint32_t)0x00001000),  /*!< ADC Channel 12 */
    ADC_CHANNEL_13   = ((uint32_t)0x00002000),  /*!< ADC Channel 13 */
    ADC_CHANNEL_14   = ((uint32_t)0x00004000),  /*!< ADC Channel 14 */
    ADC_CHANNEL_15   = ((uint32_t)0x00008000),  /*!< ADC Channel 15 */
    ADC_CHANNEL_16   = ((uint32_t)0x00010000),  /*!< ADC Channel 16 */
    ADC_CHANNEL_17   = ((uint32_t)0x00020000),  /*!< ADC Channel 17 */
    ADC_CHANNEL_18   = ((uint32_t)0x00040000)   /*!< ADC Channel 18 */
} Adc_ChannleType;

/**
 * @brief   ADC interrupts definition
 */
typedef enum
{
    ADC_INT_ADRDY    = ((uint8_t)0x01), /*!< ADC ready interrupt */
    ADC_INT_CSMP     = ((uint8_t)0x02), /*!< End of sampling interrupt */
    ADC_INT_CC       = ((uint8_t)0x04), /*!< End of conversion interrupt */
    ADC_INT_CS       = ((uint8_t)0x08), /*!< End of sequence interrupt */
    ADC_INT_OVR      = ((uint8_t)0x10), /*!< ADC overrun interrupt */
    ADC_INT_AWD      = ((uint8_t)0x80)  /*!< Analog watchdog interrupt */
} Adc_IntType;

/**
 * @brief   ADC Interrupt flag
 */
typedef enum
{
    ADC_INT_FLAG_ADRDY    = ((uint8_t)0x01),    /*!< ADC ready interrupt flag */
    ADC_INT_FLAG_CSMP     = ((uint8_t)0x02),    /*!< End of sampling interrupt flag */
    ADC_INT_FLAG_CC       = ((uint8_t)0x04),    /*!< End of conversion interrupt flag */
    ADC_INT_FLAG_CS       = ((uint8_t)0x08),    /*!< End of sequence interrupt flag */
    ADC_INT_FLAG_OVR      = ((uint8_t)0x10),    /*!< ADC overrun interrupt flag */
    ADC_INT_FLAG_AWD      = ((uint8_t)0x80)     /*!< Analog watchdog interrupt flag */
} Adc_IntFlagType;

/**
 * @brief   ADC flag
 */
typedef enum
{
    ADC_FLAG_ADCON   = ((uint32_t)0x01000001),  /*!< ADC enable flag */
    ADC_FLAG_ADCOFF  = ((uint32_t)0x01000002),  /*!< ADC disable flag */
    ADC_FLAG_ADCSTA  = ((uint32_t)0x01000004),  /*!< ADC start conversion flag */
    ADC_FLAG_ADCSTOP = ((uint32_t)0x01000010),  /*!< ADC stop conversion flag */
/*** @warning g32a10xx_adc_h_MISRA_REF1 Constant: Casting to a signed integer type of insufficient size. */
/*** @warning g32a10xx_adc_h_MISRA_REF2 Integer literal constant is of an unsigned type but does not include a "U" suffix. */
    ADC_FLAG_ADCCAL  = ((int)     0x81000000),  /*!< ADC calibration flag */
    ADC_FLAG_ADRDY   = ((uint8_t)0x01),         /*!< ADC ready flag */
    ADC_FLAG_CSMP    = ((uint8_t)0x02),         /*!< End of sampling flag */
    ADC_FLAG_CC      = ((uint8_t)0x04),         /*!< End of conversion flag */
    ADC_FLAG_CS      = ((uint8_t)0x08),         /*!< End of sequence flag */
    ADC_FLAG_OVR     = ((uint8_t)0x10),         /*!< ADC overrun flag */
    ADC_FLAG_AWD     = ((uint8_t)0x80)          /*!< Analog watchdog flag */
} Adc_FlagType;

/**@} end of group ADC_Enumerations */

/** @defgroup ADC_Structures Structures
  @{
*/

/**
 * @brief   ADC Config struct definition
 */
typedef struct
{
    Adc_ResolutionType      resolution;     /*!< Specifies the ADC data resolution */
    Adc_DataAlignType       dataAlign;      /*!< Specifies the data alignment mode */
    Adc_ScanDirType         scanDir;        /*!< Specifies the scan mode */
    Adc_ConversionType      convMode;       /*!< Specifies the conversion mode */
    Adc_ExtTrigConvType     extTrigConv;    /*!< Specifies the external trigger sources */
    Adc_ExtTrigEdgeType     extTrigEdge;    /*!< Specifies the external conversion trigger edge */
} Adc_ConfigType;

/**@} end of group ADC_Structures */

/** @defgroup ADC_Functions Functions
  @{
*/

/* ADC reset and configuration */
void Adc_Reset(void);
void Adc_Config(const Adc_ConfigType* adcConfigPtr);
void Adc_ConfigStructInit(Adc_ConfigType* adcConfigPtr);
void Adc_Enable(void);
void Adc_Disable(void);
void Adc_EnableAutoPowerOff(void);
void Adc_DisableAutoPowerOff(void);
void Adc_EnableWaitMode(void);
void Adc_DisableWaitMode(void);
void Adc_ConfigChannel(uint32_t channel, uint8_t sampleTime);
void Adc_EnableContinuousMode(void);
void Adc_DisableContinuousMode(void);
void Adc_EnableDiscMode(void);
void Adc_DisableDiscMode(void);
void Adc_EnableOverrunMode(void);
void Adc_DisableOverrunMode(void);
void Adc_StopConversion(void);
void Adc_StartConversion(void);
void Adc_DmaRequestMode(Adc_DmaModeType dmaRequestMode);

/* ADC clock and jitter */
void Adc_ClockMode(Adc_ClockModeType clockMode);
void Adc_EnableJitter(Adc_JitterType jitter);
void Adc_DisableJitter(Adc_JitterType jitter);

/* ADC analog watchdog */
void Adc_EnableAnalogWatchdog(void);
void Adc_DisableAnalogWatchdog(void);
void Adc_AnalogWatchdogLowThreshold(uint16_t lowThreshold);
void Adc_AnalogWatchdogHighThreshold(uint16_t highThreshold);
void Adc_AnalogWatchdogSingleChannel(uint32_t channel);
void Adc_EnableAnalogWatchdogSingleChannel(void);
void Adc_DisableAnalogWatchdogSingleChannel(void);

/* ADC common configuration */
void Adc_EnableTempSensor(void);
void Adc_DisableTempSensor(void);
void Adc_EnableVrefint(void);
void Adc_DisableVrefint(void);
void Adc_EnableVbat(void);
void Adc_DisableVbat(void);

/* Read data  */
uint32_t Adc_ReadCalibrationFactor(void);
uint16_t Adc_ReadConversionValue(void);

/* DMA */
void Adc_EnableDma(void);
void Adc_DisableDma(void);

/* Interrupt and flag */
void Adc_EnableInterrupt(uint8_t interrupt);
void Adc_DisableInterrupt(uint8_t interrupt);
uint8_t Adc_ReadStatusFlag(Adc_FlagType flag);
void Adc_ClearStatusFlag(uint32_t flag);
uint8_t Adc_ReadIntFlag(Adc_IntFlagType flag);
void Adc_ClearIntFlag(uint32_t flag);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_ADC_H */

/**@} end of group ADC_Functions */
/**@} end of group ADC_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
