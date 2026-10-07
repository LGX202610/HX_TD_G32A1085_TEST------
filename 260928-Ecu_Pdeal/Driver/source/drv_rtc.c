/* Includes */
#include "drv_rtc.h"
#include "g32a10xx_rtc.h"
#include "g32a10xx_eint.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_pmu.h"
#include "g32a10xx_misc.h"
#include <stddef.h>


/*
 * 亚秒闹钟：配合 AlarmMask=RTC_MASK_ALL，按亚秒比较产生周期唤醒。
 *
 * SUBSEC 从 SynchPrediv(319) 递减到 0，再进位日历秒。
 * MASKSEL=n 时只比较 SUBSEC[n-1:0]，更高位忽略。
 *
 * MASKSEL=5：比较 SUBSEC[4:0] 与 VALUE=0x1F。
 * 每 2^5=32 个 ck_spre 节拍匹配一次。
 * ck_spre = LSICLK/(APSC+1) = 32kHz/100 = 320Hz，节拍=1/320s，约 3.125ms。
 * 周期 = 32*(1/320)s = 100ms。
 */
#define ALRMA_SUB_SECOND_VALUE    0x1F
#define ALRMA_SUB_SECOND_MASK     5


/* 低功耗唤醒闹钟，单位s： */
#define RTC_WAKEUP_SEC    10U





// 设置时分秒初始值
static Rtc_TimeType timeConfig = {
    .H12 = (uint8_t)RTC_H12_AM, 	// 24小时值无效值
    .hours = 0,                 	// 时
    .minutes = 0,               	// 分
    .seconds = 0,               	// 秒
};



// 闹钟 A 日历比较值
static Rtc_AlarmType alarmConfig = {
    .time.hours = 0,                           // 比较时：被屏蔽 
    .time.minutes = 0,                         // 比较分：被屏蔽 
    .time.seconds = RTC_WAKEUP_SEC,                        // 比较秒：
    .time.H12 = (uint8_t)RTC_H12_AM,           // 24小时值无效值
    .AlarmDateWeekDay = 1,                     // 日期 1 或星期值：被屏蔽
    .AlarmDateWeekDaySel = RTC_WEEKDAY_SEL_DATE, // 按日期比较（非按星期）；被屏蔽 
    .AlarmMask = RTC_MASK_DATEWEEK,              // RTC_MASK_ALL：MSKDATE|MSKHOUR|MSKMIN|MSKSEC 
};

/**********************************************************
  * @brief	配置闹钟 A
  * @param
  * @retval
  * @note	按秒配置，屏蔽其余时间	
 **********************************************************/
void drv_rtc_config_alarm(uint8_t seconds)
{
  alarmConfig.time.seconds = seconds;

	Rtc_ConfigAlarm(RTC_FORMAT_BIN, &alarmConfig);
}

/**********************************************************
  * @brief	drv RTC init
  * @param
  * @retval
  * @note	1Hz calendar, EINT17, ALREN off
 **********************************************************/
void drv_rtc_init(void)
{
    Eint_ConfigType eint_config;
    Rtc_ConfigType rtcConfig;

    eint_config.line = EINT_LINE17;
    eint_config.lineCmd = ENABLE;
    eint_config.mode = EINT_MODE_INTERRUPT;
    eint_config.trigger = EINT_TRIGGER_RISING;

    /*
     * calendar = RTCCLK / (AsynchPrediv+1) / (SynchPrediv+1)
     * LSI ~32kHz -> 1Hz: 32kHz / 100 / 320
     */
    rtcConfig.format = RTC_HOURFORMAT_24;
    rtcConfig.SynchPrediv = 320-1;
    rtcConfig.AsynchPrediv = 100-1;


    /* 1. 开 APB1 上 PMU 时钟，才能写备份域控制位 */
    Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_PMU);

    /* 2. 解除备份域写保护，才能改 RTC 时钟源和 RTC 寄存器 */
    Pmu_EnableBackupAccess();

    /* 3. 打开 LSI（内部约 32kHz 低速振荡），作为 RTC 时钟 */
    Rcm_EnableLsi();

    /* 4. 等 LSI 稳定（LSIRDY=1）后再切时钟，避免 RTC 无时钟 */
    while (Rcm_ReadStatusFlag(RCM_FLAG_LSIRDY) == (uint16_t)RESET)
    {
        /* do nothing */
    }
    
    /* 5. 复位 RTC 寄存器到默认值，清日历/闹钟/预分频，ALREN=0 */
    Rtc_Reset();
    
    /* 6. RTC 时钟源选 LSI（非 LSE/HSE 分频） */
    Rcm_ConfigRtcClk(RCM_RTCCLK_LSI);

    /* 7. 把选中的 RTCCLK 送到 RTC 外设 */
    Rcm_EnableRtcClk();

    /* 8. 等 APB 影子寄存器与 RTC 日历同步 */
    Rtc_WaitForSynchro();

    /* 9. 写入小时制和预分频，得到 1Hz 日历时钟 */
    Rtc_Config(&rtcConfig);


    /* 12. 预配 EINT17 上升沿 */
    Eint_Config(&eint_config);
    
    /* 13. 开 RTC 闹钟中断和 NVIC；比较器仍关 */
    Rtc_EnableInterrupt((uint32_t)RTC_INT_ALR);
    Nvic_EnableIrqRequest(RTC_IRQn, 1U);
}

/**********************************************************
  * @brief	reset RTC calendar to 00:00:00
  * @param
  * @retval
  * @note
 **********************************************************/
void drv_rtc_reset_time(void)
{
    (void)Rtc_ConfigTime(RTC_FORMAT_BIN, &timeConfig);
}


/**********************************************************
  * @brief	使能 唤醒闹钟
  * @param
  * @retval
  * @note
 **********************************************************/
void drv_rtc_enable_wakeup_alarm(void)
{
    drv_rtc_reset_time();

    Rtc_ClearIntFlag((uint32_t)RTC_INT_FLAG_ALR);
    Eint_ClearIntFlag((uint32_t)EINT_LINE17);

    Rtc_EnableAlarm();
}


/**********************************************************
  * @brief	关闭 唤醒闹钟
  * @param
  * @retval
  * @note
 **********************************************************/
void drv_rtc_disable_wakeup_alarm(void)
{
    Rtc_DisableAlarm();

    drv_rtc_reset_time();
}


/**********************************************************
  * @brief	注册 RTC 中断钩子
  * @param
  * @retval
  * @note
 **********************************************************/
static Drv_Rtc_IrqHookFunc_t drv_rtc_irq_hook = NULL;
void drv_register_rtc_irq_hook(Drv_Rtc_IrqHookFunc_t hook_func)
{
    drv_rtc_irq_hook = hook_func;
}

/**********************************************************
  * @brief	RTC 闹钟中断
  * @param
  * @retval
  * @note
 **********************************************************/
void drv_rtc_irq_handle(void)
{

    if(Rtc_ReadIntFlag(RTC_INT_FLAG_ALR) == 1U)
    {
        /* The interrupt flag bit must be cleared first. */
        Rtc_ClearIntFlag((uint32_t)RTC_INT_FLAG_ALR);
        /* Clear the EINT17 interrupt flag which is related to RTC alarm interrupt */
        Eint_ClearIntFlag((uint32_t)EINT_LINE17);

        /* reload the time of RTC*/
        drv_rtc_reset_time();
		

        if(drv_rtc_irq_hook != NULL)
        {
            drv_rtc_irq_hook();
        }
    }
    else
    {
        // drv_rtc_enable_wakeup_alarm();
    }
}
