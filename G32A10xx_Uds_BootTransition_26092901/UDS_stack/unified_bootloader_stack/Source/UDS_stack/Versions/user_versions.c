#include "user_versions.h"
#include "g32a10xx_fls.h"
#include "uds_dtc_nvm.h"
#include "flash_hal.h"
#include "watchdog_hal.h"
#include "uds_app_cfg.h"
#include <string.h>
#include <stddef.h>


//  1. 编译时固定的默认值（存放在PFlash，只读 lu-）
static const ECU_Information_t g_ECU_Info_default  = {

    // ---------- 区块0：版本信息 ----------
    .sw_version           = "1.01.05",
    .sw_number            = "LP-EPS051-AA",
    .pbl_version          = "0.00.01",
    .calibration_version  = "0.00.00",
    .hw_version           = "0.17.00",
    .diag_version         = {0x02,0x01,0x03,0x02},

    // ---------- 区块1：产品标识 ----------
    .part_number          = "LP-EPS051-AA-01",
    .product_model        = "LP-EPM060-AA",
    .supplier_code        = "3FA",

    // ---------- 区块2：序列号/生产信息 ----------
    .pcba_serial          = {0},
    .ecu_serial           = {0},
    .production_date      = {0x20,0x23,0x05,0x08},
    .vin                  = {0},

    // ---------- 区块3：编程记录 ----------
    .tool_serial          = {0},
    .reprogram_date       = {0},
    .program_success_cnt  = 0,
    .program_attempt_cnt  = 0,
    .boot_app_id          = VAL_BOOT_APP_ID_BOOT,  /* 0xF1EF：Boot 固定 0x01 */
    .supplier_sw_version  = "0.00.01",
    .supplier_hw_version  = "0.17.00",

};

// 运行时结构体 lu-
static ECU_Information_t g_ECU_Info_running;

/**********************************************************
 * @brief  擦一页 DFlash（512B），清掉旧 APP 留下的版本
 **********************************************************/
static void nvm_erase_one_page(uint32_t page_addr)
{
    Fmc_StateType ret;

    WATCHDOG_HAL_Fed();
    DisableAllInterrupts();
    Fmc_Unlock();
    FLASH_HAL_ClearFmcErrorFlags();
    ret = Fmc_ErasePage(page_addr);
    Fmc_Lock();
    EnableAllInterrupts();
    WATCHDOG_HAL_Fed();

    if (FMC_STATE_COMPLETE != ret)
    {
        UDSCFGDebugLog("Transition erase 0x%x fail %d\r\n", page_addr, ret);
    }
}

// ==================== 初始化函数 ====================
void Did_Info_Init(void) {
    /* 上电擦页0(F189 等)和页1，新 Boot 版本低时 6001 才不会被旧 APP 版本拦住 */
    nvm_erase_one_page(NVM_BASE_ADDR);
    nvm_erase_one_page(NVM_PAGE1_ADDR);

    memcpy(&g_ECU_Info_running, &g_ECU_Info_default, sizeof(ECU_Information_t));
    g_ECU_Info_running.boot_app_id = VAL_BOOT_APP_ID_BOOT;
}

ECU_Information_t* Did_Info_Get(void)
{
    return &g_ECU_Info_running;
}
ECU_Information_t* Did_Info_Get_Def(void)
{
    return (ECU_Information_t*)&g_ECU_Info_default;
}
//==========================================================================
//  5. 获取结构体大小
//==========================================================================
uint16_t Did_Info_Get_Size(void)
{
    return sizeof(ECU_Information_t);
}
void Did_Nvm_Read(uint32_t offset, uint8_t* data, uint16_t len)
{
    uint32_t addr = NVM_BASE_ADDR + offset;
    uint16_t i;
    for (i = 0; i < len; i++)
	{
        data[i] = *(uint8_t*)(addr + i);
    }
}
int Did_Nvm_Chk_Crc(void)
{
    uint32_t stored_crc, calc_crc;

    Did_Nvm_Read(NVM_CRC_OFFSET, (uint8_t*)&stored_crc, 4);
    calc_crc = crc32_calc((const uint8_t*)NVM_BASE_ADDR, NVM_CRC_DATA_LEN);
    UDSCFGDebugLog("Version stored_crc = %d, calc_crc = %d\r\n",stored_crc,calc_crc);
    return (stored_crc == calc_crc) ? 0 : -1;
}
void Did_Nvm_Wr_Clear_Test(void)
{
    /* 页0/页1 已在 Did_Info_Init 擦除 */
}

uint8_t Did_Nvm_Write(uint32_t offset, const uint8_t* data, uint8_t len)
{
    (void)offset;
    (void)data;
    (void)len;
    /* 过渡程序不写 DFlash DID */
    return 0u;
}

int Did_Nvm_Upd_Crc(void)
{
    return 0;
}

void Did_Nvm_Format(void)
{
    /* 过渡程序不 Format DID 页 */
}
int Did_Nvm_Init(void)
{
    /* 过渡程序不 Format DID 页 */
    return 0;
}

