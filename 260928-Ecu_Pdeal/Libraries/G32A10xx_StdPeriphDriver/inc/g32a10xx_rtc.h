/*!
 * @file        g32a10xx_rtc.h
 *
 * @brief       This file contains all the functions prototypes for the RTC firmware library.
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

#ifndef G32A10xx_RTC_H
#define G32A10xx_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup RTC_Driver
  @{
*/

/** @defgroup RTC_Enumerations Enumerations
  @{
*/

/**
 * @brief RTC Hour Formats
 */
typedef enum
{
    RTC_HOURFORMAT_24,   /*!< 24 hour/day format */
    RTC_HOURFORMAT_12    /*!< AM/PM hour format */
} Rtc_HourFormatType;

/**
 * @brief RTC Input parameter format
 */
typedef enum
{
    RTC_FORMAT_BIN,  /*!< Format in BIN */
    RTC_FORMAT_BCD   /*!< Format in BCD */
} Rtc_FormatType;

/**
 * @brief RTC_AM_PM
 */
typedef enum
{
    RTC_H12_AM,  /*!< Set RTC time to AM */
    RTC_H12_PM   /*!< Set RTC time to PM */
} Rtc_H12Type;

/**
 * @brief RTC DayLightSaving
 */
typedef enum
{
    RTC_DLS_SUB1H,  /*!< Winter time change */
    RTC_DLS_ADD1H   /*!< Summer time change */
} Rtc_DaylightSavingType;

/**
 * @brief RTC DayLightSaving
 */
typedef enum
{
    RTC_SO_RESET = BIT_RESET,   /*!< Reset backup value */
    RTC_SO_SET   = BIT_SET      /*!< Set backup value */
} Rtc_StoreOperationType;

/**
 * @brief RTC Output selection
 */
typedef enum
{
    RTC_OPSEL_DISABLE = 0x00,  /*!< output disable */
    RTC_OPSEL_ALARMA  = 0x01,  /*!< alarma output enable */
    RTC_OPSEL_WAKEUP  = 0x03   /*!< wake up enable */
} Rtc_OpselType;

/**
 * @brief RTC Output Polarity
 */
typedef enum
{
    RTC_OPP_HIGH,  /*!< output polarity is high */
    RTC_OPP_LOW    /*!< output polarity is low */
} Rtc_OppType;

/**
 * @brief RTC Calib Output selection
 */
typedef enum
{
    RTC_CALIBOUTPUT_512Hz,  /*!< calib output is 512Hz */
    RTC_CALIBOUTPUT_1Hz     /*!< calib output is 1Hz */
} Rtc_CalibOutputType;

/**
 * @brief RTC Smooth calib period
 */
typedef enum
{
    RTC_SCP_16SEC,   /*!<The smooth calibration periode is 16 */
    RTC_SCP_8SEC     /*!<The smooth calibration periode is 8 */
} Rtc_ScpType;

/**
 * @brief RTC Smooth calib period
 */
typedef enum
{
    RTC_SCPP_RESET,  /*!< Add one RTCCLK puls every 2**11 pulses */
    RTC_SCPP_SET     /*!< No RTCCLK pulses are added */
} Rtc_ScppType;

/**
 * @brief RTC Time Stamp Edges
 */
typedef enum
{
    RTC_TIME_STAMPEDGE_RISING,  /*!< event occurs on the rising edge */
    RTC_TIME_STAMPEDGE_FALLING  /*!< event occurs on the falling edge */
} Rtc_TimestampEdgeType;

/**
 * @brief RTC Tamper Trigger
 */
typedef enum
{
    RTC_TAMPER_TRIGGER_RISINGEDGE   = 0x00,  /*!< Rising Edge of the tamper pin causes tamper event */
    RTC_TAMPER_TRIGGER_FALLINGEDGE  = 0x01,  /*!< Falling Edge of the tamper pin causes tamper event */
    RTC_TAMPER_TRIGGER_LOWLEVEL     = 0x00,  /*!< Low Level of the tamper pin causes tamper event */
    RTC_TAMPER_TRIGGER_HIGHLEVEL    = 0x01   /*!< High Level of the tamper pin causes tamper event */
} Rtc_TamperTriggerType;

/**
 * @brief RTC Tamper Pins
 */
typedef enum
{
    RTC_TAMPER_1,  /*!< Select Tamper 1 */
    RTC_TAMPER_2   /*!< Select Tamper 1 */
} Rtc_TamperType;

