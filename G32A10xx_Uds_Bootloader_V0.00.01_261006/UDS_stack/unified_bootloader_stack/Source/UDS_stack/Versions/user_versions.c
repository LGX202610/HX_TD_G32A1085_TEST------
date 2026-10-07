#include "user_versions.h"
#include "g32a10xx_fls.h"
#include "uds_dtc_nvm.h"
#include "flash_hal.h"
#include "uds_app_cfg.h"
#include <string.h>
#include <stddef.h>


//  1. 编译时固定的默认值（存放在PFlash，只读 lu-）
static const ECU_Information_t g_ECU_Info_default  = {

    // ---------- 区块0：版本信息 ----------
    .sw_version           = "0.00.01",
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
    .production_date      = {0x20,0x26,0x09,0x29},
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


// ==================== 初始化函数 ====================
void Did_Info_Init(void) {
    memcpy(&g_ECU_Info_running, &g_ECU_Info_default, sizeof(ECU_Information_t));
    Did_Nvm_Read(NVM_DATA_OFFSET, (uint8_t*)&g_ECU_Info_running, sizeof(ECU_Information_t));
    g_ECU_Info_running.boot_app_id = VAL_BOOT_APP_ID_BOOT;

    /* 只把本份 Boot hex 的 F180 写回，F189 等跟 APP */
    if (0 != memcmp(g_ECU_Info_running.pbl_version,
                    g_ECU_Info_default.pbl_version,
                    LEN_PBL_VERSION))
    {
        memcpy(g_ECU_Info_running.pbl_version,
               g_ECU_Info_default.pbl_version,
               LEN_PBL_VERSION);
        (void)Did_Nvm_Write((uint32_t)offsetof(ECU_Information_t, pbl_version),
                            (const uint8_t *)g_ECU_Info_running.pbl_version,
                            (uint8_t)LEN_PBL_VERSION);
        (void)Did_Nvm_Upd_Crc();
    }
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
	uint8 write_sta=0;
	Fmc_StateType ret;
	uint32_t data=0x11991234;
	uint32_t stored;
	
	DisableAllInterrupts();
    Fmc_Unlock();
	//擦除整个DFlash页（512字节） 
	ret = Fmc_ErasePage(NVM_BASE_ADDR);
	if(FMC_STATE_COMPLETE == ret)
	{
	}
	else
	{
		UDSCFGDebugLog("ErasePage fail %d\r\n",ret);
	}
	ret = Fmc_ErasePage(NVM_BASE_ADDR+0x200u);
	if(FMC_STATE_COMPLETE == ret)
	{
	}
	else
	{
		UDSCFGDebugLog("ErasePage fail %d\r\n",ret);
	}
//	if(FMC_STATE_COMPLETE == Fmc_ProgramWord(NVM_BASE_ADDR, (uint32_t)FMC_LEN_64BIT, &data))
//	{
//		
//	}
//	else
//	{
//		UDSCFGDebugLog("Program fail\r\n");
//	}
	Fmc_Lock();
    EnableAllInterrupts();

    // 读取存储的数据
    //Did_Nvm_Read(0, (uint8_t*)&stored, 4);
	//UDSCFGDebugLog("Did_Nvm_Read 0x%x\r\n",stored);
}
uint8_t Did_Nvm_Write(uint32_t offset, const uint8_t* data, uint8_t len)
{
	uint8 write_sta=0;
	Fmc_StateType ret;
	uint32_t stored;
    uint32_t i;
    uint32_t addr;
    uint32_t *p_data;
    const uint32_t words_per_chunk = 16u;   /* 16*4=64 字节，对应 FMC_LEN_512BIT */
    const uint32_t bytes_per_chunk = 64u;
    const uint32_t chunks = NVM_PAGE_SIZE / bytes_per_chunk;
    // 步骤1：在RAM中开辟512字节的缓存区
    //         __attribute__((aligned(4))) 保证地址4字节对齐
    //         因为 Fmc_ProgramWord 要求数据地址是4字节对齐
    uint8_t buffer[NVM_PAGE_SIZE] __attribute__((aligned(4)));

    if ((data == NULL) || ((offset + (uint32_t)len) > NVM_PAGE_SIZE))
    {
        return 1;
    }
    // 步骤2：读取整个DFlash页（512字节）到RAM缓存
    // NVM_BASE_ADDR = 0x08040000（DFlash页起始地址，页对齐）
    (void)memcpy(buffer, (const void*)NVM_BASE_ADDR, NVM_PAGE_SIZE);
    // 步骤3：在RAM缓存中更新要修改的数据
    (void)memcpy(&buffer[offset], data, len);
    DisableAllInterrupts();
    Fmc_Unlock();
    /* DFlash 与 PFlash 共用 STS，PFlash 残留 PE/PAE 会让 ErasePage 直接返回 2 */
    FLASH_HAL_ClearFmcErrorFlags();
    // 步骤6：擦除整个DFlash页（512字节），地址必须页对齐
	ret = Fmc_ErasePage(NVM_BASE_ADDR);
	if(FMC_STATE_COMPLETE == ret)
	{		
	}
	else
	{
		UDSCFGDebugLog("ErasePage fail %d\r\n",ret);
		write_sta = 1;
	}
	if (0u == write_sta)
	{
    // 步骤7：按块写入数据（每次写入64字节，分8次写完512字节）        
    //         为什么分8次？因为 Fmc_ProgramWord 一次最多写入512位（64字节）
    //         512字节 ÷ 64字节/次 = 8次    
    //         为什么用 FMC_LEN_512BIT？
    //         这个枚举值 = 3，表示一次写入512位（64字节）
    //         这是硬件支持的最大单次写入长度，效率最高
    p_data = (uint32_t*)buffer;
    addr = NVM_BASE_ADDR;
    for (i = 0; i < chunks; i++) 
    {
        //   参数1: 当前写入地址（64字节对齐，与 PROGLEN=512bit 匹配）
        //   参数2: FMC_LEN_512BIT = 3，表示本次写入512位（64字节）
        //   参数3: 数据指针（指向RAM缓存中当前要写入的数据）
        if(FMC_STATE_COMPLETE == Fmc_ProgramWord(addr, (uint32_t)FMC_LEN_512BIT, p_data))
        {
			
		}
		else
		{
			write_sta++;
		}
        addr += bytes_per_chunk;
        p_data += words_per_chunk;
    }
	}
    Fmc_Lock();
    EnableAllInterrupts();
	
	// 读取存储的数据
    Did_Nvm_Read(0, (uint8_t*)&stored, 4);
	UDSCFGDebugLog("Did_Nvm_Read 0x%x\r\n",stored);
	
	if(write_sta > 0) {return write_sta;}
		else return 0;
}
int Did_Nvm_Upd_Crc(void)
{
    uint32_t crc;
    uint8_t ret;

    crc = crc32_calc((const uint8_t*)NVM_BASE_ADDR, NVM_CRC_DATA_LEN);
	UDSCFGDebugLog("crc = 0x%x\r\n",crc);
    ret = Did_Nvm_Write(NVM_CRC_OFFSET, (uint8_t*)&crc, 4);

    if(ret != 0)return ret;
        else return 0;
}
void Did_Nvm_Format(void)
{
	int8_t ret=0;
    ECU_Information_t* default_data = Did_Info_Get_Def();
    uint32_t magic = NVM_MAGIC_VALUE;
    char saved_sw[LEN_SW_VERSION];
    uint8_t keep_sw = 0u;
    uint8_t i;

    /* F189 跟 APP hex，格式化时不要用 Boot 默认盖掉 */
    Did_Nvm_Read((uint32_t)offsetof(ECU_Information_t, sw_version),
                 (uint8_t *)saved_sw, LEN_SW_VERSION);
    for (i = 0u; i < LEN_SW_VERSION; i++)
    {
        if ((uint8_t)saved_sw[i] == 0xFFu)
        {
            keep_sw = 0u;  //说明 NVM 这块区域是空的，没有存有效的版本号
            break;
        }
        if ((saved_sw[i] >= '0') && (saved_sw[i] <= '9'))
        {
            keep_sw = 1u;
        }
    }

    // 2. 写入默认值
    ret = Did_Nvm_Write(NVM_DATA_OFFSET, (uint8_t*)default_data, sizeof(ECU_Information_t));
	if(ret != 0) {UDSCFGDebugLog("write default fail %d\r\n",ret);}
    if (0u != keep_sw)
    {
        ret = Did_Nvm_Write((uint32_t)offsetof(ECU_Information_t, sw_version),
                            (const uint8_t *)saved_sw, (uint8_t)LEN_SW_VERSION);
        if(ret != 0) {UDSCFGDebugLog("write f189 fail %d\r\n",ret);}
    }
    // 3. 计算并写入CRC
    ret = Did_Nvm_Upd_Crc();
	if(ret != 0){UDSCFGDebugLog("update_crc fail\r\n");}
    // 4. 写入魔数
    ret = Did_Nvm_Write(NVM_MAGIC_OFFSET, (uint8_t*)&magic, 4);
	if(ret != 0) {UDSCFGDebugLog("write MAGIC fail %d\r\n",ret);}
}
int Did_Nvm_Init(void)
{
    uint32_t magic;

    Did_Nvm_Read(NVM_MAGIC_OFFSET, (uint8_t*)&magic, 4);
    UDSCFGDebugLog("Version magic = 0x%x\r\n", magic);
    if (magic != NVM_MAGIC_VALUE) {
        Did_Nvm_Format();
    }
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

static uint8_t s_version_nvm_dirty = 0u;

int Did_Upd_Sw_Ver(const uint8_t *version)
{
    ECU_Information_t *info = Did_Info_Get();

    if ((version == NULL) || (info == NULL)) {
        return -1;
    }

    memcpy(info->sw_version, version, LEN_SW_VERSION);
    s_version_nvm_dirty = 1u;
    UDSCFGDebugLog("Did_Upd_Sw_Ver: %.10s\r\n", info->sw_version);
    return 0;
}

void Did_Flush(void)
{
    ECU_Information_t *info = Did_Info_Get();
    uint8_t nvm_ret;

    if (s_version_nvm_dirty == 0u) {
        return;
    }
    if (info == NULL) {
        s_version_nvm_dirty = 0u;
        return;
    }

    nvm_ret = Did_Nvm_Write(NVM_DATA_OFFSET, (uint8_t *)info, sizeof(ECU_Information_t));
    if (0u == nvm_ret)
    {
        nvm_ret = (uint8_t)Did_Nvm_Upd_Crc();
    }
    if (0u == nvm_ret)
    {
        s_version_nvm_dirty = 0u;
        UDSCFGDebugLog("Did_Flush: sw_version saved\r\n");
    }
    else
    {
        UDSCFGDebugLog("Did_Flush: nvm write fail %d\r\n", nvm_ret);
    }
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
    int ret = 0;

    switch (did)
    {
        // ========== 只读DID（不支持写入） ==========
        case 0xF189:   // 控制器软件版本号
        case 0xF188:   // 控制器软件号
        case 0xF180:   // 主引导程序版本号
        case 0xF182:   // 标定软件版本号
        case 0xF150:   // 控制器硬件版本号
        case 0xFF00:   // 诊断版本号
        case 0xF187:   // 零部件号
        case 0xF197:   // 产品型号
        case 0xF18A:   // 供应商代码
        case 0xF191:   // PCBA序列号（生产写入，但通过工厂会话）
        case 0xF18C:   // 控制器序列号（生产写入，但通过工厂会话）
        case 0xF18B:   // 控制器生产日期（生产写入，但通过工厂会话）
        case 0xF1EF:   // Boot&App 区分标识（只读，由运行分区决定）
        case 0xF190:   // VIN：Boot 不支持读/写
        case 0xF186:
        case 0xF195:
        case 0xF193:
            return -1;   // NRC: 0x31 RequestOutOfRange（只读）

        // ========== 区块2：VIN（可写） ==========
        // case 0xF190:   // VIN码
        //     if (len != LEN_VIN)
        //         return -2;   // NRC: 0x13 incorrectMessageLength
        //     memcpy(info->vin, data, LEN_VIN);
        //     break;

        // ========== 区块3：编程记录（可写） ==========
        case 0xF198:   // 诊断工具序列号
            if (len != LEN_TOOL_SERIAL)
                return -2;
            memcpy(info->tool_serial, data, LEN_TOOL_SERIAL);
            break;

        case 0xF199:   // 重编程日期
            if (len != LEN_REPROG_DATE)
                return -2;
            memcpy(info->reprogram_date, data, LEN_REPROG_DATE);
            break;
/*
        case 0x0200:   // 编程成功次数
            if (len != LEN_PROG_SUCCESS)
                return -2;
            info->program_success_cnt = (uint16_t)((data[0] << 8) | data[1]);
            break;

        case 0x0201:   // 尝试编程次数
            if (len != LEN_PROG_ATTEMPT)
                return -2;
            info->program_attempt_cnt = data[0];
            break;

        // ========== 区块4：密钥/安全状态（可写） ==========
        case 0xFA19:   // 27服务诊断密钥状态
            if (len != LEN_KEY_STATUS)
                return -2;
            info->key_status = data[0];
            break;

        case 0xF1ED:   // 安全刷写失败原因
            if (len != LEN_SECURE_FAIL_REASON)
                return -2;
            info->secure_fail_reason = data[0];
            break;

        case 0xF1EE:   // 安全启动失败原因
            if (len != LEN_BOOT_FAIL_REASON)
                return -2;
            info->boot_fail_reason = data[0];
            break;
*/

        // ========== 不支持的DID ==========
        default:
            return -1;   // NRC: 0x31 RequestOutOfRange
    }

    // 关键：任何写入操作完成后，整块写回DFlash并更新CRC
    Did_Nvm_Write(NVM_DATA_OFFSET, (uint8_t*)info, sizeof(ECU_Information_t));
    Did_Nvm_Upd_Crc();   // 重新计算并写入CRC

    UDSCFGDebugLog("Did_Write: DID 0x%04X write success\n", did);

    return ret;
}





