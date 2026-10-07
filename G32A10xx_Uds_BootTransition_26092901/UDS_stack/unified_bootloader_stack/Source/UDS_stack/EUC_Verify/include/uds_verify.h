#ifndef UDS_VERIFT_H_
#define UDS_VERIFT_H_
#include "includes.h"


/**
 * @brief 完整校验文件合法性（步骤1 + 步骤2）
 * @param pVerifyParam 1322字节校验参数
 * @return 0=通过，-1=证书无效，-2=签名无效
 */
int UDS_Verify_CheckFileValidity(const uint8_t* pVerifyParam);
/**
 * @brief 获取保存的版本信息
 * @return 版本信息指针，无效时返回 NULL
 */
 uint8_t *UDS_Verify_GetSavedVersion(void);
/**
 * @brief 获取保存的摘要值
 * @return 摘要值指针，无效时返回 NULL
 */
uint8_t* UDS_Verify_GetSavedDigest(void);
/**
 * @brief 清空摘要值
 */
void UDS_Verify_ClearDigest(void);
/**
 * @brief 清空版本信息
 */
 void UDS_Verify_ClearVersion(void);
/**
 * @brief 检查已下载数据完整性（0x31 0x0203）
 * @return 0=成功，-1=失败（数据不一致）
 */
int UDS_Verify_CheckDownloadedData(void);






/**
 * @brief 验证签名信息块
 * @param sign_info   [IN] 1322字节签名信息块
 * @param ca_modulus  [IN] CA公钥模数N（256字节）
 * @param ca_exponent [IN] CA公钥指数E（通常为0x10001）
 * @return 0=验签成功，非0=失败
 */
 int verify_sign_info(const uint8_t *sign_info);

/**
 * @brief 解密并验证X.509证书
 * @param encrypted_cert [IN] 1024字节加密证书
 * @param ca_modulus     [IN] CA公钥模数N
 * @param ca_exponent    [IN] CA公钥指数E
 * @param out_public_key [OUT] 证书中的公钥模数N（256字节）
 * @return 0=成功，非0=失败
 */
int decrypt_and_verify_cert(const uint8_t *encrypted_cert,
                            const uint8_t *ca_modulus,
                            uint32_t ca_exponent,
                            uint8_t *out_public_key);


#endif