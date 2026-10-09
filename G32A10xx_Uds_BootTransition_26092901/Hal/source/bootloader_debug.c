#include "bootloader_debug.h"

#if (UDS_DEBUG_PRINTF != 0)
#include <stdio.h>

/**********************************************************
  * @brief  printf 底层：Keil 把字符送到 USART1
 **********************************************************/
int fputc(int ch, FILE *f)
{
    (void)f;
    Usart_TxData(USART1, (uint16_t)ch);
    while (Usart_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET)
    {
    }
    return ch;
}

/**********************************************************
  * @brief  初始化调试串口 USART1（PB6/PB7，115200）
 **********************************************************/
static void DebugUart_Init(void)
{
    Gpio_ConfigType gpioConfig;
    Usart_ConfigType usartConfigStruct;

    Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_GPIOB);
    Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_USART1);

    Gpio_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_6, GPIO_AF_PIN0);
    Gpio_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_7, GPIO_AF_PIN0);

    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.pin = GPIO_PIN_6;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_PU;
    Gpio_Config(GPIOB, &gpioConfig);

    gpioConfig.pin = GPIO_PIN_7;
    Gpio_Config(GPIOB, &gpioConfig);

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    Usart_Config(USART1, &usartConfigStruct);
    Usart_EnableInterrupt(USART1, USART_INT_RXBNEIE);
    Nvic_EnableIrqRequest(USART1_IRQn, 2);
    Usart_Enable(USART1);
}
#endif

void PrepareBootloaderDebug(void)
{
#if (UDS_DEBUG_PRINTF != 0)
    DebugUart_Init();
#endif
}
