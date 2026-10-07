/*!
 * @file        g32a10xx_rtc.c
 *
 * @brief       This file provides firmware functions to manage the following
 *              functionalities of the Real-Time Clock (RTC) peripheral
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

#include "g32a10xx_rtc.h"
#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup RTC_Driver
  @{
*/

/** @defgroup RTC_Functions Functions
  @{
*/

static uint8_t Rtc_ByteConBcd2(uint8_t val);
static uint8_t Rtc_Bcd2ConByte(uint8_t val);

/*!
 * @brief       Deinitializes the RTC registers to their default reset values
 *
 * @param       None
 *
 * @retval      SUCCESS or ERROR
 */
uint8_t Rtc_Reset(void)
{
    uint8_t ret = (uint8_t)ERROR;
    Rtc_DisableWriteProtection();

    if (Rtc_EnableInit() == (uint8_t)ERROR)
    {
        Rtc_EnableWriteProtection();
        ret = ERROR;
    }
    else
    {
        RTC->TIME_R.TIME     = (uint32_t)0x00000000;
        RTC->DATE_R.DATE     = (uint32_t)0x00002101;
        RTC->CTRL_R.CTRL    &= (uint32_t)0x00000000;
        RTC->PSC_R.PSC      = (uint32_t)0x007F00FF;
        RTC->ALRMA_R.ALRMA    = (uint32_t)0x00000000;
        RTC->SHIFT_R.SHIFT    = (uint32_t)0x00000000;
        RTC->CAL_R.CAL      = (uint32_t)0x00000000;
        RTC->ALRMASS_R.ALRMASS  = (uint32_t)0x00000000;

        RTC->STS_R.STS = (uint32_t)0x00000007;

        if (Rtc_WaitForSynchro() == (uint8_t)ERROR)
        {
            Rtc_EnableWriteProtection();
            ret = ERROR;
        }
        else
        {
            Rtc_EnableWriteProtection();
            ret = SUCCESS;
        }
    }

    return ret;
}

/*!
 * @brief       Deinitializes the RTC registers to their default reset values
 *
 * @param       StructPtr : pointer to a Rtc_ConfigType structure which will be initialized
 *
 * @retval      SUCCESS or ERROR
 */
uint8_t Rtc_Config(const Rtc_ConfigType* StructPtr)
{
    uint8_t ret = (uint8_t)ERROR;
    Rtc_DisableWriteProtection();

    if (Rtc_EnableInit() == (uint8_t)ERROR)
    {
        Rtc_EnableWriteProtection();
        ret =  ERROR;
    }
    else
    {
        RTC->CTRL_R.CTRL_B.TIMEFCFG  = (uint32_t)(StructPtr->format);
        RTC->PSC_R.PSC_B.SPSC = (StructPtr->SynchPrediv);
        RTC->PSC_R.PSC_B.APSC = (StructPtr->AsynchPrediv);
        Rtc_DisableInit();
        Rtc_EnableWriteProtection();
        ret =  SUCCESS;
    }

    return ret;
}

/*!
 * @brief       Fills each RTC_ConfigStruct member with its default value
 *
 * @param       StructPtr : pointer to a Rtc_ConfigType structure which will be initialized
 *
 * @retval      None
 */
void Rtc_ConfigStructInit(Rtc_ConfigType* StructPtr)
{
    StructPtr->format = RTC_HOURFORMAT_24;
    StructPtr->AsynchPrediv = (uint32_t)0x7F;
    StructPtr->SynchPrediv = (uint32_t)0xFF;
}

/*!
 * @brief       Enable the write protection for RTC registers
 *
 * @param       None
 *
 * @retval      None
 */
void Rtc_EnableWriteProtection(void)
{
    RTC->WRPROT_R.WRPROT = 0xFF;
}

/*!
 * @brief       Disable the write protection for RTC registers
 *
 * @param       None
 *
 * @retval      None
 */
void Rtc_DisableWriteProtection(void)
{
    RTC->WRPROT_R.WRPROT = 0xCA;
    RTC->WRPROT_R.WRPROT = 0x53;
}

/*!
 * @brief       Enable the RTC Initialization mode.
 *
 * @param       None
 *
 * @retval      SUCCESS or ERROR
 */
uint8_t Rtc_EnableInit(void)
{
    uint8_t ret = (uint8_t)ERROR;
    __IO uint32_t cnt = 0x00;
    uint32_t initstatus = 0x00;

    if (RTC->STS_R.STS_B.RINITFLG == (uint32_t)BIT_RESET)
    {
        RTC->STS_R.STS = (uint32_t)0xFFFFFFFFU;

        do
        {
            initstatus = RTC->STS_R.STS_B.RINITFLG;
            cnt = cnt + 1U;
        }
        while ((cnt != (uint32_t)RTC_INITMODE_TIMEOUT) && (initstatus == 0U));

        if (RTC->STS_R.STS_B.RINITFLG != (uint32_t)BIT_RESET)
        {
            ret =  SUCCESS;
        }
        else
        {
            ret =  ERROR;
        }
    }
    else
    {
        ret =  SUCCESS;
    }

    return ret;
}

