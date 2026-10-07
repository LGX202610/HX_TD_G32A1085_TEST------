/*!
 * @file        flash.h
 *
 * @brief       This file provides all the FLASH firmware functions
 *
 * @version     V1.0.1
 *
 * @date        2024-03-20
 *
 * @attention
 *
 *  Copyright (C) 2023-2024 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "includes.h"

/*******************************************************************************
* Callback function prototype
*******************************************************************************/
/*! @brief Call back function pointer data type
 *
 *   If using callback in the application, any code reachable from this function
 *   must not be placed in a Flash block targeted for a program/erase operation.
 *   Functions can be placed in RAM section by using the START/END_FUNCTION_DEFINITION/DECLARATION_RAMSECTION macros.
 */
typedef void (* FLASH_CALLBACK_T)(void);

#ifdef USE_FLASH_DRIVER
#define FALSH_DRIVER_START (0x12u)
#define FALSH_DRIVER_END   (0xABu)
#endif

/*! @brief Null callback */
#define NULL_CALLBACK      ((FLASH_CALLBACK_T)0xFFFFFFFFU)

#ifndef FLASH_SDK_USING

/* Word size 2 bytes */
#define FTFx_WORD_SIZE     0x0002U
/* Long word size 4 bytes */
#define FTFx_LONGWORD_SIZE 0x0004U
/* Phrase size 8 bytes */
#define FTFx_PHRASE_SIZE   0x0008U
/* Double-phrase size 16 bytes */
#define FTFx_DPHRASE_SIZE  0x0010U

#define FEATURE_FLASH_PF_START_ADDRESS           (0x08000000U)
#define FEATURE_FLASH_PF_BLOCK_SIZE              (40000U)
#define FEATURE_FLASH_DF_START_ADDRESS           (0U)
#define FEATURE_FLASH_CFGRAM_START_ADDRESS       (0U)

#define FLASH_BASE      0x40022000
#define FLASH           ((FLASH_TypeDef*) FLASH_BASE)

// Flash Keys
#define RDPRT_KEY               ((unsigned int)    0x55AA)
#define FLASH_KEY1              ((unsigned int)0x45670123)
#define FLASH_KEY2              ((unsigned int)0xCDEF89AB)
#define FLASH_OPTKEY1           ((unsigned int)0x45670123)
#define FLASH_OPTKEY2           ((unsigned int)0xCDEF89AB)

// Flash Control Register definitions
#define FLASH_PG                ((unsigned int)0x00000001)
#define FLASH_PER               ((unsigned int)0x00000002)
#define FLASH_MER               ((unsigned int)0x00000004)
#define FLASH_OPTPG             ((unsigned int)0x00000010)
#define FLASH_OPTER             ((unsigned int)0x00000020)
#define FLASH_STRT              ((unsigned int)0x00000040)
#define FLASH_LOCK              ((unsigned int)0x00000080)
#define FLASH_OPTWRE            ((unsigned int)0x00000100)

// Flash Status Register definitions
#define FLASH_BSY               ((unsigned int)0x00000001)
#define FLASH_PGERR             ((unsigned int)0x00000004)
#define FLASH_WRPRTERR          ((unsigned int)0x00000010)
#define FLASH_EOP               ((unsigned int)0x00000020)

#define FLASH_ERR               (FLASH_PGERR | FLASH_WRPRTERR)

/*!
 * @brief Flash User Configuration Structure
 *
 * Implements : flash_user_config_t_Class
 */
typedef struct
{
    uint32_t pflashBase;            /*!< The base address of P-Flash memory */
    uint32_t pflashSize;            /*!< The size in byte of P-Flash memory */
    uint32_t dflashBase;            /*!< For CfgNVM device, this is the base address of D-Flash memory
                                     *    (CfgNVM memory); For non-CfgNVM device, this field is unused */
    uint32_t eeramBase;             /*!< The base address of CfgRAM (for CfgNVM device)
                                     *    or acceleration RAM memory (for non-CfgNVM device) */
    FLASH_CALLBACK_T callBack;      /*!< Call back function to service the time critical events. Any code reachable from this function
                                     *   must not be placed in a Flash block targeted for a program/erase operation */
} FLASH_USER_CONFIG_T;


