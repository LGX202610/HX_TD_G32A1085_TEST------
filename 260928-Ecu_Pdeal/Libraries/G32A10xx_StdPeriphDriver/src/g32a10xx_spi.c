/*!
 * @file        g32a10xx_spi.c
 *
 * @brief       This file contains all the functions for the SPI peripheral
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

#include "g32a10xx_spi.h"
#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup SPI_Driver
  @{
*/

/** @defgroup SPI_Functions Functions
  @{
*/

/*!
 * @brief       Set the SPI peripheral registers to their default reset values
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_Reset(const SPI_T* Spi)
{
    if (Spi == SPI)
    {
        Rcm_EnableApb2PeriphReset(RCM_APB2_PERIPH_SPI1);
        Rcm_DisableApb2PeriphReset(RCM_APB2_PERIPH_SPI1);
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Config the SPI peripheral according to the specified parameters in the adcConfig
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @param       SpiConfig:  Pointer to a Spi_ConfigType structure that
 *                          contains the configuration information for the SPI peripheral
 *
 * @retval      None
 */
void Spi_Config(SPI_T* Spi, const Spi_ConfigType* SpiConfig)
{
    Spi->CTRL1_R.CTRL1_B.MSMCFG = (uint8_t)SpiConfig->mode;
    Spi->CTRL2_R.CTRL2_B.DSCFG   = (uint8_t)SpiConfig->length;
    Spi->CTRL1_R.CTRL1_B.CPHA  = (uint8_t)SpiConfig->phase;
    Spi->CTRL1_R.CTRL1_B.CPOL  = (uint8_t)SpiConfig->polarity;
    Spi->CTRL1_R.CTRL1_B.SSEN = (uint8_t)SpiConfig->slaveSelect;
    Spi->CTRL1_R.CTRL1_B.LSBSEL = (uint8_t)SpiConfig->firstBit;

    Spi->CTRL1_R.CTRL1 &= (uint16_t)(~0xC400u);
    Spi->CTRL1_R.CTRL1 |= (uint32_t)SpiConfig->direction;

    Spi->CTRL1_R.CTRL1_B.BRSEL = (uint8_t)SpiConfig->baudrateDiv;

    Spi->CRCPOLY_R.CRCPOLY |= (uint8_t)SpiConfig->crcPolynomial;
    Spi->CTRL1_R.CTRL1_B.BMEN = (uint8_t)SpiConfig->bidirection;
    Spi->CTRL1_R.CTRL1_B.RXOMEN = (uint8_t)SpiConfig->receiveOnly;
    Spi->CTRL1_R.CTRL1_B.BMOEN = (uint8_t)SpiConfig->dataTransferDirection;
    Spi->CTRL1_R.CTRL1_B.ISSEL = (uint8_t)SpiConfig->internalSlaveSelect;
    Spi->CTRL2_R.CTRL2_B.SSOEN = (uint8_t)SpiConfig->ssOutput;
    Spi->CTRL2_R.CTRL2_B.FRTCFG = (uint8_t)SpiConfig->threshold;
}

/*!
 * @brief       Fills each SpiConfig member with its default value
 *
 * @param       SpiConfig:  Pointer to a Spi_ConfigType structure which will be initialized
 *
 * @retval      None
 */
void Spi_ConfigStructInit(Spi_ConfigType* SpiConfig)
{
    SpiConfig->mode      = SPI_MODE_SLAVE;
    SpiConfig->length    = SPI_DATA_LENGTH_8B;
    SpiConfig->phase     = SPI_CLKPHA_1EDGE;
    SpiConfig->polarity  = SPI_CLKPOL_HIGH;
    SpiConfig->slaveSelect = SPI_SSC_DISABLE;
    SpiConfig->firstBit    = SPI_FIRST_BIT_MSB;
    SpiConfig->direction   = SPI_DIRECTION_2LINES_FULLDUPLEX;
    SpiConfig->baudrateDiv = SPI_BAUDRATE_DIV_2;
    SpiConfig->crcPolynomial = 7;
    SpiConfig->bidirection = SPI_BMEN_ENABLE;
    SpiConfig->receiveOnly = SPI_RXOMEN_DISABLE;
    SpiConfig->dataTransferDirection = SPI_BMOEN_DISABLE;
    SpiConfig->internalSlaveSelect = SPI_ISSEL_DISABLE;
    SpiConfig->ssOutput = SPI_SSOEN_DISABLE;
    SpiConfig->threshold = SPI_RXFIFO_HALF;
}