/*!
 * @brief       Disable the RTC Initialization mode.
 *
 * @param       None
 *
 * @retval      None
 */
void Rtc_DisableInit(void)
{
    RTC->STS_R.STS_B.INITEN = BIT_RESET;
}

/*!
 * @brief       Waits until the RTC Time and Date registers (RTC_TIME and RTC_DATA) are
 *              synchronized with RTC APB clock
 * @param       None
 *
 * @retval      SUCCESS or ERROR
 */
uint8_t Rtc_WaitForSynchro(void)
{
    uint8_t ret = (uint8_t)ERROR;
    __IO uint32_t cnt = 0x00;
    uint32_t synchrostatus = 0x00;

    if (RTC->CTRL_R.CTRL_B.RCMCFG == (uint32_t)BIT_RESET)
    {
        ret = SUCCESS;
    }
    else
    {
        Rtc_DisableWriteProtection();
        RTC->STS_R.STS &= (uint32_t)0xFFFFFF5FU;

        do
        {
            synchrostatus = RTC->STS_R.STS_B.RSFLG;
            cnt = cnt + 1U;
        }
        while ((cnt != (uint32_t)RTC_SYNCHRO_TIMEOUT) && (synchrostatus == 0U));

        if (RTC->STS_R.STS_B.RSFLG != (uint32_t)BIT_RESET)
        {
            Rtc_EnableWriteProtection();
            ret = SUCCESS;
        }
        else
        {
            Rtc_EnableWriteProtection();
            ret = ERROR;
        }
    }

    return ret;
}

/*!
 * @brief       Enables the RTC reference clock detection
 *
 * @param       None
 *
 * @retval      SUCCESS or ERROR
 */
uint8_t Rtc_EnableRefClock(void)
{
    uint8_t ret = (uint8_t)ERROR;

    Rtc_DisableWriteProtection();

    if (Rtc_EnableInit() == (uint8_t)ERROR)
    {
        Rtc_EnableWriteProtection();
        ret = ERROR;
    }
    else
    {
        Rtc_DisableInit();
        Rtc_EnableWriteProtection();
        ret = SUCCESS;
    }

    return ret;
}

/*!
 * @brief       Disable the RTC reference clock detection
 *
 * @param       None
 *
 * @retval      SUCCESS or ERROR
 */
uint8_t Rtc_DisableRefClock(void)
{
    uint8_t ret = (uint8_t)ERROR;

    Rtc_DisableWriteProtection();

    if (Rtc_EnableInit() == (uint8_t)ERROR)
    {
        Rtc_EnableWriteProtection();
        ret = ERROR;
    }
    else
    {

        Rtc_DisableInit();
        Rtc_EnableWriteProtection();
        ret = SUCCESS;
    }

    return ret;
}

/*!
 * @brief       Enable the RTC reference clock detection
 *
 * @param       None
 *
 * @retval      None
 */
void Rtc_EnableBypassShadow(void)
{
    Rtc_DisableWriteProtection();
    RTC->CTRL_R.CTRL_B.RCMCFG = BIT_SET;
    Rtc_EnableWriteProtection();
}

/*!
 * @brief       Disable the RTC reference clock detection
 *
 * @param       None
 *
 * @retval      None
 */
void Rtc_DisableBypassShadow(void)
{
    Rtc_DisableWriteProtection();
    RTC->CTRL_R.CTRL_B.RCMCFG = BIT_RESET;
    Rtc_EnableWriteProtection();
}

/*!
 * @brief       Config the RTC current time
 *
 * @param       Format: specifies the format to write
 *                      This parameter can be one of the following values:
 *                      @arg RTC_FORMAT_BIN: format in Bin
 *                      @arg RTC_FORMAT_BCD: format in BCD
 *
 * @param       TimeStructPtr:  Pointer to a Rtc_TimeType structure that
 *                          contains the configuration information for the RTC peripheral
 *
 * @retval      None
 */
