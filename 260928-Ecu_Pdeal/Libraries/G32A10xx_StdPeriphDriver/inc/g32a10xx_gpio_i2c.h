/*!
 * @file        g32a10xx_gpio_i2c.h
 *
 * @brief       This file contains the headers of the interrupt handlers
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

/*
* MISRA-C:2012 compliance checks
*
*       g32a10xx_gpio_i2c_h_MISRA_REF1
*          Breaks the required Rule-8.5 of MISRA 2012 guidelines,
*          An external object or function shall be declared once in one and only one file
*/

/* Define to prevent recursive inclusion */
#ifndef G32A10xx_GPIO_I2C_H
#define G32A10xx_GPIO_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"
#include "g32a10xx_gpio.h"
/** @defgroup GPIO_I2C_Variables
  @{
  */
extern volatile uint32_t sysTick;
/*** @warning g32a10xx_gpio_i2c_h_MISRA_REF1 Multiple declarations of external object or function. */
/*** @warning g32a10xx_gpio_i2c_h_MISRA_REF1 The global identifier 'g_sdaGpioConfig' has been declared in more than one file. */
extern Gpio_ConfigType g_sdaGpioConfig;
/*** @warning g32a10xx_gpio_i2c_h_MISRA_REF1 Multiple declarations of external object or function. */
/*** @warning g32a10xx_gpio_i2c_h_MISRA_REF1 The global identifier 'g_sclGpioConfig' has been declared in more than one file. */
extern Gpio_ConfigType g_sclGpioConfig;
/*** @warning g32a10xx_gpio_i2c_h_MISRA_REF1 Multiple declarations of external object or function. */
/*** @warning g32a10xx_gpio_i2c_h_MISRA_REF1 The global identifier 'g_wpGpioConfig' has been declared in more than one file. */
extern Gpio_ConfigType g_wpGpioConfig;
/**@} end of group GPIO_I2C_Variables */


/** @addtogroup G32A10xx_StdPeriphDriver
  @{
  */

/** @addtogroup I2C_Driver
  @{
  */

/** @defgroup GPIO_I2C_Macros Macros
  @{
  */

/* pin define */
#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN    GPIO_PIN_7
#endif

#ifndef I2C_SDA_PORT
#define I2C_SDA_PORT   GPIOB
#endif

#ifndef I2C_SDA_CLK
#define I2C_SDA_CLK    RCM_AHB_PERIPH_GPIOB
#endif

#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN    GPIO_PIN_6
#endif

#ifndef I2C_SCL_PORT
#define I2C_SCL_PORT   GPIOB
#endif

#ifndef I2C_SCL_CLK
#define I2C_SCL_CLK    RCM_AHB_PERIPH_GPIOB
#endif

#ifndef I2C_WP_PIN
#define I2C_WP_PIN     GPIO_PIN_10
#endif

#ifndef I2C_WP_PORT
#define I2C_WP_PORT    GPIOB
#endif

#ifndef I2C_WP_CLK
#define I2C_WP_CLK     RCM_AHB_PERIPH_GPIOB
#endif



/* WP control */
#define EEPROM_WP_ENABLE()  Gpio_SetBit(I2C_WP_PORT, (uint16_t)I2C_WP_PIN) 
#define EEPROM_WP_DISABLE() Gpio_ClearBit(I2C_WP_PORT, (uint16_t)I2C_WP_PIN)

/* I2C bus level */
#define I2C_SDA_HIGH()  Gpio_SetBit(I2C_SDA_PORT, (uint16_t)I2C_SDA_PIN)
#define I2C_SDA_LOW()   Gpio_ClearBit(I2C_SDA_PORT, (uint16_t)I2C_SDA_PIN)

#define I2C_SCL_HIGH()  Gpio_SetBit(I2C_SCL_PORT, (uint16_t)I2C_SCL_PIN)
#define I2C_SCL_LOW()   Gpio_ClearBit(I2C_SCL_PORT, (uint16_t)I2C_SCL_PIN)

#define I2C_SDA_READ()  Gpio_ReadInputBit(I2C_SDA_PORT, (uint16_t)I2C_SDA_PIN)

/**@} end of group GPIO_I2C_Macros */


/** @defgroup GPIO_I2C_Enumerations Enumerations
  @{
  */

/**
 * @brief   I2C clock rate
 */
typedef enum
{
    I2C_CLK_150KHz = 1,     /*!< I2C clock rate is 150KHz */
    I2C_CLK_100KHz,         /*!< I2C clock rate is 100KHz */
    I2C_CLK_10KHz           /*!< I2C clock rate is  10KHz */
}I2c_ClkRateType;

/**@} end of group GPIO_I2C_Enumerations */


/** @defgroup GPIO_I2C_Functions Functions
  @{
  */
void I2c_Delay(void);
void I2c_DelayMs(uint32_t Millisecond);
void I2c_SdaOutputMode(void);
void I2c_SclOutputMode(void);
void I2c_WpOutputMode(void);
void I2c_Start(void);
void I2c_Stop(void);
uint8_t I2c_WaitAck(void);
void I2c_SendAck(void);
void I2c_SendNack(void);
void I2c_SendByte(uint8_t Byte);
uint8_t I2c_ReadByte(uint8_t Ack);
void I2c_InitGpio(void);
void I2c_InitSysClock(I2c_ClkRateType I2c_Clock);

#ifdef __cplusplus
}
#endif

#endif /*__24Cxx_H */

/**@} end of group GPIO_I2C_Functions */
/**@} end of group I2C_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */


