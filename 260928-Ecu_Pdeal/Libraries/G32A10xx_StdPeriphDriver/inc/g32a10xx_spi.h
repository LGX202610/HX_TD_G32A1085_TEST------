/*!
 * @file        g32a10xx_spi.h
 *
 * @brief       This file contains all the functions prototypes for the SPI firmware library
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

#ifndef G32A10xx_SPI_H
#define G32A10xx_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup SPI_Driver
  @{
*/

/** @defgroup SPI_Enumerations Enumerations
  @{
*/

/**
 * @brief   SPI data direction mode
 */
typedef enum
{
    SPI_DIRECTION_2LINES_FULLDUPLEX     = ((uint16_t)0x0000),  //!< Full duplex mode,in 2-line unidirectional data mode
    SPI_DIRECTION_2LINES_RXONLY         = ((uint16_t)0x0400),  //!< Receiver only, in 2-line unidirectional data mode
    SPI_DIRECTION_1LINE_RX              = ((uint16_t)0x8000),  //!< Receiver mode, in 1 line bidirectional data mode
    SPI_DIRECTION_1LINE_TX              = ((uint16_t)0xC000)   //!< Transmit mode, in 1 line bidirectional data mode
} Spi_DirectionType;

/**
 * @brief   SPI mode
 */
typedef enum
{
    SPI_MODE_SLAVE      = ((uint8_t)0),     //!< Slave mode
    SPI_MODE_MASTER     = ((uint8_t)1)      //!< Master mode
} Spi_ModeType;

/**
 * @brief   SPI data length
 */
typedef enum
{
    SPI_DATA_LENGTH_4B    = ((uint8_t)0x03),   //!< Set data length to 4 bits
    SPI_DATA_LENGTH_5B    = ((uint8_t)0x04),   //!< Set data length to 5 bits
    SPI_DATA_LENGTH_6B    = ((uint8_t)0x05),   //!< Set data length to 6 bits
    SPI_DATA_LENGTH_7B    = ((uint8_t)0x06),   //!< Set data length to 7 bits
    SPI_DATA_LENGTH_8B    = ((uint8_t)0x07),   //!< Set data length to 8 bits
    SPI_DATA_LENGTH_9B    = ((uint8_t)0x08),   //!< Set data length to 9 bits
    SPI_DATA_LENGTH_10B   = ((uint8_t)0x09),   //!< Set data length to 10 bits
    SPI_DATA_LENGTH_11B   = ((uint8_t)0x0A),   //!< Set data length to 11 bits
    SPI_DATA_LENGTH_12B   = ((uint8_t)0x0B),   //!< Set data length to 12 bits
    SPI_DATA_LENGTH_13B   = ((uint8_t)0x0C),   //!< Set data length to 13 bits
    SPI_DATA_LENGTH_14B   = ((uint8_t)0x0D),   //!< Set data length to 14 bits
    SPI_DATA_LENGTH_15B   = ((uint8_t)0x0E),   //!< Set data length to 15 bits
    SPI_DATA_LENGTH_16B   = ((uint8_t)0x0F)    //!< Set data length to 16 bits
} Spi_DataLengthType;

/**
 * @brief   SPI CRC length
 */
typedef enum
{
    SPI_CRC_LENGTH_8B       = ((uint8_t)0), //!< 8-bit CRC length
    SPI_CRC_LENGTH_16B      = ((uint8_t)1)  //!< 16-bit CRC length
} Spi_CrcLengthType;

/**
 * @brief   SPI Clock Polarity
 */
typedef enum
{
    SPI_CLKPOL_LOW          = ((uint8_t)0), //!< Clock Polarity low
    SPI_CLKPOL_HIGH         = ((uint8_t)1)  //!< Clock Polarity high
} Spi_ClkPolType;

/**
 * @brief   SPI Clock Phase
 */
typedef enum
{
    SPI_CLKPHA_1EDGE        = ((uint8_t)0), //!< 1 edge clock phase
    SPI_CLKPHA_2EDGE        = ((uint8_t)1)  //!< 2 edge clock phase
} Spi_ClkPhaType;

/**
 * @brief   Software slave control
 */
typedef enum
{
    SPI_SSC_DISABLE         = ((uint8_t)0), //!< Disable software select slave
    SPI_SSC_ENABLE          = ((uint8_t)1)  //!< Enable software select slave
} Spi_SscType;