uint8_t Rtc_ConfigTime(Rtc_FormatType Format, Rtc_TimeType* TimeStructPtr)
{
    uint8_t state = ERROR;
    uint32_t temp = 0;

    if (Format == RTC_FORMAT_BIN)
    {
        if (RTC->CTRL_R.CTRL_B.TIMEFCFG == (uint32_t)BIT_RESET)
        {
            TimeStructPtr->H12 = (uint8_t)RTC_H12_AM;
        }
        else
        {
            /* nothing */
        }
    }
    else
    {
        if (RTC->CTRL_R.CTRL_B.TIMEFCFG == (uint32_t)BIT_RESET)
        {
            TimeStructPtr->H12 = (uint8_t)RTC_H12_AM;
        }
        else
        {
            /* nothing */
        }
    }

    if (Format != RTC_FORMAT_BIN)
    {
        temp = (((uint32_t)(TimeStructPtr->hours) << 16) | \
                ((uint32_t)(TimeStructPtr->minutes) << 8) | \
                ((uint32_t)(TimeStructPtr->seconds)) | \
                ((uint32_t)(TimeStructPtr->H12) << 22));
    }
    else
    {
        temp = (((uint32_t)Rtc_ByteConBcd2(TimeStructPtr->hours) << 16) | \
                ((uint32_t)Rtc_ByteConBcd2(TimeStructPtr->minutes) << 8) | \
                ((uint32_t)Rtc_ByteConBcd2(TimeStructPtr->seconds)) | \
                ((uint32_t)(TimeStructPtr->H12) << 22));
    }

    Rtc_DisableWriteProtection();

    if (Rtc_EnableInit() == (uint8_t)ERROR)
    {
        state = ERROR;
    }
    else
    {
        RTC->TIME_R.TIME = (uint32_t)(temp & 0x007F7F7FU);
        Rtc_DisableInit();

        if (RTC->CTRL_R.CTRL_B.RCMCFG == (uint32_t)RESET)
        {
            if (Rtc_WaitForSynchro() == (uint8_t)ERROR)
            {
                state = ERROR;
            }
            else
            {
                state = SUCCESS;
            }
        }
        else
        {
            state = SUCCESS;
        }
    }

    Rtc_EnableWriteProtection();
    return state;
}

/*!
 * @brief       Fills each timeStruct member with its default value
 *
 * @param       TimeStructPtr:  Pointer to a Rtc_TimeType structure that
 *                           contains the configuration information for the RTC peripheral
 *
 * @retval      None
 */
void Rtc_ConfigTimeStructInit(Rtc_TimeType* TimeStructPtr)
{
    /** ALRMA Time Settings : Time = 00h:00mn:00sec */
    TimeStructPtr->hours = 0;
    TimeStructPtr->minutes = 0;
    TimeStructPtr->seconds = 0;
    TimeStructPtr->H12 = (uint8_t)RTC_H12_AM;
}

/*!
 * @brief       Read the RTC current Time
 *
 * @param       Format: specifies the format to write
 *                      This parameter can be one of the following values:
 *                      @arg RTC_FORMAT_BIN: format in Bin
 *                      @arg RTC_FORMAT_BCD: format in BCD
 *
 * @param       TimeStructPtr:  Pointer to a Rtc_TimeType structure that
 *                           contains the configuration information for the RTC peripheral
 *
 * @retval      None
 */
void Rtc_ReadTime(Rtc_FormatType Format, Rtc_TimeType* TimeStructPtr)
{
    uint32_t temp = 0;
    temp = (uint32_t)((RTC->TIME_R.TIME) & 0x007F7F7FU);

    TimeStructPtr->hours   = (uint8_t)((temp & 0x003F0000U) >> 16);
    TimeStructPtr->minutes = (uint8_t)((temp & 0x00007F00U) >> 8);
    TimeStructPtr->seconds = (uint8_t)(temp &  0x0000007FU);
    TimeStructPtr->H12 = (uint8_t)((temp & 0x00400000U) >> 22);

    if (Format == RTC_FORMAT_BIN)
    {
        TimeStructPtr->hours   = (uint8_t)Rtc_Bcd2ConByte(TimeStructPtr->hours);
        TimeStructPtr->minutes = (uint8_t)Rtc_Bcd2ConByte(TimeStructPtr->minutes);
        TimeStructPtr->seconds = (uint8_t)Rtc_Bcd2ConByte(TimeStructPtr->seconds);
    }
    else
    {
         /* nothing */
    }
}

/*!
 * @brief       Read the RTC current Calendar Subseconds value
 *
 * @param       None
 *
 * @retval      RTC current Calendar Subseconds value
 */
uint32_t Rtc_ReadSubSecond(void)
{
    uint32_t temp = 0;
    temp = (uint32_t)(RTC->SUBSEC_R.SUBSEC);
    (void)(RTC->DATE_R.DATE);
    return (temp);
}

/*!
 * @brief       Config the RTC current time
 *
 * @param       Format: specifies the format to write
 *                      This parameter can be one of the following values:
 *                      @arg RTC_FORMAT_BIN: format in Bin
 *                      @arg RTC_FORMAT_BCD: format in BCD
 *
 * @param       DateStructPtr:  Pointer to a Rtc_DateType structure that
 *                           contains the configuration DATE information for the RTC peripheral
 *
 * @retval      None
 */
