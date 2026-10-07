/*!
 * @file        G32A10xx.h
 *
 * @brief       CMSIS Cortex-M0 Device Peripheral Access Layer Header File.
 *
 * @details     This file contains all the peripheral register's definitions, bits definitions and memory mapping
 *
 * @version     V1.0.3
 *
 * @date        2025-07-03
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

/*!
* MISRA-C:2012 compliance checks
*
*       g32a10xx_h_MISRA_REF1
*          Breaks the required Rule-1.3 of MISRA 2012 guidelines,
*          There shall be no occurrence of undefined or critical unspecified behaviour
*
*       g32a10xx_h_MISRA_REF2
*          Breaks the required Rule-21.1 of MISRA 2012 guidelines,
*          #define and #undef shall not be used on a reserved identifier or reserved macro name
*
*       g32a10xx_h_MISRA_REF3
*          Breaks the required Namecheck4.1.0 of MISRA 2012 guidelines,
*          A grouping of messages related to: Local Standards
*
*/


/* Define to prevent recursive inclusion */
#ifndef G32A10xx_H
#define G32A10xx_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup CMSIS
  @{
*/

/** @addtogroup G32A10xx
  * @brief Peripheral Access Layer
  @{
*/
#if !defined (G32A10xx)
#error "Please select first the target G32A10xx device used in your application (in G32A10xx.h file)"
#endif


/** @defgroup HSE_Macros
  @{
*/

/**
 * @brief Define Value of the External oscillator in Hz
 */
#ifndef  HSE_VALUE
#define  HSE_VALUE              ((uint32_t)16000000)
#endif

/* Time out for HSE start up */
#define HSE_STARTUP_TIMEOUT     ((uint32_t)0x10000)

/* Time out for PLL start up */
#define PLL_STARTUP_TIMEOUT     ((uint32_t)0x10000)

/* Time out for HSI start up */
#define HSI_STARTUP_TIMEOUT    ((uint32_t)0x0500)

/* Value of the Internal oscillator in Hz */
#define HSI_VALUE              ((uint32_t)8000000)
#define HSI14_VALUE            ((uint32_t)14000000)

#define LSI_VALUE              ((uint32_t)32000)

/**@} end of group HSE_Macros */

/** @defgroup G32A10xx_StdPeripheral_Library_Version
  @{
*/

/*!< [31:16] G32A1085 Standard Peripheral Library main version V1.0.0*/
#define G32A10xx_DEVICE_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define G32A10xx_DEVICE_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define G32A10xx_DEVICE_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define G32A10xx_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define G32A10xx_DEVICE_VERSION        ((G32A10xx_DEVICE_VERSION_MAIN << 24)\
                                       |(G32A10xx_DEVICE_VERSION_SUB1 << 16)\
                                       |(G32A10xx_DEVICE_VERSION_SUB2 << 8 )\
                                       |(G32A10xx_DEVICE_VERSION_RC))

/**@} end of group G32A10xx_StdPeripheral_Library_Version */

/** @defgroup Configuraion_for_CMSIS
  @{
*/

/** @warning g32a10xx_h_MISRA_REF1 [U] The macro identifier '__CM0PLUS_REV' is reserved. */
/** @warning g32a10xx_h_MISRA_REF1 [U] The identifier '__CM0PLUS_REV' is reserved for use by the library. */
/** @warning g32a10xx_h_MISRA_REF3 [U] The identifier '__CM0PLUS_REV' does not conform to the name rule. */
/* Core Revision r0p1  */
#define __CM0PLUS_REV             0
/** @warning g32a10xx_h_MISRA_REF1 [U] The macro identifier '__MPU_PRESENT' is reserved. */
/** @warning g32a10xx_h_MISRA_REF1 [U] The identifier '__MPU_PRESENT' is reserved for use by the library. */
/** @warning g32a10xx_h_MISRA_REF3 [U] The identifier '__MPU_PRESENT' does not conform to the name rule. */
/* G32A1085 do not provide MPU  */
#define __MPU_PRESENT             0
/** @warning g32a10xx_h_MISRA_REF1 [U] The macro identifier '__NVIC_PRIO_BITS' is reserved. */
/** @warning g32a10xx_h_MISRA_REF1 [U] The identifier '__NVIC_PRIO_BITS' is reserved for use by the library. */
/** @warning g32a10xx_h_MISRA_REF3 [U] The identifier '__NVIC_PRIO_BITS' does not conform to the name rule. */
/* G32A1085 uses 2 Bits for the Priority Levels */
#define __NVIC_PRIO_BITS          2
/** @warning g32a10xx_h_MISRA_REF1 [U] The macro identifier '__Vendor_SysTickConfig' is reserved. */
/** @warning g32a10xx_h_MISRA_REF1 [U] The identifier '__Vendor_SysTickConfig' is reserved for use by the library. */
/** @warning g32a10xx_h_MISRA_REF3 [U] The identifier '__Vendor_SysTickConfig' does not conform to the name rule. */
/* Set to 1 if different SysTick Config is used */
#define __Vendor_SysTickConfig    0
/** @warning g32a10xx_h_MISRA_REF1 [U] The macro identifier '__VTOR_PRESENT' is reserved. */
/** @warning g32a10xx_h_MISRA_REF1 [U] The identifier '__VTOR_PRESENT' is reserved for use by the library. */
/** @warning g32a10xx_h_MISRA_REF3 [U] The identifier '__VTOR_PRESENT' does not conform to the name rule. */
/* Set to 1 if different Vector Table Offset is used */
#define __VTOR_PRESENT            1

/**
 * @brief    Interrupt Number Definition
 */
typedef enum
{
    /*  Cortex-M0 Processor Exceptions Numbers */
    NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt */
    HardFault_IRQn              = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt */
    SVC_IRQn                    = -5,     /*!< 11 Cortex-M0 SV Call Interrupt */
    PendSV_IRQn                 = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt */
    SysTick_IRQn                = -1,     /*!< 15 Cortex-M0 System Tick Interrupt */

    /*  G32A1085 specific Interrupt Numbers */
    PVD_IRQn                    =  1,     /*!< PVD global Interrupt */
    RTC_IRQn                    =  2,     /*!< RTC Interrupt through EINT Lines 17, 19 and 20 */
    FLASH_IRQn                  =  3,     /*!< FLASH global Interrupt */
    RCM_IRQn                    =  4,     /*!< RCM global Interrupt */
    EINT0_1_IRQn                =  5,     /*!< EINT Line 0 and 1 Interrupt */
    EINT2_3_IRQn                =  6,     /*!< EINT Line 2 and 3 Interrupt */
    EINT4_15_IRQn               =  7,     /*!< EINT Line 4 to 15 Interrupt */
    SMS_IRQn                    =  8,     /*!< SRAM ECC Interrupt */
    DMA_CH1_IRQn                =  9,     /*!< DMA Channel 1 Interrupt */
    DMA_CH2_3_IRQn              = 10,     /*!< DMA Channel 2 and Channel 3 Interrupt */
    DMA_CH4_5_IRQn              = 11,     /*!< DMA Channel 4 and Channel 5 Interrupt */
    ADC_IRQn                    = 12,     /*!< ADC Interrupt */
    TMR1_BRK_UP_TRG_COM_IRQn    = 13,     /*!< TMR1 Break, Update, Trigger and Commutation Interrupt */
    TMR1_CC_IRQn                = 14,     /*!< TMR1 Capture Compare Interrupt */
    TMR2_IRQn                   = 15,     /*!< TMR2 global Interrupt */
    TMR3_IRQn                   = 16,     /*!< TMR3 global Interrupt */
    TMR6_IRQn                   = 17,     /*!< TMR6 global Interrupt */
    TMR7_IRQn                   = 18,     /*!< TMR7 global Interrupt */
    TMR4_IRQn                   = 19,     /*!< TMR4 global Interrupt */
    SHA256_IRQn                 = 21,     /*!< SHA256 global Interrupt */
    AES256_IRQn                 = 22,     /*!< AES256 global Interrupt */
    CAN_IT0_IRQn                = 23,     /*!< Can Line 0 Interrupt */
    CAN_IT1_IRQn                = 24,     /*!< CAN Line 1 Interrupt */
    SPI_IRQn                    = 25,     /*!< SPI global Interrupt */
    USART1_IRQn                 = 27,     /*!< USART1 global Interrupt */
    USART2_IRQn                 = 28,     /*!< USART2 global Interrupt */
    TRNG_IRQn                   = 29,     /*!< TRNG global Interrupt */
    TIM8_IRQn                   = 30,     /*!< TIM8 global Interrupt */
    CAN_SMS_IT_IRQn             = 31      /*!< CAN SRAM ECC and Downtime wake up Interrupt */
} IRQn_Type;

/**@} end of group Configuraion_for_CMSIS */

/* Includes */

#include "core_cm0plus.h"     /*!< Cortex-M0+ processor and core peripherals */
#include "system_g32a10xx.h" /*!<G32A085 System Header */
#include <stdint.h>

/** @defgroup Exported_types
  * @{
*/

typedef enum {FALSE, TRUE} BOOL;

enum {BIT_RESET = 0U, BIT_SET = 1U};

enum {RESET = 0U, SET = 1U};

enum {DISABLE = 0U, ENABLE = 1U};

enum {ERROR = 0U, SUCCESS = 1U};

#ifndef __IM
#define __IM   __I
#endif
#ifndef __OM
#define __OM   __O
#endif
#ifndef __IOM
#define __IOM  __IO
#endif

/** @warning g32a10xx_h_MISRA_REF2 [U] The macro 'NULL' is also defined in '<stdef.h>'. */
#ifndef NULL
#define NULL   ((void *)0)
#endif

#if defined (__CC_ARM )
#pragma anon_unions
#endif

/**@} end of group Exported_types */

/** @defgroup Peripheral_registers_structures
  @{
*/

/**
  * @brief Analog-to-digital converter (ADC)
  */

typedef struct
{
    /* interrupt and status register */
    union
    {
        __IOM uint32_t STS;

        struct
        {
            __IOM uint32_t ADCRDYFLG  : 1;
            __IOM uint32_t EOSMPFLG   : 1;
            __IOM uint32_t EOCFLG     : 1;
            __IOM uint32_t EOSEQFLG   : 1;
            __IOM uint32_t OVREFLG    : 1;
            __IM  uint32_t RESERVED1  : 2;
            __IOM uint32_t AWDFLG     : 1;
            __IM  uint32_t RESERVED2  : 24;
        } STS_B;
    } STS_R;

    /* interrupt enable register */
    union
    {
        __IOM uint32_t IEN;

        struct
        {
            __IOM uint32_t ADCRDYIEN  : 1;
            __IOM uint32_t EOSMPIEN   : 1;
            __IOM uint32_t EOCIEN     : 1;
            __IOM uint32_t EOSEQIEN   : 1;
            __IOM uint32_t OVRIEN     : 1;
            __IM  uint32_t RESERVED1  : 2;
            __IOM uint32_t AWDIEN     : 1;
            __IM  uint32_t RESERVED2  : 24;
        } IEN_B;
    } IEN_R;

    /* control register */
    union
    {
        __IOM uint32_t CTRL;

        struct
        {
            __IOM uint32_t ADCEN      : 1;
            __IOM uint32_t ADCD       : 1;
            __IOM uint32_t STARTCEN   : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t STOPCEN    : 1;
            __IM  uint32_t RESERVED2  : 26;
            __IOM uint32_t CAL        : 1;
        } CTRL_B;
    } CTRL_R;

    /* configuration register 1 */
    union
    {
        __IOM uint32_t CFG1;

        struct
        {
            __IOM uint32_t DMAEN      : 1;
            __IOM uint32_t DMACFG     : 1;
            __IOM uint32_t SCANSEQDIR : 1;
            __IOM uint32_t DATARESCFG : 2;
            __IOM uint32_t DALIGCFG   : 1;
            __IOM uint32_t EXTTRGSEL  : 3;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t EXTPOLSEL  : 2;
            __IOM uint32_t OVRMAG     : 1;
            __IOM uint32_t CMODESEL   : 1;
            __IOM uint32_t WAITCEN    : 1;
            __IOM uint32_t AOEN       : 1;
            __IOM uint32_t DISCEN     : 1;
            __IM  uint32_t RESERVED2  : 5;
            __IOM uint32_t AWDCHEN    : 1;
            __IOM uint32_t AWDEN      : 1;
            __IM  uint32_t RESERVED3  : 2;
            __IOM uint32_t AWDCHSEL   : 5;
            __IM  uint32_t RESERVED4  : 1;
        } CFG1_B;
    } CFG1_R;

    /* configuration register 2 */
    union
    {
        __IOM uint32_t CFG2;

        struct
        {
            __IM  uint32_t RESERVED1  : 30;
            __IOM uint32_t CLKCFG     : 2;
        } CFG2_B;
    } CFG2_R;

    /* sampling time register */
    union
    {
        __IOM uint32_t SMPTIM;

        struct
        {
            __IOM uint32_t SMPCYCSEL  : 3;
            __IM  uint32_t RESERVED1  : 29;
        } SMPTIM_B;
    } SMPTIM_R;
    __IM  uint32_t  RESERVED[2];

    /* watchdog threshold register */
    union
    {
        __IOM uint32_t AWDT;

        struct
        {
            __IOM uint32_t AWDLT      : 12;
            __IM  uint32_t RESERVED1  : 4;
            __IOM uint32_t AWDHT      : 12;
            __IM  uint32_t RESERVED2  : 4;
        } AWDT_B;
    } AWDT_R;
    __IM  uint32_t  RESERVED1;

    /* channel selection register */
    union
    {
        __IOM uint32_t CHSEL;

        struct
        {
            __IOM uint32_t CH0SEL     : 1;
            __IOM uint32_t CH1SEL     : 1;
            __IOM uint32_t CH2SEL     : 1;
            __IOM uint32_t CH3SEL     : 1;
            __IOM uint32_t CH4SEL     : 1;
            __IOM uint32_t CH5SEL     : 1;
            __IOM uint32_t CH6SEL     : 1;
            __IOM uint32_t CH7SEL     : 1;
            __IOM uint32_t CH8SEL     : 1;
            __IOM uint32_t CH9SEL     : 1;
            __IOM uint32_t CH10SEL    : 1;
            __IOM uint32_t CH11SEL    : 1;
            __IOM uint32_t CH12SEL    : 1;
            __IOM uint32_t CH13SEL    : 1;
            __IOM uint32_t CH14SEL    : 1;
            __IOM uint32_t CH15SEL    : 1;
            __IOM uint32_t CH16SEL    : 1;
            __IOM uint32_t CH17SEL    : 1;
            __IM  uint32_t RESERVED1  : 14;
        } CHSEL_B;
    } CHSEL_R;
    __IM  uint32_t  RESERVED2[5];

    /* data register */
    union
    {
        __IM  uint32_t DATA;

        struct
        {
            __IM  uint32_t CDATA       : 16;
            __IM  uint32_t RESERVED1  : 16;
        } DATA_B;
    } DATA_R;
    __IM  uint32_t  RESERVED3[175];

    /* Analog Power Switching Register */
    union
    {
        __IOM uint32_t ANA_SWITCH;

        struct
        {
            __IOM  uint32_t ADC_ANA_SWITCH   : 2;
            __IM  uint32_t RESERVED1         : 30;
        } ANA_SWITCH_B;
    } ANA_SWITCH_R;
    __IM  uint32_t  RESERVED4;

    /* common configuration register */
    union
    {
        __IOM uint32_t CCFG;

        struct
        {
            __IM  uint32_t RESERVED1  : 22;
            __IOM uint32_t VREFEN     : 1;
            __IOM uint32_t TSEN       : 1;
            __IM  uint32_t RESERVED2  : 8;
        } CCFG_B;
    } CCFG_R;
} ADC_T;

/**
* @brief Advanced Encryption Standard 256-bit (AES256)
*/

typedef struct
{
    /* Data Input Register */
    union
    {
        __IOM  uint32_t DATAIN_0;

        struct
        {
            __IOM  uint32_t IN_0      : 32;
        } DATAIN_0_B;
    } DATAIN_0_R;

    /* Data Input Register */
    union
    {
        __IOM  uint32_t DATAIN_1;

        struct
        {
            __IOM  uint32_t IN_1      : 32;
        } DATAIN_1_B;
    } DATAIN_1_R;

    /* Data Input Register */
    union
    {
        __IOM  uint32_t DATAIN_2;

        struct
        {
            __IOM  uint32_t IN_2      : 32;
        } DATAIN_2_B;
    } DATAIN_2_R;

    /* Data Input Register */
    union
    {
        __IOM  uint32_t DATAIN_3;

        struct
        {
            __IOM  uint32_t IN_3      : 32;
        } DATAIN_3_B;
    } DATAIN_3_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_0;

        struct
        {
            __IOM  uint32_t KEY_0      : 32;
        } KEY_0_B;
    } KEY_0_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_1;

        struct
        {
            __IOM  uint32_t KEY_1      : 32;
        } KEY_1_B;
    } KEY_1_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_2;

        struct
        {
            __IOM  uint32_t KEY_2      : 32;
        } KEY_2_B;
    } KEY_2_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_3;

        struct
        {
            __IOM  uint32_t KEY_3      : 32;
        } KEY_3_B;
    } KEY_3_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_4;

        struct
        {
            __IOM  uint32_t KEY_4      : 32;
        } KEY_4_B;
    } KEY_4_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_5;

        struct
        {
            __IOM  uint32_t KEY_5      : 32;
        } KEY_5_B;
    } KEY_5_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_6;

        struct
        {
            __IOM  uint32_t KEY_6      : 32;
        } KEY_6_B;
    } KEY_6_R;

    /* Key Register */
    union
    {
        __IOM  uint32_t KEY_7;

        struct
        {
            __IOM  uint32_t KEY_7      : 32;
        } KEY_7_B;
    } KEY_7_R;

    /* Initialize the vector register */
    union
    {
        __IOM  uint32_t IV_0;

        struct
        {
            __IOM  uint32_t IV_0      : 32;
        } IV_0_B;
    } IV_0_R;

    /* Initialize the vector register */
    union
    {
        __IOM  uint32_t IV_1;

        struct
        {
            __IOM  uint32_t IV_1      : 32;
        } IV_1_B;
    } IV_1_R;

    /* Initialize the vector register */
    union
    {
        __IOM  uint32_t IV_2;

        struct
        {
            __IOM  uint32_t IV_2      : 32;
        } IV_2_B;
    } IV_2_R;

    /* Initialize the vector register */
    union
    {
        __IOM  uint32_t IV_3;

        struct
        {
            __IOM  uint32_t IV_3      : 32;
        } IV_3_B;
    } IV_3_R;

    /* Control register */
    union
    {
        __IOM  uint32_t CTRL;

        struct
        {
            __IOM  uint32_t START        : 1;
            __IOM  uint32_t KEY_INT_EN   : 1;
            __IOM  uint32_t DATA_INT_EN  : 1;
            __IOM  uint32_t BIGGER_ENDIAN   : 1;
            __IOM  uint32_t KEY_LEN      : 2;
            __IOM  uint32_t OPCODE       : 2;
            __IOM  uint32_t MODE         : 4;
            __IOM  uint32_t VALID_LENTH  : 7;
            __IOM  uint32_t SubKG        : 1;
            __IM   uint32_t RESERVED1    : 12;
        } CTRL_B;
    } CTRL_R;

    /* State register */
    union
    {
        __IOM  uint32_t STATE;

        struct
        {
            __IM   uint32_t BUSY          : 1;
            __IOM  uint32_t KEY_INT_FLG   : 1;
            __IOM  uint32_t DATA_INT_FLG  : 1;
            __IM   uint32_t RESERVED1     : 29;
        } STATE_B;
    } STATE_R;

    /* Data Onput Register */
    union
    {
        __IOM  uint32_t DATAOUT_0;

        struct
        {
            __IOM  uint32_t OUT_0      : 32;
        } DATAOUT_0_B;
    } DATAOUT_0_R;

    /* Data Onput Register */
    union
    {
        __IOM  uint32_t DATAOUT_1;

        struct
        {
            __IOM  uint32_t OUT_1      : 32;
        } DATAOUT_1_B;
    } DATAOUT_1_R;

    /* Data Onput Register */
    union
    {
        __IOM  uint32_t DATAOUT_2;

        struct
        {
            __IOM  uint32_t OUT_2      : 32;
        } DATAOUT_2_B;
    } DATAOUT_2_R;

    /* Data Onput Register */
    union
    {
        __IOM  uint32_t DATAOUT_3;

        struct
        {
            __IOM  uint32_t OUT_3      : 32;
        } DATAOUT_3_B;
    } DATAOUT_3_R;
} AES256_T;

/**
* @brief Controller Area Network (CAN)
*/