//  0x22服务：根据DID读取数据
int Did_Read(uint16_t did, uint8_t* data, TP_LengthType* len)
{
    ECU_Information_t* info = Did_Info_Get();

    switch (did)
    {
        // ========== 区块0：版本信息 ==========
        case 0xF189:
            *len = LEN_SW_VERSION;
            (void)memcpy(data, info->sw_version, LEN_SW_VERSION);
            break;
        case 0xF188:
            *len = LEN_SW_NUMBER;
            (void)memcpy(data, info->sw_number, LEN_SW_NUMBER);
            break;
        case 0xF180:
            *len = LEN_PBL_VERSION;
            (void)memcpy(data, info->pbl_version, LEN_PBL_VERSION);
            break;
        case 0xF150:
            *len = LEN_HW_VERSION;
            (void)memcpy(data, info->hw_version, LEN_HW_VERSION);
            break;
        case 0xFF00:
            *len = LEN_DIAG_VERSION;
            (void)memcpy(data, info->diag_version, LEN_DIAG_VERSION);
            break;

        // ========== 区块1：产品标识 ==========
        case 0xF187:
            *len = LEN_PART_NUMBER;
            (void)memcpy(data, info->part_number, LEN_PART_NUMBER);
            break;
        case 0xF197:
            *len = LEN_PRODUCT_MODEL;
            (void)memcpy(data, info->product_model, LEN_PRODUCT_MODEL);
            break;
        case 0xF18A:
            *len = LEN_F18A_RSP;
            (void)memcpy(data, info->supplier_code, LEN_F18A_RSP);
            break;

        // ========== 区块2：序列号/生产信息 ==========
        case 0xF191:
            *len = LEN_PCBA_SERIAL;
            (void)memcpy(data, info->pcba_serial, LEN_PCBA_SERIAL);
            break;
        case 0xF18C:
            *len = LEN_F18C_RSP;
            (void)memcpy(data, info->ecu_serial, LEN_F18C_RSP);
            break;
        case 0xF18B:
            *len = LEN_PRODUCTION_DATE;
            (void)memcpy(data, info->production_date, LEN_PRODUCTION_DATE);
            break;
        case 0xF1EF:
            /* Boot&App 区分标识：Boot 固定回 0x01，不读 NVM */
            *len = LEN_BOOT_APP_ID;
            data[0] = VAL_BOOT_APP_ID_BOOT;
            break;
        case 0xF186:
            *len = LEN_SESSION;
            data[0] = UDS_APP_GetF186Session();
            break;
        case 0xF195:
            *len = LEN_SUPPLIER_SW_VER;
            (void)memcpy(data, info->supplier_sw_version, LEN_SUPPLIER_SW_VER);
            break;
        case 0xF193:
            *len = LEN_SUPPLIER_HW_VER;
            (void)memcpy(data, info->supplier_hw_version, LEN_SUPPLIER_HW_VER);
            break;

        // ========== 区块3：编程记录 ==========
        case 0xF198:
            *len = LEN_TOOL_SERIAL;
            (void)memcpy(data, info->tool_serial, LEN_TOOL_SERIAL);
            break;
        case 0xF199:
            *len = LEN_REPROG_DATE;
            (void)memcpy(data, info->reprogram_date, LEN_REPROG_DATE);
            break;
        case 0x0200:
        case 0x0201:
        case 0xF1ED:
        case 0xF1EE:
        case 0xFA19:
        {
            uint8_t local_len = 0u;
            if (FaultInfo_ReadDID(did, data, &local_len) != 0) {
                return -1;
            }
            *len = local_len;
            break;
        }

        default:
            return -1;   // NRC: 0x31 RequestOutOfRange
    }

    return 0;
}

int Did_Upd_Sw_Ver(const uint8_t *version)
{
    (void)version;
    /* 过渡程序不改 F189 */
    return 0;
}

void Did_Flush(void)
{
    /* 过渡程序不把 DID 写回 DFlash */
    return;
}

/**
 * @brief 0x2E服务：根据DID写入数据
 * @param did 数据标识符
 * @param data 要写入的数据
 * @param len 数据长度
 * @return 0=成功，-1=不支持的DID，-2=长度错误
 * @note 写入完成后自动将整个结构体写回DFlash并更新CRC
 */
int Did_Write(uint16_t did, const uint8_t* data, uint8_t len)
{
    ECU_Information_t* info = Did_Info_Get();

    if ((NULL_PTR == info) || (NULL_PTR == data))
    {
        return -1;
    }

    /* 只收 F198/F199 到 RAM，满足刷写顺序；不写 DFlash */
    switch (did)
    {
        case 0xF198:
            if (len != LEN_TOOL_SERIAL)
            {
                return -2;
            }
            (void)memcpy(info->tool_serial, data, LEN_TOOL_SERIAL);
            break;
        case 0xF199:
            if (len != LEN_REPROG_DATE)
            {
                return -2;
            }
            (void)memcpy(info->reprogram_date, data, LEN_REPROG_DATE);
            break;
        default:
            return -1;
    }
    return 0;
}