uint8_t Rtc_ConfigDate(Rtc_FormatType Format, const Rtc_DateType* DateStructPtr)
{
    uint8_t state = ERROR;
    uint32_t temp = 0;
    
    if (Format != RTC_FORMAT_BIN)
    {
        temp = (((uint32_t)(DateStructPtr->year) << 16) | \
                ((uint32_t)(DateStructPtr->month) << 8) | \
                ((uint32_t)(DateStructPtr->date)) | \
                ((uint32_t)(DateStructPtr->weekday) << 13));
    }
    else
    {
        temp = (((uint32_t)Rtc_ByteConBcd2(DateStructPtr->year) << 16) | \
                ((uint32_t)Rtc_ByteConBcd2(DateStructPtr->month) << 8) | \
                ((uint32_t)Rtc_ByteConBcd2(DateStructPtr->date)) | \
                ((uint32_t)(DateStructPtr->weekday) << 13));

    }

    Rtc_DisableWriteProtection();

    if (Rtc_EnableInit() == (uint8_t)ERROR)
    {
        state = ERROR;
    }
    else
    {
        RTC->DATE_R.DATE = (uint32_t)(temp & 0x00FFFF3FU);
        Rtc_DisableInit();

        if (RTC->CTRL_R.CTRL_B.RCMCFG == (uint32_t)RESET)
        {
            if (Rtc_WaitForSynchro() == (uint8_t)ERROR)
            {
                state = ERROR;
            }
            else
            {
                state = SUCCESS;
            }
        }
        else
        {
            state = SUCCESS;
        }
    }

    Rtc_EnableWriteProtection();
    return state;
}

/*!
 * @brief       Fills each dateStruct member with its default value
 *
 * @param       DateStructPtr:  Pointer to a Rtc_DateType structure that
 *                           contains the configuration DATE information for the RTC peripheral
 * @retval      None
 */
void Rtc_ConfigDateStructInit(Rtc_DateType* DateStructPtr)
{
    DateStructPtr->weekday = RTC_WEEKDAY_MONDAY;
    DateStructPtr->month = RTC_MONTH_JANUARY;
    DateStructPtr->date = 1;
    DateStructPtr->year = 0;
}

/*!
 * @brief       the RTC current date
 *
 * @param       Format: specifies the format to write
 *                      This parameter can be one of the following values:
 *                      @arg RTC_FORMAT_BIN: format in Bin
 *                      @arg RTC_FORMAT_BCD: format in BCD
 *
 * @param       DateStructPtr:  Pointer to a Rtc_DateType structure that
 *                           contains the configuration DATE information for the RTC peripheral
 *
 * @retval      None
 */
void Rtc_ReadDate(Rtc_FormatType Format, Rtc_DateType* DateStructPtr)
{
    uint32_t temp = 0;
    temp = (uint32_t)((RTC->DATE_R.DATE) & 0x00FFFF3FU);

    DateStructPtr->year  = (uint8_t)((temp & 0x00FF0000U) >> 16);
    DateStructPtr->month = (uint8_t)((temp & 0x00001F00U) >> 8);
    DateStructPtr->date  = (uint8_t)(temp &  0x0000003FU);
    DateStructPtr->weekday = (uint8_t)((temp & 0x0000E000U) >> 13);

    if (Format == RTC_FORMAT_BIN)
    {
        DateStructPtr->year  = (uint8_t)Rtc_Bcd2ConByte(DateStructPtr->year);
        DateStructPtr->month = (uint8_t)Rtc_Bcd2ConByte(DateStructPtr->month);
        DateStructPtr->date  = (uint8_t)Rtc_Bcd2ConByte(DateStructPtr->date);
        DateStructPtr->weekday = (uint8_t)(DateStructPtr->weekday);
    }
    else
    {
         /* nothing */
    }
}

/*!
 * @brief       Config the specified RTC ALRMA
 *
 * @param       Format: specifies the format to write
 *                      This parameter can be one of the following values:
 *                      @arg RTC_FORMAT_BIN: format in Bin
 *                      @arg RTC_FORMAT_BCD: format in BCD
 *
 * @param       AlarmStructPtr: Pointer to a Rtc_AlarmType structure that
 *                           contains the configuration ALRMA information for the RTC peripheral
 *
 * @retval      None
 */
void Rtc_ConfigAlarm(Rtc_FormatType Format, Rtc_AlarmType* AlarmStructPtr)
{
    uint32_t temp = 0;

    if (Format != RTC_FORMAT_BCD)
    {
        if (RTC->CTRL_R.CTRL_B.TIMEFCFG == (uint32_t)BIT_RESET)
        {
            AlarmStructPtr->time.H12 = 0x00;
        }
        else
        {
             /* nothing */
        }
    }
    else
    {
        if (RTC->CTRL_R.CTRL_B.TIMEFCFG == (uint32_t)BIT_RESET)
        {
            AlarmStructPtr->time.H12 = 0x00;
        }
        else
        {
            /* nothing */
        }
    }

    if (Format == RTC_FORMAT_BCD)
    {
        temp = (((uint32_t)(AlarmStructPtr->time.hours) << 16) | \
                ((uint32_t)(AlarmStructPtr->time.minutes) << 8) | \
                ((uint32_t)AlarmStructPtr->time.seconds) | \
                ((uint32_t)(AlarmStructPtr->time.H12) << 22) | \
                ((uint32_t)(AlarmStructPtr->AlarmDateWeekDay) << 24) | \
                ((uint32_t)AlarmStructPtr->AlarmDateWeekDaySel << 30) | \
                ((uint32_t)AlarmStructPtr->AlarmMask));
    }
    else
    {
        temp = (((uint32_t)Rtc_ByteConBcd2(AlarmStructPtr->time.hours) << 16) | \
                ((uint32_t)Rtc_ByteConBcd2(AlarmStructPtr->time.minutes) << 8) | \
                ((uint32_t)Rtc_ByteConBcd2(AlarmStructPtr->time.seconds)) | \
                ((uint32_t)(AlarmStructPtr->time.H12) << 22) | \
                ((uint32_t)Rtc_ByteConBcd2(AlarmStructPtr->AlarmDateWeekDay) << 24) | \
                ((uint32_t)AlarmStructPtr->AlarmDateWeekDaySel << 30) | \
                ((uint32_t)AlarmStructPtr->AlarmMask));
    }

    Rtc_DisableWriteProtection();
    RTC->ALRMA_R.ALRMA = temp;
    Rtc_EnableWriteProtection();
}

