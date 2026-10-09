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

#include "user_config.h"
#include "debug_IO.h"

/*******************************************************************************
                            LED MACRO DEFINITION
 ******************************************************************************/
#ifdef ALLOW_DEBUG_IO

    #ifdef UDS_PROJECT_FOR_BOOTLOADER
        #define APM_MINI_LED_GPIO_PIN          GPIO_PIN_4
        #define APM_MINI_LED_GPIO_PORT         GPIOF
    #endif

    #ifdef UDS_PROJECT_FOR_APP
        #define APM_MINI_LED_GPIO_PIN          GPIO_PIN_5
        #define APM_MINI_LED_GPIO_PORT         GPIOF
    #endif

#endif

/*******************************************************************************
                            GLOBAL FUNCTION
 ******************************************************************************/
#ifdef ALLOW_DEBUG_IO
void Init_DebugIO(void)
{
    Gpio_ConfigType gpioConfig;
    Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_GPIOF);

    gpioConfig.mode = GPIO_MODE_OUT;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.speed = GPIO_SPEED_10MHz;
    gpioConfig.pin = APM_MINI_LED_GPIO_PIN;

    Gpio_Config(GPIOF, &gpioConfig);
    Gpio_SetBit(GPIOF, APM_MINI_LED_GPIO_PIN);

    /* Otherwise the LED3 will be turn on */

    #ifdef UDS_PROJECT_FOR_BOOTLOADER
			gpioConfig.pin = GPIO_PIN_5;
			Gpio_Config(GPIOF, &gpioConfig);
			Gpio_SetBit(GPIOF, GPIO_PIN_5);
    #endif

    #ifdef UDS_PROJECT_FOR_APP
			gpioConfig.pin = GPIO_PIN_4;
			Gpio_Config(GPIOF, &gpioConfig);
			Gpio_SetBit(GPIOF, GPIO_PIN_4);
    #endif

}
void Init_CAN_STB_GPIO(void)
{
    Gpio_ConfigType gpioConfig;
    Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_GPIOA);

    //CAN STB 低电平正常通讯 高电平低功耗模式
    gpioConfig.mode = GPIO_MODE_OUT;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.speed = GPIO_SPEED_10MHz;
    gpioConfig.pin = GPIO_PIN_10;

    Gpio_Config(GPIOA, &gpioConfig);
    Gpio_ClearBit(GPIOA, GPIO_PIN_10);
}
/*!
* @brief         Toggle debug Light.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void MakeDebugIOTog(void)
{
    GPIOF->ODATA_R.ODATA ^= APM_MINI_LED_GPIO_PIN;
}

void MakeDebugLEDProgBlink(void)
{
    /* PF4/PF5 与 APP 板级 LED1/LED2 相同，低电平亮 */
    GPIOF->ODATA_R.ODATA ^= (GPIO_PIN_4 | GPIO_PIN_5);
}

void MakeDebugLEDProgOff(void)
{
    Gpio_SetBit(GPIOF, GPIO_PIN_4);
    Gpio_SetBit(GPIOF, GPIO_PIN_5);
}
void MakeDebugLEDProgOn(void)
{
    Gpio_ClearBit(GPIOF, GPIO_PIN_4);
    Gpio_ClearBit(GPIOF, GPIO_PIN_5);
}

#endif


