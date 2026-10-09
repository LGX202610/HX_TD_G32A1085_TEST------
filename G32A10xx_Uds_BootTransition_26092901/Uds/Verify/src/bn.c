#include <string.h>
#include "bn.h"

static bn_t bn_sub_digit_mul(bn_t *a, bn_t *b, bn_t c, bn_t *d, uint32_t dg);
static bn_t bn_add_digit_mul(bn_t *a, bn_t *b, bn_t c, bn_t *d, uint32_t dg);
static uint32_t bn_digit_bits(bn_t a);

void bn_decode(bn_t *bn, uint32_t dg, uint8_t *hex, uint32_t l)
{
    bn_t t;
    int j;
    uint32_t i, u;
    for(i=0,j=l-1; i<dg && j>=0; i++) {
        t = 0;
        for(u=0; j>=0 && u<BN_DIGIT_BITS; j--, u+=8) {
            t |= ((bn_t)hex[j]) << u;
        }
        bn[i] = t;
    }

    for(; i<dg; i++) {
        bn[i] = 0;
    }
}

void bn_encode(uint8_t *hex, uint32_t l, bn_t *bn, uint32_t dg)
{
    bn_t t;
    int j;
    uint32_t i, u;

    for(i=0,j=l-1; i<dg && j>=0; i++) {
        t = bn[i];
        for(u=0; j>=0 && u<BN_DIGIT_BITS; j--, u+=8) {
            hex[j] = (uint8_t)(t >> u);
        }
    }

    for(; j>=0; j--) {
        hex[j] = 0;
    }
}

void bn_assign(bn_t *a, bn_t *b, uint32_t dg)
{
    uint32_t i;
    for(i=0; i<dg; i++) {
        a[i] = b[i];
    }
}

void bn_assign_zero(bn_t *a, uint32_t dg)
{
    uint32_t i;
    for(i=0; i<dg; i++) {
        a[i] = 0;
    }
}

bn_t bn_add(bn_t *a, bn_t *b, bn_t *c, uint32_t dg)
{
    bn_t ai, cy;
    uint32_t i;

    cy = 0;
    for(i=0; i<dg; i++) {
        if((ai = b[i] + cy) < cy) {
            ai = c[i];
        } else if((ai += c[i]) < c[i]) {
            cy = 1;
        } else {
            cy = 0;
        }
        a[i] = ai;
    }

    return cy;
}

bn_t bn_sub(bn_t *a, bn_t *b, bn_t *c, uint32_t dg)
{
    bn_t ai, bw;
    uint32_t i;

    bw = 0;
    for(i=0; i<dg; i++) {
        if((ai = b[i] - bw) > (BN_MAX_DIGIT - bw)) {
            ai = BN_MAX_DIGIT - c[i];
        } else if((ai -= c[i]) > (BN_MAX_DIGIT - c[i])) {
            bw = 1;
        } else {
            bw = 0;
        }
        a[i] = ai;
    }

    return bw;
}

void bn_mul(bn_t *a, bn_t *b, bn_t *c, uint32_t dg)
{
    bn_t t[2*BN_MAX_DIGITS];
    uint32_t bd, cd, i;

    bn_assign_zero(t, 2*dg);
    bd = bn_digits(b, dg);
    cd = bn_digits(c, dg);

    for(i=0; i<bd; i++) {
        t[i+cd] += bn_add_digit_mul(&t[i], &t[i], b[i], c, cd);
    }

    bn_assign(a, t, 2*dg);
    memset((uint8_t *)t, 0, sizeof(t));
}



void bn_div(bn_t *a, bn_t *b, bn_t *c, uint32_t cd, bn_t *d, uint32_t dg)
{
    dbn_t tmp;
    bn_t ai, t, cc[2*BN_MAX_DIGITS+1], dd[BN_MAX_DIGITS];
    int i;
    uint32_t ddg, shift;
    ddg = bn_digits(d, dg);
    if(ddg == 0)
        return;
    shift = BN_DIGIT_BITS - bn_digit_bits(d[ddg-1]);
    bn_assign_zero(cc, ddg);
    cc[cd] = bn_shift_l(cc, c, shift, cd);
    bn_shift_l(dd, d, shift, ddg);
    t = dd[ddg-1];
    bn_assign_zero(a, cd);
    i = cd - ddg;
    for(; i>=0; i--) {
        if(t == BN_MAX_DIGIT) {
            ai = cc[i+ddg];
        } else {
            tmp = cc[i+ddg-1];
            tmp += (dbn_t)cc[i+ddg] << BN_DIGIT_BITS;
            ai = tmp / (t + 1);
        }
        cc[i+ddg] -= bn_sub_digit_mul(&cc[i], &cc[i], ai, dd, ddg);
        while(cc[i+ddg] || (bn_cmp(&cc[i], dd, ddg) >= 0)) {
            ai++;
            cc[i+ddg] -= bn_sub(&cc[i], &cc[i], dd, ddg);
        }
        a[i] = ai;
    }
    bn_assign_zero(b, dg);
    bn_shift_r(b, cc, shift, ddg);
    memset((uint8_t *)cc, 0, sizeof(cc));
    memset((uint8_t *)dd, 0, sizeof(dd));
}