typedef struct
{
    /* Core Release Register (CREL) */
    union
    {
        __IOM uint32_t CREL;

        struct
        {
            __IM  uint32_t DAY      : 8;
            __IM  uint32_t MON      : 8;
            __IM  uint32_t YEAR     : 4;
            __IOM uint32_t SUBSTEP  : 4;
            __IOM uint32_t STEP     : 4;
            __IOM uint32_t REL      : 4;
        } CREL_B;
    } CREL_R;

    /* Endian Register (ENDN) */
    union
    {
        __IM uint32_t ENDN;

        struct
        {
            __IM  uint32_t ETV      : 32;
        } ENDN_B;
    } ENDN_R;

    /* reserved */
    __IM  uint32_t  RESERVED[1];

    /* Data Bit Timing & Prescaler Register (DBTP) */
    union
    {
        __IOM uint32_t DBTP;

        struct
        {
            __IOM  uint32_t DSJW        : 4;
            __IOM  uint32_t DTSEG2      : 4;
            __IOM  uint32_t DTSEG1      : 5;
            __IM   uint32_t RESERVED1   : 3;
            __IOM  uint32_t DBRP        : 5;
            __IM   uint32_t RESERVED2   : 2;
            __IOM  uint32_t TDC         : 1;
            __IM   uint32_t RESERVED3   : 8;
        } DBTP_B;
    } DBTP_R;

    /* Test Register (TEST) */
    union
    {
        __IOM uint32_t TEST;

        struct
        {
            __IM   uint32_t RESERVED1   : 4;
            __IOM  uint32_t LBCK        : 1;
            __IOM  uint32_t TX          : 2;
            __IM   uint32_t RX          : 1;
            __IM   uint32_t RESERVED2   : 24;
        } TEST_B;
    } TEST_R;

    /* RAM Watchdog (RWD) */
    union
    {
        __IOM uint32_t RWD;

        struct
        {
            __IOM  uint32_t WDC        : 8;
            __IM   uint32_t WDV        : 8;
            __IM   uint32_t RESERVED1  : 16;
        } RWD_B;
    } RWD_R;

    /* CC Control Register (CCCR) */
    union
    {
        __IOM uint32_t CCCR;

        struct
        {
            __IOM  uint32_t INIT       : 1;
            __IOM  uint32_t CCE        : 1;
            __IOM  uint32_t ASM        : 1;
            __IM   uint32_t CSA        : 1;
            __IOM  uint32_t CSR        : 1;
            __IOM  uint32_t MON        : 1;
            __IOM  uint32_t DAR        : 1;
            __IOM  uint32_t TEST       : 1;
            __IOM  uint32_t FDOE       : 1;
            __IOM  uint32_t BRSE       : 1;
            __IM   uint32_t RESERVED1  : 1;
            __IOM  uint32_t WMM        : 1;
            __IOM  uint32_t PXHD       : 1;
            __IOM  uint32_t EFBI       : 1;
            __IOM  uint32_t TXP        : 1;
            __IOM  uint32_t NISO       : 1;
            __IM   uint32_t RESERVED2  : 16;
        } CCCR_B;
    } CCCR_R;

    /* Nominal Bit Timing & Prescaler Register (NBTP) */
    union
    {
        __IOM uint32_t NBTP;

        struct
        {
            __IOM  uint32_t NTSEG2     : 7;
            __IM   uint32_t RESERVED1  : 1;
            __IOM  uint32_t NTSEG1     : 8;
            __IOM  uint32_t NBRP       : 9;
            __IOM  uint32_t NSJW       : 7;
        } NBTP_B;
    } NBTP_R;

    /* Timestamp Counter Configuration (TSCC) */
    union
    {
        __IOM uint32_t TSCC;

        struct
        {
            __IOM  uint32_t TSS        : 2;
            __IM   uint32_t RESERVED1  : 14;
            __IOM  uint32_t TCP        : 4;
            __IM   uint32_t RESERVED2  : 12;
        } TSCC_B;
    } TSCC_R;

    /* Timestamp Counter Value (TSCV) */
    union
    {
        __IOM uint32_t TSCV;

        struct
        {
            __IOM  uint32_t TSC        : 16;
            __IM   uint32_t RESERVED1  : 16;
        } TSCV_B;
    } TSCV_R;

    /* Timeout Counter Configuration (TOCC) */
    union
    {
        __IOM uint32_t TOCC;

        struct
        {
            __IOM  uint32_t ETOC       : 1;
            __IOM  uint32_t TOS        : 2;
            __IM   uint32_t RESERVED1  : 13;
            __IOM  uint32_t TOP        : 16;
        } TOCC_B;
    } TOCC_R;

    /* Timeout Counter Value (TOCV) */
    union
    {
        __IM uint32_t TOCV;

        struct
        {
            __IM  uint32_t TOC        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } TOCV_B;
    } TOCV_R;

    /* reserved */
    __IM  uint32_t  RESERVED1[4];

    /* Error Counter Register (ECR) */
    union
    {
        __IM uint32_t ECR;

        struct
        {
            __IM  uint32_t TEC        : 8;
            __IM  uint32_t REC        : 7;
            __IM  uint32_t RP         : 1;
            __IM  uint32_t CEL        : 8;
            __IM  uint32_t RESERVED1  : 8;
        } ECR_B;
    } ECR_R;

    /* Protocol Status Register (PSR) */
    union
    {
        __IM uint32_t PSR;

        struct
        {
            __IM  uint32_t LEC        : 3;
            __IM  uint32_t ACT        : 2;
            __IM  uint32_t EP         : 1;
            __IM  uint32_t EW         : 1;
            __IM  uint32_t BO         : 1;
            __IM  uint32_t DLEC       : 3;
            __IM  uint32_t RESI       : 1;
            __IM  uint32_t RBRS       : 1;
            __IM  uint32_t RFDF       : 1;
            __IM  uint32_t PXE        : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IM  uint32_t TDCV       : 7;
            __IM  uint32_t RESERVED2  : 9;
        } PSR_B;
    } PSR_R;

    /* Transmitter Delay Compensation Register (TDCR) */
    union
    {
        __IM uint32_t TDCR;

        struct
        {
            __IOM  uint32_t TDCF       : 7;
            __IM  uint32_t RESERVED1  : 1;
            __IOM  uint32_t TDCO       : 7;
            __IM  uint32_t RESERVED2  : 17;
        } TDCR_B;
    } TDCR_R;

    /* reserved */
    __IM  uint32_t  RESERVED2[1];

    /* Interrupt Register (IR) */
    union
    {
        __IOM uint32_t IR;

        struct
        {
            __IOM  uint32_t RF0N       : 1;
            __IOM  uint32_t RF0W       : 1;
            __IOM  uint32_t RF0F       : 1;
            __IOM  uint32_t RF0L       : 1;
            __IOM  uint32_t RF1N       : 1;
            __IOM  uint32_t RF1W       : 1;
            __IOM  uint32_t RF1F       : 1;
            __IOM  uint32_t RF1L       : 1;
            __IOM  uint32_t HPM        : 1;
            __IOM  uint32_t TC         : 1;
            __IOM  uint32_t TCF        : 1;
            __IOM  uint32_t TFE        : 1;
            __IOM  uint32_t TEFN       : 1;
            __IOM  uint32_t TEFW       : 1;
            __IOM  uint32_t TEFF       : 1;
            __IOM  uint32_t TEFL       : 1;
            __IOM  uint32_t TSW        : 1;
            __IOM  uint32_t MRAF       : 1;
            __IOM  uint32_t TOO        : 1;
            __IOM  uint32_t DRX        : 1;
            __IM   uint32_t RESERVED1  : 2;
            __IOM  uint32_t ELO        : 1;
            __IOM  uint32_t EP         : 1;
            __IOM  uint32_t EW         : 1;
            __IOM  uint32_t BO         : 1;
            __IOM  uint32_t WDI        : 1;
            __IOM  uint32_t PEA        : 1;
            __IOM  uint32_t PED        : 1;
            __IOM  uint32_t ARA        : 1;
            __IM   uint32_t RESERVED2  : 2;
        } IR_B;
    } IR_R;

    /* Interrupt Enable (IE) */
    union
    {
        __IOM uint32_t IE;

        struct
        {
            __IOM  uint32_t RF0NE       : 1;
            __IOM  uint32_t RE0WE       : 1;
            __IOM  uint32_t RF0FE       : 1;
            __IOM  uint32_t RF0LE       : 1;
            __IOM  uint32_t RF1NE       : 1;
            __IOM  uint32_t RE1WE       : 1;
            __IOM  uint32_t RF1FE       : 1;
            __IOM  uint32_t RF1LE       : 1;
            __IOM  uint32_t HPME        : 1;
            __IOM  uint32_t TCE         : 1;
            __IOM  uint32_t TCFE        : 1;
            __IOM  uint32_t TFEE        : 1;
            __IOM  uint32_t TEFNE       : 1;
            __IOM  uint32_t TEFWE       : 1;
            __IOM  uint32_t TEFFE       : 1;
            __IOM  uint32_t TEFLE       : 1;
            __IOM  uint32_t TSWE        : 1;
            __IOM  uint32_t MRAFE       : 1;
            __IOM  uint32_t TOOE        : 1;
            __IOM  uint32_t DRXE        : 1;
            __IM   uint32_t RESERVED1   : 2;
            __IOM  uint32_t ELOE        : 1;
            __IOM  uint32_t EPE         : 1;
            __IOM  uint32_t EWE         : 1;
            __IOM  uint32_t BOE         : 1;
            __IOM  uint32_t WDIE        : 1;
            __IOM  uint32_t PEAE        : 1;
            __IOM  uint32_t PEDE        : 1;
            __IOM  uint32_t ARAE        : 1;
            __IM   uint32_t RESERVED2   : 2;
        } IE_B;
    } IE_R;

    /* Interrupt Line Select (ILS) */
    union
    {
        __IOM uint32_t ILS;

        struct
        {
            __IOM  uint32_t RF0NL       : 1;
            __IOM  uint32_t RE0WL       : 1;
            __IOM  uint32_t RF0FL       : 1;
            __IOM  uint32_t RF0LL       : 1;
            __IOM  uint32_t RF1NL       : 1;
            __IOM  uint32_t RE1WL       : 1;
            __IOM  uint32_t RF1FL       : 1;
            __IOM  uint32_t RF1LL       : 1;
            __IOM  uint32_t HPML        : 1;
            __IOM  uint32_t TCL         : 1;
            __IOM  uint32_t TCFL        : 1;
            __IOM  uint32_t TFEL        : 1;
            __IOM  uint32_t TEFNL       : 1;
            __IOM  uint32_t TEFWL       : 1;
            __IOM  uint32_t TEFFL       : 1;
            __IOM  uint32_t TEFLL       : 1;
            __IOM  uint32_t TSWL        : 1;
            __IOM  uint32_t MRAFL       : 1;
            __IOM  uint32_t TOOL        : 1;
            __IOM  uint32_t DRXL        : 1;
            __IM   uint32_t RESERVED1   : 2;
            __IOM  uint32_t ELOL        : 1;
            __IOM  uint32_t EPL         : 1;
            __IOM  uint32_t EWL         : 1;
            __IOM  uint32_t BOL         : 1;
            __IOM  uint32_t WDIL        : 1;
            __IOM  uint32_t PEAL        : 1;
            __IOM  uint32_t PEDL        : 1;
            __IOM  uint32_t ARAL        : 1;
            __IM   uint32_t RESERVED2   : 2;
        } ILS_B;
    } ILS_R;

    /* Interrupt Line Enable (ILE) */
    union
    {
        __IOM uint32_t ILE;

        struct
        {
            __IOM  uint32_t EINT0      : 1;
            __IOM  uint32_t EINT1      : 1;
            __IM   uint32_t RESERVED1  : 30;
        } ILE_B;
    } ILE_R;

    /* reserved */
    __IM  uint32_t  RESERVED3[8];

    /* Global Filter Configuration (GFC) */
    union
    {
        __IM uint32_t GFC;

        struct
        {
            __IOM  uint32_t RRFE       : 1;
            __IOM  uint32_t RRFS       : 1;
            __IOM  uint32_t ANFE       : 2;
            __IOM  uint32_t ANFS       : 2;
            __IM  uint32_t RESERVED1  : 26;
        } GFC_B;
    } GFC_R;

    /* Standard ID Filter Configuration (SIDFC) */
    union
    {
        __IM uint32_t SIDFC;

        struct
        {
            __IM  uint32_t RESERVED1  : 2;
            __IOM  uint32_t FLSSA      : 14;
            __IOM  uint32_t LSS        : 8;
            __IM  uint32_t RESERVED2  : 8;
        } SIDFC_B;
    } SIDFC_R;

    /* Extended ID Filter Configuration (XIDFC) */
    union
    {
        __IM uint32_t XIDFC;

        struct
        {
            __IM  uint32_t RESERVED1  : 2;
            __IOM  uint32_t FLESA      : 14;
            __IOM  uint32_t LSE        : 7;
            __IM  uint32_t RESERVED2  : 9;
        } XIDFC_B;
    } XIDFC_R;

    /* reserved */
    __IM  uint32_t  RESERVED4[1];

    /* Extended ID AND Mask (XIDAM) */
    union
    {
        __IM uint32_t XIDAM;

        struct
        {
            __IOM  uint32_t EIDM      : 29;
            __IM  uint32_t RESERVED1  : 3;
        } XIDAM_B;
    } XIDAM_R;

    /* High Priority Message Status (HPMS) */
    union
    {
        __IM uint32_t HPMS;

        struct
        {
            __IM  uint32_t BIDX       : 6;
            __IM  uint32_t MSI        : 2;
            __IM  uint32_t FIDX       : 7;
            __IM  uint32_t FLST       : 1;
            __IM  uint32_t RESERVED1  : 16;
        } HPMS_B;
    } HPMS_R;

    /* New Data 1 (NDAT1) */
    union
    {
        __IOM uint32_t NDAT1;

        struct
        {
            __IOM  uint32_t  ND0  : 1;
            __IOM  uint32_t  ND1  : 1;
            __IOM  uint32_t  ND2  : 1;
            __IOM  uint32_t  ND3  : 1;
            __IOM  uint32_t  ND4  : 1;
            __IOM  uint32_t  ND5  : 1;
            __IOM  uint32_t  ND6  : 1;
            __IOM  uint32_t  ND7  : 1;
            __IOM  uint32_t  ND8  : 1;
            __IOM  uint32_t  ND9  : 1;
            __IOM  uint32_t  ND10 : 1;
            __IOM  uint32_t  ND11 : 1;
            __IOM  uint32_t  ND12 : 1;
            __IOM  uint32_t  ND13 : 1;
            __IOM  uint32_t  ND14 : 1;
            __IOM  uint32_t  ND15 : 1;
            __IOM  uint32_t  ND16 : 1;
            __IOM  uint32_t  ND17 : 1;
            __IOM  uint32_t  ND18 : 1;
            __IOM  uint32_t  ND19 : 1;
            __IOM  uint32_t  ND20 : 1;
            __IOM  uint32_t  ND21 : 1;
            __IOM  uint32_t  ND22 : 1;
            __IOM  uint32_t  ND23 : 1;
            __IOM  uint32_t  ND24 : 1;
            __IOM  uint32_t  ND25 : 1;
            __IOM  uint32_t  ND26 : 1;
            __IOM  uint32_t  ND27 : 1;
            __IOM  uint32_t  ND28 : 1;
            __IOM  uint32_t  ND29 : 1;
            __IOM  uint32_t  ND30 : 1;
            __IOM  uint32_t  ND31 : 1;
        } NDAT1_B;
    } NDAT1_R;

    /* New Data 2 (NDAT2) */
    union
    {
        __IOM uint32_t NDAT2;

        struct
        {
            __IOM  uint32_t  ND32 : 1;
            __IOM  uint32_t  ND33 : 1;
            __IOM  uint32_t  ND34 : 1;
            __IOM  uint32_t  ND35 : 1;
            __IOM  uint32_t  ND36 : 1;
            __IOM  uint32_t  ND37 : 1;
            __IOM  uint32_t  ND38 : 1;
            __IOM  uint32_t  ND39 : 1;
            __IOM  uint32_t  ND40 : 1;
            __IOM  uint32_t  ND41 : 1;
            __IOM  uint32_t  ND42 : 1;
            __IOM  uint32_t  ND43 : 1;
            __IOM  uint32_t  ND44 : 1;
            __IOM  uint32_t  ND45 : 1;
            __IOM  uint32_t  ND46 : 1;
            __IOM  uint32_t  ND47 : 1;
            __IOM  uint32_t  ND48 : 1;
            __IOM  uint32_t  ND49 : 1;
            __IOM  uint32_t  ND50 : 1;
            __IOM  uint32_t  ND51 : 1;
            __IOM  uint32_t  ND52 : 1;
            __IOM  uint32_t  ND53 : 1;
            __IOM  uint32_t  ND54 : 1;
            __IOM  uint32_t  ND55 : 1;
            __IOM  uint32_t  ND56 : 1;
            __IOM  uint32_t  ND57 : 1;
            __IOM  uint32_t  ND58 : 1;
            __IOM  uint32_t  ND59 : 1;
            __IOM  uint32_t  ND60 : 1;
            __IOM  uint32_t  ND61 : 1;
            __IOM  uint32_t  ND62 : 1;
            __IOM  uint32_t  ND63 : 1;
        } NDAT2_B;
    } NDAT2_R;

    /* Rx FIFO 0 Configuration (RXF0C) */
    union
    {
        __IM uint32_t RXF0C;

        struct
        {
            __IM  uint32_t RESERVED1   : 2;
            __IOM  uint32_t F0SA        : 14;
            __IOM  uint32_t F0S         : 7;
            __IOM  uint32_t RESERVED2   : 1;
            __IOM  uint32_t F0WM        : 7;
            __IOM  uint32_t F0OM        : 1;
        } RXF0C_B;
    } RXF0C_R;

    /* Rx FIFO 0 Status (RXF0S) */
    union
    {
        __IM uint32_t RXF0S;

        struct
        {
            __IM  uint32_t F0FL        : 7;
            __IM  uint32_t RESERVED1   : 1;
            __IM  uint32_t F0GI        : 6;
            __IM  uint32_t RESERVED2   : 2;
            __IM  uint32_t F0PI        : 6;
            __IM  uint32_t RESERVED3   : 2;
            __IM  uint32_t F0F         : 1;
            __IM  uint32_t RF0L        : 1;
            __IM  uint32_t RESERVED4   : 6;
        } RXF0S_B;
    } RXF0S_R;

    /* Rx FIFO 0 Acknowledge (RXF0A) */
    union
    {
        __IOM uint32_t RXF0A;

        struct
        {
            __IOM  uint32_t F0AI        : 6;
            __IM   uint32_t RESERVED1   : 16;
        } RXF0A_B;
    } RXF0A_R;

    /* Rx Buffer Configuration (RXBC) */
    union
    {
        __IM uint32_t RXBC;

        struct
        {
            __IM  uint32_t RESERVED1   : 2;
            __IOM  uint32_t RBSA       : 14;
            __IM  uint32_t RESERVED2   : 16;
        } RXBC_B;
    } RXBC_R;

    /* Rx FIFO 1 Configuration (RXF1C) */
    union
    {
        __IM uint32_t RXF1C;

        struct
        {
            __IM  uint32_t RESERVED1   : 2;
            __IOM  uint32_t F1SA        : 14;
            __IOM  uint32_t F1S         : 7;
            __IOM  uint32_t RESERVED2   : 1;
            __IOM  uint32_t F1WM        : 7;
            __IOM  uint32_t F1OM        : 1;
        } RXF1C_B;
    } RXF1C_R;

    /* Rx FIFO 1 Status (RXF1S) */
    union
    {
        __IM uint32_t RXF1S;

        struct
        {
            __IM  uint32_t F1FL        : 7;
            __IM  uint32_t RESERVED1   : 1;
            __IM  uint32_t F1GI        : 6;
            __IM  uint32_t RESERVED2   : 2;
            __IM  uint32_t F1PI        : 6;
            __IM  uint32_t RESERVED3   : 2;
            __IM  uint32_t F1F         : 1;
            __IM  uint32_t RF1L        : 1;
            __IM  uint32_t RESERVED4   : 6;
        } RXF1S_B;
    } RXF1S_R;

    /* Rx FIFO 1 Acknowledge (RXF1A) */
    union
    {
        __IOM uint32_t RXF1A;

        struct
        {
            __IOM  uint32_t F1AI        : 6;
            __IM   uint32_t RESERVED1   : 16;
        } RXF1A_B;
    } RXF1A_R;

    /* Rx Buffer / FIFO Element Size Configuration (RXESC) */
    union
    {
        __IM uint32_t RXESC;

        struct
        {
            __IOM  uint32_t F0DS        : 3;
            __IM  uint32_t RESERVED1   : 1;
            __IOM  uint32_t F1DS        : 3;
            __IM  uint32_t RESERVED2   : 1;
            __IOM  uint32_t RBDS        : 3;
            __IM  uint32_t RESERVED3   : 21;
        } RXESC_B;
    } RXESC_R;

    /* Tx Buffer Configuration (TXBC) */
    union
    {
        __IM uint32_t TXBC;

        struct
        {
            __IM  uint32_t RESERVED1   : 2;
            __OM  uint32_t TBSA        : 14;
            __IOM  uint32_t NDTB        : 6;
            __IM  uint32_t RESERVED2   : 2;
            __IOM  uint32_t TFQS        : 6;
            __IOM  uint32_t TFQM        : 1;
            __IM  uint32_t RESERVED3   : 1;
        } TXBC_B;
    } TXBC_R;

    /* Tx FIFO/Queue Status (TXFQS) */
    union
    {
        __IM uint32_t TXFQS;

        struct
        {
            __IM  uint32_t TFFL        : 6;
            __IM  uint32_t RESERVED1   : 2;
            __IM  uint32_t TFGI        : 5;
            __IM  uint32_t RESERVED2   : 3;
            __IM  uint32_t TFQPI       : 5;
            __IM  uint32_t TFQF        : 1;
            __IM  uint32_t RESERVED3   : 10;
        } TXFQS_B;
    } TXFQS_R;

    /* Tx Buffer Element Size Configuration (TXESC) */
    union
    {
        __IM uint32_t TXESC;

        struct
        {
            __IOM  uint32_t TBDS        : 3;
            __IM  uint32_t RESERVED1   : 29;
        } TXESC_B;
    } TXESC_R;

    /* Tx Buffer Request Pending (TXBRP) */
    union
    {
        __IM uint32_t TXBRP;

        struct
        {
            __IM  uint32_t  TRP0  : 1;
            __IM  uint32_t  TRP1  : 1;
            __IM  uint32_t  TRP2  : 1;
            __IM  uint32_t  TRP3  : 1;
            __IM  uint32_t  TRP4  : 1;
            __IM  uint32_t  TRP5  : 1;
            __IM  uint32_t  TRP6  : 1;
            __IM  uint32_t  TRP7  : 1;
            __IM  uint32_t  TRP8  : 1;
            __IM  uint32_t  TRP9  : 1;
            __IM  uint32_t  TRP10 : 1;
            __IM  uint32_t  TRP11 : 1;
            __IM  uint32_t  TRP12 : 1;
            __IM  uint32_t  TRP13 : 1;
            __IM  uint32_t  TRP14 : 1;
            __IM  uint32_t  TRP15 : 1;
            __IM  uint32_t  TRP16 : 1;
            __IM  uint32_t  TRP17 : 1;
            __IM  uint32_t  TRP18 : 1;
            __IM  uint32_t  TRP19 : 1;
            __IM  uint32_t  TRP20 : 1;
            __IM  uint32_t  TRP21 : 1;
            __IM  uint32_t  TRP22 : 1;
            __IM  uint32_t  TRP23 : 1;
            __IM  uint32_t  TRP24 : 1;
            __IM  uint32_t  TRP25 : 1;
            __IM  uint32_t  TRP26 : 1;
            __IM  uint32_t  TRP27 : 1;
            __IM  uint32_t  TRP28 : 1;
            __IM  uint32_t  TRP29 : 1;
            __IM  uint32_t  TRP30 : 1;
            __IM  uint32_t  TRP31 : 1;
        } TXBRP_B;
    } TXBRP_R;

    /* Tx Buffer Add Request (TXBAR) */
    union
    {
        __IOM uint32_t TXBAR;

        struct
        {
            __IOM  uint32_t  AR0  : 1;
            __IOM  uint32_t  AR1  : 1;
            __IOM  uint32_t  AR2  : 1;
            __IOM  uint32_t  AR3  : 1;
            __IOM  uint32_t  AR4  : 1;
            __IOM  uint32_t  AR5  : 1;
            __IOM  uint32_t  AR6  : 1;
            __IOM  uint32_t  AR7  : 1;
            __IOM  uint32_t  AR8  : 1;
            __IOM  uint32_t  AR9  : 1;
            __IOM  uint32_t  AR10 : 1;
            __IOM  uint32_t  AR11 : 1;
            __IOM  uint32_t  AR12 : 1;
            __IOM  uint32_t  AR13 : 1;
            __IOM  uint32_t  AR14 : 1;
            __IOM  uint32_t  AR15 : 1;
            __IOM  uint32_t  AR16 : 1;
            __IOM  uint32_t  AR17 : 1;
            __IOM  uint32_t  AR18 : 1;
            __IOM  uint32_t  AR19 : 1;
            __IOM  uint32_t  AR20 : 1;
            __IOM  uint32_t  AR21 : 1;
            __IOM  uint32_t  AR22 : 1;
            __IOM  uint32_t  AR23 : 1;
            __IOM  uint32_t  AR24 : 1;
            __IOM  uint32_t  AR25 : 1;
            __IOM  uint32_t  AR26 : 1;
            __IOM  uint32_t  AR27 : 1;
            __IOM  uint32_t  AR28 : 1;
            __IOM  uint32_t  AR29 : 1;
            __IOM  uint32_t  AR30 : 1;
            __IOM  uint32_t  AR31 : 1;
        } TXBAR_B;
    } TXBAR_R;

    /* Tx Buffer Cancellation Request (TXBCR) */
    union
    {
        __IOM uint32_t TXBCR;

        struct
        {
            __IOM  uint32_t  CR0  : 1;
            __IOM  uint32_t  CR1  : 1;
            __IOM  uint32_t  CR2  : 1;
            __IOM  uint32_t  CR3  : 1;
            __IOM  uint32_t  CR4  : 1;
            __IOM  uint32_t  CR5  : 1;
            __IOM  uint32_t  CR6  : 1;
            __IOM  uint32_t  CR7  : 1;
            __IOM  uint32_t  CR8  : 1;
            __IOM  uint32_t  CR9  : 1;
            __IOM  uint32_t  CR10 : 1;
            __IOM  uint32_t  CR11 : 1;
            __IOM  uint32_t  CR12 : 1;
            __IOM  uint32_t  CR13 : 1;
            __IOM  uint32_t  CR14 : 1;
            __IOM  uint32_t  CR15 : 1;
            __IOM  uint32_t  CR16 : 1;
            __IOM  uint32_t  CR17 : 1;
            __IOM  uint32_t  CR18 : 1;
            __IOM  uint32_t  CR19 : 1;
            __IOM  uint32_t  CR20 : 1;
            __IOM  uint32_t  CR21 : 1;
            __IOM  uint32_t  CR22 : 1;
            __IOM  uint32_t  CR23 : 1;
            __IOM  uint32_t  CR24 : 1;
            __IOM  uint32_t  CR25 : 1;
            __IOM  uint32_t  CR26 : 1;
            __IOM  uint32_t  CR27 : 1;
            __IOM  uint32_t  CR28 : 1;
            __IOM  uint32_t  CR29 : 1;
            __IOM  uint32_t  CR30 : 1;
            __IOM  uint32_t  CR31 : 1;
        } TXBCR_B;
    } TXBCR_R;

    /* Tx Buffer Transmission Occurred (TXBTO) */
    union
    {
        __IOM uint32_t TXBTO;

        struct
        {
            __IOM  uint32_t  TO0  : 1;
            __IOM  uint32_t  TO1  : 1;
            __IOM  uint32_t  TO2  : 1;
            __IOM  uint32_t  TO3  : 1;
            __IOM  uint32_t  TO4  : 1;
            __IOM  uint32_t  TO5  : 1;
            __IOM  uint32_t  TO6  : 1;
            __IOM  uint32_t  TO7  : 1;
            __IOM  uint32_t  TO8  : 1;
            __IOM  uint32_t  TO9  : 1;
            __IOM  uint32_t  TO10 : 1;
            __IOM  uint32_t  TO11 : 1;
            __IOM  uint32_t  TO12 : 1;
            __IOM  uint32_t  TO13 : 1;
            __IOM  uint32_t  TO14 : 1;
            __IOM  uint32_t  TO15 : 1;
            __IOM  uint32_t  TO16 : 1;
            __IOM  uint32_t  TO17 : 1;
            __IOM  uint32_t  TO18 : 1;
            __IOM  uint32_t  TO19 : 1;
            __IOM  uint32_t  TO20 : 1;
            __IOM  uint32_t  TO21 : 1;
            __IOM  uint32_t  TO22 : 1;
            __IOM  uint32_t  TO23 : 1;
            __IOM  uint32_t  TO24 : 1;
            __IOM  uint32_t  TO25 : 1;
            __IOM  uint32_t  TO26 : 1;
            __IOM  uint32_t  TO27 : 1;
            __IOM  uint32_t  TO28 : 1;
            __IOM  uint32_t  TO29 : 1;
            __IOM  uint32_t  TO30 : 1;
            __IOM  uint32_t  TO31 : 1;
        } TXBTO_B;
    } TXBTO_R;

    /* Tx Buffer Cancellation Finished (TXBCF) */
    union
    {
        __IOM uint32_t TXBCF;

        struct
        {
            __IOM  uint32_t  CF0  : 1;
            __IOM  uint32_t  CF1  : 1;
            __IOM  uint32_t  CF2  : 1;
            __IOM  uint32_t  CF3  : 1;
            __IOM  uint32_t  CF4  : 1;
            __IOM  uint32_t  CF5  : 1;
            __IOM  uint32_t  CF6  : 1;
            __IOM  uint32_t  CF7  : 1;
            __IOM  uint32_t  CF8  : 1;
            __IOM  uint32_t  CF9  : 1;
            __IOM  uint32_t  CF10 : 1;
            __IOM  uint32_t  CF11 : 1;
            __IOM  uint32_t  CF12 : 1;
            __IOM  uint32_t  CF13 : 1;
            __IOM  uint32_t  CF14 : 1;
            __IOM  uint32_t  CF15 : 1;
            __IOM  uint32_t  CF16 : 1;
            __IOM  uint32_t  CF17 : 1;
            __IOM  uint32_t  CF18 : 1;
            __IOM  uint32_t  CF19 : 1;
            __IOM  uint32_t  CF20 : 1;
            __IOM  uint32_t  CF21 : 1;
            __IOM  uint32_t  CF22 : 1;
            __IOM  uint32_t  CF23 : 1;
            __IOM  uint32_t  CF24 : 1;
            __IOM  uint32_t  CF25 : 1;
            __IOM  uint32_t  CF26 : 1;
            __IOM  uint32_t  CF27 : 1;
            __IOM  uint32_t  CF28 : 1;
            __IOM  uint32_t  CF29 : 1;
            __IOM  uint32_t  CF30 : 1;
            __IOM  uint32_t  CF31 : 1;
        } TXBCF_B;
    } TXBCF_R;

    /* Tx Buffer Transmission Interrupt Enable (TXBTIE) */
    union
    {
        __IOM uint32_t TXBTIE;

        struct
        {
            __IOM  uint32_t  TIE0  : 1;
            __IOM  uint32_t  TIE1  : 1;
            __IOM  uint32_t  TIE2  : 1;
            __IOM  uint32_t  TIE3  : 1;
            __IOM  uint32_t  TIE4  : 1;
            __IOM  uint32_t  TIE5  : 1;
            __IOM  uint32_t  TIE6  : 1;
            __IOM  uint32_t  TIE7  : 1;
            __IOM  uint32_t  TIE8  : 1;
            __IOM  uint32_t  TIE9  : 1;
            __IOM  uint32_t  TIE10 : 1;
            __IOM  uint32_t  TIE11 : 1;
            __IOM  uint32_t  TIE12 : 1;
            __IOM  uint32_t  TIE13 : 1;
            __IOM  uint32_t  TIE14 : 1;
            __IOM  uint32_t  TIE15 : 1;
            __IOM  uint32_t  TIE16 : 1;
            __IOM  uint32_t  TIE17 : 1;
            __IOM  uint32_t  TIE18 : 1;
            __IOM  uint32_t  TIE19 : 1;
            __IOM  uint32_t  TIE20 : 1;
            __IOM  uint32_t  TIE21 : 1;
            __IOM  uint32_t  TIE22 : 1;
            __IOM  uint32_t  TIE23 : 1;
            __IOM  uint32_t  TIE24 : 1;
            __IOM  uint32_t  TIE25 : 1;
            __IOM  uint32_t  TIE26 : 1;
            __IOM  uint32_t  TIE27 : 1;
            __IOM  uint32_t  TIE28 : 1;
            __IOM  uint32_t  TIE29 : 1;
            __IOM  uint32_t  TIE30 : 1;
            __IOM  uint32_t  TIE31 : 1;
        } TXBTIE_B;
    } TXBTIE_R;

    /* Tx Buffer Cancellation Finished Interrupt Enable (TXBCIE) */
    union
    {
        __IOM uint32_t TXBCIE;

        struct
        {
            __IOM  uint32_t  CFIE0  : 1;
            __IOM  uint32_t  CFIE1  : 1;
            __IOM  uint32_t  CFIE2  : 1;
            __IOM  uint32_t  CFIE3  : 1;
            __IOM  uint32_t  CFIE4  : 1;
            __IOM  uint32_t  CFIE5  : 1;
            __IOM  uint32_t  CFIE6  : 1;
            __IOM  uint32_t  CFIE7  : 1;
            __IOM  uint32_t  CFIE8  : 1;
            __IOM  uint32_t  CFIE9  : 1;
            __IOM  uint32_t  CFIE10 : 1;
            __IOM  uint32_t  CFIE11 : 1;
            __IOM  uint32_t  CFIE12 : 1;
            __IOM  uint32_t  CFIE13 : 1;
            __IOM  uint32_t  CFIE14 : 1;
            __IOM  uint32_t  CFIE15 : 1;
            __IOM  uint32_t  CFIE16 : 1;
            __IOM  uint32_t  CFIE17 : 1;
            __IOM  uint32_t  CFIE18 : 1;
            __IOM  uint32_t  CFIE19 : 1;
            __IOM  uint32_t  CFIE20 : 1;
            __IOM  uint32_t  CFIE21 : 1;
            __IOM  uint32_t  CFIE22 : 1;
            __IOM  uint32_t  CFIE23 : 1;
            __IOM  uint32_t  CFIE24 : 1;
            __IOM  uint32_t  CFIE25 : 1;
            __IOM  uint32_t  CFIE26 : 1;
            __IOM  uint32_t  CFIE27 : 1;
            __IOM  uint32_t  CFIE28 : 1;
            __IOM  uint32_t  CFIE29 : 1;
            __IOM  uint32_t  CFIE30 : 1;
            __IOM  uint32_t  CFIE31 : 1;
        } TXBCIE_B;
    } TXBCIE_R;

    /* reserved */
    __IM  uint32_t  RESERVED5[2];

    /* Tx Event FIFO Configuration (TXEFC) */
    union
    {
        __IM uint32_t TXEFC;

        struct
        {
            __IM  uint32_t RESERVED1   : 2;
            __IOM  uint32_t EFSA        : 14;
            __IOM  uint32_t EFS         : 6;
            __IM  uint32_t RESERVED2   : 2;
            __IOM  uint32_t EFWM        : 6;
            __IM  uint32_t RESERVED3   : 2;
        } TXEFC_B;
    } TXEFC_R;

    /* Tx Event FIFO Status (TXEFS) */
    union
    {
        __IM uint32_t TXEFS;

        struct
        {
            __IM  uint32_t EFFL        : 6;
            __IM  uint32_t RESERVED1   : 2;
            __IM  uint32_t EFGI        : 5;
            __IM  uint32_t RESERVED2   : 3;
            __IM  uint32_t EFPI        : 5;
            __IM  uint32_t RESERVED3   : 3;
            __IM  uint32_t EFF         : 1;
            __IM  uint32_t TEFL        : 1;
            __IM  uint32_t RESERVED4   : 6;
        } TXEFS_B;
    } TXEFS_R;

    /* Tx Event FIFO Acknowledge (TXEFA) */
    union
    {
        __IOM uint32_t TXEFA;

        struct
        {
            __IOM  uint32_t EFAI        : 5;
            __IM   uint32_t RESERVED1   : 27;
        } TXEFA_B;
    } TXEFA_R;
} CAN_T;