/*!
 * @brief       Fills each alarmStruct member with its default value
 *
 * @param       AlarmStructPtr: Pointer to a Rtc_AlarmType structure that
 *                           contains the configuration ALRMA information for the RTC peripheral
 *
 * @retval      None
 */
void Rtc_ConfigAlarmStructInit(Rtc_AlarmType* AlarmStructPtr)
{
    AlarmStructPtr->time.hours = 1;
    AlarmStructPtr->time.minutes = 2;
    AlarmStructPtr->time.seconds = 3;
    AlarmStructPtr->time.H12 = (uint8_t)RTC_H12_AM;
    AlarmStructPtr->AlarmDateWeekDay = 1;
    AlarmStructPtr->AlarmDateWeekDaySel = RTC_WEEKDAY_SEL_DATE;
    AlarmStructPtr->AlarmMask = (uint32_t)RTC_MASK_NONE;
}

/*!
 * @brief       Get the RTC ALRMA value and masks
 *
 * @param       Format: specifies the format to write
 *                      This parameter can be one of the following values:
 *                      @arg RTC_FORMAT_BIN: format in Bin
 *                      @arg RTC_FORMAT_BCD: format in BCD
 *
 * @param       AlarmStructPtr: Pointer to a Rtc_AlarmType structure that
 *                           contains the configuration ALRMA information for the RTC peripheral
 *
 * @retval      None
 */
void Rtc_ReadAlarm(Rtc_FormatType Format, Rtc_AlarmType* AlarmStructPtr)
{
    uint8_t day_d, day_u, hours_d, hours_u, minutes_d, minutes_u, seconds_d, seconds_u;
    uint32_t day_mask, hours_mask, minutes_mask, seconds_mask;

    day_d = RTC->ALRMA_R.ALRMA_B.DAYT << 0x04;
    day_u = RTC->ALRMA_R.ALRMA_B.DAYU;
    hours_d = RTC->ALRMA_R.ALRMA_B.HRT << 0x04;
    hours_u = RTC->ALRMA_R.ALRMA_B.HRU;
    minutes_d = RTC->ALRMA_R.ALRMA_B.MINT << 0x04;
    minutes_u = RTC->ALRMA_R.ALRMA_B.MINU;
    seconds_d = RTC->ALRMA_R.ALRMA_B.SECT << 0x04;
    seconds_u = RTC->ALRMA_R.ALRMA_B.SECU;

    day_mask = (uint32_t)RTC->ALRMA_R.ALRMA_B.DATEMEN << 8;
    hours_mask = (uint32_t)RTC->ALRMA_R.ALRMA_B.HRMEN << 8;
    minutes_mask = (uint32_t)RTC->ALRMA_R.ALRMA_B.MINMEN << 8;
    seconds_mask = (uint32_t)RTC->ALRMA_R.ALRMA_B.SECMEN << 7;

    AlarmStructPtr->time.hours   = (uint8_t)(hours_d | hours_u);
    AlarmStructPtr->time.minutes = (uint8_t)(minutes_d | minutes_u);
    AlarmStructPtr->time.seconds = (uint8_t)(seconds_d | seconds_u);
    AlarmStructPtr->time.H12     = (uint8_t)(RTC->ALRMA_R.ALRMA_B.TIMEFCFG);
    AlarmStructPtr->AlarmDateWeekDay = (uint8_t)(day_d | day_u);
    AlarmStructPtr->AlarmDateWeekDaySel = (Rtc_WeekdaySelType)(RTC->ALRMA_R.ALRMA_B.WEEKSEL);
    AlarmStructPtr->AlarmMask = (uint32_t)(day_mask | hours_mask | minutes_mask | seconds_mask);

    if (Format == RTC_FORMAT_BIN)
    {
        AlarmStructPtr->time.hours = (uint8_t)Rtc_Bcd2ConByte(AlarmStructPtr->time.hours);
        AlarmStructPtr->time.minutes = (uint8_t)Rtc_Bcd2ConByte(AlarmStructPtr->time.minutes);
        AlarmStructPtr->time.seconds = (uint8_t)Rtc_Bcd2ConByte(AlarmStructPtr->time.seconds);
        AlarmStructPtr->AlarmDateWeekDay = (uint8_t)Rtc_Bcd2ConByte(AlarmStructPtr->AlarmDateWeekDay);
    }
    else
    {
         /* nothing */
    }
}

