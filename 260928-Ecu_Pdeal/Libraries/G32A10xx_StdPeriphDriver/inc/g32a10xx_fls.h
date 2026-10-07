/*!
 * @file        g32a10xx_fls.h
 *
 * @brief       This file contains all the functions prototypes for the FMC firmware library
 *
 * @version     V1.0.0
 *
 * @date        2026-02-25
 *
 * @attention
 *
 *  Copyright (C) 2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (Geehy Semiconductor Software License Agreement).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the Geehy Semiconductor Software License Agreement for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef G32A10xx_FLS_H
#define G32A10xx_FLS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup FMC_Driver
  @{
*/

/** @defgroup FMC_Macros Macros
  @{
*/

/* Macros description */

/* Flash Read protection key */
#define FMC_RP_KEY            ((uint32_t)0XA5)

/* Flash key definition */
#define FMC_KEY_1             ((uint32_t)0x45670123U)
#define FMC_KEY_2             ((uint32_t)0xCDEF89ABU)

#define FMC_OB_KEY_1          ((uint32_t)0x45670123U)
#define FMC_OB_KEY_2          ((uint32_t)0xCDEF89ABU)

/* Delay definition */
#define FMC_DELAY_ERASE       ((uint32_t)0x000B0000)
#define FMC_DELAY_PROGRAM     ((uint32_t)0x00002000)

/* Address of the option byte */
#define FMC_OB_BASE            OB_BASE
#define FMC_WRP0_OB_ADDR       ((uint32_t)(OB_BASE + 0x08U))
#define FMC_WRP4_OB_ADDR       ((uint32_t)(OB_BASE + 0x10U))

/* 256K and 32K Flash devices */
/* PFLASH write protect page definition */
#define FMC_WRP_PFLASH_0_15              ((uint8_t)01) /*!< Write protection of page 0 to 3 */
#define FMC_WRP_PFLASH_16_31             ((uint8_t)02) /*!< Write protection of page 4 to 7 */
#define FMC_WRP_PFLASH_32_47             ((uint8_t)04) /*!< Write protection of page 8 to 11 */
#define FMC_WRP_PFLASH_48_63             ((uint8_t)08) /*!< Write protection of page 12 to 15 */
#define FMC_WRP_PFLASH_64_79             ((uint8_t)10) /*!< Write protection of page 16 to 19 */
#define FMC_WRP_PFLASH_80_95             ((uint8_t)20) /*!< Write protection of page 20 to 23 */
#define FMC_WRP_PFLASH_96_111            ((uint8_t)40) /*!< Write protection of page 24 to 27 */
#define FMC_WRP_PFLASH_112_127           ((uint8_t)80) /*!< Write protection of page 28 to 31 */
#define FMC_WRP_PFLASH_ALL               ((uint8_t)FF) /*!< Write protection of all Sectors */

/* DFLASH write protect page definition */
#define FMC_WRP_DFLASH_0_1               ((uint8_t)01) /*!< Write protection of page 0 to 3 */
#define FMC_WRP_DFLASH_2_3               ((uint8_t)02) /*!< Write protection of page 4 to 7 */
#define FMC_WRP_DFLASH_4_5               ((uint8_t)04) /*!< Write protection of page 8 to 11 */
#define FMC_WRP_DFLASH_6_7               ((uint8_t)08) /*!< Write protection of page 12 to 15 */
#define FMC_WRP_DFLASH_8_9               ((uint8_t)10) /*!< Write protection of page 16 to 19 */
#define FMC_WRP_DFLASH_10_11             ((uint8_t)20) /*!< Write protection of page 20 to 23 */
#define FMC_WRP_DFLASH_12_13             ((uint8_t)40) /*!< Write protection of page 24 to 27 */
#define FMC_WRP_DFLASH_14_15             ((uint8_t)80) /*!< Write protection of page 28 to 31 */
#define FMC_WRP_DFLASH_ALL               ((uint8_t)FF) /*!< Write protection of all Sectors */

/* Set Flash Latency to 2 */
#define FMC_SetWS2()  do{FMC->CTRL1_R.CTRL1_B.WS = 1;\
                          FMC->CTRL1_R.CTRL1_B.WS = 2;}while(0)

/**@} end of group FMC_Macros*/


/** @defgroup FMC_Enumerations Enumerations
  @{
*/

/**
 * @brief Flash Latency
 */
typedef enum
{
    FMC_LATENCY_0,                           /*!< Flash zero latency cycle */
    FMC_LATENCY_1,                           /*!< Flash one latency cycle */
    FMC_LATENCY_2                            /*!< Flash two latency cycle */
} Fmc_LatencyType;

