/*******************************************************************************
* Project Name      : CAN/LIN Protocol Stack
* Platform          : Arm
* Revision Number   : V1.0
* Compiled Version  : G32A1xxx_01-June-25
*
* Copyright (C) 2025 Geehy Semiconductor
*
* You may not use this file except in compliance with the GEEHY COPYRIGHT NOTICE
* (GEEHY SOFTWARE PACKAGE LICENSE).
*
* The program is only for reference, which is distributed in the hope that it
* will be useful and instructional for customers to develop their software.
* Unless required by applicable law or agreed to in writing, the program is
* distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY
* KIND, either express or implied. See the GEEHY SOFTWARE PACKAGE LICENSE for
* the governing permissions and limitations under the License.
*
*******************************************************************************/

#include "timer_hal.h"

/**************************************************************************
                    GLOBAL VARIABLE
**************************************************************************/
static uint16 gs_1msCnt = 0u;
static uint16 gs_100msCnt = 0u;

/*******************************************************************************
                               GLOBAL FUNCTIONS
*******************************************************************************/
/*!
* @brief         Get timer tick cnt for random seed.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        uint32
*/
uint32 TIMER_HAL_GETRandTimerCnt(void)
{
    uint32 uninitializedHardwareCnt = 0;
    uint32 uninitializedSeedCnt = 0;

    /* different process according to different compiler */
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
    #pragma GCC diagnostic ignored "-Wuninitialized"
#elif defined(__ICCARM__)
    #pragma diag_suppress=Pe530
#else
    #pragma clang diagnostic ignored "-Wuninitialized"
#endif

    return (((uninitializedHardwareCnt & 0xFFFFu)) | (uninitializedSeedCnt << 16u));
}

/*!
* @brief         This function is check timeout or not.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] gs_100msCnt
*
* @retval        boolean: TRUE / FALSE
*/
boolean TIMER_HAL_Modified100msTickCheck(void)
{
    boolean bStatus = FALSE;

    bStatus = (gs_100msCnt >= 100u) ? TRUE : FALSE;

    gs_100msCnt = (gs_100msCnt >= 100u) ? (gs_100msCnt - 100) : gs_100msCnt;

    return bStatus;
}

/*!
* @brief         This function checks if the 1ms counter has reached timeout.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] gs_1msCnt
*
* @retval        boolean: TRUE / FALSE
*/
boolean TIMER_HAL_Modified1msTickCheck(void)
{
    boolean bStatus = FALSE;

    bStatus = (gs_1msCnt) ? TRUE : FALSE;

    gs_1msCnt = (gs_1msCnt) ? (gs_1msCnt - 1) : gs_1msCnt;

    return bStatus;
}

/*!
* @brief         calculate gs_1msCnt and gs_100msCnt
*
* @param[in]     None
* @param[out]    None
* @param[in,out] gs_1msCnt and gs_100msCnt
*
* @retval        None
*/
void TIMER_HAL_1msTask(void)
{
    uint16 u16TempVal = gs_1msCnt + 1u;

    gs_1msCnt = (0u == u16TempVal) ? gs_1msCnt : (gs_1msCnt + 1);

    u16TempVal = gs_100msCnt + 1u;

    gs_100msCnt = (0u == u16TempVal) ? gs_100msCnt : (gs_100msCnt + 1);
}

void G32aEval_TMR2_Init()
{
    Tmr_TimeBaseType  timeBaseConfig;

    /* Enable Clock */
    Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
    Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_TMR2);

    /* Set clockDivision = 1 */
    timeBaseConfig.clockDivision =  TMR_CKD_DIV1;
    /* Up-counter */
    timeBaseConfig.counterMode =  TMR_COUNTER_MODE_UP;
    /* Set divider = 63.So TMR1 clock freq ~= 64/(63 + 1) = 1MHZ */
    timeBaseConfig.div = 63 ;
    /* Set counter = 0x9 */
    timeBaseConfig.period = 0x3E7;
    /* Repetition counter = 0x0 */
    timeBaseConfig.repetitionCounter =  0;

    Tmr_ConfigTimeBase(TMR2, &timeBaseConfig);

    /* Enable update interrupt*/
    Tmr_EnableInterrupt(TMR2, TMR_INT_UPDATE);
    Nvic_EnableIrqRequest(TMR2_IRQn, 0xf);

    /*  Enable TMR2  */
    Tmr_Enable(TMR2);
}

/*!
* @brief         initialize timer hal driver
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void TIMER_HAL_Init(void)
{
    G32aEval_TMR2_Init();
}