/*!
 * @brief       Enable the RTC ALRMA.
 *
 * @param       None
 *
 * @retval      None
 */
void Rtc_EnableAlarm(void)
{
    Rtc_DisableWriteProtection();
    RTC->CTRL_R.CTRL_B.ALREN = BIT_SET;
    Rtc_EnableWriteProtection();
}

/*!
 * @brief       Disable the the RTC ALRMA.
 *
 * @param       None
 *
 * @retval      None
 */
uint8_t Rtc_DisableAlarm(void)
{
    uint8_t ret = (uint8_t)ERROR;
    __IO uint32_t count = 0x00;
    Rtc_DisableWriteProtection();
    RTC->CTRL_R.CTRL_B.ALREN = BIT_RESET;

    while ((count != (uint32_t)RTC_INITMODE_TIMEOUT) && ((RTC->STS_R.STS_B.ALRWFLG) == (uint32_t)BIT_RESET))
    {
        count = count + 1U;
    }

    if ((RTC->STS_R.STS_B.ALRWFLG) == (uint32_t)BIT_RESET)
    {
        Rtc_EnableWriteProtection();
        ret = ERROR;
    }
    else
    {
        Rtc_EnableWriteProtection();
        ret = SUCCESS;
    }

    return ret;
}

/*!
 * @brief       Read the RTC ALRMA Subseconds value
 *
 * @param       Val: specifies the value for ALRMA Sub Second
 *                   this value must less than 0x00007FFF
 *
 * @param       Mask: specifies the mask for ALRMA Sub Second
 *                   this value must less than 0x0f
 *
 * @retval      None
 */
void Rtc_ConfigAlarmSubSecond(uint32_t Val, uint8_t Mask)
{
    Rtc_DisableWriteProtection();
    RTC->ALRMASS_R.ALRMASS_B.SUBSEC = Val;
    RTC->ALRMASS_R.ALRMASS_B.MASKSEL = Mask;
    Rtc_EnableWriteProtection();
}

/*!
 * @brief       Read the RTC ALRMA Subseconds value
 *
 * @param       None
 *
 * @retval      RTC ALRMA Subseconds value
 */
uint32_t Rtc_ReadAlarmSubSecond(void)
{
    return (uint32_t)(RTC->ALRMASS_R.ALRMASS_B.SUBSEC);
}

/*!
 * @brief       Adds or substract one hour from the current time
 *
 * @param       Sav:   specifies the DayLightSaving
 *                     This parameter can be one of the following values:
 *                     @arg RTC_DLS_SUB1H
 *                     @arg RTC_DLS_ADD1H
 *
 * @param       Bit:   specifies the DayLightSaving
 *                     This parameter can be one of the following values:
 *                     @arg RTC_SO_RESET
 *                     @arg RTC_SO_SET
 *
 * @retval      None
 */
void Rtc_ConfigDayLightSaving(Rtc_DaylightSavingType Sav, Rtc_StoreOperationType Bit)
{
    Rtc_DisableWriteProtection();

    if (Sav == RTC_DLS_ADD1H)
    {
        RTC->CTRL_R.CTRL_B.STCCFG = (uint32_t)BIT_SET;
        RTC->CTRL_R.CTRL_B.WTCCFG = (uint32_t)BIT_RESET;
    }
    else
    {
        RTC->CTRL_R.CTRL_B.STCCFG = (uint32_t)BIT_RESET;
        RTC->CTRL_R.CTRL_B.WTCCFG = (uint32_t)BIT_SET;
    }
    RTC->CTRL_R.CTRL_B.BAKE = (uint32_t)Bit;
    Rtc_EnableWriteProtection();
}

/*!
 * @brief       Returns the RTC Day Light Saving stored operation
 *
 * @param       None
 *
 * @retval      RTC Day Light Saving stored operation
 */
uint32_t Rtc_ReadStoreOperation(void)
{
    return (RTC->CTRL_R.CTRL_B.BAKE);
}



/*!
 * @brief       Configures the Synchronization Shift Control Settings.
 *
 * @param       Period: Select the Smooth Calibration period.
 *                      This parameter can be can be one of the following values:
 *                      @arg RTC_SCP_16SEC: The smooth calibration periode is 16.
 *                      @arg RTC_SCP_8SEC:  The smooth calibartion periode is 8s.
 *
 * @param       Bit:    Select to Set or reset the CALP bit.
 *                      This parameter can be one of the following values:
 *                      @arg RTC_SCPP_RESET: Add one RTCCLK puls every 2**11 pulses.
 *                      @arg RTC_SCPP_SET:   No RTCCLK pulses are added.
 *
 * @param       Value:  Select the value of CALM[8:0] bits.
 *                      This parameter can be one any value from 0 to 0x000001FF.
 *
 * @retval         SUCCESS or ERROR
 */
