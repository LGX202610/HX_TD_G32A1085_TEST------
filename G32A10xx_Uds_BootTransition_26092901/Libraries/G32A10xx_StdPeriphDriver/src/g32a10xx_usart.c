/*!
 * @file        g32a10xx_usart.c
 *
 * @brief       This file provides all the USART firmware functions
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

#include "g32a10xx_usart.h"
#include "g32a10xx_rcm.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup USART_Driver
  @{
*/

/** @defgroup USART_Functions Functions
  @{
*/

/*!
 * @brief       Reset UsartPtr peripheral registers to their default reset values
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */

void Usart_Reset(const USART_T* UsartPtr)
{
    if (USART1 == UsartPtr)
    {
        Rcm_EnableApb2PeriphReset(RCM_APB2_PERIPH_USART1);
        Rcm_DisableApb2PeriphReset(RCM_APB2_PERIPH_USART1);
    }
    else if (USART2 == UsartPtr)
    {
        Rcm_EnableApb1PeriphReset(RCM_APB1_PERIPH_USART2);
        Rcm_DisableApb1PeriphReset(RCM_APB1_PERIPH_USART2);
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Config the USART peripheral according to the specified parameters in the USART_InitStruct
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       ConfigStructPtr    pointer to a Usart_ConfigType structure
 *
 * @retval      None
 */
void Usart_Config(USART_T* UsartPtr, const Usart_ConfigType* ConfigStructPtr)
{
    uint32_t temp = 0, fCLK = 0, intDiv = 0, fractionalDiv = 0;

    /** Disable USART */
    UsartPtr->CTRL1_R.CTRL1_B.UEN = 0x00;

    /** WLS, PCEN, TXEN, RXEN */
    temp = UsartPtr->CTRL1_R.CTRL1;
    temp &= 0xE9F3u;
    temp |= (uint32_t)ConfigStructPtr->mode | \
            (uint32_t)ConfigStructPtr->parity | \
            (uint32_t)ConfigStructPtr->wordLength;
    UsartPtr->CTRL1_R.CTRL1 = temp;

    /** STOP bits */
    temp = UsartPtr->CTRL2_R.CTRL2;
    temp &= 0xCFFFu;
    temp |= (uint32_t)(ConfigStructPtr->stopBits);
    UsartPtr->CTRL2_R.CTRL2 = temp;

    /** Hardware Flow Control */
    temp = UsartPtr->CTRL3_R.CTRL3;
    temp &= 0xFCFFu;
    temp |= (uint32_t)ConfigStructPtr->hardwareFlowCtrl;
    UsartPtr->CTRL3_R.CTRL3 = temp;

    if (UsartPtr == USART1)
    {
        fCLK = Rcm_ReadUsartClkFreq(USART_1);
    }
    else if (UsartPtr == USART2)
    {
        fCLK = Rcm_ReadUsartClkFreq(USART_2);
    }
    else
    {
        fCLK = Rcm_ReadPclkFreq();
    }
    /* Compute the integer part */
    if (UsartPtr->CTRL1_R.CTRL1_B.OSMCFG != (uint32_t)RESET) /*!< Oversampling by 8 */
    {
        intDiv = ((25u * fCLK) / (2u * (ConfigStructPtr->baudRate)));
    }
    else /*!< Oversampling by 16 */
    {
        intDiv = ((25u * fCLK) / (4u * (ConfigStructPtr->baudRate)));
    }

    temp = (intDiv / 100u) << 4u;
    fractionalDiv = intDiv - (100u * (temp >> 4u));

    /* Implement the fractional part in the register */
    if (UsartPtr->CTRL1_R.CTRL1_B.OSMCFG != (uint32_t)RESET) /*!< Oversampling by 8 */
    {
        temp |= ((((fractionalDiv * 8u) + 50u) / 100u)) & ((uint8_t)0x07u);
    }
    else /*!< Oversampling by 16 */
    {
        temp |= ((((fractionalDiv * 16u) + 50u) / 100u)) & ((uint8_t)0x0Fu);
    }

    UsartPtr->BR_R.BR = temp;
}

/*!
 * @brief       Fills each USART_InitStruct member with its default Value
 *
 * @param       ConfigStructPtr:  pointer to a Usart_ConfigType structure which will be initialized
 *
 * @retval      None
 */
void Usart_ConfigStructInit(Usart_ConfigType* ConfigStructPtr)
{
    ConfigStructPtr->baudRate = 9600;
    ConfigStructPtr->wordLength = USART_WORD_LEN_8B;
    ConfigStructPtr->stopBits = USART_STOP_BIT_1;
    ConfigStructPtr->parity = USART_PARITY_NONE ;
    ConfigStructPtr->mode = USART_MODE_TX_RX;
    ConfigStructPtr->hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
}

/*!
 * @brief       Synchronous communication clock configuration
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       SyncClockConfigPtr:    Pointer to a Usart_SyncClockConfigType structure that
 *                                  contains the configuration information for the clock
 *
 * @retval      None
 */
void Usart_ConfigSyncClock(USART_T* UsartPtr, const Usart_SyncClockConfigType* SyncClockConfigPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.CLKEN = (uint8_t)SyncClockConfigPtr->enable;
    UsartPtr->CTRL2_R.CTRL2_B.CPHA = (uint8_t)SyncClockConfigPtr->phase;
    UsartPtr->CTRL2_R.CTRL2_B.CPOL = (uint8_t)SyncClockConfigPtr->polarity;
    UsartPtr->CTRL2_R.CTRL2_B.LBCPOEN = (uint8_t)SyncClockConfigPtr->lastBitClock;
}

/*!
 * @brief       Fills each SyncClockConfigPtr member with its default Value
 *
 * @param       SyncClockConfigPtr:    Pointer to a Usart_SyncClockConfigType structure
 *
 * @retval      None
 */
void Usart_ConfigSyncClockStructInit(Usart_SyncClockConfigType* SyncClockConfigPtr)
{
    SyncClockConfigPtr->enable = USART_CLKEN_DISABLE;
    SyncClockConfigPtr->phase = USART_CLKPHA_1EDGE;
    SyncClockConfigPtr->polarity = USART_CLKPOL_LOW;
    SyncClockConfigPtr->lastBitClock = USART_LBCP_DISABLE;
}

/*!
 * @brief       Enables the specified USART peripheral
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_Enable(USART_T* UsartPtr)
{
    UsartPtr->CTRL1_R.CTRL1_B.UEN = BIT_SET;
}

/*!
 * @brief       Disables the specified USART peripheral
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_Disable(USART_T* UsartPtr)
{
    UsartPtr->CTRL1_R.CTRL1_B.UEN = BIT_RESET;
}

/*!
 * @brief       Enables the USART direction Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Mode:   Specifies the USART direction
 *                      The parameter can be one of following values:
 *                      @arg USART_MODE_RX:    USART Transmitter
 *                      @arg USART_MODE_TX:    USART Receiver
 *                      @arg USART_MODE_TX_RX: USART Transmitter and Receiver
 *
 * @retval      None
 */
void Usart_EnableDirectionMode(USART_T* UsartPtr, Usart_ModeType Mode)
{
    if (Mode == USART_MODE_RX)
    {
        UsartPtr->CTRL1_R.CTRL1_B.RXEN = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if (Mode == USART_MODE_TX)
    {
        UsartPtr->CTRL1_R.CTRL1_B.TXEN = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if (Mode == USART_MODE_TX_RX)
    {
        UsartPtr->CTRL1_R.CTRL1_B.TXEN = BIT_SET;
        UsartPtr->CTRL1_R.CTRL1_B.RXEN = BIT_SET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Disables the USART direction Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Mode:   Specifies the USART direction
 *                      The parameter can be one of following values:
 *                      @arg USART_MODE_RX:    USART Transmitter
 *                      @arg USART_MODE_TX:    USART Receiver
 *                      @arg USART_MODE_TX_RX: USART Transmitter and Receiver
 *
 * @retval      None
 */
void Usart_DisableDirectionMode(USART_T* UsartPtr, Usart_ModeType Mode)
{
    if (Mode == USART_MODE_RX)
    {
        UsartPtr->CTRL1_R.CTRL1_B.RXEN = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }

    if (Mode == USART_MODE_TX)
    {
        UsartPtr->CTRL1_R.CTRL1_B.TXEN = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }

    if (Mode == USART_MODE_TX_RX)
    {
        UsartPtr->CTRL1_R.CTRL1_B.TXEN = BIT_RESET;
        UsartPtr->CTRL1_R.CTRL1_B.RXEN = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Enables the Over Sampling 8/16 Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 * @note        0: Oversampling by 16  1: Oversampling by 8
 */
void Usart_EnableOverSampling8(USART_T* UsartPtr)
{
    UsartPtr->CTRL1_R.CTRL1_B.OSMCFG = BIT_SET;
}

/*!
 * @brief       Disables the the Over Sampling 8/16 Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 * @note        0: Oversampling by 16  1: Oversampling by 8
 */
void Usart_DisableOverSampling8(USART_T* UsartPtr)
{
    UsartPtr->CTRL1_R.CTRL1_B.OSMCFG = BIT_RESET;
}

/*!
 * @brief       Enables the USART's one bit sampling method.
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableOneBitMethod(USART_T* UsartPtr)
{
    UsartPtr->CTRL3_R.CTRL3_B.SAMCFG = BIT_SET;
}

/*!
 * @brief       Disables the USART's one bit sampling method.
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableOneBitMethod(USART_T* UsartPtr)
{
    UsartPtr->CTRL3_R.CTRL3_B.SAMCFG = BIT_RESET;
}

/*!
 * @brief       Enables the most significant bit first
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableMsbFirst(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.MSBFEN = BIT_SET;
}

/*!
 * @brief       Disables the most significant bit first
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableMsbFirst(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.MSBFEN = BIT_RESET;
}

/*!
 * @brief       Enables the the binary Data inversion
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableDataInv(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.BINVEN = BIT_SET;
}

/*!
 * @brief       Disables the the binary Data inversion
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableDataInv(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.BINVEN = BIT_RESET;
}

/*!
 * @brief       Enables the specified USART peripheral
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       InvPin: specifies the USART pin(s) to invert
 *              This parameter can be one of the following values:
 *              @arg USART_INVERSION_RX: USART Tx pin active level inversion
 *              @arg USART_INVERSION_TX: USART Rx pin active level inversion
 *              @arg USART_INVERSION_TX_RX: USART TX Rx pin active level inversion
 *
 * @retval      None
 */
void Usart_EnableInvPin(USART_T* UsartPtr, Usart_InversionType InvPin)
{
    if (InvPin == USART_INVERSION_RX)
    {
        UsartPtr->CTRL2_R.CTRL2_B.RXINVEN = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if (InvPin == USART_INVERSION_TX)
    {
        UsartPtr->CTRL2_R.CTRL2_B.TXINVEN = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if (InvPin == (USART_INVERSION_TX_RX))
    {
        UsartPtr->CTRL2_R.CTRL2_B.TXINVEN = BIT_SET;
        UsartPtr->CTRL2_R.CTRL2_B.RXINVEN = BIT_SET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Disables the specified USART peripheral
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       InvPin: specifies the USART pin(s) to invert
 *              This parameter can be one of the following values:
 *              @arg USART_INVERSION_RX: USART Tx pin active level inversion
 *              @arg USART_INVERSION_TX: USART Rx pin active level inversion
 *              @arg USART_INVERSION_TX_RX: USART TX Rx pin active level inversion
 *
 * @retval      None
 */
void Usart_DisableInvPin(USART_T* UsartPtr, Usart_InversionType InvPin)
{
    if (InvPin == USART_INVERSION_RX)
    {
        UsartPtr->CTRL2_R.CTRL2_B.RXINVEN = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }

    if (InvPin == USART_INVERSION_TX)
    {
        UsartPtr->CTRL2_R.CTRL2_B.TXINVEN = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }

    if (InvPin == USART_INVERSION_TX_RX)
    {
        UsartPtr->CTRL2_R.CTRL2_B.TXINVEN = BIT_RESET;
        UsartPtr->CTRL2_R.CTRL2_B.RXINVEN = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Enables the swap Tx/Rx pins
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableSwapPin(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.SWAPEN = BIT_SET;
}

/*!
 * @brief       Disables the swap Tx/Rx pins
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableSwapPin(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.SWAPEN = BIT_RESET;
}

/*!
 * @brief       Enables the receiver Time Out feature
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableReceiverTimeOut(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.RXTODEN = BIT_SET;
}

/*!
 * @brief       Disables the receiver Time Out feature
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableReceiverTimeOut(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.RXTODEN = BIT_RESET;
}

/*!
 * @brief       Sets the receiver Time Out Value
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       TimeOut: Specifies the Receiver Time Out Value
 *
 * @retval      None
 *
 * @note        The Value must less than 0x00FFFFFF
 */
void Usart_ReceiverTimeOutValue(USART_T* UsartPtr, uint32_t TimeOut)
{
    UsartPtr->RXTO_R.RXTO = (uint32_t)0x00;

    if (TimeOut <= (uint32_t)0x00FFFFFF)
    {
        UsartPtr->RXTO_R.RXTO = ((uint32_t)TimeOut & 0x00FFFFFFu);
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Enables the auto baud rate
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableAutoBaudRate(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.ABRDEN = BIT_SET;
}

/*!
 * @brief       Disables the auto baud rate
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableAutoBaudRate(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.ABRDEN = BIT_RESET;
}

/*!
 * @brief       Enables the auto baud rate
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Mode:   specifies the selected USART auto baud rate method
 *                      This parameter can be one of the following values:
 *                      @arg USART_AUTO_BAUD_RATE_STARTBIT:    Start Bit duration measurement
 *                      @arg USART_AUTO_BAUD_RATE_FALLINGEDGE: Falling edge to falling edge measurement
 *
 * @retval      None
 */
void Usart_ConfigAutoBaudRate(USART_T* UsartPtr, Usart_AutoBaudRateType Mode)
{
    UsartPtr->CTRL2_R.CTRL2_B.ABRDCFG = (uint8_t)Mode;
}

/*!
 * @brief       Transmit Data
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Data:    Specifies the Transmits Data Value
 *
 * @retval      None
 *
 * @note        The Value must less than 0x01FF
 */
void Usart_TxData(USART_T* UsartPtr, uint16_t Data)
{
    UsartPtr->TXDATA_R.TXDATA = (uint16_t)(Data & (uint16_t)0x01FF);
}

/*!
 * @brief       Transmit String Data
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Data:    Specifies the Transmits String Data
 *
 * @retval      Sended char numbers
 */
uint32_t Usart_SendString(USART_T* UsartPtr, const char* Str)
{
    const char *pStr = Str; 
    uint32_t len = 0U;

    if ((UsartPtr == NULL) || (Str == NULL))
    {
        len = 0U;
    }
    else
    {
        while (*pStr != '\0')
        {
            while (Usart_ReadStatusFlag(UsartPtr, USART_FLAG_TXBE) == (uint8_t)RESET)
            {
                /* nothing */
            }
            Usart_TxData(UsartPtr, (uint16_t)(*pStr));
            pStr++;
            len++;
        }
    }

    return len;
}

/*!
 * @brief       Received Data
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      Returns the received Data Value
 */
uint16_t Usart_RxData(const USART_T* UsartPtr)
{
    return (uint16_t)(UsartPtr->RXDATA_R.RXDATA & (uint16_t)0x01FF);
}

/*!
 * @brief       Sets USART the Address
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_Address(USART_T* UsartPtr, uint8_t Address)
{
    UsartPtr->CTRL2_R.CTRL2_B.ADDRL = ((uint8_t)Address & 0x0Fu);
    UsartPtr->CTRL2_R.CTRL2_B.ADDRH = (((uint8_t)Address >> 4) & 0x0Fu);

}

/*!
 * @brief       Enables the USART's mute Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableMuteMode(USART_T* UsartPtr)
{
    UsartPtr->CTRL1_R.CTRL1_B.RXMUTEEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's mute Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableMuteMode(USART_T* UsartPtr)
{
    UsartPtr->CTRL1_R.CTRL1_B.RXMUTEEN = BIT_RESET;
}

/*!
 * @brief       Selects the USART WakeUp method from mute Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Wakeup: Specifies the selected USART auto baud rate method
 *                      This parameter can be one of the following values:
 *                      @arg USART_WAKEUP_IDLE_LINE:    WakeUp by an idle line detection
 *                      @arg USART_WAKEUP_ADDRESS_MARK: WakeUp by an Address mark
 *
 * @retval      None
 */
void Usart_ConfigMuteModeWakeUp(USART_T* UsartPtr, Usart_WakeUpType Wakeup)
{
    UsartPtr->CTRL1_R.CTRL1_B.WUPMCFG = (uint8_t)Wakeup;
}

/*!
 * @brief       Enables the USART's Half-duplex Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 * @retval      None
 */
void Usart_EnableHalfDuplex(USART_T* UsartPtr)
{
    UsartPtr->CTRL3_R.CTRL3_B.HDEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's Half-duplex Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 * @retval      None
 */
void Usart_DisableHalfDuplex(USART_T* UsartPtr)
{
    UsartPtr->CTRL3_R.CTRL3_B.HDEN = BIT_RESET;
}

/*!
 * @brief       Configure the the USART Address detection length.
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Address: Specifies the selected USART auto baud rate method
 *                       This parameter can be one of the following values:
 *                       @arg USART_ADDRESS_MODE_4B: 4-bit Address length detection
 *                       @arg USART_ADDRESS_MODE_7B: 7-bit Address length detection
 *
 * @retval      None
 */
void Usart_ConfigAddressDetection(USART_T* UsartPtr, Usart_AddressModeType Address)
{
    UsartPtr->CTRL2_R.CTRL2_B.ADDRLEN = (uint8_t)Address;
}

/*!
 * @brief       Enables the DE functionality
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_EnableDe(USART_T* UsartPtr)
{
    UsartPtr->CTRL3_R.CTRL3_B.DEN = BIT_SET;
}

/*!
 * @brief       Disables the DE functionality
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 */
void Usart_DisableDe(USART_T* UsartPtr)
{
    UsartPtr->CTRL3_R.CTRL3_B.DEN = BIT_RESET;
}

/*!
 * @brief       Selects the USART WakeUp method from mute Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Polarity: Specifies the selected USART auto baud rate method
 *                        This parameter can be one of the following values:
 *                        @arg USART_DE_POL_HIGH:  DE signal is active high
 *                        @arg USART_DE_POL_LOW:   DE signal is active low
 *
 * @retval      None
 */
void Usart_ConfigDePolarity(USART_T* UsartPtr, Usart_DEPolType Polarity)
{
    UsartPtr->CTRL3_R.CTRL3_B.DPCFG = (uint8_t)Polarity;
}

/*!
 * @brief       Sets the driver enable assertion time Value
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Value:  Specifies the DE assertion time Value
 *
 * @retval      None
 */
void Usart_DeAssertionTimeValue(USART_T* UsartPtr, uint8_t Value)
{
    UsartPtr->CTRL1_R.CTRL1_B.DLTEN = (uint8_t)0x00;

    if (Value <= (uint8_t)0x1F)
    {
        UsartPtr->CTRL1_R.CTRL1_B.DLTEN = ((uint8_t)Value & 0x1Fu);
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Sets the driver enable deassertion time Value
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Value:  Specifies the DE deassertion time Value
 *
 * @retval      None
 */
void Usart_DeDeassertionTimeValue(USART_T* UsartPtr, uint8_t Value)
{
    UsartPtr->CTRL1_R.CTRL1_B.DDLTEN = (uint8_t)0x00;

    if (Value <= (uint8_t)0x1F)
    {
        UsartPtr->CTRL1_R.CTRL1_B.DDLTEN = ((uint8_t)Value & 0x1Fu);
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Enables the USART DMA interface
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       DmaReq: Specifies the DMA Request
 *                      This parameter can be any combination of the following values:
 *                      @arg USART_DMA_REQUEST_RX:  USART DMA receive Request
 *                      @arg USART_DMA_REQUEST_TX:  USART DMA transmit Request
 *
 * @retval      None
 */
void Usart_EnableDma(USART_T* UsartPtr, uint32_t DmaReq)
{
    if (DmaReq == (uint32_t)USART_DMA_REQUEST_RX)
    {
        UsartPtr->CTRL3_R.CTRL3_B.DMARXEN = BIT_SET;
    }
    else if (DmaReq == (uint32_t)USART_DMA_REQUEST_TX)
    {
        UsartPtr->CTRL3_R.CTRL3_B.DMATXEN = BIT_SET;
    }
    else if (DmaReq == (BIT6 | BIT7))
    {
        UsartPtr->CTRL3_R.CTRL3_B.DMATXEN = BIT_SET;
        UsartPtr->CTRL3_R.CTRL3_B.DMARXEN = BIT_SET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Disables the USART DMA interface
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       DmaReq: Specifies the DMA Request
 *                      This parameter can be any combination of the following values:
 *                      @arg USART_DMA_REQUEST_RX:  USART DMA receive Request
 *                      @arg USART_DMA_REQUEST_TX:  USART DMA transmit Request
 *
 * @retval      None
 */
void Usart_DisableDma(USART_T* UsartPtr, uint32_t DmaReq)
{
    if (DmaReq == (uint32_t)USART_DMA_REQUEST_RX)
    {
        UsartPtr->CTRL3_R.CTRL3_B.DMARXEN = BIT_RESET;
    }
    else if (DmaReq == (uint32_t)USART_DMA_REQUEST_TX)
    {
        UsartPtr->CTRL3_R.CTRL3_B.DMATXEN = BIT_RESET;
    }
    else if (DmaReq == (BIT6 | BIT7))
    {
        UsartPtr->CTRL3_R.CTRL3_B.DMATXEN = BIT_RESET;
        UsartPtr->CTRL3_R.CTRL3_B.DMARXEN = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Enables or disables the USART DMA interface when reception Error occurs
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       DmaReq: Specifies the DMA Request
 *                      This parameter can be one of the following values:
 *                      @arg USART_DMA_RXERR_ENABLE:   DMA receive Request enabled
 *                      @arg USART_DMA_RXERR_DISABLE:  DMA receive Request disabled
 *
 * @retval      None
 */
void Usart_ConfigDmaReceptionError(USART_T* UsartPtr, Usart_DmaRxErrType Error)
{
    UsartPtr->CTRL3_R.CTRL3_B.DDISRXEEN = (uint8_t)Error;
}

/*!
 * @brief       Enables the specified interrupts
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Interrupt:  Specifies the USART interrupts sources
 *                          The parameter can be one of following values:
 *                          @arg USART_INT_CMIE:    Character match Interrupt
 *                          @arg USART_INT_EOBIE:   End of Block Interrupt
 *                          @arg USART_INT_RXTOIE:  Receive time out Interrupt
 *                          @arg USART_INT_CTSIE:   CTS change Interrupt
 *                          @arg USART_INT_TXBEIE:  Tansmit Data Register empty Interrupt
 *                          @arg USART_INT_TXCIE:   Transmission complete Interrupt
 *                          @arg USART_INT_RXBNEIE: Receive Data register not empty Interrupt
 *                          @arg USART_INT_IDLEIE:  Idle line detection Interrupt
 *                          @arg USART_INT_PEIE:    Parity Error Interrupt
 *                          @arg USART_INT_ERRIE:   Error Interrupt
 *                          @arg USART_INT_LBDIE:   LIN Break Detection Interrupt
 *
 * @retval      None
 */
void Usart_EnableInterrupt(USART_T* UsartPtr, Usart_IntType Interrupt)
{
    if ((Interrupt == USART_INT_ERRIE) || (Interrupt == USART_INT_CTSIE))
    {
        UsartPtr->CTRL3_R.CTRL3 |= (uint32_t)Interrupt;
    }
    else if (Interrupt == USART_INT_LBDIE)
    {
        UsartPtr->CTRL2_R.CTRL2 |= (uint32_t)Interrupt;
    }
    else
    {
        UsartPtr->CTRL1_R.CTRL1 |= (uint32_t)Interrupt;
    }
}

/*!
 * @brief       Disables the specified interrupts
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Interrupt:  Specifies the USART interrupts sources
 *                          The parameter can be one of following values:
 *                          @arg USART_INT_CMIE:    Character match Interrupt
 *                          @arg USART_INT_EOBIE:   End of Block Interrupt
 *                          @arg USART_INT_RXTOIE:  Receive time out Interrupt
 *                          @arg USART_INT_CTSIE:   CTS change Interrupt
 *                          @arg USART_INT_TXBEIE:  Tansmit Data Register empty Interrupt
 *                          @arg USART_INT_TXCIE:   Transmission complete Interrupt
 *                          @arg USART_INT_RXBNEIE: Receive Data register not empty Interrupt
 *                          @arg USART_INT_IDLEIE:  Idle line detection Interrupt
 *                          @arg USART_INT_PEIE:    Parity Error Interrupt
 *                          @arg USART_INT_ERRIE:   Error Interrupt
 *                          @arg USART_INT_LBDIE:   LIN Break Detection Interrupt
 *
 * @retval      None
 */
void Usart_DisableInterrupt(USART_T* UsartPtr, Usart_IntType Interrupt)
{
    if ((Interrupt == USART_INT_ERRIE) || (Interrupt == USART_INT_CTSIE))
    {
        UsartPtr->CTRL3_R.CTRL3 &= (uint32_t)~(uint32_t)Interrupt;
    }
    else if (Interrupt == USART_INT_LBDIE)
    {
        UsartPtr->CTRL2_R.CTRL2 &= (uint32_t)~(uint32_t)Interrupt;
    }
    else
    {
        UsartPtr->CTRL1_R.CTRL1 &= (uint32_t)~(uint32_t)Interrupt;
    }
}

/*!
 * @brief       Enables the specified USART's Request.
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Request: specifies the USART Request
 *              This parameter can be one of the following values:
 *              @arg USART_REQUEST_ABRDQ: Auto Baud Rate Request
 *              @arg USART_REQUEST_TXBFQ:  Send Break Request
 *              @arg USART_REQUEST_MUTEQ:  Mute Mode Request
 *              @arg USART_REQUEST_RXDFQ: Receive Data flush Request
 *
 * @retval      None
 */
void Usart_EnableRequest(USART_T* UsartPtr, Usart_RequestType Request)
{
    if (Request == USART_REQUEST_ABRDQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.ABRDQ = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if (Request == USART_REQUEST_TXBFQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.TXBFQ = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if (Request == USART_REQUEST_MUTEQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.MUTEQ = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if (Request == USART_REQUEST_RXDFQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.RXDFQ = BIT_SET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Disables the specified USART's Request.
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Request: specifies the USART Request
 *              This parameter can be one of the following values:
 *              @arg USART_REQUEST_ABRDQ: Auto Baud Rate Request
 *              @arg USART_REQUEST_TXBFQ:  Send Break Request
 *              @arg USART_REQUEST_MUTEQ:  Mute Mode Request
 *              @arg USART_REQUEST_RXDFQ: Receive Data flush Request
 *
 * @retval      None
 */
void Usart_DisableRequest(USART_T* UsartPtr, Usart_RequestType Request)
{
    if (Request == USART_REQUEST_ABRDQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.ABRDQ = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }

    if (Request == USART_REQUEST_TXBFQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.TXBFQ = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }

    if (Request == USART_REQUEST_MUTEQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.MUTEQ = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }

    if (Request == USART_REQUEST_RXDFQ)
    {
        UsartPtr->REQUEST_R.REQUEST_B.RXDFQ = BIT_RESET;
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief       Enables or disables the USART DMA interface when reception Error occurs
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       OverDetection: specifies the OVR detection status in case of OVR Error
 *                      This parameter can be one of the following values:
 *                      @arg USART_OVER_DETECTION_ENABLE:   OVR Error detection enabled
 *                      @arg USART_OVER_DETECTION_DISABLE:  OVR Error detection disabled
 *
 * @retval      None
 */
void Usart_ConfigOverrunDetection(USART_T* UsartPtr, Usart_OverDetectionType OverDetection)
{
    UsartPtr->CTRL3_R.CTRL3_B.OVRDEDIS = (uint8_t)OverDetection;
}

/*!
 * @brief       Read the specified USART Flag
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Flag:   Specifies the Flag to check
 *                      The parameter can be one of following values:
 *                      @arg USART_FLAG_RXENACKF: Receive Enable Acknowledge Flag
 *                      @arg USART_FLAG_TXENACKF: Transmit Enable Acknowledge Flag
 *                      @arg USART_FLAG_SBF:    Send Break Flag
 *                      @arg USART_FLAG_CMF:    Character match Flag
 *                      @arg USART_FLAG_BUSY:   Busy Flag
 *                      @arg USART_FLAG_ABRTF:  Auto baud rate Flag
 *                      @arg USART_FLAG_ABRTE:  Auto baud rate Error Flag
 *                      @arg USART_FLAG_RXTOF:  Receive time out Flag
 *                      @arg USART_FLAG_CTSF:   CTS Change Flag
 *                      @arg USART_FLAG_CTSIF:  CTS Interrupt Flag
 *                      @arg USART_FLAG_TXBE:   Transmit Data register empty Flag
 *                      @arg USART_FLAG_TXC:    Transmission Complete Flag
 *                      @arg USART_FLAG_RXBNE:  Receive Data buffer not empty Flag
 *                      @arg USART_FLAG_IDLEF:  Idle Line detection Flag
 *                      @arg USART_FLAG_OVRE:   OverRun Error Flag
 *                      @arg USART_FLAG_NEF:    Noise Error Flag
 *                      @arg USART_FLAG_FEF:    Framing Error Flag
 *                      @arg USART_FLAG_PEF:    Parity Error Flag
 *
 * @retval      The new state of Flag (SET or RESET)
 */

uint8_t Usart_ReadStatusFlag(const USART_T* UsartPtr, Usart_FlagType Flag)
{
    uint8_t state = RESET;

    if ((UsartPtr->STS_R.STS & (uint32_t)Flag) != (uint32_t)RESET)
    {
        state = SET;
    }
    else
    {
        state = RESET;
    }

    return state;
}

/*!
 * @brief       Clear the specified USART Flag
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Flag:   Specifies the Flag to clear
 *                      The parameter can be any combination of following values:
 *                      @arg USART_FLAG_CMF:    Character match Flag
 *                      @arg USART_FLAG_RXTOF:  Receive time out Flag
 *                      @arg USART_FLAG_CTSIF:  CTS Interrupt Flag
 *                      @arg USART_FLAG_TXC:    Transmission Complete Flag
 *                      @arg USART_FLAG_IDLEF:  Idle Line detection Flag
 *                      @arg USART_FLAG_OVRE:   OverRun Error Flag
 *                      @arg USART_FLAG_NEF:    Noise Error Flag
 *                      @arg USART_FLAG_FEF:    Framing Error Flag
 *                      @arg USART_FLAG_PEF:    Parity Error Flag
 *
 * @retval      Note
 */

void Usart_ClearStatusFlag(USART_T* UsartPtr, Usart_FlagType Flag)
{
    UsartPtr->INTFCLR_R.INTFCLR = (uint32_t)Flag;
}

/*!
 * @brief       Read the specified USART Interrupt Flag
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Flag:   Specifies the USART Interrupt Flag to check
 *                      The parameter can be one of following values:
 *                      @arg USART_INT_FLAG_CMF:    Character match Interrupt Flag
 *                      @arg USART_INT_FLAG_RXTOF:  Receive time out Interrupt Flag
 *                      @arg USART_INT_FLAG_CTSIF:  CTS Interrupt Flag
 *                      @arg USART_INT_FLAG_TXBE:   Transmit Data register empty Interrupt Flag
 *                      @arg USART_INT_FLAG_TXC:    Transmission Complete Interrupt Flag
 *                      @arg USART_INT_FLAG_RXBNE:  Receive Data buffer not empty Interrupt Flag
 *                      @arg USART_INT_FLAG_IDLE:   Idle Line detection Interrupt Flag
 *                      @arg USART_INT_FLAG_OVRE:   OverRun Error Interrupt Flag
 *                      @arg USART_INT_FLAG_NE:     Noise Error Interrupt Flag
 *                      @arg USART_INT_FLAG_FE:     Framing Error Interrupt Flag
 *                      @arg USART_INT_FLAG_PE:     Parity Error Interrupt Flag
 *
 * @retval      The new state of Flag (SET or RESET)
 */

uint8_t Usart_ReadIntFlag(const USART_T* UsartPtr, Usart_IntFlagType Flag)
{
    uint32_t intEnable = 0;
    uint32_t intFlag = 0;
    uint8_t state = RESET;

    if (0u != ((uint32_t)Flag & 0x0Eu))
    {
        intEnable = UsartPtr->CTRL3_R.CTRL3_B.ERRIEN;
        intFlag = (UsartPtr->STS_R.STS) & (uint32_t)Flag;
    }
    else if (0u != ((uint32_t)Flag & 0xF0u))
    {
        intEnable = (UsartPtr->CTRL1_R.CTRL1)& (uint32_t)Flag;
        intFlag = (UsartPtr->STS_R.STS) & (uint32_t)Flag;
    }
    else if (0u != ((uint32_t)Flag & 0x01u))
    {
        intEnable = UsartPtr->CTRL1_R.CTRL1_B.PEIEN;
        intFlag = UsartPtr->STS_R.STS_B.PEFLG;
    }
    else if (0u != ((uint32_t)Flag & 0x100u))
    {
        intEnable = UsartPtr->CTRL2_R.CTRL2_B.LBDIEN;
        intFlag = UsartPtr->STS_R.STS_B.LBDFLG;
    }
    else if (0u != ((uint32_t)Flag & 0x200u))
    {
        intEnable = UsartPtr->CTRL3_R.CTRL3_B.CTSIEN;
        intFlag = UsartPtr->STS_R.STS_B.CTSFLG;
    }
    else if (0u != ((uint32_t)Flag & 0x800u))
    {
        intEnable = UsartPtr->CTRL1_R.CTRL1_B.RXTOIEN;
        intFlag = UsartPtr->STS_R.STS_B.RXTOFLG;
    }
    else if (0u != ((uint32_t)Flag & 0x20000u))
    {
        intEnable = UsartPtr->CTRL1_R.CTRL1_B.CMIEN;
        intFlag = UsartPtr->STS_R.STS_B.CMFLG;
    }
    else
    {
        /* do nothing */
    }

    if ((0u != (uint32_t)intFlag) && (0u != (uint32_t)intEnable))
    {
        state = SET;
    }
    else
    {
        state = RESET;
    }

    return state;
}

/*!
 * @brief       Clears the USART Interrupt pending bits
 *
 * @param       UsartPtr:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       Flag:   Specifies the USART Interrupt Flag to clear
 *                      The parameter can be any combination following values:
 *                      @arg USART_INT_FLAG_CMF:    Character match Interrupt Flag
 *                      @arg USART_INT_FLAG_RXTOF:  Receive time out Interrupt Flag
 *                      @arg USART_INT_FLAG_CTSIF:  CTS Interrupt Flag
 *                      @arg USART_INT_FLAG_TXC:    Transmission Complete Interrupt Flag
 *                      @arg USART_INT_FLAG_IDLE:   Idle Line detection Interrupt Flag
 *                      @arg USART_INT_FLAG_OVRE:   OverRun Error Interrupt Flag
 *                      @arg USART_INT_FLAG_NE:     Noise Error Interrupt Flag
 *                      @arg USART_INT_FLAG_FE:     Framing Error Interrupt Flag
 *                      @arg USART_INT_FLAG_PE:     Parity Error Interrupt Flag
 *
 * @retval      None
 */
void Usart_ClearIntFlag(USART_T* UsartPtr, Usart_IntFlagType Flag)
{
    UsartPtr->INTFCLR_R.INTFCLR |= (uint32_t)Flag;
}

/*!
 * @brief       Sets the USART LIN Break detection length configure
 *
 * @param       UsartPtr:  Select the the USART peripheral.It can be USART1/USART2
 *
 * @param       lbdl:   LIN Break detection length configure
 *
 * @retval      None
 *
 * @note
 */
void Usart_ConfigLinBreakDetectLenCfg(USART_T* UsartPtr, Usart_LbdlcType Lbdlc)
{
    UsartPtr->CTRL2_R.CTRL2_B.LBDLCFG = (uint8_t)Lbdlc;
}

/*!
 * @brief       Enables the USART LIN Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.It can be USART1/USART2
 *
 * @retval      None
 *
 * @note
 */
void Usart_EnableLin(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.LINMEN = BIT_SET;
}

/*!
 * @brief       Disable the USART LIN Mode
 *
 * @param       UsartPtr:  Select the the USART peripheral.It can be USART1/USART2
 *
 * @retval      None
 *
 * @note
 */
void Usart_DisableLin(USART_T* UsartPtr)
{
    UsartPtr->CTRL2_R.CTRL2_B.LINMEN = BIT_RESET;
}

/**@} end of group USART_Functions*/
/**@} end of group USART_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/
