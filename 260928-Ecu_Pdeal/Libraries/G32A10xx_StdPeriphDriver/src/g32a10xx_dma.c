/*!
 * @file        g32a10xx_dma.c
 *
 * @brief       This file contains all the functions for the DMA peripheral
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

/* Includes */
#include "g32a10xx_dma.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup DMA_Driver
  @{
*/

/** @defgroup DMA_Functions Functions
  @{
  */

/*!
 * @brief     Set the DMA peripheral registers to their default reset values
 *
 * @param     ChannelPtr:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 * @retval    None
 */
void Dma_Reset(DMA_CHANNEL_T* ChannelPtr)
{
    ChannelPtr->CHCFG_R.CHCFG_B.CHEN = 0;
    ChannelPtr->CHCFG_R.CHCFG = 0;
    ChannelPtr->CHNDATA_R.CHNDATA = 0;
    ChannelPtr->CHPADDR_R.CHPADDR = 0;
    ChannelPtr->CHMADDR_R.CHMADDR = 0;

    if (ChannelPtr == DMA1_CHANNEL_1)
    {
        DMA1->INTFCLR_R.INTFCLR = (uint32_t)0x0000000F;
    }
    else if (ChannelPtr == DMA1_CHANNEL_2)
    {
        DMA1->INTFCLR_R.INTFCLR = (uint32_t)0x000000F0;
    }
    else if (ChannelPtr == DMA1_CHANNEL_3)
    {
        DMA1->INTFCLR_R.INTFCLR = (uint32_t)0x00000F00;
    }
    else if (ChannelPtr == DMA1_CHANNEL_4)
    {
        DMA1->INTFCLR_R.INTFCLR = (uint32_t)0x0000F000;
    }
    else if (ChannelPtr == DMA1_CHANNEL_5)
    {
        DMA1->INTFCLR_R.INTFCLR = (uint32_t)0x000F0000;
    }
    else
    {
        /* nothing */
    }
}

/*!
 * @brief       Config the DMA peripheral according to the specified parameters in the dmaConfig
 *
 * @param     ChannelPtr:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 *
 * @param       DmaConfigPtr:  Pointer to a Dma_ConfigType structure that
 *                          contains the configuration information for the DMA peripheral
 *
 * @retval      None
 */
void Dma_Config(DMA_CHANNEL_T* ChannelPtr, const Dma_ConfigType* DmaConfigPtr)
{
    ChannelPtr->CHCFG_R.CHCFG_B.DIRCFG   = (uint8_t)(DmaConfigPtr->direction);
    ChannelPtr->CHCFG_R.CHCFG_B.CIRMODE  = (uint8_t)(DmaConfigPtr->circular);
    ChannelPtr->CHCFG_R.CHCFG_B.M2MMODE  = (uint8_t)(DmaConfigPtr->memoryTomemory);
    ChannelPtr->CHCFG_R.CHCFG_B.CHPL     = (uint8_t)(DmaConfigPtr->priority);
    ChannelPtr->CHCFG_R.CHCFG_B.MIMODE   = (uint8_t)(DmaConfigPtr->memoryInc);
    ChannelPtr->CHCFG_R.CHCFG_B.PERIMODE = (uint8_t)(DmaConfigPtr->peripheralInc);
    ChannelPtr->CHCFG_R.CHCFG_B.MSIZE    = (uint8_t)(DmaConfigPtr->memoryDataSize);
    ChannelPtr->CHCFG_R.CHCFG_B.PERSIZE  = (uint8_t)(DmaConfigPtr->peripheralDataSize);

    ChannelPtr->CHNDATA_R.CHNDATA = DmaConfigPtr->bufferSize;
    ChannelPtr->CHMADDR_R.CHMADDR = DmaConfigPtr->memoryAddress;
    ChannelPtr->CHPADDR_R.CHPADDR = DmaConfigPtr->peripheralAddress;
}

/*!
 * @brief       Fills each dmaConfig member with its default value
 *
 * @param       DmaConfigPtr:  Pointer to a Dma_ConfigType structure which will be initialized
 *
 * @retval      None
 */
void Dma_ConfigStructInit(Dma_ConfigType* DmaConfigPtr)
{
    DmaConfigPtr->direction = DMA_DIR_PERIPHERAL;
    DmaConfigPtr->circular = DMA_CIRCULAR_DISABLE;
    DmaConfigPtr->memoryTomemory = DMA_M2M_DISABLE;
    DmaConfigPtr->priority = DMA_PRIORITY_LEVEL_LOW;
    DmaConfigPtr->memoryInc = DMA_MEMORY_INC_DISABLE;
    DmaConfigPtr->peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    DmaConfigPtr->memoryDataSize = DMA_MEMORY_DATASIZE_BYTE;
    DmaConfigPtr->peripheralDataSize = DMA_PERIPHERAL_DATASIZE_BYTE;

    DmaConfigPtr->bufferSize = 0;
    DmaConfigPtr->memoryAddress = 0;
    DmaConfigPtr->peripheralAddress = 0;
}

/*!
 * @brief       Enable the DMA peripheral
 *
 * @param       ChannelPtr:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 *
 * @retval      None
 */
void Dma_Enable(DMA_CHANNEL_T* ChannelPtr)
{
    ChannelPtr->CHCFG_R.CHCFG_B.CHEN = BIT_SET;
}

/*!
 * @brief       Disable the DMA peripheral
 *
 * @param       ChannelPtr:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 *
 * @retval      None
 */
void Dma_Disable(DMA_CHANNEL_T* ChannelPtr)
{
    ChannelPtr->CHCFG_R.CHCFG_B.CHEN = BIT_RESET;
}

/*!
 * @brief       Set the DMA Channelx transfer data of number
 *
 * @param       ChannelPtr:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 *
 * @param       DataNumber:  The number of data units in the current DMA Channel transfer
 *
 * @retval      None
 */
void Dma_SetDataNumber(DMA_CHANNEL_T* ChannelPtr, uint32_t DataNumber)
{
    ChannelPtr->CHNDATA_R.CHNDATA = (uint32_t)DataNumber;
}

/*!
 * @brief       Read the DMA Channelx transfer data of number
 *
 * @param       ChannelPtr:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 *
 * @retval      The number of data units in the current DMA Channel transfer
 */
uint32_t Dma_ReadDataNumber(const DMA_CHANNEL_T* ChannelPtr)
{
    return ((uint32_t)ChannelPtr->CHNDATA_R.CHNDATA);
}

/*!
 * @brief       Enables the specified interrupts
 * @param       DMA_CHANNEL_T:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 *
 * @param       Interrupt:  Specifies the DMA interrupts sources
 *                          The parameter can be combination of following values:
 *                          @arg DMA_INT_TFIE:    Transfer complete interrupt
 *                          @arg DMA_INT_HTIE:    Half Transfer interrupt
 *                          @arg DMA_INT_TEIE:    Transfer error interrupt
 *
 * @retval      None
 */
void Dma_EnableInterrupt(DMA_CHANNEL_T* ChannelPtr, uint32_t Interrupt)
{
    ChannelPtr->CHCFG_R.CHCFG |= (uint32_t)Interrupt;
}

/*!
 * @brief       Disables the specified interrupts
 * @param       ChannelPtr:  Pointer to a DMA_CHANNEL_T structure that
 *                          set DMA channel for the DMA peripheral
 *                          This parameter can be one of the following values:
 *                           @arg DMA1_CHANNEL_1
 *                           @arg DMA1_CHANNEL_2
 *                           @arg DMA1_CHANNEL_3
 *                           @arg DMA1_CHANNEL_4
 *                           @arg DMA1_CHANNEL_5
 *
 * @param       Interrupt:  Specifies the DMA interrupts sources
 *                          The parameter can be combination of following values:
 *                           @arg DMA_INT_TFIE:    Transfer complete interrupt
 *                           @arg DMA_INT_HTIE:    Half Transfer interrupt
 *                           @arg DMA_INT_TEIE:    Transfer error interrupt
 *
 * @retval      None
 */
void Dma_DisableInterrupt(DMA_CHANNEL_T* ChannelPtr, uint32_t Interrupt)
{
    ChannelPtr->CHCFG_R.CHCFG &= (uint32_t)~Interrupt;
}

/*!
 * @brief       Checks whether the specified DMA flag is set or not
 *
 * @param       Flag:   Specifies the flag to check
 *                      This parameter can be one of the following values:
 *                      @arg DMA1_FLAG_AL1:   DMA1 Channel 1 All flag
 *                      @arg DMA1_FLAG_TF1:   DMA1 Channel 1 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT1:   DMA1 Channel 1 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE1:   DMA1 Channel 1 Transfer Error flag
 *                      @arg DMA1_FLAG_AL2:   DMA1 Channel 2 All flag
 *                      @arg DMA1_FLAG_TF2:   DMA1 Channel 2 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT2:   DMA1 Channel 2 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE2:   DMA1 Channel 2 Transfer Error flag
 *                      @arg DMA1_FLAG_AL3:   DMA1 Channel 3 All flag
 *                      @arg DMA1_FLAG_TF3:   DMA1 Channel 3 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT3:   DMA1 Channel 3 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE3:   DMA1 Channel 3 Transfer Error flag
 *                      @arg DMA1_FLAG_AL4:   DMA1 Channel 4 All flag
 *                      @arg DMA1_FLAG_TF4:   DMA1 Channel 4 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT4:   DMA1 Channel 4 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE4:   DMA1 Channel 4 Transfer Error flag
 *                      @arg DMA1_FLAG_AL5:   DMA1 Channel 5 All flag
 *                      @arg DMA1_FLAG_TF5:   DMA1 Channel 5 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT5:   DMA1 Channel 5 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE5:   DMA1 Channel 5 Transfer Error flag
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t Dma_ReadStatusFlag(Dma_FlagType Flag)
{
    uint32_t status = 0U;
    uint8_t ret = RESET;

    status = DMA1->INTSTS_R.INTSTS & ((uint32_t)Flag & 0x0FFFFFFFU);

    if (status == (uint32_t)Flag)
    {
        ret = SET;
    }
    else
    {
        /* nothing */
    }

    return ret;
}