uint8_t Rtc_ConfigSmoothCalib(Rtc_ScpType Period, Rtc_ScppType Bit, uint32_t Value)
{
    uint8_t state = ERROR;
    uint32_t count = 0;

    Rtc_DisableWriteProtection();

    if (RTC->STS_R.STS_B.RCALPFLG != (uint32_t)BIT_RESET)
    {
        while ((RTC->STS_R.STS_B.RCALPFLG != (uint32_t)BIT_RESET) && (count != RTC_RECALPF_TIMEOUT))
        {
            count++;
        }
    }
    else
    {
         /* nothing */
    }

    if (RTC->STS_R.STS_B.RCALPFLG == (uint32_t)BIT_RESET)
    {
        if (Period == RTC_SCP_16SEC)
        {
            RTC->CAL_R.CAL = ((((uint32_t)Bit << 15) | ((uint32_t)BIT_SET << 13)) | (uint32_t)Value);
        }
        else
        {
            RTC->CAL_R.CAL = ((((uint32_t)Bit << 15) | ((uint32_t)BIT_SET << 14)) | (uint32_t)Value);
        }

        state = SUCCESS;
    }
    else
    {
        state = ERROR;
    }

    Rtc_EnableWriteProtection();
    return (state);
}


/*!
 * @brief       Read the RTC TimeStamp value and masks
 *
 * @param       Format: specifies the format of the output parameters.
 *                      This parameter can be one of the following values:
 *                      @arg RTC_Format_BIN: data in Binary format
 *                      @arg RTC_Format_BCD: data in BCD    format
 *
 * @param       TimeStructPtr: pointer to a Rtc_TimeType structure that will
 *                              contains the TimeStamp time values.
 *
 * @param       DateStructPtr: pointer to a Rtc_DateType structure that will
 *                              contains the TimeStamp date values.
 *
 * @retval      None
 */
void Rtc_ReadTimeDate(Rtc_FormatType Format, Rtc_TimeType* TimeStructPtr, Rtc_DateType* DateStructPtr)
{
    uint32_t temptime = 0, tempdate = 0;

    TimeStructPtr->hours   = (uint8_t)((temptime & 0x003F0000U) >> 16);
    TimeStructPtr->minutes = (uint8_t)((temptime & 0x00007F00U) >> 8);
    TimeStructPtr->seconds = (uint8_t)(temptime &  0x0000007FU);
    TimeStructPtr->H12 = (uint8_t)((temptime & 0x00400000U) >> 22);

    DateStructPtr->year  =  0;
    DateStructPtr->month = (uint8_t)((tempdate & 0x00001F00U) >> 8);
    DateStructPtr->date  = (uint8_t)(tempdate &  0x0000003FU);
    DateStructPtr->weekday = (uint8_t)((tempdate & 0x0000E000U) >> 13);

    if (Format == RTC_FORMAT_BIN)
    {
        TimeStructPtr->hours   = (uint8_t)Rtc_Bcd2ConByte(TimeStructPtr->hours);
        TimeStructPtr->minutes = (uint8_t)Rtc_Bcd2ConByte(TimeStructPtr->minutes);
        TimeStructPtr->seconds = (uint8_t)Rtc_Bcd2ConByte(TimeStructPtr->seconds);

        DateStructPtr->month = (uint8_t)Rtc_Bcd2ConByte(DateStructPtr->month);
        DateStructPtr->date  = (uint8_t)Rtc_Bcd2ConByte(DateStructPtr->date);
        DateStructPtr->weekday = (uint8_t)(DateStructPtr->weekday);
    }
    else
    {
         /* nothing */
    }
}



/*!
 * @brief       Enable RTC interrupts.
 *
 * @param       Interrupt: specifies the RTC interrupt sources to be enabled
 *                       This parameter can be any combination of the following values:
 *                        @arg RTC_INT_ALR:  ALRMA A interrupt mask
 *                        @arg RTC_INT_TS:   Time Stamp interrupt mask
 *                        @arg RTC_INT_TAMP: Tamper event interrupt mask
 *
 * @retval      None
 */
void Rtc_EnableInterrupt(uint32_t Interrupt)
{
    Rtc_DisableWriteProtection();
    RTC->CTRL_R.CTRL |= (uint32_t)(Interrupt & ~0x00000004U);
    Rtc_EnableWriteProtection();
}

/*!
 * @brief     Disable RTC interrupts.
 *
 * @param     Interrupt: specifies the RTC interrupt sources to be disable
 *                       This parameter can be any combination of the following values:
 *                        @arg RTC_INT_ALR:  ALRMA A interrupt mask
 *                        @arg RTC_INT_TS:   Time Stamp interrupt mask
 *                        @arg RTC_INT_TAMP: Tamper event interrupt mask
 *
 * @retval    None
 */
void Rtc_DisableInterrupt(uint32_t Interrupt)
{
    Rtc_DisableWriteProtection();
    RTC->CTRL_R.CTRL &= (uint32_t)~(Interrupt & ~0x00000004U);
    Rtc_EnableWriteProtection();
}