/**
 * @brief   Flash definition
 */
typedef enum
{
    FMC_FLAG_BUSY = ((uint8_t)0x01),         /*!< Busy flag */
    FMC_FLAG_PE   = ((uint8_t)0x04),         /*!< Program error flag */
    FMC_FLAG_WPE  = ((uint8_t)0x10),         /*!< Write protection flag */
    FMC_FLAG_OC   = ((uint8_t)0x20),         /*!< Operation complete flag */
    FMC_FLAG_DBFI = ((uint32_t)0x00010000)   /*!< Double bit fault interrupt flag */
} Fmc_FlagType;

/**
 * @brief   Flash Status
 */
typedef enum
{
    FMC_STATE_COMPLETE = ((uint8_t)0),       /*!< Operation complete */
    FMC_STATE_BUSY     = ((uint8_t)1),       /*!< Busy */
    FMC_STATE_PG_ERR   = ((uint8_t)2),       /*!< Program error */
    FMC_STATE_WRP_ERR  = ((uint8_t)3),       /*!< Write Protection error */
    FMC_STATE_DBFI     = ((uint8_t)4),       /*!< Double bit fault */
    FMC_StateTypeIMEOUT  = ((uint8_t)5)      /*!< Time out */
} Fmc_StateType;

/**
 * @brief   Interrupt source
 */
typedef enum
{
    FMC_INT_ERROR     = ((uint32_t)0x400),      /*!< Error interrupt */
    FMC_INT_COMPLETE  = ((uint32_t)0x1000),     /*!< Operation complete interrupt */
    FMC_INT_DBFI      = ((uint32_t)0x00010000), /*!< Double bit fault interrupt */
    FMC_INT_OBRIE     = ((uint32_t)0x00020000)  /*!< OB Register Store Error Interrupt */
} Fmc_IntType;
/**
 * @brief   Wipe area selection
 */
typedef enum
{
    FMC_MER_PFLASH     = ((uint8_t)0x00),       /*!< Select PFLASH for erasure */
    FMC_MER_DFLASH     = ((uint8_t)0x01),       /*!< Select DFLASH for erasure */
    FMC_MER_ALLFLASH   = ((uint8_t)0x10)        /*!< Select PFLASH and DFLASH erasure */
} Fmc_MerType;

/**
 * @brief   Wipe len selection
 */
typedef enum
{
    FMC_LEN_64BIT     = ((uint8_t)0U),       /*!< The programming length is 64 bits */
    FMC_LEN_128BIT    = ((uint8_t)1U),       /*!< The programming length is 128 bits */
    FMC_LEN_256BIT    = ((uint8_t)2U),       /*!< The programming length is 256 bits */
    FMC_LEN_512BIT    = ((uint8_t)3U)        /*!< The programming length is 512 bits */
} Fmc_ProglenType;

/**
 * @brief   Protection Level
 */
typedef enum
{
    FMC_RDP_LEVEL_0 = ((uint8_t)0xAA),       /*!< Protection Level 0 */
    FMC_RDP_LEVEL_1 = ((uint8_t)0xBB),       /*!< Protection Level 1 */
    FMC_RDP_LEVEL_2 = ((uint8_t)0xCC)        /*!< Protection Level 2 */
} Fmc_RdpType;

/**
 * @brief   Option byte WDG mode activation
 */
typedef enum
{
    FMC_OB_IWDT_HW    = ((uint32_t)0X00000000),     /*!< activated by hardware */
    FMC_OB_IWDT_SW    = ((uint32_t)0X00010000)      /*!< activated by software */
} Fmc_Ob_IwdtType;

/**
 * @brief   Option byte STOP mode activation
 */
typedef enum
{
    FMC_OB_STOP_RESET = ((uint32_t)0X00000000),     /*!< Reset generated when entering in STOP */
    FMC_OB_STOP_NRST  = ((uint32_t)0X00020000)      /*!< No reset generated when entering in STOP */
} Fmc_Ob_StopType;

/**
 * @brief   Option byte STDBY mode activation
 */
typedef enum
{
    FMC_OB_STDBY_RESET = ((uint32_t)0X00000000),    /*!< Reset generated when entering in STDBY */
    FMC_OB_STDBY_NRST  = ((uint32_t)0X00040000)     /*!< No reset generated when entering in STDBY */
} Fmc_Ob_Stdby_T;

/**
 * @brief   Flash Option Bytes BOOT0
 */
typedef enum
{
    FMC_OB_BOOT0_RESET  = ((uint32_t)0X00000000),   /*!< BOOT0 Reset */
    FMC_OB_BOOT0_SET    = ((uint32_t)0X00080000)    /*!< BOOT0 Set */
} Fmc_Ob_Boot0Type;

