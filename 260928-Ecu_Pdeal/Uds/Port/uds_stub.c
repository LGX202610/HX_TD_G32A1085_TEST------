#include "includes.h"
#include "boot.h"
#include "fls_app.h"
#include "watchdog_hal.h"
#include "uds_alg_hal.h"
#include "uds_verify.h"
#include "CRC_hal.h"
#include "bootloader_debug.h"
#include "uds_app.h"
#include "g32a10xx.h"

/* 与 boot 握手区一致：前 14 字节算 CRC，结果存成 uint16 放在 +0x0E */
#define BOOT_INFO_CRC_LEN   (14u)
#define APP_DL_SUCC_FLAG    (0xA5u)

/**********************************************************
  * @brief  按 boot 规则刷新 SRAM 握手区 CRC
 **********************************************************/
static void SetInfomationCrcValue(void)
{
    uint32 crc = 0u;

    CRC_HAL_CreatSw((const uint8 *)INFO_BEGIN_ADDR, BOOT_INFO_CRC_LEN, &crc);
    *((volatile uint16 *)(INFO_BEGIN_ADDR + 0x0Eu)) = (uint16)crc;
}

/**********************************************************
  * @brief  校验 SRAM 握手区 CRC 是否与 boot 写入的一致
 **********************************************************/
static boolean VerifyInfomationCorrect(void)
{
    uint32 crc = 0u;
    uint16 stored;

    CRC_HAL_CreatSw((const uint8 *)INFO_BEGIN_ADDR, BOOT_INFO_CRC_LEN, &crc);
    stored = *((volatile uint16 *)(INFO_BEGIN_ADDR + 0x0Eu));
	APPDebugLog("(uint16)crc=%d,stored=%d,\r\n",(uint16)crc,stored);
    return (((uint16)crc) == stored) ? TRUE : FALSE;
}

/**********************************************************
  * @brief  软件 CRC32（以太网多项式），给 boot 握手区算校验
 **********************************************************/
