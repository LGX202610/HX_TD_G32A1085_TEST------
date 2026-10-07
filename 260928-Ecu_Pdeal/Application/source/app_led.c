/* Includes */
#include "app_led.h"
#include "hal_led.h"
#include "app_timer.h"



#define LED_BLINK_FOREVER			0u	/* blink_cnt = LED_BLINK_FOREVER，持续灯语 */

#define LED_PREEMPT_BLANK_TICKS		2u	/* 该灯抢占时先灭几拍，视距换挡 */


/* 灯语优先级 */
typedef enum
{
	APP_LED_PRIO_0 = 0,	/* 最低，底 */
	APP_LED_PRIO_1,
	APP_LED_PRIO_2,
	APP_LED_PRIO_3,	
	APP_LED_PRIO_4		/* 当前最高 */
} AppLedPrio_t;
/*
	优先级分配约束：

	APP_LED_PRIO_0：

	APP_LED_PRIO_1：系统正常运行、状态类提示

	APP_LED_PRIO_2：异常状态类提示

	APP_LED_PRIO_3：故障或警报，可运行

	APP_LED_PRIO_4：严重异常，不可运行
*/

/* 灯语结构体 */
typedef struct
{
	uint8_t  steady;		/* 1=常亮或常灭，不走闪烁节拍 */
	uint8_t  level;			/* steady=1 时：1 亮，0 灭 */
	uint16_t on_ticks;		/* 亮相位，单位 10ms */
	uint16_t off_ticks;		/* 灭相位，单位 10ms；频率由 on+off 决定 */
	uint16_t blink_cnt;		/* 亮+灭=1 次；FOREVER=0一直闪 */
	uint8_t  prio;			/* 仅在同一通道内比较，越大越高 */
} AppLedPattern_t;





/* 下标 = AppLedIndicate_t */
static const AppLedPattern_t s_led_tbl[] =
{
	/* IDLE */
	[APP_LED_IND_IDLE] = 			{ 1u, 0u, 0u,   0u,   LED_BLINK_FOREVER, APP_LED_PRIO_0 },

	/* ON */
	[APP_LED_IND_ON] = 				{ 1u, 1u, 0u,   0u,   LED_BLINK_FOREVER, APP_LED_PRIO_1 },

	/* NORMAL：约 0.5Hz */
	[APP_LED_IND_NORMAL] = 			{ 0u, 0u, 100u, 100u, LED_BLINK_FOREVER, APP_LED_PRIO_1 },

	/* VIN_LOW：约 2.5Hz */
	[APP_LED_IND_VIN_LOW] = 		{ 0u, 0u, 20u,  20u,  LED_BLINK_FOREVER, APP_LED_PRIO_3 },

	/* VIN_HIGH：约 2.5Hz */
	[APP_LED_IND_VIN_HIGH] = 		{ 0u, 0u, 20u,  20u,  LED_BLINK_FOREVER, APP_LED_PRIO_3 },

	/* PEDAL_PINCH：约 2.5Hz */
	[APP_LED_IND_PEDAL_PINCH] = 	{ 0u, 0u, 20u,  20u,  2, APP_LED_PRIO_2 },

	/* MOTOR_OC：约 2.5Hz */
	[APP_LED_IND_MOTOR_OC] = 		{ 0u, 0u, 20u,  20u,  LED_BLINK_FOREVER, APP_LED_PRIO_3 },

	/* MOTOR_SC：约 2.5Hz */
	[APP_LED_IND_MOTOR_SC] = 		{ 0u, 0u, 20u,  20u,  LED_BLINK_FOREVER, APP_LED_PRIO_4 },

	/* DRVER_FAULT：约 2.5Hz */
	[APP_LED_IND_DRVER_FAULT] = 	{ 0u, 0u, 20u,  20u,  LED_BLINK_FOREVER, APP_LED_PRIO_4 },

	/* FAULT：约 5Hz、5 次 */
	[APP_LED_IND_FAULT] = { 0u, 0u, 10u,  10u,  5u,                 4u }
};




/* 通道引擎 */
typedef struct
{
	AppLedIndicate_t ind;	/* 该灯当前场景 */
	uint16_t tick;			/* 当前相位已走拍数 */
	uint16_t remain_cnt;	/* 有限次剩余；FOREVER 不用 */
	uint8_t  phase_on;		/* 1=亮相位，0=灭相位（只描述时间轴，不再取反） */
	uint8_t  frozen;		/* 1=冻结，0=运行 */
	uint8_t  blank_left;	/* 仅本灯抢占空窗，即从上次亮到下次亮之间空着的拍数 */
} AppLedChEngine_t;

