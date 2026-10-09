/*!
 * @file        g32a10xx_gpio.h
 *
 * @brief       This file contains all the functions prototypes for the GPIO firmware library
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
#ifndef G32A10xx_GPIO_H
#define G32A10xx_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup GPIO_Driver
  @{
*/

/** @defgroup GPIO_Enumerations Enumerations
  @{
*/

/**
 * @brief   Configuration Mode enumeration
 */
typedef enum
{
    GPIO_MODE_IN  = 0x00,     //!<  GPIO Input Mode 
    GPIO_MODE_OUT = 0x01,     //!< GPIO Output Mode 
    GPIO_MODE_AF  = 0x02,     //!< GPIO Alternate function Mode 
    GPIO_MODE_AN  = 0x03      //!< GPIO Analog In/Out Mode 
} Gpio_ModeType;

/**
 * @brief   Output type enumeration
 */
typedef enum
{
    GPIO_OUT_TYPE_PP = 0x00,      //!< General purpose output push-pull 
    GPIO_OUT_TYPE_OD = 0x01       //!< General purpose output Open-drain 
} Gpio_OutType;

/**
 * @brief   GPIO Output Maximum frequency selection
 */
typedef enum
{
    GPIO_SPEED_2MHz  = 0x00,      //!< Output speed up to 2 MHz 
    GPIO_SPEED_10MHz = 0x01,      //!< Output speed up to 10 MHz 
    GPIO_SPEED_50MHz = 0x03       //!< Output speed up to 50 MHz 
} Gpio_SpeedType;

/**
 * @brief   Configuration Pull-Up Pull-Down enumeration
 */
typedef enum
{
    GPIO_PUPD_NO = 0x00,          //!< GPIO no pull mode 
    GPIO_PUPD_PU = 0x01,          //!< GPIO pull-up mode 
    GPIO_PUPD_PD = 0x02           //!< GPIO pull-down mode 
} Gpio_PupdType;

/**
 * @brief   Bit SET and Bit RESET enumeration
 */
typedef enum
{
    Bit_RESET,
    Bit_SET
} Gpio_BsretType;

/**
 * @brief    Definition of the GPIO pins
 */
typedef enum
{
    GPIO_PIN_0   = ((uint16_t)BIT0),    //!< GPIO pin 0 selected
    GPIO_PIN_1   = ((uint16_t)BIT1),    //!< GPIO pin 1 selected
    GPIO_PIN_2   = ((uint16_t)BIT2),    //!< GPIO pin 2 selected
    GPIO_PIN_3   = ((uint16_t)BIT3),    //!< GPIO pin 3 selected
    GPIO_PIN_4   = ((uint16_t)BIT4),    //!< GPIO pin 4 selected
    GPIO_PIN_5   = ((uint16_t)BIT5),    //!< GPIO pin 5 selected
    GPIO_PIN_6   = ((uint16_t)BIT6),    //!< GPIO pin 6 selected
    GPIO_PIN_7   = ((uint16_t)BIT7),    //!< GPIO pin 7 selected
    GPIO_PIN_8   = ((uint16_t)BIT8),    //!< GPIO pin 8 selected
    GPIO_PIN_9   = ((uint16_t)BIT9),    //!< GPIO pin 9 selected
    GPIO_PIN_10  = ((uint16_t)BIT10),   //!< GPIO pin 10 selected
    GPIO_PIN_11  = ((uint16_t)BIT11),   //!< GPIO pin 11 selected
    GPIO_PIN_12  = ((uint16_t)BIT12),   //!< GPIO pin 12 selected
    GPIO_PIN_13  = ((uint16_t)BIT13),   //!< GPIO pin 13 selected
    GPIO_PIN_14  = ((uint16_t)BIT14),   //!< GPIO pin 14 selected
    GPIO_PIN_15  = ((uint16_t)BIT15),   //!< GPIO pin 15 selected
    GPIO_PIN_ALL = ((uint16_t)0XFFFF)   //!< GPIO all pins selected
} Gpio_PinType;

/**
 * @brief   GPIO Pin sources
 */
