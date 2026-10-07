/*!
 * @file        g32a10xx_adc.c
 *
 * @brief       This file contains all the functions for the ADC peripheral
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

/* Includes */
#include "g32a10xx_adc.h"
#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup ADC_Driver
  @{
*/

/** @defgroup ADC_Functions Functions
  @{
*/

/*!
 * @brief     Set the ADC peripheral registers to their default reset values
 *
 * @param     None
 *
 * @retval    None
 */
void Adc_Reset(void)
{
    Rcm_EnableApb2PeriphReset(RCM_APB2_PERIPH_ADC1);
    Rcm_DisableApb2PeriphReset(RCM_APB2_PERIPH_ADC1);
}

/*!
 * @brief       Config the ADC peripheral according to the specified parameters in the adcConfig
 *
 * @param       adcConfig:  Pointer to a Adc_ConfigType structure that
 *                          contains the configuration information for the ADC peripheral
 *
 * @retval      None
 */
void Adc_Config(const Adc_ConfigType* adcConfigPtr)
{
    ADC->CFG1_R.CFG1_B.DATARESCFG = (uint8_t)(adcConfigPtr->resolution);
    ADC->CFG1_R.CFG1_B.DALIGCFG   = (uint8_t)(adcConfigPtr->dataAlign);
    ADC->CFG1_R.CFG1_B.SCANSEQDIR = (uint8_t)(adcConfigPtr->scanDir);
    ADC->CFG1_R.CFG1_B.CMODESEL   = (uint8_t)(adcConfigPtr->convMode);
    ADC->CFG1_R.CFG1_B.EXTPOLSEL  = (uint8_t)(adcConfigPtr->extTrigEdge);
    ADC->CFG1_R.CFG1_B.EXTTRGSEL  = (uint8_t)(adcConfigPtr->extTrigConv);
}

/*!
 * @brief       Fills each adcConfig member with its default value
 *
 * @param       adcConfig:  Pointer to a Adc_ConfigType structure which will be initialized
 *
 * @retval      None
 */
void Adc_ConfigStructInit(Adc_ConfigType* adcConfigPtr)
{
    adcConfigPtr->resolution  = ADC_RESOLUTION_12B;
    adcConfigPtr->dataAlign   = ADC_DATA_ALIGN_RIGHT;
    adcConfigPtr->scanDir     = ADC_SCAN_DIR_UPWARD;
    adcConfigPtr->convMode    = ADC_CONVERSION_SINGLE;
    adcConfigPtr->extTrigConv = ADC_EXT_TRIG_CONV_TRG0;
    adcConfigPtr->extTrigEdge = ADC_EXT_TRIG_EDGE_NONE;
}

/*!
 * @brief       Enable the ADC peripheral
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_Enable(void)
{
    ADC->CTRL_R.CTRL_B.ADCEN = BIT_SET;
}

/*!
 * @brief       Disable the ADC peripheral
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_Disable(void)
{
    ADC->CTRL_R.CTRL_B.ADCD = BIT_SET;
}

/*!
 * @brief       Configure the ADC to either be clocked by the asynchronous clock
 *
 * @param       clockmode: selects the ADC clock mode.
 *                         The parameter can be one of following values:
 *                         @arg ADC_CLOCK_MODE_ASYNCLK:       ADC Asynchronous clock mode
 *                         @arg ADC_CLOCK_MODE_SYNCLKDIV2:    Synchronous clock mode divided by 2
 *                         @arg ADC_CLOCK_MODE_SYNCLKDIV4:    Synchronous clock mode divided by 4
 *
 * @retval      None
 */
void Adc_ClockMode(Adc_ClockModeType clockMode)
{
    ADC->CFG2_R.CFG2_B.CLKCFG = (uint32_t)clockMode;
}

/*!
 * @brief       Enables the jitter when the ADC is clocked by PCLK div2 or div4
 *
 * @param       jitter: They are replaced by PCLK div2 or div4
 *                      The parameter can be one of following values:
 *                      @arg ADC_JITTER_PCLKDIV2:    ADC clocked by PCLK div2
 *                      @arg ADC_JITTER_PCLKDIV4:    ADC clocked by PCLK div4
 *
 * @retval      None
 */
void Adc_EnableJitter(Adc_JitterType jitter)
{
    ADC->CFG2_R.CFG2_B.CLKCFG |= (uint8_t)jitter;
}

/*!
 * @brief       Disables the jitter when the ADC is clocked by PCLK div2 or div4
 *
 * @param       jitter: They are replaced by PCLK div2 or div4
 *                      The parameter can be one of following values:
 *                      @arg ADC_JITTER_PCLKDIV2:    ADC clocked by PCLK div2
 *                      @arg ADC_JITTER_PCLKDIV4:    ADC clocked by PCLK div4
 * @retval      None
 */
void Adc_DisableJitter(Adc_JitterType jitter)
{
    ADC->CFG2_R.CFG2_B.CLKCFG &= (~(uint8_t)jitter);
}

/*!
 * @brief       Enables the Auto Power Off mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableAutoPowerOff(void)
{
    ADC->CFG1_R.CFG1_B.AOEN = BIT_SET;
}

/*!
 * @brief       Disables the Auto Power Off mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableAutoPowerOff(void)
{
    ADC->CFG1_R.CFG1_B.AOEN = BIT_RESET;
}

/*!
 * @brief       Enables the Auto-delayed conversion mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableWaitMode(void)
{
    ADC->CFG1_R.CFG1_B.WAITCEN = BIT_SET;
}

/*!
 * @brief       Disables the Auto-delayed conversion mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableWaitMode(void)
{
    ADC->CFG1_R.CFG1_B.WAITCEN = BIT_RESET;
}

/*!
 * @brief       Enables the analog watchdog
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableAnalogWatchdog(void)
{
    ADC->CFG1_R.CFG1_B.AWDEN = BIT_SET;
}

/*!
 * @brief       Disables the analog watchdog
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableAnalogWatchdog(void)
{
    ADC->CFG1_R.CFG1_B.AWDEN = BIT_RESET;
}

/*!
 * @brief       The analog watchdog low threshold
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_AnalogWatchdogLowThreshold(uint16_t lowThreshold)
{
    ADC->AWDT_R.AWDT_B.AWDLT = (uint16_t)lowThreshold;
}

/*!
 * @brief       The analog watchdog High threshold
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_AnalogWatchdogHighThreshold(uint16_t highThreshold)
{
    ADC->AWDT_R.AWDT_B.AWDHT = (uint16_t)highThreshold;
}

/*!
 * @brief       Configures the analog watchdog guarded single channel
 *
 * @param       channel:    ADC analog watchdog channel selection
 *                          The parameter can be one of following values:
 *                          @arg ADC_ANALG_WDT_CHANNEL_0: AWD Channel 0
 *                          @arg ADC_ANALG_WDT_CHANNEL_1: AWD Channel 1
 *                          @arg ADC_ANALG_WDT_CHANNEL_2: AWD Channel 2
 *                          @arg ADC_ANALG_WDT_CHANNEL_3: AWD Channel 3
 *                          @arg ADC_ANALG_WDT_CHANNEL_4: AWD Channel 4
 *                          @arg ADC_ANALG_WDT_CHANNEL_5: AWD Channel 5
 *                          @arg ADC_ANALG_WDT_CHANNEL_6: AWD Channel 6
 *                          @arg ADC_ANALG_WDT_CHANNEL_7: AWD Channel 7
 *                          @arg ADC_ANALG_WDT_CHANNEL_8: AWD Channel 8
 *                          @arg ADC_ANALG_WDT_CHANNEL_9: AWD Channel 9
 *                          @arg ADC_ANALG_WDT_CHANNEL_10: AWD Channel 10
 *                          @arg ADC_ANALG_WDT_CHANNEL_11: AWD Channel 11
 *                          @arg ADC_ANALG_WDT_CHANNEL_12: AWD Channel 12
 *                          @arg ADC_ANALG_WDT_CHANNEL_13: AWD Channel 13
 *                          @arg ADC_ANALG_WDT_CHANNEL_14: AWD Channel 14
 *                          @arg ADC_ANALG_WDT_CHANNEL_15: AWD Channel 15
 *                          @arg ADC_ANALG_WDT_CHANNEL_16: AWD Channel 16 is TempSensor
 *                          @arg ADC_ANALG_WDT_CHANNEL_17: AWD Channel 17 is Vrefint
 *                          @arg ADC_ANALG_WDT_CHANNEL_18: AWD Channel 18 is Vbat
 *
 * @retval      None
 */
void Adc_AnalogWatchdogSingleChannel(uint32_t channel)
{
    ADC->CFG1_R.CFG1_B.AWDCHSEL = channel;
}

/*!
 * @brief       Enables the Analog Watchdog Single Channel
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableAnalogWatchdogSingleChannel(void)
{
    ADC->CFG1_R.CFG1_B.AWDCHEN = BIT_SET;
}

/*!
 * @brief       Disables the Analog Watchdog Single Channel
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableAnalogWatchdogSingleChannel(void)
{
    ADC->CFG1_R.CFG1_B.AWDCHEN = BIT_RESET;
}

/*!
 * @brief       Enables the temperature sensor channel
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableTempSensor(void)
{
    ADC->CCFG_R.CCFG_B.TSEN = BIT_SET;
}

/*!
 * @brief       Disables the temperature sensor channel
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableTempSensor(void)
{
    ADC->CCFG_R.CCFG_B.TSEN = BIT_RESET;
}

/*!
 * @brief       Enables the vrefint channel
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableVrefint(void)
{
    ADC->CCFG_R.CCFG_B.VREFEN = BIT_SET;
}

/*!
 * @brief       Disables the vrefint channel
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableVrefint(void)
{
    ADC->CCFG_R.CCFG_B.VREFEN = BIT_RESET;
}

/*!
 * @brief       Configures for the selected ADC channel and its sampling time
 *
 * @param       channel:    the ADC channel
 *                          The parameter can be combination of following values:
 *                          @arg ADC_CHANNEL_0: channel 0
 *                          @arg ADC_CHANNEL_1: channel 1
 *                          @arg ADC_CHANNEL_2: channel 2
 *                          @arg ADC_CHANNEL_3: channel 3
 *                          @arg ADC_CHANNEL_4: channel 4
 *                          @arg ADC_CHANNEL_5: channel 5
 *                          @arg ADC_CHANNEL_6: channel 6
 *                          @arg ADC_CHANNEL_7: channel 7
 *                          @arg ADC_CHANNEL_8: channel 8
 *                          @arg ADC_CHANNEL_9: channel 9
 *                          @arg ADC_CHANNEL_10: channel 10
 *                          @arg ADC_CHANNEL_11: channel 11
 *                          @arg ADC_CHANNEL_12: channel 12
 *                          @arg ADC_CHANNEL_13: channel 13
 *                          @arg ADC_CHANNEL_14: channel 14
 *                          @arg ADC_CHANNEL_15: channel 15
 *                          @arg ADC_CHANNEL_16: channel 16 is TempSensor
 *                          @arg ADC_CHANNEL_17: channel 17 is Vrefint
 * @param       sampleTime: the ADC sampling time
 *                          The parameter can be one of following values:
 *                          @arg ADC_SAMPLE_TIME_1_5:   ADC 1.5 clock cycles
 *                          @arg ADC_SAMPLE_TIME_7_5:   ADC 7.5 clock cycles
 *                          @arg ADC_SAMPLE_TIME_13_5:  ADC 13.5 clock cycles
 *                          @arg ADC_SAMPLE_TIME_28_5:  ADC 28.5 clock cycles
 *                          @arg ADC_SAMPLE_TIME_41_5:  ADC 41.5 clock cycles
 *                          @arg ADC_SAMPLE_TIME_55_5:  ADC 55.5 clock cycles
 *                          @arg ADC_SAMPLE_TIME_71_5:  ADC 71.5 clock cycles
 *                          @arg ADC_SAMPLE_TIME_239_5: ADC 239.5 clock cycles
 *
 * @retval      None
 */
void Adc_ConfigChannel(uint32_t channel, uint8_t sampleTime)
{
    ADC->CHSEL_R.CHSEL |= (uint32_t)channel;

    ADC->SMPTIM_R.SMPTIM |= (uint8_t)sampleTime;
}

/*!
 * @brief       Enables the continuous mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableContinuousMode(void)
{
    ADC->CFG1_R.CFG1_B.CMODESEL = BIT_SET;
}

/*!
 * @brief       Disables the continuous mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableContinuousMode(void)
{
    ADC->CFG1_R.CFG1_B.CMODESEL = BIT_RESET;
}

/*!
 * @brief       Enables the discontinuous mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableDiscMode(void)
{
    ADC->CFG1_R.CFG1_B.DISCEN = BIT_SET;
}

/*!
 * @brief       Disables the discontinuous mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableDiscMode(void)
{
    ADC->CFG1_R.CFG1_B.DISCEN = BIT_RESET;
}

/*!
 * @brief       Enables the Overrun mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableOverrunMode(void)
{
    ADC->CFG1_R.CFG1_B.OVRMAG = BIT_SET;
}

/*!
 * @brief       Disables the Overrun mode
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableOverrunMode(void)
{
    ADC->CFG1_R.CFG1_B.OVRMAG = BIT_RESET;
}

/*!
 * @brief       ADC Read Calibration Factor
 *
 * @param       None
 *
 * @retval      ADC Calibration factor
 */
uint32_t Adc_ReadCalibrationFactor(void)
{
    uint32_t temp = 0, counter = 0, status = 0;

    ADC->CTRL_R.CTRL_B.CAL = BIT_SET;

    do
    {
        status =  ADC->CTRL_R.CTRL_B.CAL;
        counter++;
    }
    while ((counter != CALIBRATION_TIMEOUT) && (status != 0x00U));

    if ((uint32_t)(ADC->CTRL_R.CTRL_B.CAL) == (uint32_t)RESET)
    {
        temp = ADC->DATA_R.DATA;
    }
    else
    {
        temp = 0x00000000;
    }

    return temp;
}

/*!
 * @brief       ADC stop conversion command
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_StopConversion(void)
{
    ADC->CTRL_R.CTRL_B.STOPCEN = BIT_SET;
}

/*!
 * @brief       ADC start conversion command
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_StartConversion(void)
{
    ADC->CTRL_R.CTRL_B.STARTCEN = BIT_SET;
}

/*!
 * @brief       Returns the last ADC conversion result data
 *
 * @param       None
 *
 * @retval      The Data conversion value
 */
uint16_t Adc_ReadConversionValue(void)
{
    return ((uint16_t)ADC->DATA_R.DATA);
}

/*!
 * @brief       Enables the ADC DMA request
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_EnableDma(void)
{
    ADC->CFG1_R.CFG1_B.DMAEN = BIT_SET;
}

/*!
 * @brief       Disables the ADC DMA request
 *
 * @param       None
 *
 * @retval      None
 */
void Adc_DisableDma(void)
{
    ADC->CFG1_R.CFG1_B.DMAEN = BIT_RESET;
}

/*!
 * @brief       ADC DMA Request Mode
 *
 * @param       DMARequestMode: Direct memery access configuration .
 *                              The parameter can be one of following values:
 *                              @arg ADC_DMA_MODE_ONESHOUT:    ADC DMA Mode Select one shot
 *                              @arg ADC_DMA_MODE_CIRCULAR:    ADC DMA Mode Select circular
 *
 * @retval      None
 */
void Adc_DmaRequestMode(Adc_DmaModeType dmaRequestMode)
{
    ADC->CFG1_R.CFG1_B.DMACFG = (uint8_t)dmaRequestMode;
}

/*!
 * @brief       Enables the specified interrupts
 *
 * @param       interrupt:  Specifies the ADC interrupts sources
 *                          The parameter can be combination of following values:
 *                          @arg ADC_INT_ADRDY:    ADC ready interrupt
 *                          @arg ADC_INT_CSMP:     End of sampling interrupt
 *                          @arg ADC_INT_CC:       End of conversion interrupt
 *                          @arg ADC_INT_CS:       End of sequence interrupt
 *                          @arg ADC_INT_OVR:      ADC overrun interrupt
 *                          @arg ADC_INT_AWD:      Analog watchdog interrupt
 *
 * @retval      None
 */
void Adc_EnableInterrupt(uint8_t interrupt)
{
    ADC->IEN_R.IEN |= (uint32_t)interrupt;
}

/*!
 * @brief       Disable the specified interrupts
 *
 * @param       interrupt:  Specifies the ADC interrupts sources
 *                          The parameter can be combination of following values:
 *                          @arg ADC_INT_ADRDY:    ADC ready interrupt
 *                          @arg ADC_INT_CSMP:     End of sampling interrupt
 *                          @arg ADC_INT_CC:       End of conversion interrupt
 *                          @arg ADC_INT_CS:       End of sequence interrupt
 *                          @arg ADC_INT_OVR:      ADC overrun interrupt
 *                          @arg ADC_INT_AWD:      Analog watchdog interrupt
 *
 * @retval      None
 */
void Adc_DisableInterrupt(uint8_t interrupt)
{
    ADC->IEN_R.IEN &= (uint8_t)~interrupt;
}

/*!
 * @brief       Checks whether the specified ADC flag is set or not
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be one of the following values:
 *                      @arg ADC_FLAG_ADCON:    ADC enable flag
 *                      @arg ADC_FLAG_ADCOFF:   ADC disable flag
 *                      @arg ADC_FLAG_ADCSTA:   ADC start conversion flag
 *                      @arg ADC_FLAG_ADCSTOP:  ADC stop conversion flag
 *                      @arg ADC_FLAG_ADCCAL:   ADC calibration flag
 *                      @arg ADC_FLAG_ADRDY:    ADC ready flag
 *                      @arg ADC_FLAG_CSMP:     End of sampling flag
 *                      @arg ADC_FLAG_CC:       End of conversion flag
 *                      @arg ADC_FLAG_CS:       End of sequence flag
 *                      @arg ADC_FLAG_OVR:      ADC overrun flag
 *                      @arg ADC_FLAG_AWD:      Analog watchdog flag
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t Adc_ReadStatusFlag(Adc_FlagType flag)
{
    uint32_t status = 0U;
    uint8_t ret = RESET;

    if (((uint32_t)flag & 0x01000000U) != 0U)
    {
        status = ADC->CTRL_R.CTRL & 0xFEFFFFFFU;
    }
    else
    {
        status = ADC->STS_R.STS;
    }

    if ((status & (uint32_t)flag) != (uint32_t)RESET)
    {
        ret = SET;
    }
    else
    {
        /* nothing */
    }

    return ret;
}