/**
 * @brief   SPI BaudRate divider
 */
typedef enum
{
    SPI_BAUDRATE_DIV_2      = ((uint8_t)0), //!< Baud rate divider is 2
    SPI_BAUDRATE_DIV_4      = ((uint8_t)1), //!< Baud rate divider is 4
    SPI_BAUDRATE_DIV_8      = ((uint8_t)2), //!< Baud rate divider is 8
    SPI_BAUDRATE_DIV_16     = ((uint8_t)3), //!< Baud rate divider is 16
    SPI_BAUDRATE_DIV_32     = ((uint8_t)4), //!< Baud rate divider is 32
    SPI_BAUDRATE_DIV_64     = ((uint8_t)5), //!< Baud rate divider is 64
    SPI_BAUDRATE_DIV_128    = ((uint8_t)6), //!< Baud rate divider is 128
    SPI_BAUDRATE_DIV_256    = ((uint8_t)7)  //!< Baud rate divider is 256
} Spi_BaudrateDivType;

/**
 * @brief   MSB or LSB is transmitted/received first
 */
typedef enum
{
    SPI_FIRST_BIT_MSB       = ((uint8_t)0), //!< First bit is MSB
    SPI_FIRST_BIT_LSB       = ((uint8_t)1)  //!< First bit is LSB
} Spi_FirstBitType;

/**
 * @brief   SPI FIFO reception threshold
 */
typedef enum
{
    SPI_RXFIFO_HALF         = ((uint8_t)0), //!< FIFO level is greater than or equal to 1/2 (16-bit)
    SPI_RXFIFO_QUARTER      = ((uint8_t)1)  //!< FIFO level is greater than or equal to 1/4 (8-bit)
} Spi_RxFifoType;

/**
 * @brief   SPI last DMA transfers and reception
 */
typedef enum
{
    SPI_LAST_DMA_TXRXEVEN    = ((uint16_t)0x0000),  //!< transmission Even reception Even
    SPI_LAST_DMA_TXEVENRXODD = ((uint16_t)0x2000),  //!< transmission Even reception Odd
    SPI_LAST_DMA_TXODDRXEVEN = ((uint16_t)0x4000),  //!< transmission Odd reception Even
    SPI_LAST_DMA_TXRXODD     = ((uint16_t)0x6000)   //!< transmission Odd reception Odd
} Spi_LastDmaType;

/**
 * @brief   SPI transmission fifo level
 */
typedef enum
{
    SPI_TXFIFO_LEVEL_EMPTY   = ((uint8_t)0x00),     //!< Transmission FIFO filled level is empty
    SPI_TXFIFO_LEVEL_QUARTER = ((uint8_t)0x01),     //!< Transmission FIFO filled level is more than quarter
    SPI_TXFIFO_LEVEL_HALF    = ((uint8_t)0x02),     //!< Transmission FIFO filled level is more than half
    SPI_TXFIFO_LEVEL_FULL    = ((uint8_t)0x03)      //!< Transmission FIFO filled level is full
} Spi_TxFifoLevelType;

/**
 * @brief   SPI reception fifo level
 */
typedef enum
{
    SPI_RXFIFO_LEVEL_EMPTY    = ((uint8_t)0x00),    //!< Reception FIFO filled level is empty
    SPI_RXFIFO_LEVEL_QUARTER  = ((uint8_t)0x01),    //!< Reception FIFO filled level is more than quarter
    SPI_RXFIFO_LEVEL_HALF     = ((uint8_t)0x02),    //!< Reception FIFO filled level is more than half
    SPI_RXFIFO_LEVEL_FULL     = ((uint8_t)0x03)     //!< Reception FIFO filled level is full
} Spi_RxFifoLevelType;

/**
 * @brief   SPI flags definition
 */
