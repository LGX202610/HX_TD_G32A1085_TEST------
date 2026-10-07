/*!
 * @file        g32a10xx_fls.c
 *
 * @brief       This file provides all the FMC firmware functions
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

#include "g32a10xx_fls.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup FMC_Driver
  @{
*/

/** @defgroup FMC_Functions Functions
  @{
  */

/*!
 * @brief     Sets the code latency value.
 *
 * @param     latency: the flash latency value.
 *                     The parameter can be one of following values:
 *                       @arg FMC_LATENCY_0
 *                       @arg FMC_LATENCY_1
 *                       @arg FMC_LATENCY_2
 * @retval    None
 */
void Fmc_SetLatency(Fmc_LatencyType Latency)
{
    if((Latency == FMC_LATENCY_2) && (FMC->CTRL1_R.CTRL1_B.WS == (uint8_t)FMC_LATENCY_0))
    {
        FMC->CTRL1_R.CTRL1_B.WS = (uint8_t)FMC_LATENCY_1;
    }
    else
    {
        /* nothing*/
    }

    FMC->CTRL1_R.CTRL1_B.WS = (uint8_t)Latency;
}

/*!
 * @brief     Enables the Prefetch Buffer.
 *
 * @param     None
 *
 * @retval    None
 */
void Fmc_EnablePrefetchBuffer(void)
{
    FMC->CTRL1_R.CTRL1_B.PBEN = ENABLE;
}

/*!
 * @brief     Disables the Prefetch Buffer.
 *
 * @param     None
 *
 * @retval    None
 */
void Fmc_DisablePrefetchBuffer(void)
{
    FMC->CTRL1_R.CTRL1_B.PBEN = DISABLE;
}

/*!
 * @brief       Checks whether the flash Prefetch Buffer status is set or not
 *
 * @param       None
 *
 * @retval      flash Prefetch Buffer Status (SET or RESET)
 */
uint8_t Fmc_ReadPrefetchBufferStatus(void)
{
    uint8_t ret = RESET;

    if ((FMC->CTRL1_R.CTRL1_B.PBSF) != 0U)
    {
        ret = SET;
    }
    else
    {
        /* nothing*/
    }

    return ret;
}

/*!
 * @brief       Unlocks the flash Program Erase Controller
 *
 * @param       None
 *
 * @retval      None
 */
void Fmc_Unlock(void)
{
    FMC->KEY_R.KEY = FMC_KEY_1;
    FMC->KEY_R.KEY = FMC_KEY_2;
}

/*!
 * @brief       Locks the flash Program Erase Controller
 *
 * @param       None
 *
 * @retval      None
 */
void Fmc_Lock(void)
{
    FMC->CTRL2_R.CTRL2_B.LOCK = BIT_SET;
}

