/*!
 * @file        g32a10xx_int.c
 *
 * @brief       Main Interrupt Service Routines
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
#include "g32a10xx_int.h"
#include "drv_tmr_base.h"
#include "drv_eint.h"
#include "drv_canfd.h"
#include "drv_rtc.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup Template_Examples
  @{
  */

/** @addtogroup Template
  @{
  */

/** @defgroup Template_INT_Functions INT_Functions
  @{
*/



/*!
 * @brief       This function handles Hard Fault exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void HardFault_Handler(void)
{
}

/*!
 * @brief       This function handles SVCall exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SVC_Handler(void)
{
}

/*!
 * @brief       This function handles PendSV_Handler exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief       This function handles SysTick exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SysTick_Handler(void)
{
}



/*!
 * @brief        This function handles TMR6 Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void TMR6_IRQHandler(void)
{
	drv_tmr6_irq_handle();
}

/*!
 * @brief        This function handles RTC Alarm through EINT17
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void RTC_IRQHandler(void)
{
	drv_rtc_irq_handle();
}


/*!
 * @brief        This function handles CanFD Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void FDCAN_IT0_IRQHandler(void)
{
	drv_can_irq_handle();
}


/*!
 * @brief        This function handles CanFD erm Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
 void FDCAN_SERM_IRQHandler(void)
 {
   drv_can_serm_irq_handle();
 }

 


/*!
 * @brief        This function handles USART1 RX interrupt Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void USART1_IRQHandler(void)
{
	drv_usart1_rx_irq_handle();
}



/*!
 * @brief        This function handles EINT4_15 interrupt Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void EINT4_15_IRQHandler(void)
{
	drv_eint4_15_irq_handle();
}







/**@} end of group Template_INT_Functions */
/**@} end of group Template */
/**@} end of group Template_Examples */
/**@} end of group Examples */
