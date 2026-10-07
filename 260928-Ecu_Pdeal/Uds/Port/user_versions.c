#include "user_versions.h"
#include "uds_dtc_nvm.h"
#include "uds_app_cfg.h"
#include "g32a10xx_fls.h"
#include <string.h>
#include <stddef.h>

static const ECU_Information_t g_ECU_Info_default =
{
    .sw_version           = "0.00.01",
    .sw_number            = "LP-EPS051-AA",
    .pbl_version          = "0.00.01",
    .calibration_version  = "0.00.00",
    .hw_version           = "0.17.00",
    .diag_version         = {0x02, 0x01, 0x03, 0x02},
    .part_number          = "LP-EPS051-AA-01",
    .product_model        = "LP-EPM060-AA",
    .supplier_code        = "3FA",
    .pcba_serial          = {0},
    .ecu_serial           = {0},
    .production_date      = {0x20, 0x26, 0x09, 0x29},
    .vin                  = {0},
    .tool_serial          = {0},
    .reprogram_date       = {0},
    .program_success_cnt  = 0,
    .program_attempt_cnt  = 0,
    .boot_app_id          = VAL_BOOT_APP_ID_APP,  /* 0xF1EF 默认 0x00：APP */
    .supplier_sw_version  = "0.00.01",            /* 0xF195 */
    .supplier_hw_version  = "0.17.00",            /* 0xF193 */
};

static ECU_Information_t g_ECU_Info_running;

/**********************************************************
  * @brief  清除 FMC 残留错误标志，避免挡住后续擦写
 **********************************************************/
static void nvm_clear_fmc_errors(void)
{
    uint32_t guard = 0x00002000u;

    while (((FMC->STS_R.STS & 0x01u) != 0u) && (0u != guard))
    {
        guard--;
    }
    FMC->STS_R.STS = 0x0001003Cu;
}

/**********************************************************
  * @brief  hex 字段跟当前 APP 镜像；工厂字段只从 NVM 恢复
 **********************************************************/
static uint8_t s_did_crc_dirty = 0u;

static void Did_LoadField(uint32_t off, void *ram, uint8_t len)
{
    Did_Nvm_Read(off, (uint8_t *)ram, len);
}

static void Did_SyncHexField(uint32_t off, void *ram, const void *hex, uint8_t len)
{
    uint8_t nvm[32];

    if (len > (uint8_t)sizeof(nvm))
    {
        return;
    }
    Did_Nvm_Read(off, nvm, len);
    (void)memcpy(ram, hex, len);
    if (0 != memcmp(nvm, hex, len))
    {
        (void)Did_Nvm_Write(off, (const uint8_t *)hex, len);
        s_did_crc_dirty = 1u;
    }
}

/**********************************************************
  * @brief  初始化：工厂项从 DFlash 恢复，版本项跟当前 APP hex
 **********************************************************/
void Did_Info_Init(void)
{
    uint32_t magic;
    const ECU_Information_t *def = &g_ECU_Info_default;
    ECU_Information_t *run = &g_ECU_Info_running;

    s_did_crc_dirty = 0u;
    (void)memcpy(run, def, sizeof(ECU_Information_t));

    Did_Nvm_Read(NVM_MAGIC_OFFSET, (uint8_t *)&magic, 4u);

    if (magic != NVM_MAGIC_VALUE)
    {
        /* 空片：用 hex 默认当初值写入 */
        (void)Did_Nvm_Write(NVM_DATA_OFFSET, (uint8_t *)run, (uint8_t)sizeof(*run));
        magic = NVM_MAGIC_VALUE;
        (void)Did_Nvm_Write(NVM_MAGIC_OFFSET, (uint8_t *)&magic, 4u);
        (void)Did_Nvm_Upd_Crc();
    }
    else
    {
        /* 工厂固化 / 编程记录：不跟 APP hex */
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, sw_number), run->sw_number, (uint8_t)LEN_SW_NUMBER);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, part_number), run->part_number, (uint8_t)LEN_PART_NUMBER);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, pcba_serial), run->pcba_serial, (uint8_t)LEN_PCBA_SERIAL);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, ecu_serial), run->ecu_serial, (uint8_t)LEN_ECU_SERIAL);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, production_date), run->production_date, (uint8_t)LEN_PRODUCTION_DATE);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, vin), run->vin, (uint8_t)LEN_VIN);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, tool_serial), run->tool_serial, (uint8_t)LEN_TOOL_SERIAL);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, reprogram_date), run->reprogram_date, (uint8_t)LEN_REPROG_DATE);
        Did_LoadField((uint32_t)offsetof(ECU_Information_t, pbl_version), run->pbl_version, (uint8_t)LEN_PBL_VERSION);

        /* 跟 APP hex，写回 DFlash 给 Boot 22 读 */
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, sw_version), run->sw_version, def->sw_version, (uint8_t)LEN_SW_VERSION);
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, product_model), run->product_model, def->product_model, (uint8_t)LEN_PRODUCT_MODEL);
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, diag_version), run->diag_version, def->diag_version, (uint8_t)LEN_DIAG_VERSION);
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, calibration_version), run->calibration_version, def->calibration_version, (uint8_t)LEN_CAL_VERSION);
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, supplier_code), run->supplier_code, def->supplier_code, (uint8_t)LEN_SUPPLIER_CODE);
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, hw_version), run->hw_version, def->hw_version, (uint8_t)LEN_HW_VERSION);
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, supplier_sw_version), run->supplier_sw_version, def->supplier_sw_version, (uint8_t)LEN_SUPPLIER_SW_VER);
        Did_SyncHexField((uint32_t)offsetof(ECU_Information_t, supplier_hw_version), run->supplier_hw_version, def->supplier_hw_version, (uint8_t)LEN_SUPPLIER_HW_VER);

        if (0u != s_did_crc_dirty)
        {
            (void)Did_Nvm_Upd_Crc();
        }
    }

    run->boot_app_id = VAL_BOOT_APP_ID_APP;
}

/**********************************************************
  * @brief  获取运行时 ECU 信息
 **********************************************************/
ECU_Information_t *Did_Info_Get(void)
{
    return &g_ECU_Info_running;
}

ECU_Information_t *Did_Info_Get_Def(void)
{
    return (ECU_Information_t *)&g_ECU_Info_default;
}

uint16_t Did_Info_Get_Size(void)
{
    return (uint16_t)sizeof(ECU_Information_t);
}

void Did_Nvm_Read(uint32_t offset, uint8_t *data, uint16_t len)
{
    uint32_t addr = NVM_BASE_ADDR + offset;
    uint16_t i;

    if (data == NULL)
    {
        return;
    }
    for (i = 0u; i < len; i++)
    {
        data[i] = *(uint8_t *)(addr + i);
    }
}

int Did_Nvm_Chk_Crc(void)
{
    uint32_t stored_crc;
    uint32_t calc_crc;
    uint32_t magic;

    Did_Nvm_Read(NVM_MAGIC_OFFSET, (uint8_t *)&magic, 4u);
    if (magic != NVM_MAGIC_VALUE)
    {
        return -1;
    }

    Did_Nvm_Read(NVM_CRC_OFFSET, (uint8_t *)&stored_crc, 4u);
    calc_crc = crc32_calc((const uint8_t *)NVM_BASE_ADDR, NVM_CRC_DATA_LEN);
    return (stored_crc == calc_crc) ? 0 : -1;
}

uint8_t Did_Nvm_Write(uint32_t offset, const uint8_t *data, uint8_t len)
{
    uint8_t write_sta = 0u;
    Fmc_StateType ret;
    uint32_t i;
    uint32_t addr;
    uint32_t *p_data;
    const uint32_t words_per_chunk = 16u;
    const uint32_t bytes_per_chunk = 64u;
    const uint32_t chunks = NVM_PAGE_SIZE / bytes_per_chunk;
    uint8_t buffer[NVM_PAGE_SIZE] __attribute__((aligned(4)));

    if ((data == NULL) || ((offset + (uint32_t)len) > NVM_PAGE_SIZE))
    {
        return 1u;
    }

    (void)memcpy(buffer, (const void *)NVM_BASE_ADDR, NVM_PAGE_SIZE);
    (void)memcpy(&buffer[offset], data, len);

    DisableAllInterrupts();
    Fmc_Unlock();
    nvm_clear_fmc_errors();
    ret = Fmc_ErasePage(NVM_BASE_ADDR);
    if (FMC_STATE_COMPLETE != ret)
    {
        write_sta = 1u;
    }
    if (0u == write_sta)
    {
        p_data = (uint32_t *)buffer;
        addr = NVM_BASE_ADDR;
        for (i = 0u; i < chunks; i++)
        {
            if (FMC_STATE_COMPLETE != Fmc_ProgramWord(addr, (uint32_t)FMC_LEN_512BIT, p_data))
            {
                write_sta++;
            }
            addr += bytes_per_chunk;
            p_data += words_per_chunk;
        }
    }
    Fmc_Lock();
    EnableAllInterrupts();

    return write_sta;
}

