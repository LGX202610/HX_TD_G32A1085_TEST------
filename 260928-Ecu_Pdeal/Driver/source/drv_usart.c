/* Includes */
#include "drv_usart.h"
#include "board.h"

#include "stdio.h"


#define DEBUG_USART  USART1



/*!
 * @brief        serial port tramsimt data
 *
 * @param        pointer to date that need to be sent
 *
 * @retval       None
 *
 * @note
 */
void drv_usart1_write(const uint8_t *data)
{
    const uint8_t *p_data = data;

    while ((p_data != (void*)0) && (*p_data != 0U))
    {
        while(Usart_ReadStatusFlag(USART1, USART_FLAG_TXBE) == (uint8_t)RESET)
        {
            /* nothing */
        }
        Usart_TxData(USART1, *p_data);
        p_data++;
    }
}




/**********************************************************
  * @brief	drv USART 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_usart1_init(void)
{
	Gpio_ConfigType  gpioConfig;
	Usart_ConfigType usart1Config;
	
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_USART1);
	Rcm_EnableAhbPeriphClock(USART1_GPIO_PER_CLOCK);

	
	/* Connect PXx to USARTx_Tx */
	Gpio_ConfigPinAF(USART1_TX_GPIO_PORT, USART1_TX_PIN_SOURCE, USART1_TX_GPIO_AF);
	/* Connect PXx to USARRX_Rx */
	Gpio_ConfigPinAF(USART1_RX_GPIO_PORT, USART1_RX_PIN_SOURCE, USART1_RX_GPIO_AF);
	

	gpioConfig.pin = (uint16_t)USART1_TX_GPIO_PIN;
	gpioConfig.mode = GPIO_MODE_AF;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	Gpio_Config(USART1_TX_GPIO_PORT, &gpioConfig);
    
	gpioConfig.pin = (uint16_t)USART1_RX_GPIO_PIN;
	Gpio_Config(USART1_RX_GPIO_PORT, &gpioConfig);
	
	
	/* USART_Config */
	usart1Config.baudRate = 115200,
	usart1Config.mode     = USART_MODE_TX_RX,
	usart1Config.hardwareFlowCtrl = USART_FLOW_CTRL_NONE,
	usart1Config.parity   = USART_PARITY_NONE,
	usart1Config.stopBits =  USART_STOP_BIT_1,
	usart1Config.wordLength = USART_WORD_LEN_8B,
	
	Usart_Config(USART1, &usart1Config);
	
	/* Enable USART_Interrupt_RXBNEIE */
//	Usart_EnableInterrupt(USART1, USART_INT_RXBNEIE);

//	Nvic_EnableIrqRequest(USART1_IRQn, 2);

	/* Enable USART */
	Usart_Enable(USART1);
}



/**********************************************************
  * @brief	drv USART 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_usart1_sleep(void)
{
		Usart_Disable(USART1);
    Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_USART1);
}

/**********************************************************
  * @brief	drv USART 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_usart1_wakeup(void)
{
    Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_USART1);
		Usart_Enable(USART1);
}   

static Drv_Usart1_RxIrqHookFunc_t s_usart1_rx_irq_hook  = NULL;		//预留钩子
/**********************************************************
  * @brief	钩子注册
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_register_usart1_rx_irq_hook(Drv_Usart1_RxIrqHookFunc_t hook_func)
{
	s_usart1_rx_irq_hook = hook_func;
}


/**********************************************************
  * @brief	定时器 中断服务函数
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_usart1_rx_irq_handle(void)
{	
	uint8_t dat = 0;
	
	if(Usart_ReadStatusFlag(USART1, USART_FLAG_RXBNE) == (uint16_t)SET)
	{		
		dat = (uint8_t)Usart_RxData(USART1);
		
		if(s_usart1_rx_irq_hook != NULL)	s_usart1_rx_irq_hook(dat);
	}
}









#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    Usart_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send  */
    while (Usart_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == (uint8_t)RESET)
    {
        /* nothing */
    }

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    Usart_TxData(DEBUG_USART, ch);

    /* wait for the data to be send  */
    while (Usart_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET)
    {
        /* nothing */
    }

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#pragma message("Not supported compiler type")
#endif

