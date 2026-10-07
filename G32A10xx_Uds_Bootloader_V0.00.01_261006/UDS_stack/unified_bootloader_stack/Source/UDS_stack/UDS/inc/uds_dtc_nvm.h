#ifndef UDS_DTC_NVM_H
#define UDS_DTC_NVM_H

#include "includes.h"
#include <stddef.h>   // 提供 offsetof 宏

/* ================================================================
 * 1. DTC 故障码定义（诊断规范第8.1节）
 * ================================================================ */

/* ---- 网络总线关闭故障 (8.1.1) ---- */
#define DTC_U007388  0x007388  /* EX-CAN网络总线关闭故障 */
#define DTC_U007488  0x007488  /* COM-CAN网络总线关闭故障 */
#define DTC_U007688  0x007688  /* 诊断CAN网络总线关闭故障 */
#define DTC_U007A88  0x007A88  /* A-CANFD网络总线关闭故障 */
#define DTC_U007B88  0x007B88  /* B-CANFD网络总线关闭故障 */
#define DTC_U007C88  0x007C88  /* C-CANFD网络总线关闭故障 */
#define DTC_U007D88  0x007D88  /* P-CANFD网络总线关闭故障 */
#define DTC_U007E88  0x007E88  /* MRS-CANFD网络总线关闭故障 */
#define DTC_U007F88  0x007F88  /* BSDR-CANFD网络总线关闭故障 */

/* ---- 电压故障 (8.1.3.1) ---- */
#define DTC_B111716  0x111716  /* 控制器输入电压过低 */
#define DTC_B111717  0x111717  /* 控制器输入电压过高 */

/* ---- 安全/配置故障 ---- */
#define DTC_U1F0052  0x1F0052  /* 安全密钥未注入 (5.8.4.2) */
#define DTC_B112355  0x112355  /* 控制器功能配置未写入 (8.2.3) */

/* ---- 通信故障子类型 (8.1.2) ---- */
/* 这些需要配合报文ID组合使用，例如：0x401丢帧 = 0x040187 */
#define DTC_SUBTYPE_TIMEOUT     0x87  /* 丢帧/接收超时 */
#define DTC_SUBTYPE_CHECKSUM    0x83  /* 校验和错误 */
#define DTC_SUBTYPE_ROLLING     0x82  /* 生命信号更新错误 */

/* ================================================================
 * 2. F1ED 安全刷写失败原因 Bit 定义 (8.2.20)
 * ================================================================ */
#define F1ED_BIT_VERIFY_FAIL        0x01  /* Bit0: 文件合法性校验失败 */
#define F1ED_BIT_INTEGRITY_FAIL     0x02  /* Bit1: 数据完整性校验失败 */
#define F1ED_BIT_ERASE_FAIL         0x04  /* Bit2: 擦除失败 */
#define F1ED_BIT_DOWNLOAD_FAIL      0x08  /* Bit3: 下载失败 */

/* ================================================================
 * 3. F1EE 安全启动失败原因 Bit 定义 (8.2.21)
 * ================================================================ */
#define F1EE_BIT_PBL_VERIFY_FAIL    0x01  /* Bit0: 主引导程序校验失败 */

/* ================================================================
 * 4. FA19 密钥状态定义 (8.2.23)
 * ================================================================ */
#define FA19_STATUS_DEFAULT         0x00  /* 默认密钥（出厂状态） */
#define FA19_STATUS_REAL            0x01  /* 真实密钥已注入 */

/* ================================================================
 * 5. 结构体定义（同一DFlash页，出厂无魔数时 format 一次）
 * ================================================================ */
#define FAULT_DTC_MAX_COUNT         16    /* 最多存储16个DTC */

typedef struct {
    uint32_t dtc_list[FAULT_DTC_MAX_COUNT];  /* DTC故障码列表 */
    uint8_t  dtc_count;                       /* 当前DTC数量 */
    uint8_t  f1ed;                           /* F1ED 安全刷写失败原因 */
    uint8_t  f1ee;                           /* F1EE 安全启动失败原因 */
    uint8_t  fa19;                           /* FA19 密钥状态 */
    uint16_t program_success_cnt;            /* 0x0200 编程成功次数 */
    uint8_t  program_attempt_cnt;            /* 0x0201 尝试编程次数 */
    uint8_t  reserved;                       /* 对齐到 crc 前 */
    uint32_t crc;                            /* CRC32校验和 */
    uint32_t magic;                          /* 魔数 "FLT!" */
} FaultInfo_t;  /* 80 字节，与原 reserved[3] 布局兼容 */

/* ================================================================
 * 6. 页1地址定义（相对 DFlash 基址 0x08040000 的偏移 0x200）
 * ================================================================ */
#define NVM_PAGE1_OFFSET            0x200u
#define NVM_PAGE1_ADDR              (0x08040000u + NVM_PAGE1_OFFSET)
#define NVM_PAGE1_SIZE              0x200u          /* 512 字节 */
#define NVM_PAGE1_MAGIC_VALUE       0x46554C54u /* "FLT!" */

/* ================================================================
 * 7. 函数声明
 * ================================================================ */

/* 初始化故障管理模块（无魔数才 format；从DFlash加载） */
void FaultInfo_Init(void);

/* 获取结构体指针 */
FaultInfo_t* FaultInfo_GetPtr(void);

/* 将 RAM 工作副本写回同一页（读改擦写，喂狗） */
void FaultInfo_Flush(void);

/* 0x22/0x2E 服务接口 */
int FaultInfo_ReadDID(uint16_t did, uint8_t* data, uint8_t* len);
int FaultInfo_WriteDID(uint16_t did, const uint8_t* data, uint8_t len);

/* ---- F1ED 安全刷写失败原因 ---- */
void FaultInfo_SetF1ED(uint8_t bit);
void FaultInfo_ClearF1ED(uint8_t bit);
uint8_t FaultInfo_GetF1ED(void);
void FaultInfo_ResetF1ED(void); /* 新一次擦除开始：整字节清 0，不立刻 Flush */

/* ---- F1EE 安全启动失败原因 ---- */
void FaultInfo_SetF1EE(uint8_t bit);
void FaultInfo_ClearF1EE(uint8_t bit);
uint8_t FaultInfo_GetF1EE(void);

/* ---- FA19 密钥状态 ---- */
void FaultInfo_SetKeyStatus(uint8_t status);
uint8_t FaultInfo_GetKeyStatus(void);

/* ---- 0x0200 / 0x0201 ---- */
void FaultInfo_IncProgramAttempt(void);   /* 本编程周期首次擦除 +1，上限 255 */
void FaultInfo_OnProgramSuccess(void);    /* 刷写成功：0200+1，0201清零 */

/* ---- 0x85 DTC 置位开关（RAM，复位/默认会话恢复为开） ---- */
void FaultInfo_SetDtcSetting(uint8_t enable);
uint8_t FaultInfo_IsDtcSettingEnabled(void);

/* ---- DTC 故障码管理 ---- */
void FaultInfo_SetDTC(uint32_t dtc);
void FaultInfo_ClearDTC(uint32_t dtc);
uint8_t FaultInfo_IsDTCSet(uint32_t dtc);
void FaultInfo_ClearAllDTC(void);   /* 只清 DTC，保留 F1ED/F1EE/FA19/0200/0201 */
uint16_t FaultInfo_GetDTCList(uint32_t* dtc_list, uint16_t max_count);


#endif
