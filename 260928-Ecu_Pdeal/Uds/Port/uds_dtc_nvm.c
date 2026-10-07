#include "uds_dtc_nvm.h"
#include "user_versions.h"
#include "CRC_hal.h"
#include <string.h>

static const FaultInfo_t g_fault_default =
{
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

static FaultInfo_t g_fault_runtime;
static uint8_t g_dtc_setting_enabled = 1u;
static uint8_t g_attempt_counted_this_cycle = 0u;

FaultInfo_t *FaultInfo_GetPtr(void)
{
    return &g_fault_runtime;
}

/**********************************************************
  * @brief  计算故障页 CRC（不含 crc/magic 字段）
 **********************************************************/
static uint32_t fault_calc_crc(const FaultInfo_t *pInfo)
{
    uint32_t crc_len = (uint32_t)((uint8_t *)&pInfo->crc - (uint8_t *)pInfo);
    return crc32_calc((const uint8_t *)pInfo, crc_len);
}

/**********************************************************
  * @brief  DFlash 页1 是否已由 Boot 写入魔数
 **********************************************************/
static uint8_t fault_is_initialized(void)
{
    uint32_t read_magic = 0u;

    Did_Nvm_Read(NVM_PAGE1_OFFSET + (uint32_t)offsetof(FaultInfo_t, magic),
                 (uint8_t *)&read_magic, 4u);
    return (read_magic == NVM_PAGE1_MAGIC_VALUE) ? 1u : 0u;
}

/**********************************************************
  * @brief  上电从 DFlash 页1 装入 F1ED/0200/0201 等（APP 只读，不 format、不写回）
 **********************************************************/
void FaultInfo_Init(void)
{
    g_dtc_setting_enabled = 1u;
    g_attempt_counted_this_cycle = 0u;
    (void)memcpy(&g_fault_runtime, &g_fault_default, sizeof(FaultInfo_t));

    if (0u == fault_is_initialized())
    {
        return;
    }

    Did_Nvm_Read(NVM_PAGE1_OFFSET, (uint8_t *)&g_fault_runtime, (uint16_t)sizeof(FaultInfo_t));
    if (fault_calc_crc(&g_fault_runtime) != g_fault_runtime.crc)
    {
        /* 与 Boot 一致：CRC 不对仍保留读到的数据，APP 不擦写该页 */
        g_fault_runtime.magic = NVM_PAGE1_MAGIC_VALUE;
    }
}

void FaultInfo_Flush(void)
{
    /* APP 不写故障页，计数由 Boot FF00/FF01 落盘 */
}

/**********************************************************
  * @brief  读取故障相关 DID（F1ED/F1EE/FA19/0200/0201）
 **********************************************************/
int FaultInfo_ReadDID(uint16_t did, uint8_t *data, uint8_t *len)
{
    FaultInfo_t *pInfo = FaultInfo_GetPtr();

    if ((data == NULL) || (len == NULL) || (pInfo == NULL))
    {
        return -1;
    }

    switch (did)
    {
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
        default:
            return -1;
    }

    return 0;
}

int FaultInfo_WriteDID(uint16_t did, const uint8_t *data, uint8_t len)
{
    (void)did;
    (void)data;
    (void)len;
    return -1;
}

void FaultInfo_SetF1ED(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ed |= bit;
}

void FaultInfo_ClearF1ED(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ed &= (uint8_t)~bit;
}

uint8_t FaultInfo_GetF1ED(void)
{
    return FaultInfo_GetPtr()->f1ed;
}

void FaultInfo_SetF1EE(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ee |= bit;
}

void FaultInfo_ClearF1EE(uint8_t bit)
{
    FaultInfo_GetPtr()->f1ee &= (uint8_t)~bit;
}

uint8_t FaultInfo_GetF1EE(void)
{
    return FaultInfo_GetPtr()->f1ee;
}

void FaultInfo_SetKeyStatus(uint8_t status)
{
    FaultInfo_GetPtr()->fa19 = status;
}

uint8_t FaultInfo_GetKeyStatus(void)
{
    return FaultInfo_GetPtr()->fa19;
}

void FaultInfo_IncProgramAttempt(void)
{
    FaultInfo_t *pInfo = FaultInfo_GetPtr();

    if (g_attempt_counted_this_cycle != 0u)
    {
        return;
    }
    g_attempt_counted_this_cycle = 1u;
    if (pInfo->program_attempt_cnt < 255u)
    {
        pInfo->program_attempt_cnt++;
    }
}

void FaultInfo_OnProgramSuccess(void)
{
    FaultInfo_t *pInfo = FaultInfo_GetPtr();

    if (pInfo->program_success_cnt < 65535u)
    {
        pInfo->program_success_cnt++;
    }
    pInfo->program_attempt_cnt = 0u;
    g_attempt_counted_this_cycle = 0u;
}

/**********************************************************
  * @brief  0x85 开关 DTC 置位（1 开 / 0 关）
 **********************************************************/
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
    FaultInfo_t *pInfo = FaultInfo_GetPtr();
    uint8_t i;

    if (g_dtc_setting_enabled == 0u)
    {
        return;
    }

    for (i = 0u; i < pInfo->dtc_count; i++)
    {
        if (pInfo->dtc_list[i] == dtc)
        {
            return;
        }
    }

    if (pInfo->dtc_count < FAULT_DTC_MAX_COUNT)
    {
        pInfo->dtc_list[pInfo->dtc_count++] = dtc;
    }
}

