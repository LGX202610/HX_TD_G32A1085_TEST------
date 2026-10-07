#ifndef HAL_CANFD_H
#define HAL_CANFD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>
#include <stdbool.h>


/* Define */
#define CAN_RX_FRAME_ID_MAX				CAN_RX_BUF_ID_MAX			//可注册ID最大数量
#define CAN_RX_FRAME_DATA_SIZE		8			//每路接收快照最大长度
#define CAN_TX_FRAME_DATA_SIZE		8			//每路发送快照最大长度



/* Enum */ 
#define CAN_FRAME_TYPE_STD				0		//标准帧，需与drv对齐
#define CAN_FRAME_TYPE_EXT				1		//扩展帧，需与drv对齐


/* Struct */ 
typedef void (*Hal_Can_RxCallback_t)(uint32_t id, uint32_t len, uint8_t *data);
typedef void (*Hal_Can_TxIrqHookFunc_t)(uint8_t box, uint32_t id);
typedef void (*Hal_Can_ErrorIrqHookFunc_t)(void);
typedef void (*Hal_Can_WakeupIrqHookFunc_t)(void);


typedef struct
{
	uint8_t							box;				//邮箱
	
	uint32_t						id;					//帧id
	uint32_t						len;				//长度
	uint8_t							data[CAN_RX_FRAME_DATA_SIZE];
	
	Hal_Can_RxCallback_t 			callback;
} Can_RxFram;			


typedef struct
{
	uint8_t					box;					//邮箱	
	uint32_t				id;						//帧id
	uint32_t				len;					//长度
	uint8_t					data[CAN_TX_FRAME_DATA_SIZE];				//数据	
} Can_TxFram;	



void hal_can_init(void);
void hal_can_enable(void);
void hal_can_sleep(void);
void hal_can_wakeup(void);
void hal_register_can_tx_irq_hook(Hal_Can_TxIrqHookFunc_t hook_func);
void hal_register_can_error_irq_hook(Hal_Can_ErrorIrqHookFunc_t hook_func);
void hal_register_can_wakeup_irq_hook(Hal_Can_WakeupIrqHookFunc_t hook_func);
bool hal_can_get_wakeup_flag(void);
void hal_can_clear_wakeup_flag(void);
int hal_can_tx_fram(uint32_t box, uint32_t id, uint8_t frame_type, uint32_t len, uint8_t *data);	

uint8_t hal_register_can_rx_callback(uint32_t id, uint8_t frame_type, Hal_Can_RxCallback_t callback);


#ifdef __cplusplus
}
#endif

#endif /*  */
