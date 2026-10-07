/* Includes */
#include "g32a10xx_spi.h"
#include "g32a10xx_rcm.h"
#include "g32a10xx_misc.h"
#include "g32a10xx_gpio.h"
#include "board.h"
#include "drv_spi.h"


/**********************************************************
  * @brief	drv SPI 初始化
  * @param
  * @retval
  * @note		
 **********************************************************/
void drv_spi1_init(void)
{
	Gpio_ConfigType	gpioConfig;
	Spi_ConfigType	spiConfig;

	/* Enable related clock*/
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_SPI1);
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

	Rcm_EnableAhbPeriphClock(SPI_GPIO_PER_CLOCK);

	/* NSS：复用为硬件片选输出 */
	gpioConfig.mode = GPIO_MODE_OUT;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.pin = SPI1_NSS_GPIO_PIN;

	// Gpio_ConfigPinAF(SPI1_NSS_GPIO_PORT, SPI1_NSS_PIN_SOURCE, SPI1_NSS_GPIO_AF);
	Gpio_Config(SPI1_NSS_GPIO_PORT, &gpioConfig);
	Gpio_SetBit(SPI1_NSS_GPIO_PORT, (uint16_t)SPI1_NSS_GPIO_PIN);

	/* SCK */
	gpioConfig.mode = GPIO_MODE_AF;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	gpioConfig.pupd = GPIO_PUPD_PD;
	gpioConfig.pin = SPI1_SCK_GPIO_PIN;

	Gpio_ConfigPinAF(SPI1_SCK_GPIO_PORT, SPI1_SCK_PIN_SOURCE, SPI1_SCK_GPIO_AF);
	Gpio_Config(SPI1_SCK_GPIO_PORT, &gpioConfig);

	/* MISO */
	gpioConfig.mode = GPIO_MODE_AF;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.pin = SPI1_MISO_GPIO_PIN;

	Gpio_ConfigPinAF(SPI1_MISO_GPIO_PORT, SPI1_MISO_PIN_SOURCE, SPI1_MISO_GPIO_AF);
	Gpio_Config(SPI1_MISO_GPIO_PORT, &gpioConfig);

	/* MOSI */
	gpioConfig.mode = GPIO_MODE_AF;
	gpioConfig.outtype = GPIO_OUT_TYPE_PP;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	gpioConfig.pupd = GPIO_PUPD_PU;
	gpioConfig.pin = SPI1_MOSI_GPIO_PIN;

	Gpio_ConfigPinAF(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_PIN_SOURCE, SPI1_MOSI_GPIO_AF);
	Gpio_Config(SPI1_MOSI_GPIO_PORT, &gpioConfig);


	spiConfig.mode 								= SPI_MODE_MASTER;				// 主机模式，对应LPSPI
	spiConfig.polarity 						= SPI_CLKPOL_LOW;				// 时钟空闲高电平
	spiConfig.phase 							= SPI_CLKPHA_2EDGE;				// 第2边沿采样
	spiConfig.slaveSelect 				= SPI_SSC_ENABLE;				// 硬件NSS：关闭软件片选，由外设驱动
	spiConfig.baudrateDiv 				= SPI_BAUDRATE_DIV_64;		// 波特率分频64
	spiConfig.length							= SPI_DATA_LENGTH_16B;		// 16位一帧
	spiConfig.firstBit						= SPI_FIRST_BIT_MSB;			// MSB先发，对应
	spiConfig.direction   				= SPI_DIRECTION_2LINES_FULLDUPLEX;	// 4线全双工
	spiConfig.crcPolynomial 			= 7;
	spiConfig.internalSlaveSelect = SPI_ISSEL_ENABLE;
	spiConfig.ssOutput 						= SPI_SSOEN_DISABLE;				// 主机NSS输出使能
	spiConfig.threshold 					= SPI_RXFIFO_HALF;				// 16位帧用1/2阈值			
	spiConfig.bidirection 				= SPI_BMEN_ENABLE;				//
	spiConfig.receiveOnly 				= SPI_RXOMEN_DISABLE;
	spiConfig.dataTransferDirection = SPI_BMOEN_DISABLE;

	/* SPI RESET*/
	Spi_Reset(SPI);

	/* SPI configuration*/
	Spi_Config(SPI, &spiConfig);

	// Spi_EnableNSSPulse(SPI);		//改由软件片选

	Spi_Enable(SPI);
}