/**
* @brief  Controller Area Network SRAM ECC(CAN_ERM)
*/

typedef struct
{
    /* ERM Control register CTRL */
    union
    {
        __IOM uint32_t CTRL;

        struct
        {
            __IOM  uint32_t ECC_EN         : 1;
            __IOM  uint32_t BEC_INT_EN     : 1;
            __IOM  uint32_t BEU_INT_EN     : 1;
            __IOM  uint32_t WAKE_INT_EN    : 1;
            __IOM  uint32_t WAKE_EVENT_EN  : 1;
            __IM   uint32_t RESERVED1      : 27;
        } CTRL_B;
    } CTRL_R;

    /* ERM Status register STS */
    union
    {
        __IOM uint32_t STS;

        struct
        {
            __IOM  uint32_t BEC_FLAG         : 1;
            __IOM  uint32_t BEU_FLAG         : 1;
            __IOM  uint32_t CAN_WAKEUP_FLAG  : 1;
            __IM   uint32_t RESERVED1        : 29;
        } STS_B;
    } STS_R;

    /* ERM ECC Log register (ECCLOG) */
    union
    {
        __IOM uint32_t ECCLOG;

        struct
        {
            __IOM  uint32_t ERR_ADDR         : 9;
            __IM   uint32_t RESERVED1        : 23;
        } ECCLOG_B;
    } ECCLOG_R;
} CAN_ERM_T;

/**
* @brief Cyclic redundancy check calculation unit (CRC)
*/

typedef struct
{
    /* Data register */
    union
    {
        __IOM uint32_t DATA;

        struct
        {
            __IOM uint32_t DATA       : 32;
        } DATA_B;
    } DATA_R;

    /* Independent data register */
    union
    {
        __IOM uint32_t INDATA;

        struct
        {
            __IOM uint32_t INDATA     : 8;
            __IM  uint32_t RESERVED1  : 24;
        } INDATA_B;
    } INDATA_R;

    /* Control register */
    union
    {
        __IOM uint32_t CTRL;

        struct
        {
            __IOM uint32_t RST        : 1;
            __IM  uint32_t RESERVED1  : 4;
            __IOM uint32_t REVI       : 2;
            __IOM uint32_t REVO       : 1;
            __IM  uint32_t RESERVED2  : 24;
        } CTRL_B;
    } CTRL_R;
    __IM  uint32_t  RESERVED[1];

    /* Initial CRC value */
    union
    {
        __IOM uint32_t INITVAL;

        struct
        {
            __IOM uint32_t VALUE      : 32;
        } INITVAL_B;
    } INITVAL_R;
} CRC_T;

/**
  * @brief Debug support (DBGMCU)
  */

typedef struct
{

    /* MCU Device ID Code Register */
    union
    {
        __IM  uint32_t IDCODE;

        struct
        {
            __IM  uint32_t EQR        : 12;
            __IM  uint32_t RESERVED1  : 4;
            __IM  uint32_t WVR        : 16;
        } IDCODE_B;
    } IDCODE_R;

    /* Debug MCU Configuration Register */
    union
    {
        __IOM uint32_t CFG;

        struct
        {
            __IM  uint32_t RESERVED1        : 1;
            __IOM uint32_t STOP_CLK_STS     : 1;
            __IOM uint32_t STANDBY_CLK_STS  : 1;
            __IM  uint32_t RESERVED2        : 29;
        } CFG_B;
    } CFG_R;

    /* APB Low Freeze Register */
    union
    {
        __IOM uint32_t APB1F;

        struct
        {
            __IOM uint32_t TMR2_STS   : 1;
            __IOM uint32_t TMR3_STS   : 1;
            __IM  uint32_t RESERVED1  : 2;
            __IOM uint32_t TMR6_STS   : 1;
            __IM  uint32_t RESERVED2  : 3;
            __IOM uint32_t TMR4_STS   : 1;
            __IM  uint32_t RESERVED3  : 1;
            __IOM uint32_t RTC_STS    : 1;
            __IM  uint32_t RESERVED4  : 1;
            __IOM uint32_t IWDT_STS   : 1;
            __IM  uint32_t RESERVED5  : 19;
        } APB1F_B;
    } APB1F_R;

    /* APB High Freeze Register */
    union
    {
        __IOM uint32_t APB2F;

        struct
        {
            __IM  uint32_t RESERVED1  : 11;
            __IOM uint32_t TMR1_STS   : 1;
            __IM  uint32_t RESERVED2  : 4;
            __IOM uint32_t TMR7_STS   : 1;
            __IOM uint32_t TMR8_STS   : 1;
            __IM  uint32_t RESERVED3  : 14;
        } APB2F_B;
    } APB2F_R;
} DBG_T;

/**
  * @brief DMA controller (DMA)
  */