bn_t bn_shift_l(bn_t *a, bn_t *b, uint32_t c, uint32_t dg)
{
    bn_t bi, cy;
    uint32_t i, t;

    if(c >= BN_DIGIT_BITS)
        return 0;

    t = BN_DIGIT_BITS - c;
    cy = 0;
    for(i=0; i<dg; i++) {
        bi = b[i];
        a[i] = (bi << c) | cy;
        cy = c ? (bi >> t) : 0;
    }

    return cy;
}

bn_t bn_shift_r(bn_t *a, bn_t *b, uint32_t c, uint32_t dg)
{
    bn_t bi, cy;
    int i;
    uint32_t t;

    if(c >= BN_DIGIT_BITS)
        return 0;

    t = BN_DIGIT_BITS - c;
    cy = 0;
    i = dg - 1;
    for(; i>=0; i--) {
        bi = b[i];
        a[i] = (bi >> c) | cy;
        cy = c ? (bi << t) : 0;
    }

    return cy;
}

void bn_mod(bn_t *a, bn_t *b, uint32_t bd, bn_t *c, uint32_t cd)
{
    bn_t t[2*BN_MAX_DIGITS] = {0};
    bn_div(t, a, b, bd, c, cd);
    memset((uint8_t *)t, 0, sizeof(t));
}

void bn_mod_mul(bn_t *a, bn_t *b, bn_t *c, bn_t *d, uint32_t dg)
{
    bn_t t[2*BN_MAX_DIGITS];
    bn_mul(t, b, c, dg);
    bn_mod(a, t, 2*dg, d, dg);
    memset((uint8_t *)t, 0, sizeof(t));
}

void bn_mod_exp(bn_t *a, bn_t *b, bn_t *c, uint32_t cd, bn_t *d, uint32_t dg)
{
    bn_t bp[3][BN_MAX_DIGITS], ci, t[BN_MAX_DIGITS];
    int i;
    uint32_t ci_bits, j, s;

    bn_assign(bp[0], b, dg);
    bn_mod_mul(bp[1], bp[0], b, d, dg);
    bn_mod_mul(bp[2], bp[1], b, d, dg);

    BN_ASSIGN_DIGIT(t, 1, dg);

    cd = bn_digits(c, cd);
    i = cd - 1;
    for(; i>=0; i--) {
        ci = c[i];
        ci_bits = BN_DIGIT_BITS;

        if(i == (int)(cd - 1)) {
            while(!DIGIT_2MSB(ci)) {
                ci <<= 2;
                ci_bits -= 2;
            }
        }

        for(j=0; j<ci_bits; j+=2) {
            bn_mod_mul(t, t, t, d, dg);
            bn_mod_mul(t, t, t, d, dg);
            if((s = DIGIT_2MSB(ci)) != 0) {
                bn_mod_mul(t, t, bp[s-1], d, dg);
            }
            ci <<= 2;
        }
    }

    bn_assign(a, t, dg);

    memset((uint8_t *)bp, 0, sizeof(bp));
    memset((uint8_t *)t, 0, sizeof(t));
}

int bn_cmp(bn_t *a, bn_t *b, uint32_t dg)
{
    int i;
    for(i=dg-1; i>=0; i--) {
        if(a[i] > b[i])     return 1;
        if(a[i] < b[i])     return -1;
    }

    return 0;
}

uint32_t bn_digits(bn_t *a, uint32_t dg)
{
    int i;
    for(i=dg-1; i>=0; i--) {
        if(a[i])    break;
    }

    return (i + 1);
}

static bn_t bn_add_digit_mul(bn_t *a, bn_t *b, bn_t c, bn_t *d, uint32_t dg)
{
    dbn_t res;
    bn_t cy, rh, rl;
    uint32_t i;

    if(c == 0)
        return 0;

    cy = 0;
    for(i=0; i<dg; i++) {
        res = (dbn_t)c * d[i];
        rl = res & BN_MAX_DIGIT;
        rh = (res >> BN_DIGIT_BITS) & BN_MAX_DIGIT;
        if((a[i] = b[i] + cy) < cy) {
            cy = 1;
        } else {
            cy = 0;
        }
        if((a[i] += rl) < rl) {
            cy++;
        }
        cy += rh;
    }

    return cy;
}

static bn_t bn_sub_digit_mul(bn_t *a, bn_t *b, bn_t c, bn_t *d, uint32_t dg)
{
    dbn_t res;
    bn_t bw, rh, rl;
    uint32_t i;

    if(c == 0)
        return 0;

    bw = 0;
    for(i=0; i<dg; i++) {
        res = (dbn_t)c * d[i];
        rl = res & BN_MAX_DIGIT;
        rh = (res >> BN_DIGIT_BITS) & BN_MAX_DIGIT;
        if((a[i] = b[i] - bw) > (BN_MAX_DIGIT - bw)) {
            bw = 1;
        } else {
            bw = 0;
        }
        if((a[i] -= rl) > (BN_MAX_DIGIT - rl)) {
            bw++;
        }
        bw += rh;
    }

    return bw;
}

static uint32_t bn_digit_bits(bn_t a)
{
    uint32_t i;
    for(i=0; i<BN_DIGIT_BITS; i++) {
        if(a == 0)  break;
        a >>= 1;
    }

    return i;
}