typedef struct
{
    uint32_t pflashBase;          /*!< The base address of P-Flash memory */
    uint32_t pflashSize;          /*!< The size in byte of P-Flash memory */
    uint32_t dflashBase;          /*!< For CfgNVM device, this is the base address of D-Flash memory (CfgNVM memory);
                                   *    For non-CfgNVM device, this field is unused */
    uint32_t dflashSize;          /*!< For CfgNVM device, this is the size in byte of area
                                   *    which is used as D-Flash from CfgNVM memory;
                                   *    For non-CfgNVM device, this field is unused */
    uint32_t eeramBase;           /*!< The base address of CfgRAM (for CfgNVM device)
                                   *    or acceleration RAM memory (for non-CfgNVM device) */
    uint32_t eeeSize;             /*!< For CfgNVM device, this is the size in byte of EEPROM area which was partitioned
                                   *    from CfgRAM; For non-CfgNVM device, this field is unused */
    FLASH_CALLBACK_T callBack;    /*!< Call back function to service the time critical events. Any code reachable from this function
                                   *   must not be placed in a Flash block targeted for a program/erase operation */
} FLASH_SSD_CONFIG_T;
#endif /* FLASH_SDK_USING */

/* 本结构体内各函数指针成员顺序必须与 Flash Driver 工程一一对应 */
typedef struct
{
    STATUS_T (*FLASH_EraseSector)   (uint32_t dest, uint32_t keyVal);
    STATUS_T (*FLASH_VerifySection) (uint32_t dest);
    STATUS_T (*FLASH_Program)       (uint32_t dest, uint32_t size, const uint8_t *pData);
    STATUS_T (*FLASH_ProgramCheck)  (uint32_t dest);
} tFlashOptInfo;

/**
 * @brief   Flash definition
 */
typedef enum
{
    FMC_FLAG_BUSY = ((uint8_t)0x01),         //!< Busy flag
    FMC_FLAG_PE   = ((uint8_t)0x04),         //!< Program error flag
    FMC_FLAG_WPE  = ((uint8_t)0x10),         //!< Write protection flag
    FMC_FLAG_OC   = ((uint8_t)0x20),         //!< Operation complete flag
} FMC_FLAG_T;

// Flash Registers
typedef struct
{
    uint32_t ACR;                                             // offset  0x000
    uint32_t KEYR;                                            // offset  0x004
    uint32_t OPTKEYR;                                         // offset  0x008
    uint32_t SR;                                              // offset  0x00C
    uint32_t CR;                                              // offset  0x010
    uint32_t AR;                                              // offset  0x014
    uint32_t RESERVED0[1];
    uint32_t OBR;                                             // offset  0x01C
    uint32_t WRPR;                                            // offset  0x020
    uint32_t RESERVED1[7];
    uint32_t PROG_DATA0;
    uint32_t PROG_DATA1;
} FLASH_TypeDef;

unsigned char EraseFlashSector(const unsigned long i_ulLogicalAddr,
                               const unsigned long i_ulEraseLen);

unsigned char WriteFlash(const uint32_t i_xStartAddr,
                         const void *i_pvDataBuf,
                         const unsigned short i_usDataLen);

void InitFlash(void);

#ifdef APP_SINGLE_UPDATA
void SetFlashDriverPosition(uint32_t i_flashDriverAddr);
#endif

void InitFlashAPI(void);

/* 0x0203 哈希用：InitFlashAPI 重定位前的驱动镜像，失败返回 NULL */
const uint8_t *Flash_GetUnrelocatedDriver(void);
uint32_t Flash_GetUnrelocatedDriverMaxSize(void);

unsigned char ReadFlashByte(const unsigned long i_ulGloabalAddress);

void ReadFlashMemory(const unsigned long i_ulLogicalAddr,
                     const unsigned long i_ulLength,
                     unsigned char *o_pucDataBuf);

#endif
