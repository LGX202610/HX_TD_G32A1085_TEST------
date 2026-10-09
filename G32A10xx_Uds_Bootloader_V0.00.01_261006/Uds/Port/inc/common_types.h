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

#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

/**************************************************************************
                    OTHER TYPE DEFINITION
**************************************************************************/
typedef unsigned char boolean;

typedef unsigned int AddrType;
//typedef unsigned long long uint64;

typedef signed char sint8_t;
typedef signed short sint16_t;
typedef signed int sint32_t;
typedef signed long long sint64_t;

typedef sint8_t sint8;
typedef sint32_t sint32;

#if (!defined TYPEDEFS_H) && (!defined _STDINT) && (!defined _SYS__STDINT_H ) && (!defined _EWL_CSTDINT)
typedef sint8_t int8_t;
typedef sint16_t int16_t;
typedef sint32_t int32_t;
typedef sint64_t int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

/*!
 * @brief   Error codes
 * @details Error codes will be a unified enumeration which contains all error
 *          codes (common and specific). There will be separate spaces, each of
 *          256 positions, allocated for each functionality.
 */
typedef enum
{
    /* Generic error codes */
    STATUS_SUCC                             = 0x0000U,  /* Generic success status */
    STATUS_ERR                              = 0x0001U,  /* Generic failure status */
    STATUS_BSY                              = 0x0002U,  /* Generic busy status */
    STATUS_TOUT                             = 0x0003U,  /* Generic timeout status */
    STATUS_UNSUPPORT                        = 0x0004U,  /* Generic unsupported status */

    /* MCU error codes */
    STATUS_GATE_OFF                         = 0x0100U,   /* Module is gated off */
    STATUS_TRANSITION_FAILED                = 0x0101U,  /* Error occurs during transition */
    STATUS_STATE_INVALID                    = 0x0102U,  /* Unsupported in current state */
    STATUS_NOTIFY_BEFORE_ERROR              = 0x0103U,  /* Error occurs during send "BEFORE" notification */
    STATUS_NOTIFY_AFTER_ERROR               = 0x0104U,  /* Error occurs during send "AFTER" notification */

    /* CAN error codes */
    STATUS_CAN_NO_TRANSFER_IN_PROGRESS      = 0x0200U,  /* There is no transmission or reception in progress */
    STATUS_CAN_MB_OUT_OF_RANGE              = 0x0201U,  /* The specified MB index is out of the configurable range */

    /* UART error codes */
    STATUS_UART_ABORTED                     = 0x0500U,  /* A transfer was aborted */
    STATUS_UART_RX_OVERRUN                  = 0x0501U,  /* RX overrun error */
    STATUS_UART_TX_UNDERRUN                 = 0x0502U,  /* TX underrun error */
    STATUS_UART_NOISE_ERROR                 = 0x0503U,  /* Noise error */
    STATUS_UART_PARITY_ERROR                = 0x0504U,  /* Parity error */
    STATUS_UART_FRAMING_ERROR               = 0x0505U,  /* Framing error */
} STATUS_T;

/**************************************************************************
                    MACRO DEFINITION
**************************************************************************/
#ifndef NULL
    #define NULL ((void *)0)
#endif

#ifndef NULL_PTR
    #define NULL_PTR ((void *)0)
#endif

#ifndef FALSE
    #define FALSE 0U
#endif

#ifndef TRUE
    #define TRUE 1U
#endif

#endif
