#ifndef BOOT_CFG_H_
#define BOOT_CFG_H_

#include "includes.h"

#ifdef UDS_PROJECT_FOR_APP
void BootloaderAccepteReq(void);
void ClearFlagForDownloadAppOk(void);
boolean VerifyDownloadAppOk(void);
#endif

#endif
