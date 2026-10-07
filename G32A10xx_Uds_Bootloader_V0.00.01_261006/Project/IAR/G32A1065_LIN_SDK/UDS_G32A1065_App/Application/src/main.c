#include "lin_app.h"
#include "UDS_app.h"
#include "TP.h"
#include "bootloader_debug.h"
#include "bootloader_main.h"
#include "watchdog_hal.h"
#include "timer_hal.h"
#include "boot.h"
#include "includes.h"
#include "flash.h"

uint16_t timerOverflowInterruptCount = 0u;

uint8 g_linRxBuf[8u] = {0u};
uint8 g_linTxBuf[8u] = {0u};

uint32_t LIN_GetTimerCallback(uint32_t *ns)
{
    static uint32_t previousCountValue = 0UL;

    uint32_t counterValue = 0U;
    *ns = ((uint32_t)(counterValue + timerOverflowInterruptCount * 2000U - previousCountValue)) * 1000UL / 4U;
    timerOverflowInterruptCount = 0UL;
    previousCountValue = counterValue;

    return 0UL;
}



static void BSP_Init(void)
{
    Gpio_ConfigType gpioConfig;
    Rcm_EnableAhbPeriphClock(RCM_AHB_PERIPH_GPIOA);

    gpioConfig.mode = GPIO_MODE_OUT;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.speed = GPIO_SPEED_10MHz;
    gpioConfig.pin = GPIO_PIN_8;

    Gpio_Config(GPIOA, &gpioConfig);
    /* Wakeup the Lin transfer and receiver */
    Gpio_SetBit(GPIOA, GPIO_PIN_8);
    Lin_Init();

    InitFlash();
}

static void BSP_AbortCANTxMsg(void)
{

}


int main(void)
{
    uint32 tempMsgId = 0u;
    uint32 tempMsgLength = 0u;
    
    /* Relocation the interrupt table */
    SCB->VTOR = APP_A_BEGIN_ADDR + 0x200;
    udsTaskPrepare(BSP_Init, BSP_AbortCANTxMsg);
    
    #ifdef APP_A
        PrintDebugLog("Enter App_A!!!\r\n");
    #else
        PrintDebugLog("Enter App_B!!!\r\n");
    #endif
    
    for (;;)
    {
        udsTaskContent();
        if(FALSE == g_needTxMsg)
        {
            if(TRUE == TP_ReadDataInTransport(8u, g_linTxBuf, &tempMsgId, &tempMsgLength))
            {
                g_needTxMsg = TRUE;
            }
        }
    }

}