typedef struct
{

    /* DMA interrupt status register */
    union
    {
        __IM  uint32_t INTSTS;

        struct
        {
            __IM  uint32_t GINTFLG1   : 1;
            __IM  uint32_t TCFLG1     : 1;
            __IM  uint32_t HTFLG1     : 1;
            __IM  uint32_t TERRFLG1   : 1;
            __IM  uint32_t GINTFLG2   : 1;
            __IM  uint32_t TCFLG2     : 1;
            __IM  uint32_t HTFLG2     : 1;
            __IM  uint32_t TERRFLG2   : 1;
            __IM  uint32_t GINTFLG3   : 1;
            __IM  uint32_t TCFLG3     : 1;
            __IM  uint32_t HTFLG3     : 1;
            __IM  uint32_t TERRFLG3   : 1;
            __IM  uint32_t GINTFLG4   : 1;
            __IM  uint32_t TCFLG4     : 1;
            __IM  uint32_t HTFLG4     : 1;
            __IM  uint32_t TERRFLG4   : 1;
            __IM  uint32_t GINTFLG5   : 1;
            __IM  uint32_t TCFLG5     : 1;
            __IM  uint32_t HTFLG5     : 1;
            __IM  uint32_t TERRFLG5   : 1;
            __IM  uint32_t RESERVED1  : 12;
        } INTSTS_B;
    } INTSTS_R;

    /* DMA interrupt flag clear register */
    union
    {
        __OM  uint32_t INTFCLR;

        struct
        {
            __OM  uint32_t GINTCLR1   : 1;
            __OM  uint32_t TCCLR1     : 1;
            __OM  uint32_t HTCLR1     : 1;
            __OM  uint32_t TERRCLR1   : 1;
            __OM  uint32_t GINTCLR2   : 1;
            __OM  uint32_t TCCLR2     : 1;
            __OM  uint32_t HTCLR2     : 1;
            __OM  uint32_t TERRCLR2   : 1;
            __OM  uint32_t GINTCLR3   : 1;
            __OM  uint32_t TCCLR3     : 1;
            __OM  uint32_t HTCLR3     : 1;
            __OM  uint32_t TERRCLR3   : 1;
            __OM  uint32_t GINTCLR4   : 1;
            __OM  uint32_t TCCLR4     : 1;
            __OM  uint32_t HTCLR4     : 1;
            __OM  uint32_t TERRCLR4   : 1;
            __OM  uint32_t GINTCLR5   : 1;
            __OM  uint32_t TCCLR5     : 1;
            __OM  uint32_t HTCLR5     : 1;
            __OM  uint32_t TERRCLR5   : 1;
            __IM  uint32_t RESERVED1  : 12;
        } INTFCLR_B;
    } INTFCLR_R;

    /* DMA channel 1 configuration register */
    union
    {
        __IOM uint32_t CHCFG1;

        struct
        {
            __IOM uint32_t CHEN       : 1;
            __IOM uint32_t TCINTEN    : 1;
            __IOM uint32_t HTINTEN    : 1;
            __IOM uint32_t TERRINTEN  : 1;
            __IOM uint32_t DIRCFG     : 1;
            __IOM uint32_t CIRMODE    : 1;
            __IOM uint32_t PERIMODE   : 1;
            __IOM uint32_t MIMODE     : 1;
            __IOM uint32_t PERSIZE    : 2;
            __IOM uint32_t MSIZE      : 2;
            __IOM uint32_t CHPL       : 2;
            __IOM uint32_t M2MMODE    : 1;
            __IM  uint32_t RESERVED1  : 17;
        } CHCFG1_B;
    } CHCFG1_R;

    /* DMA channel 1 number of data register */
    union
    {
        __IOM uint32_t CHNDATA1;

        struct
        {
            __IOM uint32_t NDATAT     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CHNDATA1_B;
    } CHNDATA1_R;

    /* DMA channel 1 peripheral address register */
    union
    {
        __IOM uint32_t CHPADDR1;

        struct
        {
            __IOM uint32_t PERADDR    : 32;
        } CHPADDR1_B;
    } CHPADDR1_R;

    /* DMA channel 1 memory address register */
    union
    {
        __IOM uint32_t CHMADDR1;

        struct
        {
            __IOM uint32_t MEMADDR     : 32;
        } CHMADDR1_B;
    } CHMADDR1_R;
    __IM  uint32_t  RESERVED1;

    /* DMA channel 2 configuration register */
    union
    {
        __IOM uint32_t CHCFG2;

        struct
        {
            __IOM uint32_t CHEN       : 1;
            __IOM uint32_t TCINTEN    : 1;
            __IOM uint32_t HTINTEN    : 1;
            __IOM uint32_t TERRINTEN  : 1;
            __IOM uint32_t DIRCFG     : 1;
            __IOM uint32_t CIRMODE    : 1;
            __IOM uint32_t PERIMODE   : 1;
            __IOM uint32_t MIMODE     : 1;
            __IOM uint32_t PERSIZE    : 2;
            __IOM uint32_t MSIZE      : 2;
            __IOM uint32_t CHPL       : 2;
            __IOM uint32_t M2MMODE    : 1;
            __IM  uint32_t RESERVED1  : 17;
        } CHCFG2_B;
    } CHCFG2_R;

    /* DMA channel 2 number of data register */
    union
    {
        __IOM uint32_t CHNDATA2;

        struct
        {
            __IOM uint32_t NDATAT     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CHNDATA2_B;
    } CHNDATA2_R;

    /* DMA channel 2 peripheral address register */
    union
    {
        __IOM uint32_t CHPADDR2;

        struct
        {
            __IOM uint32_t PERADDR    : 32;
        } CHPADDR2_B;
    } CHPADDR2_R;

    /* DMA channel 2 memory address register */
    union
    {
        __IOM uint32_t CHMADDR2;

        struct
        {
            __IOM uint32_t MEMADDR     : 32;
        } CHMADDR2_B;
    } CHMADDR2_R;
    __IM  uint32_t  RESERVED2;

    /* DMA channel 3 configuration register */
    union
    {
        __IOM uint32_t CHCFG3;

        struct
        {
            __IOM uint32_t CHEN       : 1;
            __IOM uint32_t TCINTEN    : 1;
            __IOM uint32_t HTINTEN    : 1;
            __IOM uint32_t TERRINTEN  : 1;
            __IOM uint32_t DIRCFG     : 1;
            __IOM uint32_t CIRMODE    : 1;
            __IOM uint32_t PERIMODE   : 1;
            __IOM uint32_t MIMODE     : 1;
            __IOM uint32_t PERSIZE    : 2;
            __IOM uint32_t MSIZE      : 2;
            __IOM uint32_t CHPL       : 2;
            __IOM uint32_t M2MMODE    : 1;
            __IM  uint32_t RESERVED1  : 17;
        } CHCFG3_B;
    } CHCFG3_R;

    /* DMA channel 3 number of data register */
    union
    {
        __IOM uint32_t CHNDATA3;

        struct
        {
            __IOM uint32_t NDATAT     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CHNDATA3_B;
    } CHNDATA3_R;

    /* DMA channel 3 peripheral address register */
    union
    {
        __IOM uint32_t CHPADDR3;

        struct
        {
            __IOM uint32_t PERADDR    : 32;
        } CHPADDR3_B;
    } CHPADDR3_R;

    /* DMA channel 3 memory address register */
    union
    {
        __IOM uint32_t CHMADDR3;

        struct
        {
            __IOM uint32_t MEMADDR    : 32;
        } CHMADDR3_B;
    } CHMADDR3_R;
    __IM  uint32_t  RESERVED3;

    /* DMA channel 4 configuration register */
    union
    {
        __IOM uint32_t CHCFG4;

        struct
        {
            __IOM uint32_t CHEN       : 1;
            __IOM uint32_t TCINTEN    : 1;
            __IOM uint32_t HTINTEN    : 1;
            __IOM uint32_t TERRINTEN  : 1;
            __IOM uint32_t DIRCFG     : 1;
            __IOM uint32_t CIRMODE    : 1;
            __IOM uint32_t PERIMODE   : 1;
            __IOM uint32_t MIMODE     : 1;
            __IOM uint32_t PERSIZE    : 2;
            __IOM uint32_t MSIZE      : 2;
            __IOM uint32_t CHPL       : 2;
            __IOM uint32_t M2MMODE    : 1;
            __IM  uint32_t RESERVED1  : 17;
        } CHCFG4_B;
    } CHCFG4_R;

    /* DMA channel 4 number of data register */
    union
    {
        __IOM uint32_t CHNDATA4;

        struct
        {
            __IOM uint32_t NDATAT     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CHNDATA4_B;
    } CHNDATA4_R;

    /* DMA channel 4 peripheral address register */
    union
    {
        __IOM uint32_t CHPADDR4;

        struct
        {
            __IOM uint32_t PERADDR    : 32;
        } CHPADDR4_B;
    } CHPADDR4_R;

    /* DMA channel 4 memory address register */
    union
    {
        __IOM uint32_t CHMADDR4;

        struct
        {
            __IOM uint32_t MEMADDR    : 32;
        } CHMADDR4_B;
    } CHMADDR4_R;
    __IM  uint32_t  RESERVED4;

    /* DMA channel 5 configuration register */
    union
    {
        __IOM uint32_t CHCFG5;

        struct
        {
            __IOM uint32_t CHEN       : 1;
            __IOM uint32_t TCINTEN    : 1;
            __IOM uint32_t HTINTEN    : 1;
            __IOM uint32_t TERRINTEN  : 1;
            __IOM uint32_t DIRCFG     : 1;
            __IOM uint32_t CIRMODE    : 1;
            __IOM uint32_t PERIMODE   : 1;
            __IOM uint32_t MIMODE     : 1;
            __IOM uint32_t PERSIZE    : 2;
            __IOM uint32_t MSIZE      : 2;
            __IOM uint32_t CHPL       : 2;
            __IOM uint32_t M2MMODE    : 1;
            __IM  uint32_t RESERVED1  : 17;
        } CHCFG5_B;
    } CHCFG5_R;

    /* DMA channel 5 number of data register */
    union
    {
        __IOM uint32_t CHNDATA5;

        struct
        {
            __IOM uint32_t NDATAT     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CHNDATA5_B;
    } CHNDATA5_R;

    /* DMA channel 5 peripheral address register */
    union
    {
        __IOM uint32_t CHPADDR5;

        struct
        {
            __IOM uint32_t PERADDR    : 32;
        } CHPADDR5_B;
    } CHPADDR5_R;

    /* DMA channel 5 memory address register */
    union
    {
        __IOM uint32_t CHMADDR5;

        struct
        {
            __IOM uint32_t MEMADDR    : 32;
        } CHMADDR5_B;
    } CHMADDR5_R;
} DMA_T;

/**
  * @brief DMA CHANNEL register
  */

typedef struct
{

    /* DMA channel configuration register */
    union
    {
        __IOM uint32_t CHCFG;

        struct
        {
            __IOM uint32_t CHEN       : 1;
            __IOM uint32_t TCINTEN    : 1;
            __IOM uint32_t HTINTEN    : 1;
            __IOM uint32_t TERRINTEN  : 1;
            __IOM uint32_t DIRCFG     : 1;
            __IOM uint32_t CIRMODE    : 1;
            __IOM uint32_t PERIMODE   : 1;
            __IOM uint32_t MIMODE     : 1;
            __IOM uint32_t PERSIZE    : 2;
            __IOM uint32_t MSIZE      : 2;
            __IOM uint32_t CHPL       : 2;
            __IOM uint32_t M2MMODE    : 1;
            __IM  uint32_t RESERVED1  : 17;
        } CHCFG_B;
    } CHCFG_R;

    /* DMA channelx  number of data register */
    union
    {
        __IOM uint32_t CHNDATA;

        struct
        {
            __IOM uint32_t NDATAT     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CHNDATA_B;
    } CHNDATA_R;

    /* DMA channelx  peripheral address register */
    union
    {
        __IOM uint32_t CHPADDR;

        struct
        {
            __IOM uint32_t PERADDR    : 32;
        } CHPADDR_B;
    } CHPADDR_R;

    /* DMA channelx  memory address register */
    union
    {
        __IOM uint32_t CHMADDR;

        struct
        {
            __IOM uint32_t MEMADDR    : 32;
        } CHMADDR_B;
    } CHMADDR_R;
} DMA_CHANNEL_T;

/**
  * @brief External interrupt/event  controller (EINT)
  */

typedef struct
{
    /* Interrupt mask register */
    union
    {
        __IOM uint32_t IMASK;

        struct
        {
            __IOM uint32_t IMASK0     : 1;
            __IOM uint32_t IMASK1     : 1;
            __IOM uint32_t IMASK2     : 1;
            __IOM uint32_t IMASK3     : 1;
            __IOM uint32_t IMASK4     : 1;
            __IOM uint32_t IMASK5     : 1;
            __IOM uint32_t IMASK6     : 1;
            __IOM uint32_t IMASK7     : 1;
            __IOM uint32_t IMASK8     : 1;
            __IOM uint32_t IMASK9     : 1;
            __IOM uint32_t IMASK10    : 1;
            __IOM uint32_t IMASK11    : 1;
            __IOM uint32_t IMASK12    : 1;
            __IOM uint32_t IMASK13    : 1;
            __IOM uint32_t IMASK14    : 1;
            __IOM uint32_t IMASK15    : 1;
            __IOM uint32_t IMASK16    : 1;
            __IOM uint32_t IMASK17    : 1;
            __IOM uint32_t IMASK18    : 1;
            __IOM uint32_t IMASK19    : 1;
            __IOM uint32_t IMASK20    : 1;
            __IOM uint32_t IMASK21    : 1;
            __IOM uint32_t IMASK22    : 1;
            __IOM uint32_t IMASK23    : 1;
            __IOM uint32_t IMASK24    : 1;
            __IOM uint32_t IMASK25    : 1;
            __IOM uint32_t IMASK26    : 1;
            __IOM uint32_t IMASK27    : 1;
            __IM  uint32_t RESERVED1  : 4;
        } IMASK_B;
    } IMASK_R;

    /* Event mask register EINT_EMASK */
    union
    {

        __IOM uint32_t EMASK;

        struct
        {
            __IOM uint32_t EMASK0     : 1;
            __IOM uint32_t EMASK1     : 1;
            __IOM uint32_t EMASK2     : 1;
            __IOM uint32_t EMASK3     : 1;
            __IOM uint32_t EMASK4     : 1;
            __IOM uint32_t EMASK5     : 1;
            __IOM uint32_t EMASK6     : 1;
            __IOM uint32_t EMASK7     : 1;
            __IOM uint32_t EMASK8     : 1;
            __IOM uint32_t EMASK9     : 1;
            __IOM uint32_t EMASK10    : 1;
            __IOM uint32_t EMASK11    : 1;
            __IOM uint32_t EMASK12    : 1;
            __IOM uint32_t EMASK13    : 1;
            __IOM uint32_t EMASK14    : 1;
            __IOM uint32_t EMASK15    : 1;
            __IOM uint32_t EMASK16    : 1;
            __IOM uint32_t EMASK17    : 1;
            __IOM uint32_t EMASK18    : 1;
            __IOM uint32_t EMASK19    : 1;
            __IOM uint32_t EMASK20    : 1;
            __IOM uint32_t EMASK21    : 1;
            __IOM uint32_t EMASK22    : 1;
            __IOM uint32_t EMASK23    : 1;
            __IOM uint32_t EMASK24    : 1;
            __IOM uint32_t EMASK25    : 1;
            __IOM uint32_t EMASK26    : 1;
            __IOM uint32_t EMASK27    : 1;
            __IM  uint32_t RESERVED1  : 4;
        } EMASK_B;
    } EMASK_R;

    union
    {
        __IOM uint32_t RTEN;

        struct
        {
            __IOM uint32_t RTEN0      : 1;
            __IOM uint32_t RTEN1      : 1;
            __IOM uint32_t RTEN2      : 1;
            __IOM uint32_t RTEN3      : 1;
            __IOM uint32_t RTEN4      : 1;
            __IOM uint32_t RTEN5      : 1;
            __IOM uint32_t RTEN6      : 1;
            __IOM uint32_t RTEN7      : 1;
            __IOM uint32_t RTEN8      : 1;
            __IOM uint32_t RTEN9      : 1;
            __IOM uint32_t RTEN10     : 1;
            __IOM uint32_t RTEN11     : 1;
            __IOM uint32_t RTEN12     : 1;
            __IOM uint32_t RTEN13     : 1;
            __IOM uint32_t RTEN14     : 1;
            __IOM uint32_t RTEN15     : 1;
            __IOM uint32_t RTEN16     : 1;
            __IM  uint32_t RESERVED1  : 15;
        } RTEN_B;
    } RTEN_R;

    /* Falling Trigger selection register */
    union
    {
        __IOM uint32_t FTEN;

        struct
        {
            __IOM uint32_t FTEN0      : 1;
            __IOM uint32_t FTEN1      : 1;
            __IOM uint32_t FTEN2      : 1;
            __IOM uint32_t FTEN3      : 1;
            __IOM uint32_t FTEN4      : 1;
            __IOM uint32_t FTEN5      : 1;
            __IOM uint32_t FTEN6      : 1;
            __IOM uint32_t FTEN7      : 1;
            __IOM uint32_t FTEN8      : 1;
            __IOM uint32_t FTEN9      : 1;
            __IOM uint32_t FTEN10     : 1;
            __IOM uint32_t FTEN11     : 1;
            __IOM uint32_t FTEN12     : 1;
            __IOM uint32_t FTEN13     : 1;
            __IOM uint32_t FTEN14     : 1;
            __IOM uint32_t FTEN15     : 1;
            __IOM uint32_t FTEN16     : 1;
            __IM  uint32_t RESERVED1  : 15;
        } FTEN_B;
    } FTEN_R;

    /* Software interrupt event register */
    union
    {
        __IOM uint32_t SWINTE;

        struct
        {
            __IOM uint32_t SWINTE0    : 1;
            __IOM uint32_t SWINTE1    : 1;
            __IOM uint32_t SWINTE2    : 1;
            __IOM uint32_t SWINTE3    : 1;
            __IOM uint32_t SWINTE4    : 1;
            __IOM uint32_t SWINTE5    : 1;
            __IOM uint32_t SWINTE6    : 1;
            __IOM uint32_t SWINTE7    : 1;
            __IOM uint32_t SWINTE8    : 1;
            __IOM uint32_t SWINTE9    : 1;
            __IOM uint32_t SWINTE10   : 1;
            __IOM uint32_t SWINTE11   : 1;
            __IOM uint32_t SWINTE12   : 1;
            __IOM uint32_t SWINTE13   : 1;
            __IOM uint32_t SWINTE14   : 1;
            __IOM uint32_t SWINTE15   : 1;
            __IOM uint32_t SWINTE16   : 1;
            __IM  uint32_t RESERVED1  : 15;
        } SWINTE_B;
    } SWINTE_R;

    /* Pending register */
    union
    {
        __IOM uint32_t IPEND;

        struct
        {
            __IOM uint32_t IPEND0     : 1;
            __IOM uint32_t IPEND1     : 1;
            __IOM uint32_t IPEND2     : 1;
            __IOM uint32_t IPEND3     : 1;
            __IOM uint32_t IPEND4     : 1;
            __IOM uint32_t IPEND5     : 1;
            __IOM uint32_t IPEND6     : 1;
            __IOM uint32_t IPEND7     : 1;
            __IOM uint32_t IPEND8     : 1;
            __IOM uint32_t IPEND9     : 1;
            __IOM uint32_t IPEND10    : 1;
            __IOM uint32_t IPEND11    : 1;
            __IOM uint32_t IPEND12    : 1;
            __IOM uint32_t IPEND13    : 1;
            __IOM uint32_t IPEND14    : 1;
            __IOM uint32_t IPEND15    : 1;
            __IOM uint32_t IPEND16    : 1;
            __IM  uint32_t RESERVED1  : 15;
        } IPEND_B;
    } IPEND_R;
} EINT_T;

/**
  * @brief FMC (FMC)
  */

typedef struct
{

    /* Flash access control register  */
    union
    {
        __IOM uint32_t CTRL1;

        struct
        {
            __IOM uint32_t WS          : 4;
            __IOM uint32_t PBEN        : 1;
            __IM  uint32_t PBSF        : 1;
            __IM  uint32_t RESERVED1   : 2;
            __IOM uint32_t VREAD0_EN   : 1;
            __IM  uint32_t VREAD0_STS  : 1;
            __IM  uint32_t RESERVED2   : 2;
            __IOM uint32_t VREAD1_EN   : 1;
            __IM  uint32_t VREAD1_STS  : 1;
            __IM  uint32_t RESERVED3   : 2;
            __IOM uint32_t ECCEN       : 1;
            __IM  uint32_t RESERVED4   : 15;
        } CTRL1_B;
    } CTRL1_R;

    /* Flash key register  */
    union
    {
        __OM  uint32_t KEY;

        struct
        {
            __OM  uint32_t KEY        : 32;
        } KEY_B;
    } KEY_R;

    /* Flash option key register   */
    union
    {
        __OM  uint32_t OBKEY;

        struct
        {
            __OM  uint32_t OBKEY      : 32;
        } OBKEY_B;
    } OBKEY_R;

    /* Flash status register  */
    union
    {
        __IOM uint32_t STS;

        struct
        {
            __IM  uint32_t BUSYF      : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t PEF        : 1;
            __IOM uint32_t PAEF       : 1;
            __IOM uint32_t WPEF       : 1;
            __IOM uint32_t OCF        : 1;
            __IM  uint32_t RESERVED2  : 10;
            __IOM uint32_t DBFIFLG    : 1;
            __IM  uint32_t RESERVED3  : 15;
        } STS_B;
    } STS_R;

    /* Flash control register */
    union
    {
        __IOM uint32_t CTRL2;

        struct
        {
            __IOM uint32_t PG         : 1;
            __IOM uint32_t PAGEERA    : 1;
            __IOM uint32_t MASSERA    : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t OBP        : 1;
            __IOM uint32_t OBE        : 1;
            __IOM uint32_t STA        : 1;
            __IOM uint32_t LOCK       : 1;
            __IM  uint32_t RESERVED2  : 1;
            __IOM uint32_t OBWEN      : 1;
            __IOM uint32_t ERRIE      : 1;
            __IM  uint32_t RESERVED3  : 1;
            __IOM uint32_t OCIE       : 1;
            __IOM uint32_t OBLOAD     : 1;
            __IM  uint32_t RESERVED4  : 2;
            __IOM uint32_t DBFIEN     : 1;
            __IOM uint32_t OBRIE      : 1;
            __IOM uint32_t MER_TYPE   : 2;
            __IOM uint32_t PROGLEN    : 2;
            __IM  uint32_t RESERVED5  : 2;
            __OM  uint32_t SCR        : 8;
        } CTRL2_B;
    } CTRL2_R;

    /* Flash address register */
    union
    {
        __OM  uint32_t ADDR;

        struct
        {
            __OM  uint32_t ADDR       : 32;
        } ADDR_B;
    } ADDR_R;
    __IM  uint32_t  RESERVED;

    /* Option byte register */
    union
    {
        __IM  uint32_t OBCS;

        struct
        {
            __IM  uint32_t OBE        : 1;
            __IM  uint32_t READPROT   : 2;
            __IM  uint32_t SCRKERR    : 1;
            __IM  uint32_t SCRECCERR  : 1;
            __IM  uint32_t OPTRERR    : 1;
            __IM  uint32_t OPTECCERR  : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IM  uint32_t WDTSEL     : 1;
            __IM  uint32_t RSTSTOP    : 1;
            __IM  uint32_t RSTSTDB    : 1;
            __IM  uint32_t nBOOT0     : 1;
            __IM  uint32_t nBOOT1     : 1;
            __IM  uint32_t VDDAMONI   : 1;
            __IM  uint32_t RESERVED2  : 2;
            __IM  uint32_t DATA0      : 8;
            __IM  uint32_t DATA1      : 8;
        } OBCS_B;
    } OBCS_R;

    /* Write protection register */
    union
    {
        __IM  uint32_t WRTPROT0;

        struct
        {
            __IM  uint32_t PWRTPROT0  : 32;
        } WRTPROT0_B;
    } WRTPROT0_R;

    /* Write protection register */
    union
    {
        __IM  uint32_t WRTPROT1;

        struct
        {
            __IM  uint32_t WRTPROT1    : 32;
        } WRTPROT1_B;
    } WRTPROT1_R;

    /* reserved */
    __IM  uint32_t  RESERVED1[2];

    /* Flash ECC Address register */
    union
    {
        __IM  uint32_t ECC_ADDR;

        struct
        {
            __IM  uint32_t FMC_ECC_ADDR  : 32;
        } ECC_ADDR_B;
    } ECC_ADDR_R;

    /* reserved */
    __IM  uint32_t  RESERVED2[3];

    /* Flash program data register 0 */
    union
    {
        __OM  uint32_t PROG_DATA0;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA0    : 32;
        } PROG_DATA0_B;
    } PROG_DATA0_R;

    /* Flash program data register 1 */
    union
    {
        __OM  uint32_t PROG_DATA1;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA1    : 32;
        } PROG_DATA1_B;
    } PROG_DATA1_R;

    /* Flash program data register 2 */
    union
    {
        __OM  uint32_t PROG_DATA2;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA2    : 32;
        } PROG_DATA2_B;
    } PROG_DATA2_R;

    /* Flash program data register 3 */
    union
    {
        __OM  uint32_t PROG_DATA3;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA3    : 32;
        } PROG_DATA3_B;
    } PROG_DATA3_R;

    /* Flash program data register 4 */
    union
    {
        __OM  uint32_t PROG_DATA4;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA4    : 32;
        } PROG_DATA4_B;
    } PROG_DATA4_R;

    /* Flash program data register 5 */
    union
    {
        __OM  uint32_t PROG_DATA5;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA5    : 32;
        } PROG_DATA5_B;
    } PROG_DATA5_R;

    /* Flash program data register 6 */
    union
    {
        __OM  uint32_t PROG_DATA6;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA6    : 32;
        } PROG_DATA6_B;
    } PROG_DATA6_R;

    /* Flash program data register 7 */
    union
    {
        __OM  uint32_t PROG_DATA7;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA7    : 32;
        } PROG_DATA7_B;
    } PROG_DATA7_R;

    /* Flash program data register 8 */
    union
    {
        __OM  uint32_t PROG_DATA8;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA8    : 32;
        } PROG_DATA8_B;
    } PROG_DATA8_R;

    /* Flash program data register 9 */
    union
    {
        __OM  uint32_t PROG_DATA9;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA9    : 32;
        } PROG_DATA9_B;
    } PROG_DATA9_R;

    /* Flash program data register 10 */
    union
    {
        __OM  uint32_t PROG_DATA10;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA10    : 32;
        } PROG_DATA10_B;
    } PROG_DATA10_R;

    /* Flash program data register 11 */
    union
    {
        __OM  uint32_t PROG_DATA11;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA11    : 32;
        } PROG_DATA11_B;
    } PROG_DATA11_R;

    /* Flash program data register 12 */
    union
    {
        __OM  uint32_t PROG_DATA12;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA12    : 32;
        } PROG_DATA12_B;
    } PROG_DATA12_R;

    /* Flash program data register 13 */
    union
    {
        __OM  uint32_t PROG_DATA13;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA13    : 32;
        } PROG_DATA13_B;
    } PROG_DATA13_R;

    /* Flash program data register 14 */
    union
    {
        __OM  uint32_t PROG_DATA14;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA14    : 32;
        } PROG_DATA14_B;
    } PROG_DATA14_R;

    /* Flash program data register 15 */
    union
    {
        __OM  uint32_t PROG_DATA15;

        struct
        {
            __OM  uint32_t FMC_PROG_DATA15    : 32;
        } PROG_DATA15_B;
    } PROG_DATA15_R;
} FMC_T;