/*!
 * @brief       Clear whether the specified DMA flag is set or not
 *
 * @param       Flag:   Specifies the flag to Clear
 *                      This parameter can be any combination of the following values:
 *                      @arg DMA1_FLAG_AL1:   DMA1 Channel 1 All flag
 *                      @arg DMA1_FLAG_TF1:   DMA1 Channel 1 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT1:   DMA1 Channel 1 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE1:   DMA1 Channel 1 Transfer Error flag
 *                      @arg DMA1_FLAG_AL2:   DMA1 Channel 2 All flag
 *                      @arg DMA1_FLAG_TF2:   DMA1 Channel 2 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT2:   DMA1 Channel 2 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE2:   DMA1 Channel 2 Transfer Error flag
 *                      @arg DMA1_FLAG_AL3:   DMA1 Channel 3 All flag
 *                      @arg DMA1_FLAG_TF3:   DMA1 Channel 3 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT3:   DMA1 Channel 3 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE3:   DMA1 Channel 3 Transfer Error flag
 *                      @arg DMA1_FLAG_AL4:   DMA1 Channel 4 All flag
 *                      @arg DMA1_FLAG_TF4:   DMA1 Channel 4 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT4:   DMA1 Channel 4 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE4:   DMA1 Channel 4 Transfer Error flag
 *                      @arg DMA1_FLAG_AL5:   DMA1 Channel 5 All flag
 *                      @arg DMA1_FLAG_TF5:   DMA1 Channel 5 Transfer Complete flag
 *                      @arg DMA1_FLAG_HT5:   DMA1 Channel 5 Half Transfer Complete flag
 *                      @arg DMA1_FLAG_TE5:   DMA1 Channel 5 Transfer Error flag
 *
 * @retval      None
 */

