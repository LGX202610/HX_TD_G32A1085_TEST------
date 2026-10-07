#ifndef DRV_CANFD_H
#define DRV_CANFD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>

/* Define */

#define CANFD_USED				1		//1 CAN；2 CANFD		
#if(CANFD_USED==1)

#define	CAN_ARB_BAUDRATE				500000U
#define	CAN_DATA_BAUDRATE				500000U
#define CAN_DATA_SIZE           (8U)

#elif(CANFD_USED==2)

#define	CANFD_ARB_BAUDRATE			500000U
#define	CANFD_DATA_BAUDRATE			5000000U
#define CAN_DATA_SIZE           (64U)

#endif



#define CAN_RX_STD_LIST_MAX					10		//标准帧接收滤波器数量
#define CAN_RX_EXT_LIST_MAX					10		//扩展帧接收滤波器数量

#define CAN_RX_BUF_ID_MAX					(CAN_RX_STD_LIST_MAX + CAN_RX_EXT_LIST_MAX)		//接收邮箱数量
#define CAN_TX_BUF_ID_MAX					10		//发送邮箱数量

#define DRV_CAN_FRAME_STD_ID				(0U)	/* match CAN_FRAME_STD_ID */
#define DRV_CAN_FRAME_EXT_ID				(1U)	/* match CAN_FRAME_EXT_ID */


/*************** RAM分配 ***************/
#define CAN_SRAM_SIZE                  (0x800U)			//RAM大小，2kb
#define STD_FILTER_OFS                 (0x000U)			//标准帧，4byte * 20 = 0x50
#define EXT_FILTER_OFS                 (0x50U)			//拓展帧，STD_FILTER_OFS + 8byte * 20 = 0xF0
#define RX_BUFFER_OFS                  (0xF0U)			//接收邮箱，EXT_FILTER_OFS + 16byte * 20 = 0x230
#define TX_BUFFER_OFS                  (0x230U)			//发送邮箱，RX_BUFFER_OFS + 16byte * 20 = 0x370





#if ((CAN_RX_STD_LIST_MAX + CAN_RX_EXT_LIST_MAX) != CAN_RX_BUF_ID_MAX)
#error "CAN_RX_BUF_ID_MAX must equal CAN_RX_STD_LIST_MAX+CAN_RX_EXT_LIST_MAX"
#endif


/* Enum */ 





/* Struct */ 













typedef void (*Drv_Can_RxIrqHookFunc_t)(uint8_t hw_box, uint32_t id, uint32_t len, uint8_t *data);
typedef void (*Drv_Can_TxIrqHookFunc_t)(uint8_t hw_box, uint32_t id, uint32_t len, uint8_t *data);
typedef void (*Drv_Can_ErrorIrqHookFunc_t)(uint8_t hw_box, uint32_t id, uint32_t len, uint8_t *data);
typedef void (*Drv_Can_WakeupIrqHookFunc_t)(void);
void drv_register_can_rx_irq_hook(Drv_Can_RxIrqHookFunc_t hook_func);
void drv_register_can_tx_irq_hook(Drv_Can_TxIrqHookFunc_t hook_func);
void drv_register_can_error_irq_hook(Drv_Can_ErrorIrqHookFunc_t hook_func);
void drv_register_can_wakeup_irq_hook(Drv_Can_WakeupIrqHookFunc_t hook_func);

void drv_can_init(void);
void drv_can_sleep(void);
void drv_can_wakeup(void);
void drv_can_irq_handle(void);
void drv_can_serm_irq_handle(void);


int drv_alloc_can_rx_box(uint32_t filter_id, uint8_t frame_type);
void drv_can_enable(void);
void drv_can_disable(void);

int drv_can_tx_fram(uint32_t box, uint32_t id, uint8_t frame_type, uint32_t len, uint8_t *data);
void drv_can_tx_reset(void);


#ifdef __cplusplus
}
#endif

#endif /*  */
