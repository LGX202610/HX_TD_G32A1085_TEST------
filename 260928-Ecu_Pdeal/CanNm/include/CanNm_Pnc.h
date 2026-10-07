#ifndef CANNM_PNC_H
#define CANNM_PNC_H

#include "CanNm_Types.h"


void CanNm_PncInit(const CanNm_ConfigType *ConfigPtr);

void CanNm_PncDeInit(void);

void CanNm_PncMainFunction(uint32_t ElapsedMs);

Std_ReturnType CanNm_RequestPnc(PNCHandleType PncId);

Std_ReturnType CanNm_ReleasePnc(PNCHandleType PncId);

Std_ReturnType CanNm_GetPncState(PNCHandleType PncId, CanNm_PncStateType *PncStatePtr);

void CanNm_PncRxIndication(const uint8_t *PnInfoPtr, uint8_t PnInfoLength);

void CanNm_PncCopyTxPnInfo(uint8_t *NmPduPtr, uint8_t PduLength);

boolean CanNm_PncIsIraActive(void);

boolean CanNm_PncIsEraActive(void);

boolean CanNm_PncIsEiraActive(void);

#endif /* CANNM_PNC_H */
