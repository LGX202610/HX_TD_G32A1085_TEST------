/*!
 * @file        g32a10xx_gpio.c
 *
 * @brief       This file contains all the functions for the GPIO peripheral
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

/* Includes */
#include "g32a10xx_gpio.h"
#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup GPIO_Driver
  @{
*/

/** @defgroup GPIO_Functions Functions
  @{
  */

/*!
 * @brief       Reset GPIO peripheral registers to their default reset values
 *
 * @param       Port:   GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOF
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_Reset(const GPIO_T* Port)
{
    if (Port == GPIOA)
    {
        Rcm_EnableAhbPeriphReset(RCM_AHB_PERIPH_GPIOA);
        Rcm_DisableAhbPeriphReset(RCM_AHB_PERIPH_GPIOA);
    }
    else if (Port == GPIOB)
    {
        Rcm_EnableAhbPeriphReset(RCM_AHB_PERIPH_GPIOB);
        Rcm_DisableAhbPeriphReset(RCM_AHB_PERIPH_GPIOB);
    }
    else if (Port == GPIOC)
    {
        Rcm_EnableAhbPeriphReset(RCM_AHB_PERIPH_GPIOC);
        Rcm_DisableAhbPeriphReset(RCM_AHB_PERIPH_GPIOC);
    }
    else if (Port == GPIOD)
    {
        Rcm_EnableAhbPeriphReset(RCM_AHB_PERIPH_GPIOD);
        Rcm_DisableAhbPeriphReset(RCM_AHB_PERIPH_GPIOD);
    }
    else if (Port == GPIOF)
    {
        Rcm_EnableAhbPeriphReset(RCM_AHB_PERIPH_GPIOF);
        Rcm_DisableAhbPeriphReset(RCM_AHB_PERIPH_GPIOF);
    }
    else
    {
        /* Nothing */
    }
}

/*!
 * @brief       Config the GPIO peripheral according to the specified parameters in the gpioConfig
 *
 * @param       Port:   GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOF
 *
 * @param       GpioConfig:     Pointer to a Gpio_ConfigType structure that
 *                              contains the configuration information for the specified GPIO peripheral
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_Config(GPIO_T* Port, const Gpio_ConfigType* GpioConfig)
{
    uint32_t i = 0;
    uint32_t bit = 0;

    for (i = 0; i < 16U; i++)
    {
        bit = (uint32_t)1 << i;

        if ((GpioConfig->pin & bit) != 0U)
        {
            if ((GpioConfig->mode == GPIO_MODE_OUT) || (GpioConfig->mode == GPIO_MODE_AF))
            {
                /* speed */
                Port->OSSEL_R.OSSEL &= ~(((uint32_t)0x03U) << (i * 2U));
                Port->OSSEL_R.OSSEL |= ((uint32_t)(GpioConfig->speed) << (i * 2U));

                /* Output mode configuration */
                Port->OMODE_R.OMODE &= ~((uint32_t)1U << i);
                Port->OMODE_R.OMODE |= (((uint32_t)GpioConfig->outtype) << i);
            }
            else
            {
                /* nothing */
            }
            /* input/output mode */
            Port->MODE_R.MODE  &= ~((uint32_t)0x03U << (i * 2U));
            Port->MODE_R.MODE |= (((uint32_t)GpioConfig->mode) << (i * 2U));

            /* Pull-up Pull down resistor configuration */
            Port->PUPD_R.PUPD &= ~((uint32_t)0x03U << (i * 2U));
            Port->PUPD_R.PUPD |= (((uint32_t)GpioConfig->pupd) << (i * 2U));
        }
        else
        {
            /* nothing */
        }

    }
}

/*!
 * @brief       Fills each Gpio_ConfigType member with its default value
 *
 * @param       GpioConfig: Pointer to a Gpio_ConfigType structure which will be initialized
 *
 * @retval      None
 */
void Gpio_ConfigStructInit(Gpio_ConfigType* GpioConfig)
{
    GpioConfig->pin     = (uint16_t)GPIO_PIN_ALL;
    GpioConfig->mode    = GPIO_MODE_IN;
    GpioConfig->outtype = GPIO_OUT_TYPE_PP;
    GpioConfig->speed   = GPIO_SPEED_10MHz;
    GpioConfig->pupd    = GPIO_PUPD_NO;
}

/*!
 * @brief       Locks GPIO Pins configuration registers
 *
 * @param       Port:   GPIOA/B peripheral
 *
 * @param       Pin:    specifies the port bit to be written
 *
 * @retval      None
 */
void Gpio_ConfigPinLock(GPIO_T* Port, uint16_t Pin)
{
    uint32_t val = 0x00010000;

    val |= Pin;
    /* Set LOCK bit */
    Port->LOCK_R.LOCK = val ;
    /* Reset LOCK bit */
    Port->LOCK_R.LOCK = Pin;
    /* Set LOCK bit */
    Port->LOCK_R.LOCK = val;
    /* Read LOCK bit*/
    (void)Port->LOCK_R.LOCK;
    /* Read LOCK bit*/
    (void)Port->LOCK_R.LOCK;
}

/*!
 * @brief       Reads the specified input port pin
 *
 * @param       Port:   GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOF
 *
 * @param       Pin:    specifies pin to read
 *
 * @retval      The input port pin value
 *
 * @note        None
 */
