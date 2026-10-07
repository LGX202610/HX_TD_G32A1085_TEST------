/*!
 * @file        g32a10xx_sms.h
 *
 * @brief       This file contains all the functions prototypes for the Sms firmware library
 *
 * @version     V1.0.0
 *
 * @date        2026-02-25
 *
 * @attention
 *
 *  Copyright (C) 2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (Geehy Semiconductor Software License Agreement).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the Geehy Semiconductor Software License Agreement for the governing permissions
 *  and limitations under the License.
 */

#ifndef G32A10xx_SMS_H
#define G32A10xx_SMS_H

#ifdef __cplusplus
extern "C"{
#endif

#include "g32a10xx.h"
#include "g32a10xx_misc.h"

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @defgroup SMS_Driver
  @{
*/

/**********************************************************************************************************************
                                             OTHER TYPE DEFINITION
***********************************************************************************************************************/
/** @defgroup SMS_Enumerations Enumerations
  @{
*/

/**
 * @brief    Defines the error injecting type.
 */
typedef enum
{
    SMS_INJECT_NO_ERROR = 0U,
    SMS_INJECT_SINGLE_BIT_ERROR = 0x01U,
    SMS_INJECT_DOUBLE_BIT_ERROR = 0x02U,
    SMS_INJECT_TRIPLE_BIT_ERROR = 0x03U
} Sms_InjectBitErrType;

/**@} end of group SMS_Enumerations */

/** @defgroup SMS_Structures Structures
  @{
*/

/** Defines the fault id */
typedef uint8_t Sms_FaultIdType;

/**
* @brief    The type definition of SMS configuration struct 
*/
typedef struct
{
    uint8_t smsEccEn;          //!< Enable/Disable the ECC
    uint8_t smsRdCorEn;        //!< Enable/Disable the ECC Correct,when read
    uint8_t smsRmwCorEn;       //!< Enable/Disable the ECC Correct,when read-modify-write
    uint32_t  smsInjectErrAddr;  //!< The address to be injectded faults in the Sram
} Sms_ConfigType;

/**@} end of group SMS_Structures */
/***********************************************************************************************************
                                            MACRO DEFINITION
************************************************************************************************************/
/** @defgroup SMS_Macros Macros
  @{
*/
#define SMS_NO_ERR                          (0U)

#define RD_ERR_SIN_ID                       ((Sms_FaultIdType)0U)
#define RD_ERR_DBL_ID                       ((Sms_FaultIdType)0x01U)
#define RMW_ERR_SIN_ID                      ((Sms_FaultIdType)0x02U)
#define RMW_ERR_DBL_ID                      ((Sms_FaultIdType)0x03U)

#define SMS_INTRRn_RD_ERR_DET_MASK          ((uint32_t)0x00000001U)
#define SMS_INTRRn_RMW_ERR_DET_MASK         ((uint32_t)0x00000008U)

#define SMS_INTRMn_RD_ERR_DET_MASK          ((uint32_t)0x00000001U)
#define SMS_INTRMn_RD_ERR_SIN_MASK          ((uint32_t)0x00000002U)
#define SMS_INTRMn_RD_ERR_DBL_MASK          ((uint32_t)0x00000004U)
#define SMS_INTRMn_RMW_ERR_DET_MASK         ((uint32_t)0x00000008U)
#define SMS_INTRMn_RMW_ERR_SIN_MASK         ((uint32_t)0x00000010U)
#define SMS_INTRMn_RMW_ERR_DBL_MASK         ((uint32_t)0x00000020U)

#define SMS_STAT_RD_ERR_SIN_SHIFT           ((uint8_t)0x01U)
#define SMS_STAT_RD_ERR_DBL_SHIFT           ((uint8_t)0x02U)
#define SMS_STAT_RMW_ERR_SIN_SHIFT          ((uint8_t)0x04U)
#define SMS_STAT_RMW_ERR_DBL_SHIFT          ((uint8_t)0x05U)

#define SMS_ECC_LOGn_ADDR_SHIFT             ((uint8_t)0x07U)

/* SRAM start address */
#define SMS_RAM_START_ADDRESS               (0x20000000U)
/* Total count of all errors */            
#define SMS_FAULTS_TOTAL_COUNT              (0x04U)

#define E_OK                                ((uint8_t)0U)
#define E_NOT_OK                            ((uint8_t)0x01U)

/**@} end of group SMS_Macros Macros */
/**********************************************************************************************************************
                                        INLINE FUNCTIONS
***********************************************************************************************************************/
/** @defgroup SMS_Functions Functions
  @{
*/

/*!
 * @brief Read the status flags
 *
 * @return  The flag status
 */
static inline uint32_t Sms_ReadStsFlag(void)
{
    return (SMS->STAT_R.STAT);
}

/*!
 * @brief Read the error address
 *
 * @return  The error address
 */
static inline uint32_t Sms_ReadErrorAddr(void)
{
    return (SMS->ECC_LOGn_R.ECC_LOGn_B.ADDR);
}

/*!
 * @brief Read the interrupt removing value
 *
 * @param IntMask    The interrupt bit mask value
 * @return  The interrupt removing value
 */
static inline uint32_t Sms_ReadIntRem(uint32_t IntMask)
{
    return (SMS->INTRRn_R.INTRRn & IntMask);
}

/*!
 * @brief Write the interrupt removing value
 *
 * @param Val    The value to be set
 */
static inline void Sms_WriteIntRem(uint32_t Val)
{
    SMS->INTRRn_R.INTRRn = Val;
}

/*!
 * @brief Read the interrupt mask value
 *
 * @param IntMask    The interrupt bit mask value
 * @return  The interrupt mask value
 */
static inline uint32_t Sms_ReadIntMask(uint32_t IntMask)
{
    return (SMS->INTRMn_R.INTRMn & IntMask);
}

/*!
 * @brief Enable the interrupt mask
 *
 * @param IntMask    The interrupt bit mask value
 */
static inline void Sms_EnableIntMask(uint32_t IntMask)
{
    SMS->INTRMn_R.INTRMn |= IntMask;
}

/*!
 * @brief Disable the interrupt mask
 *
 * @param IntMask    The interrupt bit mask value
 */
static inline void Sms_DisableIntMask(uint32_t IntMask)
{
    SMS->INTRMn_R.INTRMn &= (~IntMask);
}

/*!
 * @brief Read the interrupt status value
 *
 * @return  The interrupt status value
 */
static inline uint32_t Sms_ReadIntSts(void)
{
    return SMS->INTRSn_R.INTRSn;
}

/**********************************************************************************************************************
                                        FUNCTION DECLARATION
***********************************************************************************************************************/
uint8_t Sms_Init(const Sms_ConfigType *SmsConfigPtr);
uint8_t Sms_ClearFault(void);
void Sms_InjectFault(Sms_InjectBitErrType InjectBitErrType);
uint8_t Sms_GetErrors(uint32_t *FaultPoolPtr);
uint32_t Sms_GetErrorAddr(void);
uint32_t Sms_GetErrorBitPosition(uint8_t FaultId);

/**@} end of group SMS_Functions Functions */

#ifdef __cplusplus
}
#endif

#endif

/**@} end of group SMS_Driver */
/**@} end of group G32A10xx_StdPeriphDriver  */
