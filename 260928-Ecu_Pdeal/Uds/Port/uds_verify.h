#ifndef UDS_VERIFT_H_
#define UDS_VERIFT_H_

#include "includes.h"

int UDS_Verify_CheckFileValidity(const uint8_t* pVerifyParam);
uint8_t *UDS_Verify_GetSavedVersion(void);
uint8_t* UDS_Verify_GetSavedDigest(void);
void UDS_Verify_ClearDigest(void);
void UDS_Verify_ClearVersion(void);
int UDS_Verify_CheckDownloadedData(void);

#endif