/*!
 * @brief       Enable the SPI peripheral
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_Enable(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.SPIEN = BIT_SET;
}

/*!
 * @brief       Disable the SPI peripheral
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_Disable(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.SPIEN = BIT_RESET;
}

/*!
 * @brief       Enable the frame format mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableFrameFormatMode(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.FRFCFG = BIT_SET;
}

/*!
 * @brief       Disable the frame format mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableFrameFormatMode(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.FRFCFG = BIT_RESET;
}

/*!
 * @brief       Configures the SPI data length
 *
 * @param       Length:  specifies the SPI length
 *                          The parameter can be one of following values:
 *                          @arg SPI_DATA_LENGTH_4B:  Set data length to 4 bits
 *                          @arg SPI_DATA_LENGTH_5B:  Set data length to 5 bits
 *                          @arg SPI_DATA_LENGTH_6B:  Set data length to 6 bits
 *                          @arg SPI_DATA_LENGTH_7B:  Set data length to 7 bits
 *                          @arg SPI_DATA_LENGTH_8B:  Set data length to 8 bits
 *                          @arg SPI_DATA_LENGTH_9B:  Set data length to 9 bits
 *                          @arg SPI_DATA_LENGTH_10B:  Set data length to 10 bits
 *                          @arg SPI_DATA_LENGTH_11B:  Set data length to 11 bits
 *                          @arg SPI_DATA_LENGTH_12B:  Set data length to 12 bits
 *                          @arg SPI_DATA_LENGTH_13B:  Set data length to 13 bits
 *                          @arg SPI_DATA_LENGTH_14B:  Set data length to 14 bits
 *                          @arg SPI_DATA_LENGTH_15B:  Set data length to 15 bits
 *                          @arg SPI_DATA_LENGTH_16B:  Set data length to 16 bits
 *
 * @retval      None
 */
void Spi_ConfigDatalength(SPI_T* Spi, uint8_t Length)
{
    Spi->CTRL2_R.CTRL2_B.DSCFG = (uint8_t)Length;
}

/*!
 * @brief       Configures the FIFO reception threshold
 *
 * @param       threshold: selects the SPI FIFO reception threshold
 *                         The parameter can be one of following values:
 *                         @arg SPI_RXFIFO_HALF:    FIFO level is greater than or equal to 1/2 (16-bit)
 *                         @arg SPI_RXFIFO_QUARTER: FIFO level is greater than or equal to 1/4 (8-bit)
 *
 * @retval      None
 */
void Spi_ConfigFifoThreshold(SPI_T* Spi, Spi_RxFifoType  Threshold)
{
    Spi->CTRL2_R.CTRL2_B.FRTCFG  =  (uint8_t)Threshold;
}

/*!
 * @brief       Enable the data transfer direction
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableOutputDirection(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.BMOEN  =  BIT_SET;
}

/*!
 * @brief       Disable the data transfer direction
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableOutputDirection(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.BMOEN  =  BIT_RESET;
}

/*!
 * @brief       Enable internal slave select
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableInternalSlave(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.ISSEL = BIT_SET;
}

/*!
 * @brief       Disable internal slave select
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableInternalSlave(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.ISSEL = BIT_RESET;
}

/*!
 * @brief       Enable the SS output mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableSsoutput(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.SSOEN = BIT_SET;
}

/*!
 * @brief       Disable the SS output mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableSsoutput(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.SSOEN = BIT_RESET;
}

/*!
 * @brief       Enable the NSS pulse management mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableNSSPulse(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.NSSPEN = BIT_SET;
}

/*!
 * @brief       Disable the NSS pulse management mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableNssPulse(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.NSSPEN = BIT_RESET;
}

/*!
 * @brief       Transmits a Data
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @param       Data:   Byte to be transmitted
 *
 * @retval      None
 */
void Spi_TxData16(SPI_T* Spi, uint16_t Data)
{
    Spi->DATA_R.DATA = (uint16_t)Data;
}

/*!
 * @brief       Transmits a  uint8_t Data
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @param       Data:   Byte to be transmitted
 *
 * @retval      None
 */
void Spi_TxData8(SPI_T* Spi, uint8_t Data)
{
    *((volatile uint8_t *) & (Spi->DATA_R.DATA)) = Data;
}

/*!
 * @brief       Returns the most recent received data by the SPI peripheral
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @param       None
 *
 * @retval      The value of the received data
 */
uint16_t Spi_RxData16(const SPI_T* Spi)
{
    return ((uint16_t)Spi->DATA_R.DATA);
}

/*!
 * @brief       Returns the most recent received data by the SPI peripheral
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @param       None
 *
 * @retval      The value of the received data
 */
uint8_t Spi_RxData8(const SPI_T* Spi)
{
    return  *((const volatile uint8_t *) & (Spi->DATA_R.DATA));
}

/*!
 * @brief       Selects the data transfer direction
 * @param       Spi:    Select the the SPI peripheral.
 * @param       crcLength: selects the SPI transfer direction
 *                         The parameter can be one of following values:
 *                         @arg SPI_CRC_LENGTH_8B:  8-bit CRC length
 *                         @arg SPI_CRC_LENGTH_16B: 16-bit CRC length
 *
 * @retval      None
 */
