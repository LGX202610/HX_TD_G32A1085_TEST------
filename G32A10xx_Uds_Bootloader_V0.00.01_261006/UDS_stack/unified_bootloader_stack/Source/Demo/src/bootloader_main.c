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

#include "bootloader_main.h"
#include "includes.h"
#include "uds_app.h"
#include "TP.h"
#include "fls_app.h"
#include "timer_hal.h"
#include "watchdog_hal.h"
#include "CRC_HAL.h"
#include "user_config.h"
#include "boot.h"


boolean SendMsgToHost(void)
{
    UDS_APP_tLocalAppMsgType tMsgInfo = {0u, 0u, {0u}, NULL_PTR};
    boolean bResult = FALSE;
    tMsgInfo.xUdsMsgId = TP_GetTransportTxID();
    tMsgInfo.xDataMsgLength = 2u;
    /* Boot工程发送0x50 0x02响应 */
    tMsgInfo.aDataBuf[0u] = 0x50u;
    tMsgInfo.aDataBuf[1u] = 0x02u;
    tMsgInfo.aDataBuf[2u] = 0x00u;
    tMsgInfo.aDataBuf[3u] = 0x32u; /* P2 = 50ms */
    tMsgInfo.aDataBuf[4u] = 0x00u;
    tMsgInfo.aDataBuf[5u] = 0xC8u; /* P2* = 2000ms, 10ms resolution */
    tMsgInfo.xDataMsgLength = 6u;
    //tMsgInfo.pfTxMsgCb = UDS_APP_ConfirmTxMessage;

    bResult = TP_DataTransferQueueFrame(tMsgInfo.xUdsMsgId,
                                        NULL_PTR,
                                        tMsgInfo.xDataMsgLength,
                                        tMsgInfo.aDataBuf);
    return bResult;
}
/**************************************************************************
                    GLOBAL FUNCTION
**************************************************************************/
/*!
* @brief         uds Task Content.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void udsTaskContent(void)
{
    boolean ret = FALSE;
#ifdef ALLOW_DEBUG_IO
    static uint16 oneMSecond = 0u;
#endif

    /** check 1ms configuration */
    ret = TIMER_HAL_Modified1msTickCheck();
    if (ret == FALSE)
    {
        /** do nothing */
    }
    else
    {
        TP_SytstemTickControl();
        UDS_APP_HandleSystemTicks();
#ifdef ALLOW_DEBUG_IO
#ifdef UDS_PROJECT_FOR_BOOTLOADER
        /* 编程会话：LED 约 2Hz 闪烁，表示正在刷写；退出后灭灯 */
        {
            static uint8 s_was_prog_sess = 0u;

            if (TRUE == UDS_APP_CheckIfProgramSession())
            {
                s_was_prog_sess = 1u;
                oneMSecond++;
                if (oneMSecond >= 250u)
                {
                    oneMSecond = 0u;
                    MakeDebugLEDProgBlink();
                }
            }
            else
            {
                oneMSecond = 0u;
                if (0u != s_was_prog_sess)
                {
                    s_was_prog_sess = 0u;
                    MakeDebugLEDProgOn();
                }
            }
        }
#else
        oneMSecond++;
    #ifdef UDS_PROJECT_FOR_APP
			oneMSecond = (oneMSecond == 250U) ? 0 : oneMSecond;
    #endif
        if (oneMSecond != 0U)
        {
            /** do nothing */
        }
        else
        {
            MakeIOForDebugUseTog();
        }
#endif
#endif
    }

    /** check 100ms configuration */
    ret = TIMER_HAL_Modified100msTickCheck();
    if (ret == FALSE)
    {
        /** do nothing */
    }
    else
    {
        WATCHDOG_HAL_Fed(); /** refresh watchdog */
		//OtherDebugLog("WATCHDOG_HAL_Fed!!!\r\n");
    }

    TP_MainFunction();
    UDS_MainFunction();
    FlashTaskRunLogic();
}

/*!
* @brief         uds Task Prepare.
*
* @param[in]     function pointer
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void udsTaskPrepare(void (*bspPrepare)(void), void (*patm)(void))
{
    boolean ret = FALSE;

	Init_CAN_STB_GPIO();
	PrepareBootloaderDebug();
	OtherDebugLog("\nHERE BOOT\r\n");
	
#ifdef UDS_PROJECT_FOR_BOOTLOADER
    uint8 btVer[] = LOADER_SOFT_VERSION;

    ret = VerifyDueToPowerOnReset();

    if (ret == TRUE)
    {
        ClearPowerOnFlags();
    }
	OtherDebugLog("BOOT VerifyDueToPowerOnReset %d\r\n",ret);
	
#ifdef ALLOW_DELAY_TIME
    ret = VerifyDueToWdtReset();

    if (ret == TRUE)
#endif
    {
        JudgeJumpToApp();
    }
#endif

    Init_DebugIO();

    if (bspPrepare != NULL_PTR)
    {
        (*bspPrepare)();
    }

    //PrepareBootloaderDebug();

    ret = CRC_HAL_Init();

    if (ret == FALSE)
    {
        OtherDebugLog("\nCRC_HAL_Init failed!\n");
    }

    TIMER_HAL_Init();
    WATCHDOG_HAL_Init();
    
    TP_Init();

#ifdef UDS_PROJECT_FOR_BOOTLOADER
    ret = FLASH_HAL_VerifyAppFlashConfiguration();

    if (ret == FALSE)
    {
        OtherDebugLog("\nFLASH_HAL_APPAddrCheck check error!\n");
    }
#endif

    UDS_Init();

#ifdef UDS_PROJECT_FOR_BOOTLOADER
    VerifyBootloaderModeOnRequest();
#endif

#ifdef UDS_PROJECT_FOR_APP
    VerifyAppStatusAfterDownlaod();
#endif

    TP_RegisterAbortTxCall(patm);
    PrepareFlsForApp();
#ifdef UDS_PROJECT_FOR_BOOTLOADER
    /** avoid compiler warning */
    (void)btVer;
    //APPDebugLog("\nBootloader SW version:%d.%d.%d\n", btVer[1u], btVer[2u], btVer[3u]);
#endif
}
