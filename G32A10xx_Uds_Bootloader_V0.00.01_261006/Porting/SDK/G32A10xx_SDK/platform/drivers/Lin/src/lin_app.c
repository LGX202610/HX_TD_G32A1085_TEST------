/*!
 * @file        lin_app.c
 *
 * @brief       The source file for the lin module
 *
 * @version     V1.0.0
 *
 * @date        2026-03-17
 *
 * @attention
 *
 *  Copyright (C) 2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "lin_app.h"
#include "TP_cfg.h"

/** @defgroup USART_LIN_Variables Variables
  @{
*/
extern uint8 g_linTxBuf[];

Lin_MessageType MasterSend;
Lin_MessageType MasterReceived;
boolean g_needTxMsg = FALSE;

uint8_t MasterReceivedProces = 0;
uint8_t MasterReceiveDataProces = 0;
uint8_t MasterReceivedOverFlag = 0;

Lin_MessageType SlaveSend;
Lin_MessageType SlaveReceived;

uint8_t SlaveReceivedProces = 0;
uint8_t SlaveReceivedDataProces = 0;
uint8_t SlaveReceivedOverFlag = 0;

void LIN_CallPid(void)
{

}

void LINBUSAbortTxMsg(void)
{
    
}

void LIN_SlaveTxOkCallBack(void)
{
    g_needTxMsg = FALSE;
    TP_DoTxMessageSusCallback();
}

void LIN_CallBack(void)
{
    TP_WriteDataInTransport(0x3Cu, 8u, SlaveReceived.data);
}

/** @defgroup USART_LIN_Functions Functions
  @{
*/
/*!
 * @brief       UART2 initializes functions
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Lin_Init(void)
{
    Gpio_ConfigType gpioConfig;
    Usart_ConfigType usartConfig;

    /* Enable GPIO clock */
    Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_GPIOA);

    /* Enable USART2 clock */
    Rcm_EnableApb1PeriphClock(RCM_APB1_PERIPH_USART2);

    /* Connect PXx to USART2_Tx */
    Gpio_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_PIN1);

    /* Connect PXx to USART2_Rx */
    Gpio_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_PIN1);

    /* Configure USART2 Tx as alternate function push-pull */
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.pin = GPIO_PIN_2;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_PU;
    Gpio_Config(GPIOA, &gpioConfig);

    /* Configure USART2 Rx as input floating */
    gpioConfig.pin = GPIO_PIN_3;
    Gpio_Config(GPIOA, &gpioConfig);

    /*  BaudRate is 19200 */
    usartConfig.baudRate = 19200;
    /*  Enable receiver */
    usartConfig.mode = USART_MODE_TX_RX;
    /*  Parity disable */
    usartConfig.parity = USART_PARITY_NONE;
    /*  One stop bit */
    usartConfig.stopBits = USART_STOP_BIT_1;
    /*  Word length is 8bit */
    usartConfig.wordLength = USART_WORD_LEN_8B;
    /*  Hardware flow control */
    usartConfig.hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
    /*  USART2 configuration */
    Usart_Config(USART2, &usartConfig);

    Usart_ConfigLinBreakDetectLenCfg(USART2, USART_LBDLC_11B);
    Usart_EnableLin(USART2);

    /*  Enable USART2 */
    Usart_Enable(USART2);

    /* Clear USARTx TXC Flag */
    Usart_ClearStatusFlag(USART2, USART_FLAG_TXC);
    Usart_ClearStatusFlag(USART2, USART_FLAG_TXBE);

    /* Enable USART2_Interrupt_RXBNEIE */
    Usart_EnableInterrupt(USART2, USART_INT_LBDIE);
    Usart_EnableInterrupt(USART2, USART_INT_RXBNEIE);
    Usart_EnableInterrupt(USART2, USART_INT_TXCIE);

    /*  Enable USART2 IRQ request */
    Nvic_EnableIrqRequest(USART2_IRQn, 0x01);
}

