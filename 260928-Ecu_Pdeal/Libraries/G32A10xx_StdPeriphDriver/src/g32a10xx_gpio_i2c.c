/*!
 * @file        g32a10xx_gpio_i2c.c
 *
 * @brief       This file contains all the functions for the GPIO I2C peripheral
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
#include "g32a10xx_gpio_i2c.h"
#include "Board.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
  */

/** @addtogroup I2C_Driver
  @{
  */

/** @defgroup GPIO_I2C_Variables Variables
  @{
  */
static uint32_t s_usVar;
volatile uint32_t sysTick;
/**@} end of group GPIO_I2C_Variables */

/** @defgroup GPIO_I2C_Functions Functions
  @{
  */

/*!
 * @brief       config the system clock for I2C
 *
 * @param       I2c_Clock: i2c clock .It can be I2C_CLK_150KHz/I2C_CLK_100KHz/I2C_CLK_10KHz
 *
 * @retval      None
 *
 * @note
 */
void I2c_InitSysClock(I2c_ClkRateType I2c_Clock)
{
    /* Configure the SysTick to generate a time base equal to 1 us */
    SysTick_Config(SystemCoreClock / 1000000U);
    
    if(I2c_Clock == I2C_CLK_150KHz)
    {
        s_usVar = 1;
    }
    else if(I2c_Clock == I2C_CLK_100KHz)
    {
        s_usVar = 2;
    }
    else if(I2c_Clock == I2C_CLK_10KHz)
    {
        s_usVar = 30;
    }
    else
    {
        s_usVar = 30;
    }
}

/*!
 * @brief       Implements a simple delay function for gpio i2c driver
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_Delay(void)
{
    sysTick = 0;
    
    /*  wait until sysTick equal 1  */
    while(sysTick < s_usVar)
    {
        /* nothing */
    }
}

/*!
 * @brief       Implements a simple delay function for gpio i2c driver
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_DelayMs(uint32_t Millisecond)
{
    sysTick = 0;
    /*  wait sysTick equal Millisecond * 1000us  */
    while(sysTick <= (Millisecond * 1000U))
    {
        /* nothing */
    }
}

/*!
 * @brief       Set SDA pin to output mode
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_SdaOutputMode(void)
{
    Rcm_EnableAhbPeriphClock(I2C_SDA_CLK);
    Gpio_Config(I2C_SDA_PORT, &g_sdaGpioConfig);
}

/*!
 * @brief       Set SCL pin to output mode
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_SclOutputMode(void)
{
    Rcm_EnableAhbPeriphClock(I2C_SCL_CLK);
    Gpio_Config(I2C_SCL_PORT, &g_sclGpioConfig);
}

/*!
 * @brief       Set WP pin to output mode
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_WpOutputMode(void)
{
    Rcm_EnableAhbPeriphClock(I2C_WP_CLK);
    Gpio_Config(I2C_WP_PORT, &g_wpGpioConfig);
}

/*!
 * @brief       I2C bus start signal
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_Start(void)
{
    I2C_SDA_HIGH();
    I2C_SCL_HIGH();
    I2c_Delay();
    I2C_SDA_LOW();
    I2c_Delay();
    I2C_SCL_LOW();
}

/*!
 * @brief       I2C bus start signal
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_Stop(void)
{
    I2C_SCL_LOW();
    I2C_SDA_LOW();
    I2c_Delay();
    I2C_SCL_HIGH();
    I2c_Delay();
    I2C_SDA_HIGH();
    I2c_Delay();
}

/*!
 * @brief       I2C bus wait ACK signal
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
uint8_t I2c_WaitAck(void)
{
    uint8_t ack = 0;

    I2C_SCL_HIGH();
    I2c_Delay();
    if(I2C_SDA_READ() == 1U)
    {
        ack = 1;
    }
    else
    {
        /* nothing */ 
    }
    I2C_SCL_LOW();
    I2c_Delay();
    return ack;
}

/*!
 * @brief       I2C bus send ACK signal
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_SendAck(void)
{
    I2C_SCL_LOW();
    I2C_SDA_LOW();
    I2c_Delay();
    I2C_SCL_HIGH();
    I2c_Delay();
    I2C_SCL_LOW();
    I2C_SDA_HIGH();
}

/*!
 * @brief       I2C bus send NACK signal
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_SendNack(void)
{
    I2C_SCL_LOW();
    I2C_SDA_HIGH();
    I2c_Delay();
    I2C_SCL_HIGH();
    I2c_Delay();
    I2C_SCL_LOW();
    I2C_SDA_HIGH();
}

/*!
 * @brief       I2C bus send one byte
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_SendByte(uint8_t Byte)
{
    uint8_t i = 8;
    uint8_t bit = Byte;

    I2C_SCL_LOW();
    while(i != 0U)
    {
        i = i - 1U;
        if((bit & 0x80U) != 0U)
        {
            I2C_SDA_HIGH();
        } 
        else
        {
            I2C_SDA_LOW();
        }
        bit <<= 1;
        I2c_Delay();
        I2C_SCL_HIGH();
        I2c_Delay();
        I2C_SCL_LOW();
        I2c_Delay();
    }
}

/*!
 * @brief       I2C bus read one byte
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
uint8_t I2c_ReadByte(uint8_t Ack)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    while(i != 0U)
    {
        i = i - 1U;
        byte <<= 1;
        I2C_SCL_HIGH();
        I2c_Delay();
        if(I2C_SDA_READ() != 0U)
        {
            byte |= 0x01U;
        }
        else
        {
            /* nothing */
        }
        I2C_SCL_LOW();
        I2c_Delay();
    }
    if(Ack != 0U)
    {
        I2c_SendAck();
    }
    else
    {
        I2c_SendNack();
    }
    return byte;
}

/*!
 * @brief       Initialize the I2C gpio
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void I2c_InitGpio(void)
{ 
    I2c_SclOutputMode();
    I2c_SdaOutputMode();
    I2c_WpOutputMode();

    // enable write
    EEPROM_WP_ENABLE(); 
}

/**@} end of group GPIO_I2C_Functions */
/**@} end of group I2C_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
