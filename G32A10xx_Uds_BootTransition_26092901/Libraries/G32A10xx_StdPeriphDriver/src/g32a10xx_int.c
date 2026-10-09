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
 *  Copyright (C) 2025-2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "g32a10xx_int.h"
#include "timer_hal.h"
#ifdef ALLOW_CAN_TP
#include "g32a10xx_can.h"
#endif
#ifdef ALLOW_LIN_TP
#include "lin_app.h"
#endif

/** @addtogroup Examples
  @{
  */

/** @defgroup G32A1085_INT_Macros INT_Macros
  @{
  */

/**@} end of group G32A1085_INT_Macros */

/** @defgroup G32A1085_INT_Enumerations INT_Enumerations
  @{
  */

/**@} end of group G32A1085_INT_Enumerations */

/** @defgroup G32A1085_INT_Structures INT_Structures
  @{
  */

/**@} end of group G32A1085_INT_Structures */

/** @defgroup G32A1085_INT_Variables INT_Variables
  @{
  */

/**@} end of group G32A1085_INT_Variables */

/** @defgroup G32A1085_INT_Functions INT_Functions
  @{
  */

/*!
 * @brief        This function handles Hard Fault exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void HardFault_Handler(void)
{
}

/*!
 * @brief        This function handles SVCall exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void SVC_Handler(void)
{
}

/*!
 * @brief        This function handles PendSV_Handler exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief        This function handles SysTick Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void SysTick_Handler(void)
{
}

/*!
 * @brief        This function handles Tmr2 Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void TMR2_IRQHandler(void)
{
    if(Tmr_ReadIntFlag(TMR2, TMR_INT_FLAG_UPDATE) == SET)
    {
        Tmr_ClearIntFlag(TMR2, TMR_INT_FLAG_UPDATE);
        TIMER_HAL_1msTask();
    }
}

/*!
 * @brief        This function handles USART1 interrupt Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void USART1_IRQHandler(void)
{

}

/*!
 * @brief        This function handles USART2 RX interrupt Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void USART2_IRQHandler(void)
{
#ifdef ALLOW_LIN_TP
    Lin_SlaveISR();
#endif
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
#ifdef ALLOW_CAN_TP
    Can_TransferHandleIsr(CAN, s_canHandleArr[0]);
#endif
}

/**@} end of group G32A1085_INT_Functions */
/**@} end of group ADC_TMRTrigger */
/**@} end of group Examples */