/**********************************************************
  * @brief	drv SPI 片选使能
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_spi1_enable_nss(void)
{
	Gpio_ClearBit(SPI1_NSS_GPIO_PORT, (uint16_t)SPI1_NSS_GPIO_PIN);
}

/**********************************************************
  * @brief	drv SPI 片选禁用
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_spi1_disable_nss(void)
{
	Gpio_SetBit(SPI1_NSS_GPIO_PORT, (uint16_t)SPI1_NSS_GPIO_PIN);
}



/**********************************************************
  * @brief	drv SPI 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv_spi1_sleep(void)
{
	drv_spi1_disable_nss();
	
	Spi_Disable(SPI);
	Rcm_DisableApb2PeriphClock(RCM_APB2_PERIPH_SPI1);
}
 
 /**********************************************************
   * @brief	drv SPI 唤醒
   * @param  
   * @retval 
   * @note		
  **********************************************************/
void drv_spi1_wakeup(void)
{
	Rcm_EnableApb2PeriphClock(RCM_APB2_PERIPH_SPI1);
	Spi_Reset(SPI);
 
	Spi_Enable(SPI);
}








/**********************************************************
  * @brief	drv SPI 16位读写（一帧）
  * @param	tx_word 发送的16位字，MSB先出线
  * @retval	1成功 0超时
  * @note		NSS在本帧16个时钟期间为低，结束后拉高
 **********************************************************/
uint8_t drv_spi1_rw_word(uint16_t tx_word, uint16_t *rx_word)
{
	uint16_t retry=0;

	drv_spi1_enable_nss();

	while (Spi_ReadStatusFlag(SPI, SPI_FLAG_TXBE) == (uint8_t)RESET)
	{
		retry++;
		if(retry>2000)	
		{
			drv_spi1_disable_nss();
			return 0;
		}
	}
	Spi_TxData16(SPI, tx_word);

	retry=0;
	while (Spi_ReadStatusFlag(SPI, SPI_FLAG_RXBNE) == (uint8_t)RESET)
	{
		retry++;
		if(retry>2000)	
		{
			drv_spi1_disable_nss();
			return 0;
		}
	}

	*rx_word = Spi_RxData16(SPI);

	retry=0;
	while (Spi_ReadStatusFlag(SPI, SPI_FLAG_BUSY) != (uint8_t)RESET)
	{
		retry++;
		if(retry>2000)	
		{
			drv_spi1_disable_nss();
			return 0;
		}
	}

	drv_spi1_disable_nss();

	return 1;
}


/**********************************************************
  * @brief	drv SPI 按16位帧读写缓冲区
  * @param	len 必须为偶数，每2字节组成一个16位字
  * @retval	1成功 0失败
  * @note		小端组字：buf[0]低字节、buf[1]高字节。MSB先发则线上先出buf[1]
 **********************************************************/
 uint8_t drv_spi1_rw_data(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t len)
 {
	 uint16_t i=0;
	 uint16_t tx_word=0;
	 uint16_t rx_word=0;
 
	 if((len == 0) || ((len & 1U) != 0))
	 {
		 return 0;
	 }
 
	 for(i=0; i<len; i+=2)
	 {
		 tx_word = ((uint16_t)tx_buf[i + 1] << 8) | tx_buf[i];
		 if(drv_spi1_rw_word(tx_word, &rx_word) == 0)
		 {
			 return 0;
		 }
		 rx_buf[i] = (uint8_t)(rx_word & 0xFF);
		 rx_buf[i + 1] = (uint8_t)(rx_word >> 8);
	 }
	 return 1;
 }