/*!
 * @brief     Read interrupt flag bit is set
 *
 * @param     Flag: specifies the flag to read.
 *                  This parameter can be one of the following values:
 *                  @arg RTC_INT_FLAG_ALR: ALRMA interrupt
 *                  @arg RTC_INT_FLAG_TS: Time Stamp interrupt
 *                  @arg RTC_INT_FLAG_TAMP1: Tamper1 event interrupt
 *                  @arg RTC_INT_FLAG_TAMP2: Tamper2 event interrupt
 * @retval    The new state of flag (SET or RESET).
 */
uint8_t Rtc_ReadIntFlag(Rtc_IntFlagType Flag)
{
    uint32_t intEnable = 0;
    uint32_t intStatus = 0;
    uint8_t ret = (uint8_t)RESET;

    if (((uint32_t)Flag & 0x04U) == 0U)
    {
        intEnable = (uint32_t)RTC->CTRL_R.CTRL;
		intStatus = (uint32_t)(RTC->STS_R.STS & ((uint32_t)Flag >> 4));
    }
    else
    {
        /* nothing */
    }

    if (intEnable != 0U)
    {
        if (intStatus != 0U)
        {
            ret = (uint8_t)SET;
        }
        else
        {
             /* nothing */
        }
    }
    else
    {
         /* nothing */
    }

    return ret;
}

/*!
 * @brief     Clear RTC interrupt flag bit
 *
 * @param     Flag: specifies the flag to clear.
 *                  This parameter can be any combination the following values:
 *                  @arg RTC_INT_FLAG_ALR: ALRMA interrupt
 *                  @arg RTC_INT_FLAG_TS: Time Stamp interrupt
 *                  @arg RTC_INT_FLAG_TAMP1: Tamper1 event interrupt
 *                  @arg RTC_INT_FLAG_TAMP2: Tamper2 event interrupt
 * @retval    The new state of flag (SET or RESET).
 */
void Rtc_ClearIntFlag(uint32_t Flag)
{
    RTC->STS_R.STS &= (uint32_t) ~(Flag >> 4);
}

/*!
 * @brief     Checks whether the specified RTC flag is set or not.
 *
 * @param     Flag: specifies the flag to check.
 *                  This parameter can be one of the following values:
 *                  @arg RTC_FLAG_ISF
 *                  @arg RTC_FLAG_RSF
 *                  @arg RTC_FLAG_INTF
 *                  @arg RTC_FLAG_ALRF
 *                  @arg RTC_FLAG_TSF
 *                  @arg RTC_FLAG_TSOF
 *                  @arg RTC_FLAG_TP1F
 *                  @arg RTC_FLAG_TP2F
 *                  @arg RTC_FLAG_RPF
 * @retval    The new state of RTC_FLAG (SET or RESET).
 */
uint8_t Rtc_ReadStatusFlag(Rtc_FlagType Flag)
{
    uint8_t ret = 0U;

    if((RTC->STS_R.STS & (uint32_t)Flag) != 0U)
    {
        ret = (uint8_t)SET;
    }
    else
    {
        ret = (uint8_t)RESET;
    }

    return ret;
}

/*!
 * @brief     Clears the RTC's status flags.
 * @param     flag: specifies the RTC flag to clear.
 *                  This parameter can be any combination of the following values:
 *                  @arg RTC_FLAG_TP2F: Tamper 2 event flag
 *                  @arg RTC_FLAG_TP1F: Tamper 1 event flag
 *                  @arg RTC_FLAG_TSOF: Time Stamp Overflow flag
 *                  @arg RTC_FLAG_TSF : Time Stamp event flag
 *                  @arg RTC_FLAG_ALRF: ALRMA A flag
 *                  @arg RTC_FLAG_RSF:  Registers Synchronized flag
 */
void Rtc_ClearStatusFlag(uint32_t Flag)
{
    RTC->STS_R.STS &= (uint32_t)~Flag;
}

/*!
 * @brief     Converts a 2 digit decimal to BCD format
 *
 * @param     val: Byte to be converted
 *
 * @retval    Converted byte
 */
static uint8_t Rtc_ByteConBcd2(uint8_t val)
{
    uint8_t bcdhigh = 0;
    uint8_t value = val;

    while (value >= 10U)
    {
        bcdhigh++;
        value -= 10U;
    }

    return ((uint8_t)(bcdhigh << 4) | value);
}

/*!
 * @brief     Convert from 2 digit BCD to Binary
 *
 * @param     val: BCD value to be converted
 *
 * @retval    Converted word
 */
static uint8_t Rtc_Bcd2ConByte(uint8_t val)
{
    uint8_t tmp = 0;
    tmp = ((uint8_t)(val & (uint8_t)0xF0) >> (uint8_t)0x4) * 10U;
    return (tmp + (val & (uint8_t)0x0F));
}

/**@} end of group RTC_Functions*/
/**@} end of group RTC_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/