/*
  * @brief General-purpose I/Os (GPIO)
  */

typedef struct
{
    /* GPIO port mode register*/
    union
    {
        __IOM uint32_t MODE;

        struct
        {
            __IOM uint32_t MODE0      : 2;
            __IOM uint32_t MODE1      : 2;
            __IOM uint32_t MODE2      : 2;
            __IOM uint32_t MODE3      : 2;
            __IOM uint32_t MODE4      : 2;
            __IOM uint32_t MODE5      : 2;
            __IOM uint32_t MODE6      : 2;
            __IOM uint32_t MODE7      : 2;
            __IOM uint32_t MODE8      : 2;
            __IOM uint32_t MODE9      : 2;
            __IOM uint32_t MODE10     : 2;
            __IOM uint32_t MODE11     : 2;
            __IOM uint32_t MODE12     : 2;
            __IOM uint32_t MODE13     : 2;
            __IOM uint32_t MODE14     : 2;
            __IOM uint32_t MODE15     : 2;
        } MODE_B;
    } MODE_R;

    /* GPIO port output type register*/
    union
    {
        __IOM uint32_t OMODE;

        struct
        {
            __IOM uint32_t OMODE0     : 1;
            __IOM uint32_t OMODE1     : 1;
            __IOM uint32_t OMODE2     : 1;
            __IOM uint32_t OMODE3     : 1;
            __IOM uint32_t OMODE4     : 1;
            __IOM uint32_t OMODE5     : 1;
            __IOM uint32_t OMODE6     : 1;
            __IOM uint32_t OMODE7     : 1;
            __IOM uint32_t OMODE8     : 1;
            __IOM uint32_t OMODE9     : 1;
            __IOM uint32_t OMODE10    : 1;
            __IOM uint32_t OMODE11    : 1;
            __IOM uint32_t OMODE12    : 1;
            __IOM uint32_t OMODE13    : 1;
            __IOM uint32_t OMODE14    : 1;
            __IOM uint32_t OMODE15    : 1;
            __IOM uint32_t RESERVED1  : 16;
        } OMODE_B;
    } OMODE_R;

    /* GPIO port output speed register*/
    union
    {
        __IOM uint32_t OSSEL;

        struct
        {
            __IOM uint32_t OSSEL0     : 2;
            __IOM uint32_t OSSEL1     : 2;
            __IOM uint32_t OSSEL2     : 2;
            __IOM uint32_t OSSEL3     : 2;
            __IOM uint32_t OSSEL4     : 2;
            __IOM uint32_t OSSEL5     : 2;
            __IOM uint32_t OSSEL6     : 2;
            __IOM uint32_t OSSEL7     : 2;
            __IOM uint32_t OSSEL8     : 2;
            __IOM uint32_t OSSEL9     : 2;
            __IOM uint32_t OSSEL10    : 2;
            __IOM uint32_t OSSEL11    : 2;
            __IOM uint32_t OSSEL12    : 2;
            __IOM uint32_t OSSEL13    : 2;
            __IOM uint32_t OSSEL14    : 2;
            __IOM uint32_t OSSEL15    : 2;
        } OSSEL_B;
    } OSSEL_R;

    /* GPIO port pull-up/pull-down register*/
    union
    {
        __IOM uint32_t PUPD;

        struct
        {
            __IOM uint32_t PUPD0      : 2;
            __IOM uint32_t PUPD1      : 2;
            __IOM uint32_t PUPD2      : 2;
            __IOM uint32_t PUPD3      : 2;
            __IOM uint32_t PUPD4      : 2;
            __IOM uint32_t PUPD5      : 2;
            __IOM uint32_t PUPD6      : 2;
            __IOM uint32_t PUPD7      : 2;
            __IOM uint32_t PUPD8      : 2;
            __IOM uint32_t PUPD9      : 2;
            __IOM uint32_t PUPD10     : 2;
            __IOM uint32_t PUPD11     : 2;
            __IOM uint32_t PUPD12     : 2;
            __IOM uint32_t PUPD13     : 2;
            __IOM uint32_t PUPD14     : 2;
            __IOM uint32_t PUPD15     : 2;
        } PUPD_B;
    } PUPD_R;

    /* GPIO port input data register*/
    union
    {
        __IM  uint32_t IDATA;

        struct
        {
            __IM  uint32_t IDATA0       : 1;
            __IM  uint32_t IDATA1       : 1;
            __IM  uint32_t IDATA2       : 1;
            __IM  uint32_t IDATA3       : 1;
            __IM  uint32_t IDATA4       : 1;
            __IM  uint32_t IDATA5       : 1;
            __IM  uint32_t IDATA6       : 1;
            __IM  uint32_t IDATA7       : 1;
            __IM  uint32_t IDATA8       : 1;
            __IM  uint32_t IDATA9       : 1;
            __IM  uint32_t IDATA10      : 1;
            __IM  uint32_t IDATA11      : 1;
            __IM  uint32_t IDATA12      : 1;
            __IM  uint32_t IDATA13      : 1;
            __IM  uint32_t IDATA14      : 1;
            __IM  uint32_t IDATA15      : 1;
            __IM  uint32_t RESERVED1    : 16;
        } IDATA_B;
    } IDATA_R;

    /* GPIO port output data register*/
    union
    {
        __IOM uint32_t ODATA;

        struct
        {
            __IOM uint32_t ODATA0      : 1;
            __IOM uint32_t ODATA1      : 1;
            __IOM uint32_t ODATA2      : 1;
            __IOM uint32_t ODATA3      : 1;
            __IOM uint32_t ODATA4      : 1;
            __IOM uint32_t ODATA5      : 1;
            __IOM uint32_t ODATA6      : 1;
            __IOM uint32_t ODATA7      : 1;
            __IOM uint32_t ODATA8      : 1;
            __IOM uint32_t ODATA9      : 1;
            __IOM uint32_t ODATA10     : 1;
            __IOM uint32_t ODATA11     : 1;
            __IOM uint32_t ODATA12     : 1;
            __IOM uint32_t ODATA13     : 1;
            __IOM uint32_t ODATA14     : 1;
            __IOM uint32_t ODATA15     : 1;
            __IOM uint32_t RESERVED1  : 16;
        } ODATA_B;
    } ODATA_R;

    /* GPIO port bit set/clear register*/
    union
    {
        __OM  uint32_t BSC;

        struct
        {
            __OM  uint32_t BS0        : 1;
            __OM  uint32_t BS1        : 1;
            __OM  uint32_t BS2        : 1;
            __OM  uint32_t BS3        : 1;
            __OM  uint32_t BS4        : 1;
            __OM  uint32_t BS5        : 1;
            __OM  uint32_t BS6        : 1;
            __OM  uint32_t BS7        : 1;
            __OM  uint32_t BS8        : 1;
            __OM  uint32_t BS9        : 1;
            __OM  uint32_t BS10       : 1;
            __OM  uint32_t BS11       : 1;
            __OM  uint32_t BS12       : 1;
            __OM  uint32_t BS13       : 1;
            __OM  uint32_t BS14       : 1;
            __OM  uint32_t BS15       : 1;
            __OM  uint32_t BC0        : 1;
            __OM  uint32_t BC1        : 1;
            __OM  uint32_t BC2        : 1;
            __OM  uint32_t BC3        : 1;
            __OM  uint32_t BC4        : 1;
            __OM  uint32_t BC5        : 1;
            __OM  uint32_t BC6        : 1;
            __OM  uint32_t BC7        : 1;
            __OM  uint32_t BC8        : 1;
            __OM  uint32_t BC9        : 1;
            __OM  uint32_t BC10       : 1;
            __OM  uint32_t BC11       : 1;
            __OM  uint32_t BC12       : 1;
            __OM  uint32_t BC13       : 1;
            __OM  uint32_t BC14       : 1;
            __OM  uint32_t BC15       : 1;
        } BSC_B;
    } BSC_R;

    /* GPIO port configuration lock register*/
    union
    {
        __IOM uint32_t LOCK;

        struct
        {
            __IOM uint32_t LOCK0      : 1;
            __IOM uint32_t LOCK1      : 1;
            __IOM uint32_t LOCK2      : 1;
            __IOM uint32_t LOCK3      : 1;
            __IOM uint32_t LOCK4      : 1;
            __IOM uint32_t LOCK5      : 1;
            __IOM uint32_t LOCK6      : 1;
            __IOM uint32_t LOCK7      : 1;
            __IOM uint32_t LOCK8      : 1;
            __IOM uint32_t LOCK9      : 1;
            __IOM uint32_t LOCK10     : 1;
            __IOM uint32_t LOCK11     : 1;
            __IOM uint32_t LOCK12     : 1;
            __IOM uint32_t LOCK13     : 1;
            __IOM uint32_t LOCK14     : 1;
            __IOM uint32_t LOCK15     : 1;
            __IOM uint32_t LOCKKEY    : 1;
            __IOM uint32_t RESERVED1  : 15;
        } LOCK_B;
    } LOCK_R;

    /* GPIO alternate function low register*/
    union
    {
        __IOM uint32_t ALFL;

        struct
        {
            __IOM uint32_t ALFSEL0    : 4;
            __IOM uint32_t ALFSEL1    : 4;
            __IOM uint32_t ALFSEL2    : 4;
            __IOM uint32_t ALFSEL3    : 4;
            __IOM uint32_t ALFSEL4    : 4;
            __IOM uint32_t ALFSEL5    : 4;
            __IOM uint32_t ALFSEL6    : 4;
            __IOM uint32_t ALFSEL7    : 4;
        } ALFL_B;
    } ALFL_R;

    /* GPIO alternate function high register*/
    union
    {
        __IOM uint32_t ALFH;

        struct
        {
            __IOM uint32_t ALFSEL8        : 4;
            __IOM uint32_t ALFSEL9        : 4;
            __IOM uint32_t ALFSEL10       : 4;
            __IOM uint32_t ALFSEL11       : 4;
            __IOM uint32_t ALFSEL12       : 4;
            __IOM uint32_t ALFSEL13       : 4;
            __IOM uint32_t ALFSEL14       : 4;
            __IOM uint32_t ALFSEL15       : 4;
        } ALFH_B;
    } ALFH_R;

    /* Port bit clear register*/
    union
    {
        __OM  uint32_t BR;

        struct
        {
            __OM  uint32_t BR0        : 1;
            __OM  uint32_t BR1        : 1;
            __OM  uint32_t BR2        : 1;
            __OM  uint32_t BR3        : 1;
            __OM  uint32_t BR4        : 1;
            __OM  uint32_t BR5        : 1;
            __OM  uint32_t BR6        : 1;
            __OM  uint32_t BR7        : 1;
            __OM  uint32_t BR8        : 1;
            __OM  uint32_t BR9        : 1;
            __OM  uint32_t BR10       : 1;
            __OM  uint32_t BR11       : 1;
            __OM  uint32_t BR12       : 1;
            __OM  uint32_t BR13       : 1;
            __OM  uint32_t BR14       : 1;
            __OM  uint32_t BR15       : 1;
            __IM  uint32_t RESERVED1  : 16;
        } BR_B;
    } BR_R;

    /* reserved */
    __IM  uint32_t  RESERVED[1];

    /* Port filter enable register */
    union
    {
        __IOM  uint32_t FILTER_EN;

        struct
        {
            __IOM  uint32_t FILTER_EN0        : 1;
            __IOM  uint32_t FILTER_EN1        : 1;
            __IOM  uint32_t FILTER_EN2        : 1;
            __IOM  uint32_t FILTER_EN3        : 1;
            __IOM  uint32_t FILTER_EN4        : 1;
            __IOM  uint32_t FILTER_EN5        : 1;
            __IOM  uint32_t FILTER_EN6        : 1;
            __IOM  uint32_t FILTER_EN7        : 1;
            __IOM  uint32_t FILTER_EN8        : 1;
            __IOM  uint32_t FILTER_EN9        : 1;
            __IOM  uint32_t FILTER_EN10       : 1;
            __IOM  uint32_t FILTER_EN11       : 1;
            __IOM  uint32_t FILTER_EN12       : 1;
            __IOM  uint32_t FILTER_EN13       : 1;
            __IOM  uint32_t FILTER_EN14       : 1;
            __IOM  uint32_t FILTER_EN15       : 1;
            __IM   uint32_t RESERVED1         : 16;
        } FILTER_EN_B;
    } FILTER_EN_R;
} GPIO_T;

/**
  * @brief Independent watchdog (IWDT)
  */

typedef struct
{

    /* Key register */
    union
    {
        __OM  uint32_t KEY;

        struct
        {
            __OM  uint32_t KEY        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } KEY_B;
    } KEY_R;

    /* Prescaler register */
    union
    {
        __IOM uint32_t PSC;

        struct
        {
            __IOM uint32_t PSC        : 3;
            __IM  uint32_t RESERVED1  : 29;
        } PSC_B;
    } PSC_R;

    /* Counter reload register */
    union
    {
        __IOM uint32_t CNTRLD;

        struct
        {
            __IOM uint32_t CNTRLD     : 12;
            __IM  uint32_t RESERVED1  : 20;
        } CNTRLD_B;
    } CNTRLD_R;

    /* Status register */
    union
    {
        __IM  uint32_t STS;

        struct
        {
            __IM  uint32_t PSCUFLG    : 1;
            __IM  uint32_t CNTUFLG    : 1;
            __IM  uint32_t WINUFLG    : 1;
            __IM  uint32_t RESERVED1  : 29;
        } STS_B;
    } STS_R;

    /* Window register */
    union
    {
        __IOM uint32_t WIN;

        struct
        {
            __IOM uint32_t WIN        : 12;
            __IM  uint32_t RESERVED1  : 20;
        } WIN_B;
    } WIN_R;
} IWDT_T;

/**
  * @brief Option bytes (OB)
  */

typedef struct
{

    /* Read protection option byte */
    union
    {
        __IOM uint16_t READPROT;

        struct
        {
            __IOM uint32_t READPROT   : 8;
            __IM  uint32_t nREADPROT  : 8;
        } READPORT_B;
    } READPORT_R;

    /* User protection option byte */
    union
    {
        __IOM uint16_t UOB;

        struct
        {
            __IOM uint32_t WDTSEL        : 1;
            __IOM uint32_t nRSTSTOP      : 1;
            __IOM uint32_t nRSTSTDBY     : 1;
            __IOM uint32_t nBOOT0        : 1;
            __IOM uint32_t nBOOT1        : 1;
            __IOM uint32_t VDDA_MONITOR  : 1;
            __IM  uint32_t RESERVED      : 2;
            __IM  uint32_t nUOB          : 8;
        } UOB_B;
    } UOB_R;

    /* User data option byte */
    union
    {
        __IOM uint16_t DATA0;

        struct
        {
            __IOM uint32_t DATA0      : 8;
            __IM  uint32_t nDATA0     : 8;
        } DATA0_B;
    } DATA0_R;

    /* User data option byte */
    union
    {
        __IOM uint16_t DATA1;

        struct
        {
            __IOM uint32_t DATA1      : 8;
            __IM  uint32_t nDATA1     : 8;
        } DATA1_B;
    } DATA1_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT0;

        struct
        {
            __IOM uint32_t WRTPROT0   : 8;
            __IM  uint32_t nWRTPROT0  : 8;
        } WRTPROT0_B;
    } WRTPROT0_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT1;

        struct
        {
            __IOM uint32_t WRTPROT1   : 8;
            __IM  uint32_t nWRTPROT1  : 8;
        } WRTPROT1_B;
    } WRTPROT1_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT2;

        struct
        {
            __IOM uint32_t WRTPROT2   : 8;
            __IM  uint32_t nWRTPROT2  : 8;
        } WRTPROT2_B;
    } WRTPROT2_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT3;

        struct
        {
            __IOM uint32_t WRTPROT3   : 8;
            __IM  uint32_t nWRTPROT3  : 8;
        } WRTPROT3_B;
    } WRTPROT3_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT4;

        struct
        {
            __IOM uint32_t WRTPROT4   : 8;
            __IM  uint32_t nWRTPROT4  : 8;
        } WRTPROT4_B;
    } WRTPROT4_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT5;

        struct
        {
            __IOM uint32_t WRTPROT5   : 8;
            __IM  uint32_t nWRTPROT5  : 8;
        } WRTPROT5_B;
    } WRTPROT5_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT6;

        struct
        {
            __IOM uint32_t WRTPROT6   : 8;
            __IM  uint32_t nWRTPROT6  : 8;
        } WRTPROT6_B;
    } WRTPROT6_R;

    /* Write protection option byte */
    union
    {
        __IOM uint16_t WRTPROT7;

        struct
        {
            __IOM uint32_t WRTPROT7   : 8;
            __IM  uint32_t nWRTPROT7  : 8;
        } WRTPROT7_B;
    } WRTPROT7_R;
} OB_T;

/**
  * @brief Power control (PMU)
  */

typedef struct
{
    /* power control register */
    union
    {
        __IOM uint32_t CTRL;

        struct
        {
            __IOM uint32_t LPDSCFG        : 1;
            __IOM uint32_t PDDSCFG        : 1;
            __IOM uint32_t WUFLGCLR       : 1;
            __IOM uint32_t SBFLGCLR       : 1;
            __IOM uint32_t PVDEN          : 1;
            __IOM uint32_t PLSEL          : 3;
            __IOM uint32_t BPWEN          : 1;
            __IOM uint32_t STOP_FILTER_EN : 1;
            __IM  uint32_t RESERVED2      : 22;
        } CTRL_B;
    } CTRL_R;

    /* power control/status register */
    union
    {
        __IM uint32_t CSTS;

        struct
        {
            __IM  uint32_t WUEFLG     : 1;
            __IM  uint32_t SBFLG      : 1;
            __IM  uint32_t PVDOFLG    : 1;
            __IM  uint32_t RESERVED1  : 29;
        } CSTS_B;
    } CSTS_R;
} PMU_T;

