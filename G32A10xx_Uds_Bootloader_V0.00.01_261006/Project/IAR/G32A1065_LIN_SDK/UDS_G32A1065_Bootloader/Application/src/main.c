#include "includes.h"
#include "lin_app.h"
#include "bootloader_main.h"
#include "TP.h"
#include "timer_hal.h"
#include "flash.h"

uint16_t timerOverflowInterruptCount = 0u;

uint8 g_linRxBuf[8u] = {0u};
uint8 g_linTxBuf[8u] = {0u};

void delay(int x)
{
    while(x--);
}

uint32_t LIN_GetTimerCallback(uint32_t *ns)
{
    static uint32_t previousCountValue = 0UL;

    uint32_t counterValue = 0U;
    *ns = ((uint32_t)(counterValue + timerOverflowInterruptCount * 2000U - previousCountValue)) * 1000UL / 4U;
    timerOverflowInterruptCount = 0UL;
    previousCountValue = counterValue;
    
    return 0UL;
}

void BSP_init(void)
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

int main(void)
{
    uint32 tempMsgId = 0u;
    uint32 tempMsgLength = 0u;

    udsTaskPrepare(&BSP_init, &LINBUSAbortTxMsg);

    APPDebugLog("Welcome enter LIN bootloader based on G32A10xx!\r\n");

    while(1)
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
