#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* 必须先包含芯片头：g32a10xx.h 用 enum {FALSE, TRUE}，若先宏定义 TRUE/FALSE 会编不过 */
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#include "g32a10xx_misc.h"

#include "common_types.h"
#include "toolchain.h"
#include "user_config.h"
#include "autolibc.h"

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

#ifndef ALLOW_APP_DEBUG
    #define APPDebugLog(...) ((void)0)
#else
    #define APPDebugLog(...) printf(__VA_ARGS__)
#endif

#ifndef EN_TP_DEBUG
    #define TPDebugLog(...) ((void)0)
#else
    #define TPDebugLog(...) printf(__VA_ARGS__)
#endif

#ifndef EN_UDS_APP_CFG_DEBUG
    #define UDSCFGDebugLog(...) ((void)0)
#else
    #define UDSCFGDebugLog(...) printf(__VA_ARGS__)
#endif

#ifndef EN_DEBUG_FLS_MODULE
    #define FlashDebugLog(...) ((void)0)
#else
    #define FlashDebugLog(...) printf(__VA_ARGS__)
#endif


#endif