void Spi_CrcLength(SPI_T* Spi, Spi_CrcLengthType  CrcLength)
{
    Spi->CTRL1_R.CTRL1_B.CRCLSEL  =  (uint8_t)CrcLength;
}

/*!
 * @brief       Enable the CRC value calculation
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableCrc(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.CRCEN = BIT_SET;
}

/*!
 * @brief       Disable the CRC value calculation
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableCrc(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.CRCEN = BIT_RESET;
}

/*!
 * @brief       Transmit CRC value
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_TxCrc(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.CRCNXT = BIT_SET;
}

/*!
 * @brief       Returns the receive CRC register value
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 *
 * @note        None
 */
uint16_t Spi_ReadRxCrc(const SPI_T* Spi)
{
    return (uint16_t)Spi->RXCRC_R.RXCRC;
}

/*!
 * @brief       Returns the transmit CRC register value
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 *
 * @note        None
 */
uint16_t Spi_ReadTxCrc(const SPI_T* Spi)
{
    return (uint16_t)Spi->TXCRC_R.TXCRC;
}

/*!
 * @brief       Returns the CRC Polynomial register value
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
uint16_t Spi_ReadCrcPolynomial(const SPI_T* Spi)
{
    return (uint16_t)Spi->CRCPOLY_R.CRCPOLY;
}

/*!
 * @brief       Enable the DMA Rx buffer
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableDmaRxBuffer(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.RXDEN = BIT_SET;
}

/*!
 * @brief       Disable the DMA Rx buffer
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableDmaRxBuffer(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.RXDEN = BIT_RESET;
}

/*!
 * @brief       Enable the DMA Tx buffer
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableDmaTxBuffer(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.TXDEN = BIT_SET;
}

/*!
 * @brief       Disable the DMA Tx buffer
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableDmaTxBuffer(SPI_T* Spi)
{
    Spi->CTRL2_R.CTRL2_B.TXDEN = BIT_RESET;
}

/*!
 * @brief       Selects the last DMA transfer is type(Even/Odd)
 *
 * @param       LastDma:   specifies the SPI last DMA transfers
 *                         The parameter can be one of following values:
 *                         @arg SPI_LAST_DMA_TXRXEVEN:    transmission Even reception Even
 *                         @arg SPI_LAST_DMA_TXEVENRXODD: transmission Even reception Odd
 *                         @arg SPI_LAST_DMA_TXODDRXEVEN: transmission Odd reception Even
 *                         @arg SPI_LAST_DMA_TXRXODD:     transmission Odd reception Odd
 *
 * @retval      None
 */
void Spi_LastDmaTransfer(SPI_T* Spi, Spi_LastDmaType  LastDma)
{
    Spi->CTRL2_R.CTRL2 &= 0x9FFFu;
    Spi->CTRL2_R.CTRL2 |= (uint16_t)LastDma;
}

/*!
 * @brief       Returns the SPI Transmission FIFO filled level
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      Transmission FIFO filled level:
 *              SPI_TXFIFO_LEVEL_EMPTY:   Transmission FIFO filled level is empty
 *              SPI_TXFIFO_LEVEL_QUARTER: Transmission FIFO filled level is more than quarter
 *              SPI_TXFIFO_LEVEL_HALF:    Transmission FIFO filled level is more than half
 *              SPI_TXFIFO_LEVEL_FULL:    Transmission FIFO filled level is full
 */
uint8_t Spi_ReadTransmissionFifoLeve(const SPI_T* Spi)
{
    return (uint8_t)((Spi->STS_R.STS_B.FTLSEL & 0x03u));
}

/*!
 * @brief       Returns the SPI Reception FIFO filled level
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      Reception FIFO filled level:
 *              SPI_RXFIFO_LEVEL_EMPTY:   Reception FIFO filled level is empty
 *              SPI_RXFIFO_LEVEL_QUARTER: Reception FIFO filled level is more than quarter
 *              SPI_RXFIFO_LEVEL_HALF:    Reception FIFO filled level is more than half
 *              SPI_RXFIFO_LEVEL_FULL:    Reception FIFO filled level is full
 */
uint8_t Spi_ReadReceptionFifoLeve(const SPI_T* Spi)
{
    return (uint8_t)((Spi->STS_R.STS_B.FRLSEL & 0x03u));
}

/*!
 * @brief       Enable the SPI interrupts
 *
 * @param       interrupt:  Specifies the SPI interrupts sources
 *                          The parameter can be combination of following values:
 *                          @arg SPI_INT_ERRIE:    Error interrupt
 *                          @arg SPI_INT_RXBNEIE:  Receive buffer not empty interrupt
 *                          @arg SPI_INT_TXBEIE:   Transmit buffer empty interrupt
 *
 * @retval      None
 */
