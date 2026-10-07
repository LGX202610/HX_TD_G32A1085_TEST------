/*!
 * @file        g32a10xx_usart.h
 *
 * @brief       This file contains all the functions prototypes for the USART firmware library
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

#ifndef G32A10xx_USART_H
#define G32A10xx_USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g32a10xx.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup USART_Driver
  @{
*/

/** @defgroup USART_Macros Macros
  @{
*/

/** Macros description */
#define USART_MACROS      1

/**@} end of group USART_Macros*/

/** @defgroup USART_Enumerations Enumerations
  @{
*/

/**
 * @brief   USART Word Length define
 */
typedef enum
{
    USART_WORD_LEN_8B = 0,            //!< 8-bit Data length
    USART_WORD_LEN_9B = BIT12         //!< 9-bit Data length
} Usart_WordLenType;

/**
 * @brief   USART Stop bits define
 */
typedef enum
{
    USART_STOP_BIT_1    = 0,             //!< 1-bit stop bit
    USART_STOP_BIT_2    = BIT13,         //!< 2-bit stop bit
    USART_STOP_BIT_1_5  = BIT12 | BIT13  //!< 1.5-bit stop bit
} Usart_StopBitType;

/**
 * @brief   USART Parity define
 */
typedef enum
{
    USART_PARITY_NONE   = 0,            //!< Disable parity control
    USART_PARITY_EVEN   = BIT10,        //!< Enable even parity control
    USART_PARITY_ODD    = BIT10 | BIT9  //!< Enable odd parity control
} Usart_ParityType;

/**
 * @brief   USART Mode define
 */
typedef enum
{
    USART_MODE_RX       = BIT2,        //!< Enable USART Receive Mode
    USART_MODE_TX       = BIT3,        //!< Enable USART transmit Mode
    USART_MODE_TX_RX    = BIT2 | BIT3  //!< Enable USART receive and transmit Mode
} Usart_ModeType;

/**
 * @brief   USART hardware flow control define
 */
typedef enum
{
    USART_FLOW_CTRL_NONE    = 0,           //!< Disable hardware flow control
    USART_FLOW_CTRL_RTS     = BIT8,        //!< Enable RTS hardware flow control
    USART_FLOW_CTRL_CTS     = BIT9,        //!< Enable CTS hardware flow control
    USART_FLOW_CTRL_RTS_CTS = BIT8 | BIT9  //!< Enable RTS and CTS hardware flow control
} Usart_HardwareFlowCtrlType;

/**
 * @brief    USART synchronization clock enable/disable
 */
typedef enum
{
    USART_CLKEN_DISABLE     = ((uint8_t)0),   //!< Disable UsartPtr clock
    USART_CLKEN_ENABLE      = ((uint8_t)1)    //!< Enable UsartPtr clock
} Usart_ClkEnType;

/**
 * @brief    USART Clock Polarity define
 */
typedef enum
{
    USART_CLKPOL_LOW        = ((uint8_t)0),  //!< Set clock Polarity to low
    USART_CLKPOL_HIGH       = ((uint8_t)1)   //!< Set clock Polarity to high
} Usart_ClkPolType;

/**
 * @brief    USART Clock phase define
 */
typedef enum
{
    USART_CLKPHA_1EDGE      = ((uint8_t)0),  //!< Set UsartPtr to sample at the edge of the first clock
    USART_CLKPHA_2EDGE      = ((uint8_t)1)   //!< Set UsartPtr to sample at the edge of the second clock
} Usart_ClkPhaType;

/**
 * @brief    USART Last bit clock pulse enable
 */
typedef enum
{
    USART_LBCP_DISABLE      = ((uint8_t)0),  //!< Disable output last bit clock pulse
    USART_LBCP_ENABLE       = ((uint8_t)1)   //!< Enable output last bit clock pulse
} Usart_LBCPType;

/**
 * @brief   USART DMA requests
 */
