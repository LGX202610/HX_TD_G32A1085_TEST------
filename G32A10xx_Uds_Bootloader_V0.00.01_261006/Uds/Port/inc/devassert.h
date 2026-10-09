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

#ifndef DEVASSERT_H
#define DEVASSERT_H

#include <stdbool.h>

#if defined (CUSTOM_DEVASSERT)
    /* If the CUSTOM_DEVASSERT symbol is defined, then add the custom implementation */
    #include CUSTOM_DEVASSERT
#elif defined (DEV_ERROR_DETECT)
    /* Implement default assert macro */
static inline void DevAssert(volatile bool x)
{
    if(x) { } else { BKPT_ASM; for(;;) {} }
}
    #define DEV_ASSERT(x) DevAssert(x)
#else
    /* Assert macro does nothing */
    #define DEV_ASSERT(x) ((void)0)
#endif

#endif /* DEVASSERT_H */
