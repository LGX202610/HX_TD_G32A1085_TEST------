#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

#ifndef UDS_PROJECT_FOR_APP
#define UDS_PROJECT_FOR_APP
#endif

#define MCU_G32A10xx (1)
#define MCU_CORE_NUMBER (1u)
#define ALLOW_CAN_TP
#define ALLOW_CRC_SW
#define SA_ALGORITHM_SEED_SIZE (4u)
#define FALSH_MEMORY_CONTINUE (0u)
#define RECEIVE_BUS_FIFO_CHAR  ('r')
#define APP_INFORMATION_SIZE   (0x00000200u)

#define EnableAllInterrupts() ResumeAllInterrupts()
#define DisableAllInterrupts() SuspendAllInterrupts()
#define FLASH_ERASE_KEY       (0xA55AA55AU)

/* 串口调试打印：1=全开  0=全关。只改这一行 */
#define UDS_DEBUG_PRINTF  0
#if (UDS_DEBUG_PRINTF != 0)
#define ALLOW_APP_DEBUG
#define EN_TP_DEBUG
#define EN_UDS_APP_CFG_DEBUG
#define EN_DEBUG_FLS_MODULE
#endif

#define USE_CAN_EXT_ID
#define RECEIVE_ADDR       (0x18DAA1F1u)  /* 物理寻址请求，扩展帧 */
#define RECEIVE_FUN        (0x18DB33F1u)  /* 功能寻址请求，扩展帧 */
#define TRANSMIT_RESP      (0x18DAF1A1u)  /* 诊断响应发送，扩展帧 */

#define RECEIVE_BUS_FIFO_SIZE     (96u)
#define TRANSMIT_BUS_FIFO_CHAR     ('t')
#define TRANSMIT_BUS_FIFO_SIZE     (64u)

/* 与 boot 约定的 SRAM 握手区：写 0x5A 后复位，boot 留下不跳 APP */
#define INFO_BEGIN_ADDR     0x20007FF0u
#define APP_DL_SUC_ADDR     0x20007FF0u
#define REQ_ENTER_BL_ADDR   0x20007FF1u

typedef enum
{
    APP_A_ID = 0u,
    APP_USELESS_ID = 0xFFu,
} AppIdType;

#ifndef ASSERT
#define ASSERT(num)
#endif


#endif
