/*!
 * @file        g32a10xx_syscfg.h
 *
 * @brief       This file contains all the functions prototypes for the SYSCFG firmware library
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

#ifndef G32A10xx_SYSCFG_H
#define G32A10xx_SYSCFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup SYSCFG_Driver
  @{
*/

/** @defgroup SYSCFG_Macros Macros
  @{
*/

/** Macros description */
#define SYSCFG_CFG1_MEMMODE      ((uint32_t)0x00000003);   /*!< SYSCFG_Memory Remap Config */

/**@} end of group SYSCFG_Macros*/

/** @defgroup SYSCFG_Enumerations Enumerations
  @{
*/

/**
 * @brief   SYSCFG EINT Port Sources
 */
typedef enum
{
    SYSCFG_PORT_GPIOA   = ((uint8_t)0x00),   /*!< Port Source GPIOA */
    SYSCFG_PORT_GPIOB   = ((uint8_t)0x01),   /*!< Port Source GPIOB */
    SYSCFG_PORT_GPIOC   = ((uint8_t)0x02),   /*!< Port Source GPIOC */
    SYSCFG_PORT_GPIOD   = ((uint8_t)0x03),   /*!< Port Source GPIOD */
    SYSCFG_PORT_GPIOF   = ((uint8_t)0x05)    /*!< Port Source GPIOF */
} Syscfg_PortTypes;

/**
 * @brief   SYSCFG EINT Pin Sources
 */
typedef enum
{
    SYSCFG_PIN_0    = ((uint8_t)0x00),       /*!< Pin Source 0 */
    SYSCFG_PIN_1    = ((uint8_t)0x01),       /*!< Pin Source 1 */
    SYSCFG_PIN_2    = ((uint8_t)0x02),       /*!< Pin Source 2 */
    SYSCFG_PIN_3    = ((uint8_t)0x03),       /*!< Pin Source 3 */
    SYSCFG_PIN_4    = ((uint8_t)0x04),       /*!< Pin Source 4 */
    SYSCFG_PIN_5    = ((uint8_t)0x05),       /*!< Pin Source 5 */
    SYSCFG_PIN_6    = ((uint8_t)0x06),       /*!< Pin Source 6 */
    SYSCFG_PIN_7    = ((uint8_t)0x07),       /*!< Pin Source 7 */
    SYSCFG_PIN_8    = ((uint8_t)0x08),       /*!< Pin Source 8 */
    SYSCFG_PIN_9    = ((uint8_t)0x09),       /*!< Pin Source 9 */
    SYSCFG_PIN_10   = ((uint8_t)0x0A),       /*!< Pin Source 10 */
    SYSCFG_PIN_11   = ((uint8_t)0x0B),       /*!< Pin Source 11 */
    SYSCFG_PIN_12   = ((uint8_t)0x0C),       /*!< Pin Source 12 */
    SYSCFG_PIN_13   = ((uint8_t)0x0D),       /*!< Pin Source 13 */
    SYSCFG_PIN_14   = ((uint8_t)0x0E),       /*!< Pin Source 14 */
    SYSCFG_PIN_15   = ((uint8_t)0x0F)        /*!< Pin Source 15 */
} Syscfg_PinTypes;

/**
 * @brief   SYSCFG Memory Remap Config
 */
typedef enum
{
    SYSCFG_MEMORY_REMAP_FMC     = ((uint8_t)0x00),         /*!< SYSCFG MemoryRemap Flash */
    SYSCFG_MEMORY_REMAP_SYSTEM  = ((uint8_t)0x01),         /*!< SYSCFG MemoryRemap SystemMemory */
    SYSCFG_MEMORY_REMAP_SRAM    = ((uint8_t)0x03)          /*!< SYSCFG MemoryRemap SRAM */
} Syscfg_MemoryRemapTypes;

/**
 * @brief   SYSCFG DMA Remap Config
 */
typedef enum
{
    SYSCFG_DAM_REMAP_ADC       = ((uint32_t)0x00000100),   /*!< ADC DMA remap */
    SYSCFG_DAM_REMAP_USART1TX  = ((uint32_t)0x00000200),   /*!< USART1 TX DMA remap */
    SYSCFG_DAM_REMAP_USART1RX  = ((uint32_t)0x00000400),   /*!< USART1 RX DMA remap */
    SYSCFG_DAM_REMAP_TMR16     = ((uint32_t)0x00000800),   /*!< Timer 16 DMA remap */
    SYSCFG_DAM_REMAP_TMR17     = ((uint32_t)0x00001000)    /*!< Timer 17 DMA remap */
} Syscfg_DmaRemapTypes;

/**
 * @brief   SYSCFG I2C FastModePlus Config
 */
typedef enum
{
    SYSCFG_I2C_FMP_PB6       = ((uint32_t)0x00010000),     /*!< I2C PB6 Fast mode plus */
    SYSCFG_I2C_FMP_PB7       = ((uint32_t)0x00020000),     /*!< I2C PB7 Fast mode plus */
    SYSCFG_I2C_FMP_PB8       = ((uint32_t)0x00040000),     /*!< I2C PB8 Fast mode plus */
    SYSCFG_I2C_FMP_PB9       = ((uint32_t)0x00080000),     /*!< I2C PB9 Fast mode plus */
    SYSCFG_I2C_FMP_I2C1      = ((uint32_t)0x00100000),     /*!< Enable Fast Mode Plus on PB10, PB11, PF6 and PF7 */
    SYSCFG_I2C_FMP_PA9       = ((uint32_t)0x00400000),     /*!< I2C PA9 Fast mode plus */
    SYSCFG_I2C_FMP_PA10      = ((uint32_t)0x00800000)      /*!< I2C PA10 Fast mode plus */
} Syscfg_I2cFmpTypes;

/**
 * @brief   SYSCFG Lock Config
 */
typedef enum
{
    SYSCFG_LOCK_LOCKUP     = ((uint32_t)0x00000001),       /*!< Cortex-M0 LOCKUP bit enable */
    SYSCFG_LOCK_SRAM       = ((uint32_t)0x00000002)        /*!< SRAM parity lock bit */
} Syscfg_LockTypes;

/**
 * @brief   SYSCFG flag
 */
typedef enum
{
    SYSCFG_CFG2_SRAMPEF     = ((uint32_t)0x00000100)       /*!< SRAM Parity error flag */
} Syscfg_FlagTypes;

/**@} end of group SYSCFG_Enumerations*/

/** @defgroup SYSCFG_Functions Functions
  @{
*/
/* Reset */
void Syscfg_Reset(void);

/* Memory Remap selects */
void Syscfg_MemoryRemapSelect(uint8_t memory);

/* DMA Channel Remap */
void Syscfg_EnableDMAChannelRemap(uint32_t channel);
void Syscfg_DisableDMAChannelRemap(uint32_t channel);

/* I2C Fast Mode Plus */
void Syscfg_EnableI2CFastModePlus(uint32_t pin);
void Syscfg_DisableI2CFastModePlus(uint32_t pin);

/* Eint Line */
void Syscfg_EintLine(Syscfg_PortTypes port, Syscfg_PinTypes pin);

/* Break lock */
void Syscfg_BreakLock(uint32_t lock);

/* Flag */
uint8_t Syscfg_ReadStatusFlag(uint32_t flag);
void Syscfg_ClearStatusFlag(uint8_t flag);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_SYSCFG_H */

/**@} end of group SYSCFG_Functions */
/**@} end of group SYSCFG_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */

