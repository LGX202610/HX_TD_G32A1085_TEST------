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

#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

#include <stdio.h>

/**************************************************************************
                    MACRO DEFINITION
**************************************************************************/
//#define APP_SINGLE_UPDATA
//#define APP_BICLE_UPDATA

//#define UDS_PROJECT_FOR_BOOTLOADER  //keil里有define 正式记得注释

#define MCU_G32A10xx (1)
#define MCU_CORE_NUMBER (1u)
#define ALLOW_CAN_TP
#define ALLOW_DEBUG_IO
#define ALLOW_CRC_SW

/* 串口调试打印：1=全开  0=全关。只改这一行 */
#define UDS_DEBUG_PRINTF  1
#if (UDS_DEBUG_PRINTF != 0)
//#define ALLOW_APP_DEBUG
//#define EN_TP_DEBUG
//#define EN_UDS_APP_CFG_DEBUG
//#define EN_DEBUG_FLS_MODULE
#define EN_MAIN_DEBUG
#define EN_DEBUG_LOG
//#define EN_OTER_DEBUG
#endif

//#define ALLOW_ZLG_ZXDOC_SA_ALGORITHM
//#define USE_ECU_BUS_PRO
#define EN_AES_SA_ALGORITHM_SW 

#define SA_ALGORITHM_SEED_SIZE (4u)
#define FALSH_MEMORY_CONTINUE (0u)
#define RECEIVE_BUS_FIFO_CHAR  ('r')
#define APP_INFORMATION_SIZE   (0x00000200u)
#define EnableAllInterrupts() ResumeAllInterrupts()
#define DisableAllInterrupts() SuspendAllInterrupts()
//#define ALLOW_DELAY_TIME
//#define LONGEST_DELAY_TIME_MS (2000u)
/* Flash driver erase operation KEY */
#define FLASH_ERASE_KEY       (0xA55AA55AU)

#ifdef ALLOW_CAN_TP

    /* Boot 诊断 CAN：扩展 29 位 ID（原 ADAPTE_STD_CAN_ID 为 0x74C/0x7DF/0x75C） */
    #define USE_CAN_EXT_ID

    #if defined (ADAPTE_STD_CAN_ID)
        #define RECEIVE_ADDR       (0x74Cu)
        #define RECEIVE_FUN        (0x7DFu)
        #define TRANSMIT_RESP      (0x75Cu)
    #elif defined (USE_CAN_EXT_ID)
        #define RECEIVE_ADDR       (0x18DAA1F1u)  /* 物理寻址接收 */
        #define RECEIVE_FUN        (0x18DB33F1u)  /* 功能寻址接收 */
        #define TRANSMIT_RESP      (0x18DAF1A1u)  /* 应答发送 */
    #else
        #error "CAN ID is wrong"
    #endif

    #if (defined MCU_CORE_NUMBER) && (MCU_CORE_NUMBER < 1)
        #undef MCU_CORE_NUMBER
        #define MCU_CORE_NUMBER (1u)
    #elif (!defined MCU_CORE_NUMBER)
        #define MCU_CORE_NUMBER (1u)
    #endif

#endif

#ifdef ALLOW_LIN_TP
    #define TRANSMIT_RESP      (0x35u)
    #define RECEIVE_ADDR       (0x55u)
    #define RECEIVE_FUN        (0x7Eu)
    #define RECEIVE_BOARD_ID   (0x7Fu)
#endif

#ifdef ALLOW_CAN_TP
    #define RECEIVE_BUS_FIFO_SIZE     (300u)
#elif defined (ALLOW_LIN_TP)
    #define RECEIVE_BUS_FIFO_SIZE     (50)
#else
    #define RECEIVE_BUS_FIFO_SIZE     (50u)
#endif

#ifdef ALLOW_LIN_TP
    #define TRANSMIT_BUS_FIFO_CHAR     ('t')
    #define TRANSMIT_BUS_FIFO_SIZE     (50u)
#elif defined (ALLOW_CAN_TP)
    #define TRANSMIT_BUS_FIFO_CHAR     ('t')
    #define TRANSMIT_BUS_FIFO_SIZE     (100u)
#endif

typedef enum
{
    APP_A_ID = 0u,
#ifdef APP_SINGLE_UPDATA
    APP_A_BACKUP_ID = 1u,
#endif
#ifdef APP_BICLE_UPDATA
    APP_B_ID = 2u,
#endif
    APP_USELESS_ID = 0xFFu,
} AppIdType;

#if ((defined G32A1085) || (defined G32A1065) || (G32A1045))
    #define MCU_VENDOR_ID (MCU_G32A10xx)
#endif

#if (defined MCU_VENDOR_ID) && (MCU_VENDOR_ID == MCU_G32A10xx)

    #define FLS_DRV_BEGIN_ADDR      0x20000000u
    #define FLS_DRV_END_ADDR        0x20000400u

    /* 标定数据：DFlash 页2（页大小 512B，结束地址不含）
     * 页0: 0x08040000~0x080401FF
     * 页1: 0x08040200~0x080403FF（故障 NVM）
     * 页2: 0x08040400~0x080405FF
     */
    #define CAL_DATA_BEGIN_ADDR     0x08040400u
    #define CAL_DATA_END_ADDR       0x08040600u

    #ifdef G32A1085
        #define INFO_BEGIN_ADDR     0x20007FF0u
        #define APP_DL_SUC_ADDR     0x20007FF0u
        #define REQ_ENTER_BL_ADDR   0x20007FF1u
    #elif defined(G32A1065)
        #define INFO_BEGIN_ADDR     0x20003FF0u
        #define APP_DL_SUC_ADDR     0x20003FF0u
        #define REQ_ENTER_BL_ADDR   0x20003FF1u
    #endif

    #ifdef G32A1085
        /* App_A:90K */
        #define APP_A_BEGIN_ADDR    0x0800F600u  //61K开始
        #define APP_A_END_ADDR      0x08026400u  //153k结束

        #ifdef APP_SINGLE_UPDATA
            #define APP_A_BACKUP_BEGIN_ADDR     0x08026600u
            #define APP_A_BACKUP_END_ADDR       0x0803CE00u
        #endif
    #elif defined(G32A1065)
        /* App_A:45K */
        /* App_A starting addresss */
        #define APP_A_BEGIN_ADDR                0x08007A00u
        /* App_A end address */
        #define APP_A_END_ADDR                  0x08012E00u 
    
        #ifdef APP_SINGLE_UPDATA
            /* Backup App_A starting addresss */
            #define APP_A_BACKUP_BEGIN_ADDR     0x08013E00u
            /* Backup App_A end addresss */
            #define APP_A_BACKUP_END_ADDR       0x0801F200u
        #endif
    #endif

    #ifdef APP_BICLE_UPDATA
        #ifdef G32A1085
            #define APP_B_BEGIN_ADDR            0x00058000u
            #define APP_B_END_ADDR              0x00080000u
        #elif defined(G32A1065)
            #define APP_B_BEGIN_ADDR            0x00080000u
            #define APP_B_END_ADDR              0x000C6000u
        #endif
    #endif

#endif

#endif
