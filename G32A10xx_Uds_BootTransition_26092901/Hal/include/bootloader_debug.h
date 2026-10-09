#ifndef BOOTLOADER_DEBUG_H_
#define BOOTLOADER_DEBUG_H_

#include "includes.h"

#ifdef EN_DEBUG_TIMER
#include "debug_timer.h"
#endif

#ifdef ALLOW_DEBUG_IO
#include "debug_IO.h"
#endif

void PrepareBootloaderDebug(void);

/* 开打印时走标准 printf；关则编译成空操作。总开关在 user_config.h 的 UDS_DEBUG_PRINTF */
#if (UDS_DEBUG_PRINTF != 0)
#define PrintDebugLog printf
#else
#define PrintDebugLog(...) ((void)0)
#endif

#ifdef ALLOW_DEBUG_IO
#define MakeIOForDebugUseTog() MakeDebugIOTog()
#else
#define MakeIOForDebugUseTog()
#endif

#endif
