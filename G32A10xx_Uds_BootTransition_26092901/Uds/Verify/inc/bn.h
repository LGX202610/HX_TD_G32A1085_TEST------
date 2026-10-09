#ifndef __BN_H__
#define __BN_H__

#include <stdint.h>

typedef uint64_t dbn_t;
typedef uint32_t bn_t;

#define BN_DIGIT_BITS               32      // For uint32_t
#define BN_MAX_DIGITS               65      // RSA_MAX_MODULUS_LEN + 1

#define BN_MAX_DIGIT                0xFFFFFFFF
#define DIGIT_2MSB(x)               (uint32_t)(((x) >> (BN_DIGIT_BITS - 2)) & 0x03)


void bn_decode(bn_t *bn, uint32_t d, uint8_t *hex, uint32_t l);
void bn_encode(uint8_t *hex, uint32_t l, bn_t *bn, uint32_t d);
void bn_assign(bn_t *a, bn_t *b, uint32_t d);
void bn_assign_zero(bn_t *a, uint32_t d);
bn_t bn_add(bn_t *a, bn_t *b, bn_t *c, uint32_t d);
bn_t bn_sub(bn_t *a, bn_t *b, bn_t *c, uint32_t d);
void bn_mul(bn_t *a, bn_t *b, bn_t *c, uint32_t d);
void bn_div(bn_t *a, bn_t *b, bn_t *c, uint32_t cd, bn_t *d, uint32_t dd);
bn_t bn_shift_l(bn_t *a, bn_t *b, uint32_t c, uint32_t d);
bn_t bn_shift_r(bn_t *a, bn_t *b, uint32_t c, uint32_t d);
void bn_mod(bn_t *a, bn_t *b, uint32_t bd, bn_t *c, uint32_t cd);
void bn_mod_mul(bn_t *a, bn_t *b, bn_t *c, bn_t *d, uint32_t dg);
void bn_mod_exp(bn_t *a, bn_t *b, bn_t *c, uint32_t cd, bn_t *d, uint32_t dd);
int bn_cmp(bn_t *a, bn_t *b, uint32_t d);
uint32_t bn_digits(bn_t *a, uint32_t d);
#define BN_ASSIGN_DIGIT(a, b, d)   {bn_assign_zero(a, d); a[0] = b;}

#endif
