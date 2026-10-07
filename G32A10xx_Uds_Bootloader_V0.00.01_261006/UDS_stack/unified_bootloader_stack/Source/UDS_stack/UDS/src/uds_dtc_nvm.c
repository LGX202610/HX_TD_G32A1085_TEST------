#include "uds_dtc_nvm.h"
#include "user_versions.h"
#include "g32a10xx_fls.h"
#include "watchdog_hal.h"
#include "flash_hal.h"
#include <string.h>

static const FaultInfo_t g_fault_default = {
    .dtc_list = {0},
    .dtc_count = 0,
    .f1ed = 0x00,
    .f1ee = 0x00,
    .fa19 = 0x00,
    .program_success_cnt = 0,
    .program_attempt_cnt = 0,
    .reserved = 0,
    .crc = 0,
    .magic = NVM_PAGE1_MAGIC_VALUE,
};

static FaultInfo_t g_fault_runtime; // 故障信息运行时数据
static uint8_t g_dtc_setting_enabled = 1u; // DTC设置使能标志
static uint8_t g_attempt_counted_this_cycle = 0u; // 本次刷写周期内是否已计数过成功刷写

FaultInfo_t* FaultInfo_GetPtr(void)
{
    return &g_fault_runtime;
}

static uint32_t fault_calc_crc(const FaultInfo_t* pInfo)
{
    uint32_t crc_len = (uint32_t)((uint8_t*)&pInfo->crc - (uint8_t*)pInfo);
    return crc32_calc((const uint8_t*)pInfo, crc_len);
}

static uint8_t fault_is_initialized(void)
{
    uint32_t read_magic;
    Did_Nvm_Read(NVM_PAGE1_OFFSET + offsetof(FaultInfo_t, magic),
             (uint8_t*)&read_magic, 4);
    return (read_magic == NVM_PAGE1_MAGIC_VALUE);
}

static void fault_program_page(const uint8_t *buffer)
{
    uint32_t *p_data = (uint32_t *)buffer;
    uint32_t addr = NVM_PAGE1_ADDR;
    const uint32_t bytes_per_chunk = 64u;   /* 与 FMC_LEN_512BIT 一次 64 字节对应 */
    const uint32_t words_per_chunk = 16u;
    const uint32_t chunks = NVM_PAGE1_SIZE / bytes_per_chunk;
    uint32_t i;
    Fmc_StateType erase_ret;

    WATCHDOG_HAL_Fed();
    Fmc_Unlock();
    FLASH_HAL_ClearFmcErrorFlags();
    WATCHDOG_HAL_Fed();
    /* NVM_PAGE1_ADDR=0x08040200，页对齐 512B，与 Fmc_ErasePage 要求一致 */
    erase_ret = Fmc_ErasePage(NVM_PAGE1_ADDR);
    if (FMC_STATE_COMPLETE != erase_ret)
    {
        Fmc_Lock();
        UDSCFGDebugLog("ErasePage fail %d\r\n", erase_ret);
        return;
    }

    for (i = 0u; i < chunks; i++) {
        WATCHDOG_HAL_Fed();
        (void)Fmc_ProgramWord(addr, (uint32_t)FMC_LEN_512BIT, p_data);
        addr += bytes_per_chunk;
        p_data += words_per_chunk;
    }

    Fmc_Lock();
    WATCHDOG_HAL_Fed();
}

static void fault_format(void)
{
    uint8_t buffer[NVM_PAGE1_SIZE] __attribute__((aligned(4)));
    uint32_t crc;

    memset(buffer, 0xFF, NVM_PAGE1_SIZE);
    memcpy(buffer, &g_fault_default, sizeof(FaultInfo_t));
    crc = fault_calc_crc(&g_fault_default);
    memcpy(buffer + offsetof(FaultInfo_t, crc), &crc, 4);

    fault_program_page(buffer);
    UDSCFGDebugLog("Fault: Page1 formatted (first time)\n");
}

void FaultInfo_Init(void)
{
    g_dtc_setting_enabled = 1u;
    g_attempt_counted_this_cycle = 0u;

    if (!fault_is_initialized()) {
        fault_format();
    }

    Did_Nvm_Read(NVM_PAGE1_OFFSET, (uint8_t*)&g_fault_runtime, sizeof(FaultInfo_t));

    if (fault_calc_crc(&g_fault_runtime) != g_fault_runtime.crc) {       
        UDSCFGDebugLog("Fault: CRC mismatch, keep data, repair on next flush\n");
        g_fault_runtime.magic = NVM_PAGE1_MAGIC_VALUE;
    }

    UDSCFGDebugLog("Fault: Init f1ed=0x%02X f1ee=0x%02X fa19=0x%02X dtc=%d att=%d ok=%d\n",
                   g_fault_runtime.f1ed, g_fault_runtime.f1ee,
                   g_fault_runtime.fa19, g_fault_runtime.dtc_count,
                   g_fault_runtime.program_attempt_cnt,
                   g_fault_runtime.program_success_cnt);
}

void FaultInfo_Flush(void)
{
    uint8_t buffer[NVM_PAGE1_SIZE] __attribute__((aligned(4)));

    Did_Nvm_Read(NVM_PAGE1_OFFSET, buffer, NVM_PAGE1_SIZE);

    g_fault_runtime.magic = NVM_PAGE1_MAGIC_VALUE;
    g_fault_runtime.crc = fault_calc_crc(&g_fault_runtime);
    memcpy(buffer, &g_fault_runtime, sizeof(FaultInfo_t));

    fault_program_page(buffer);
    UDSCFGDebugLog("fault_save_to_flash finish\r\n");
}

