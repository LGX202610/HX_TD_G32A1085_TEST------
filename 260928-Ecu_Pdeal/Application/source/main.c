/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "hal_adc.h"
#include "hal_dma.h"
#include "hal_rtc.h"
#include "hal_iwdt.h"
#include "app_usart.h"
#include "app_power.h"
#include "app_timer.h"
#include "app_canfd.h"
#include "app_led.h"
#include "app_pedal.h"

#include "CanNm.h"

#include "uds_port.h"
#include "user_versions.h"


/** @addtogroup Examples
  @{
  */

/** @addtogroup Template_Examples
  @{
  */

/** @addtogroup Template
  @{
  */

/** @defgroup Template_Macros Macros
  @{
*/
/**@} end of group Template_Macros */

/** @defgroup Template_Functions Functions
  @{
  */

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */

uint32_t cnt_main=0;
 /* APP 链接在 0x0800F800，与 BOOT 的 APP_A_BEGIN_ADDR+0x200 对齐 */
#define APP_START_ADDR 0x0800F800u
int main(void)
{
#ifdef UDS_PROJECT_FOR_APP
	
	SCB->VTOR = APP_START_ADDR;
#endif
	
	hal_dma_init();
	hal_adc_init();
	hal_rtc_init();
	hal_iwdt_init();

	app_usart_init();
	app_power_init();
	app_led_init();
	uds_app_init();
	app_can_init();	
	app_timer_init();
	app_pedal_init();
	
	uint8_t sw[LEN_SW_VERSION]={0};
	TP_LengthType sw_len=0;
	Did_Read(0xF189,sw,&sw_len);
	printf("Enter APP Successfully--261010-->>> SW = %s\r\n",sw);
	
	while (1)
	{
//		cnt_main++;
//		if(cnt_main==220000)
//		{
//			set_pedal_l_open();
//		}
//		if(cnt_main>440000)
//		{
//			cnt_main=0;
//			set_pedal_l_close();
//		}
		
		pedal_l_stage();
		uds_app_main();
		CanNm_MainFunction();
		app_can_cyclic_main();
		app_power_main();
		ecu_power_sleep_wakeup();

		hal_iwdt_refresh();
	}
}
