#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "bn.h"

#ifdef __cplusplus
extern "C"
{
#endif

static int public_block_operation(uint8_t *out, uint32_t *out_len, uint8_t *in, uint32_t in_len, rsa_pk_t *pk)
{
    uint32_t dg_e, dg_n;
    unsigned char pk_e[4] = {0x00,0x01,0x00,0x01};
    bn_t c[BN_MAX_DIGITS], e[BN_MAX_DIGITS], m[BN_MAX_DIGITS], n[BN_MAX_DIGITS];

    memset(pk->e, 0x00, 256);
    memcpy(pk->e + 252, pk_e, 4);

    bn_decode(m, BN_MAX_DIGITS, in, in_len);
    bn_decode(n, BN_MAX_DIGITS, pk->m, RSA_MAX_MODULUS_LEN);
    bn_decode(e, BN_MAX_DIGITS, pk->e, RSA_MAX_MODULUS_LEN);

    dg_n = bn_digits(n, BN_MAX_DIGITS);
    dg_e = bn_digits(e, BN_MAX_DIGITS);

    if(bn_cmp(m, n, dg_n) >= 0) {
        return ERR_WRONG_DATA;
    }

    bn_mod_exp(c, m, e, dg_e, n, dg_n);

    *out_len = (pk->bits + 7) / 8;
    bn_encode(out, *out_len, c, dg_n);

    memset((uint8_t *)c, 0, sizeof(c));
    memset((uint8_t *)m, 0, sizeof(m));

    return 0;
}

int rsa_public_decrypt(uint8_t *out, uint32_t *out_len, uint8_t *in, uint32_t in_len, rsa_pk_t *pk)
{
    int status;
    uint8_t pb[RSA_MAX_MODULUS_LEN];
    uint32_t i, m_len, pb_len;

    m_len = (pk->bits + 7) / 8;
    if(in_len > m_len)
        return ERR_WRONG_LEN;

    status = public_block_operation(pb, &pb_len, in, in_len, pk);
    if(status != 0)
        return status;

    if(pb_len != m_len)
        return ERR_WRONG_LEN;

    if((pb[0] != 0) || (pb[1] != 1))
        return ERR_WRONG_DATA;

    for(i=2; i<m_len-1; i++) {
        if(pb[i] != 0xFF)   break;
    }

    if(pb[i++] != 0)
        return ERR_WRONG_DATA;

    *out_len = m_len - i;
    if(*out_len + 11 > m_len)
        return ERR_WRONG_DATA;

    memcpy((uint8_t *)out, (uint8_t *)&pb[i], *out_len);
    memset((uint8_t *)pb, 0, sizeof(pb));

    return status;
}

#define INC_AND_LEN_CHECK(x, y, limit)\
do{\
  (x) = (x) + (y);\
  if ((x)>(limit))\
    return -1;\
}while (0)

static int get_len(const uint8_t *p, uint16_t len_limit, uint16_t *len, uint16_t *pos)
{
  p++;
  if (*p<128)
  {
    *pos += 2;
    *len = *p;
  }
  else if (*p==0x81)
  {
    *pos += 3;
    p++;
    *len = *p;
  }
  else if (*p==0x82)
  {
    *pos += 4;
    p++;

    *len = *p++;
    *len = (*len<<8) + *p;
  }
  else
    return -1;

  if ((*pos + *len) > len_limit)
    return -1;

  return 0;
}

static int find_item(const uint8_t *x, uint16_t len, uint16_t *pos, uint8_t end_field)
{
  uint8_t i;
  uint16_t s;
    
  for (i=0; i<end_field; i++)
  {
    if (get_len(&x[*pos], len, &s, pos)!=0)
      return -1;
    *pos += s;
  }
  return 0;
}

enum {
  TBSCERT_STRUCT_V,
  TBSCERT_STRUCT_SN,
  TBSCERT_STRUCT_SIGN_ALG,
  TBSCERT_STRUCT_ISSUER,
  TBSCERT_STRUCT_VALID,
  TBSCERT_STRUCT_SUBJ,
  TBSCERT_STRUCT_PKEY,
};

enum {
  PKEY_STRUCT_ALG,
  PKEY_STRUCT_PKEY 
};

#define TBSCERT_STRUCT_START_POS 8 

enum {
  CERT_STRUCT_CERT,
  CERT_STRUCT_SIG_ALG,
  CERT_STRUCT_SIG,
};


int calc_sha256(const uint8_t *buf, uint16_t len, uint8_t *hash)
{
	struct sha256_ctx ctx;
  
	SHA256_init(&ctx);
	SHA256_update(&ctx, buf, len);
  memcpy(hash, SHA256_final(&ctx), 32);
  return 0;
}

int calc_cert_sha256(const uint8_t *x, uint16_t len, uint8_t *hash)
{
  uint16_t s;
  uint16_t pos = 0;
  const uint8_t * cert = x + 4;
  
  if (get_len(cert, len, &s, &pos))
    return -1;
  pos += s; 
  return calc_sha256(cert, pos, hash);
}

int get_x509_signature(const uint8_t *x, uint16_t len, cont *sig)
{
  uint16_t pos = 4;

  if (find_item(x, len, &pos, CERT_STRUCT_SIG))
    return -1;

  if (get_len(&x[pos], len, &sig->len, &pos))
    return -1;
  sig->addr = &x[pos+1]; 
  sig->len -= 1;
 
  return 0; 
}

enum{
  SIG_ALG,
  SIG
};

int get_hash_from_signature(const uint8_t *sig_buf, uint16_t len, cont *hash)
{
  uint16_t pos=2;

  if (find_item(sig_buf, len, &pos, SIG)!=0)
    return -1;

  if (get_len(&sig_buf[pos], len, &hash->len, &pos)!=0)
    return -1;
  hash->addr = &sig_buf[pos];
  return 0;
}

int get_x509_pkey(const uint8_t *x, uint16_t len, cont *m, cont *e)
{
  uint16_t pos = TBSCERT_STRUCT_START_POS;

  if (find_item(x, len, &pos, TBSCERT_STRUCT_PKEY)!=0)
    return -1;
  
  INC_AND_LEN_CHECK(pos, 4, len);

  if (find_item(x, len, &pos, PKEY_STRUCT_PKEY)!=0)
    return -1;

  INC_AND_LEN_CHECK(pos, 9, len);

  if (get_len(&x[pos], len, &m->len, &pos)!=0)
    return -1;
  m->addr = &x[pos+1];
  
  pos += m->len;
  if (get_len(&x[pos], len, &e->len, &pos)!=0)
    return -1;
  e->addr = &x[pos];

  m->len -= 1;

  return 0;
}


#ifdef __cplusplus
}
#endif