/*!
 * @brief       Transmits break characters
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @retval      None
 *
 */
void Lin_SendBreak(USART_T* usart)
{
    usart->REQUEST_R.REQUEST_B.TXBFQ = BIT_SET;
}

/*!
 * @brief       Transmits Sync Segment
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @retval      None
 *
 */
void Lin_SendSyncSegment(USART_T* usart)
{
    Usart_TxData(usart, 0x55);

    /* wait for the data to be send */
    while (Usart_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
}


/*!
 * @brief       ID check
 *
 * @param       id: frame ID
 *
 * @retval      checked id
 *
 */
uint8_t Lin_CheckPid(uint8_t id)
{
    uint8_t returnpid ;
    uint8_t P0 ;
    uint8_t P1 ;

    P0 = (((id) ^ (id >> 1) ^ (id >> 2) ^ (id >> 4)) & 0x01) << 6 ;
    P1 = ((~((id >> 1) ^ (id >> 3) ^ (id >> 4) ^ (id >> 5))) & 0x01) << 7 ;

    returnpid = id | P0 | P1 ;

    return returnpid ;
}

/*!
 * @brief       The master header frame sending function.
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @param       LinMessage: LIN Message structure @Lin_MessageType
 *
 * @retval      None
 *
 */
void LIN_SendHead(USART_T* usart, Lin_MessageType LinMessage)
{
    Lin_SendBreak(usart);
    Lin_SendSyncSegment(usart);
    Usart_TxData(usart, Lin_CheckPid(LinMessage.ID));

    /* wait for the data to be send  */
    while (Usart_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
}


/*!
 * @brief       Check data sum
 *
 * @param       id: frame ID
 *
 * @param       data: frame data
 *
 * @retval      checked sum
 *
 */
uint8_t Lin_CheckSum(uint8_t id, uint8_t* data)
{
    uint8_t t ;
    uint16_t sum ;

    if ((id == 0x3CU) || (id == 0x7DU) || (id == 0xFEU) || (id == 0xBFU))
    {
        /* Do not add PID in checksum calculation */
        sum = 0U;
    }
    else
    {
        /* Add PID in checksum calculation */
        sum = id;
    }

    sum = data[0];

    for (t = 1; t < 8; t++)
    {
        sum += data[t];

        if (sum & 0xff00)
        {
            sum &= 0x00ff;
            sum += 1;
        }
    }

#if 0
    /* In the case of diagnostic frames, the classic checksum is used. */
    if (id != 0x3C)
    {
        sum += Lin_CheckPid(id);

        if (sum & 0xff00)
        {
            sum &= 0x00ff;
            sum += 1;
        }
    }
#endif

    sum = ~sum;
    return (uint8_t)sum ;
}

/*!
 * @brief       Transmits data
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @param       data: frame data
 *
 * @retval      None
 *
 */
void Lin_TxData(USART_T* usart, uint8_t* data)
{
    uint8_t t, txdata ;

    for (t = 0; t < 8; t++)
    {
        txdata = data[t];

        Usart_TxData(usart, txdata);

        /* wait for the data to be send */
        while (Usart_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
    }
}


/*!
 * @brief       Transmits Answer
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @param       LinMessage: LIN Message strucstruct @Lin_MessageType
 *
 * @retval      None
 *
 */
void Lin_SendAnswer(USART_T* usart, Lin_MessageType LinMessage)
{
    Lin_TxData(usart, LinMessage.data);
    Usart_TxData(usart, Lin_CheckSum(LinMessage.ID, LinMessage.data));

    /* wait for the data to be send */
    while (Usart_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
}

/*!
 * @brief       Master interrupt server functions
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Lin_MasterISR(void)
{
    uint8_t ReceiveData;

    if (Usart_ReadIntFlag(USART2, USART_INT_FLAG_RXBNE) == SET)
    {
        ReceiveData = Usart_RxData(USART2);

        MasterReceivedOverFlag = 0 ;

        if (MasterReceivedProces == 0)
        {
            if (MasterReceiveDataProces < 8)
            {
                MasterReceived.data[MasterReceiveDataProces] = ReceiveData ;
                MasterReceiveDataProces += 1 ;

                if (MasterReceiveDataProces == 8)
                {
                    MasterReceiveDataProces = 0 ;
                    MasterReceivedProces = 1 ;
                }
            }
        }
        else
        {
            MasterReceived.CheckSum = ReceiveData;
            MasterReceivedProces = 0 ;
            MasterReceivedOverFlag = 1 ;
        }
    }
}

/*!
 * @brief       Slave interrupt server functions
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Lin_SlaveISR(void)
{
    uint8_t ReceiveData;
    uint8_t ReceiveID;

    if (Usart_ReadIntFlag(USART2, USART_INT_FLAG_LBD) == SET)
    {
        /* Re-sync parser at each LIN break to avoid stale receive state. */
        SlaveReceivedProces = 0u;
        SlaveReceivedDataProces = 0u;
        Usart_ClearIntFlag(USART2, USART_INT_FLAG_LBD);
    }
    else if (Usart_ReadIntFlag(USART2, USART_INT_FLAG_RXBNE) == SET)
    {
        ReceiveData = Usart_RxData(USART2);

        SlaveReceivedOverFlag = 0;

        if (SlaveReceivedProces == 0)       /*!< step1: sync */
        {
            /* Sync frame processing */
            if (ReceiveData == 0x55)
            {
                SlaveReceivedProces = 1 ;
            }
        }
        else if (SlaveReceivedProces == 1)  /*!< step2: id */
        {
            SlaveReceived.ID = ReceiveData ;
            ReceiveID = SlaveReceived.ID & 0x3f ;

            /* The master reads the slave data */
            if (ReceiveID == 0x3D)
            {
                if(TRUE == g_needTxMsg)
                {
                    Lin_TxData(USART2, g_linTxBuf);
                    Usart_TxData(USART2, Lin_CheckSum(ReceiveID, g_linTxBuf));
                }

                SlaveReceivedProces = 0;
            }
            else
            {
                SlaveReceivedProces = 2 ;
            }
        }
        else if (SlaveReceivedProces == 2) /*!< step3: data */
        {
            /* data frame processing */
            if (SlaveReceivedDataProces < 8)
            {
                SlaveReceived.data[SlaveReceivedDataProces] = ReceiveData ;
                SlaveReceivedDataProces += 1 ;

                if (SlaveReceivedDataProces == 8)
                {
                    SlaveReceivedDataProces = 0 ;
                    SlaveReceivedProces = 3 ;
                    return ;
                }
            }
        }
        else if (SlaveReceivedProces == 3) /*!< step4: checksum */
        {
            /* Check frame processing */
            SlaveReceived.CheckSum = ReceiveData ;
            
            if(SlaveReceived.CheckSum == Lin_CheckSum(SlaveReceived.ID, SlaveReceived.data))
            {
                LIN_CallBack();
            }

            SlaveReceivedProces = 0 ;
            SlaveReceivedOverFlag = 1 ;
        }
    
        Usart_ClearIntFlag(USART2, USART_INT_FLAG_RXBNE);
    }
    else if (Usart_ReadIntFlag(USART2, USART_INT_FLAG_TXC) == SET)
    {
        LIN_SlaveTxOkCallBack();
        Usart_ClearStatusFlag(USART2, USART_FLAG_TXC);
    }
    else if (Usart_ReadStatusFlag(USART2, USART_FLAG_OVRE) == SET)
    {
        Usart_ClearStatusFlag(USART2, USART_FLAG_OVRE);
    }
}

/**@} end of group USART_Interrupt_Functions */
/**@} end of group USART_Interrupt */
/**@} end of group Examples */