/*!
 * @brief       Read flash state
 *
 * @param       None
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_BUSY
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 */
Fmc_StateType Fmc_ReadState(void)
{
    uint32_t status = 0U;
    Fmc_StateType state = FMC_STATE_COMPLETE;

    status = FMC->STS_R.STS;

    if ((status & (uint8_t)FMC_FLAG_PE) != 0U)
    {
        state = FMC_STATE_PG_ERR;
    }
    else if ((status & (uint8_t)FMC_FLAG_WPE) != 0U)
    {
        state = FMC_STATE_WRP_ERR;
    }
    else if ((status & (uint8_t)FMC_FLAG_BUSY) != 0U)
    {
        state = FMC_STATE_BUSY;
    }
    else if ((status & (uint32_t)FMC_FLAG_DBFI) != 0U)
    {
        state = FMC_STATE_DBFI;
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Wait for flash controler ready
 *
 * @param       timeOut:    Specifies the time to wait
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_BUSY
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 *                 @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_WaitForReady(uint32_t TimeOut)
{
    Fmc_StateType state = FMC_STATE_COMPLETE;
    uint32_t period = TimeOut;

    do
    {
        state = Fmc_ReadState();
        period--;
    }
    while ((state == FMC_STATE_BUSY) && (period != 0U));

    if (period == 0U)
    {
        state = FMC_StateTypeIMEOUT;
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Erases a specified flash page
 *
 * @param       pageAddr:   Specifies the page address
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 *                 @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_ErasePage(uint32_t PageAddr)
{
    Fmc_StateType state = FMC_STATE_COMPLETE;

    state = Fmc_WaitForReady(FMC_DELAY_ERASE);

    if (state == FMC_STATE_COMPLETE)
    {
        FMC->CTRL2_R.CTRL2_B.PAGEERA = BIT_SET;

        FMC->ADDR_R.ADDR = PageAddr;

        FMC->CTRL2_R.CTRL2_B.STA = BIT_SET;

        state = Fmc_WaitForReady(FMC_DELAY_ERASE);

        FMC->CTRL2_R.CTRL2_B.PAGEERA = BIT_RESET;
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Erases all flash pages
 *
* @param        WipeAreaSel:   Wipe area selection
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 *                 @arg Fmc_StateTypeIMEOUT
 * @note
 */
Fmc_StateType Fmc_EraseAllPages(Fmc_MerType WipeAreaSel)
{
    Fmc_StateType state = FMC_STATE_COMPLETE;

    state = Fmc_WaitForReady(FMC_DELAY_ERASE);

    if (state == FMC_STATE_COMPLETE)
    {
        FMC->CTRL2_R.CTRL2_B.MASSERA = BIT_SET;
        FMC->CTRL2_R.CTRL2_B.MER_TYPE = (uint8_t)WipeAreaSel;
        FMC->CTRL2_R.CTRL2_B.STA = BIT_SET;

        state = Fmc_WaitForReady(FMC_DELAY_ERASE);

        FMC->CTRL2_R.CTRL2_B.MASSERA = BIT_RESET;
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Program a word at a specified address
 *
 * @param       addr:   Specifies the address to be programmed
 *
 * @param       len:    Specifies the len to be programmed
 *
 * @param       data:   Specifies the data to be programmed
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 *                 @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_ProgramWord(uint32_t Addr,uint32_t Len, const uint32_t *Data)
{
    const uint32_t FLASH_PROG_DATA[4]={2,4,8,16};
    Fmc_StateType state = FMC_STATE_COMPLETE;
    volatile uint32_t *flashProgPtr = (volatile uint32_t *)&FMC->PROG_DATA0_R.PROG_DATA0;
    uint32_t i = 0U;

    state = Fmc_WaitForReady(FMC_DELAY_PROGRAM);

    if (state == FMC_STATE_COMPLETE)
    {
        FMC->CTRL2_R.CTRL2_B.PG = BIT_SET;
        FMC->CTRL2_R.CTRL2_B.PROGLEN = Len;
        
        FMC->ADDR_R.ADDR_B.ADDR = Addr;
        
        for(i = 0; i<FLASH_PROG_DATA[Len]; i++)
        {
            *flashProgPtr = Data[i];
            
            flashProgPtr++;
        }
        
        FMC->CTRL2_R.CTRL2_B.STA = BIT_SET;

        state = Fmc_WaitForReady(FMC_DELAY_PROGRAM);
        
        FMC->CTRL2_R.CTRL2_B.PROGLEN = BIT_RESET;
        FMC->CTRL2_R.CTRL2_B.PG = BIT_RESET;

    }
    else
    {
        /* nothing*/
    }

    return state;
}



/*!
 * @brief       Unlocks the option bytes block access
 *
 * @param       None
 *
 * @retval      None
 */
void Fmc_UnlockOptionByte(void)
{
    FMC->OBKEY_R.OBKEY = FMC_OB_KEY_1;
    FMC->OBKEY_R.OBKEY = FMC_OB_KEY_2;
}

/*!
 * @brief       Locks the option bytes block access
 *
 * @param       None
 *
 * @retval      None
 */
void Fmc_LockOptionByte(void)
{
    FMC->CTRL2_R.CTRL2_B.OBWEN = BIT_RESET;
}

/*!
 * @brief       Launch the option byte loading
 *
 * @param       None
 *
 * @retval      None
 */
void Fmc_LaunchOptionByte(void)
{
    FMC->CTRL2_R.CTRL2_B.OBLOAD = BIT_SET;
}

/*!
 * @brief       Erase the flash option bytes
 *
 * @param       None
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 *                 @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_EraseOptionByte(void)
{
    Fmc_StateType state = FMC_STATE_COMPLETE;

    state = Fmc_WaitForReady(FMC_DELAY_ERASE);

    if (state == FMC_STATE_COMPLETE)
    {
        FMC->OBKEY_R.OBKEY = FMC_KEY_1;
        FMC->OBKEY_R.OBKEY = FMC_KEY_2;

        FMC->CTRL2_R.CTRL2_B.OBE = BIT_SET;
        FMC->CTRL2_R.CTRL2_B.STA = BIT_SET;

        state = Fmc_WaitForReady(FMC_DELAY_ERASE);

        FMC->CTRL2_R.CTRL2_B.OBE = BIT_RESET;
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Perform a programming operation at the specified address of the option byte.
 *
 * @param       addr:   Specifies the address to be programmed
 *
 * @param       data:   Specifies the data to be programmed
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 *                 @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_ProgramOptionByte(uint32_t Addr, const uint32_t *Data)
{
    uint8_t i = 0U;
    Fmc_StateType state = FMC_STATE_COMPLETE;
    volatile uint32_t *flashProgPtr = (volatile uint32_t *)&FMC->PROG_DATA0_R.PROG_DATA0;
    
    state = Fmc_WaitForReady(FMC_DELAY_PROGRAM);

    if (state == FMC_STATE_COMPLETE)
    {
        FMC->CTRL2_R.CTRL2_B.OBP = BIT_SET;
        FMC->CTRL2_R.CTRL2_B.PROGLEN = (uint8_t)FMC_LEN_64BIT;
        
        FMC->ADDR_R.ADDR_B.ADDR = Addr;
        
        for(i = 0; i < 2U; i++)
        {
            *flashProgPtr = Data[i];
            
            flashProgPtr++;
        }
        
        FMC->CTRL2_R.CTRL2_B.STA = BIT_SET;
        
        state = Fmc_WaitForReady(FMC_DELAY_PROGRAM);
        
        FMC->CTRL2_R.CTRL2_B.PROGLEN = BIT_RESET;
        FMC->CTRL2_R.CTRL2_B.OBP = BIT_RESET;
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Enable the write protection function for the specified pages of PFLASH
 *
 * @param       wrppConfig:   A pointer pointing to a "Fmc_Wrpp_ConfigType" structure, 
 *                            which contains the configuration information for 
 *                            the PFLASH write protection option.
 *
 * @retval      Returns the flash state.It can be one of value:
 *                      @arg FMC_STATE_COMPLETE
 *                      @arg FMC_STATE_PG_ERR
 *                      @arg FMC_STATE_WRP_ERR
 *                      @arg FMC_STATE_DBFI
 *                      @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_EnableWriteProtectionPflash(Fmc_Wrpp_ConfigType WrppConfig)
{
    Fmc_StateType state = FMC_STATE_COMPLETE;
    uint32_t temp = 0xFF00FF00U;
    uint32_t Data[2]={0,0};

    temp |= ((uint32_t)WrppConfig.WRP0);
    temp |= ((uint32_t)WrppConfig.WRP1 << 16);
    
    Data[0] = temp;

    temp = 0xFF00FF00U;
    
    temp |= ((uint32_t)WrppConfig.WRP2);
    temp |= ((uint32_t)WrppConfig.WRP3 << 16);
    
    Data[1] = temp;

    state = Fmc_WaitForReady(FMC_DELAY_PROGRAM);

    if (state == FMC_STATE_COMPLETE)
    {
        state = Fmc_ProgramOptionByte(FMC_WRP0_OB_ADDR, Data);
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Enable the write protection function for the specified pages of DFLASH
 *
 * @param       wrppConfig:   A pointer pointing to a "Fmc_Wrpd_ConfigType" structure, 
 *                            which contains the configuration information for 
 *                            the DFLASH write protection option.
 *
 * @retval      Returns the flash state.It can be one of value:
 *                      @arg FMC_STATE_COMPLETE
 *                      @arg FMC_STATE_PG_ERR
 *                      @arg FMC_STATE_WRP_ERR
 *                      @arg FMC_STATE_DBFI
 *                      @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_EnableWriteProtectionDflash(Fmc_Wrpd_ConfigType WrpdConfig)
{
    Fmc_StateType state = FMC_STATE_COMPLETE;
    uint32_t temp = 0xFF00FF00U;
    uint32_t Data[2]={0,0};

    temp |= ((uint32_t)WrpdConfig.WRP4);
    temp |= ((uint32_t)WrpdConfig.WRP5 << 16);
    
    Data[0] = temp;
    
    temp = 0xFF00FF00U;
    
    temp |= ((uint32_t)WrpdConfig.WRP6);
    temp |= ((uint32_t)WrpdConfig.WRP7 << 16);
    
    Data[1] = temp;
    
    state = Fmc_WaitForReady(FMC_DELAY_PROGRAM);

    if (state == FMC_STATE_COMPLETE)
    {
        state = Fmc_ProgramOptionByte(FMC_WRP4_OB_ADDR,Data);
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       User option byte configuration
 *
 * @param       userConfig: Pointer to a Fmc_User_ConfigType structure that
 *                          contains the configuration information for User option byte
 *
 * @retval      Returns the flash state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_DBFI
 *                 @arg Fmc_StateTypeIMEOUT
 */
Fmc_StateType Fmc_ConfigOptionByteUser(const Fmc_User_ConfigType* UserConfig)
{
    Fmc_StateType state = FMC_STATE_COMPLETE;
    uint32_t temp = 0xFFFFFF00U;
    uint32_t Data[2]={0,0};
    uint32_t data0 = 0U; 
    uint32_t data1 = 0U; 

    if(UserConfig->READROT == FMC_RDP_LEVEL_0)
    {
        temp |= (uint8_t)FMC_RDP_LEVEL_0;
    }
    else if(UserConfig->READROT == FMC_RDP_LEVEL_2)
    {
        temp |= (uint8_t)FMC_RDP_LEVEL_2;
    }
    else
    {
        temp |= (uint8_t)FMC_RDP_LEVEL_1;
    }
    
    if(UserConfig->IWDTSW == FMC_OB_IWDT_HW)
    {
        temp &= ~(uint32_t)FMC_OB_IWDT_SW;
    }
    else
    {
        /* nothing*/
    }

    if(UserConfig->STOPCE == FMC_OB_STOP_RESET)
    {
        temp &= ~(uint32_t)FMC_OB_STOP_NRST;
    }
    else
    {
        /* nothing*/
    }

    if(UserConfig->STDBYCE == FMC_OB_STDBY_RESET)
    {
        temp &= ~(uint32_t)FMC_OB_STDBY_NRST;
    }
    else
    {
        /* nothing*/
    }

    if(UserConfig->BOOT0SW == FMC_OB_BOOT0_RESET)
    {
        temp &= ~(uint32_t)FMC_OB_BOOT0_SET;
    }
    else
    {
        /* nothing*/
    }

    if(UserConfig->BOOT1SW == FMC_OB_BOOT1_RESET)
    {
        temp &= ~(uint32_t)FMC_OB_BOOT1_SET;
    }
    else
    {
        /* nothing*/
    }

    if(UserConfig->VDDASW == FMC_OB_VDDA_ANALOG_OFF)
    {
        temp &= ~(uint32_t)FMC_OB_VDDA_ANALOG_ON;
    }
    else
    {
        /* nothing*/
    }
    Data[0] = temp;
    
    data0 = (uint32_t)OB->DATA0_R.DATA0; 
    data1 = (uint32_t)OB->DATA1_R.DATA1; 
    temp = data0 | (data1 << 16);         
    
    Data[1] = temp;
    
    state = Fmc_WaitForReady(FMC_DELAY_PROGRAM);
    
    if (state == FMC_STATE_COMPLETE)
    {
        state = Fmc_ProgramOptionByte(FMC_OB_BASE,Data);
    }
    else
    {
        /* nothing*/
    }

    return state;
}

/*!
 * @brief       Returns the Flash User Option Bytes values
 *
 * @param       None
 *
 * @retval      The flash User Option Bytes
 */
uint8_t Fmc_ReadOptionByteUser(void)
{
    return (uint8_t)(FMC->OBCS_R.OBCS >> 8);
}

/*!
 * @brief       Returns the Pflash Write Protection Option Bytes value:
 *
 * @param       None
 *
 * @retval      The Flash Write Protection Option Bytes value:
 */
uint32_t Fmc_ReadPflashWriteProtection(void)
{
    return (uint32_t)(FMC->WRTPROT0_R.WRTPROT0);
}

/*!
 * @brief       Returns the Dflash Write Protection Option Bytes value:
 *
 * @param       None
 *
 * @retval      The Flash Write Protection Option Bytes value:
 */
uint32_t Fmc_ReadDflashWriteProtection(void)
{
    return (uint32_t)(FMC->WRTPROT1_R.WRTPROT1);
}

/*!
 * @brief       Checks whether the Flash Read Protection Status is set or not
 *
 * @param       None
 *
 * @retval      Flash ReadOut Protection Status(SET or RESET)
 */
uint8_t Fmc_GetReadProtectionStatus(void)
{
    uint8_t ret = RESET;

    if ((FMC->OBCS_R.OBCS_B.READPROT) != 0U)
    {
        ret = SET;
    }
    else
    {
        /* nothing*/
    }

    return ret;
}
/*!
 * @brief       Enable the specified flash interrupts
 *
 * @param       interrupt:  Specifies the flash interrupt sources
 *                          The parameter can be combination of following values:
 *                          @arg FMC_INT_ERROR:       Error interruption
 *                          @arg FMC_INT_COMPLETE:    operation complete interruption
 *
 * @retval      None
 */
void Fmc_EnableInterrupt(uint32_t Interrupt)
{
    FMC->CTRL2_R.CTRL2 |= Interrupt;
}

/*!
 * @brief       Disable the specified flash interrupts
 *
 * @param       interrupt:  Specifies the flash interrupt sources
 *                          The parameter can be combination of following values:
 *                          @arg FMC_INT_ERROR:       Error interruption
 *                          @arg FMC_INT_COMPLETE:    operation complete interruption
 *
 * @retval      None
 */
void Fmc_DisableInterrupt(uint32_t Interrupt)
{
    FMC->CTRL2_R.CTRL2 &= ~Interrupt;
}

/*!
 * @brief       Checks whether the specified flash flag is set or not
 *

 * @param       flag:   Specifies the flash flag to check
 *                      The parameter can be one of following values:
 *                      @arg FMC_FLAG_BUSY: Busy flag
 *                      @arg FMC_FLAG_PE:   Program error flag
 *                      @arg FMC_FLAG_WPE:  Write protection flag
 *                      @arg FMC_FLAG_OC:   Operation complete flag
 *                      @arg FMC_FLAG_DBFI:   Double BIT fault interrupt flag
 *
 * @retval      None
 */
uint8_t Fmc_ReadStatusFlag(Fmc_FlagType Flag)
{
    uint32_t status = 0U;
    uint8_t ret = RESET;

    if (((uint32_t)Flag & 0xffffffffU) != 0U)
    {
        status = FMC->STS_R.STS & (uint32_t)Flag;
    }
    else
    {
        /* nothing */
    }

    if (status != 0U)
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
 * @brief       Checks whether the OBCS specified flash flag is set or not
 *

 * @param       flag:   Specifies the flash flag to check
 *                      The parameter can be one of following values:
 *                      @arg FMC_FLAG_OBE: OptionwBytewError
 *                      @arg FMC_FLAG_SCRKERR:   Program error flag
 *                      @arg FMC_FLAG_SCRECCERR:  Write protection flag
 *                      @arg FMC_FLAG_OPTRERR:   Operation complete flag
 *                      @arg FMC_FLAG_OPTECCERR:   Double BIT fault interrupt flag
 *
 * @retval      None
 */
uint8_t Fmc_ReadOBCSStatusFlag(Fmc_FlagType Flag)
{
    uint32_t status = 0U;
    uint8_t ret = RESET;

    if (((uint32_t)Flag & 0xffffffffU) != 0U)
    {
        status = FMC->OBCS_R.OBCS & (uint32_t)Flag;
    }
    else
    {
        /* nothing */
    }

    if (status != 0U)
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
 * @brief       Clear the specified flash flag
 *
 * @param       flag:   Specifies the flash flag to clear
 *                      This parameter can be any combination of the following values:
 *                      @arg FMC_FLAG_BUSY:   Busy flag
 *                      @arg FMC_FLAG_PE:     Program error flag
 *                      @arg FMC_FLAG_WPE:    Write protection error flag
 *                      @arg FMC_FLAG_OC:     Operation complete flag
 *                      @arg FMC_FLAG_DBFI:   Double BIT fault interrupt flag
 *
 * @retval      None
 */
void Fmc_ClearStatusFlag(Fmc_FlagType Flag)
{
    if (((uint32_t)Flag & 0xffffffffU) != 0U)
    {
        FMC->STS_R.STS = (uint32_t)Flag;
    }
    else
    {
        /* nothing*/
    }
}

/**@} end of group FMC_Functions*/
/**@} end of group FMC_Driver*/
/**@} end of group G32A10xx_StdPeriphDriver*/