/**
  * @brief Reset and clock control (RCM)
  */

typedef struct
{
    /*Clock control register 1 */
    union
    {
        /* Clock control register */
        __IOM uint32_t CTRL1;

        struct
        {
            __IOM uint32_t HSIEN      : 1;
            __IM  uint32_t HSIRDYFLG  : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t HSITRM     : 5;
            __IM  uint32_t HSICAL     : 8;
            __IOM uint32_t HSEEN      : 1;
            __IM  uint32_t HSERDYFLG  : 1;
            __IOM uint32_t HSEBCFG    : 1;
            __IOM uint32_t CSSEN      : 1;
            __IM  uint32_t RESERVED2  : 4;
            __IOM uint32_t PLLEN      : 1;
            __IM  uint32_t PLLRDYFLG  : 1;
            __IM  uint32_t RESERVED3  : 6;
        } CTRL1_B;
    } CTRL1_R;

    /* Clock configuration register 1 */
    union
    {
        __IOM uint32_t CFG1;

        struct
        {
            __IOM uint32_t SCLKSEL     : 2;
            __IM  uint32_t SCLKSELSTS  : 2;
            __IOM uint32_t AHBPSC      : 4;
            __IOM uint32_t APB1PSC     : 3;
            __IM  uint32_t RESERVED1   : 5;
            __IOM uint32_t PLLSRCSEL   : 1;
            __IOM uint32_t PLLHSEPSC   : 1;
            __IOM uint32_t PLLMULCFG   : 4;
            __IM  uint32_t RESERVED2   : 2;
            __IOM uint32_t MCOSEL      : 4;
            __IOM uint32_t MCOPRE      : 3;
            __IM  uint32_t RESERVED3   : 1;
        } CFG1_B;
    } CFG1_R;

    /* Clock interrupt register 1 */
    union
    {
        __IOM uint32_t INT1;

        struct
        {
            __IM  uint32_t LSIRDYFLG    : 1;
            __IM  uint32_t RESERVED1    : 1;
            __IM  uint32_t HSIRDYFLG    : 1;
            __IM  uint32_t HSERDYFLG    : 1;
            __IM  uint32_t PLLRDYFLG    : 1;
            __IM  uint32_t HSI14RDYFLG  : 1;
            __IM  uint32_t RESERVED2    : 1;
            __IM  uint32_t CSSFLG       : 1;
            __IOM uint32_t LSIRDYEN     : 1;
            __IM  uint32_t RESERVED3    : 1;
            __IOM uint32_t HSIRDYEN     : 1;
            __IOM uint32_t HSERDYEN     : 1;
            __IOM uint32_t PLLRDYEN     : 1;
            __IOM uint32_t HSI14RDYEN   : 1;
            __IM  uint32_t RESERVED4    : 2;
            __OM  uint32_t LSIRDYCLR    : 1;
            __IM  uint32_t RESERVED5    : 1;
            __OM  uint32_t HSIRDYCLR    : 1;
            __OM  uint32_t HSERDYCLR    : 1;
            __OM  uint32_t PLLRDYCLR    : 1;
            __OM  uint32_t HSI14RDYCLR  : 1;
            __IM  uint32_t RESERVED6    : 1;
            __OM  uint32_t CSSCLR       : 1;
            __IM  uint32_t RESERVED7    : 8;
        } INT1_B;
    } INT1_R;

    /* APB2 peripheral reset register */
    union
    {
        __IOM uint32_t APBRST2;

        struct
        {
            __IOM uint32_t SYSCFGRST      : 1;
            __IM  uint32_t RESERVED1      : 8;
            __IOM uint32_t ADCRST         : 1;
            __IM  uint32_t RESERVED2      : 1;
            __IOM uint32_t TMR1RST        : 1;
            __IOM uint32_t SPI1RST        : 1;
            __IM  uint32_t RESERVED3      : 1;
            __IOM uint32_t USART1RST      : 1;
            __IOM uint32_t TIM8RST        : 1;
            __IM  uint32_t RESERVED4      : 6;
            __IOM uint32_t DBGRST         : 1;
            __IOM uint32_t SMSRST         : 1;
            __IM  uint32_t RESERVED5      : 8;
        } APBRST2_B;
    } APBRST2_R;

    /*APB1 peripheral reset register */
    union
    {
        __IOM uint32_t APBRST1;

        struct
        {
            __IOM uint32_t TMR2RST        : 1;
            __IOM uint32_t TMR3RST        : 1;
            __IOM uint32_t TMR4RST        : 1;
            __IM  uint32_t RESERVED1      : 1;
            __IOM uint32_t TMR6RST        : 1;
            __IOM uint32_t TMR7RST        : 1;
            __IM  uint32_t RESERVED2      : 5;
            __IOM uint32_t WWDTRST        : 1;
            __IM  uint32_t RESERVED3      : 5;
            __IOM uint32_t USART2RST      : 1;
            __IM  uint32_t RESERVED4      : 7;
            __IOM uint32_t CANRST         : 1;
            __IM  uint32_t RESERVED5      : 2;
            __IOM uint32_t PMURST         : 1;
            __IM  uint32_t RESERVED6      : 3;
        } APBRST1_B;
    } APBRST1_R;

    /* AHB Peripheral Clock enable register */
    union
    {
        __IOM uint32_t AHBCLKEN;

        struct
        {
            __IOM uint32_t DMA1EN         : 1;
            __IM  uint32_t RESERVED1      : 1;
            __IOM uint32_t SRAMEN         : 1;
            __IM  uint32_t RESERVED2      : 1;
            __IOM uint32_t FMCEN          : 1;
            __IM  uint32_t RESERVED3      : 1;
            __IOM uint32_t CRCEN          : 1;
            __IOM uint32_t TRNGEN         : 1;
            __IOM uint32_t SHAEN          : 1;
            __IOM uint32_t AES256EN       : 1;
            __IM  uint32_t RESERVED4      : 7;
            __IOM uint32_t PAEN           : 1;
            __IOM uint32_t PBEN           : 1;
            __IOM uint32_t PCEN           : 1;
            __IOM uint32_t PDEN           : 1;
            __IM  uint32_t RESERVED5      : 1;
            __IOM uint32_t PFEN           : 1;
            __IM  uint32_t RESERVED6      : 9;
        } AHBCLKEN_B;
    } AHBCLKEN_R;

    /* APB Peripheral Clock enable register 2 */
    union
    {
        __IOM uint32_t APBCLKEN2;

        struct
        {
            __IOM uint32_t SCFGEN         : 1;
            __IM  uint32_t RESERVED1      : 8;
            __IOM uint32_t ADCEN          : 1;
            __IM  uint32_t RESERVED2      : 1;
            __IOM uint32_t TMR1EN         : 1;
            __IOM uint32_t SPIEN          : 1;
            __IM  uint32_t RESERVED3      : 1;
            __IOM uint32_t USART1EN       : 1;
            __IOM uint32_t TMR8EN         : 1;
            __IM  uint32_t RESERVED4      : 6;
            __IOM uint32_t DBGEN          : 1;
            __IOM uint32_t SMSEN          : 1;
            __IM  uint32_t RESERVED5      : 8;
        } APBCLKEN2_B;
    } APBCLKEN2_R;

    /* APB1 peripheral clock enable register 1 */
    union
    {
        __IOM uint32_t APBCLKEN1;

        struct
        {
            __IOM uint32_t TMR2EN         : 1;
            __IOM uint32_t TMR3EN         : 1;
            __IOM uint32_t TMR4EN         : 1;
            __IM  uint32_t RESERVED1      : 1;
            __IOM uint32_t TMR6EN         : 1;
            __IOM uint32_t TMR7EN         : 1;
            __IM  uint32_t RESERVED2      : 11;
            __IOM uint32_t USART2EN       : 1;
            __IM  uint32_t RESERVED3      : 7;
            __IOM uint32_t CANEN          : 1;
            __IM  uint32_t RESERVED4      : 2;
            __IOM uint32_t PMUEN          : 1;
            __IM  uint32_t RESERVED5      : 3;
        } APBCLKEN1_B;
    } APBCLKEN1_R;

    /* RTC control register */
    union
    {
        __IOM uint32_t RTCCTRL;

        struct
        {
            __IM  uint32_t RESERVED1  : 8;
            __IOM uint32_t RTCSRCSEL  : 2;
            __IM  uint32_t RESERVED2  : 5;
            __IOM uint32_t RTCCLKEN   : 1;
            __IOM uint32_t RTCRST     : 1;
            __IM  uint32_t RESERVED3  : 15;
        } RTCCTRL_B;
    } RTCCTRL_R;

    /* Control/status register */
    union
    {
        __IOM uint32_t CSTS;

        struct
        {
            __IOM uint32_t LSIEN      : 1;
            __IM  uint32_t LSIRDYFLG  : 1;
            __IM  uint32_t RESERVED1  : 20;
            __IM  uint32_t LOCRSTFLG  : 1;
            __IM  uint32_t PWRRSTFLG  : 1;
            __IOM uint32_t RSTFLGCLR  : 1;
            __IM  uint32_t OBRSTFLG   : 1;
            __IM  uint32_t PINRSTFLG  : 1;
            __IM  uint32_t PODRSTFLG  : 1;
            __IM  uint32_t SWRSTFLG   : 1;
            __IM  uint32_t IWDTRSTFLG : 1;
            __IM  uint32_t RESERVED2  : 1;
            __IM  uint32_t LPWRRSTFLG : 1;
        } CSTS_B;
    } CSTS_R;

    /* AHB peripheral reset register */
    union
    {
        __IOM uint32_t AHBRST;

        struct
        {
            __IM  uint32_t RESERVED1  : 7;
            __IOM uint32_t TRNGEN     : 1;
            __IOM uint32_t SHAEN      : 1;
            __IOM uint32_t AES256EN   : 1;
            __IM  uint32_t RESERVED2  : 7;
            __IOM uint32_t PARST      : 1;
            __IOM uint32_t PBRST      : 1;
            __IOM uint32_t PCRST      : 1;
            __IOM uint32_t PDRST      : 1;
            __IM  uint32_t RESERVED3  : 1;
            __IOM uint32_t PFRST      : 1;
            __IM  uint32_t RESERVED4  : 9;
        } AHBRST_B;
    } AHBRST_R;

    /* Clock configuration register 2 */
    union
    {
        __IOM uint32_t CFG2;

        struct
        {
            __IOM uint32_t PLLDIVCFG  : 4;
            __IM  uint32_t RESERVED1  : 28;
        } CFG2_B;
    } CFG2_R;

    /*Clock configuration register 3 */
    union
    {
        __IOM uint32_t CFG3;

        struct
        {
            __IOM uint32_t USART1SEL  : 2;
            __IOM uint32_t USART2SEL  : 2;
            __IM  uint32_t RESERVED1  : 8;
            __IOM uint32_t CANSEL     : 1;
            __IM  uint32_t RESERVED2  : 19;
        } CFG3_B;
    } CFG3_R;

    union
    {
        __IOM uint32_t CTRL2;

        struct
        {
            __IOM uint32_t HSI14EN       : 1;
            __IM uint32_t HSI14RDFLG     : 1;
            __IOM uint32_t HSI14TO       : 1;
            __IOM uint32_t HSI14TRM      : 5;
            __IM uint32_t HSI14CAL       : 8;
            __IM  uint32_t RESERVED1     : 2;
            __IOM uint32_t LSITRM        : 6;
            __IM uint32_t LSICAL         : 8;
        } CTRL2_B;
    } CTRL2_R;

    /* reserved */
    __IM  uint32_t  RESERVED[2];

    /* Clock interrupt register 2 */
    union
    {
        __IOM uint32_t INT2;

        struct
        {
            __IM  uint32_t FLLFLG     : 1;
            __IM  uint32_t FHHFLG     : 1;
            __IM  uint32_t LOCFLG     : 1;
            __IM  uint32_t RESERVED1  : 5;
            __IOM uint32_t FLLIE      : 1;
            __IOM uint32_t FHHIE      : 1;
            __IOM uint32_t LOCIE      : 1;
            __IM  uint32_t RESERVED2  : 5;
            __OM  uint32_t FLLCLR     : 1;
            __OM  uint32_t FHHCLR     : 1;
            __OM  uint32_t LOCCLR     : 1;
            __IM  uint32_t RESERVED3  : 7;
            __IOM uint32_t SLOCEN     : 1;
            __IM  uint32_t RESERVED4  : 5;
        } INT2_B;
    } INT2_R;

    /* Clock configuration register 4 */
    union
    {
        __IOM uint32_t CFG4;

        struct
        {
            __IOM uint32_t CMU_EN        : 1;
            __IOM uint32_t CLK_SEL       : 1;
            __IM  uint32_t RESERVED1     : 6;
            __IM  uint32_t FLL           : 1;
            __IM  uint32_t FHH           : 1;
            __IOM uint32_t LOC           : 1;
            __IM  uint32_t VLD           : 1;
            __IM  uint32_t RESERVED2     : 4;
            __IOM uint32_t REFPRE        : 6;
            __IM  uint32_t RESERVED3     : 10;
        } CFG4_B;
    } CFG4_R;

    /* Reference Clock Period Configurate Register */
    union
    {
        __IOM uint32_t REF_CNT;

        struct
        {
            __IOM uint32_t REF_CNT    : 6;
            __IM  uint32_t RESERVED1  : 26;
        } REF_CNT_B;
    } REF_CNT_R;

    /* Measurement Result Latch Register */
    union
    {
        __IOM uint32_t MEAS;

        struct
        {
            __IOM uint32_t MEAS_VAL      : 16;
            __IM  uint32_t RESERVED1     : 16;
        } MEAS_B;
    } MEAS_R;

    /* Frequency Threshold Upper Limit Register */
    union
    {
        __IOM uint32_t FHCR;

        struct
        {
            __IOM uint32_t FH_VAL     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } FHCR_B;
    } FHCR_R;

    /* Frequency Threshold Lower Limit Register */
    union
    {
        __IOM uint32_t FLCR;

        struct
        {
            __IOM uint32_t FL_VAL     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } FLCR_B;
    } FLCR_R;
} RCM_T;

/**
* @brief Real-time clock (RTC)
*/

typedef struct
{

    /* time register */
    union
    {
        __IOM uint32_t TIME;

        struct
        {
            __IOM uint32_t SECU       : 4;
            __IOM uint32_t SECT       : 3;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t MINU       : 4;
            __IOM uint32_t MINT       : 3;
            __IM  uint32_t RESERVED2  : 1;
            __IOM uint32_t HRU        : 4;
            __IOM uint32_t HRT        : 2;
            __IOM uint32_t TIMEFCFG   : 1;
            __IM  uint32_t RESERVED3  : 9;
        } TIME_B;
    } TIME_R;

    /* date register */
    union
    {
        __IOM uint32_t DATE;

        struct
        {
            __IOM uint32_t DAYU       : 4;
            __IOM uint32_t DAYT       : 2;
            __IM  uint32_t RESERVED1  : 2;
            __IOM uint32_t MONU       : 4;
            __IOM uint32_t MONT       : 1;
            __IOM uint32_t WEEKSEL    : 3;
            __IOM uint32_t YRU        : 4;
            __IOM uint32_t YRT        : 4;
            __IM  uint32_t RESERVED2  : 8;
        } DATE_B;
    } DATE_R;

    /* control register */
    union
    {
        __IOM uint32_t CTRL;

        struct
        {
            __IM  uint32_t RESERVED1  : 5;
            __IOM uint32_t RCMCFG     : 1;
            __IOM uint32_t TIMEFCFG   : 1;
            __IM  uint32_t RESERVED2  : 1;
            __IOM uint32_t ALREN      : 1;
            __IM  uint32_t RESERVED3  : 3;
            __IOM uint32_t ALRIEN     : 1;
            __IM  uint32_t RESERVED4  : 1;
            __IOM uint32_t WUTIEN     : 1;
            __IM  uint32_t RESERVED5  : 1;
            __OM  uint32_t STCCFG     : 1;
            __OM  uint32_t WTCCFG     : 1;
            __IOM uint32_t BAKE       : 1;
            __IM  uint32_t RESERVED6  : 13;
        } CTRL_B;
    } CTRL_R;

    /* initialization and status register */
    union
    {
        __IOM uint32_t STS;

        struct
        {
            __IM  uint32_t ALRWFLG    : 1;
            __IM  uint32_t RESERVED1  : 2;
            __IM  uint32_t SOPFLG     : 1;
            __IM  uint32_t INITSFLG   : 1;
            __IOM uint32_t RSFLG      : 1;
            __IM  uint32_t RINITFLG   : 1;
            __IOM uint32_t INITEN     : 1;
            __IOM uint32_t ALRAFLG    : 1;
            __IM  uint32_t RESERVED2  : 7;
            __IM  uint32_t RCALPFLG   : 1;
            __IM  uint32_t RESERVED3  : 15;
        } STS_B;
    } STS_R;

    /* prescaler register */
    union
    {
        __IOM uint32_t PSC;

        struct
        {
            __IOM uint32_t SPSC       : 15;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t APSC       : 7;
            __IM  uint32_t RESERVED2  : 9;
        } PSC_B;
    } PSC_R;

    __IM  uint32_t  RESERVED[2];

    /* alarm A register */
    union
    {
        __IOM uint32_t ALRMA;

        struct
        {
            __IOM uint32_t SECU       : 4;
            __IOM uint32_t SECT       : 3;
            __IOM uint32_t SECMEN     : 1;
            __IOM uint32_t MINU       : 4;
            __IOM uint32_t MINT       : 3;
            __IOM uint32_t MINMEN     : 1;
            __IOM uint32_t HRU        : 4;
            __IOM uint32_t HRT        : 2;
            __IOM uint32_t TIMEFCFG   : 1;
            __IOM uint32_t HRMEN      : 1;
            __IOM uint32_t DAYU       : 4;
            __IOM uint32_t DAYT       : 2;
            __IOM uint32_t WEEKSEL    : 1;
            __IOM uint32_t DATEMEN    : 1;
        } ALRMA_B;
    } ALRMA_R;

    __IM  uint32_t  RESERVED1;

    /* write protection register */
    union
    {
        __OM  uint32_t WRPROT;

        struct
        {
            __OM  uint32_t KEY        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } WRPROT_B;
    } WRPROT_R;

    /* sub second register */
    union
    {
        __IM  uint32_t SUBSEC;

        struct
        {
            __IM  uint32_t SUBSEC     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } SUBSEC_B;
    } SUBSEC_R;

    /* shift control register */
    union
    {
        __OM  uint32_t SHIFT;

        struct
        {
            __OM  uint32_t SFSEC      : 15;
            __IM  uint32_t RESERVED1  : 16;
            __OM  uint32_t ADD1SECEN  : 1;
        } SHIFT_B;
    } SHIFT_R;

    /* reserved */
    __IM  uint32_t  RESERVED2[3];

    /* calibration register */
    union
    {
        __IOM uint32_t CAL;

        struct
        {
            __IOM uint32_t RECALF     : 9;
            __IM  uint32_t RESERVED1  : 4;
            __IOM uint32_t CAL16CFG   : 1;
            __IOM uint32_t CAL8CFG    : 1;
            __IOM uint32_t ICALFEN    : 1;
            __IM  uint32_t RESERVED2  : 16;
        } CAL_B;
    } CAL_R;

    /* reserved */
    __IM  uint32_t  RESERVED3[1];

    /* alarm A sub second register */
    union
    {
        __IOM uint32_t ALRMASS;

        struct
        {
            __IOM uint32_t SUBSEC     : 15;
            __IM  uint32_t RESERVED1  : 9;
            __IOM uint32_t MASKSEL    : 4;
            __IM  uint32_t RESERVED2  : 4;
        } ALRMASS_B;
    } ALRMASS_R;
} RTC_T;

/**
* @brief Secure Hash Algorithm 256-bit (SHA256)
*/

typedef struct
{
    /* version register */
    union
    {
        __IM  uint32_t REV;

        struct
        {
            __IM  uint32_t MIN        : 8;
            __IM  uint32_t MID        : 8;
            __IM  uint32_t MAJ        : 8;
            __IM  uint32_t RESERVED1  : 8;
        } REV_B;
    } REV_R;

    /* Control register */
    union
    {
        __IOM  uint32_t CTRL;

        struct
        {
            __IOM  uint32_t OP_START   : 1;
            __IOM  uint32_t IE         : 1;
            __IOM  uint32_t TYPE       : 4;
            __IOM  uint32_t INI_MODE   : 1;
            __IOM  uint32_t PAD_MODE   : 1;
            __IM   uint32_t RESERVED1  : 24;
        } CTRL_B;
    } CTRL_R;

    /* Status register */
    union
    {
        __IOM  uint32_t STATUS;

        struct
        {
            __IOM  uint32_t OP_DONE    : 1;
            __IM   uint32_t BUSY       : 1;
            __IM   uint32_t INTR       : 1;
            __IM   uint32_t RESERVED1  : 29;
        } STATUS_B;
    } STATUS_R;

    /* Block counter register */
    union
    {
        __IOM  uint32_t BLKCNT;

        struct
        {
            __IOM  uint32_t BLKN       : 6;
            __IM   uint32_t RESERVED1  : 26;
        } BLKCNT_B;
    } BLKCNT_R;

    /* DILH register */
    union
    {
        __IOM  uint32_t DILH;

        struct
        {
            __IOM  uint32_t DILH       : 32;
        } DILH_B;
    } DILH_R;

    /* DILL register */
    union
    {
        __IOM  uint32_t DILL;

        struct
        {
            __IOM  uint32_t DILL       : 32;
        } DILL_B;
    } DILL_R;

    /* DIN register */
    union
    {
        __OM  uint32_t DIN;

        struct
        {
            __OM  uint32_t DIN       : 32;
        } DIN_B;
    } DIN_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H0;

        struct
        {
            __IM  uint32_t DOUT_0  : 32;
        } DOUT_H0_B;
    } DOUT_H0_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H1;

        struct
        {
            __IM  uint32_t DOUT_1  : 32;
        } DOUT_H1_B;
    } DOUT_H1_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H2;

        struct
        {
            __IM  uint32_t DOUT_2  : 32;
        } DOUT_H2_B;
    } DOUT_H2_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H3;

        struct
        {
            __IM  uint32_t DOUT_3  : 32;
        } DOUT_H3_B;
    } DOUT_H3_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H4;

        struct
        {
            __IM  uint32_t DOUT_4  : 32;
        } DOUT_H4_B;
    } DOUT_H4_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H5;

        struct
        {
            __IM  uint32_t DOUT_5  : 32;
        } DOUT_H5_B;
    } DOUT_H5_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H6;

        struct
        {
            __IM  uint32_t DOUT_6  : 32;
        } DOUT_H6_B;
    } DOUT_H6_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H7;

        struct
        {
            __IM  uint32_t DOUT_7  : 32;
        } DOUT_H7_B;
    } DOUT_H7_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H8;

        struct
        {
            __IM  uint32_t DOUT_8  : 32;
        } DOUT_H8_B;
    } DOUT_H8_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H9;

        struct
        {
            __IM  uint32_t DOUT_9  : 32;
        } DOUT_H9_B;
    } DOUT_H9_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H10;

        struct
        {
            __IM  uint32_t DOUT_10  : 32;
        } DOUT_H10_B;
    } DOUT_H10_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H11;

        struct
        {
            __IM  uint32_t DOUT_11  : 32;
        } DOUT_H11_B;
    } DOUT_H11_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H12;

        struct
        {
            __IM  uint32_t DOUT_12  : 32;
        } DOUT_H12_B;
    } DOUT_H12_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H13;

        struct
        {
            __IM  uint32_t DOUT_13  : 32;
        } DOUT_H13_B;
    } DOUT_H13_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H14;

        struct
        {
            __IM  uint32_t DOUT_14  : 32;
        } DOUT_H14_B;
    } DOUT_H14_R;

    /* data output register */
    union
    {
        __IM  uint32_t DOUT_H15;

        struct
        {
            __IM  uint32_t DOUT_15  : 32;
        } DOUT_H15_B;
    } DOUT_H15_R;

    /* IV register */
    union
    {
        __OM  uint32_t DIGEST;

        struct
        {
            __OM  uint32_t IV       : 32;
        } DIGEST_B;
    } DIGEST_R;
} SHA256_T;

/**
* @brief Electronic Signature (SIGNATURE)
*/

typedef struct
{
    /* ID register */
    union
    {
        __IM uint32_t ID_0;

        struct
        {
            __IM uint32_t U_ID    : 32;
        } ID_0_B;
    } ID_0_R;

    /* ID register */
    union
    {
        __IM uint32_t ID_1;

        struct
        {
            __IM uint32_t U_ID    : 32;
        } ID_1_B;
    } ID_1_R;

    /* ID register */
    union
    {
        __IM uint32_t ID_2;

        struct
        {
            __IM uint32_t U_ID    : 32;
        } ID_2_B;
    } ID_2_R;

    /* flash information register */
    union
    {
        __IM uint32_t FLSINFO;

        struct
        {
            __IM uint32_t PF_SIZE    : 16;
            __IM uint32_t DF_SIZE    : 16;
        } FLSINFO_B;
    } FLSINFO_R;

    /* PID register */
    union
    {
        __IM uint32_t PID;

        struct
        {
            __IM uint32_t P_VERSION    : 8;
            __IM uint32_t P_SERIES1    : 8;
            __IM uint32_t P_SERIES2    : 8;
            __IM uint32_t RESERVED1    : 8;
        } PID_B;
    } PID_R;
} SIGNATURE_T;

/**
* @brief StaticwMemorywSubsystem (SMS)
*/

typedef struct
{
    /* reserved */
    __IM  uint32_t  RESERVED[1];

    /* ECC Configurate register */
    union
    {
        __IOM uint32_t ECC_CFG;

        struct
        {
            __IOM uint32_t ECC_EN       : 1;
            __IOM uint32_t RD_COR_EN    : 1;
            __IOM uint32_t RMW_COR_EN   : 1;
            __IOM uint32_t INSERT_ERR   : 2;
            __IM  uint32_t RESERVED1    : 27;
        } ECC_CFG_B;
    } ECC_CFG_R;

    /* Status register */
    union
    {
        __IM uint32_t STAT;

        struct
        {
            __IM uint32_t RD_ERR_DET    : 1;
            __IM uint32_t RD_ERR_SIN    : 1;
            __IM uint32_t RD_ERR_DBL    : 1;
            __IM uint32_t RMW_ERR_DET   : 1;
            __IM uint32_t RMW_ERR_SIN   : 1;
            __IM uint32_t RMW_ERR_DBL   : 1;
            __IM uint32_t RESERVED1     : 10;
            __IM uint32_t IF_INTR       : 1;
            __IM uint32_t RESERVED2     : 15;
        } STAT_B;
    } STAT_R;

    /* reserved */
    __IM  uint32_t  RESERVED1[61];

    /* error address register */
    union
    {
        __IM uint32_t ECC_LOGn;

        struct
        {
            __IM uint32_t RESERVED1     : 7;
            __IM uint32_t ADDR          : 13;
            __IM uint32_t RESERVED2     : 12;
        } ECC_LOGn_B;
    } ECC_LOGn_R;

    /* reserved */
    __IM  uint32_t  RESERVED2[15];

    /* interrupt clear register */
    union
    {
        __IOM uint32_t INTRRn;

        struct
        {
            __IOM uint32_t RD_ERR_DET    : 1;
            __IOM uint32_t RD_ERR_SIN    : 1;
            __IOM uint32_t RD_ERR_DBL    : 1;
            __IOM uint32_t RMW_ERR_DET   : 1;
            __IOM uint32_t RMW_ERR_SIN   : 1;
            __IOM uint32_t RMW_ERR_DBL   : 1;
            __IM  uint32_t RESERVED1     : 26;
        } INTRRn_B;
    } INTRRn_R;

    /* reserved */
    __IM  uint32_t  RESERVED3[15];

    /* interrupt mask register */
    union
    {
        __IOM uint32_t INTRMn;

        struct
        {
            __IOM uint32_t RD_ERR_DET    : 1;
            __IOM uint32_t RD_ERR_SIN    : 1;
            __IOM uint32_t RD_ERR_DBL    : 1;
            __IOM uint32_t RMW_ERR_DET   : 1;
            __IOM uint32_t RMW_ERR_SIN   : 1;
            __IOM uint32_t RMW_ERR_DBL   : 1;
            __IM  uint32_t RESERVED1     : 26;
        } INTRMn_B;
    } INTRMn_R;

    /* reserved */
    __IM  uint32_t  RESERVED4[15];

    /* interrupt flag register */
    union
    {
        __IM uint32_t INTRSn;

        struct
        {
            __IM uint32_t RD_ERR_DET    : 1;
            __IM uint32_t RD_ERR_SIN    : 1;
            __IM uint32_t RD_ERR_DBL    : 1;
            __IM uint32_t RMW_ERR_DET   : 1;
            __IM uint32_t RMW_ERR_SIN   : 1;
            __IM uint32_t RMW_ERR_DBL   : 1;
            __IM uint32_t RESERVED1     : 26;
        } INTRSn_B;
    } INTRSn_R;
} SMS_T;

/**
* @brief Serial peripheral interface (SPI)
*/

typedef struct
{
    /* control register 1 */
    union
    {
        __IOM uint32_t CTRL1;

        struct
        {
            __IOM uint32_t CPHA       : 1;
            __IOM uint32_t CPOL       : 1;
            __IOM uint32_t MSMCFG     : 1;
            __IOM uint32_t BRSEL      : 3;
            __IOM uint32_t SPIEN      : 1;
            __IOM uint32_t LSBSEL     : 1;
            __IOM uint32_t ISSEL      : 1;
            __IOM uint32_t SSEN       : 1;
            __IOM uint32_t RXOMEN     : 1;
            __IOM uint32_t CRCLSEL    : 1;
            __IOM uint32_t CRCNXT     : 1;
            __IOM uint32_t CRCEN      : 1;
            __IOM uint32_t BMOEN      : 1;
            __IOM uint32_t BMEN       : 1;
            __IM  uint32_t RESERVED1  : 16;
        } CTRL1_B;
    } CTRL1_R;

    /* control register 2 */
    union
    {
        __IOM uint32_t CTRL2;

        struct
        {
            __IOM uint32_t RXDEN      : 1;
            __IOM uint32_t TXDEN      : 1;
            __IOM uint32_t SSOEN      : 1;
            __IOM uint32_t NSSPEN     : 1;
            __IOM uint32_t FRFCFG     : 1;
            __IOM uint32_t ERRIEN     : 1;
            __IOM uint32_t RXBNEIEN   : 1;
            __IOM uint32_t TXBEIEN    : 1;
            __IOM uint32_t DSCFG      : 4;
            __IOM uint32_t FRTCFG     : 1;
            __IOM uint32_t LDRX       : 1;
            __IOM uint32_t LDTX       : 1;
            __IM  uint32_t RESERVED1  : 17;
        } CTRL2_B;
    } CTRL2_R;

    /* status register */
    union
    {
        __IOM uint32_t STS;

        struct
        {
            __IM  uint32_t RXBNEFLG   : 1;
            __IM  uint32_t TXBEFLG    : 1;
            __IM  uint32_t SCHDIR     : 1;
            __IM  uint32_t UDRFLG     : 1;
            __IOM uint32_t CRCEFLG    : 1;
            __IM  uint32_t MEFLG      : 1;
            __IM  uint32_t OVRFLG     : 1;
            __IM  uint32_t BSYFLG     : 1;
            __IM  uint32_t FREFLG     : 1;
            __IM  uint32_t FRLSEL     : 2;
            __IM  uint32_t FTLSEL     : 2;
            __IM  uint32_t RESERVED2  : 19;
        } STS_B;
    } STS_R;

    /* data register */
    union
    {
        __IOM uint32_t DATA;

        struct
        {
            __IOM uint32_t DATA       : 16;
            __IM  uint32_t RESERVED1  : 16;
        } DATA_B;
    } DATA_R;

    /* CRC polynomial register */
    union
    {
        __IOM uint32_t CRCPOLY;

        struct
        {
            __IOM uint32_t CRCPOLY    : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CRCPOLY_B;
    } CRCPOLY_R;

    /*RX CRC register */
    union
    {

        __IM  uint32_t RXCRC;

        struct
        {
            __IM  uint32_t RXCRC      : 16;
            __IM  uint32_t RESERVED1  : 16;
        } RXCRC_B;
    } RXCRC_R;

    /* TX CRC register */
    union
    {

        __IM  uint32_t TXCRC;

        struct
        {
            __IM  uint32_t TXCRC      : 16;
            __IM  uint32_t RESERVED1  : 16;
        } TXCRC_B;
    } TXCRC_R;
} SPI_T;

/**
* @brief System configuration controller (SYSCFG)
*/

typedef struct
{
    /* configuration register 1 */
    union
    {
        __IOM uint32_t CFG1;

        struct
        {
            __IOM uint32_t MMSEL        : 2;
            __IM  uint32_t RESERVED1    : 6;
            __IOM uint32_t ADCDMARMP    : 1;
            __IOM uint32_t USART1TXRMP  : 1;
            __IOM uint32_t USART1RXRMP  : 1;
            __IM  uint32_t RESERVED2    : 5;
            __IOM uint32_t TIM4CH3RMP   : 1;
            __IOM uint32_t TIM4UPRMP    : 1;
            __IM  uint32_t RESERVED3    : 14;
        } CFG1_B;
    } CFG1_R;
    __IM  uint32_t  RESERVED;

    /* external interrupt configuration register 1 */
    union
    {

        __IOM uint32_t EINTCFG1;

        struct
        {
            __IOM uint32_t EINT0      : 4;
            __IOM uint32_t EINT1      : 4;
            __IOM uint32_t EINT2      : 4;
            __IOM uint32_t EINT3      : 4;
            __IM  uint32_t RESERVED1  : 16;
        } EINTCFG1_B;
    } EINTCFG1_R;

    /* external interrupt configuration register 2 */
    union
    {
        __IOM uint32_t EINTCFG2;

        struct
        {
            __IOM uint32_t EINT4      : 4;
            __IOM uint32_t EINT5      : 4;
            __IOM uint32_t EINT6      : 4;
            __IOM uint32_t EINT7      : 4;
            __IM  uint32_t RESERVED1  : 16;
        } EINTCFG2_B;
    } EINTCFG2_R;

    /* external interrupt configuration register 3 */
    union
    {
        __IOM uint32_t EINTCFG3;

        struct
        {
            __IOM uint32_t EINT8      : 4;
            __IOM uint32_t EINT9      : 4;
            __IOM uint32_t EINT10     : 4;
            __IOM uint32_t EINT11     : 4;
            __IM  uint32_t RESERVED1  : 16;
        } EINTCFG3_B;
    } EINTCFG3_R;

    /* external interrupt configuration register 4 */
    union
    {
        __IOM uint32_t EINTCFG4;

        struct
        {
            __IOM uint32_t EINT12     : 4;
            __IOM uint32_t EINT13     : 4;
            __IOM uint32_t EINT14     : 4;
            __IOM uint32_t EINT15     : 4;
            __IM  uint32_t RESERVED1  : 16;
        } EINTCFG4_B;
    } EINTCFG4_R;

    /* configuration register 2 */
    union
    {
        __IOM uint32_t CFG2;

        struct
        {
            __IOM uint32_t LOCK       : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t PVDLOCK    : 1;
            __IM  uint32_t RESERVED2  : 29;
        } CFG2_B;
    } CFG2_R;
} SYSCFG_T;

/**
* @brief Advanced-timers (TMR)
*/

typedef struct
{
    /* control register 1 */
    union
    {
        __IOM uint32_t CTRL1;

        struct
        {
            __IOM uint32_t CNTEN      : 1;
            __IOM uint32_t UD         : 1;
            __IOM uint32_t URSSEL     : 1;
            __IOM uint32_t SPMEN      : 1;
            __IOM uint32_t CNTDIR     : 1;
            __IOM uint32_t CAMSEL     : 2;
            __IOM uint32_t ARPEN      : 1;
            __IOM uint32_t CLKDIV     : 2;
            __IM  uint32_t RESERVED1  : 22;
        } CTRL1_B;
    } CTRL1_R;

    /* control register 2 */
    union
    {
        __IOM uint32_t CTRL2;

        struct
        {
            __IOM uint32_t CCPEN      : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t CCUSEL     : 1;
            __IOM uint32_t CCDSEL     : 1;
            __IOM uint32_t MMSEL      : 3;
            __IOM uint32_t TI1SEL     : 1;
            __IOM uint32_t OC1OIS     : 1;
            __IOM uint32_t OC1NOIS    : 1;
            __IOM uint32_t OC2OIS     : 1;
            __IOM uint32_t OC2NOIS    : 1;
            __IOM uint32_t OC3OIS     : 1;
            __IOM uint32_t OC3NOIS    : 1;
            __IOM uint32_t OC4OIS     : 1;
            __IM  uint32_t RESERVED2  : 17;
        } CTRL2_B;
    } CTRL2_R;

    /* slave mode control register */
    union
    {
        __IOM uint32_t SMCTRL;

        struct
        {
            __IOM uint32_t SMFSEL     : 3;
            __IOM uint32_t OCCSEL     : 1;
            __IOM uint32_t TRGSEL     : 3;
            __IOM uint32_t MSMEN      : 1;
            __IOM uint32_t ETFCFG     : 4;
            __IOM uint32_t ETPCFG     : 2;
            __IOM uint32_t ECEN       : 1;
            __IOM uint32_t ETPOL      : 1;
            __IM  uint32_t RESERVED1  : 16;
        } SMCTRL_B;
    } SMCTRL_R;

    /* DMA/Interrupt enable register */
    union
    {
        __IOM uint32_t DIEN;

        struct
        {
            __IOM uint32_t UIEN       : 1;
            __IOM uint32_t CC1IEN     : 1;
            __IOM uint32_t CC2IEN     : 1;
            __IOM uint32_t CC3IEN     : 1;
            __IOM uint32_t CC4IEN     : 1;
            __IOM uint32_t COMIEN     : 1;
            __IOM uint32_t TRGIEN     : 1;
            __IOM uint32_t BRKIEN     : 1;
            __IOM uint32_t UDIEN      : 1;
            __IOM uint32_t CC1DEN     : 1;
            __IOM uint32_t CC2DEN     : 1;
            __IOM uint32_t CC3DEN     : 1;
            __IOM uint32_t CC4DEN     : 1;
            __IOM uint32_t COMDEN     : 1;
            __IOM uint32_t TRGDEN     : 1;
            __IM  uint32_t RESERVED1  : 17;
        } DIEN_B;
    } DIEN_R;

    /* status register */
    union
    {
        __IOM uint32_t STS;

        struct
        {
            __IOM uint32_t UIFLG      : 1;
            __IOM uint32_t CC1IFLG    : 1;
            __IOM uint32_t CC2IFLG    : 1;
            __IOM uint32_t CC3IFLG    : 1;
            __IOM uint32_t CC4IFLG    : 1;
            __IOM uint32_t COMIFLG    : 1;
            __IOM uint32_t TRGIFLG    : 1;
            __IOM uint32_t BRKIFLG    : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t CC1RCFLG   : 1;
            __IOM uint32_t CC2RCFLG   : 1;
            __IOM uint32_t CC3RCFLG   : 1;
            __IOM uint32_t CC4RCFLG   : 1;
            __IM  uint32_t RESERVED2  : 19;
        } STS_B;
    } STS_R;

    /* event generation register */
    union
    {
        __OM  uint32_t CEG;

        struct
        {
            __OM  uint32_t UEG        : 1;
            __OM  uint32_t CC1EG      : 1;
            __OM  uint32_t CC2EG      : 1;
            __OM  uint32_t CC3EG      : 1;
            __OM  uint32_t CC4EG      : 1;
            __OM  uint32_t COMG       : 1;
            __OM  uint32_t TEG        : 1;
            __OM  uint32_t BEG        : 1;
            __IM  uint32_t RESERVED1  : 24;
        } CEG_B;
    } CEG_R;

    union
    {
        /* capture/compare mode register (output mode) */
        union
        {
            __IOM uint32_t CCM1_OUTPUT;

            struct
            {
                __IOM uint32_t CC1SEL     : 2;
                __IOM uint32_t OC1FEN     : 1;
                __IOM uint32_t OC1PEN     : 1;
                __IOM uint32_t OC1MOD     : 3;
                __IOM uint32_t OC1CEN     : 1;
                __IOM uint32_t CC2SEL     : 2;
                __IOM uint32_t OC2FEN     : 1;
                __IOM uint32_t OC2PEN     : 1;
                __IOM uint32_t OC2MOD     : 3;
                __IOM uint32_t OC2CEN     : 1;
                __IM  uint32_t RESERVED1  : 16;
            } CCM1_OUTPUT_B;
        } CCM1_OUTPUT_R;

        /* capture/compare mode register 1 (input mode) */
        union
        {
            __IOM uint32_t CCM1_INPUT;

            struct
            {
                __IOM uint32_t CC1SEL     : 2;
                __IOM uint32_t IC1PSC     : 2;
                __IOM uint32_t IC1F       : 4;
                __IOM uint32_t CC2SEL     : 2;
                __IOM uint32_t IC2PSC     : 2;
                __IOM uint32_t IC2F       : 4;
                __IM  uint32_t RESERVED1  : 16;
            } CCM1_INPUT_B;
        } CCM1_INPUT_R;
    }CCM1R;

    union
    {
        /* capture/compare mode register (output mode) */
        union
        {
            __IOM uint32_t CCM2_OUTPUT;

            struct
            {
                __IOM uint32_t CC3SEL     : 2;
                __IOM uint32_t OC3FEN     : 1;
                __IOM uint32_t OC3PEN     : 1;
                __IOM uint32_t OC3MOD     : 3;
                __IOM uint32_t OC3CEN     : 1;
                __IOM uint32_t CC4SEL     : 2;
                __IOM uint32_t OC4FEN     : 1;
                __IOM uint32_t OC4PEN     : 1;
                __IOM uint32_t OC4MOD     : 3;
                __IOM uint32_t OC4CEN     : 1;
                __IM  uint32_t RESERVED1  : 16;
            } CCM2_OUTPUT_B;
        } CCM2_OUTPUT_R;

        /* capture/compare mode register 2 (input mode) */
        union
        {
            __IOM uint32_t CCM2_INPUT;

            struct
            {
                __IOM uint32_t CC3SEL     : 2;
                __IOM uint32_t IC3PSC     : 2;
                __IOM uint32_t IC3F       : 4;
                __IOM uint32_t CC4SEL     : 2;
                __IOM uint32_t IC4PSC     : 2;
                __IOM uint32_t IC4F       : 4;
                __IM  uint32_t RESERVED1  : 16;
            } CCM2_INPUT_B;
        } CCM2_INPUT_R;
    }CCM2R;

    /* capture/compare enable register */
    union
    {
        __IOM uint32_t CCEN;

        struct
        {
            __IOM uint32_t CC1EN      : 1;
            __IOM uint32_t CC1POL     : 1;
            __IOM uint32_t CC1NEN     : 1;
            __IOM uint32_t CC1NPOL    : 1;
            __IOM uint32_t CC2EN      : 1;
            __IOM uint32_t CC2POL     : 1;
            __IOM uint32_t CC2NEN     : 1;
            __IOM uint32_t CC2NPOL    : 1;
            __IOM uint32_t CC3EN      : 1;
            __IOM uint32_t CC3POL     : 1;
            __IOM uint32_t CC3NEN     : 1;
            __IOM uint32_t CC3NPOL    : 1;
            __IOM uint32_t CC4EN      : 1;
            __IOM uint32_t CC4POL     : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t CC4NPOL    : 1;
            __IM  uint32_t RESERVED2  : 16;
        } CCEN_B;
    } CCEN_R;

    /* counter */
    union
    {
        __IOM uint32_t CNT;

        struct
        {
            __IOM uint32_t CNT        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CNT_B;
    } CNT_R;

    /* prescaler */
    union
    {
        __IOM uint32_t PSC;

        struct
        {
            __IOM uint32_t PSC        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } PSC_B;
    } PSC_R;

    /* auto-reload register */
    union
    {
        __IOM uint32_t AUTORLD;

        struct
        {
            __IOM uint32_t AUTORLD    : 16;
            __IM  uint32_t RESERVED1  : 16;
        } AUTORLD_B;
    } AUTORLD_R;

    /* repetition counter register */
    union
    {
        __IOM uint32_t REPCNT;

        struct
        {
            __IOM uint32_t REPCNT     : 8;
            __IM  uint32_t RESERVED1  : 24;
        } REPCNT_B;
    } REPCNT_R;

    /* capture/compare register 1 */
    union
    {
        __IOM uint32_t CC1;

        struct
        {
            __IOM uint32_t CC1        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CC1_B;
    } CC1_R;

    /* capture/compare register 2 */
    union
    {
        __IOM uint32_t CC2;

        struct
        {
            __IOM uint32_t CC2        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CC2_B;
    } CC2_R;

    /* capture/compare register 3 */
    union
    {
        __IOM uint32_t CC3;

        struct
        {
            __IOM uint32_t CC3        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CC3_B;
    } CC3_R;

    /* capture/compare register 4 */
    union
    {
        __IOM uint32_t CC4;

        struct
        {
            __IOM uint32_t CC4        : 16;
            __IM  uint32_t RESERVED1  : 16;
        } CC4_B;
    } CC4_R;

    /* break and dead-time register */
    union
    {
        __IOM uint32_t BDT;

        struct
        {
            __IOM uint32_t DTS        : 8;
            __IOM uint32_t LOCKCFG    : 2;
            __IOM uint32_t IMOS       : 1;
            __IOM uint32_t RMOS       : 1;
            __IOM uint32_t BRKEN      : 1;
            __IOM uint32_t BRKPOL     : 1;
            __IOM uint32_t AOEN       : 1;
            __IOM uint32_t MOEN       : 1;
            __IM  uint32_t RESERVED1  : 16;
        } BDT_B;
    } BDT_R;

    /* DMA control register */
    union
    {
        __IOM uint32_t DCTRL;

        struct
        {
            __IOM uint32_t DBADDR     : 5;
            __IM  uint32_t RESERVED1  : 3;
            __IOM uint32_t DBLEN      : 5;
            __IM  uint32_t RESERVED2  : 19;
        } DCTRL_B;
    } DCTRL_R;

    /* DMA address for full transfer */
    union
    {
        __IOM uint32_t DMADDR;

        struct
        {
            __IOM uint32_t DMADDR     : 16;
            __IM  uint32_t RESERVED1  : 16;
        } DMADDR_B;
    } DMADDR_R;

    /* TMR14 Remap */
    union
    {
        __IOM uint32_t OPT;

        struct
        {
            __IOM uint32_t RMPSEL     : 2;
            __IM  uint32_t RESERVED1  : 30;
        } OPT_B;
    } OPT_R;
} TMR_T;

/**
* @brief True Random Number (TRNG)
*/

typedef struct
{
    /* Control register */
    union
    {
        __IOM uint32_t CTRL;

        struct
        {
            __IM  uint32_t RESERVED1  : 2;
            __IOM uint32_t TRNGEN     : 1;
            __IOM uint32_t INTEN      : 1;
            __IM  uint32_t RESERVED2  : 28;
        } CTRL_B;
    } CTRL_R;

    /* Status register */
    union
    {
        __IOM uint32_t STS;

        struct
        {
            __IM  uint32_t DATARDY    : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IM  uint32_t FSCSTS     : 1;
            __IM  uint32_t RESERVED2  : 3;
            __IOM uint32_t FSINT      : 1;
            __IM  uint32_t RESERVED3  : 25;
        } STS_B;
    } STS_R;

    /* Data register */
    union
    {
        __IM uint32_t DATA;

        struct
        {
            __IM  uint32_t DATA  : 32;
        } DATA_B;
    } DATA_R;
} TRNG_T;

/**
* @brief Universal synchronous asynchronous receiver transmitter (USART)
*/

typedef struct
{
    /* Control register 1 */
    union
    {
        __IOM uint32_t CTRL1;

        struct
        {
            __IOM uint32_t UEN        : 1;
            __IOM uint32_t USWMEN     : 1;
            __IOM uint32_t RXEN       : 1;
            __IOM uint32_t TXEN       : 1;
            __IOM uint32_t IDLEIEN    : 1;
            __IOM uint32_t RXBNEIEN   : 1;
            __IOM uint32_t TXCIEN     : 1;
            __IOM uint32_t TXBEIEN    : 1;
            __IOM uint32_t PEIEN      : 1;
            __IOM uint32_t PCFG       : 1;
            __IOM uint32_t PCEN       : 1;
            __IOM uint32_t WUPMCFG    : 1;
            __IOM uint32_t DBLCFG0    : 1;
            __IOM uint32_t RXMUTEEN   : 1;
            __IOM uint32_t CMIEN      : 1;
            __IOM uint32_t OSMCFG     : 1;
            __IOM uint32_t DDLTEN     : 5;
            __IOM uint32_t DLTEN      : 5;
            __IOM uint32_t RXTOIEN    : 1;
            __IM  uint32_t RESERVED1  : 5;
        } CTRL1_B;
    } CTRL1_R;

    /* Control register 2 */
    union
    {
        __IOM uint32_t CTRL2;

        struct
        {
            __IM  uint32_t RESERVED1  : 4;
            __IOM uint32_t ADDRLEN    : 1;
            __IOM uint32_t LBDLCFG    : 1;
            __IOM uint32_t LBDIEN     : 1;
            __IM  uint32_t RESERVED2  : 1;
            __IOM uint32_t LBCPOEN    : 1;
            __IOM uint32_t CPHA       : 1;
            __IOM uint32_t CPOL       : 1;
            __IOM uint32_t CLKEN      : 1;
            __IOM uint32_t STOPCFG    : 2;
            __IOM uint32_t LINMEN     : 1;
            __IOM uint32_t SWAPEN     : 1;
            __IOM uint32_t RXINVEN    : 1;
            __IOM uint32_t TXINVEN    : 1;
            __IOM uint32_t BINVEN     : 1;
            __IOM uint32_t MSBFEN     : 1;
            __IOM uint32_t ABRDEN     : 1;
            __IOM uint32_t ABRDCFG    : 2;
            __IOM uint32_t RXTODEN    : 1;
            __IOM uint32_t ADDRL      : 4;
            __IOM uint32_t ADDRH      : 4;
        } CTRL2_B;
    } CTRL2_R;

    /* Control register 3 */
    union
    {
        __IOM uint32_t CTRL3;

        struct
        {
            __IOM uint32_t ERRIEN     : 1;
            __IM  uint32_t RESERVED1  : 2;
            __IOM uint32_t HDEN       : 1;
            __IM  uint32_t RESERVED2  : 2;
            __IOM uint32_t DMARXEN    : 1;
            __IOM uint32_t DMATXEN    : 1;
            __IOM uint32_t RTSEN      : 1;
            __IOM uint32_t CTSEN      : 1;
            __IOM uint32_t CTSIEN     : 1;
            __IOM uint32_t SAMCFG     : 1;
            __IOM uint32_t OVRDEDIS   : 1;
            __IOM uint32_t DDISRXEEN  : 1;
            __IOM uint32_t DEN        : 1;
            __IOM uint32_t DPCFG      : 1;
            __IM  uint32_t RESERVED3  : 4;
            __IOM uint32_t WSIFLGSEL  : 2;
            __IOM uint32_t WSMIEN     : 1;
            __IM  uint32_t RESERVED4  : 9;
        } CTRL3_B;
    } CTRL3_R;

    /* Baud rate register */
    union
    {
        __IOM uint32_t BR;

        struct
        {
            __IOM uint32_t FBR        : 4;
            __IOM uint32_t IBR        : 12;
            __IM  uint32_t RESERVED1  : 16;
        } BR_B;
    } BR_R;

    __IM  uint32_t  RESERVED1;
    /* Receiver timeout register */
    union
    {
        __IOM uint32_t RXTO;

        struct
        {
            __IOM uint32_t RXTO       : 24;
            __IM  uint32_t RESERVED1  : 8;
        } RXTO_B;
    } RXTO_R;

    /* Request register */
    union
    {
        __OM uint32_t REQUEST;

        struct
        {
            __OM uint32_t ABRDQ      : 1;
            __OM uint32_t TXBFQ      : 1;
            __OM uint32_t MUTEQ      : 1;
            __OM uint32_t RXDFQ      : 1;
            __IM uint32_t RESERVED1  : 28;
        } REQUEST_B;
    } REQUEST_R;

    /* Interrupt & status register */
    union
    {
        __IM  uint32_t STS;

        struct
        {
            __IM  uint32_t PEFLG      : 1;
            __IM  uint32_t FEFLG      : 1;
            __IM  uint32_t NEFLG      : 1;
            __IM  uint32_t OVREFLG    : 1;
            __IM  uint32_t IDLEFLG    : 1;
            __IM  uint32_t RXBNEFLG   : 1;
            __IM  uint32_t TXCFLG     : 1;
            __IM  uint32_t TXBEFLG    : 1;
            __IM  uint32_t LBDFLG     : 1;
            __IM  uint32_t CTSFLG     : 1;
            __IM  uint32_t CTSCFG     : 1;
            __IM  uint32_t RXTOFLG    : 1;
            __IM  uint32_t RESERVED1  : 2;
            __IM  uint32_t ABRDEFLG   : 1;
            __IM  uint32_t ABRDFLG    : 1;
            __IM  uint32_t BSYFLG     : 1;
            __IM  uint32_t CMFLG      : 1;
            __IM  uint32_t TXBFFLG    : 1;
            __IM  uint32_t RXWFMUTE   : 1;
            __IM  uint32_t WSMFLG     : 1;
            __IM  uint32_t TXENACKFLG : 1;
            __IM  uint32_t RXENACKFLG : 1;
            __IM  uint32_t RESERVED2  : 9;
        } STS_B;
    } STS_R;

    /* Interrupt flag clear register */
    union
    {
        __IOM uint32_t INTFCLR;

        struct
        {
            __IOM uint32_t PECLR      : 1;
            __IOM uint32_t FECLR      : 1;
            __IOM uint32_t NECLR      : 1;
            __IOM uint32_t OVRECLR    : 1;
            __IOM uint32_t IDLECLR    : 1;
            __IM  uint32_t RESERVED1  : 1;
            __IOM uint32_t TXCCLR     : 1;
            __IM  uint32_t RESERVED2  : 1;
            __IOM uint32_t LBDCLR     : 1;
            __IOM uint32_t CTSCLR     : 1;
            __IM  uint32_t RESERVED3  : 1;
            __IOM uint32_t RXTOCLR    : 1;
            __IM  uint32_t RESERVED4  : 5;
            __IOM uint32_t CMCLR      : 1;
            __IM  uint32_t RESERVED5  : 2;
            __IOM uint32_t WSMCLR     : 1;
            __IM  uint32_t RESERVED6  : 11;
        } INTFCLR_B;
    } INTFCLR_R;

    /* Receive data register */
    union
    {
        __IM  uint32_t RXDATA;

        struct
        {
            __IM  uint32_t RXDATA     : 9;
            __IM  uint32_t RESERVED1  : 23;
        } RXDATA_B;
    } RXDATA_R;

    /* Transmit data register */
    union
    {
        __IOM uint32_t TXDATA;

        struct
        {
            __IOM uint32_t TXDATA     : 9;
            __IM  uint32_t RESERVED1  : 23;
        } TXDATA_B;
    } TXDATA_R;
} USART_T;


/**@} end of group Peripheral_registers_structures*/

/** @defgroup Peripheral_memory_map
  @{
*/

/*@} end of group Device_Register*/

/* FMC base address in the alias region */
#define FMC_BASE                ((uint32_t)0x08000000U)
/** FMC base address in the alias region */
#define DFLASH_BASE             ((uint32_t)0x08040000U)
/** SRAM base address in the alias region */
#define SRAM_BASE               ((uint32_t)0x20000000U)
/* Peripheral base address in the alias region */
#define PERIPH_BASE             ((uint32_t)0x40000000U)

/* Peripheral memory map */
#define APBPERIPH_BASE           PERIPH_BASE
#define AHBPERIPH_BASE          (PERIPH_BASE + 0x00020000U)
#define AHB2PERIPH_BASE         (PERIPH_BASE + 0x08000000U)

#define TMR2_BASE               (APBPERIPH_BASE + 0x00000000U)
#define TMR3_BASE               (APBPERIPH_BASE + 0x00000400U)
#define TMR6_BASE               (APBPERIPH_BASE + 0x00001000U)
#define TMR4_BASE               (APBPERIPH_BASE + 0x00002000U)
#define RTC_BASE                (APBPERIPH_BASE + 0x00002800U)
#define IWDT_BASE               (APBPERIPH_BASE + 0x00003000U)
#define USART2_BASE             (APBPERIPH_BASE + 0x00004400U)
#define SMS_BASE                (APBPERIPH_BASE + 0x00005400U)
#define CAN_ERM_BASE            (APBPERIPH_BASE + 0x00005800U)
#define CAN_SRAM_BASE           (APBPERIPH_BASE + 0x00005C00U)
#define CAN_BASE                (APBPERIPH_BASE + 0x00006400U)
#define PMU_BASE                (APBPERIPH_BASE + 0x00007000U)

#define SYSCFG_BASE             (APBPERIPH_BASE + 0x00010000U)
#define EINT_BASE               (APBPERIPH_BASE + 0x00010400U)
#define ADC_BASE                (APBPERIPH_BASE + 0x00012400U)
#define TMR1_BASE               (APBPERIPH_BASE + 0x00012C00U)
#define SPI_BASE                (APBPERIPH_BASE + 0x00013000U)
#define USART1_BASE             (APBPERIPH_BASE + 0x00013800U)
#define TMR7_BASE               (APBPERIPH_BASE + 0x00014000U)
#define TMR8_BASE               (APBPERIPH_BASE + 0x00014400U)
#define DBG_BASE                (APBPERIPH_BASE + 0x00015800U)

#define DMA_BASE                 AHBPERIPH_BASE
#define DMA1_CHANNEL_1_BASE     (DMA_BASE + 0x00000008U)
#define DMA1_CHANNEL_2_BASE     (DMA_BASE + 0x0000001CU)
#define DMA1_CHANNEL_3_BASE     (DMA_BASE + 0x00000030U)
#define DMA1_CHANNEL_4_BASE     (DMA_BASE + 0x00000044U)
#define DMA1_CHANNEL_5_BASE     (DMA_BASE + 0x00000058U)

#define RCM_BASE                (AHBPERIPH_BASE + 0x00001000U)
#define FMC_R_BASE              (AHBPERIPH_BASE + 0x00002000U)
#define CRC_BASE                (AHBPERIPH_BASE + 0x00003000U)
#define AES256_BASE             (AHBPERIPH_BASE + 0x00006000U)
#define SHA256_BASE             (AHBPERIPH_BASE + 0x00006400U)
#define TRNG_BASE               (AHBPERIPH_BASE + 0x00006800U)

#define SIGNATURE_BASE                 ((uint32_t)0x1FFFF440U)
#define OB_BASE                        ((uint32_t)0x1FFFF800U)

#define GPIOA_BASE              (AHB2PERIPH_BASE + 0x00000000U)
#define GPIOB_BASE              (AHB2PERIPH_BASE + 0x00000400U)
#define GPIOC_BASE              (AHB2PERIPH_BASE + 0x00000800U)
#define GPIOD_BASE              (AHB2PERIPH_BASE + 0x00000C00U)
#define GPIOF_BASE              (AHB2PERIPH_BASE + 0x00001400U)

/**@} end of group Peripheral_memory_map*/

/** @defgroup Peripheral_declaration
  @{
*/

#define ADC                     ((ADC_T*)           ADC_BASE)
#define AES256                  ((AES256_T*)        AES256_BASE)
#define CAN                     ((CAN_T*)           CAN_BASE)
#define CAN_ERM                 ((CAN_ERM_T*)       CAN_ERM_BASE)
#define CRC                     ((CRC_T*)           CRC_BASE)
#define DBG                     ((DBG_T*)           DBG_BASE)
#define EINT                    ((EINT_T*)          EINT_BASE)
#define FMC                     ((FMC_T*)           FMC_R_BASE)
#define IWDT                    ((IWDT_T*)          IWDT_BASE)
#define OB                      ((OB_T*)            OB_BASE)
#define PMU                     ((PMU_T*)           PMU_BASE)
#define RCM                     ((RCM_T*)           RCM_BASE)
#define RTC                     ((RTC_T*)           RTC_BASE)
#define SHA256                  ((SHA256_T*)        SHA256_BASE)
#define SIGNATURE               ((SIGNATURE_T*)     SIGNATURE_BASE)
#define SMS                     ((SMS_T*)           SMS_BASE)
#define SPI                     ((SPI_T*)           SPI_BASE)
#define SYSCFG                  ((SYSCFG_T*)        SYSCFG_BASE)
#define TRNG                    ((TRNG_T*)          TRNG_BASE)
#define USART1                  ((USART_T*)         USART1_BASE)
#define USART2                  ((USART_T*)         USART2_BASE)

#define DMA1                     ((DMA_T*)           DMA_BASE)
#define DMA1_CHANNEL_1          ((DMA_CHANNEL_T*)   DMA1_CHANNEL_1_BASE)
#define DMA1_CHANNEL_2          ((DMA_CHANNEL_T*)   DMA1_CHANNEL_2_BASE)
#define DMA1_CHANNEL_3          ((DMA_CHANNEL_T*)   DMA1_CHANNEL_3_BASE)
#define DMA1_CHANNEL_4          ((DMA_CHANNEL_T*)   DMA1_CHANNEL_4_BASE)
#define DMA1_CHANNEL_5          ((DMA_CHANNEL_T*)   DMA1_CHANNEL_5_BASE)

#define GPIOF                   ((GPIO_T*)          GPIOF_BASE)
#define GPIOD                   ((GPIO_T*)          GPIOD_BASE)
#define GPIOC                   ((GPIO_T*)          GPIOC_BASE)
#define GPIOB                   ((GPIO_T*)          GPIOB_BASE)
#define GPIOA                   ((GPIO_T*)          GPIOA_BASE)

#define TMR1                    ((TMR_T*)           TMR1_BASE)
#define TMR2                    ((TMR_T*)           TMR2_BASE)
#define TMR3                    ((TMR_T*)           TMR3_BASE)
#define TMR4                    ((TMR_T*)           TMR4_BASE)
#define TMR6                    ((TMR_T*)           TMR6_BASE)
#define TMR7                    ((TMR_T*)           TMR7_BASE)
#define TMR8                    ((TMR_T*)           TMR8_BASE)

/**@} end of group Peripheral_declaration*/

/** @defgroup Exported_Macros
  @{
*/

/* Define one bit mask */
#define BIT0    0x00000001U
#define BIT1    0x00000002U
#define BIT2    0x00000004U
#define BIT3    0x00000008U
#define BIT4    0x00000010U
#define BIT5    0x00000020U
#define BIT6    0x00000040U
#define BIT7    0x00000080U
#define BIT8    0x00000100U
#define BIT9    0x00000200U
#define BIT10   0x00000400U
#define BIT11   0x00000800U
#define BIT12   0x00001000U
#define BIT13   0x00002000U
#define BIT14   0x00004000U
#define BIT15   0x00008000U
#define BIT16   0x00010000U
#define BIT17   0x00020000U
#define BIT18   0x00040000U
#define BIT19   0x00080000U
#define BIT20   0x00100000U
#define BIT21   0x00200000U
#define BIT22   0x00400000U
#define BIT23   0x00800000U
#define BIT24   0x01000000U
#define BIT25   0x02000000U
#define BIT26   0x04000000U
#define BIT27   0x08000000U
#define BIT28   0x10000000U
#define BIT29   0x20000000U
#define BIT30   0x40000000U
#define BIT31   0x80000000U

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/**@} end of group Exported_Macros*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* G32A10xx_H */

/**@} end of group G32A1085 */
/**@} end of group CMSIS */