typedef enum
{
    USART_DMA_REQUEST_RX    = BIT6,  //!< USART DMA receive Request
    USART_DMA_REQUEST_TX    = BIT7   //!< USART DMA transmit Request
} Usart_DmaRequestType;

/**
 * @brief   USART DMA reception Error
 */
typedef enum
{
    USART_DMA_RXERR_ENABLE  = ((uint8_t)0),  //!< USART DMA reception Error enable
    USART_DMA_RXERR_DISABLE = ((uint8_t)1)   //!< USART DMA reception Error disable
} Usart_DmaRxErrType;

/**
 * @brief    USART Wakeup method
 */
typedef enum
{
    USART_WAKEUP_IDLE_LINE      = ((uint8_t)0),  //!< WakeUp by an idle line detection
    USART_WAKEUP_ADDRESS_MARK   = ((uint8_t)1)   //!< WakeUp by an Address mark
} Usart_WakeUpType;

/**
 * @brief    USART Address Mode
 */
typedef enum
{
    USART_ADDRESS_MODE_4B   = ((uint8_t)0),  //!< 4-bit Address detection
    USART_ADDRESS_MODE_7B   = ((uint8_t)1)   //!< 7-bit Address detection
} Usart_AddressModeType;

/**
 * @brief    USART driver enable Polarity select
 */
typedef enum
{
    USART_DE_POL_HIGH       = ((uint8_t)0),  //!< driver enable Polarity is high
    USART_DE_POL_LOW        = ((uint8_t)1)   //!< driver enable Polarity is low
} Usart_DEPolType;

/**
 * @brief    USART inversion Pins
 */
typedef enum
{
    USART_INVERSION_RX       = BIT16,       //!< UsartPtr RX Pins active level inversion
    USART_INVERSION_TX       = BIT17,       //!< UsartPtr TX Pins active level inversion
    USART_INVERSION_TX_RX    = BIT16 | BIT17 //!< UsartPtr RX TX Pins active level inversion
} Usart_InversionType;

/**
 * @brief    USART IrDA Low Power
 */
typedef enum
{
    USART_IRDA_MODE_NORMAL     = ((uint8_t)0),  //!< UsartPtr irda works in normal Mode
    USART_IRDA_MODE_LOWPOWER   = ((uint8_t)1)   //!< UsartPtr irda works in low-power Mode
} Usart_IrdaModeType;

/**
 * @brief    USART auto baud rate Mode
 */
typedef enum
{
    USART_AUTO_BAUD_RATE_STARTBIT     = ((uint8_t)0x00),  //!< auto-baud measure start bit
    USART_AUTO_BAUD_RATE_FALLINGEDGE  = ((uint8_t)0x01),  //!< auto-baud measure falling edge
    USART_AUTO_BAUD_RATE_0X7F         = ((uint8_t)0x02),  //!< auto-baud measure 0x7F
    USART_AUTO_BAUD_RATE_0X55         = ((uint8_t)0x03)   //!< auto-baud measure 0x55
} Usart_AutoBaudRateType;

/**
 * @brief    USART  over detection  disable
 */
typedef enum
{
    USART_OVER_DETECTION_ENABLE     = ((uint8_t)0),  //!< enable overrun detection
    USART_OVER_DETECTION_DISABLE    = ((uint8_t)1)   //!< disable overrun detection
} Usart_OverDetectionType;

/**
 * @brief   USART Request
 */
typedef enum
{
    USART_REQUEST_ABRDQ    = ((uint8_t)0x01),         //!< Auto Baud Rate Request
    USART_REQUEST_TXBFQ    = ((uint8_t)0x02),         //!< Send Break Request
    USART_REQUEST_MUTEQ    = ((uint8_t)0x04),         //!< Mute Mode Request
    USART_REQUEST_RXDFQ    = ((uint8_t)0x08)          //!< Receive Data flush Request
} Usart_RequestType;

/**
 * @brief    USART Flag definition
 */