/**
 * @brief Tampers Sampling Frequency
 */
typedef enum
{
    RTC_TAMPER_FILTER_DISABLE,  /*!< Tamper filter is disabled */
    RTC_TAMPER_FILTER_2SAMPLE,  /*!< Tamper is activated after 2 consecutive samples at the active level */
    RTC_TAMPER_FILTER_4SAMPLE,  /*!< Tamper is activated after 4 consecutive samples at the active level */
    RTC_TAMPER_FILTER_8SAMPLE   /*!< Tamper is activated after 8 consecutive samples at the active level */
} Rtc_TamperFillerType;

/**
 * @brief Tampers Sampling Frequency
 */
typedef enum
{
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768,  /*!< Tampers Sampling Frequency = RTC_CLK / 32768 */
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384,  /*!< Tampers Sampling Frequency = RTC_CLK / 16384 */
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192,   /*!< Tampers Sampling Frequency = RTC_CLK / 8192 */
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096,   /*!< Tampers Sampling Frequency = RTC_CLK / 4096 */
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048,   /*!< Tampers Sampling Frequency = RTC_CLK / 2048 */
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV1024,   /*!< Tampers Sampling Frequency = RTC_CLK / 1024 */
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512,    /*!< Tampers Sampling Frequency = RTC_CLK / 512 */
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256     /*!< Tampers Sampling Frequency = RTC_CLK / 256 */
} Rtc_TamperSamplingFreqType;

/**
 * @brief Precharge Duration
 */
typedef enum
{
    RTC_PRECHARGEDURATION_1RTCCLK,  /*!< Duration is 1 RTCCLK cycle */
    RTC_PRECHARGEDURATION_2RTCCLK,  /*!< Duration is 2 RTCCLK cycle */
    RTC_PRECHARGEDURATION_4RTCCLK,  /*!< Duration is 4 RTCCLK cycle */
    RTC_PRECHARGEDURATION_8RTCCLK   /*!< Duration is 8 RTCCLK cycle */
} Rtc_PrechargeDurationType;

/**
 * @brief RTC Add 1 Second Parameter
 */
typedef enum
{
    RTC_SHIFTADD1S_RESET,  /*!< No effect */
    RTC_SHIFTADD1S_SET     /*!< Add one second to the clock calendar */
} Rtc_ShiftAdd1SType;

/**
 * @brief
 */
typedef enum
{
    RTC_OPENDRAIN,  /*!< RTC Output is configured in Open Drain mode */
    RTC_PUSHPULL    /*!< RTC Output is configured in Push Pull mode */
} Rtc_OutputType;

/**
  * @brief  RTC Interrupts  Soure
  */
typedef enum
{
    RTC_INT_ALR         = 0x00001000,  /*!< ALRMA A interrupt mask */
    RTC_INT_TS          = 0x00008000,  /*!< Time Stamp interrupt mask */
    RTC_INT_TAMP        = 0x00000004   /*!< Tamper event interrupt mask */
} Rtc_IntType;

/**
  * @brief  RTC Interrupts  Flag
  */
typedef enum
{
    RTC_INT_FLAG_ALR    = 0x00001000,  /*!< ALRMA interrupt */
    RTC_INT_FLAG_TS     = 0x00008000,  /*!< Time Stamp interrupt mask */
    RTC_INT_FLAG_TAMP1  = 0x00020004,  /*!< Tamper1 event interrupt mask */
    RTC_INT_FLAG_TAMP2  = 0x00040004   /*!< Tamper2 event interrupt mask */
} Rtc_IntFlagType;

/**
  * @brief  RTC flag
  */
typedef enum
{
    RTC_FLAG_AWF        = BIT0,  /*!< Alarm Write Flag */
    RTC_FLAG_SOPF       = BIT3,  /*!< Shift Operation Pending Flag */
    RTC_FLAG_ISF        = BIT4,  /*!< Initialization State Flag */
    RTC_FLAG_RSF        = BIT5,  /*!< Registers Synchronization Flag */
    RTC_FLAG_INTF       = BIT6,  /*!< Register Initialization Flag */
    RTC_FLAG_ALRF       = BIT8,  /*!< Alarm Match Flag */
    RTC_FLAG_TSF        = BIT11, /*!< Time Stamp Flag */
    RTC_FLAG_TSOF       = BIT12, /*!< Time Stamp Overflow Flag */
    RTC_FLAG_TP1F       = BIT13, /*!< Tamper 1 event Detection Flag */
    RTC_FLAG_TP2F       = BIT14, /*!< Tamper 2 event Detection Flag */
    RTC_FLAG_RPF        = BIT16  /*!< Recalibration Pending Flag */
} Rtc_FlagType;

