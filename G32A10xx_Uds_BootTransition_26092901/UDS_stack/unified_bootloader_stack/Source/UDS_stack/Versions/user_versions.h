#ifndef UDS_USER_VERSIONS_H_
#define UDS_USER_VERSIONS_H_

#include "stdint.h"
#include "CRC_hal.h"
#include "can_tp_cfg.h"

//  1. DFlash 基地址 lu-
#define NVM_BASE_ADDR                   0x08040000u      // DFlash起始地址
//  2. 页大小配置（G32A1085 DFlash页大小为512字节）
#define NVM_PAGE_SIZE                   0x200u           // 512字节
//  3. 数据偏移定义（从页开头偏移）
//     结构体放在页开头，CRC和魔数紧跟在后面
#define NVM_DATA_OFFSET                 0x0000u          // 结构体从页头开始
#define NVM_DATA_SIZE                   0x01F4u          // 前 500 字节留给结构体
#define NVM_MAGIC_OFFSET                0x01F4u          // 魔数：第 500 字节起
#define NVM_CRC_OFFSET                  0x01F8u          // CRC32 紧跟魔数
#define NVM_CRC_DATA_LEN                NVM_DATA_SIZE
#define NVM_MAGIC_VALUE                 0x4E564D21u      // "NVM!" 布局固定，只认这一个

// ===== 刷写规范要求的DID长度 =====
//  1. 各DID数据长度宏定义（依据诊断表8.2节）
// -------------------- 区块0：版本信息 --------------------
#define LEN_SW_VERSION          10u     // 0xF189 控制器软件版本号
#define LEN_SW_NUMBER           19u     // 0xF188 控制器软件号
#define LEN_PBL_VERSION         10u     // 0xF180 主引导程序版本号
#define LEN_CAL_VERSION         10u     // 0xF182 标定软件版本号
#define LEN_HW_VERSION          10u     // 0xF150 控制器硬件版本号
#define LEN_SUPPLIER_SW_VER     10u     // 0xF195 供应商软件版本号
#define LEN_SUPPLIER_HW_VER     10u     // 0xF193 供应商硬件版本号
#define LEN_DIAG_VERSION        4u      // 0xFF00 诊断版本号
#define LEN_SESSION             1u      // 0xF186 当前诊断会话
#define LEN_F18A_RSP            3u      // 0xF18A 调查表 3 字节（结构体仍 6 兼容旧 NVM）
#define LEN_F18C_RSP            29u     // 0xF18C 调查表 29 字节（结构体仍 30）
// -------------------- 区块1：产品标识 --------------------
#define LEN_PART_NUMBER         19u     // 0xF187 零部件号
#define LEN_PRODUCT_MODEL       16u     // 0xF197 产品型号
#define LEN_SUPPLIER_CODE       6u      // 0xF18A 供应商代码
// -------------------- 区块2：序列号/生产信息 --------------------
#define LEN_PCBA_SERIAL         29u     // 0xF191 PCBA序列号
#define LEN_ECU_SERIAL          30u     // 0xF18C 控制器序列号
#define LEN_PRODUCTION_DATE     4u      // 0xF18B 控制器生产日期（BCD格式）
#define LEN_VIN                 17u     // 0xF190 车辆识别码（VIN）
#define LEN_BOOT_APP_ID         1u      // 0xF1EF Boot&App 区分标识（1 Byte Hex）
#define VAL_BOOT_APP_ID_APP     0x00u   // 0xF1EF：当前运行 APP
#define VAL_BOOT_APP_ID_BOOT    0x01u   // 0xF1EF：当前运行 Boot
// -------------------- 区块3：编程记录 --------------------
#define LEN_TOOL_SERIAL         16u     // 0xF198 诊断工具序列号
#define LEN_REPROG_DATE         4u      // 0xF199 重编程日期（BCD格式）
#define LEN_PROG_SUCCESS        2u      // 0x0200 编程成功次数
#define LEN_PROG_ATTEMPT        1u      // 0x0201 尝试编程次数