typedef enum
{
    GPIO_PIN_SOURCE_0   = ((uint8_t)0x00),  //!< GPIO pin source 0
    GPIO_PIN_SOURCE_1   = ((uint8_t)0x01),  //!< GPIO pin source 1
    GPIO_PIN_SOURCE_2   = ((uint8_t)0x02),  //!< GPIO pin source 2
    GPIO_PIN_SOURCE_3   = ((uint8_t)0x03),  //!< GPIO pin source 3
    GPIO_PIN_SOURCE_4   = ((uint8_t)0x04),  //!< GPIO pin source 4
    GPIO_PIN_SOURCE_5   = ((uint8_t)0x05),  //!< GPIO pin source 5
    GPIO_PIN_SOURCE_6   = ((uint8_t)0x06),  //!< GPIO pin source 6
    GPIO_PIN_SOURCE_7   = ((uint8_t)0x07),  //!< GPIO pin source 7
    GPIO_PIN_SOURCE_8   = ((uint8_t)0x08),  //!< GPIO pin source 8
    GPIO_PIN_SOURCE_9   = ((uint8_t)0x09),  //!< GPIO pin source 9
    GPIO_PIN_SOURCE_10  = ((uint8_t)0x0A),  //!< GPIO pin source 10
    GPIO_PIN_SOURCE_11  = ((uint8_t)0x0B),  //!< GPIO pin source 11
    GPIO_PIN_SOURCE_12  = ((uint8_t)0x0C),  //!< GPIO pin source 12
    GPIO_PIN_SOURCE_13  = ((uint8_t)0x0D),  //!< GPIO pin source 13
    GPIO_PIN_SOURCE_14  = ((uint8_t)0x0E),  //!< GPIO pin source 14
    GPIO_PIN_SOURCE_15  = ((uint8_t)0x0F)   //!< GPIO pin source 15
} Gpio_PinSourceType;

/**
 * @brief   gpio alternate function define
 */
typedef enum
{
    GPIO_AF_PIN0 = ((uint8_t)0x00),  //!< GPIO alternate function pin 0
    GPIO_AF_PIN1 = ((uint8_t)0x01),  //!< GPIO alternate function pin 1
    GPIO_AF_PIN2 = ((uint8_t)0x02),  //!< GPIO alternate function pin 2
    GPIO_AF_PIN3 = ((uint8_t)0x03),  //!< GPIO alternate function pin 3
    GPIO_AF_PIN4 = ((uint8_t)0x04),  //!< GPIO alternate function pin 4
    GPIO_AF_PIN5 = ((uint8_t)0x05),  //!< GPIO alternate function pin 5
    GPIO_AF_PIN6 = ((uint8_t)0x06),  //!< GPIO alternate function pin 6
    GPIO_AF_PIN7 = ((uint8_t)0x07)   //!< GPIO alternate function pin 7
} Gpio_AfType;

/**@} end of group GPIO_Enumerations */

/** @defgroup GPIO_Structures Structures
  @{
*/

/**
 * @brief   GPIO Config structure definition
 */
typedef struct
{
    uint16_t         pin;      //!< Specifies the GPIO pins to be configured 
    Gpio_ModeType    mode;     //!< Specifies the operating mode for the selected pins 
    Gpio_OutType     outtype;  //!< Specifies the operating output type for the selected pins 
    Gpio_SpeedType   speed;    //!< Specifies the speed for the selected pins 
    Gpio_PupdType    pupd;     //!< Specifies the operating Pull-up/Pull down for the selected pins 
} Gpio_ConfigType;

/**@} end of group GPIO_Structures */

/** @defgroup GPIO_Functions Functions
  @{
*/

/* Reset and common Configuration */
void Gpio_Reset(const GPIO_T* Port);
void Gpio_Config(GPIO_T* Port, const Gpio_ConfigType* GpioConfig);
void Gpio_ConfigStructInit(Gpio_ConfigType* GpioConfig);

/* GPIO Lock functions */
void Gpio_ConfigPinLock(GPIO_T* Port, uint16_t Pin);

/* GPIO Read functions */
uint16_t Gpio_ReadOutputPort(const GPIO_T* Port);
uint16_t Gpio_ReadInputPort(const GPIO_T* Port);
uint8_t Gpio_ReadInputBit(const GPIO_T* Port, uint16_t Pin);
uint8_t Gpio_ReadOutputBit(const GPIO_T* Port, uint16_t Pin);

/* GPIO Write functions */
void Gpio_SetBit(GPIO_T* Port, uint16_t Pin);
void Gpio_ClearBit(GPIO_T* Port, uint16_t Pin);
void Gpio_WriteBitValue(GPIO_T* Port, uint16_t Pin, Gpio_BsretType BitVal);
void Gpio_WriteOutputPort(GPIO_T* Port, uint16_t PortValue);
void Gpio_SetOutputType(GPIO_T* Port, Gpio_PinType Pin, Gpio_OutType OutType);

/* GPIO Other functions */
void Gpio_ConfigPinAF(GPIO_T* Port, Gpio_PinSourceType PinSource, Gpio_AfType AfPin);

void Gpio_Toggle(GPIO_T* Port, Gpio_PinType Pin);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_GPIO_H */

/**@} end of group GPIO_Functions */
/**@} end of group GPIO_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
