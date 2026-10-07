#ifndef UDS_ALG_HAL_H
#define UDS_ALG_HAL_H

#include "includes.h"

boolean UDS_ALG_HAL_DataEncryptionProcess(const uint8 *pDataIn, const uint32 dataLength, uint8 *pDataOut);
boolean UDS_ALG_HAL_DataDecryptionMethod(const uint8 *pDeCipherData, const uint32 pDataLength, uint8 *pDataBuffer);
boolean UDS_ALG_HAL_MyReplacedFunc(const uint32 needLen, uint8 *pOutBuf);
void UDS_ALG_HAL_AddSWTimerTickCnt(void);

#endif
