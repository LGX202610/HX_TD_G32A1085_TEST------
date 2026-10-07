/*!
 * @file        g32a10xx_syscfg.c
 *
 * @brief       This file contains all the functions for the SYSCFG peripheral
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

#include "g32a10xx_syscfg.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup SYSCFG_Driver
  @{
*/

/** @defgroup SYSCFG_Functions Functions
  @{
*/

/*!
 * @brief        Set SYSCFG CFG0/1 EINTCFG1/2/3/4 register to reset value
 *
 * @param        None
 *
 * @retval       None
 */
void Syscfg_Reset(void)
{
    SYSCFG->CFG1_R.CFG1 &= (uint32_t) SYSCFG_CFG1_MEMMODE;
    SYSCFG->EINTCFG1_R.EINTCFG1 = 0;
    SYSCFG->EINTCFG2_R.EINTCFG2 = 0;
    SYSCFG->EINTCFG3_R.EINTCFG3 = 0;
    SYSCFG->EINTCFG4_R.EINTCFG4 = 0;
    SYSCFG->CFG2_R.CFG2 |= (uint32_t) SYSCFG_CFG2_SRAMPEF;
}

/*!
 * @brief       SYSCFG Memory Remap selects
 *
 * @param       memory: selects the memory remapping
 *                      The parameter can be one of following values:
 *                      @arg SYSCFG_MEMORY_REMAP_FMC:      SYSCFG MemoryRemap Flash
 *                      @arg SYSCFG_MEMORY_REMAP_SYSTEM:   SYSCFG MemoryRemap SystemMemory
 *                      @arg SYSCFG_MEMORY_REMAP_SRAM:     SYSCFG MemoryRemap SRAM
 *
 * @retval      None
 */
void Syscfg_MemoryRemapSelect(uint8_t memory)
{
    SYSCFG->CFG1_R.CFG1_B.MMSEL = (uint8_t)memory;
}

/*!
 * @brief       Enables SYSCFG DMA Channel Remap
 *
 * @param       channel: selects the DMA channels remap.
 *                       The parameter can be any combination of following values:
 *                       @arg SYSCFG_DAM_REMAP_ADC:      ADC DMA remap
 *                       @arg SYSCFG_DAM_REMAP_USART1TX: USART1 TX DMA remap
 *                       @arg SYSCFG_DAM_REMAP_USART1RX: USART1 RX DMA remap
 *                       @arg SYSCFG_DAM_REMAP_TMR16:    Timer 16 DMA remap
 *                       @arg SYSCFG_DAM_REMAP_TMR17:    Timer 17 DMA remap
 *
 * @retval      None
 */
void Syscfg_EnableDMAChannelRemap(uint32_t channel)
{
    SYSCFG->CFG1_R.CFG1 |= (uint32_t)channel;
}

/*!
 * @brief       Disables SYSCFG DMA Channel Remap
 *
 * @param       channel: selects the DMA channels remap.
 *                       The parameter can be any combination of following values:
 *                       @arg SYSCFG_DAM_REMAP_ADC:      ADC DMA remap
 *                       @arg SYSCFG_DAM_REMAP_USART1TX: USART1 TX DMA remap
 *                       @arg SYSCFG_DAM_REMAP_USART1RX: USART1 RX DMA remap
 *                       @arg SYSCFG_DAM_REMAP_TMR16:    Timer 16 DMA remap
 *                       @arg SYSCFG_DAM_REMAP_TMR17:    Timer 17 DMA remap
 *
 * @retval      None
 */
void Syscfg_DisableDMAChannelRemap(uint32_t channel)
{
    SYSCFG->CFG1_R.CFG1 &= (uint32_t)~channel;
}

/*!
 * @brief       Enables SYSCFG I2C Fast Mode Plus
 *
 * @param       pin:     selects the pin.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_I2C_FMP_PB6:    I2C PB6 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PB7:    I2C PB7 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PB8:    I2C PB8 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PB9:    I2C PB9 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PA9:    I2C PA9 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PA10:   I2C PA10 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_I2C1:   PB10, PB11, PF6 and PF7
 *
 * @retval      None
 */
void Syscfg_EnableI2CFastModePlus(uint32_t pin)
{
    SYSCFG->CFG1_R.CFG1 |= (uint32_t)pin;
}