static AppLedChEngine_t s_led_ch[APP_LED_CH_USED];	/* 通道引擎 */



/* 映射通道 → HAL */
static void (*const s_led_hal[APP_LED_CH_USED])(LedState) =
{
	hal_set_led1_state,
	hal_set_led2_state
};

/* 通道写入 */
static void led_write_ch(AppLedChannel_t ch, uint8_t on)
{
	s_led_hal[ch]((on != 0u) ? _led_on : _led_off);
}

/**********************************************************
  * @brief	LED 通道写入
  * @param  ch: 通道
  * @retval 
  * @note	
 **********************************************************/
static void led_apply_ch(AppLedChannel_t ch)
{
	const AppLedPattern_t *p = &s_led_tbl[s_led_ch[ch].ind];

	/* 如果通道是常亮或常灭，则写入常亮或常灭 */
	if (p->steady != 0u)
	{
		led_write_ch(ch, p->level);
	}
	/* 如果通道是闪烁，则写入闪烁 */
	else
	{
		led_write_ch(ch, s_led_ch[ch].phase_on);
	}
}

/**********************************************************
  * @brief	LED 引擎加载
  * @param  ch: 通道
  * @param  ind: 指示
  * @param  from_preempt: 是否抢占
  * @retval 
  * @note	如果通道被抢占，则先灭几拍，视距换挡
 **********************************************************/
static void led_engine_load(AppLedChannel_t ch, AppLedIndicate_t ind, uint8_t from_preempt)
{
	const AppLedPattern_t *p = &s_led_tbl[ind];
	AppLedChEngine_t *e = &s_led_ch[ch];

	e->ind = ind;
	e->tick = 0u;
	e->remain_cnt = p->blink_cnt;
	e->phase_on = 1u;

	/* 如果通道被抢占，则先灭几拍，视距换挡 */
	if (from_preempt != 0u)
	{
		e->blank_left = LED_PREEMPT_BLANK_TICKS;	
		led_write_ch(ch, 0u);
		return;
	}

	e->blank_left = 0u;
	led_apply_ch(ch);
}

/**********************************************************
  * @brief	LED 引擎定时器
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void led_timer_ch(AppLedChannel_t ch)
{
	AppLedChEngine_t *e = &s_led_ch[ch];
	const AppLedPattern_t *p;

	/* 如果通道被冻结，则不运行 */
	if (e->frozen != 0u)
	{
		return;
	}

	/* 如果通道有抢占空窗，则先灭几拍，视距换挡 */
	if (e->blank_left > 0u)
	{
		e->blank_left--;

		/* 抢占空窗未结束，不亮灯 */
		if (e->blank_left > 0u)
		{
			return;
		}

		/* 抢占空窗结束，亮灯 */
		e->tick = 0u;
		e->phase_on = 1u;
		led_apply_ch(ch);

		return;
	}

	p = &s_led_tbl[e->ind];
	/* 如果通道是常亮或常灭，则不运行 */
	if (p->steady != 0u)
	{
		return;
	}
	/* 如果通道是闪烁，则走拍 */
	e->tick++;

	/* 如果通道是亮相位，则走亮相位 */
	if (e->phase_on != 0u)
	{
		if (e->tick < p->on_ticks)
		{
			return;
		}
		/* 亮相位结束，走灭相位 */
		e->tick = 0u;
		e->phase_on = 0u;
		led_apply_ch(ch);
		return;
	}

	/*  */
	if (e->tick < p->off_ticks)
	{
		return;
	}

	/* 灭相位结束，走亮相位 */
	e->tick = 0u;
	e->phase_on = 1u;

	/* 如果通道是有限次闪烁，则走有限次闪烁 */
	if (p->blink_cnt != LED_BLINK_FOREVER)
	{
		if (e->remain_cnt > 0u)
		{
			e->remain_cnt--;
		}
		if (e->remain_cnt == 0u)
		{
			led_engine_load(ch, APP_LED_IND_IDLE, 0u);
			return;
		}
	}

	led_apply_ch(ch);
}

/**********************************************************
  * @brief	LED 定时器
  * @param  
  * @retval 
  * @note		
 **********************************************************/
static void led_timer(void)
{
	AppLedChannel_t ch;

	for (ch = 0u; ch < APP_LED_CH_USED; ch++)
	{
		led_timer_ch(ch);
	}
}

/**********************************************************
  * @brief	LED 指示
  * @param  ch: 通道
  * @param  ind: 指示
  * @retval 
  * @note		
 **********************************************************/
void app_led_indicate(AppLedChannel_t ch, AppLedIndicate_t ind)
{
	/* 如果通道无效，则不执行 */
	if (ch >= APP_LED_CH_USED)
	{
		return;
	}
	/* 如果指示无效，则不执行 */
	if ((uint32_t)ind >= (sizeof(s_led_tbl) / sizeof(s_led_tbl[0])))
	{
		return;
	}
	/* 如果指示与当前指示相同，则不执行 */
	if (ind == s_led_ch[ch].ind)
	{
		return;
	}
	/* 如果指示优先级小于当前指示优先级，则不执行 */
	if (s_led_tbl[ind].prio <= s_led_tbl[s_led_ch[ch].ind].prio)
	{
		return;
	}
	/* 加载指示 */
	led_engine_load(ch, ind, 1u);
}

/**********************************************************
  * @brief	LED 清除
  * @param  ch: 通道
  * @param  ind: 指示
  * @retval 
  * @note		
 **********************************************************/
void app_led_clear(AppLedChannel_t ch, AppLedIndicate_t ind)
{
	/* 如果通道无效，则不执行 */
	if (ch >= APP_LED_CH_USED)
	{
		return;
	}
	/* 如果指示无效，则不执行 */
	if ((uint32_t)ind >= (sizeof(s_led_tbl) / sizeof(s_led_tbl[0])))
	{
		return;
	}
	/* 如果指示与当前指示相同，则清除 */
	if (s_led_ch[ch].ind == ind)
	{
		led_engine_load(ch, APP_LED_IND_IDLE, 0u);
	}
}

/**********************************************************
  * @brief	LED 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_led_init(void)
{
	AppLedChannel_t ch;

	hal_led_init();

	for (ch = 0u; ch < APP_LED_CH_USED; ch++)
	{
		s_led_ch[ch].frozen = 0u;
		led_engine_load(ch, APP_LED_IND_IDLE, 0u);
	}

	app_register_timer_callback(led_timer, TIMER_10MS_CALLBACK_TYPE);
	
	app_led_indicate(0,APP_LED_IND_NORMAL);
}

/**********************************************************
  * @brief	LED 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_led_sleep(void)
{
	AppLedChannel_t ch;

	for (ch = 0u; ch < APP_LED_CH_USED; ch++)
	{
		s_led_ch[ch].frozen = 1u;
		s_led_ch[ch].blank_left = 0u;
		led_write_ch(ch, 0u);
	}
}

/**********************************************************
  * @brief	LED 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void app_led_wakeup(void)
{
	AppLedChannel_t ch;

	hal_led_init();

	for (ch = 0u; ch < APP_LED_CH_USED; ch++)
	{
		s_led_ch[ch].frozen = 0u;
		led_engine_load(ch, s_led_ch[ch].ind, 0u);
	}
	
	app_led_indicate(0,APP_LED_IND_ON);
}


// /**********************************************************
//   * @brief	LED 定时器
//   * @param  
//   * @retval 
//   * @note		
//  **********************************************************/
// static void led_timer(void)
// {
// 	static uint16_t cnt_1s=0;
	
// 	++cnt_1s;
// 	if(cnt_1s == 100)
// 	{
// 		hal_set_led1_state(_led_on);
// 		hal_set_led2_state(_led_off);
// 	}
// 	else if(cnt_1s >= 200)
// 	{
// 		cnt_1s=0;
// 		hal_set_led1_state(_led_off);
// 		hal_set_led2_state(_led_on);
// 	}
// }



// /**********************************************************
//   * @brief	LED 初始化
//   * @param  
//   * @retval 
//   * @note		
//  **********************************************************/
// void app_led_init(void)
// {
// 	hal_led_init();
	
// 	hal_set_led1_state(_led_off);
// 	hal_set_led2_state(_led_off);
	
// 	app_register_timer_callback(led_timer, TIMER_10MS_CALLBACK_TYPE);
// }



// /**********************************************************
//   * @brief	LED 休眠
//   * @param  
//   * @retval 
//   * @note		
//  **********************************************************/
// void app_led_sleep(void)
// {
// 	hal_set_led1_state(_led_off);
// 	hal_set_led2_state(_led_off);
// }

// /**********************************************************
//   * @brief	LED 唤醒
//   * @param  
//   * @retval 
//   * @note		
//  **********************************************************/
// void app_led_wakeup(void)
// {
// 	hal_led_init();
	
// 	hal_set_led1_state(_led_off);
// 	hal_set_led2_state(_led_on);
// }