uint8_t Gpio_ReadInputBit(const GPIO_T* Port, uint16_t Pin)
{
    uint8_t ret = 0;

    if((Port->IDATA_R.IDATA & Pin) != 0U)
    {
        ret = BIT_SET;
    }
    else
    {
        ret = BIT_RESET;
    }

    return ret;
}

/*!
 * @brief       Reads the specified GPIO input data port
 *
 * @param       Port:   GPIO peripheral
 *
 * @retval      GPIO input data port value
 *
 * @note        None
 */
uint16_t Gpio_ReadInputPort(const GPIO_T* Port)
{
    return ((uint16_t)Port->IDATA_R.IDATA);
}

/*!
 * @brief       Reads the specified output data port bit
 *
 * @param       Port:   GPIO peripheral
 *
 * @param       Pin:    specifies pin to read
 *
 * @retval      The output port pin value
 *
 * @note        None
 */
uint8_t Gpio_ReadOutputBit(const GPIO_T* Port, uint16_t Pin)
{

    uint8_t ret = 0;

    if((Port->ODATA_R.ODATA & Pin) != 0U)
    {
        ret = BIT_SET;
    }
    else
    {
        ret = BIT_RESET;
    }

    return ret;
}

/*!
 * @brief       Reads the specified GPIO output data port
 *
 * @param       Port:   GPIO peripheral
 *
 * @retval      output data port value
 *
 * @note        None
 */
uint16_t Gpio_ReadOutputPort(const GPIO_T* Port)
{
    return ((uint16_t)Port->ODATA_R.ODATA);
}

/*!
 * @brief       Sets the selected data port bits
 *
 * @param       Port:   GPIO peripheral
 *
 * @param       Pin:    specifies the port bits to be written
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_SetBit(GPIO_T* Port, uint16_t Pin)
{
    Port->BSC_R.BSC = (uint32_t)Pin;
}

/*!
 * @brief       Clears the selected data port bits
 *
 * @param       Port:   GPIO peripheral
 *
 * @param       Pin:    specifies the port bits to be written
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_ClearBit(GPIO_T* Port, uint16_t Pin)
{
    Port->BR_R.BR = (uint32_t)Pin;
}

/*!
 * @brief       Sets or clears the selected data port bit
 *
 * @param       Port:   GPIO peripheral
 *
 * @param       Pin:    specifies the port bits to be written
 *
 * @param       BitVal
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_WriteBitValue(GPIO_T* Port, uint16_t Pin, Gpio_BsretType BitVal)
{
    if (BitVal != Bit_RESET)
    {
        Port->BSC_R.BSC = Pin;
    }
    else
    {
        Port->BR_R.BR = Pin ;
    }
}

/*!
 * @brief       Writes data to the specified GPIO data port
 *
 * @param       Port:     GPIO peripheral
 *
 * @param       PortValue:  Write value to the port output data register
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_WriteOutputPort(GPIO_T* Port, uint16_t PortValue)
{
    Port->ODATA_R.ODATA = (uint32_t)PortValue;
}

/*!
 * @brief       Changes the mapping of the specified pin
 *
 * @param       Port: GPIO peripheral
 *
 * @param       PinSource:  Specifies the pin for the Alternate function.
 *
 * @param       AfPin: Selects the pin to used as Alternate function.
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_ConfigPinAF(GPIO_T* Port, Gpio_PinSourceType PinSource, Gpio_AfType AfPin)
{
    uint32_t temp  = 0x00;
    uint32_t temp1 = 0x00;

    if ((uint8_t)PinSource <= 0x07U)
    {
        temp = (uint32_t)AfPin << ((uint8_t)PinSource * 4U);
        Port->ALFL_R.ALFL &= ~((uint32_t)0xfU << ((uint8_t)PinSource * 4U));
        Port->ALFL_R.ALFL |=  temp;
    }
    else
    {
        temp1 = (uint32_t)AfPin << (((uint8_t)PinSource & 0x07U) * 4U);
        Port->ALFH_R.ALFH &= ~((uint32_t)0xfU << (((uint8_t)PinSource & 0x07U) * 4U));
        Port->ALFH_R.ALFH |=  temp1;
    }
}

/*!
 * @brief       Toggles the selected  port bit
 *
 * @param       Port:   GPIO peripheral
 *
 * @param       Pin:    specifies the port bits to be written
 *
 * @param       BitVal
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_Toggle(GPIO_T* Port, Gpio_PinType Pin)
{
    Port->ODATA_R.ODATA ^= (uint16_t)Pin;
}

/*!
 * @brief       Gpio Set Output Type
 *
 * @param       Port:   GPIO peripheral
 *
 * @param       Pin:    specifies the port bits to be written
 *
 * @param       OutType:    out type
 *
 * @retval      None
 *
 * @note        None
 */
void Gpio_SetOutputType(GPIO_T* Port, Gpio_PinType Pin, Gpio_OutType OutType)
{
    Port->OMODE_R.OMODE &= (~(uint32_t)Pin);
    if (OutType == GPIO_OUT_TYPE_OD)
    {
        Port->OMODE_R.OMODE |= (uint32_t)Pin;
    }
    else
    {
        /* do nothing */
    }
}

/**@} end of group GPIO_Functions */
/**@} end of group GPIO_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