/**
  * @brief  RTC_Backup
  */
typedef enum
{
    RTC_BAKP_DATA0,  /*!< set Backup data0 */
    RTC_BAKP_DATA1,  /*!< set Backup data1 */
    RTC_BAKP_DATA2,  /*!< set Backup data2 */
    RTC_BAKP_DATA3,  /*!< set Backup data3 */
    RTC_BAKP_DATA4   /*!< set Backup data4 */
} Rtc_BakpDataType;

/**@} end of group RTC_Enumerations*/

/** @addtogroup RTC_Macros Macros
  @{
*/

/** Macros description */
#define RTC_INITMODE_TIMEOUT      ((uint32_t) 0x00004000)
#define RTC_SYNCHRO_TIMEOUT       ((uint32_t) 0x00008000)
#define RTC_RECALPF_TIMEOUT       ((uint32_t) 0x00001000)
#define RTC_SHPF_TIMEOUT          ((uint32_t) 0x00001000)

#define RTC_CTRL_INT              ((uint32_t) 0x0000D000)
#define RTC_TAFCFG_INT            ((uint32_t) 0x00000004)
#define RTC_CTRL_INT              ((uint32_t) 0x0000D000)
#define RTC_TAFCFG_INT            ((uint32_t) 0x00000004)

#define RTC_MONTH_JANUARY         ((uint8_t)0x01)
#define RTC_MONTH_FEBRUARY        ((uint8_t)0x02)
#define RTC_MONTH_MARCH           ((uint8_t)0x03)
#define RTC_MONTH_APRIL           ((uint8_t)0x04)
#define RTC_MONTH_MAY             ((uint8_t)0x05)
#define RTC_MONTH_JUNE            ((uint8_t)0x06)
#define RTC_MONTH_JULY            ((uint8_t)0x07)
#define RTC_MONTH_AUGUST          ((uint8_t)0x08)
#define RTC_MONTH_SEPTEMBER       ((uint8_t)0x09)
#define RTC_MONTH_OCTOBER         ((uint8_t)0x0A)
#define RTC_MONTH_NOVEMBER        ((uint8_t)0x0B)
#define RTC_MONTH_DECEMBER        ((uint8_t)0x0C)

#define RTC_WEEKDAY_MONDAY        ((uint8_t)0x01)
#define RTC_WEEKDAY_TUESDAY       ((uint8_t)0x02)
#define RTC_WEEKDAY_WEDNESDAY     ((uint8_t)0x03)
#define RTC_WEEKDAY_THURSDAY      ((uint8_t)0x04)
#define RTC_WEEKDAY_FRIDAY        ((uint8_t)0x05)
#define RTC_WEEKDAY_SATURDAY      ((uint8_t)0x06)
#define RTC_WEEKDAY_SUNDAY        ((uint8_t)0x07)

#define RTC_WEEKDAY_SEL_DATE      (0U)
#define RTC_WEEKDAY_SEL_WEEKDAY   (0x01U)

#define RTC_MASK_NONE             ((uint32_t)0x00000000U)
#define RTC_MASK_DATEWEEK         ((uint32_t)0x80000000U)
#define RTC_MASK_HOURS            ((uint32_t)0x00800000U)
#define RTC_MASK_MINUTES          ((uint32_t)0x00008000U)
#define RTC_MASK_SECONDS          ((uint32_t)0x00000080U)
#define RTC_MASK_ALL              ((uint32_t)0x80808080U)

/**@} end of group RTC_Macros*/

/** @defgroup RTC_Structures Structures
  @{
*/

/**
 * @brief RTC AlarmDateWeekDay
 */
typedef uint8_t Rtc_WeekdaySelType;

/**
  * @brief  RTC Init structures definition
  */
typedef struct
{
    Rtc_HourFormatType format;   /*!< RTC hour formats selection */
    uint32_t AsynchPrediv;      /*!< Asynchronous prescaler coefficient setting */
    uint32_t SynchPrediv;       /*!< Synchronous prescaler coefficient setting */
} Rtc_ConfigType;

/**
  * @brief  RTC Time structure definition
  */