/*!
 * @brief       Clear the specified ADC flag
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be any combination of the following values:
 *                      @arg ADC_FLAG_ADRDY:    ADC ready flag
 *                      @arg ADC_FLAG_CSMP:     End of sampling flag
 *                      @arg ADC_FLAG_CC:       End of conversion flag
 *                      @arg ADC_FLAG_CS:       End of sequence flag
 *                      @arg ADC_FLAG_OVR:      ADC overrun flag
 *                      @arg ADC_FLAG_AWD:      Analog watchdog flag
 *
 * @retval      None
 */
void Adc_ClearStatusFlag(uint32_t flag)
{
    ADC->STS_R.STS = (uint32_t)flag;
}

/*!
 * @brief       Checks whether the specified interrupt has occurred or not
 *
 * @param       flag:   Specifies the ADC interrupt pending bit to check
 *                      The parameter can be one of following values:
 *                      @arg ADC_INT_FLAG_ADRDY:    ADC ready interrupt
 *                      @arg ADC_INT_FLAG_CSMP:     End of sampling interrupt
 *                      @arg ADC_INT_FLAG_CC:       End of conversion interrupt
 *                      @arg ADC_INT_FLAG_CS:       End of sequence interrupt
 *                      @arg ADC_INT_FLAG_OVR:      ADC overrun interrupt
 *                      @arg ADC_INT_FLAG_AWD:      Analog watchdog interrupt
 *
 * @retval      None
 */
