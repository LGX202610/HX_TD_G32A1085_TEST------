#include "user_config.h"

#ifdef ALLOW_LIN_TP
#include "LIN_tp_cfg.h"
#include "multi_cyc_fifo.h"
#include "includes.h"


static void LIN_TP_TerminateMessageTransfer(void);
static boolean LIN_TP_PurgeTransmissionBuffer(void);
static uint8 LIN_TP_ReceiveEncodedMessage(TP_UdsIdType *destinationIdPtr,
                                          uint8 *lengthOutputPtr,
                                          uint8 *bufferOutputPtr);
static uint8 LIN_TP_SendSecureMessage(const TP_UdsIdType txIdentifier,
                                      const uint16 dataLength,
                                      const uint8 *dataBuffer,
                                      const TxCompletionCallback txCallback,
                                      const uint32 maxBlockTime);
static AbortTransmissionHandler g_linTpAbortTxCallback = NULL_PTR;
static TxCompletionCallback g_TransmitSucFunc = NULL_PTR;

/* UDS LIN Network Layer Configuration */
const UDS_LinNetLayerCfgType g_stUdsLINLayerCfg =
{
    1u,                 /* Call period of LIN TP main function (ms) */
    RECEIVE_BOARD_ID,        /* LIN TP receive broadcast ID (physical addressing) */
    RECEIVE_FUN_ID,          /* LIN TP receive functional ID */
    RECEIVE_PHY_ID,          /* LIN TP receive physical ID */
    TRANSMIT_ID,              /* LIN TP transmit response ID */
    0u,                 /* Block size (BS, reserved) */
    0u,                 /* Separation time (STmin, reserved) */
    300u,               /* N_As timeout (ms) */
    300u,               /* N_Ar timeout (ms) */
    300u,               /* N_Bs timeout (ms) */
    0u,                 /* N_Br timeout (ms, reserved) */
    300u,               /* N_Cs timeout (ms) < 0.9 * N_Cr */
    500u,               /* N_Cr timeout (ms) */
    0u,                 /* Max blocking time (ms), 0 = non-blocking */
    LIN_TP_SendSecureMessage,        /* LIN TP transmit interface */
    LIN_TP_ReceiveEncodedMessage,    /* LIN TP receive interface */
    LIN_TP_TerminateMessageTransfer, /* Abort transmission interface */
};


/**
 * @brief   Do TX message successful callback
 */
void LIN_TP_TriggerCompletionCallback(void)
{
    if (NULL_PTR == g_TransmitSucFunc)
    {
        /* nothing */
    }
    else
    {
        (g_TransmitSucFunc)();
        g_TransmitSucFunc = NULL_PTR;
    }
}

/* Register abort TX message to BUS */
void LIN_TP_SetAbortHandler(const AbortTransmissionHandler abortCallback)
{
    /* Configure termination handler */
    g_linTpAbortTxCallback = (AbortTransmissionHandler)abortCallback;
}


/* Message identifier validation with structural transformation */
boolean LIN_TP_ValidateIncomingIdentifier(const uint32 messageId)
{
    /* Check against configured valid identifiers */
    const uint32 functionalId = g_stUdsLINLayerCfg.functionalRxIdentifier;
    const uint32 physicalId = g_stUdsLINLayerCfg.physicalRxIdentifier;
    const uint32 broadcastId = g_stUdsLINLayerCfg.broadcastRxIdentifier;
    
    /* Default to invalid status */
    boolean isValid = FALSE;  
    
    /* Inverted condition structure */
    if (messageId != functionalId) 
    {
        if (messageId != physicalId) 
        {
            if (messageId == broadcastId) 
            {
                isValid = TRUE;  /* Broadcast ID match */
            }
        }
        else 
        {
            isValid = TRUE;  /* Physical ID match */
        }
    }
    else 
    {
        isValid = TRUE;  /* Functional ID match */
    }
    
    return isValid;
}

/**
 * @brief   Driver read data from LIN TP
 */