int FaultInfo_ReadDID(uint16_t did, uint8_t* data, uint8_t* len)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();

    switch (did) {
        case 0xF1ED:
            *len = 1;
            data[0] = pInfo->f1ed;
            break;
        case 0xF1EE:
            *len = 1;
            data[0] = pInfo->f1ee;
            break;
        case 0xFA19:
            *len = 1;
            data[0] = pInfo->fa19;
            break;
        case 0x0200:
            *len = 2;
            data[0] = (uint8_t)(pInfo->program_success_cnt >> 8);
            data[1] = (uint8_t)(pInfo->program_success_cnt & 0xFFu);
            break;
        case 0x0201:
            *len = 1;
            data[0] = pInfo->program_attempt_cnt;
            break;
        case 0xFA00:
            *len = (uint8_t)(pInfo->dtc_count * 4u);
            memcpy(data, pInfo->dtc_list, *len);
            break;
        default:
            return -1;
    }

    return 0;
}

int FaultInfo_WriteDID(uint16_t did, const uint8_t* data, uint8_t len)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();

    switch (did) {
        case 0xF1ED:
            if (len != 1) return -2;
            pInfo->f1ed = data[0];
            break;
        case 0xF1EE:
            if (len != 1) return -2;
            pInfo->f1ee = data[0];
            break;
        case 0xFA19:
            if (len != 1) return -2;
            pInfo->fa19 = data[0];
            break;
        case 0xFA00:
            if (len > FAULT_DTC_MAX_COUNT * 4) return -2;
            pInfo->dtc_count = (uint8_t)(len / 4u);
            memcpy(pInfo->dtc_list, data, pInfo->dtc_count * 4u);
            break;
        default:
            return -1;
    }

    FaultInfo_Flush();
    return 0;
}

void FaultInfo_SetF1ED(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ed |= bit;
    FaultInfo_Flush();
}

void FaultInfo_ClearF1ED(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ed &= (uint8_t)~bit;
    FaultInfo_Flush();
}

uint8_t FaultInfo_GetF1ED(void)
{
    return FaultInfo_GetPtr()->f1ed;
}

void FaultInfo_ResetF1ED(void)
{
    FaultInfo_GetPtr()->f1ed = 0u;
}

void FaultInfo_SetF1EE(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ee |= bit;
    FaultInfo_Flush();
}

void FaultInfo_ClearF1EE(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ee &= (uint8_t)~bit;
    FaultInfo_Flush();
}

uint8_t FaultInfo_GetF1EE(void)
{
    return FaultInfo_GetPtr()->f1ee;
}

void FaultInfo_SetKeyStatus(uint8_t status)
{
    FaultInfo_GetPtr()->fa19 = status;
    FaultInfo_Flush();
}

uint8_t FaultInfo_GetKeyStatus(void)
{
    return FaultInfo_GetPtr()->fa19;
}

void FaultInfo_IncProgramAttempt(void)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();

    if (g_attempt_counted_this_cycle != 0u) {
        return; // 如果本次刷写周期内已计数过成功刷写，则直接返回
    }
    g_attempt_counted_this_cycle = 1u; // 设置本次刷写周期内已计数过成功刷写
    if (pInfo->program_attempt_cnt < 255u) {
        pInfo->program_attempt_cnt++;
    }
}

void FaultInfo_OnProgramSuccess(void)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();

    if (pInfo->program_success_cnt < 65535u) {
        pInfo->program_success_cnt++;
    }
    pInfo->program_attempt_cnt = 0u;
    g_attempt_counted_this_cycle = 0u;

}

void FaultInfo_SetDtcSetting(uint8_t enable)
{
    g_dtc_setting_enabled = (enable != 0u) ? 1u : 0u;
}

uint8_t FaultInfo_IsDtcSettingEnabled(void)
{
    return g_dtc_setting_enabled;
}

void FaultInfo_SetDTC(uint32_t dtc)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();
    uint8_t i;

    if (g_dtc_setting_enabled == 0u) {
        return;
    }

    for (i = 0; i < pInfo->dtc_count; i++) {
        if (pInfo->dtc_list[i] == dtc) {
            return;
        }
    }

    if (pInfo->dtc_count < FAULT_DTC_MAX_COUNT) {
        pInfo->dtc_list[pInfo->dtc_count++] = dtc;
        FaultInfo_Flush();
    }
}

void FaultInfo_ClearDTC(uint32_t dtc)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();
    uint8_t i;

    for (i = 0; i < pInfo->dtc_count; i++) {
        if (pInfo->dtc_list[i] == dtc) {
            pInfo->dtc_list[i] = pInfo->dtc_list[pInfo->dtc_count - 1u];
            pInfo->dtc_count--;
            FaultInfo_Flush();
            return;
        }
    }
}

void FaultInfo_ClearAllDTC(void)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();
    pInfo->dtc_count = 0;
    memset(pInfo->dtc_list, 0, sizeof(pInfo->dtc_list));

}

uint8_t FaultInfo_IsDTCSet(uint32_t dtc)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();
    uint8_t i;

    for (i = 0; i < pInfo->dtc_count; i++) {
        if (pInfo->dtc_list[i] == dtc) {
            return 1;
        }
    }
    return 0;
}

uint16_t FaultInfo_GetDTCList(uint32_t* dtc_list, uint16_t max_count)
{
    FaultInfo_t* pInfo = FaultInfo_GetPtr();
    uint16_t count = (pInfo->dtc_count < max_count) ? pInfo->dtc_count : max_count;
    memcpy(dtc_list, pInfo->dtc_list, count * sizeof(uint32_t));
    return count;
}
