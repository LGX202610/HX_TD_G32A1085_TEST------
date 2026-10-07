/*!
 * @file        g32a10xx_can.c
 *
 * @brief       This file provides firmware functions to manage the following
 *              functionalities of the can peripheral
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

#include "g32a10xx_can.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup CAN_Driver
  @{
*/

/** @defgroup CAN_Functions Functions
  @{
*/


/*******************************************************************************
                            GLOBAL VARIABLES
*******************************************************************************/

/* The Can base address array */
static CAN_T *const s_canBaseAddrs[CAN_MODULE_NUM] = {CAN};
/* The Can handle array */
Can_HandleType *s_canHandleArr[CAN_MODULE_NUM] = {0U};

/* The dlc region in the frame to the data bytes number mapping table */
uint8_t g_CanGblFdDlcConvDb[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

/*******************************************************************************
                        LOCAL FUNCTIONS
*******************************************************************************/
/*!
 * @brief Get the can instance from peripheral base address.
 *
 * @param ModulePtr The pointer to the module.
 * @return can instance.
 */
static uint32_t Can_GetModuleId(const CAN_T *ModulePtr)
{
    uint32_t instance = 0U;

    /* Convert the base pointer/address to an instance number using the mapping table. */
    for (instance = 0U; instance < CAN_MODULE_NUM; instance++)
    {
        if (s_canBaseAddrs[instance] == ModulePtr)
        {
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    return instance;
}

/*!
 * @brief Reset the can module
 *
 * @param ModulePtr  The The pointer to the module
 */
static void Can_ResetModule(CAN_T *ModulePtr)
{
    ModulePtr->CCCR_R.CCCR_B.INIT = BIT_SET;

    /* Wait for the value to be committed. */
    while (0U == ModulePtr->CCCR_R.CCCR_B.INIT)
    {
    }

    /* Unlock protected configuration space by asserting CCE, 
    and clear specific status registers afterward */
    ModulePtr->CCCR_R.CCCR_B.CCE = BIT_SET;
}

/*!
 * @brief Retrieve the address of the element to be read from Receive FIFO 0
 *
 * @param ModulePtr  The The pointer to the module.
 * @return The address of the element in fifo 0.
 */
static uint32_t Can_GetRxFifo0EleAddr(const CAN_T *ModulePtr)
{
    uint32_t eleAddr = 0U;
    uint32_t eleSize = 0U;

    eleSize = ModulePtr->RXESC_R.RXESC_B.F0DS;

    if (eleSize < 5U)
    {
        eleSize += 4U;
    }
    else
    {
        eleSize = ((eleSize * 4U) - 10U);
    }

    eleAddr = ((uint32_t)ModulePtr->RXF0C_R.RXF0C_B.F0SA << MSG_RAM_ELE_ADDR_SHIFT);
    eleAddr += (ModulePtr->RXF0S_R.RXF0S_B.F0GI) * eleSize * 4U;

    return eleAddr;
}

/*!
 * @brief Retrieve the address of the element to be read from Receive FIFO 1
 *
 * @param ModulePtr  The The pointer to the module
 * @return The address of the element in fifo 1
 */
static uint32_t Can_GetRxFifo1EleAddr(const CAN_T *ModulePtr)
{
    uint32_t eleAddr = 0U;
    uint32_t eleSize = 0U;

    eleSize = ModulePtr->RXESC_R.RXESC_B.F1DS;;

    if (eleSize < 5U)
    {
        eleSize += 4U;
    }
    else
    {
        eleSize = ((eleSize * 4U) - 10U);
    }

    eleAddr = ((uint32_t)ModulePtr->RXF1C_R.RXF1C_B.F1SA << MSG_RAM_ELE_ADDR_SHIFT);
    eleAddr += (ModulePtr->RXF1S_R.RXF1S_B.F1GI) * eleSize * 4U;

    return eleAddr;
}

/*!
 * @brief Retrieve the address of the element to be read from the receive buffer
 *
 * @param ModulePtr  The The pointer to the module
 * @param BufId      The buffer element id
 * @return The address of the element in receive buffer
 */
static uint32_t Can_GetRxBufEleAddr(const CAN_T *ModulePtr, uint8_t BufId)
{
    uint32_t eleSize = 0U;
    uint32_t retVal = 0U;

    if(BufId > 63U)
    {
        retVal = 0U;
    }
    else
    {
        eleSize = ModulePtr->RXESC_R.RXESC_B.RBDS;

        if (eleSize < 5U)
        {
            eleSize += 4U;
        }
        else
        {
            eleSize = ((eleSize * 4U) - 10U);
        }
        
        retVal = (uint32_t)(((uint32_t)(ModulePtr->RXBC_R.RXBC_B.RBSA) << MSG_RAM_ELE_ADDR_SHIFT) + (BufId * eleSize * 4U));
    }

    return retVal;
}

/*!
 * @brief Retrieve the address of the element to be read from the transmit buffer
 *
 * @param ModulePtr  The The pointer to the module
 * @param BufId      The buffer element id
 * @return Address of the element in transmit buffer.
 */
static uint32_t Can_GetTxBufEleAddr(const CAN_T *ModulePtr, uint8_t BufId)
{
    uint32_t eleSize = 0U;
    uint32_t retVal = 0U;

    if(BufId > 31U)
    {
        retVal = 0U;
    }
    else
    {
        eleSize = ModulePtr->TXESC_R.TXESC_B.TBDS;

        if (eleSize < 5U)
        {
            eleSize += 4U;
        }
        else
        {
            eleSize = ((eleSize * 4U) - 10U);
        }
        
        retVal = (uint32_t)(((uint32_t)(ModulePtr->TXBC_R.TXBC_B.TBSA) << MSG_RAM_ELE_ADDR_SHIFT) + ((uint32_t)BufId * eleSize * 4U));
    }

    return retVal;
}

/*!
 * @brief Can handle the Rx dedicate buffer interrupt
 *
 * @param ModulePtr  The pointer to the module
 * @param HandlePtr  The pointer to the handle
 */
static void Can_HandleDedBufRxInt(CAN_T *ModulePtr, Can_HandleType *HandlePtr)
{
    Can_TransferStsType retVal = CAN_NO_HANDLE_STS;
    uint32_t resVal = 0U;
    uint8_t bufId = 0U;

    for (bufId = 0U; bufId < CAN_RX_DEDICATE_BUF_NUM; bufId++)
    {
        if (0U != Can_ReadRxBufNewDataFlg(ModulePtr, (Can_RxBufIdType)bufId))
        {
            if (ModulePtr->IE_R.IE_B.DRXE != 0U)
            {
                Can_ReadRxBuffer(ModulePtr, (Can_RxBufIdType)bufId, HandlePtr->rxBufFrame[bufId]);
                Can_FinishReceiveBuf(ModulePtr, HandlePtr, (Can_RxBufIdType)bufId);
                /* Record the Rx buffer received frame */
                resVal = bufId;
                retVal = CAN_RX_IDLE;
                if (HandlePtr->callback != NULL)
                {
                    HandlePtr->callback(ModulePtr, HandlePtr, retVal, resVal, HandlePtr->InputParaPtr);
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* do nothing */
        }
    }
}

/*******************************************************************************
                            GLOBAL FUNCTIONS
*******************************************************************************/
/*!
 * @brief Initialize the can module
 *
 * This function initializes the can module with user-defined configurations.
 * @param ModulePtr  The pointer to the module
 * @param CanConfigPtr The pointer to the user-defined can configuration
 */
void Can_Init(CAN_T *ModulePtr, const Can_ConfigType *CanConfigPtr)
{
    const Can_BaudrateConfigType *timConfigPtr = &(CanConfigPtr->baudrateConfig);

    Can_ResetModule(ModulePtr);

    if ((uint8_t)ENABLE == CanConfigPtr->loopBackInterEn)
    {
        ModulePtr->CCCR_R.CCCR_B.TEST = BIT_SET;
        ModulePtr->CCCR_R.CCCR_B.MON = BIT_SET;
        ModulePtr->TEST_R.TEST_B.LBCK = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if ((uint8_t)ENABLE == CanConfigPtr->loopBackExtEn)
    {
        ModulePtr->CCCR_R.CCCR_B.TEST = BIT_SET;
        ModulePtr->TEST_R.TEST_B.LBCK = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if ((uint8_t)ENABLE == CanConfigPtr->busMonEn)
    {
        ModulePtr->CCCR_R.CCCR_B.MON = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    /* Update the timing parameters to configure the baud rate for the arbitration phase */
    Can_SetArbTimConfig(ModulePtr, timConfigPtr);

    if ((uint8_t)ENABLE == CanConfigPtr->canfdNorEn)
    {
        ModulePtr->CCCR_R.CCCR_B.FDOE = BIT_SET;
    }
    else
    {
        /* do nothing */
    }

    if ((uint8_t)ENABLE == CanConfigPtr->canfdBrsEn)
    {
        /* Enable the Bit Rate Switch and CAN FD mode */
        ModulePtr->CCCR_R.CCCR_B.FDOE = BIT_SET;
        ModulePtr->CCCR_R.CCCR_B.BRSE = BIT_SET;

        /* Update the timing parameters to configure the baud rate for the data phase */
        Can_SetDataTimConfig(ModulePtr, timConfigPtr);

        if (((uint8_t)FALSE == CanConfigPtr->loopBackInterEn) && ((uint8_t)FALSE == CanConfigPtr->loopBackExtEn))
        {
            /* Enable the Transceiver Delay Compensation */
            ModulePtr->DBTP_R.DBTP_B.TDC = BIT_SET;
            /* Reset the TDCO */
            ModulePtr->TDCR_R.TDCR_B.TDCO = BIT_RESET;

            /* Set the TDC offset as: (DTSEG1 + 2) * (DBRP + 1) */
            if ((((uint32_t)timConfigPtr->dataPhaseSeg1 + 2U) * 
                ((uint32_t)timConfigPtr->dataClkPsc + 1U)) < TDCOFF_MAX_VAL)
            {
                ModulePtr->TDCR_R.TDCR_B.TDCO = ((uint32_t)timConfigPtr->dataPhaseSeg1 + 2U) * 
                    ((uint32_t)timConfigPtr->dataClkPsc + 1U);
            }
            else
            {
                /* Configure the TDC offset to the maximum value */
                ModulePtr->TDCR_R.TDCR_B.TDCO = TDCOFF_MAX_VAL;
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief Deinitialize the can module
 *
 * This function deinitializes the can module
 *
 * @param ModulePtr  The pointer to the module
 */
void Can_Deinit(CAN_T *ModulePtr)
{
    /* Reset all the register */
    Can_ResetModule(ModulePtr);
}

/*!
 * @brief Configure the Can protocol data phase timing
 *
 * @param ModulePtr  The pointer to the module
 * @param TimConfigPtr The pointer to the Can timing configuration
 */
void Can_SetDataTimConfig(CAN_T *ModulePtr, const Can_BaudrateConfigType *TimConfigPtr)
{
    if((NULL == TimConfigPtr) || (NULL == ModulePtr))
    {
        /* do nothing */
    }
    else
    {
        /* Reset the timing configuration */
        ModulePtr->DBTP_R.DBTP_B.DSJW = BIT_RESET;
        ModulePtr->DBTP_R.DBTP_B.DTSEG2 = BIT_RESET;
        ModulePtr->DBTP_R.DBTP_B.DTSEG1 = BIT_RESET;
        ModulePtr->DBTP_R.DBTP_B.DBRP = BIT_RESET;
        
        /* Refresh the timing configuration */
        ModulePtr->DBTP_R.DBTP_B.DSJW = TimConfigPtr->dataResyncJumpWidth;
        ModulePtr->DBTP_R.DBTP_B.DTSEG2 = TimConfigPtr->dataPhaseSeg2;
        ModulePtr->DBTP_R.DBTP_B.DTSEG1 = TimConfigPtr->dataPhaseSeg1;
        ModulePtr->DBTP_R.DBTP_B.DBRP = TimConfigPtr->dataClkPsc;
    }
}

/*!
 * @brief Configures the Can protocol timing characteristics for the arbitration phase
 *
 * @param ModulePtr  The pointer to the module
 * @param TimConfigPtr Pointer to the timing configuration structure
 */
void Can_SetArbTimConfig(CAN_T *ModulePtr, const Can_BaudrateConfigType *TimConfigPtr)
{
    if(NULL == TimConfigPtr)
    {
        /* do nothing */
    }
    else
    {
        /* Reset the Timing configuration */
        ModulePtr->NBTP_R.NBTP = 0U;
        
        /* Refresh Timing configuration */
        ModulePtr->NBTP_R.NBTP_B.NTSEG2 = TimConfigPtr->phaseSeg2;
        ModulePtr->NBTP_R.NBTP_B.NTSEG1 = TimConfigPtr->phaseSeg1;
        ModulePtr->NBTP_R.NBTP_B.NBRP = TimConfigPtr->clkPsc;
        ModulePtr->NBTP_R.NBTP_B.NSJW = TimConfigPtr->resyncJumpWidth;
    }
}

/*!
 * @brief Configure the Can filter
 *
 * This function enables global filtering for remote and 
 * non-matching frames, and programs the start address and 
 * list size for both standard and extended ID filter configurations
 *
 * @param ModulePtr  The pointer to the module
 * @param FilterConfigPtr The pointer to the Can filter configuration.
 */
void Can_SetFilterConfig(CAN_T *ModulePtr, const Can_FilterConfigType *FilterConfigPtr)
{
    /* Set global configuration of remote/nonmasking frames, set filter address and list size. */
    if (FilterConfigPtr->idFormat == CAN_FRAME_STD_ID)
    {
        ModulePtr->GFC_R.GFC_B.RRFS = (uint8_t)FilterConfigPtr->remFrame;
        ModulePtr->GFC_R.GFC_B.ANFS = (uint8_t)FilterConfigPtr->nmFrame;

        ModulePtr->SIDFC_R.SIDFC_B.FLSSA = (FilterConfigPtr->address >> MSG_RAM_ELE_ADDR_SHIFT);
        ModulePtr->SIDFC_R.SIDFC_B.LSS = FilterConfigPtr->listSize;
    }
    else
    {
        ModulePtr->GFC_R.GFC_B.RRFE = (uint8_t)FilterConfigPtr->remFrame;
        ModulePtr->GFC_R.GFC_B.ANFE = (uint8_t)FilterConfigPtr->nmFrame;

        ModulePtr->XIDFC_R.XIDFC_B.FLESA = (FilterConfigPtr->address >> MSG_RAM_ELE_ADDR_SHIFT);
        ModulePtr->XIDFC_R.XIDFC_B.LSE = FilterConfigPtr->listSize;
    }
}

/*!
 * @brief Configure the Can Rx fifo 0
 *
 * This function programs Rx FIFO 0 parameters, including start address, element size, 
 * watermark, operation mode, and data field size
 *
 * @param ModulePtr  The pointer to the module
 * @param RxFifoConfigPtr The pointer to the fifo 0 configuration
 */
void Can_SetRxFifo0Config(CAN_T *ModulePtr, const Can_RxFifoConfigType *RxFifoConfigPtr)
{
    ModulePtr->RXF0C_R.RXF0C_B.F0SA = (RxFifoConfigPtr->address >> MSG_RAM_ELE_ADDR_SHIFT);
    ModulePtr->RXF0C_R.RXF0C_B.F0S = RxFifoConfigPtr->elementSize;
    ModulePtr->RXF0C_R.RXF0C_B.F0WM = RxFifoConfigPtr->watermark;
    ModulePtr->RXF0C_R.RXF0C_B.F0OM = (uint8_t)RxFifoConfigPtr->opmode;

    ModulePtr->RXESC_R.RXESC_B.F0DS = RxFifoConfigPtr->datafieldSize;
}

/*!
 * @brief Configure the Can Rx fifo 1
 *
 * This function programs Rx FIFO 1 parameters, including start address, element size, 
 * watermark, operation mode, and data field size
 *
 * @param ModulePtr  The pointer to the module
 * @param RxFifoConfigPtr The pointer to the fifo 1 configuration
 */
void Can_SetRxFifo1Config(CAN_T *ModulePtr, const Can_RxFifoConfigType *RxFifoConfigPtr)
{
    ModulePtr->RXF1C_R.RXF1C_B.F1SA = (RxFifoConfigPtr->address >> MSG_RAM_ELE_ADDR_SHIFT);
    ModulePtr->RXF1C_R.RXF1C_B.F1S = RxFifoConfigPtr->elementSize;
    ModulePtr->RXF1C_R.RXF1C_B.F1WM = RxFifoConfigPtr->watermark;
    ModulePtr->RXF1C_R.RXF1C_B.F1OM = (uint8_t)RxFifoConfigPtr->opmode;

    ModulePtr->RXESC_R.RXESC_B.F1DS = RxFifoConfigPtr->datafieldSize;
}

/*!
 * @brief Configure the Can Rx buffer
 *
 * This function programs the receive buffer parameters, including start address and data field size
 *
 * @param ModulePtr  The pointer to the module
 * @param RxBufConfigPtr The pointer to the Rx buffer configuration
 */
void Can_SetRxBufferConfig(CAN_T *ModulePtr, const Can_RxBufConfigType *RxBufConfigPtr)
{
    /* Set the Rx Buffer start address */
    ModulePtr->RXBC_R.RXBC_B.RBSA = (RxBufConfigPtr->address >> MSG_RAM_ELE_ADDR_SHIFT);

    /* Set the Rx Buffer data field size */
    ModulePtr->RXESC_R.RXESC_B.RBDS = RxBufConfigPtr->datafieldSize;
}

/*!
 * @brief Configure the Can Tx event fifo
 *
 * This function programs the transmit event FIFO parameters, including start address, element size, and watermark
 *
 * @param ModulePtr  The pointer to the module
 * @param TxEvtFifoConfigPtr The pointer to the Tx Event FIFO configuration
 */
void Can_SetTxEvtFifoConfig(CAN_T *ModulePtr, const Can_TxFifoConfigType *TxEvtFifoConfigPtr)
{
    ModulePtr->TXEFC_R.TXEFC_B.EFSA = (TxEvtFifoConfigPtr->address >> MSG_RAM_ELE_ADDR_SHIFT);
    ModulePtr->TXEFC_R.TXEFC_B.EFS = TxEvtFifoConfigPtr->elementSize;
    ModulePtr->TXEFC_R.TXEFC_B.EFWM = TxEvtFifoConfigPtr->watermark;
}

/*!
 * @brief Configure the Can Tx buffer.
 *
 * This function programs the Tx buffer parameters, including start address, 
 * element size, FIFO/queue mode, and data-field size
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufConfigPtr The pointer to the Tx Buffer configuration
 */
void Can_SetTxBufConfig(CAN_T *ModulePtr, const Can_TxBufConfigType *TxBufConfigPtr)
{
    if((TxBufConfigPtr->dedicatedSize + TxBufConfigPtr->fifoQueCnt) > 32U)
    {
        /* do nothing */
    }
    else
    {
        ModulePtr->TXBC_R.TXBC_B.TBSA = (TxBufConfigPtr->address >> MSG_RAM_ELE_ADDR_SHIFT);
        ModulePtr->TXBC_R.TXBC_B.NDTB = TxBufConfigPtr->dedicatedSize;
        ModulePtr->TXBC_R.TXBC_B.TFQS = TxBufConfigPtr->fifoQueCnt;
        ModulePtr->TXBC_R.TXBC_B.TFQM = (uint8_t)TxBufConfigPtr->mode;
        
        ModulePtr->TXESC_R.TXESC_B.TBDS = TxBufConfigPtr->datafieldSize;
    }
}

/*!
 * @brief Configure the Message RAM
 *
 * This function configures standard/extended ID filtering, 
 * Rx FIFO 0/1, Rx buffer settings, Tx event FIFO, and Tx buffer settings
 * 
 * @param ModulePtr  The pointer to the module
 * @param MsgRamConfigPtr The pointer to the Tx Buffer configuration
 * 
 * @retval CAN_API_SUCCESS: Message RAM configuration Successfully
 * @retval CAN_API_FAIL: Failed to configure Message RAM because the address parameter is invalid
 */
Can_ApiRetStsType Can_SetMsgRamConfig(CAN_T *ModulePtr, const Can_MessageRamConfig *MsgRamConfigPtr)
{
    uint32_t addrMid   = 0U;
    uint32_t eleSize  = 0U;
    Can_ApiRetStsType resVal = CAN_API_SUCCESS;

    Can_EnterInitMode(ModulePtr);

    /* Configure the standard fiter */
    if (MsgRamConfigPtr->stdFilterMsgRamCfgPtr != NULL)
    {
        if ((MsgRamConfigPtr->stdFilterMsgRamCfgPtr->idFormat == CAN_FRAME_STD_ID) && 
            (0U == (MsgRamConfigPtr->stdFilterMsgRamCfgPtr->address % 4U)))
        {
            addrMid = MsgRamConfigPtr->stdFilterMsgRamCfgPtr->listSize * 4U;
            Can_SetFilterConfig(ModulePtr, MsgRamConfigPtr->stdFilterMsgRamCfgPtr);
        }
        else
        {
            resVal = CAN_API_FAIL;
        }
    }
    else
    {

    }
    
    /* Configure the EXT fiter */
    if ((MsgRamConfigPtr->extFilterMsgRamCfgPtr != NULL) && (CAN_API_SUCCESS == resVal))
    {
        if ((MsgRamConfigPtr->extFilterMsgRamCfgPtr->idFormat == CAN_FRAME_EXT_ID) && 
            (MsgRamConfigPtr->extFilterMsgRamCfgPtr->address >= addrMid) &&
            (0U == (MsgRamConfigPtr->extFilterMsgRamCfgPtr->address % 4U)))
        {
            addrMid = (MsgRamConfigPtr->extFilterMsgRamCfgPtr->address) + ((MsgRamConfigPtr->extFilterMsgRamCfgPtr->listSize) * 8U);
            Can_SetFilterConfig(ModulePtr, MsgRamConfigPtr->extFilterMsgRamCfgPtr);
        }
        else
        {
            resVal = CAN_API_FAIL;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Configure the Rx Fifo 0 */
    if ((MsgRamConfigPtr->rxFifo0MsgRamCfgPtr != NULL) && (CAN_API_SUCCESS == resVal))
    {
        eleSize = ((uint32_t)MsgRamConfigPtr->rxFifo0MsgRamCfgPtr->datafieldSize < 5U) ?
                    ((uint32_t)MsgRamConfigPtr->rxFifo0MsgRamCfgPtr->datafieldSize + 4U) :
                    (((uint32_t)MsgRamConfigPtr->rxFifo0MsgRamCfgPtr->datafieldSize * 4U) - 10U);

        if ((MsgRamConfigPtr->rxFifo0MsgRamCfgPtr->address >= addrMid) && (0U == (MsgRamConfigPtr->rxFifo0MsgRamCfgPtr->address % 4U)))
        {
            addrMid = (MsgRamConfigPtr->rxFifo0MsgRamCfgPtr->address) + ((MsgRamConfigPtr->rxFifo0MsgRamCfgPtr->elementSize) * eleSize * 4U);
            Can_SetRxFifo0Config(ModulePtr, MsgRamConfigPtr->rxFifo0MsgRamCfgPtr);
        }
        else
        {
            resVal = CAN_API_FAIL;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Configure the Rx Fifo 1 */
    if ((MsgRamConfigPtr->rxFifo1MsgRamCfgPtr != NULL) && (CAN_API_SUCCESS == resVal))
    {
        eleSize = ((uint32_t)MsgRamConfigPtr->rxFifo1MsgRamCfgPtr->datafieldSize < 5U) ?
                    ((uint32_t)MsgRamConfigPtr->rxFifo1MsgRamCfgPtr->datafieldSize + 4U) :
                    (((uint32_t)MsgRamConfigPtr->rxFifo1MsgRamCfgPtr->datafieldSize * 4U) - 10U);

        if ((MsgRamConfigPtr->rxFifo1MsgRamCfgPtr->address >= addrMid) && (0U == (MsgRamConfigPtr->rxFifo1MsgRamCfgPtr->address % 4U)))
        {
            addrMid = MsgRamConfigPtr->rxFifo1MsgRamCfgPtr->address + (MsgRamConfigPtr->rxFifo1MsgRamCfgPtr->elementSize * eleSize * 4U);
            Can_SetRxFifo1Config(ModulePtr, MsgRamConfigPtr->rxFifo1MsgRamCfgPtr);
        }
        else
        {
            resVal = CAN_API_FAIL;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Configure the Rx buffer */
    if ((MsgRamConfigPtr->rxDeBufMsgRamCfgPtr != NULL) && (CAN_API_SUCCESS == resVal))
    {
        eleSize = ((uint32_t)MsgRamConfigPtr->rxDeBufMsgRamCfgPtr->datafieldSize < 5U) ?
                    ((uint32_t)MsgRamConfigPtr->rxDeBufMsgRamCfgPtr->datafieldSize + 4U) :
                    ((((uint32_t)MsgRamConfigPtr->rxDeBufMsgRamCfgPtr->datafieldSize) * 4U) - 10U);

        if ((MsgRamConfigPtr->rxDeBufMsgRamCfgPtr->address >= addrMid) && (0U == (MsgRamConfigPtr->rxDeBufMsgRamCfgPtr->address % 4U)))
        {
            addrMid = (MsgRamConfigPtr->rxDeBufMsgRamCfgPtr->address) + (64U * eleSize * 4U);
            Can_SetRxBufferConfig(ModulePtr, MsgRamConfigPtr->rxDeBufMsgRamCfgPtr);
        }
        else
        {
            resVal = CAN_API_FAIL;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Configure the Tx Fifo */
    if ((MsgRamConfigPtr->txFifoMsgRamCfgPtr != NULL) && (CAN_API_SUCCESS == resVal))
    {
        if ((MsgRamConfigPtr->txFifoMsgRamCfgPtr->address >= addrMid) && (0U == (MsgRamConfigPtr->txFifoMsgRamCfgPtr->address % 4U)))
        {
            addrMid = (MsgRamConfigPtr->txFifoMsgRamCfgPtr->address) + (MsgRamConfigPtr->txFifoMsgRamCfgPtr->elementSize * 8U);
            Can_SetTxEvtFifoConfig(ModulePtr, MsgRamConfigPtr->txFifoMsgRamCfgPtr);
        }
        else
        {
            resVal = CAN_API_FAIL;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Configure the Tx buffer */
    if ((MsgRamConfigPtr->txDeBufMsgRamCfgPtr != NULL) && (CAN_API_SUCCESS == resVal))
    {
        if ((MsgRamConfigPtr->txDeBufMsgRamCfgPtr->address < addrMid) || (0U != (MsgRamConfigPtr->txDeBufMsgRamCfgPtr->address % 4U)))
        {
            resVal = CAN_API_FAIL;
        }
        else
        {
            Can_SetTxBufConfig(ModulePtr, MsgRamConfigPtr->txDeBufMsgRamCfgPtr);
        }
    }
    else
    {
        /* do nothing */
    }

    Can_EnterNorMode(ModulePtr);

    return resVal;
}

/*!
 * @brief This function configures the standard message ID filter element
 *
 * @param FilterConfigPtr The pointer to the Can filter configuration
 * @param StdFilterEleConfigPtr The pointer to the Can filter element configuration
 * @param FilterId The element index of the standard message ID filter 
 */
void Can_SetStdFilterEle(const Can_FilterConfigType *FilterConfigPtr,
                              const Can_StdFilterEleConfigType *StdFilterEleConfigPtr,
                              uint8_t FilterId)
{
    uint32_t *elementAddress = NULL;
    uint32_t filterIdU32 = (uint32_t)FilterId;

    elementAddress  = (uint32_t *)(CAN_SRAM_BASE + (FilterConfigPtr->address) + (filterIdU32 * 4U));
    *elementAddress = *((const uint32_t *)((uint32_t)StdFilterEleConfigPtr));
}

/*!
 * @brief This function configures the EXT message ID filter element
 *
 * @param FilterConfigPtr The pointer to the Can filter configuration
 * @param ExtFilterEleConfigPtr The pointer to the Can filter element configuration
 * @param FilterId The element index of the EXT message ID filter 
 */
void Can_SetExtFilterEle(const Can_FilterConfigType *FilterConfigPtr,
                              const Can_ExtFilterEleConfigType *ExtFilterEleConfigPtr,
                              uint8_t FilterId)
{
    uint32_t *elementAddress = NULL;
    uint32_t filterIdU32 = (uint32_t)FilterId;

    elementAddress  = (uint32_t *)((CAN_SRAM_BASE + (FilterConfigPtr->address) + (filterIdU32 * 8U)));

    *elementAddress = *((const uint32_t *)((uint32_t)ExtFilterEleConfigPtr));
    *((uint32_t *)((uint32_t)elementAddress + 4U)) = *((uint32_t *)((uint32_t)ExtFilterEleConfigPtr + 4U));
}


/*!
 * @brief This function retrieves the Tx buffer request pending flag/status
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId   The index of the Tx Buffer
 * 
 * @retval: 1: Pending status
 *          0: Not pending status 
 */
uint32_t Can_CheckTransReqPending(const CAN_T *ModulePtr, Can_TxBufferIdType TxBufId)
{
    return ((ModulePtr->TXBRP_R.TXBRP & ((uint32_t)1U << TxBufId)) >> (uint32_t)TxBufId);
}

/*!
 * @brief This function retrieves the Tx buffer transmission status
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId   The index of the Tx Buffer
 * 
 * @retval: 1: Occurred status
 *          0: Not occurred status 
 */
uint32_t Can_CheckTransOccurred(const CAN_T *ModulePtr, Can_TxBufferIdType TxBufId)
{
    return (ModulePtr->TXBTO_R.TXBTO & ((uint32_t)1U << TxBufId)) >> (uint32_t)TxBufId;
}

/*!
 * @brief This function transfers an Can message to the transmit buffer
 *
 * This function loads a CAN message to the selected transmit message buffer, 
 * requests transmission by changing the buffer state, and returns without waiting for completion
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId    The index of the Tx Buffer
 * @param TxFramePtr The pointer to the CAN message frame to be transmitted
 * 
 * @retval CAN_API_SUCCESS: Write Successfully
 * @retval CAN_API_FAIL: Write Failed
 */
Can_ApiRetStsType Can_WriteTxBuffer(const CAN_T *ModulePtr, Can_TxBufferIdType TxBufId, const Can_TxFrameType *TxFramePtr)
{
    Can_ApiRetStsType retVal = CAN_API_FAIL;
    uint32_t *eleAddr        = NULL;
    uint32_t *elePayloadAddr = NULL;
    uint8_t circleCnt = 0U;
    uint8_t sendDataLen = 0U;
    uint8_t dataBytePos = 0U;
    volatile uint8_t dataTemp[64U] = {0U};
    const uint32_t *dataPtr = NULL;

    if(NULL == TxFramePtr)
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        if (0U == Can_CheckTransReqPending(ModulePtr, TxBufId))
        {
            eleAddr        = (uint32_t *)(CAN_SRAM_BASE + Can_GetTxBufEleAddr(ModulePtr, TxBufId));
            elePayloadAddr = (uint32_t *)((uint32_t)eleAddr + 8U);
        
            /* Refresh the configuration field */
            for(circleCnt = 0U; circleCnt < 2U; circleCnt++)
            {
                eleAddr[circleCnt] = ((const uint32_t *)((uint32_t)TxFramePtr))[circleCnt];
            }
        
            sendDataLen = g_CanGblFdDlcConvDb[TxFramePtr->Can_TxFrameHead1.dlc];
            
            /* Set loop for write with Number of Data Byte */
            while(sendDataLen > dataBytePos)
            {
                /* Check byte position is with in the DLC */
                if(dataBytePos < TxFramePtr->dataSize)
                {
                    /* Store the data to the Tx Buffer */
                    /* Copy Message data to the Tx Buffer */
                    dataTemp[dataBytePos] = TxFramePtr->data[dataBytePos];
                }
                else
                {
                    /* Store with padding values to the Tx buffer */
                    dataTemp[dataBytePos] = 0U;
                }
                
                /* Set for next data byte */
                dataBytePos++;
            }
            
            dataPtr = (const uint32_t*)((uint32_t)dataTemp);

            /* Refresh the data field */
            for(circleCnt = 0U; circleCnt < g_CanGblFdDlcConvDb[TxFramePtr->Can_TxFrameHead1.dlc]; circleCnt += 4U)
            {
                *elePayloadAddr = *dataPtr;
                elePayloadAddr++;
                dataPtr++;
            }
            
                retVal = CAN_API_SUCCESS;
        }
        else
        {
            retVal = CAN_API_FAIL;
        }
    }

    return retVal;
}

/*!
 * @brief This function reads a message from the Rx buffer
 *
 * @param ModulePtr  The pointer to the module
 * @param RxBufId    The index of the Rx Buffer
 * @param RxFramePtr The pointer to reception of the Can message frame
 * 
 * @retval CAN_API_SUCCESS: Read Successfully
 * @retval CAN_API_FAIL: Read Failed
 */
Can_ApiRetStsType Can_ReadRxBuffer(const CAN_T *ModulePtr, Can_RxBufIdType RxBufId, Can_RxFrameType *RxFramePtr)
{
    const uint32_t *eleAddr = NULL;
    uint8_t circleCnt = 0U;
    Can_ApiRetStsType retVal = CAN_API_FAIL;

    if(NULL == RxFramePtr)
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        eleAddr = (uint32_t *)(CAN_SRAM_BASE + 
        Can_GetRxBufEleAddr(ModulePtr, RxBufId));

        /* Refresh the configuration field */
        for(circleCnt = 0U; circleCnt < 2U; circleCnt++)
        {
            ((uint32_t *)((uint32_t)RxFramePtr))[circleCnt] = eleAddr[circleCnt];
        }
        /* Refresh the data field */
        for(circleCnt = 0U; circleCnt < g_CanGblFdDlcConvDb[RxFramePtr->Can_RxFrameHead1.dlc]; circleCnt++)
        {
            RxFramePtr->data[circleCnt] = ((uint8_t *)((uint32_t)eleAddr + 8U))[circleCnt];
        }
        
        retVal = CAN_API_SUCCESS;
    }

    return retVal;
}

/*!
 * @brief This function reads a message from the Rx Fifo
 *
 * @param ModulePtr  The pointer to the module
 * @param FifoBlkId  Rx FIFO 0 or 1
 * @param RxFramePtr The pointer to reception of the Can message frame
 * 
 * @retval CAN_API_SUCCESS: Read Successfully
 * @retval CAN_API_FAIL: Read Failed
 */
Can_ApiRetStsType Can_ReadRxFifo(CAN_T *ModulePtr, Can_RxFifoType FifoBlkId, Can_RxFrameType *RxFramePtr)
{
    const uint32_t *eleAddr = NULL;
    uint8_t circleCnt = 0U;
    Can_ApiRetStsType retVal = CAN_API_FAIL;

    if(((CAN_RX_FIFO0 != FifoBlkId) && (CAN_RX_FIFO1 != FifoBlkId)) || (NULL == RxFramePtr))
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        if(CAN_RX_FIFO0 == FifoBlkId)
        {
            if (ModulePtr->RXF0S_R.RXF0S_B.F0FL != 0U)
            {
                eleAddr = (uint32_t *)(CAN_SRAM_BASE + Can_GetRxFifo0EleAddr(ModulePtr));
                /* Refresh the configuration field */
                for(circleCnt = 0U; circleCnt < 2U; circleCnt++)
                {
                    ((uint32_t *)((uint32_t)RxFramePtr))[circleCnt] = eleAddr[circleCnt];
                }
                /* Refresh the data field */
                for(circleCnt = 0U; circleCnt < g_CanGblFdDlcConvDb[RxFramePtr->Can_RxFrameHead1.dlc]; circleCnt++)
                {
                    RxFramePtr->data[circleCnt] = ((uint8_t *)((uint32_t)eleAddr + 8U))[circleCnt];
                }

                /* Acknowledge the fifo */
                ModulePtr->RXF0A_R.RXF0A = ModulePtr->RXF0S_R.RXF0S_B.F0GI;
                
                retVal = CAN_API_SUCCESS;
            }
            else
            {
                retVal = CAN_API_FAIL;
            }
        }
        else
        {
            if(ModulePtr->RXF1S_R.RXF1S_B.F1FL != 0U)
            {
                eleAddr = (uint32_t *)(CAN_SRAM_BASE + Can_GetRxFifo1EleAddr(ModulePtr));
                /* Refresh the configuration field */
                for(circleCnt = 0U; circleCnt < 2U; circleCnt++)
                {
                    ((uint32_t *)((uint32_t)RxFramePtr))[circleCnt] = eleAddr[circleCnt];
                }
                /* Refresh the data field */
                for(circleCnt = 0U; circleCnt < g_CanGblFdDlcConvDb[RxFramePtr->Can_RxFrameHead1.dlc]; circleCnt++)
                {
                    RxFramePtr->data[circleCnt] = ((uint8_t *)((uint32_t)eleAddr + 8U))[circleCnt];
                }

                /* Acknowledge the fifo */
                ModulePtr->RXF1A_R.RXF1A = ModulePtr->RXF1S_R.RXF1S_B.F1GI;
                
                retVal = CAN_API_SUCCESS;
            }
            else
            {
                retVal = CAN_API_FAIL;
            }
        }
    }
    
    return retVal;
}

/*!
 * @brief This funciton sends a Can frame using polling mode
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId    The index of the Tx Buffer
 * @param TxFramePtr The pointer to the CAN message frame to be transmitted
 * 
 * @retval CAN_API_SUCCESS:Send Message Successfully
 * @retval CAN_API_FAIL:The Tx Message Buffer is busy
 */
Can_ApiRetStsType Can_SendBlocking(CAN_T *ModulePtr, Can_TxBufferIdType TxBufId, const Can_TxFrameType *TxFramePtr)
{
    Can_ApiRetStsType retVal = CAN_API_FAIL;

    if(CAN_API_SUCCESS == Can_WriteTxBuffer(ModulePtr, TxBufId, TxFramePtr))
    {
        Can_TxAddReq(ModulePtr, TxBufId);

        /* Block until message has been sent */
        while (0U == Can_CheckTransOccurred(ModulePtr, TxBufId))
        {
        }

        retVal = CAN_API_SUCCESS;
    }
    else
    {
        retVal = CAN_API_FAIL;
    }

    return retVal;
}

/*!
 * @brief This funciton receives a Can frame using polling mode
 *
 * @param ModulePtr  The pointer to the module
 * @param RxBufId    The index of the Rx Buffer
 * @param RxFramePtr The pointer to reception of the Can message frame
 * 
 * @retval CAN_API_SUCCESS - Read Rx Message Buffer Successfully.
 * @retval CAN_API_FAIL    - No new message.
 */
Can_ApiRetStsType Can_ReceiveBlocking(CAN_T *ModulePtr, Can_RxBufIdType RxBufId, Can_RxFrameType *RxFramePtr)
{
    Can_ApiRetStsType retVal = CAN_API_SUCCESS;
#if (defined(CAN_RETRY_COUNT) && CAN_RETRY_COUNT)
    uint32_t retryCnt = CAN_RETRY_COUNT;
#endif

    if(RxBufId > 63U)
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        while (0U == Can_ReadRxBufNewDataFlg(ModulePtr, RxBufId))
        {
    #if (defined(CAN_RETRY_COUNT) && CAN_RETRY_COUNT)
            if (0U == retryCnt--)
            {
                retVal = CAN_API_FAIL;
                break;
            }
            else
            {
                /* do nothing */
            }
    #endif
        }
    
    #if (defined(CAN_RETRY_COUNT) && CAN_RETRY_COUNT)
        if (CAN_API_SUCCESS == retVal)
    #endif
        {
            Can_ClearRxBufNewDataFlg(ModulePtr, RxBufId);
            retVal = Can_ReadRxBuffer(ModulePtr, RxBufId, RxFramePtr);
        }
    }

    return retVal;
}

/*!
 * @brief This funciton receives a Can frame using polling mode
 *
 * @param ModulePtr  The pointer to the module
 * @param FifoBlkId  Rx FIFO 0 or 1
 * @param RxFramePtr The pointer to reception of the Can message frame
 * 
 * @retval CAN_API_SUCCESS:Read Message successfully
 * @retval CAN_API_FAIL:No new message in Rx FIFO
 */
Can_ApiRetStsType Can_ReceiveFifoBlocking(CAN_T *ModulePtr, Can_RxFifoType FifoBlkId, Can_RxFrameType *RxFramePtr)
{
    Can_ApiRetStsType retVal = CAN_API_SUCCESS;
    uint32_t irValMsk = 0U;
    uint32_t irRf0nMask = 0x01U;
    uint32_t irRf1nMask = 0x10U;
#if (defined(CAN_RETRY_COUNT) && CAN_RETRY_COUNT)
    uint32_t retryCnt = CAN_RETRY_COUNT;
#endif

    if(((CAN_RX_FIFO0 != FifoBlkId) && (CAN_RX_FIFO1 != FifoBlkId)) || (NULL == RxFramePtr))
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        irValMsk = (CAN_RX_FIFO0 == FifoBlkId) ? irRf0nMask : irRf1nMask;

        while (0U == Can_ReadIntFlag(ModulePtr, irValMsk))
        {
    #if (defined(CAN_RETRY_COUNT) && CAN_RETRY_COUNT)
            if (0U == retryCnt--)
            {
                retVal = CAN_API_FAIL;
                break;
            }
            else
            {
                /* do nothing */
            }
    #endif
        }
    
    #if (defined(CAN_RETRY_COUNT) && CAN_RETRY_COUNT)
        if (CAN_API_SUCCESS == retVal)
    #endif
        {
            Can_ClearIntFlag(ModulePtr, irValMsk);
            retVal = Can_ReadRxFifo(ModulePtr, FifoBlkId, RxFramePtr);
        }
    }

    return retVal;
}

/*!
 * @brief Configure the Can handle.
 *
 * @param ModulePtr  The pointer to the module
 * @param HandlePtr  The pointer to the handle
 * @param Callback   The callback function
 * @param InputParaPtr  The pointer to the inputting parameter for the callback
 */
void Can_CreateHandle(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_RxTxCallbackType Callback, 
    uint8_t *InputParaPtr)
{
    uint8_t moduleId = 0U;
    uint8_t circleCnt = 0U;

    if(NULL == HandlePtr)
    {
        /* do nothing */
    }
    else
    {
        moduleId = (uint8_t)Can_GetModuleId(ModulePtr);

        /* Clear the tx status of the tx buffers */
        for(; circleCnt < 32U; circleCnt++)
        {
            HandlePtr->txBufSts[circleCnt] = (uint8_t)CAN_IDLE_STATE;
        }
        /* Clear the rx status of the rx buffers */
        for(circleCnt = 0U; circleCnt < 64U; circleCnt++)
        {
            HandlePtr->rxBufSts[circleCnt] = (uint8_t)CAN_IDLE_STATE;
        }
        /* Clear the rx fifo status */
        HandlePtr->rxFifoState[CAN_RX_FIFO0] = (uint8_t)CAN_IDLE_STATE;
        HandlePtr->rxFifoState[CAN_RX_FIFO1] = (uint8_t)CAN_IDLE_STATE;
        
        if(moduleId < CAN_MODULE_NUM)
        {
            s_canHandleArr[moduleId] = HandlePtr;
        }
        else
        {
            /* do nothing */
        }        

        /* Register the Callback */
        HandlePtr->callback = Callback;
        HandlePtr->InputParaPtr = InputParaPtr;
        
        if (HandlePtr->callback != NULL)
        {
            Can_EnableInt(ModulePtr, 0U, (uint32_t)CAN_BUSOFF_INT_EN |
                 (uint32_t)CAN_ERROR_INT_EN | (uint32_t)CAN_WARNING_INT_EN);
        }
        else
        {
            Can_DisableInt(ModulePtr, (uint32_t)CAN_BUSOFF_INT_EN | 
                (uint32_t)CAN_ERROR_INT_EN | (uint32_t)CAN_WARNING_INT_EN);
        }
        
        (void)Nvic_EnableIrqRequest(CAN_IT0_IRQn, 0xf);
        (void)Nvic_EnableIrqRequest(CAN_IT1_IRQn, 0xe);
    }
}

/*!
 * @brief This funciton sends a Can frame using interrupt mode
 *
 * @param ModulePtr  The pointer to the module
 * @param HandlePtr  The pointer to the handle
 * @param TxInfoPtr  The pointer to the tx Buffer transfer infomation
 * 
 * @retval CAN_API_SUCCESS  Send successfully
 * @retval CAN_API_FAIL     Send failed
 */
Can_ApiRetStsType Can_SendNonBlocking(CAN_T *ModulePtr, Can_HandleType *HandlePtr, const Can_BufTransInfoType *TxInfoPtr)
{
    Can_ApiRetStsType retVal = CAN_API_FAIL;

    if((NULL == HandlePtr) || (NULL == TxInfoPtr) || (TxInfoPtr->bufferIdx > 63U))
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        if ((uint8_t)CAN_IDLE_STATE == HandlePtr->txBufSts[TxInfoPtr->bufferIdx])
        {
            if ((uint8_t)CAN_REMOTE_FRAME == TxInfoPtr->txFramePtr->Can_TxFrameHead0.rtr)
            {
                HandlePtr->txBufSts[TxInfoPtr->bufferIdx] = (uint8_t)CAN_TX_REMOTE_STATE;
            }
            else
            {
                HandlePtr->txBufSts[TxInfoPtr->bufferIdx] = (uint8_t)CAN_TX_DATA_STATE;
            }
        
            if (CAN_API_SUCCESS == Can_WriteTxBuffer(ModulePtr, TxInfoPtr->bufferIdx, TxInfoPtr->txFramePtr))
            {
                /* Enable Buffer Interrupt */
                Can_EnableTxBufInt(ModulePtr, TxInfoPtr->bufferIdx);
                Can_EnableInt(ModulePtr, 0U, CAN_IE_TCE_MASK);
        
                Can_TxAddReq(ModulePtr, TxInfoPtr->bufferIdx);
        
                retVal = CAN_API_SUCCESS;
            }
            else
            {
                HandlePtr->txBufSts[TxInfoPtr->bufferIdx] = (uint8_t)CAN_IDLE_STATE;
                retVal  = CAN_API_FAIL;
            }
        }
        else
        {
            retVal = CAN_API_FAIL;
        }
    }

    return retVal;
}

/*!
 * @brief This funciton receives a Can frame from FIFO using interrupt mode
 *
 * @param ModulePtr  The pointer to the module
 * @param FifoBlkId  Rx FIFO 0 or 1
 * @param HandlePtr  The pointer to the handle
 * @param RxFifoTranPtr The pointer to Rx FIFO transfer
 * 
 * @retval CAN_API_SUCCESS : Rx FIFO received successfully
 * @retval CAN_API_FAIL : Rx FIFO received failed
 */
Can_ApiRetStsType Can_ReceiveFifoNonBlocking(CAN_T *ModulePtr,
                                             Can_RxFifoType FifoBlkId,
                                             Can_HandleType *HandlePtr,
                                             const Can_RxFifoTransInfoType *RxFifoTranPtr)
{
    Can_ApiRetStsType retVal = CAN_API_FAIL;

    if(((CAN_RX_FIFO0 != FifoBlkId) && (CAN_RX_FIFO1 != FifoBlkId)) || (NULL == HandlePtr) ||
        (NULL == RxFifoTranPtr))
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        if ((uint8_t)CAN_IDLE_STATE == HandlePtr->rxFifoState[FifoBlkId])
        {
            HandlePtr->rxFifoState[FifoBlkId] = (uint8_t)CAN_RX_FIFO_STATE;
        
            /* Register Message Buffer */
            HandlePtr->rxFifoFrameBuf[FifoBlkId] = RxFifoTranPtr->rxFramePtr;
        
            /* Enable FIFO Interrupt. */
            if (CAN_RX_FIFO1 == FifoBlkId)
            {
                Can_EnableInt(ModulePtr, 0U, CAN_IE_RF1NE_MASK);
            }
            else
            {
                Can_EnableInt(ModulePtr, 0U, CAN_IE_RF0NE_MASK);
            }
            retVal = CAN_API_SUCCESS;
        }
        else
        {
            retVal = CAN_API_FAIL;
        }
    }

    return retVal;
}

/*!
 * @brief This funciton receives a Can frame from buffers using interrupt mode
 *
 * @param ModulePtr  The pointer to the module
 * @param RxBufId    The index of the Rx Buffer
 * @param HandlePtr  The pointer to the handle
 * @param RxBufFramePtr The pointer to Rx buffer Frame
 * 
 * @retval CAN_API_SUCCESS : Rx buffer received successfully
 * @retval CAN_API_FAIL : Rx buffer received failed
 */
Can_ApiRetStsType Can_ReceiveNonBlocking(CAN_T *ModulePtr,
                                         Can_RxBufIdType RxBufId,
                                         Can_HandleType *HandlePtr,
                                         Can_RxFrameType *RxBufFramePtr)
{
    Can_ApiRetStsType retVal = CAN_API_FAIL;

    if((RxBufId > CAN_RX_BUF_ID_63) || (NULL == HandlePtr) ||
        (NULL == RxBufFramePtr))
    {
        retVal = CAN_API_FAIL;
    }
    else
    {
        if ((uint8_t)CAN_IDLE_STATE == HandlePtr->rxBufSts[RxBufId])
        {
            HandlePtr->rxBufSts[RxBufId] = (uint8_t)CAN_RX_DATA_STATE;
        
            /* Register Message Buffer */
            HandlePtr->rxBufFrame[RxBufId] = RxBufFramePtr;
        
            /* Enable the Rx buffer Interrupt */
            Can_EnableInt(ModulePtr, 0U, CAN_IE_DRXE_MASK);
        
            retVal = CAN_API_SUCCESS;
        }
        else
        {
            retVal = CAN_API_FAIL;
        }
    }

    return retVal;
}

/*!
 * @brief Finish data transfer by completing the process of sending data
 *        and disabling the interrupt
 *
 * @param ModulePtr  The pointer to the module
 * @param HandlePtr  The pointer to the handle
 * @param TxBufId    The index of the Tx Buffer
 */
void Can_FinishDataTransfer(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_TxBufferIdType TxBufId)
{
    if((NULL == HandlePtr) || (TxBufId > 31U))
    {
        /* do nothing */
    }
    else
    {
        Can_DisableTxBufInt(ModulePtr, TxBufId);
        Can_TxCancelReq(ModulePtr, TxBufId);

        HandlePtr->txBufSts[TxBufId] = (uint8_t)CAN_IDLE_STATE;
    }
}

/*!
 * @brief Finish receive data from the RxFIFO using interrupt mode
 *
 * @param ModulePtr  The pointer to the module
 * @param FifoBlkId  Rx FIFO 0 or 1
 * @param HandlePtr  The pointer to the handle
 */
void Can_FinishReceiveFifo(CAN_T *ModulePtr, Can_RxFifoType FifoBlkId, Can_HandleType *HandlePtr)
{
    if((NULL == HandlePtr) || ((CAN_RX_FIFO0 != FifoBlkId) && (CAN_RX_FIFO1 != FifoBlkId)))
    {
        /* do nothing */
    }
    else
    {
        if (CAN_RX_FIFO1 == FifoBlkId)
        {
            Can_DisableInt(ModulePtr, CAN_IE_RF1NE_MASK);
        }
        else
        {
            Can_DisableInt(ModulePtr, CAN_IE_RF0NE_MASK);
        }
        
        HandlePtr->rxFifoFrameBuf[FifoBlkId] = NULL;
        HandlePtr->rxFifoState[FifoBlkId] = (uint8_t)CAN_IDLE_STATE;
    }
}

/*!
 * @brief Finish receive data from the dedicate buffer using interrupt mode
 *        and disabling the interrupt
 *
 * @param ModulePtr  The pointer to the module
 * @param HandlePtr  The pointer to the handle
 * @param RxBufId    The index of the Rx Buffer
 */
void Can_FinishReceiveBuf(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_RxBufIdType RxBufId)
{
    if((NULL == HandlePtr) || (RxBufId > 63U))
    {
        /* do nothing */
    }
    else
    {
        Can_ClearRxBufNewDataFlg(ModulePtr, RxBufId);

        /* Un-register handle. */
        HandlePtr->rxBufFrame[RxBufId] = NULL;
        
        HandlePtr->rxBufSts[RxBufId] = (uint8_t)CAN_IDLE_STATE;
    }
}

/*!
 * @brief Convert the data length in bytes to dlc in the frame
 *
 * @param Len  The data length in bytes
 * @retval The dlc value
 */
uint8_t Can_ConvertLenByteToDlc(uint8_t Len)
{
    uint8_t txMsgDlc = 0U;

    /* Check DLC length and calculate the Hw support value if DLC > 24 */
    if (Len > 24U)
    {
        /* Set DLC length with Hw Supported value */
        txMsgDlc = (((Len + 15U) >> 4U) + 11U);
    }
    /* Check DLC length and calculate the Hw support value if DLC > 8 */
    else if (Len > 8U)
    {
        /* Set DLC length with Hw Supported value */
        txMsgDlc = (((Len + 3U) >> 2U) + 6U);
    }
    else
    {
        txMsgDlc = Len;
    }

    return txMsgDlc;
}

/*!
 * @brief Can interrupt handle:including Can Error, the Tx Buffer, the Rx FIFO IRQ 
 * request and the Rx dedicate buffer interrupt
 *
 * @param ModulePtr  The pointer to the module
 * @param HandlePtr  The pointer to the handle
 */
void Can_TransferHandleIsr(CAN_T *ModulePtr, Can_HandleType *HandlePtr)
{
    Can_TransferStsType retVal = CAN_NO_HANDLE_STS;
    uint32_t irVal = 0U;
    uint32_t resVal = 0U;
    uint8_t bufId = 0U;

    if(NULL == HandlePtr)
    {
        /* do nothing */
    }
    else
    {
        /* Get the IR register value */
        irVal = ModulePtr->IR_R.IR;
        
        do
        {
            if (0U != (irVal & ((uint32_t)CAN_EW_INT_STS | (uint32_t)CAN_EP_INT_STS |
                                  (uint32_t)CAN_BO_INT_STS)))
            {
                resVal = (irVal & ((uint32_t)CAN_EW_INT_STS | (uint32_t)CAN_EP_INT_STS |
                                     (uint32_t)CAN_BO_INT_STS));
                retVal = CAN_ERROR_STS;
            }
            else if (0U != (irVal & (uint32_t)CAN_TC_INT_STS))
            {
                for (bufId = 0U; bufId < (uint8_t)(ModulePtr->TXBC_R.TXBC_B.NDTB); bufId++)
                {
                    if (0U != Can_CheckTransOccurred(ModulePtr, (Can_TxBufferIdType)bufId))
                    {
                        if (((ModulePtr->TXBTIE_R.TXBTIE) & ((uint32_t)1U << bufId)) != 0U)
                        {
                            Can_FinishDataTransfer(ModulePtr, HandlePtr, (Can_TxBufferIdType)bufId);
                        }
                        else
                        {
                            /* do nothing */
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
        
                resVal = (uint32_t)CAN_TC_INT_STS;
                retVal = CAN_TX_IDLE;
            }
            else if (0U != (irVal & (uint32_t)CAN_RF0N_INT_STS))
            {
                (void)Can_ReadRxFifo(ModulePtr, CAN_RX_FIFO0, HandlePtr->rxFifoFrameBuf[CAN_RX_FIFO0]);
                resVal = (uint32_t)CAN_RF0N_INT_STS;
                retVal = CAN_FIFO0_RX_IDLE;
                Can_FinishReceiveFifo(ModulePtr, CAN_RX_FIFO0, HandlePtr);
            }
            else if (0U != (irVal & (uint32_t)CAN_RF0L_INT_STS))
            {
                resVal = (uint32_t)CAN_RF0L_INT_STS;
                retVal = CAN_FIFO0_RX_LOST;
            }
            else if (0U != (irVal & (uint32_t)CAN_RF1N_INT_STS))
            {
                (void)Can_ReadRxFifo(ModulePtr, CAN_RX_FIFO1, HandlePtr->rxFifoFrameBuf[CAN_RX_FIFO1]);
                resVal = (uint32_t)CAN_RF1N_INT_STS;
                retVal = CAN_FIFO1_RX_IDLE;
                Can_FinishReceiveFifo(ModulePtr, CAN_RX_FIFO1, HandlePtr);
            }
            else if (0U != (irVal & (uint32_t)CAN_RF1L_INT_STS))
            {
                resVal = (uint32_t)CAN_RF1L_INT_STS;
                retVal = CAN_FIFO1_RX_LOST;
            }
            else if (0U != (irVal & (uint32_t)CAN_DRX_INT_STS))
            {
                Can_HandleDedBufRxInt(ModulePtr, HandlePtr);
        
                retVal = CAN_RX_IDLE;
            }
            else
            {
                /* The driver does not implement handling for these interrupt flags.
                   They are forwarded to the user via the callback for optional processing.
                   Remove the pending condition by clearing the flags in this handler.*/
                resVal = irVal;
                resVal &= ~((uint32_t)CAN_EW_INT_STS | (uint32_t)CAN_EP_INT_STS |
                            (uint32_t)CAN_BO_INT_STS | (uint32_t)CAN_TC_INT_STS |
                            (uint32_t)CAN_RF0N_INT_STS | (uint32_t)CAN_RF0L_INT_STS |
                            (uint32_t)CAN_RF1N_INT_STS | (uint32_t)CAN_RF1L_INT_STS | 
                            (uint32_t)CAN_DRX_INT_STS);
            }
        
            /* Reset interrupt status for errors, Rx FIFO/Tx buffer notifications, 
               plus any additional unsupported flags to avoid retriggering. */
            if(CAN_RX_IDLE == retVal)
            {
                Can_ClearIntFlag(ModulePtr, (uint32_t)CAN_DRX_INT_STS);
            }
            else
            {
                Can_ClearIntFlag(ModulePtr, resVal);
            }
        
            if(CAN_RX_IDLE != retVal)
            {
                if (HandlePtr->callback != NULL)
                {
                    HandlePtr->callback(ModulePtr, HandlePtr, retVal, resVal, HandlePtr->InputParaPtr);
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }
        
            retVal = CAN_NO_HANDLE_STS;
            resVal = 0U;
            irVal = ModulePtr->IR_R.IR;
        } while (0U != irVal);
    }
}

/**@} end of group CAN_Functions */
/**@} end of group CAN_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