typedef enum
{
    USART_FLAG_RXENACKF    = ((uint32_t)0x00400000),  //!< Receive Enable Acknowledge Flag
    USART_FLAG_TXENACKF    = ((uint32_t)0x00200000),  //!< Transmit Enable Acknowledge Flag
    USART_FLAG_SBF         = ((uint32_t)0x00040000),  //!< Send Break Flag
    USART_FLAG_CMF         = ((uint32_t)0X00020000),  //!< Character match Flag
    USART_FLAG_BUSY        = ((uint32_t)0X00010000),  //!< Busy Flag
    USART_FLAG_ABRTF       = ((uint32_t)0X00008000),  //!< Auto baud rate Flag
    USART_FLAG_ABRTE       = ((uint32_t)0X00004000),  //!< Auto baud rate Error Flag
    USART_FLAG_RXTOF       = ((uint32_t)0X00000800),  //!< Receive time out Flag
    USART_FLAG_CTSF        = ((uint32_t)0X00000400),  //!< CTS Change Flag
    USART_FLAG_CTSIF       = ((uint32_t)0X00000200),  //!< CTS Interrupt Flag
    USART_FLAG_LBD         = ((uint32_t)0X00000100),  //!< LBD Interrupt Flag
    USART_FLAG_TXBE        = ((uint32_t)0X00000080),  //!< Transmit Data register empty Flag
    USART_FLAG_TXC         = ((uint32_t)0X00000040),  //!< Transmission Complete Flag
    USART_FLAG_RXBNE       = ((uint32_t)0X00000020),  //!< Receive Data buffer not empty Flag
    USART_FLAG_IDLEF       = ((uint32_t)0X00000010),  //!< Idle Line detection Flag
    USART_FLAG_OVRE        = ((uint32_t)0X00000008),  //!< OverRun Error Flag
    USART_FLAG_NEF         = ((uint32_t)0X00000004),  //!< Noise Error Flag
    USART_FLAG_FEF         = ((uint32_t)0X00000002),  //!< Framing Error Flag
    USART_FLAG_PEF         = ((uint32_t)0X00000001)   //!< Parity Error Flag
} Usart_FlagType;

/**
 * @brief   USART interrupts source
 */
typedef enum
{
    USART_INT_CMIE         = ((uint32_t)0x00004000),  //!< Character match Interrupt
    USART_INT_RXTOIE       = ((uint32_t)0x04000000),  //!< Receive time out Interrupt
    USART_INT_CTSIE        = ((uint32_t)0x00000400),  //!< CTS change Interrupt
    USART_INT_TXBEIE       = ((uint32_t)0x00000080),  //!< Tansmit Data Register empty Interrupt
    USART_INT_TXCIE        = ((uint32_t)0x40000040),  //!< Transmission complete Interrupt
    USART_INT_RXBNEIE      = ((uint32_t)0x00000020),  //!< Receive Data buffer not empty Interrupt
    USART_INT_IDLEIE       = ((uint32_t)0x00000010),  //!< Idle line detection Interrupt
    USART_INT_PEIE         = ((uint32_t)0x00000100),  //!< Parity Error Interrupt
    USART_INT_ERRIE        = ((uint32_t)0x00000001),  //!< Error Interrupt
    USART_INT_LBDIE        = ((uint32_t)0x00000040)   //!< LIN Break Detection Interrupt
} Usart_IntType;

/**
 * @brief   USART Interrupt Flag definition
 */