int Did_Nvm_Upd_Crc(void)
{
    uint32_t crc;
    uint8_t ret;

    crc = crc32_calc((const uint8_t *)NVM_BASE_ADDR, NVM_CRC_DATA_LEN);
    ret = Did_Nvm_Write(NVM_CRC_OFFSET, (uint8_t *)&crc, 4u);
    return (ret != 0u) ? (int)ret : 0;
}

void Did_Nvm_Format(void)
{
}

int Did_Nvm_Init(void)
{
    return 0;
}

void Did_Nvm_Wr_Test(void)
{
}

/**********************************************************
  * @brief  将 RAM 中的 VIN 写回 DFlash 页0，并更新 CRC/魔数
 **********************************************************/
static int Did_Save_Vin(void)
{
    uint8_t nvm_ret;
    uint32_t magic = NVM_MAGIC_VALUE;

    nvm_ret = Did_Nvm_Write((uint32_t)offsetof(ECU_Information_t, vin),
                        (const uint8_t *)g_ECU_Info_running.vin,
                        (uint8_t)LEN_VIN);
    if (0u != nvm_ret)
    {
        return -1;
    }

    nvm_ret = Did_Nvm_Write(NVM_MAGIC_OFFSET, (uint8_t *)&magic, 4u);
    if (0u != nvm_ret)
    {
        return -1;
    }

    if (Did_Nvm_Upd_Crc() != 0)
    {
        return -1;
    }
    return 0;
}

/**********************************************************
  * @brief  0x22 按 DID 读取版本/标识数据
  * @param  did  数据标识符，如 0xF189 软件版本
  * @param  data 输出缓冲区
  * @param  len  输出数据长度
  * @retval 0 成功，-1 不支持的 DID
 **********************************************************/
int Did_Read(uint16_t did, uint8_t *data, TP_LengthType *len)
{
    ECU_Information_t *info = Did_Info_Get();
    uint8_t local_len = 0u;

    if ((info == NULL) || (data == NULL) || (len == NULL))
    {
        return -1;
    }

    switch (did)
    {
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
        case 0xF182:
            *len = LEN_CAL_VERSION;
            (void)memcpy(data, info->calibration_version, LEN_CAL_VERSION);
            break;
        case 0xF150:
            *len = LEN_HW_VERSION;
            (void)memcpy(data, info->hw_version, LEN_HW_VERSION);
            break;
        case 0xFF00:
            *len = LEN_DIAG_VERSION;
            (void)memcpy(data, info->diag_version, LEN_DIAG_VERSION);
            break;
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
        case 0xF190:
            *len = LEN_VIN;
            (void)memcpy(data, info->vin, LEN_VIN);
            break;
        case 0xF1EF:
            /* Boot&App 区分标识：APP 固定回 0x00 */
            *len = LEN_BOOT_APP_ID;
            data[0] = VAL_BOOT_APP_ID_APP;
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
            if (FaultInfo_ReadDID(did, data, &local_len) != 0)
            {
                return -1;
            }
            *len = local_len;
            break;
        default:
            return -1;
    }

    return 0;
}

/**********************************************************
  * @brief  0x2E 写 DID：APP 仅允许写 VIN(0xF190)
 **********************************************************/
int Did_Write(uint16_t did, const uint8_t *data, uint8_t len)
{
    ECU_Information_t *info = Did_Info_Get();

    if ((info == NULL) || (data == NULL))
    {
        return -1;
    }

    if (did != 0xF190u)
    {
        return -1;
    }
    if (len != LEN_VIN)
    {
        return -2;
    }

    (void)memcpy(info->vin, data, LEN_VIN);
    if (Did_Save_Vin() != 0)
    {
        return -3;
    }
    return 0;
}

int Did_Upd_Sw_Ver(const uint8_t *version)
{
    ECU_Information_t *info = Did_Info_Get();

    if ((version == NULL) || (info == NULL))
    {
        return -1;
    }

    (void)memcpy(info->sw_version, version, LEN_SW_VERSION);
    return 0;
}

void Did_Flush(void)
{
}