uint8_t Adc_ReadIntFlag(Adc_IntFlagType flag)
{
    uint8_t intEnable = 0U;
    uint8_t intStatus = 0U;
    uint8_t ret = RESET;

    intEnable = (uint8_t)(ADC->IEN_R.IEN & (uint32_t)flag);

    intStatus = (uint8_t)(ADC->STS_R.STS & ((uint32_t)flag & 0xffU));

    if(intEnable != 0U)
    {
        if(intStatus != 0U)
        {
            ret = SET;
        }
        else
        {
            /*  nothing */
        }
    }
    else
    {
        /*  nothing */
    }

    return ret;
}

/*!
 * @brief       Clears the specified interrupt pending bits
 *
 * @param       flag:   Specifies the ADC interrupt pending bit to check
 *                      The parameter can be combination of following values:
 *                      @arg ADC_INT_FLAG_ADRDY:    ADC ready interrupt
 *                      @arg ADC_INT_FLAG_CSMP:     End of sampling interrupt
 *                      @arg ADC_INT_FLAG_CC:       End of conversion interrupt
 *                      @arg ADC_INT_FLAG_CS:       End of sequence interrupt
 *                      @arg ADC_INT_FLAG_OVR:      ADC overrun interrupt
 *                      @arg ADC_INT_FLAG_AWD:      Analog watchdog interrupt
 *
 * @retval      None
 */

void Adc_ClearIntFlag(uint32_t flag)
{
    ADC->STS_R.STS = flag;
}

/**@} end of group ADC_Functions */
/**@} end of group ADC_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