// ===== 版本信息结构体（暂时只包含刷写规范里的内容） =====
//  2. FlashInfo_t 结构体定义（连续内存布局）
//==========================================================================
typedef struct {

    // ======== 区块0：版本信息 ========
    char     sw_version[LEN_SW_VERSION];           // 0xF189 控制器软件版本号
    char     sw_number[LEN_SW_NUMBER];             // 0xF188 控制器软件号
    char     pbl_version[LEN_PBL_VERSION];         // 0xF180 主引导程序版本号
    char     calibration_version[LEN_CAL_VERSION]; // 0xF182 标定软件版本号
    char     hw_version[LEN_HW_VERSION];           // 0xF150 控制器硬件版本号
    uint8_t  diag_version[LEN_DIAG_VERSION];       // 0xFF00 诊断版本号

    // ======== 区块1：产品标识 ========
    char     part_number[LEN_PART_NUMBER];         // 0xF187 零部件号
    char     product_model[LEN_PRODUCT_MODEL];     // 0xF197 产品型号
    char     supplier_code[LEN_SUPPLIER_CODE];     // 0xF18A 供应商代码

    // ======== 区块2：序列号/生产信息 ========
    char     pcba_serial[LEN_PCBA_SERIAL];         // 0xF191 PCBA序列号
    char     ecu_serial[LEN_ECU_SERIAL];           // 0xF18C 控制器序列号
    uint8_t  production_date[LEN_PRODUCTION_DATE]; // 0xF18B 生产日期(BCD)
    char     vin[LEN_VIN];                         // 0xF190 VIN码

    // ======== 区块3：编程记录 ========
    char     tool_serial[LEN_TOOL_SERIAL];         // 0xF198 诊断工具序列号
    uint8_t  reprogram_date[LEN_REPROG_DATE];      // 0xF199 重编程日期(BCD)
    uint16_t program_success_cnt;                  // 0x0200 编程成功次数
    uint8_t  program_attempt_cnt;                  // 0x0201 尝试编程次数
    uint8_t  boot_app_id;                          // 0xF1EF Boot&App 区分标识（不作为 NVM 身份源）
    char     supplier_sw_version[LEN_SUPPLIER_SW_VER]; // 0xF195 跟 APP hex
    char     supplier_hw_version[LEN_SUPPLIER_HW_VER]; // 0xF193 跟 APP hex

//    // ======== 区块4：密钥/安全状态 ========
//    uint8_t  key_status;                           // 0xFA19 27服务密钥状态
//    uint8_t  secure_fail_reason;                   // 0xF1ED 安全刷写失败原因
//    uint8_t  boot_fail_reason;                     // 0xF1EE 安全启动失败原因
}ECU_Information_t;

/* DID 数据标识符接口：Did_ 前缀，单词首字母大写、下划线分隔 */
void Did_Info_Init(void);                          /* 加载 DID 默认值与 NVM */
ECU_Information_t* Did_Info_Get(void);             /* 取运行时 DID 结构体 */
ECU_Information_t* Did_Info_Get_Def(void);         /* 取编译期 DID 默认表 */
uint16_t Did_Info_Get_Size(void);                  /* DID 结构体字节数 */
void Did_Nvm_Read(uint32_t offset, uint8_t* data, uint16_t len); /* 读 DFlash */
int Did_Nvm_Chk_Crc(void);                         /* 校验 DID 页 CRC */
uint8_t Did_Nvm_Write(uint32_t offset, const uint8_t* data, uint8_t len); /* 写 DFlash */
int Did_Nvm_Upd_Crc(void);                         /* 更新 DID 页 CRC */
void Did_Nvm_Format(void);                         /* 按默认值格式化 DID 页 */
int Did_Nvm_Init(void);                            /* 上电初始化 DID NVM */
int Did_Read(uint16_t did, uint8_t* data, TP_LengthType* len); /* 0x22 读 DID */
int Did_Write(uint16_t did, const uint8_t* data, uint8_t len); /* 0x2E 写 DID */
int Did_Upd_Sw_Ver(const uint8_t *version);        /* 更新 F189 软件版本 */
void Did_Flush(void);                              /* 脏 DID 数据刷回 NVM */
void Did_Nvm_Wr_Clear_Test(void);                        /* DFlash 清除Dflash页0和页1，写测试 */


#endif  //UDS_USER_VERSIONS_H_
