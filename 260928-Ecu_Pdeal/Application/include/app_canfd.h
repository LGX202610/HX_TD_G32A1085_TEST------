#ifndef APP_CANFD_H
#define APP_CANFD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>
#include "user_config.h"



/* Define */

/* 接收 ID */
#define CAN_RX_FRAME_ID_0			(0x401)			// CANNM
#define CAN_RX_FRAME_ID_1			(0x100)			// 
#define CAN_RX_FRAME_ID_2			RECEIVE_ADDR	// UDS physical 扩展帧
#define CAN_RX_FRAME_ID_3			RECEIVE_FUN		// UDS functional 扩展帧
#define CAN_RX_FRAME_ID_4			TRANSMIT_RESP	// UDS response 扩展帧 

#define CAN_RX_FRAME_ID_5			(0x3B7)			// 车速/档位/门状态
#define CAN_RX_FRAME_ID_6			(0x3B9)			// 碰撞
#define CAN_RX_FRAME_ID_7			(0x220)			// 洗车
#define CAN_RX_FRAME_ID_8			(0x22C)			// 解设防
#define CAN_RX_FRAME_ID_9			(0x3CF)			// 自动开启/临时开启
#define CAN_RX_FRAME_ID_10		(0x5F5)			// IVI
#define CAN_RX_FRAME_ID_11		(0x510)			// 踏板状态
#define CAN_RX_FRAME_ID_12		(0x511)			// 踏板状态

/* 接收 BUF */
#define CAN_RX_BUF_ID_0				(0U)
#define CAN_RX_BUF_ID_1				(1U)
#define CAN_RX_BUF_ID_2				(2U)
#define CAN_RX_BUF_ID_3				(3U)
#define CAN_RX_BUF_ID_4				(4U)
#define CAN_RX_BUF_ID_5				(5U)
#define CAN_RX_BUF_ID_6				(6U)
#define CAN_RX_BUF_ID_7				(7U)
#define CAN_RX_BUF_ID_8				(8U)
#define CAN_RX_BUF_ID_9				(9U)
#define CAN_RX_BUF_ID_10			(10U)
#define CAN_RX_BUF_ID_11			(11U)
#define CAN_RX_BUF_ID_12			(12U)


/* 发送 ID */
#define CAN_TX_FRAME_ID_510			(0x510u)		// 踏板运行状态周期上报
#define CAN_TX_FRAME_ID_511			(0x511u)		// 踏板运行状态周期上报
#define CAN_TX_FRAME_ID_650			(0x650u)		// 碰撞状态周期上报
#define CAN_TX_FRAME_ID_62D			(0x62Du)		// 碰撞状态周期上报
//#define CAN_TX_FRAME_ID_0			(0x100)		// CANNM
//#define CAN_TX_FRAME_ID_1			(0x101)		// UDS
//#define CAN_TX_FRAME_ID_2			(0x102)
//#define CAN_TX_FRAME_ID_3			(0x103)
//#define CAN_TX_FRAME_ID_4			(0x104)
//#define CAN_TX_FRAME_ID_5			(0x105)
//#define CAN_TX_FRAME_ID_6			(0x106)
//#define CAN_TX_FRAME_ID_7			(0x107)
//#define CAN_TX_FRAME_ID_8			(0x108)
//#define CAN_TX_FRAME_ID_9			(0x109)

/* 发送 BUF */
#define CAN_TX_BUF_ID_0				(0U)
#define CAN_TX_BUF_ID_1				(1U)
#define CAN_TX_BUF_ID_2				(2U)
#define CAN_TX_BUF_ID_3				(3U)
#define CAN_TX_BUF_ID_4				(4U)
#define CAN_TX_BUF_ID_5				(5U)
#define CAN_TX_BUF_ID_6				(6U)
#define CAN_TX_BUF_ID_7				(7U)
#define CAN_TX_BUF_ID_8				(8U)
#define CAN_TX_BUF_ID_9				(9U)






/* Enum */ 





/* Struct */

typedef void (*App_CanCyclicPackFn)(uint8_t *data, uint8_t dlc);

typedef struct
{
	uint32_t				id;				/* CAN ID */
	uint8_t					box;			/* 发送邮箱 */
	uint8_t					frame_type;		/* CAN_FRAME_TYPE_STD / CAN_FRAME_TYPE_EXT */
	uint8_t					dlc;			/* 数据长度 1~8 */
	uint16_t				period_10ms;	/* 发送周期，单位 10ms，例如 10=100ms */
	uint16_t				offset_10ms;	/* 相位偏移，单位 10ms，错开多帧发送时刻 */
	uint8_t					enable;			/* 1=参与周期发送，0=关闭 */
	App_CanCyclicPackFn		pack;			/* 组包回调，在发送前填充 data[] */
} App_CanCyclicPduCfg_t;


void app_can_init(void);
void app_can_cyclic_init(void);
void app_can_cyclic_main(void);
void app_can_cyclic_set_enable(uint8_t index, uint8_t enable);

int app_can_tx_fram(uint32_t box, uint32_t id, uint8_t frame_type, uint32_t len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif /*  */