uint32_t crc32_calc(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    uint32_t i;
    uint32_t bit;

    for (i = 0u; i < len; i++)
    {
        crc ^= (uint32_t)data[i];
        for (bit = 0u; bit < 8u; bit++)
        {
            if ((crc & 1u) != 0u)
            {
                crc = (crc >> 1) ^ 0xEDB88320u;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

/**********************************************************
  * @brief  CRC 模块初始化（APP 用软件 CRC，无需硬件）
 **********************************************************/
boolean CRC_HAL_Init(void)
{
    return TRUE;
}

/**********************************************************
  * @brief  硬件 CRC 入口，APP 直接转软件实现
 **********************************************************/
void CRC_HAL_CreatHw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc)
{
    CRC_HAL_CreatSw(dataBuf, dataLen, curCrc);
}

/**********************************************************
  * @brief  软件计算 CRC32
 **********************************************************/
void CRC_HAL_CreatSw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc)
{
    if ((dataBuf == NULL_PTR) || (curCrc == NULL_PTR))
    {
        return;
    }
    *curCrc = crc32_calc(dataBuf, dataLen);
}

/**********************************************************
  * @brief  复位下载状态（APP 不刷写，空实现供链接）
 **********************************************************/
void PrepareDLInformation(void)
{
}

/**********************************************************
  * @brief  boot 调试 IO 初始化（APP 不使用）
 **********************************************************/
void PrepareBootloaderDebug(void)
{
}

/**********************************************************
  * @brief  看门狗初始化桩
 **********************************************************/
void WATCHDOG_HAL_Init(void)
{
}

/**********************************************************
  * @brief  喂狗桩（APP 主循环已有 hal_iwdt_refresh）
 **********************************************************/
void WATCHDOG_HAL_Fed(void)
{
}

/**********************************************************
  * @brief  MCU 复位，10 02 进 boot 前调用
 **********************************************************/
void WATCHDOG_HAL_SystemReset(void)
{
    NVIC_SystemReset();
}

/**********************************************************
  * @brief  请求进入 Bootloader
  * @note   向 SRAM 握手区写 0x5A 并更新 CRC，复位后 boot 读到该标志会留下
 **********************************************************/
void BootloaderAccepteReq(void)
{
    *((volatile uint8 *)REQ_ENTER_BL_ADDR) = 0x5Au;
    SetInfomationCrcValue();
}

/**********************************************************
  * @brief  清除 APP 下载成功标志并更新握手 CRC
 **********************************************************/
void ClearFlagForDownloadAppOk(void)
{
    *((volatile uint8 *)APP_DL_SUC_ADDR) = 0u;
    SetInfomationCrcValue();
}

/**********************************************************
  * @brief  查询 boot 在 0x11 复位前写入的下载成功标志（0xA5）
 **********************************************************/
boolean VerifyDownloadAppOk(void)
{
    if (TRUE != VerifyInfomationCorrect())
    {
        return FALSE;
    }
    if (APP_DL_SUCC_FLAG != *((volatile uint8 *)APP_DL_SUC_ADDR))
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************
  * @brief  刷写后首次进入 APP：清标志并主动发 0x51 01
  * @note   boot 对 0x11 01 只回 0x78 后复位，正响应由 APP 补发
 **********************************************************/
boolean VerifyAppStatusAfterDownlaod(void)
{
    boolean retResult = FALSE;
	APPDebugLog("app VerifyAppStatusAfterDownlaod \r\n");
    if (TRUE == VerifyDownloadAppOk())
    {
        ClearFlagForDownloadAppOk();
        retResult = (TRUE == UDS_APP_SendMsgToHost()) ? TRUE : FALSE;
		APPDebugLog("app UDS_APP_SendMsgToHost %d ,is true?\r\n",retResult);
    }

    return retResult;
}

/**********************************************************
  * @brief  安全算法加密桩（刷写服务仅在 boot）
 **********************************************************/
boolean UDS_ALG_HAL_DataEncryptionProcess(const uint8 *pDataIn, const uint32 dataLength, uint8 *pDataOut)
{
    (void)pDataIn;
    (void)dataLength;
    (void)pDataOut;
    return FALSE;
}

/**********************************************************
  * @brief  安全算法解密桩
 **********************************************************/
boolean UDS_ALG_HAL_DataDecryptionMethod(const uint8 *pDeCipherData, const uint32 pDataLength, uint8 *pDataBuffer)
{
    (void)pDeCipherData;
    (void)pDataLength;
    (void)pDataBuffer;
    return FALSE;
}

/**********************************************************
  * @brief  种子生成桩
 **********************************************************/
boolean UDS_ALG_HAL_MyReplacedFunc(const uint32 needLen, uint8 *pOutBuf)
{
    static uint32 s_seed = 0x13579BDFu;
    uint32 mix;
    uint32 i;

    if ((0u == needLen) || (NULL_PTR == pOutBuf))
    {
        return FALSE;
    }

    s_seed += 0x9E3779B9u;
    mix = s_seed ^ (uint32)(&s_seed);
    for (i = 0u; i < needLen; i++)
    {
        pOutBuf[i] = (uint8)((mix >> ((i & 3u) * 8u)) & 0xFFu);
        if ((i & 3u) == 3u)
        {
            mix = (mix * 1664525u) + 1013904223u;
        }
    }
    if ((needLen >= 4u) &&
        ((pOutBuf[0] | pOutBuf[1] | pOutBuf[2] | pOutBuf[3]) == 0u))
    {
        pOutBuf[3] = 0x01u;
    }
    return TRUE;
}

/**********************************************************
  * @brief  安全算法软件计时桩
 **********************************************************/
void UDS_ALG_HAL_AddSWTimerTickCnt(void)
{
}

/**********************************************************
  * @brief  文件合法性校验桩
 **********************************************************/
int UDS_Verify_CheckFileValidity(const uint8_t *pVerifyParam)
{
    (void)pVerifyParam;
    return -1;
}

/**********************************************************
  * @brief  读取已保存升级版本桩
 **********************************************************/
uint8_t *UDS_Verify_GetSavedVersion(void)
{
    return NULL_PTR;
}

/**********************************************************
  * @brief  读取已保存摘要桩
 **********************************************************/
uint8_t *UDS_Verify_GetSavedDigest(void)
{
    return NULL_PTR;
}

/**********************************************************
  * @brief  清空摘要桩
 **********************************************************/
void UDS_Verify_ClearDigest(void)
{
}

/**********************************************************
  * @brief  清空版本桩
 **********************************************************/
void UDS_Verify_ClearVersion(void)
{
}

/**********************************************************
  * @brief  下载数据完整性检查桩
 **********************************************************/
int UDS_Verify_CheckDownloadedData(void)
{
    return -1;
}