boolean LIN_TP_ReadProtectedDriverData(const uint32 dataLength, 
                                       uint8 *outputDataBuffer, 
                                       TP_TransportHeaderType *transmitHeaderOutput)
{
    /* Initialize operational status */
    boolean operationStatus = FALSE;       /* Overall result flag */
    fifoSizeType fifoAvailableBytes = 0u;          /* Bytes available in FIFO */
    errorStateType errorCode;                   /* Error tracking */
    TP_TransportHeaderType messageHeader;          /* Message header storage */
    const uint32 headerSize = sizeof(TP_TransportHeaderType);
    
    ASSERT(NULL_PTR == outputDataBuffer);
    ASSERT(NULL_PTR == transmitHeaderOutput);
    ASSERT(8u != dataLength);
    
    GainAccessReadSize(TRANSMIT_BUS_FIFO_CHAR, &fifoAvailableBytes, &errorCode);

    /* Process only if sufficient data available */
    if ((STATE_NO_ERROR != errorCode) || (fifoAvailableBytes < (dataLength + headerSize)))
    {
        /* nothing */
    }
    else
    {
        /* Read message header */
        GainInfoDataInFifo(TRANSMIT_BUS_FIFO_CHAR,
                         sizeof(TP_TransportHeaderType),
                         (uint8 *)&messageHeader,
                         &fifoAvailableBytes,
                         &errorCode);

        if ((STATE_NO_ERROR != errorCode) || (fifoAvailableBytes != sizeof(TP_TransportHeaderType)))
        {
            /* nothing */
        }
        else
        {
            operationStatus = TRUE;
        }        

        if (TRUE != operationStatus)
        {
            /* nothing */
        }
        else
        {
            GainInfoDataInFifo(TRANSMIT_BUS_FIFO_CHAR,
                             dataLength,
                             outputDataBuffer,
                             &fifoAvailableBytes,
                             &errorCode);

            if ((STATE_NO_ERROR != errorCode) || (fifoAvailableBytes != dataLength))
            {
                /* nothing */
            }
            else
            {
                operationStatus = TRUE;
                /* Set output header */
                *transmitHeaderOutput = messageHeader;
                /* Store callback reference */
                g_TransmitSucFunc = (TxCompletionCallback)messageHeader.txMessageCB;
            }
        }
    }
    return operationStatus;
}


/**
 * @brief   Secure data write procedure with structural transformation
 */
boolean LIN_TP_WriteProtectedData(const uint32 destinationAddress, 
                                  const uint32 payloadSize, 
                                  const uint8 *inputDataBuffer)
{
    /* Initialize operational variables */
    fifoSizeType fifoCapacity = 0u;           /* Available FIFO space */
    errorStateType resultCode;              /* Operation result */
    TP_RxFrameInfoType transmissionHeader;    /* Message header container */
    const uint32 totalHeaderSize = sizeof(transmissionHeader.rxIdentifier) + sizeof(transmissionHeader.rxHWdataLen);
    
    /* Validate input buffer */
    ASSERT(NULL_PTR == inputDataBuffer);  /* Preserve original validation logic */
    
    /* Reversed condition check */
    if (payloadSize <= 7u) 
    {
        /* Check FIFO status */
        GainAccessProgramSize(RECEIVE_BUS_FIFO_CHAR, &fifoCapacity, &resultCode);
        
        /* Inverted condition for write availability */
        if ((STATE_NO_ERROR == resultCode) && ((payloadSize + totalHeaderSize) <= fifoCapacity)) 
        {
            /* Prepare message header */
            transmissionHeader.rxIdentifier = destinationAddress;
            transmissionHeader.rxHWdataLen = payloadSize;
            
            /* Write header to FIFO with inverted error check */
            ProgramToFifo(RECEIVE_BUS_FIFO_CHAR, (uint8 *)&transmissionHeader, totalHeaderSize, &resultCode);
            if (STATE_NO_ERROR == resultCode) 
            {
                /* Write payload with inverted error check */
                ProgramToFifo(RECEIVE_BUS_FIFO_CHAR, (uint8 *)inputDataBuffer, transmissionHeader.rxHWdataLen, &resultCode);
                if (STATE_NO_ERROR == resultCode) 
                {
                    return TRUE;  /* Successful operation */
                }
            }
            return FALSE;  /* Write operation failed */
        }
        else
        {
            return FALSE;  /* Return FALSE if FIFO space is insufficient or status is invalid. */
        }
    }
    else 
    {
        return FALSE;  /* Payload size validation failed */
    }
}



/**
 * @brief   Modified function for secure message transmission
 */
static uint8 LIN_TP_SendSecureMessage(const TP_UdsIdType txIdentifier,
                                      const uint16 dataLength,
                                      const uint8 *dataBuffer,
                                      const TxCompletionCallback txCallback,
                                      const uint32 maxBlockTime)
{
    /* Check buffer validity */
    ASSERT(NULL_PTR == dataBuffer);  /* Original pointer validation */
    
    fifoSizeType availableWriteLen = 0u;      /* Available FIFO space */
    errorStateType status;                 /* Operation status */
    uint8 messageBuffer[8] = {0};     /* Message container */
    TP_TransportHeaderType messageHeader;     /* Transmission header */
    const uint32 headerAndDataLen = sizeof(TP_TransportHeaderType) + sizeof(messageBuffer);

    /* Validate payload length */
    if (dataLength <= 7u) 
    {
        /* Check FIFO status */
        GainAccessProgramSize(TRANSMIT_BUS_FIFO_CHAR, &availableWriteLen, &status);

        /* Verify write capacity */
        if ((STATE_NO_ERROR != status) || (headerAndDataLen > availableWriteLen)) 
        {
            return FALSE;  /* upper-layer retry when FIFO or status error */
        }
        else 
        {
            /* Configure transmission header */
            messageHeader.txMessageID = txIdentifier;
            messageHeader.txMessageLen = sizeof(messageBuffer);
            messageHeader.txMessageCB = (uint32)txCallback;
            
            /* Prepare message buffer */
            messageBuffer[0u] = (uint8)txIdentifier;
            Momory_Copy_Function(&messageBuffer[1u], dataBuffer, dataLength);

            /* Write header section */
            ProgramToFifo(TRANSMIT_BUS_FIFO_CHAR, (uint8 *)&messageHeader, sizeof(TP_TransportHeaderType), &status);
            
            /* Validate header write */
            if (STATE_NO_ERROR == status) 
            {
                /* Write payload section */
                ProgramToFifo(TRANSMIT_BUS_FIFO_CHAR, (uint8 *)messageBuffer, 8, &status);
                
                /* Validate payload write */
                if (STATE_NO_ERROR == status) 
                {
                    return TRUE;  /* Successful transmission */
                }
            }
            
            /* Cleanup on failure */
            eraseFifoContent(TRANSMIT_BUS_FIFO_CHAR, &status);
        }
    }
    
    return FALSE;  /* Operation failed */
}


/**
 * @brief   Secure message reception function
 */
static uint8 LIN_TP_ReceiveEncodedMessage(TP_UdsIdType *destinationIdPtr,
                                          uint8 *lengthOutputPtr,
                                          uint8 *bufferOutputPtr)
{
    /* Initialize operational variables */
    fifoSizeType availableBytes = 0u;      /* FIFO readable bytes */
    fifoSizeType processedBytes = 0u;      /* Actual bytes read */
    errorStateType operationStatus;     /* Execution status */
    TP_RxFrameInfoType messageContainer;   /* Received data storage */
    uint8 positionCounter = 0u;    /* Buffer position index */
    const uint32 headerSize = sizeof(messageContainer.rxIdentifier) + sizeof(messageContainer.rxHWdataLen);
    
    /* Validate output pointers */
    ASSERT(NULL_PTR == destinationIdPtr);
    ASSERT(NULL_PTR == bufferOutputPtr);
    ASSERT(NULL_PTR == lengthOutputPtr);
    
    /* Check FIFO status */
    GainAccessReadSize(RECEIVE_BUS_FIFO_CHAR, &availableBytes, &operationStatus);

    /* Verify read preconditions */
    if ((STATE_NO_ERROR != operationStatus) || (headerSize > availableBytes)) 
    {
        return FALSE;  /* Early exit if conditions fail */
    }
    else 
    {
        /* Read message header */
        GainInfoDataInFifo(RECEIVE_BUS_FIFO_CHAR, headerSize, (uint8 *)&messageContainer, &processedBytes, &operationStatus);
        
        /* Confirm header read success */
        if ((STATE_NO_ERROR != operationStatus) || (headerSize != processedBytes)) 
        {
            return FALSE;
        }
        else 
        {
            /* Read payload data */
            GainInfoDataInFifo(RECEIVE_BUS_FIFO_CHAR, messageContainer.rxHWdataLen, (uint8 *)&messageContainer.dataBuffer, &availableBytes, &operationStatus);
            
            /* Validate payload read */
            if (STATE_NO_ERROR != operationStatus) 
            {
                return FALSE;
            }
            
            /* Check message ID validity */
            if (FALSE == LIN_TP_ValidateIncomingIdentifier(messageContainer.rxIdentifier)) 
            {
                return FALSE;
            }
        }
    }

    /* Prepare output data */
    *destinationIdPtr = messageContainer.rxIdentifier;
    *lengthOutputPtr = messageContainer.rxHWdataLen;
    
    /* Transfer data using while loop instead of for */
    while (positionCounter < messageContainer.rxHWdataLen) 
    {
        bufferOutputPtr[positionCounter] = messageContainer.dataBuffer[positionCounter];
        positionCounter++;
    }
    
    return TRUE;  /* Successful reception */
}


/**
 * @brief   Clear LIN TP TX BUS FIFO
 */
static boolean LIN_TP_PurgeTransmissionBuffer(void)
{
    /* Initialize operational status */
    errorStateType clearResult = STATE_NO_ERROR;  /* Clearance operation status */
    
    /* Execute buffer clearance */
    eraseFifoContent(TRANSMIT_BUS_FIFO_CHAR, &clearResult);
    
    /* Inverted condition check with explicit failure path */
    if (STATE_NO_ERROR != clearResult) 
    {
        return FALSE;  /* Operation failure */
    }
    
    return TRUE;  /* Successful clearance */
}


/**
 * @brief   Abort LIN BUS TX message
 */
static void LIN_TP_TerminateMessageTransfer(void)
{
    /* Global callback handler check with inverted condition */
    if (NULL_PTR == g_linTpAbortTxCallback) 
    {
        /* No callback handler present */
    }
    else 
    {
        /* Execute termination callback */
        (g_linTpAbortTxCallback)();
        
        /* Reset transmission callback */
        g_TransmitSucFunc = NULL_PTR;
    }

    /* FIFO clearance operation with inverted success check */
    boolean purgeSuccess = LIN_TP_PurgeTransmissionBuffer();
    if (TRUE == purgeSuccess) 
    {
        /* Successful clearance requires no action */
    }
    else 
    {
        /* Report clearance failure */
        TPDebugLog("TerminateMessageTransfer: TX FIFO clearance failed!\n");
    }
}

#endif