/**
 * @brief   Flash Option Bytes BOOT1
 */
typedef enum
{
    FMC_OB_BOOT1_RESET    = ((uint32_t)0X00000000), /*!< BOOT1 Reset */
    FMC_OB_BOOT1_SET      = ((uint32_t)0X00100000)  /*!< BOOT1 Set */
} Fmc_Ob_Boot1Type;

/**
 * @brief   Flash Option Bytes VDDA Analog Monitoring
 */
typedef enum
{
    FMC_OB_VDDA_ANALOG_OFF = ((uint32_t)0X00000000), /*!< Analog monitoring on VDDA Power source OFF */
    FMC_OB_VDDA_ANALOG_ON  = ((uint32_t)0X00200000)  /*!< Analog monitoring on VDDA Power source ON */
} Fmc_ObVddaAnalogType;

/**@} end of group FMC_Enumerations*/

/** @defgroup FMC_Structures Structures
  @{
*/

/**
 * @brief   User Option byte config struct definition
 */
typedef struct
{
    Fmc_RdpType             READROT;
    Fmc_Ob_IwdtType         IWDTSW;
    Fmc_Ob_StopType         STOPCE;
    Fmc_Ob_Stdby_T          STDBYCE;
    Fmc_Ob_Boot0Type        BOOT0SW;
    Fmc_Ob_Boot1Type        BOOT1SW;
    Fmc_ObVddaAnalogType    VDDASW;
} Fmc_User_ConfigType;

/**
 * @brief   Option byte WRP0-3 configuration structure definition
 */
typedef struct
{
    uint8_t         WRP0;
    uint8_t         WRP1;
    uint8_t         WRP2;
    uint8_t         WRP3;
} Fmc_Wrpp_ConfigType;

/**
 * @brief   Option byte WRP4-7 configuration structure definition
 */
typedef struct
{
    uint8_t         WRP4;
    uint8_t         WRP5;
    uint8_t         WRP6;
    uint8_t         WRP7;
} Fmc_Wrpd_ConfigType;

/**@} end of group FMC_Structures*/


/** @defgroup FMC_Functions Functions
  @{
*/

/* Function description */

/* Latency */
void Fmc_SetLatency(Fmc_LatencyType Latency);

/* Prefetch Buffer */
void Fmc_EnablePrefetchBuffer(void);
void Fmc_DisablePrefetchBuffer(void);
uint8_t Fmc_ReadPrefetchBufferStatus(void);

/* Lock */
void Fmc_Unlock(void);
void Fmc_Lock(void);

/* Erase and Program */
Fmc_StateType Fmc_ErasePage(uint32_t PageAddr);
Fmc_StateType Fmc_EraseAllPages(Fmc_MerType WipeAreaSel);

Fmc_StateType Fmc_ProgramWord(uint32_t Addr, uint32_t Len, const uint32_t *Data);

/* FMC Option Bytes Programming functions */
void Fmc_UnlockOptionByte(void);
void Fmc_LockOptionByte(void);
void Fmc_LaunchOptionByte(void);
Fmc_StateType Fmc_EraseOptionByte(void);
Fmc_StateType Fmc_ProgramOptionByte(uint32_t Addr, const uint32_t *Data);
Fmc_StateType Fmc_EnableWriteProtectionPflash(Fmc_Wrpp_ConfigType WrppConfig);
Fmc_StateType Fmc_EnableWriteProtectionDflash(Fmc_Wrpd_ConfigType WrpdConfig);
Fmc_StateType Fmc_ConfigOptionByteUser(const Fmc_User_ConfigType* UserConfig);
uint8_t Fmc_ReadOptionByteUser(void);
uint32_t Fmc_ReadPflashWriteProtection(void);
uint32_t Fmc_ReadDflashWriteProtection(void);
uint8_t Fmc_GetReadProtectionStatus(void);

/* Interrupt and Flag */
void Fmc_EnableInterrupt(uint32_t Interrupt);
void Fmc_DisableInterrupt(uint32_t Interrupt);
uint8_t Fmc_ReadStatusFlag(Fmc_FlagType Flag);
uint8_t Fmc_ReadOBCSStatusFlag(Fmc_FlagType Flag);
void Fmc_ClearStatusFlag(Fmc_FlagType Flag);

/* State management */
Fmc_StateType Fmc_ReadState(void);
Fmc_StateType Fmc_WaitForReady(uint32_t TimeOut);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_FLS_H */

/**@} end of group FMC_Functions*/
/**@} end of group FMC_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/