void Dma_ClearStatusFlag(uint32_t Flag)
{
    DMA1->INTFCLR_R.INTFCLR |= (uint32_t)(Flag & 0x0FFFFFFFU);
}

/*!
 * @brief       Checks whether the specified interrupt has occurred or not
 *
 * @param       Flag:   Specifies the DMA interrupt pending bit to check
 *                      The parameter can be one following values:
 *                      @arg DMA1_INT_FLAG_AL1:   DMA1_Channel 1 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF1:   DMA1_Channel 1 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT1:   DMA1_Channel 1 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE1:   DMA1_Channel 1 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL2:   DMA1_Channel 2 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF2:   DMA1_Channel 2 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT2:   DMA1_Channel 2 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE2:   DMA1_Channel 2 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL3:   DMA1_Channel 3 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF3:   DMA1_Channel 3 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT3:   DMA1_Channel 3 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE3:   DMA1_Channel 3 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL4:   DMA1_Channel 4 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF4:   DMA1_Channel 4 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT4:   DMA1_Channel 4 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE4:   DMA1_Channel 4 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL5:   DMA1_Channel 5 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF5:   DMA1_Channel 5 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT5:   DMA1_Channel 5 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE5:   DMA1_Channel 5 Transfer Error interrupt flag
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t Dma_ReadIntFlag(Dma_IntFlagType Flag)
{
    uint32_t status = 0U;
    uint8_t ret = RESET;

    status = DMA1->INTSTS_R.INTSTS & ((uint32_t)Flag & 0x0FFFFFFFU);

    if (status == (uint32_t)Flag)
    {
        ret = SET;
    }
    else
    {
        /* nothing */
    }

    return ret;
}

/*!
 * @brief       Clears the specified interrupt pending bits
 *
 * @param       Flag:   Specifies the DMA interrupt pending bit to clear
 *                      The parameter can be combination of following values:
 *                      @arg DMA1_INT_FLAG_AL1:   DMA1_Channel 1 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF1:   DMA1_Channel 1 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT1:   DMA1_Channel 1 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE1:   DMA1_Channel 1 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL2:   DMA1_Channel 2 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF2:   DMA1_Channel 2 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT2:   DMA1_Channel 2 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE2:   DMA1_Channel 2 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL3:   DMA1_Channel 3 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF3:   DMA1_Channel 3 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT3:   DMA1_Channel 3 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE3:   DMA1_Channel 3 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL4:   DMA1_Channel 4 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF4:   DMA1_Channel 4 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT4:   DMA1_Channel 4 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE4:   DMA1_Channel 4 Transfer Error interrupt flag
 *                      @arg DMA1_INT_FLAG_AL5:   DMA1_Channel 5 All interrupt flag
 *                      @arg DMA1_INT_FLAG_TF5:   DMA1_Channel 5 Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_HT5:   DMA1_Channel 5 Half Transfer Complete interrupt flag
 *                      @arg DMA1_INT_FLAG_TE5:   DMA1_Channel 5 Transfer Error interrupt flag
 *
 * @retval      None
 */
void Dma_ClearIntFlag(uint32_t Flag)
{
    DMA1->INTFCLR_R.INTFCLR |= (uint32_t)(Flag & 0x0FFFFFFFU);
}

/**@} end of group DMA_Functions */
/**@} end of group DMA_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */

