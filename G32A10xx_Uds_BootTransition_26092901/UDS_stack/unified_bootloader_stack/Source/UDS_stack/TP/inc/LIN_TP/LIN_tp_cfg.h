#ifndef LIN_TP_CFG_H
#define LIN_TP_CFG_H

#include "includes.h"

#ifdef ALLOW_LIN_TP
#include "TP_cfg.h"


#define DATA_LENGTH (7u)

#define SF_MAX_DATA_LENGTH (6u)
#define FF_MIN_DATA_LENGTH (7u)
#define CF_MAX_DATA_LENGTH (6u)
#define MAX_CF_DATA_LENGTH (150u)

/**
 * @brief   Define TX completion callback
 */
typedef void (*TxCompletionCallback)(void);

/**
 * @brief   Define block size type
 */
typedef unsigned short TP_BlockSizeType;

/**
 * @brief   Define network timing type
 */
typedef unsigned short TP_TimingType;

/**
 * @brief   Define data length type
 */
typedef uint32 TP_LengthType;

/**
 * @brief   Define ID type
 */
typedef uint32 TP_UdsIdType;

/**
 * @brief   Define LIN TP data length type
 */ 
typedef unsigned short TP_LINDataLengthType;

/**
 * @brief   Define TX abort function
 */ 
typedef void (*AbortTransmissionHandler)(void);

/**
 * @brief   Define message transmission function
 */ 
typedef unsigned char (*TxMessageHandler)(const TP_UdsIdType, const unsigned short, 
                                          const unsigned char *, 
                                          const TxCompletionCallback, 
                                          const uint32);

/**
 * @brief   Define message reception function
 */ 
typedef unsigned char (*RxMessageHandler)(TP_UdsIdType *, unsigned char *, unsigned char *);

/**
 * @brief   LIN network layer configuration structure
 */
typedef struct
{
    unsigned char executionInterval;
    TP_UdsIdType broadcastRxIdentifier;
    TP_UdsIdType functionalRxIdentifier;
    TP_UdsIdType physicalRxIdentifier;
    TP_UdsIdType transmissionIdentifier;
    TP_BlockSizeType blockLimit;
    TP_TimingType stMinSepTime;
    TP_TimingType senderTimeoutValue;
    TP_TimingType receiverTimeoutValue;
    TP_TimingType senderBufferThreshold;
    TP_TimingType receiverBufferThreshold;
    TP_TimingType senderConsecutiveLimit;
    TP_TimingType receiverConsecutiveLimit;
    uint32 maxTransmitBlockTimeMs;
    TxMessageHandler transmitMessage;
    RxMessageHandler receiveMessage;
    AbortTransmissionHandler cancelTransmission;
} UDS_LinNetLayerCfgType;

/* UDS network layer config info */
extern const UDS_LinNetLayerCfgType g_stUdsLINLayerCfg;

void LIN_TP_SetAbortHandler(const AbortTransmissionHandler abortCallback);

boolean LIN_TP_ReadProtectedDriverData(const uint32 dataLength,  uint8 *outputDataBuffer, TP_TransportHeaderType *transmitHeaderOutput);

void LIN_TP_TriggerCompletionCallback(void);

boolean LIN_TP_WriteProtectedData(const uint32 destinationAddress, const uint32 payloadSize, const uint8 *inputDataBuffer);

boolean LIN_TP_ValidateIncomingIdentifier(const uint32 messageId);

#endif

#endif