void FaultInfo_ClearDTC(uint32_t dtc)
{
    FaultInfo_t *pInfo = FaultInfo_GetPtr();
    uint8_t i;

    for (i = 0u; i < pInfo->dtc_count; i++)
    {
        if (pInfo->dtc_list[i] == dtc)
        {
            pInfo->dtc_list[i] = pInfo->dtc_list[pInfo->dtc_count - 1u];
            pInfo->dtc_count--;
            return;
        }
    }
}

/**********************************************************
  * @brief  0x14 清除全部 DTC
 **********************************************************/
void FaultInfo_ClearAllDTC(void)
{
    FaultInfo_t *pInfo = FaultInfo_GetPtr();
    pInfo->dtc_count = 0;
    (void)memset(pInfo->dtc_list, 0, sizeof(pInfo->dtc_list));
}

uint8_t FaultInfo_IsDTCSet(uint32_t dtc)
{
    FaultInfo_t *pInfo = FaultInfo_GetPtr();
    uint8_t i;

    for (i = 0u; i < pInfo->dtc_count; i++)
    {
        if (pInfo->dtc_list[i] == dtc)
        {
            return 1u;
        }
    }
    return 0u;
}

uint16_t FaultInfo_GetDTCList(uint32_t *dtc_list, uint16_t max_count)
{
    FaultInfo_t *pInfo = FaultInfo_GetPtr();
    uint16_t count = (pInfo->dtc_count < max_count) ? pInfo->dtc_count : max_count;

    if (dtc_list != NULL)
    {
        (void)memcpy(dtc_list, pInfo->dtc_list, count * sizeof(uint32_t));
    }
    return count;
}

/* 0x19 0A 用的“支持的 DTC”固定表，未置位也要报（状态 0x00）
 * 当前是头文件里已有的码；电踏电机/霍尔等码定了以后只改这张表 */
static const uint32_t g_dtc_supported[] =
{
    DTC_U007388,
    DTC_U007488,
    DTC_U007688,
    DTC_U007A88,
    DTC_U007B88,
    DTC_U007C88,
    DTC_U007D88,
    DTC_U007E88,
    DTC_U007F88,
    DTC_B111716,
    DTC_B111717,
    DTC_U1F0052,
    DTC_B112355,
};

uint16_t FaultInfo_GetSupportedDTCCount(void)
{
    return (uint16_t)(sizeof(g_dtc_supported) / sizeof(g_dtc_supported[0]));
}

uint32_t FaultInfo_GetSupportedDTC(uint16_t index)
{
    if (index >= FaultInfo_GetSupportedDTCCount())
    {
        return 0u;
    }
    return g_dtc_supported[index];
}