typedef enum
{
    USART_INT_FLAG_CMF     = ((uint32_t)0X00020000),  //!< Character match Flag
    USART_INT_FLAG_RXTOF   = ((uint32_t)0X00000800),  //!< Receive time out Flag
    USART_INT_FLAG_CTSIF   = ((uint32_t)0X00000200),  //!< CTS Interrupt Flag
    USART_INT_FLAG_LBD     = ((uint32_t)0X00000100),  //!< LBD Interrupt Flag
    USART_INT_FLAG_TXBE    = ((uint32_t)0X00000080),  //!< Transmit Data register empty Flag
    USART_INT_FLAG_TXC     = ((uint32_t)0X00000040),  //!< Transmission Complete Flag
    USART_INT_FLAG_RXBNE   = ((uint32_t)0X00000020),  //!< Receive Data buffer not empty Flag
    USART_INT_FLAG_IDLE    = ((uint32_t)0X00000010),  //!< Idle Line detection Flag
    USART_INT_FLAG_OVRE    = ((uint32_t)0X00000008),  //!< OverRun Error Flag
    USART_INT_FLAG_NE      = ((uint32_t)0X00000004),  //!< Noise Error Flag
    USART_INT_FLAG_FE      = ((uint32_t)0X00000002),  //!< Framing Error Flag
    USART_INT_FLAG_PE      = ((uint32_t)0X00000001)   //!< Parity Error Flag
} Usart_IntFlagType;


/**
 * @brief    USART LIN break detection length configure
 */
typedef enum
{
    USART_LBDLC_10B          = ((uint8_t)0),
    USART_LBDLC_11B          = ((uint8_t)1)
}Usart_LbdlcType;

/**@} end of group USART_Enumerations*/

/** @defgroup USART_Structures Structures
  @{
*/

/**
 * @brief   USART Config struct definition
 */
typedef struct
{
    uint32_t                    baudRate;       //!< Specifies the baud rate
    Usart_WordLenType            wordLength;     //!< Specifies the word length
    Usart_StopBitType           stopBits;       //!< Specifies the stop bits
    Usart_ParityType              parity;         //!< Specifies the parity
    Usart_ModeType                mode;           //!< Specifies the Mode
    Usart_HardwareFlowCtrlType  hardwareFlowCtrl;
} Usart_ConfigType;

/**
 * @brief   USART synchronous communication clock config struct definition
 */
typedef struct
{
    Usart_ClkEnType               enable;          //!< Enable or Disable Clock
    Usart_ClkPolType              polarity;        //!< Specifies the clock Polarity
    Usart_ClkPhaType              phase;           //!< Specifies the clock phase
    Usart_LBCPType                lastBitClock;    //!< Enable or Disable last bit clock
} Usart_SyncClockConfigType;

/**@} end of group USART_Structures*/

/** @addtogroup USART_Functions Functions
  @{
*/

/* USART peripheral Reset and Configuration */
void Usart_Reset(const USART_T* UsartPtr);
void Usart_Config(USART_T* UsartPtr, const Usart_ConfigType* ConfigStructPtr);
void Usart_ConfigStructInit(Usart_ConfigType* ConfigStructPtr);
void Usart_ConfigSyncClock(USART_T* UsartPtr, const Usart_SyncClockConfigType* SyncClockConfigPtr);
void Usart_ConfigSyncClockStructInit(Usart_SyncClockConfigType* SyncClockConfigPtr);
void Usart_Enable(USART_T* UsartPtr);
void Usart_Disable(USART_T* UsartPtr);
void Usart_EnableDirectionMode(USART_T* UsartPtr, Usart_ModeType Mode);
void Usart_DisableDirectionMode(USART_T* UsartPtr, Usart_ModeType Mode);
void Usart_EnableOverSampling8(USART_T* UsartPtr);
void Usart_DisableOverSampling8(USART_T* UsartPtr);
void Usart_EnableMsbFirst(USART_T* UsartPtr);
void Usart_DisableMsbFirst(USART_T* UsartPtr);
void Usart_EnableOneBitMethod(USART_T* UsartPtr);
void Usart_DisableOneBitMethod(USART_T* UsartPtr);
void Usart_EnableDataInv(USART_T* UsartPtr);
void Usart_DisableDataInv(USART_T* UsartPtr);
void Usart_EnableInvPin(USART_T* UsartPtr, Usart_InversionType InvPin);
void Usart_DisableInvPin(USART_T* UsartPtr, Usart_InversionType InvPin);
void Usart_EnableSwapPin(USART_T* UsartPtr);
void Usart_DisableSwapPin(USART_T* UsartPtr);
void Usart_EnableReceiverTimeOut(USART_T* UsartPtr);
void Usart_DisableReceiverTimeOut(USART_T* UsartPtr);
void Usart_ReceiverTimeOutValue(USART_T* UsartPtr, uint32_t TimeOut);
void Usart_EnableAutoBaudRate(USART_T* UsartPtr);
void Usart_DisableAutoBaudRate(USART_T* UsartPtr);
void Usart_ConfigAutoBaudRate(USART_T* UsartPtr, Usart_AutoBaudRateType Mode);
void Usart_ConfigOverrunDetection(USART_T* UsartPtr, Usart_OverDetectionType OverDetection);