typedef enum
{
    SPI_FLAG_RXBNE      = ((uint16_t)0x0001),       //!< Receive buffer not empty flag
    SPI_FLAG_TXBE       = ((uint16_t)0x0002),       //!< Transmit buffer empty flag
    I2S_FLAG_CHDIR      = ((uint16_t)0x0004),       //!< Channel direction flag
    I2S_FLAG_UDR        = ((uint16_t)0x0008),       //!< Underrun flag
    SPI_FLAG_CRCE       = ((uint16_t)0x0010),       //!< CRC error flag
    SPI_FLAG_MME        = ((uint16_t)0x0020),       //!< Master mode error flag
    SPI_FLAG_OVR        = ((uint16_t)0x0040),       //!< Receive Overrun flag
    SPI_FLAG_BUSY       = ((uint16_t)0x0080),       //!< Busy flag
    SPI_FLAG_FFE        = ((uint16_t)0x0100)        //!< Frame format error flag
} Spi_FlagType;

/**
 * @brief   SPI interrupt source
 */
typedef enum
{
    SPI_INT_ERRIE       = ((uint8_t)0x20),          //!< Error interrupt
    SPI_INT_RXBNEIE     = ((uint8_t)0x40),          //!< Receive buffer not empty interrupt
    SPI_INT_TXBEIE      = ((uint8_t)0x80)           //!< Transmit buffer empty interrupt
} Spi_IntType;

/**
 * @brief   SPI interrupt flag
 */
typedef enum
{
    SPI_INT_FLAG_RXBNE      = ((uint32_t)0x400001), //!< Receive buffer not empty interrupt flag
    SPI_INT_FLAG_TXBE       = ((uint32_t)0x800002), //!< Transmit buffer empty interrupt flag
    SPI_INT_FLAG_UDR        = ((uint32_t)0x200008), //!< Underrun flag interrupt flag
    SPI_INT_FLAG_MME        = ((uint32_t)0x200020), //!< Master mode error interrupt flag
    SPI_INT_FLAG_OVR        = ((uint32_t)0x200040), //!< Receive Overrun interrupt flag
    SPI_INT_FLAG_FFE        = ((uint32_t)0x200100)  //!< Frame format error interrupt flag
} Spi_IntFlagType;

/**
 * @brief   SPI bidirectional mode
 */
typedef enum
{
    SPI_BMEN_ENABLE         = ((uint8_t)0), //!< Enable bidirectional mode
    SPI_BMEN_DISABLE        = ((uint8_t)1)  //!< Disable bidirectional mode
} Spi_BidirectionType;

/**
 * @brief   SPI receive only mode
 */
typedef enum
{
    SPI_RXOMEN_DISABLE        = ((uint8_t)0), //!< Disable receive only mode
    SPI_RXOMEN_ENABLE         = ((uint8_t)1) //!< Enable receive only mode
} Spi_ReceiveOnlyType;

/**
 * @brief   SPI data transfer direction
 */
typedef enum
{
    SPI_BMOEN_DISABLE        = ((uint8_t)0), //!< Disable data transfer direction
    SPI_BMOEN_ENABLE         = ((uint8_t)1) //!< Enable data transfer direction
} Spi_DataTransferDirectionType;

/**
 * @brief   SPI internal slave select
 */
typedef enum
{
    SPI_ISSEL_DISABLE        = ((uint8_t)0), //!< Disable internal slave select
    SPI_ISSEL_ENABLE         = ((uint8_t)1) //!< Enable internal slave select
} Spi_InternalSlaveSelectType;

/**
 * @brief   SPI SS output mode
 */
typedef enum
{
    SPI_SSOEN_DISABLE        = ((uint8_t)0), //!< Disable SS output mode
    SPI_SSOEN_ENABLE         = ((uint8_t)1) //!< Enable SS output mode
} Spi_SsOutputType;

/**@} end of group SPI_Enumerations*/

/** @defgroup SPI_Structures Structures
  @{
*/

/**
 * @brief   SPI Config struct definition
 */
typedef struct
{
    Spi_ModeType           mode;          //!< Specifies the SPI mode
    Spi_DataLengthType     length;        //!< Specifies the SPI data length
    Spi_ClkPhaType         phase;         //!< Specifies the Clock phase
    Spi_ClkPolType         polarity;      //!< Specifies the Clock polarity
    Spi_SscType            slaveSelect;   //!< Specifies the slave select mode
    Spi_FirstBitType       firstBit;      //!< Specifies the Frame format
    Spi_DirectionType      direction;     //!< Specifies the data direction mode
    Spi_BaudrateDivType    baudrateDiv;   //!< Specifies the baud rate divider
    uint8_t                crcPolynomial; //!< Specifies the CRC polynomial
    Spi_BidirectionType    bidirection;   //!< Bidirectional mode
    Spi_ReceiveOnlyType    receiveOnly;   //!< Receive only mode
    Spi_DataTransferDirectionType dataTransferDirection; //!< Data transfer direction
    Spi_InternalSlaveSelectType internalSlaveSelect; //!< Internal slave select
    Spi_SsOutputType       ssOutput;      //!< SS output mode
    Spi_RxFifoType         threshold;     //!< FIFO reception threshold
} Spi_ConfigType;

