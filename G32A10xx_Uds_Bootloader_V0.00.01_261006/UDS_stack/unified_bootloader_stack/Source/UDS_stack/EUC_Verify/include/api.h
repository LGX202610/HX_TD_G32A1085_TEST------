#ifndef __API_H__
#define __API_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
  const uint8_t * addr;
  uint16_t len;
} cont;

#define SHA256_DIGEST_SIZE 32
#define SHA256_BLOCK_SIZE 64

struct sha256_ctx {
	uint32_t h[8];
	uint32_t tot_len;
	uint32_t len;
	uint8_t block[2 * SHA256_BLOCK_SIZE];
	uint8_t buf[SHA256_DIGEST_SIZE];  /* Used to store the final digest. */
};

// RSA key lengths
#define RSA_MAX_MODULUS_BITS                2048
#define RSA_MAX_MODULUS_LEN                 ((RSA_MAX_MODULUS_BITS + 7) / 8)
#define RSA_MAX_PRIME_BITS                  ((RSA_MAX_MODULUS_BITS + 1) / 2)
#define RSA_MAX_PRIME_LEN                   ((RSA_MAX_PRIME_BITS + 7) / 8)

// Error codes
#define ERR_WRONG_DATA                      0x1001
#define ERR_WRONG_LEN                       0x1002


//typedef uint64_t dbn_t;
//typedef uint32_t bn_t;

typedef struct {
    uint32_t bits;
    uint8_t  m[RSA_MAX_MODULUS_LEN];
    uint8_t  e[RSA_MAX_MODULUS_LEN];
} rsa_pk_t;

/*
@SHA256_init
@功能：准备SHA256上下文结构
@参数:  ctx  [IN ] SHA256上下文结构
@返回值：无
*/
void SHA256_init(struct sha256_ctx *ctx);

/*
@SHA256_update
@功能：计算SHA256
@参数:  ctx  [IN ] SHA256上下文结构
@       data [IN ] 待计算SHA256内容
@       len  [IN ] 待计算SHA256内容长度
@返回值：无
*/
void SHA256_update(struct sha256_ctx *ctx, const uint8_t *data, uint32_t len);

/*
@SHA256_final
@功能：获得计算结果
@参数:  ctx  [IN ] SHA256上下文结构
@返回值：SHA256结果
*/
uint8_t *SHA256_final(struct sha256_ctx *ctx);

/*
@calc_cert_sha256
@功能：计算x506证书缓存中的证书部分的SHA256值
@参数:  x  [IN ] x509缓存
@       len       [IN ] x509缓存长度
@       hash      [IN/OUT] 返回的SHA256值,最少32BYTE预分配内存
@返回值：0, 成功； 非0失败
*/
int calc_cert_sha256(const uint8_t *x, uint16_t len, uint8_t *hash);

/*
@calc_sha256
@功能：单次计算sha256摘要值
@参数:  buf  [IN ] 缓存数据
@       len  [IN ] 缓存长度
@       hash [IN/OUT] 返回的SHA256值,最少32BYTE预分配内存
@返回值：0, 成功； 非0失败
*/
int calc_sha256(const uint8_t *buf, uint16_t len, uint8_t *hash);

/*
@get_x509_signature
@功能：从x509证书缓存中找到证书签名
@参数:  x  [IN ] x509缓存
@       len       [IN ] x509缓存长度
@       sig       [IN/OUT] 缓存中签名部分
@返回值：0, 成功； 非0失败
*/
int get_x509_signature(const uint8_t *x, uint16_t len, cont *sig);

/*
@get_hash_from_signature
@功能：从解密后的签名缓存里获取HASH值
@参数:  sig_buf  [IN ] 解密后的签名缓存
@       len      [IN ] 解密后的签名缓存的长度
@       hash     [IN/OUT] 签名缓存中的HASH/SHA256值
@返回值：0, 成功； 非0失败
*/
int get_hash_from_signature(const uint8_t *sig_buf, uint16_t len, cont *hash);

/*
@rsa_public_decrypt
@功能：用公钥解密
@参数:  out      [IN/OUT ] 解密后的内容
@       out_len  [IN/OUT ] 解密后的长度
@       in       [IN] 待机密缓存
@       in_len   [IN] 待解密缓存长度
@       pk       [IN] 公钥
@返回值：返回指向证书文件的缓存，使用后需要手动释放
*/
int rsa_public_decrypt (uint8_t *out, uint32_t *out_len, uint8_t *in, uint32_t in_len, rsa_pk_t *pk);


int get_x509_pkey(const uint8_t *x, uint16_t len, cont *m, cont *e);

#ifdef __cplusplus
}
#endif

#endif