/*!
 * @brief       Disables SYSCFG I2C Fast Mode Plus
 *
 * @param       pin:     selects the pin.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_I2C_FMP_PB6:    I2C PB6 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PB7:    I2C PB7 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PB8:    I2C PB8 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PB9:    I2C PB9 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PA9:    I2C PA9 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_PA10:   I2C PA10 Fast mode plus
 *                       @arg SYSCFG_I2C_FMP_I2C1:   PB10, PB11, PF6 and PF7
 *
 * @retval      None
 */
void Syscfg_DisableI2CFastModePlus(uint32_t pin)
{
    SYSCFG->CFG1_R.CFG1 &= (uint32_t)~pin;
}

/*!
 * @brief       Selects the GPIO pin used as EINT Line.
 *
 * @param       port:   selects the port can be GPIOA/B/C/D/F
 *
 * @param       pin:    selects the pin can be SYSCFG_PIN_(0..15)
 *
 * @retval      None
 */

void Syscfg_EintLine(Syscfg_PortTypes port, Syscfg_PinTypes pin)
{
    uint32_t status;
    uint32_t mask;

    status = (((uint32_t)0x0FU) & (uint8_t)port) << (0x04U * ((uint8_t)pin & 0x03U));
    mask = ((uint32_t)0x0FU) << (0x04U * ((uint8_t)pin & 0x03U));

    if ((uint8_t)pin <= 0x03U)
    {
        SYSCFG->EINTCFG1_R.EINTCFG1 &= ~mask;
        SYSCFG->EINTCFG1_R.EINTCFG1 |= status;
    }
    else if ((uint8_t)pin <= 0x07U)
    {
        SYSCFG->EINTCFG2_R.EINTCFG2 &= ~mask;
        SYSCFG->EINTCFG2_R.EINTCFG2 |= status;
    }
    else if ((uint8_t)pin <= 0x0BU)
    {
        SYSCFG->EINTCFG3_R.EINTCFG3 &= ~mask;
        SYSCFG->EINTCFG3_R.EINTCFG3 |= status;
    }
    else if ((uint8_t)pin <= 0x0FU)
    {
        SYSCFG->EINTCFG4_R.EINTCFG4 &= ~mask;
        SYSCFG->EINTCFG4_R.EINTCFG4 |= status;
    }
    else
    {
        /* nothing */
    }
}

/*!
 * @brief       Selected parameter to the break input of TMR1.
 *
 * @param       lock:   selects the configuration to break
 *                      The parameter can be one of following values:
 *                      @arg SYSCFG_LOCK_LOCKUP: Cortex-M0 LOCKUP bit
 *                      @arg SYSCFG_LOCK_SRAM:  SRAM parity lock bit
 *
 * @retval      None
 */
void Syscfg_BreakLock(uint32_t lock)
{
    SYSCFG->CFG2_R.CFG2_B.LOCK = 0;

    if (lock == (uint32_t)SYSCFG_LOCK_LOCKUP)
    {
        SYSCFG->CFG2_R.CFG2_B.LOCK = BIT_SET;
    }
    else
    {
        /* nothing */
    }
}

/*!
 * @brief       Read the specified SYSCFG flag
 *
 * @param       flag:   SRAM Parity error flag
 *                      @arg SYSCFG_CFG2_SRAMPEF
 *
 * @retval      None
 */
uint8_t Syscfg_ReadStatusFlag(uint32_t flag)
{
    uint32_t status;
    uint8_t ret = (uint8_t)RESET;

    status = (uint32_t)(SYSCFG->CFG2_R.CFG2 & flag);

    if (status == flag)
    {
        ret = SET;
    }
    else
    {
        /* nothing */
    }

    return ret;
}

/*!
 * @brief       Clear the specified SYSCFG flag
 *
 * @param       flag:   SRAM Parity error flag
 *                      @arg SYSCFG_CFG2_SRAMPEF
 *
 * @retval      None
 */
void Syscfg_ClearStatusFlag(uint8_t flag)
{
    SYSCFG->CFG2_R.CFG2 |= (uint32_t) flag;
}

/**@} end of group SYSCFG_Functions*/
/**@} end of group SYSCFG_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/
