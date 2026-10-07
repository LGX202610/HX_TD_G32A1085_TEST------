#ifndef CRC_HAL_H
#define CRC_HAL_H

#include "includes.h"

#define CRC_SEED_INIT_VALUE 0xFFFF

typedef uint32 tCrc;

boolean CRC_HAL_Init(void);
void CRC_HAL_CreatHw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc);
void CRC_HAL_CreatSw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc);
uint32_t crc32_calc(const uint8_t* data, uint32_t len);

#endif