typedef struct
{
    uint8_t hours;      /*!< Set hours of RTC time */
    uint8_t minutes;    /*!< Set minutes of RTC time */
    uint8_t seconds;    /*!< Set seconds of RTC time */
    uint8_t H12;        /*!< Set RTC time to AM or PM */
} Rtc_TimeType;

/**
  * @brief  RTC Date structure definition
  */
typedef struct
{
    uint8_t weekday;  /*!< Set weekday of RTC date */
    uint8_t month;    /*!< Set month of RTC date */
    uint8_t date;     /*!< Set data of RTC date */
    uint8_t year;     /*!< Set year of RTC date */
} Rtc_DateType;

/**
  * @brief  RTC ALRMA structure definition
  */
typedef struct
{
    Rtc_TimeType time;                        /*!< Set RTC time */
    uint32_t AlarmMask;                     /*!< Set alarm mask */
    Rtc_WeekdaySelType AlarmDateWeekDaySel;  /*!< Set weekday's DAYU of alarm date */
    uint8_t AlarmDateWeekDay;               /*!< Set weekday of alarm date */
} Rtc_AlarmType;

/**@} end of group RTC_Structures*/

/** @defgroup RTC_Functions Functions
  @{
*/

/** Initialization and Configuration functions */
uint8_t Rtc_Reset(void);

uint8_t Rtc_Config(const Rtc_ConfigType* StructPtr);
void Rtc_ConfigStructInit(Rtc_ConfigType* StructPtr);

void Rtc_EnableWriteProtection(void);
void Rtc_DisableWriteProtection(void);

uint8_t Rtc_EnableInit(void);
void Rtc_DisableInit(void);

uint8_t Rtc_WaitForSynchro(void);
uint8_t Rtc_EnableRefClock(void);
uint8_t Rtc_DisableRefClock(void);
void Rtc_EnableBypassShadow(void);
void Rtc_DisableBypassShadow(void);

/** Time and Date configuration functions */
uint8_t Rtc_ConfigTime(Rtc_FormatType Format, Rtc_TimeType* TimeStructPtr);
void Rtc_ConfigTimeStructInit(Rtc_TimeType* TimeStructPtr);
void Rtc_ReadTime(Rtc_FormatType Format, Rtc_TimeType* TimeStructPtr);
uint32_t Rtc_ReadSubSecond(void);
uint8_t Rtc_ConfigDate(Rtc_FormatType Format, const Rtc_DateType* DateStructPtr);
void Rtc_ConfigDateStructInit(Rtc_DateType* DateStructPtr);
void Rtc_ReadDate(Rtc_FormatType Format, Rtc_DateType* DateStructPtr);

/**  Alarms (ALRMA A) configuration functions  */
void Rtc_ConfigAlarm(Rtc_FormatType Format, Rtc_AlarmType* AlarmStructPtr);
void Rtc_ConfigAlarmStructInit(Rtc_AlarmType* AlarmStructPtr);
void Rtc_ReadAlarm(Rtc_FormatType Format, Rtc_AlarmType* AlarmStructPtr);
void Rtc_EnableAlarm(void);
uint8_t Rtc_DisableAlarm(void);
void Rtc_ConfigAlarmSubSecond(uint32_t Val, uint8_t Mask);
uint32_t Rtc_ReadAlarmSubSecond(void);

/** Daylight Saving configuration functions */
void Rtc_ConfigDayLightSaving(Rtc_DaylightSavingType Sav, Rtc_StoreOperationType Bit);
uint32_t Rtc_ReadStoreOperation(void);

/** Digital Calibration configuration functions */
uint8_t Rtc_ConfigSmoothCalib(Rtc_ScpType Period, Rtc_ScppType Bit, uint32_t Value);

/** TimeStamp configuration functions */
void Rtc_ReadTimeDate(Rtc_FormatType Format, Rtc_TimeType* TimeStructPtr,
                      Rtc_DateType* DateStructPtr);
uint32_t Rtc_ReadTimeStampSubSecond(void);

/** Interrupts and flags management functions */
void Rtc_EnableInterrupt(uint32_t Interrupt);
void Rtc_DisableInterrupt(uint32_t Interrupt);
uint8_t Rtc_ReadStatusFlag(Rtc_FlagType Flag);
void Rtc_ClearStatusFlag(uint32_t Flag);
uint8_t Rtc_ReadIntFlag(Rtc_IntFlagType Flag);
void Rtc_ClearIntFlag(uint32_t Flag);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_RTC_H */

/**@} end of group RTC_Functions */
/**@} end of group RTC_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