void Spi_EnableInterrupt(SPI_T* Spi, uint8_t Interrupt)
{
    Spi->CTRL2_R.CTRL2 |= (uint8_t)Interrupt;
}

/*!
 * @brief       Disable the SPI interrupts
 *
 * @param       interrupt:  Specifies the SPI interrupts sources
 *                          The parameter can be combination of following values:
 *                          @arg SPI_INT_ERRIE:    Error interrupt
 *                          @arg SPI_INT_RXBNEIE:  Receive buffer not empty interrupt
 *                          @arg SPI_INT_TXBEIE:   Transmit buffer empty interrupt
 *
 * @retval      None
 */
void Spi_DisableInterrupt(SPI_T* Spi, uint8_t Interrupt)
{
    Spi->CTRL2_R.CTRL2 &= ~(uint32_t)Interrupt;
}

/*!
 * @brief       Checks whether the specified SPI flag is set or not
 *
 * @param       Flag:   Specifies the flag to check
 *                      This parameter can be one of the following values:
 *                      @arg SPI_FLAG_RXBNE:    Receive buffer not empty flag
 *                      @arg SPI_FLAG_TXBE:     Transmit buffer empty flag
 *                      @arg I2S_FLAG_CHDIR:    Channel direction flag
 *                      @arg I2S_FLAG_UDR:      Underrun flag
 *                      @arg SPI_FLAG_CRCE:     CRC error flag
 *                      @arg SPI_FLAG_MME:      Master mode error flag
 *                      @arg SPI_FLAG_OVR:      Receive Overrun flag
 *                      @arg SPI_FLAG_BUSY:     Busy flag
 *                      @arg SPI_FLAG_FFE:      Frame format error flag
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t Spi_ReadStatusFlag(const SPI_T* Spi, Spi_FlagType Flag)
{
    uint16_t status = RESET;

    status = (uint16_t)(Spi->STS_R.STS & (uint32_t)Flag);

    if (status == (uint16_t)Flag)
    {
        status = SET;
    }
    else
    {
        /* do nothing */
    }

    return (uint8_t)status;
}

/*!
 * @brief       Clear the specified SPI flag
 *
 * @param       Flag:   Specifies the flag to clear
 *                      This parameter can be any combination of the following values:
 *                      @arg SPI_FLAG_CRCE:     CRC error flag

 * @retval      None
 */
void Spi_ClearStatusFlag(SPI_T* Spi, uint8_t Flag)
{
    Spi->STS_R.STS &= (uint32_t)~(uint32_t)Flag;
}

/*!
 * @brief       Checks whether the specified interrupt has occurred or not
 *
 * @param       flag:   Specifies the SPI interrupt pending bit to check
 *                      This parameter can be one of the following values:
 *                      @arg SPI_INT_FLAG_RXBNE:    Receive buffer not empty flag
 *                      @arg SPI_INT_FLAG_TXBE:     Transmit buffer empty flag
 *                      @arg SPI_INT_FLAG_UDR:      Underrun flag interrupt flag
 *                      @arg SPI_INT_FLAG_MME:      Master mode error flag
 *                      @arg SPI_INT_FLAG_OVR:      Receive Overrun flag
 *                      @arg SPI_INT_FLAG_FFE:      Frame format error interrupt flag
 *
 * @retval      None
 */
uint8_t Spi_ReadIntFlag(const SPI_T* Spi, Spi_IntFlagType Flag)
{
    uint32_t intEnable = 0;
    uint32_t intStatus = RESET;

    intEnable = (uint32_t)(Spi->CTRL2_R.CTRL2 & (uint32_t)((uint32_t)Flag >> 16u));

    intStatus = (uint32_t)(Spi->STS_R.STS & (uint32_t)((uint32_t)Flag & 0x1ffu));

    if ((0u != intEnable) && (0u != intStatus))
    {
        intStatus = SET;
    }
    else
    {
        /* do nothing */
    }

    return (uint8_t)intStatus;
}

/*!
 * @brief       Enable Bidirectional Mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableBidirectionalMode(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.BMEN  =  BIT_RESET;
}

/*!
 * @brief       Disable Bidirectional Mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableBidirectionalMode(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.BMEN  =  BIT_SET;
}

/*!
 * @brief       Enable Receive Only Mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_EnableReceiveOnlyMode(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.RXOMEN  =  BIT_SET;
}

/*!
 * @brief       Disable Receive Only Mode
 *
 * @param       Spi:    Select the the SPI peripheral.
 *
 * @retval      None
 */
void Spi_DisableReceiveOnlyMode(SPI_T* Spi)
{
    Spi->CTRL1_R.CTRL1_B.RXOMEN  =  BIT_RESET;
}

/**@} end of group SPI_Functions*/
/**@} end of group SPI_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/