/**@} end of group SPI_Structures*/

/** @defgroup SPI_Functions Functions
  @{
*/

/** SPI reset and configuration */
void Spi_Reset(const SPI_T* Spi);
void Spi_Config(SPI_T* Spi, const Spi_ConfigType* SpiConfig);
void Spi_ConfigStructInit(Spi_ConfigType* SpiConfig);
void Spi_Enable(SPI_T* Spi);
void Spi_Disable(SPI_T* Spi);
void Spi_EnableFrameFormatMode(SPI_T* Spi);
void Spi_DisableFrameFormatMode(SPI_T* Spi);
void Spi_ConfigDatalength(SPI_T* Spi, uint8_t Length);
void Spi_EnableOutputDirection(SPI_T* Spi);
void Spi_DisableOutputDirection(SPI_T* Spi);
void Spi_EnableInternalSlave(SPI_T* Spi);
void Spi_DisableInternalSlave(SPI_T* Spi);
void Spi_EnableSsoutput(SPI_T* Spi);
void Spi_DisableSsoutput(SPI_T* Spi);
void Spi_EnableNSSPulse(SPI_T* Spi);
void Spi_DisableNssPulse(SPI_T* Spi);
void Spi_EnableBidirectionalMode(SPI_T* Spi);
void Spi_DisableBidirectionalMode(SPI_T* Spi);
void Spi_EnableReceiveOnlyMode(SPI_T* Spi);
void Spi_DisableReceiveOnlyMode(SPI_T* Spi);

/**  CRC */
void Spi_CrcLength(SPI_T* Spi, Spi_CrcLengthType  CrcLength);
void Spi_EnableCrc(SPI_T* Spi);
void Spi_DisableCrc(SPI_T* Spi);
void Spi_TxCrc(SPI_T* Spi);
uint16_t Spi_ReadRxCrc(const SPI_T* Spi);
uint16_t Spi_ReadTxCrc(const SPI_T* Spi);
uint16_t Spi_ReadCrcPolynomial(const SPI_T* Spi);

/**  DMA */
void Spi_EnableDmaRxBuffer(SPI_T* Spi);
void Spi_DisableDmaRxBuffer(SPI_T* Spi);
void Spi_EnableDmaTxBuffer(SPI_T* Spi);
void Spi_DisableDmaTxBuffer(SPI_T* Spi);
void Spi_LastDmaTransfer(SPI_T* Spi, Spi_LastDmaType  LastDma);

/** FIFO */
void Spi_ConfigFifoThreshold(SPI_T* Spi, Spi_RxFifoType  Threshold);
uint8_t Spi_ReadTransmissionFifoLeve(const SPI_T* Spi);
uint8_t Spi_ReadReceptionFifoLeve(const SPI_T* Spi);

/** Interrupt */
void Spi_EnableInterrupt(SPI_T* Spi, uint8_t Interrupt);
void Spi_DisableInterrupt(SPI_T* Spi, uint8_t Interrupt);

/** Transmit and receive */
void Spi_TxData8(SPI_T* Spi, uint8_t Data);
void Spi_TxData16(SPI_T* Spi, uint16_t Data);
uint8_t Spi_RxData8(const SPI_T* Spi);
uint16_t Spi_RxData16(const SPI_T* Spi);

/** Flag */
uint8_t Spi_ReadStatusFlag(const SPI_T* Spi, Spi_FlagType Flag);
void Spi_ClearStatusFlag(SPI_T* Spi, uint8_t Flag);
uint8_t Spi_ReadIntFlag(const SPI_T* Spi, Spi_IntFlagType Flag);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_SPI_H */

/**@} end of group SPI_Functions */
/**@} end of group SPI_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
