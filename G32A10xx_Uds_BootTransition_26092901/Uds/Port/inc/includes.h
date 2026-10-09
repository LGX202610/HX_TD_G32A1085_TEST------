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

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include "stdint.h"
#include "g32a10xx_can.h"
#include "g32a10xx_gpio.h"
#include "g32a10xx_iwdt.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_tmr.h"
#include "g32a10xx_usart.h"
#include "common_types.h"
#include "toolchain.h"
#include "autolibc.h"
#include "user_config.h"
#include "bootloader_debug.h"


/**************************************************************************
                    MACRO DEFINITION
**************************************************************************/
#define CHECK_SINGLE_TP() \
    ((defined(ALLOW_CAN_TP) ? 1 : 0) + \
     (defined(ALLOW_LIN_TP) ? 1 : 0) + \
     (defined(EN_ETHERNET_TP) ? 1 : 0) + \
     (defined(EN_OTHERS_TP) ? 1 : 0))

#if (CHECK_SINGLE_TP() > 1)
    #error "Only one TP is allowed to be enabled!"
#elif (CHECK_SINGLE_TP() == 0)
    #error "Please enable one TP (ALLOW_CAN_TP/ALLOW_LIN_TP/EN_ETHERNET_TP/EN_OTHERS_TP)"
#endif

#ifdef ALLOW_ASSERT
    #define ASSERT(num)\
        do{\
            if(num)\
            {\
                while(1){}\
            }\
        }while(0)
#else
    #define ASSERT(num)
#endif

#ifndef EN_MAIN_DEBUG
    #define MainDebugLog(...) ((void)0)
#else
    #define MainDebugLog PrintDebugLog
#endif

#ifndef EN_OTER_DEBUG
    #define OtherDebugLog(...) ((void)0)
#else
    #define OtherDebugLog PrintDebugLog
#endif
	
#ifndef ALLOW_APP_DEBUG
    #define APPDebugLog(...) ((void)0)
#else
    #define APPDebugLog PrintDebugLog
#endif

#ifndef EN_TP_DEBUG
    #define TPDebugLog(...) ((void)0)
#else
    #define TPDebugLog PrintDebugLog
#endif

#ifndef EN_UDS_APP_CFG_DEBUG
    #define UDSCFGDebugLog(...) ((void)0)
#else
    #define UDSCFGDebugLog PrintDebugLog
#endif

#ifndef EN_DEBUG_FLS_MODULE
    #define FlashDebugLog(...) ((void)0)
#else
    #define FlashDebugLog PrintDebugLog
#endif

#endif
