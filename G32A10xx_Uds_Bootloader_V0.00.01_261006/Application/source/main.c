/**************************************************************************
* @file             main.c
*
* @version          1.0.0
*
* @brief            Bootloader Framework -  main function
*
* Copyright (C) 2025 Geehy Semiconductor
*
* You may not use this file except in compliance with the GEEHY COPYRIGHT NOTICE
* (GEEHY SOFTWARE PACKAGE LICENSE).
*
* The program is only for reference, which is distributed in the hope that it
* will be useful and instructional for customers to develop their software.
* Unless required by applicable law or agreed to in writing, the program is
* distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY
* KIND, either express or implied. See the GEEHY SOFTWARE PACKAGE LICENSE for
* the governing permissions and limitations under the License.
*
**************************************************************************/

#include "includes.h"
#include "bootloader_main.h"
#include "flash.h"
#include "TP.h"
#include "can_driver.h"
#include "user_versions.h"
#include "uds_dtc_nvm.h"

#include "CRC_hal.h"

/**************************************************************************
                    LOCAL FUNCTION
**************************************************************************/
/*!
 * @brief          The function sends the message.
 *
 * @param[in]      void
 *
 * @retval         void
 */
static void Bsp_AbortCanTxMsg(void)
{

}

/*!
 * @brief          The function initializes the board periperals.
 *
 * @param[in]      void
 *
 * @retval         void
 */
static void Bsp_Init(void)
{
   /* Init the mcu,port,uart and ocu modules */
    InitCAN();
    
    InitFlash();
}

/**************************************************************************
                    main.c
**************************************************************************/
 uint32_t ecu_start_delay=0;
int main(void)
{
    uint8 array[8];
    uint32 id = 0u;
    uint32 space = 0u;
    uint8_t sw[LEN_SW_VERSION]={0};
	TP_LengthType sw_len=0;
	
    udsTaskPrepare(Bsp_Init, Bsp_AbortCanTxMsg);

	MainDebugLog("ready Enter BootLoader \r\n");
    Did_Nvm_Init();
    Did_Info_Init();
    FaultInfo_Init();

	MakeDebugLEDProgOn();    
	Did_Read(0xF189,sw,&sw_len);
    MainDebugLog("Enter BootLoader  Successfully!!--26100602-->>> %s\r\n",sw); 

    //Did_Nvm_Wr_Clear_Test();//«Â≥˝Dflash“≥0∫Õ“≥1
	//crc32_test();
	
    while(1)
    {
        udsTaskContent();

        if (TP_ReadDataInTransport(8u, &array[0u], &id, &space) == TRUE)
        {
            TransmitCANMsg(id, space, array, &TP_DoTxMessageSusCallback, 0u);
        }
    }
}