/* Address */
void Usart_Address(USART_T* UsartPtr, uint8_t Address);
void Usart_ConfigAddressDetection(USART_T* UsartPtr, Usart_AddressModeType Address);

/* Transmit and receive */
void Usart_TxData(USART_T* UsartPtr, uint16_t Data);
uint16_t Usart_RxData(const USART_T* UsartPtr);
uint32_t Usart_SendString(USART_T* UsartPtr, const char* Str);

/* Mute Mode */
void Usart_EnableMuteMode(USART_T* UsartPtr);
void Usart_DisableMuteMode(USART_T* UsartPtr);
void Usart_ConfigMuteModeWakeUp(USART_T* UsartPtr, Usart_WakeUpType Wakeup);

/* Half-duplex Mode  */
void Usart_EnableHalfDuplex(USART_T* UsartPtr);
void Usart_DisableHalfDuplex(USART_T* UsartPtr);

/* Driver enable Configuration */
void Usart_EnableDe(USART_T* UsartPtr);
void Usart_DisableDe(USART_T* UsartPtr);
void Usart_ConfigDePolarity(USART_T* UsartPtr, Usart_DEPolType Polarity);
void Usart_DeAssertionTimeValue(USART_T* UsartPtr, uint8_t Value);
void Usart_DeDeassertionTimeValue(USART_T* UsartPtr, uint8_t Value);

/* DMA */
void Usart_EnableDma(USART_T* UsartPtr, uint32_t DmaReq);
void Usart_DisableDma(USART_T* UsartPtr, uint32_t DmaReq);
void Usart_ConfigDmaReceptionError(USART_T* UsartPtr, Usart_DmaRxErrType Error);

/* Request */
void Usart_EnableRequest(USART_T* UsartPtr, Usart_RequestType Request);
void Usart_DisableRequest(USART_T* UsartPtr, Usart_RequestType Request);

/* Interrupt */
void Usart_EnableInterrupt(USART_T* UsartPtr, Usart_IntType Interrupt);
void Usart_DisableInterrupt(USART_T* UsartPtr, Usart_IntType Interrupt);
uint8_t Usart_ReadIntFlag(const USART_T* UsartPtr, Usart_IntFlagType Flag);
void Usart_ClearIntFlag(USART_T* UsartPtr, Usart_IntFlagType Flag);

/* Flag */
uint8_t Usart_ReadStatusFlag(const USART_T* UsartPtr, Usart_FlagType Flag);
void Usart_ClearStatusFlag(USART_T* UsartPtr, Usart_FlagType Flag);

/* Lin */
void Usart_ConfigLinBreakDetectLenCfg(USART_T* UsartPtr, Usart_LbdlcType Lbdlc);
void Usart_EnableLin(USART_T* UsartPtr);
void Usart_DisableLin(USART_T* UsartPtr);

#ifdef __cplusplus
}
#endif

#endif /* G32A10xx_USART_H */

/**@} end of group USART_Functions */
/**@} end of group USART_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
