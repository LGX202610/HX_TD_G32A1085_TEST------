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

#ifndef TOOLCHAIN_H_
#define TOOLCHAIN_H_

/**************************************************************************
                    MACRO DEFINITION
**************************************************************************/
#if defined __IAR_SYSTEMS_ICC__
    #define INLINE              inline
    #define INTERRUPT_FUNC      __interrupt
    #define ASM_KEYWORD         __asm volatile
    #define PACKED_STRUCT_END
    #define PACKED_STRUCT_FIELD(x)  x
    #define PACKED_STRUCT_BEGIN     __packed
    #define STRINGIZE(X) #X
    #define ALIGNEDXB(n)            _Pragma(STRINGIZE(data_alignment=n))

#elif defined __ghs__ || defined (__GNUC__)
//    #define INLINE              inline
//    #define INTERRUPT_FUNC      __interrupt
    #define ASM_KEYWORD         __asm
    #define PACKED_STRUCT_END   __attribute__((packed))
    #define PACKED_STRUCT_FIELD(x) x __attribute__((packed))
    #define PACKED_STRUCT_BEGIN
    #define ALIGNEDXB(x)        __attribute__((aligned(x)))

#elif defined __MWERKS__
    #define INLINE              inline

#elif defined __DCC__
    #define INLINE              __inline__
    #define INTERRUPT_FUNC      __interrupt__
    #define ASM_KEYWORD         __asm volatile
    #define PACKED_STRUCT_END
    #define PACKED_STRUCT_FIELD(x) x
    #define PACKED_STRUCT_BEGIN __packed__
    #define ALIGNEDXB(x)        __attribute__((aligned(x)))

#elif defined __CC_ARM
    #define INLINE          __inline
    #define INTERRUPT_FUNC      __irq
    #define ASM_KEYWORD         __asm
    #define PACKED_STRUCT_END   __attribute__((packed))
    #define PACKED_STRUCT_FIELD(x) x __attribute__((packed))
    #define PACKED_STRUCT_BEGIN
    #define ALIGNEDXB(x)        __attribute__((aligned(x)))

#else
    #error "Unknown toolchain"
#endif

#endif
