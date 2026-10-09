/*!
 * @file        g32a10xx_can.h
 *
 * @brief       This file contains all the functions prototypes for the Can firmware library
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

#ifndef G32A10xx_CAN_H
#define G32A10xx_CAN_H


#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "g32a10xx.h"
#include "g32a10xx_misc.h"
#include <string.h>
#include <stdlib.h>

/** @addtogroup G32A10xx_StdPeriphDriver
  @{
*/

/** @addtogroup CAN_Driver
  @{
*/

/** @defgroup CAN_Macros Macros
  @{
*/

/*******************************************************************************
                                MACROS
*******************************************************************************/
/*BIT MASK AND OFFSET FOR REGISTER: DBTP (Base + 0x0C) */
#define CAN_DBTP_DSJW_MASK                  ((uint32_t)0x0000000FU)
#define CAN_DBTP_DSJW_SHIFT                 ((uint8_t)0U)

#define CAN_DBTP_DTSEG2_MASK                ((uint32_t)0x000000F0U)
#define CAN_DBTP_DTSEG2_SHIFT               ((uint8_t)4U)

#define CAN_DBTP_DTSEG1_MASK                ((uint32_t)0x00001F00U)
#define CAN_DBTP_DTSEG1_SHIFT               ((uint8_t)8U)

#define CAN_DBTP_DBRP_MASK                  ((uint32_t)0x001F0000U)
#define CAN_DBTP_DBRP_SHIFT                 ((uint8_t)16U)

#define CAN_DBTP_TDC_MASK                   ((uint32_t)0x00800000U)
#define CAN_DBTP_TDC_SHIFT                  ((uint8_t)23U)

/***********************************************************************************************************
        BIT MASK AND OFFSET FOR REGISTER: NBTP (Base + 0x1C)
************************************************************************************************************/
#define CAN_NBTP_NTSEG2_MASK                ((uint32_t)0x0000007FU)
#define CAN_NBTP_NTSEG2_SHIFT               ((uint8_t)0U)

#define CAN_NBTP_NTSEG1_MASK                ((uint32_t)0x0000FF00U)
#define CAN_NBTP_NTSEG1_SHIFT               ((uint8_t)8U)

#define CAN_NBTP_NBRP_MASK                  ((uint32_t)0x01FF0000U)
#define CAN_NBTP_NBRP_SHIFT                 ((uint8_t)16U)

#define CAN_NBTP_NSJW_MASK                  ((uint32_t)0xFE000000U)
#define CAN_NBTP_NSJW_SHIFT                 ((uint8_t)25U)

/* BIT MASK AND OFFSET FOR REGISTER: IR (Base + 0x50) */
#define CAN_IR_RF0N_MASK                    ((uint32_t)0x00000001U)
#define CAN_IR_RF0N_SHIFT                   ((uint8_t)0U)

#define CAN_IR_RF0W_MASK                    ((uint32_t)0x00000002U)
#define CAN_IR_RF0W_SHIFT                   ((uint8_t)1U)

#define CAN_IR_RF0F_MASK                    ((uint32_t)0x00000004U)
#define CAN_IR_RF0F_SHIFT                   ((uint8_t)2U)

#define CAN_IR_RF0L_MASK                    ((uint32_t)0x00000008U)
#define CAN_IR_RF0L_SHIFT                   ((uint8_t)3U)

#define CAN_IR_RF1N_MASK                    ((uint32_t)0x00000010U)
#define CAN_IR_RF1N_SHIFT                   ((uint8_t)4U)

#define CAN_IR_RF1W_MASK                    ((uint32_t)0x00000020U)
#define CAN_IR_RF1W_SHIFT                   ((uint8_t)5U)

#define CAN_IR_RF1F_MASK                    ((uint32_t)0x00000040U)
#define CAN_IR_RF1F_SHIFT                   ((uint8_t)6U)

#define CAN_IR_RF1L_MASK                    ((uint32_t)0x00000080U)
#define CAN_IR_RF1L_SHIFT                   ((uint8_t)7U)

#define CAN_IR_HPM_MASK                     ((uint32_t)0x00000100U)
#define CAN_IR_HPM_SHIFT                    ((uint8_t)8U)

#define CAN_IR_TC_MASK                      ((uint32_t)0x00000200U)
#define CAN_IR_TC_SHIFT                     ((uint8_t)9U)

#define CAN_IR_TCF_MASK                     ((uint32_t)0x00000400U)
#define CAN_IR_TCF_SHIFT                    ((uint8_t)10U)

#define CAN_IR_TFE_MASK                     ((uint32_t)0x00000800U)
#define CAN_IR_TFE_SHIFT                    ((uint8_t)11U)

#define CAN_IR_TEFN_MASK                    ((uint32_t)0x00001000U)
#define CAN_IR_TEFN_SHIFT                   ((uint8_t)12U)

#define CAN_IR_TEFW_MASK                    ((uint32_t)0x00002000U)
#define CAN_IR_TEFW_SHIFT                   ((uint8_t)13U)

#define CAN_IR_TEFF_MASK                    ((uint32_t)0x00004000U)
#define CAN_IR_TEFF_SHIFT                   ((uint8_t)14U)

#define CAN_IR_TEFL_MASK                    ((uint32_t)0x00008000U)
#define CAN_IR_TEFL_SHIFT                   ((uint8_t)15U)

#define CAN_IR_TSW_MASK                     ((uint32_t)0x00010000U)
#define CAN_IR_TSW_SHIFT                    ((uint8_t)16U)

#define CAN_IR_MRAF_MASK                    ((uint32_t)0x00020000U)
#define CAN_IR_MRAF_SHIFT                   ((uint8_t)17U)

#define CAN_IR_TOO_MASK                     ((uint32_t)0x00040000U)
#define CAN_IR_TOO_SHIFT                    ((uint8_t)18U)

#define CAN_IR_DRX_MASK                     ((uint32_t)0x00080000U)
#define CAN_IR_DRX_SHIFT                    ((uint8_t)19U)

#define CAN_IR_ELO_MASK                     ((uint32_t)0x00400000U)
#define CAN_IR_ELO_SHIFT                    ((uint8_t)22U)

#define CAN_IR_EP_MASK                      ((uint32_t)0x00800000U)
#define CAN_IR_EP_SHIFT                     ((uint8_t)23U)

#define CAN_IR_EW_MASK                      ((uint32_t)0x01000000U)
#define CAN_IR_EW_SHIFT                     ((uint8_t)24U)

#define CAN_IR_BO_MASK                      ((uint32_t)0x02000000U)
#define CAN_IR_BO_SHIFT                     ((uint8_t)25U)

#define CAN_IR_WDI_MASK                     ((uint32_t)0x04000000U)
#define CAN_IR_WDI_SHIFT                    ((uint8_t)26U)

#define CAN_IR_PEA_MASK                     ((uint32_t)0x08000000U)
#define CAN_IR_PEA_SHIFT                    ((uint8_t)27U)

#define CAN_IR_PED_MASK                     ((uint32_t)0x10000000U)
#define CAN_IR_PED_SHIFT                    ((uint8_t)28U)

#define CAN_IR_ARA_MASK                     ((uint32_t)0x20000000U)
#define CAN_IR_ARA_SHIFT                    ((uint8_t)29U)

/* BIT MASK AND OFFSET FOR REGISTER: IE (Base + 0x54) */
#define CAN_IE_RF0NE_MASK                   ((uint32_t)0x00000001U)
#define CAN_IE_RF0NE_SHIFT                  ((uint8_t)0U)

#define CAN_IE_RF0WE_MASK                   ((uint32_t)0x00000002U)
#define CAN_IE_RF0WE_SHIFT                  ((uint8_t)1U)

#define CAN_IE_RF0FE_MASK                   ((uint32_t)0x00000004U)
#define CAN_IE_RF0FE_SHIFT                  ((uint8_t)2U)

#define CAN_IE_RF0LE_MASK                   ((uint32_t)0x00000008U)
#define CAN_IE_RF0LE_SHIFT                  ((uint8_t)3U)

#define CAN_IE_RF1NE_MASK                   ((uint32_t)0x00000010U)
#define CAN_IE_RF1NE_SHIFT                  ((uint8_t)4U)

#define CAN_IE_RF1WE_MASK                   ((uint32_t)0x00000020U)
#define CAN_IE_RF1WE_SHIFT                  ((uint8_t)5U)

#define CAN_IE_RF1FE_MASK                   ((uint32_t)0x00000040U)
#define CAN_IE_RF1FE_SHIFT                  ((uint8_t)6U)

#define CAN_IE_RF1LE_MASK                   ((uint32_t)0x00000080U)
#define CAN_IE_RF1LE_SHIFT                  ((uint8_t)7U)

#define CAN_IE_HPME_MASK                    ((uint32_t)0x00000100U)
#define CAN_IE_HPME_SHIFT                   ((uint8_t)8U)

#define CAN_IE_TCE_MASK                     ((uint32_t)0x00000200U)
#define CAN_IE_TCE_SHIFT                    ((uint8_t)9U)

#define CAN_IE_TCFE_MASK                    ((uint32_t)0x00000400U)
#define CAN_IE_TCFE_SHIFT                   ((uint8_t)10U)

#define CAN_IE_TFEE_MASK                    ((uint32_t)0x00000800U)
#define CAN_IE_TFEE_SHIFT                   ((uint8_t)11U)

#define CAN_IE_TEFNE_MASK                   ((uint32_t)0x00001000U)
#define CAN_IE_TEFNE_SHIFT                  ((uint8_t)12U)

#define CAN_IE_TEFWE_MASK                   ((uint32_t)0x00002000U)
#define CAN_IE_TEFWE_SHIFT                  ((uint8_t)13U)

#define CAN_IE_TEFFE_MASK                   ((uint32_t)0x00004000U)
#define CAN_IE_TEFFE_SHIFT                  ((uint8_t)14U)

#define CAN_IE_TEFLE_MASK                   ((uint32_t)0x00008000U)
#define CAN_IE_TEFLE_SHIFT                  ((uint8_t)15U)

#define CAN_IE_TSWE_MASK                    ((uint32_t)0x00010000U)
#define CAN_IE_TSWE_SHIFT                   ((uint8_t)16U)

#define CAN_IE_MRAFE_MASK                   ((uint32_t)0x00020000U)
#define CAN_IE_MRAFE_SHIFT                  ((uint8_t)17U)

#define CAN_IE_TOOE_MASK                    ((uint32_t)0x00040000U)
#define CAN_IE_TOOE_SHIFT                   ((uint8_t)18U)

#define CAN_IE_DRXE_MASK                    ((uint32_t)0x00080000U)
#define CAN_IE_DRXE_SHIFT                   ((uint8_t)19U)

#define CAN_IE_ELOE_MASK                    ((uint32_t)0x00400000U)
#define CAN_IE_ELOE_SHIFT                   ((uint8_t)22U)

#define CAN_IE_EPE_MASK                     ((uint32_t)0x00800000U)
#define CAN_IE_EPE_SHIFT                    ((uint8_t)23U)

#define CAN_IE_EWE_MASK                     ((uint32_t)0x01000000U)
#define CAN_IE_EWE_SHIFT                    ((uint8_t)24U)

#define CAN_IE_BOE_MASK                     ((uint32_t)0x02000000U)
#define CAN_IE_BOE_SHIFT                    ((uint8_t)25U)

#define CAN_IE_WDIE_MASK                    ((uint32_t)0x04000000U)
#define CAN_IE_WDIE_SHIFT                   ((uint8_t)26U)

#define CAN_IE_PEAE_MASK                    ((uint32_t)0x08000000U)
#define CAN_IE_PEAE_SHIFT                   ((uint8_t)27U)

#define CAN_IE_PEDE_MASK                    ((uint32_t)0x10000000U)
#define CAN_IE_PEDE_SHIFT                   ((uint8_t)28U)

#define CAN_IE_ARAE_MASK                    ((uint32_t)0x20000000U)
#define CAN_IE_ARAE_SHIFT                   ((uint8_t)29U)

/* BIT MASK AND OFFSET FOR REGISTER: ILS (Base + 0x58) */
#define CAN_ILS_RF0NL_MASK                  ((uint32_t)0x00000001U)
#define CAN_ILS_RF0NL_SHIFT                 ((uint8_t)0U)

#define CAN_ILS_RF0WL_MASK                  ((uint32_t)0x00000002U)
#define CAN_ILS_RF0WL_SHIFT                 ((uint8_t)1U)

#define CAN_ILS_RF0FL_MASK                  ((uint32_t)0x00000004U)
#define CAN_ILS_RF0FL_SHIFT                 ((uint8_t)2U)

#define CAN_ILS_RF0LL_MASK                  ((uint32_t)0x00000008U)
#define CAN_ILS_RF0LL_SHIFT                 ((uint8_t)3U)

#define CAN_ILS_RF1NL_MASK                  ((uint32_t)0x00000010U)
#define CAN_ILS_RF1NL_SHIFT                 ((uint8_t)4U)

#define CAN_ILS_RF1WL_MASK                  ((uint32_t)0x00000020U)
#define CAN_ILS_RF1WL_SHIFT                 ((uint8_t)5U)

#define CAN_ILS_RF1FL_MASK                  ((uint32_t)0x00000040U)
#define CAN_ILS_RF1FL_SHIFT                 ((uint8_t)6U)

#define CAN_ILS_RF1LL_MASK                  ((uint32_t)0x00000080U)
#define CAN_ILS_RF1LL_SHIFT                 ((uint8_t)7U)

#define CAN_ILS_HPML_MASK                   ((uint32_t)0x00000100U)
#define CAN_ILS_HPML_SHIFT                  ((uint8_t)8U)

#define CAN_ILS_TCL_MASK                    ((uint32_t)0x00000200U)
#define CAN_ILS_TCL_SHIFT                   ((uint8_t)9U)

#define CAN_ILS_TCFL_MASK                   ((uint32_t)0x00000400U)
#define CAN_ILS_TCFL_SHIFT                  ((uint8_t)10U)

#define CAN_ILS_TFEL_MASK                   ((uint32_t)0x00000800U)
#define CAN_ILS_TFEL_SHIFT                  ((uint8_t)11U)

#define CAN_ILS_TEFNL_MASK                  ((uint32_t)0x00001000U)
#define CAN_ILS_TEFNL_SHIFT                 ((uint8_t)12U)

#define CAN_ILS_TEFWL_MASK                  ((uint32_t)0x00002000U)
#define CAN_ILS_TEFWL_SHIFT                 ((uint8_t)13U)

#define CAN_ILS_TEFFL_MASK                  ((uint32_t)0x00004000U)
#define CAN_ILS_TEFFL_SHIFT                 ((uint8_t)14U)

#define CAN_ILS_TEFLL_MASK                  ((uint32_t)0x00008000U)
#define CAN_ILS_TEFLL_SHIFT                 ((uint8_t)15U)

#define CAN_ILS_TSWL_MASK                   ((uint32_t)0x00010000U)
#define CAN_ILS_TSWL_SHIFT                  ((uint8_t)16U)

#define CAN_ILS_MRAFL_MASK                  ((uint32_t)0x00020000U)
#define CAN_ILS_MRAFL_SHIFT                 ((uint8_t)17U)

#define CAN_ILS_TOOL_MASK                   ((uint32_t)0x00040000U)
#define CAN_ILS_TOOL_SHIFT                  ((uint8_t)18U)

#define CAN_ILS_DRXL_MASK                   ((uint32_t)0x00080000U)
#define CAN_ILS_DRXL_SHIFT                  ((uint8_t)19U)

#define CAN_ILS_ELOL_MASK                   ((uint32_t)0x00400000U)
#define CAN_ILS_ELOL_SHIFT                  ((uint8_t)22U)

#define CAN_ILS_EPL_MASK                    ((uint32_t)0x00800000U)
#define CAN_ILS_EPL_SHIFT                   ((uint8_t)23U)

#define CAN_ILS_EWL_MASK                    ((uint32_t)0x01000000U)
#define CAN_ILS_EWL_SHIFT                   ((uint8_t)24U)

#define CAN_ILS_BOL_MASK                    ((uint32_t)0x02000000U)
#define CAN_ILS_BOL_SHIFT                   ((uint8_t)25U)

#define CAN_ILS_WDIL_MASK                   ((uint32_t)0x04000000U)
#define CAN_ILS_WDIL_SHIFT                  ((uint8_t)26U)

#define CAN_ILS_PEAL_MASK                   ((uint32_t)0x08000000U)
#define CAN_ILS_PEAL_SHIFT                  ((uint8_t)27U)

#define CAN_ILS_PEDL_MASK                   ((uint32_t)0x10000000U)
#define CAN_ILS_PEDL_SHIFT                  ((uint8_t)28U)

#define CAN_ILS_ARAL_MASK                   ((uint32_t)0x20000000U)
#define CAN_ILS_ARAL_SHIFT                  ((uint8_t)29U)

/***********************************************************************************************************
        BIT MASK AND OFFSET FOR REGISTER: TDCR (Base + 0x48)
************************************************************************************************************/
#define CAN_TDCR_TDCF_MASK                  ((uint32_t)0x0000007FU)
#define CAN_TDCR_TDCF_SHIFT                 ((uint8_t)0U)

#define CAN_TDCR_TDCO_MASK                  ((uint32_t)0x00007F00U)
#define CAN_TDCR_TDCO_SHIFT                 ((uint8_t)8U)

/***********************************************************************************************************
        BIT MASK AND OFFSET FOR REGISTER: TEST (Base + 0x10)
************************************************************************************************************/
#define CAN_TEST_LBCK_MASK                  ((uint32_t)0x00000010U)
#define CAN_TEST_LBCK_SHIFT                 ((uint8_t)4U)

#define CAN_TEST_TX_MASK                    ((uint32_t)0x00000060U)
#define CAN_TEST_TX_SHIFT                   ((uint8_t)5U)

#define CAN_TEST_RX_MASK                    ((uint32_t)0x00000080U)
#define CAN_TEST_RX_SHIFT                   ((uint8_t)7U)

/* Can bit time related */
#define NSJW_MAX_VAL                        (CAN_NBTP_NSJW_MASK >> CAN_NBTP_NSJW_SHIFT)
#define NTSEG2_MAX_VAL                      (CAN_NBTP_NTSEG2_MASK >> CAN_NBTP_NTSEG2_SHIFT)
#define NTSEG1_MAX_VAL                      (CAN_NBTP_NTSEG1_MASK >> CAN_NBTP_NTSEG1_SHIFT)
#define NBRP_MAX_VAL                        (CAN_NBTP_NBRP_MASK >> CAN_NBTP_NBRP_SHIFT)

#define DSJW_MAX_VAL                        (CAN_DBTP_DSJW_MASK >> CAN_DBTP_DSJW_SHIFT)
#define DTSEG2_MAX_VAL                      (CAN_DBTP_DTSEG2_MASK >> CAN_DBTP_DTSEG2_SHIFT)
#define DTSEG1_MAX_VAL                      (CAN_DBTP_DTSEG1_MASK >> CAN_DBTP_DTSEG1_SHIFT)
#define DBRP_MAX_VAL                        (CAN_DBTP_DBRP_MASK >> CAN_DBTP_DBRP_SHIFT)

#define NBTP_MAX_TQ_VAL                     (NTSEG1_MAX_VAL + NTSEG2_MAX_VAL + 3U)
#define NBTP_MIN_TQ_VAL                     (3U)
#define DBTP_MAX_TQ_VAL                     (DTSEG1_MAX_VAL + DTSEG2_MAX_VAL + 3U)
#define DBTP_MIN_TQ_VAL                     (3U)

#define TDCOFF_MAX_VAL                      ((uint32_t)CAN_TDCR_TDCO_MASK >> CAN_TDCR_TDCO_SHIFT)

/* Max baudrate */
#define CANFD_MAX_BAUDRATE_VAL              (8000000U)
#define CAN_MAX_BAUDRATE_VAL                (1000000U)

/* The number of the Can module */
#define CAN_MODULE_NUM                      (1U)

/* The number of the Can Rx dedicate buffer */
#define CAN_RX_DEDICATE_BUF_NUM             (64U)

/* The element address shift value in the message ram */
#define MSG_RAM_ELE_ADDR_SHIFT              (2U)
/* The standard id shift bit in the frame */
#define CAN_FRAME_STD_ID_SHIFT              (18U)

/* CAN FD nominal/data sample point per CiA 1301 v1.0.0. */
#define CANFD_DATA_SP1                      (800U)
#define CANFD_DATA_SP2                      (750U)
#define CANFD_DATA_SP3                      (700U)
#define CANFD_DATA_SP4                      (625U)
#define CANFD_COMMON_SP                     (800U)

/* Classic CAN sample point per CiA 301 v4.2.0 and earlier. */
#define CAN_DATA_SP1                        (750U)
#define CAN_DATA_SP2                        (800U)
#define CAN_DATA_SP3                        (875U)
#define CAN_DATA_SP4                        (1000U)

#ifndef CAN_RETRY_COUNT
/* Set to 0 by default: wait/retry without a limit until the flag toggles (asserts or clears).
 * Override this macro if you need a bounded number of retries. */
#define CAN_RETRY_COUNT                     (0x00U)
#endif

/* The Rx Dedicated buffer id */
#define CAN_RX_BUF_ID_0                     ((uint8_t)0U)
#define CAN_RX_BUF_ID_1                     ((uint8_t)1U)
#define CAN_RX_BUF_ID_2                     ((uint8_t)2U) 
#define CAN_RX_BUF_ID_3                     ((uint8_t)3U)
#define CAN_RX_BUF_ID_4                     ((uint8_t)4U)
#define CAN_RX_BUF_ID_5                     ((uint8_t)5U)
#define CAN_RX_BUF_ID_6                     ((uint8_t)6U)
#define CAN_RX_BUF_ID_7                     ((uint8_t)7U)
#define CAN_RX_BUF_ID_8                     ((uint8_t)8U)
#define CAN_RX_BUF_ID_9                     ((uint8_t)9U) 
#define CAN_RX_BUF_ID_10                    ((uint8_t)10U)
#define CAN_RX_BUF_ID_11                    ((uint8_t)11U)
#define CAN_RX_BUF_ID_12                    ((uint8_t)12U)
#define CAN_RX_BUF_ID_13                    ((uint8_t)13U)
#define CAN_RX_BUF_ID_14                    ((uint8_t)14U)
#define CAN_RX_BUF_ID_15                    ((uint8_t)15U)
#define CAN_RX_BUF_ID_16                    ((uint8_t)16U)
#define CAN_RX_BUF_ID_17                    ((uint8_t)17U)
#define CAN_RX_BUF_ID_18                    ((uint8_t)18U)
#define CAN_RX_BUF_ID_19                    ((uint8_t)19U)
#define CAN_RX_BUF_ID_20                    ((uint8_t)20U)
#define CAN_RX_BUF_ID_21                    ((uint8_t)21U)
#define CAN_RX_BUF_ID_22                    ((uint8_t)22U)
#define CAN_RX_BUF_ID_23                    ((uint8_t)23U)
#define CAN_RX_BUF_ID_24                    ((uint8_t)24U)
#define CAN_RX_BUF_ID_25                    ((uint8_t)25U)
#define CAN_RX_BUF_ID_26                    ((uint8_t)26U)
#define CAN_RX_BUF_ID_27                    ((uint8_t)27U)
#define CAN_RX_BUF_ID_28                    ((uint8_t)28U)
#define CAN_RX_BUF_ID_29                    ((uint8_t)29U)
#define CAN_RX_BUF_ID_30                    ((uint8_t)30U)
#define CAN_RX_BUF_ID_31                    ((uint8_t)31U)
#define CAN_RX_BUF_ID_32                    ((uint8_t)32U)
#define CAN_RX_BUF_ID_33                    ((uint8_t)33U)
#define CAN_RX_BUF_ID_34                    ((uint8_t)34U)
#define CAN_RX_BUF_ID_35                    ((uint8_t)35U)
#define CAN_RX_BUF_ID_36                    ((uint8_t)36U)
#define CAN_RX_BUF_ID_37                    ((uint8_t)37U)
#define CAN_RX_BUF_ID_38                    ((uint8_t)38U)
#define CAN_RX_BUF_ID_39                    ((uint8_t)39U)
#define CAN_RX_BUF_ID_40                    ((uint8_t)40U)
#define CAN_RX_BUF_ID_41                    ((uint8_t)41U)
#define CAN_RX_BUF_ID_42                    ((uint8_t)42U)
#define CAN_RX_BUF_ID_43                    ((uint8_t)43U)
#define CAN_RX_BUF_ID_44                    ((uint8_t)44U)
#define CAN_RX_BUF_ID_45                    ((uint8_t)45U)
#define CAN_RX_BUF_ID_46                    ((uint8_t)46U)
#define CAN_RX_BUF_ID_47                    ((uint8_t)47U) 
#define CAN_RX_BUF_ID_48                    ((uint8_t)48U)
#define CAN_RX_BUF_ID_49                    ((uint8_t)49U)
#define CAN_RX_BUF_ID_50                    ((uint8_t)50U)
#define CAN_RX_BUF_ID_51                    ((uint8_t)51U)
#define CAN_RX_BUF_ID_52                    ((uint8_t)52U)
#define CAN_RX_BUF_ID_53                    ((uint8_t)53U)
#define CAN_RX_BUF_ID_54                    ((uint8_t)54U)
#define CAN_RX_BUF_ID_55                    ((uint8_t)55U)
#define CAN_RX_BUF_ID_56                    ((uint8_t)56U)
#define CAN_RX_BUF_ID_57                    ((uint8_t)57U)
#define CAN_RX_BUF_ID_58                    ((uint8_t)58U)
#define CAN_RX_BUF_ID_59                    ((uint8_t)59U)
#define CAN_RX_BUF_ID_60                    ((uint8_t)60U)
#define CAN_RX_BUF_ID_61                    ((uint8_t)61U)
#define CAN_RX_BUF_ID_62                    ((uint8_t)62U)
#define CAN_RX_BUF_ID_63                    ((uint8_t)63U)
#define CAN_RX_BUF_ID_NONE                  ((uint8_t)0xFFU)

/* The Tx Dedicated buffer id */
#define CAN_TX_BUF_ID_0                     ((uint8_t)0U)
#define CAN_TX_BUF_ID_1                     ((uint8_t)1U)
#define CAN_TX_BUF_ID_2                     ((uint8_t)2U)
#define CAN_TX_BUF_ID_3                     ((uint8_t)3U)
#define CAN_TX_BUF_ID_4                     ((uint8_t)4U)
#define CAN_TX_BUF_ID_5                     ((uint8_t)5U)
#define CAN_TX_BUF_ID_6                     ((uint8_t)6U)
#define CAN_TX_BUF_ID_7                     ((uint8_t)7U)
#define CAN_TX_BUF_ID_8                     ((uint8_t)8U)
#define CAN_TX_BUF_ID_9                     ((uint8_t)9U)
#define CAN_TX_BUF_ID_10                    ((uint8_t)10U)
#define CAN_TX_BUF_ID_11                    ((uint8_t)11U)
#define CAN_TX_BUF_ID_12                    ((uint8_t)12U)
#define CAN_TX_BUF_ID_13                    ((uint8_t)13U)
#define CAN_TX_BUF_ID_14                    ((uint8_t)14U)
#define CAN_TX_BUF_ID_15                    ((uint8_t)15U)
#define CAN_TX_BUF_ID_16                    ((uint8_t)16U)
#define CAN_TX_BUF_ID_17                    ((uint8_t)17U)
#define CAN_TX_BUF_ID_18                    ((uint8_t)18U)
#define CAN_TX_BUF_ID_19                    ((uint8_t)19U)
#define CAN_TX_BUF_ID_20                    ((uint8_t)20U)
#define CAN_TX_BUF_ID_21                    ((uint8_t)21U)
#define CAN_TX_BUF_ID_22                    ((uint8_t)22U)
#define CAN_TX_BUF_ID_23                    ((uint8_t)23U)
#define CAN_TX_BUF_ID_24                    ((uint8_t)24U)
#define CAN_TX_BUF_ID_25                    ((uint8_t)25U)
#define CAN_TX_BUF_ID_26                    ((uint8_t)26U)
#define CAN_TX_BUF_ID_27                    ((uint8_t)27U)
#define CAN_TX_BUF_ID_28                    ((uint8_t)28U)
#define CAN_TX_BUF_ID_29                    ((uint8_t)29U)
#define CAN_TX_BUF_ID_30                    ((uint8_t)30U)
#define CAN_TX_BUF_ID_31                    ((uint8_t)31U)
#define CAN_TX_FIFO_QUEUE_ID_32             ((uint8_t)32U)

/* Can data size type */
#define CAN_DATA_SIZE_8                     ((uint8_t)0U)
#define CAN_DATA_SIZE_12                    ((uint8_t)1U)
#define CAN_DATA_SIZE_16                    ((uint8_t)2U)
#define CAN_DATA_SIZE_20                    ((uint8_t)3U)
#define CAN_DATA_SIZE_24                    ((uint8_t)4U)
#define CAN_DATA_SIZE_32                    ((uint8_t)5U)
#define CAN_DATA_SIZE_48                    ((uint8_t)6U)
#define CAN_DATA_SIZE_64                    ((uint8_t)7U)

/**@} end of group CAN_Macros */


/** @defgroup CAN_Enumerations Enumerations
  @{
*/

/**
 * @brief    Can State
 */
typedef enum
{
    CAN_IDLE_STATE      = 0U, //!< Idle state
    CAN_RX_DATA_STATE   = 1U, //!< Rx data state
    CAN_RX_REMOTE_STATE = 2U, //!< Rx remote frame state
    CAN_TX_DATA_STATE   = 3U, //!< Tx data state
    CAN_TX_REMOTE_STATE = 4U, //!< Tx remote frame state
    CAN_RX_FIFO_STATE   = 5U  //!< FIFO Rx state
}Can_ModlueStateType;

/**
 * @brief    CAN TX/RX status
 */
typedef enum
{
    CAN_TX_BUSY              = 0U,  //!< Tx Buffer busy
    CAN_TX_IDLE              = 1U,  //!< Tx Buffer idle
    CAN_RX_BUSY              = 2U,  //!< Rx Buffer busy
    CAN_RX_IDLE              = 3U,  //!< Rx Buffer idle
    CAN_FIFO0_RX_NEW         = 4U,  //!< Rx FIFO 0 has new message
    CAN_FIFO0_RX_IDLE        = 5U,  //!< Rx FIFO 0 idle
    CAN_FIFO0_RX_REACH_WM    = 6U,  //!< Rx FIFO 0 reaches watermark
    CAN_FIFO0_RX_FULL        = 7U,  //!< Rx FIFO 0 full
    CAN_FIFO0_RX_LOST        = 8U,  //!< Rx FIFO 0 lost message
    CAN_FIFO1_RX_NEW         = 9U,  //!< Rx FIFO 1 has new message
    CAN_FIFO1_RX_IDLE        = 10U, //!< Rx FIFO 1 idle
    CAN_FIFO1_RX_REACH_WM    = 11U, //!< Rx FIFO 1 reaches watermark
    CAN_FIFO1_RX_FULL        = 12U, //!< Rx FIFO 1 full
    CAN_FIFO1_RX_LOST        = 13U, //!< Rx FIFO 1 lost message
    CAN_FIFO0_RX_BUSY        = 14U, //!< Rx FIFO 0 busy
    CAN_FIFO1_RX_BUSY        = 15U, //!< Rx FIFO 1 busy
    CAN_ERROR_STS            = 16U, //!< Can error
    CAN_NO_HANDLE_STS        = 17U  //!< No handle
}Can_TransferStsType;

/*!
 * @brief Can interrupt status
 */
typedef enum
{
    CAN_ARA_INT_STS     = CAN_IR_ARA_MASK, //!< Can Access a reserved address interrupt status
    CAN_PED_INT_STS     = CAN_IR_PED_MASK, //!< Can Data-phase protocol error interrupt status
    CAN_PEA_INT_STS     = CAN_IR_PEA_MASK, //!< Can Arbitration-phase protocol error interrupt status
    CAN_BO_INT_STS      = CAN_IR_BO_MASK,  //!< Can Bus off interrupt status
    CAN_EW_INT_STS      = CAN_IR_EW_MASK,  //!< Can Error interrupt status 
    CAN_EP_INT_STS      = CAN_IR_EP_MASK,  //!< Can Passive Error interrupt status
    CAN_RF0N_INT_STS    = CAN_IR_RF0N_MASK, //!< Can Rx FIFO 0 new message interrupt status
    CAN_RF0W_INT_STS    = CAN_IR_RF0W_MASK, //!< Can Rx FIFO 0 watermark reached interrupt status
    CAN_RF0F_INT_STS    = CAN_IR_RF0F_MASK, //!< Can Rx FIFO 0 full interrupt status
    CAN_RF0L_INT_STS    = CAN_IR_RF0L_MASK, //!< Can Rx FIFO 0 message lost interrupt status
    CAN_RF1N_INT_STS    = CAN_IR_RF1N_MASK, //!< Can Rx FIFO 1 new message interrupt status
    CAN_RF1W_INT_STS    = CAN_IR_RF1W_MASK, //!< Can Rx FIFO 1 watermark reached interrupt status
    CAN_RF1F_INT_STS    = CAN_IR_RF1F_MASK, //!< Can Rx FIFO 1 full interrupt status
    CAN_RF1L_INT_STS    = CAN_IR_RF1L_MASK, //!< Can Rx FIFO 1 message lost interrupt status
    CAN_TC_INT_STS      = CAN_IR_TC_MASK,   //!< Can Transmission completed interrupt status
    CAN_TCF_INT_STS     = CAN_IR_TCF_MASK,  //!< Can Transmission cancellation finished interrupt status
    CAN_TEFL_INT_STS    = CAN_IR_TEFL_MASK, //!< Can Tx Event FIFO element lost interrupt status
    CAN_TEFF_INT_STS    = CAN_IR_TEFF_MASK, //!< Can Tx Event FIFO full interrupt status
    CAN_TEFW_INT_STS    = CAN_IR_TEFW_MASK, //!< Can Tx Event FIFO reached watermark interrupt status
    CAN_TEFN_INT_STS    = CAN_IR_TEFN_MASK, //!< Can New Tx Event FIFO element interrupt status
    CAN_TFE_INT_STS     = CAN_IR_TFE_MASK,  //!< Can Tx FIFO empty interrupt status
    CAN_DRX_INT_STS     = CAN_IR_DRX_MASK   //!< Can RX Dedicate Buffer has new message interrupt status
}Can_InterruptStsType;

/*!
 * @brief Can interrupt enable 
 */
typedef enum
{
    CAN_BUSOFF_INT_EN  = CAN_IE_BOE_MASK, //!< Bus Off interrupt eanable
    CAN_ERROR_INT_EN   = CAN_IE_EPE_MASK, //!< Error interrupt enable
    CAN_WARNING_INT_EN = CAN_IE_EWE_MASK //!< Warning interrupt enable
}Can_InterruptEnType;

/*!
 * @brief Can Fram id type 
 */
typedef enum
{
    CAN_FRAME_STD_ID = 0x0U,   //!< Standard ID
    CAN_FRAME_EXT_ID = 0x1U    //!< Extend ID
} Can_FrameIdType;

/*!
 * @brief Can Fram type 
 */
typedef enum
{
    CAN_DATA_FRAME   = 0x0U, //!< Data frame
    CAN_REMOTE_FRAME = 0x1U  //!< Remote frame
} Can_FrameType;

/*!
 * @brief Can Rx FIFO type
 */
typedef enum
{
    CAN_RX_FIFO0 = 0x0U, //!< CAN Rx FIFO 0
    CAN_RX_FIFO1 = 0x1U //!< CAN Rx FIFO 1
} Can_RxFifoType;

/*!
 * @brief Can Rx FIFO operation type
 */
typedef enum
{
    CAN_RX_FIFO_BLOCK_OPERATION     = 0x0U, //!< FIFO blocking operation
    CAN_RX_FIFO_OVERWRITE_OPERATION = 0x1U //!< FIFO overwrite operation
} Can_RxFifoOperationType;

/*!
 * @brief Can Tx FIFO/Queue type
 */
typedef enum
{
    CAN_TX_FIFO_MODE  = 0x0U, //!< Tx FIFO mode
    CAN_TX_QUEUE_MODE = 0x1U //!< Tx Queue mode
} Can_TxBufQueorFifoType;

/*!
 * @brief remote frames treatment type
 */
typedef enum
{
    CAN_FILTER_REMOTE_FRAME = 0x0U, //!< Filter remote frames
    CAN_REJECT_REMOTE_FRAME = 0x1U  //!< Reject all remote frames
} Can_RemoteFrameTreatType;

/*!
 * @brief Non-matched frames treatment type
 */
typedef enum
{
    CAN_ACCEPT_FIFO0 = 0x0U, //!< Accept non-matched frames in Rx FIFO 0
    CAN_ACCEPT_FIFO1 = 0x1U, //!< Accept non-matched frames in Rx FIFO 1
    CAN_REJECT       = 0x2U  //!< Reject non-matched frames
} Can_NonMatchFramTreaTyp;

/*!
 * @brief Filter element operation type
 */
typedef enum
{
    CAN_FILTER_ELE_DISABLE             = 0x0U, //!< Disable filter element
    CAN_FILTER_STORE_IN_FIFO0          = 0x1U, //!< Store in Rx FIFO 0
    CAN_FILTER_STORE_IN_FIFO1          = 0x2U, //!< Store in Rx FIFO 1
    CAN_FILTER_REJECT                  = 0x3U, //!< Reject ID
    CAN_FILTER_SET_PRIORITY            = 0x4U, //!< Set priority
    CAN_FILTER_SET_PRIORITY_IN_FIFO0   = 0x5U, //!< Set priority and store in FIFO 0
    CAN_FILTER_SET_PRIORITY_IN_FIFO1   = 0x6U, //!< Set priority and store in FIFO 1
    CAN_FILTER_STORE_IN_RX_BUF         = 0x7U  //!< Store into Rx Buffer
} Can_FilterEleOperationType;

/*!
 * @brief Api return type
 */
typedef enum
{
    CAN_API_SUCCESS = 0U,
    CAN_API_FAIL    =1U
}Can_ApiRetStsType;

/*!
 * @brief Filter mode type
 */
typedef enum
{
    CAN_FILTER_RANGE_MODE           = 0x0U, //!< Range filter from SFID1 to SFID2
    CAN_FILTER_DUAL_MODE            = 0x1U, //!< Dual ID filter for SFID1 or SFID2
    CAN_FILTER_CLASSIC_MODE         = 0x2U, //!< Classic filter: SFID1 = filter, SFID2 = mask
    CAN_FILTER_DISABLED_MODE        = 0x3U  //!< Filter element disabled
} Can_FilterModeType;

/*!
 * @brief Type of last error to occur on the CAN node
 */
typedef enum
{
    CAN_NO_ERR       = 0U, //!< No Error occured in the recent CAN message transmission or reception
    CAN_STUFF_ERR    = 1U, //!< More than 5 consecutive equal bits recieved in CAN message
    CAN_FORM_ERR     = 2U, //!< Fixed format part of recieved frame has wrong format
    CAN_ACK_ERR      = 3U, //!< No ACK recieved from another node for transmitted CAN message
    CAN_BIT1_ERR     = 4U, //!< Transmitted recessive and read back dominant on the CAN bus
    CAN_bit0_ERR     = 5U, //!< Transmitted dominant and read back recessive on the CAN bus
    CAN_CRC_ERR      = 6U, //!< CRC Error */
    CAN_NO_BUS_EVT   = 7U  //!< No CAN bus event occured */
} Can_LastErrorCodeType;

/**@} end of group CAN_Enumerations */


/** @defgroup CAN_Structures Structures
  @{
*/

/* Rx buffer id type */
typedef uint8_t Can_RxBufIdType;
/* Tx buffer id type */
typedef uint8_t Can_TxBufferIdType;
/* Data region size type */
typedef uint8_t Can_DataSizeType;

/*!
 * @brief Tx buffer frame structure
 */
typedef struct
{
    struct
    {
        uint32_t id : 29; //!< Frame id
        uint32_t rtr : 1; //!< Frame type(DATA or REMOTE)
        uint32_t xtd : 1; //!< Frame id Type(STD or EXT)
        uint32_t esi : 1; //!< Frame error state
    }Can_TxFrameHead0;
    struct
    {
        uint32_t : 16;
        uint32_t dlc : 4; //!< Data length
        uint32_t brs : 1; //!< Bit rate switch
        uint32_t fdf : 1; //!< CAN FD enable status
        uint32_t : 1;     //!< Reserved
        uint32_t efc : 1; //!< Event FIFO control
        uint32_t mm : 8;  //!< Message Marker
    }Can_TxFrameHead1;

    uint8_t *data;
    uint8_t dataSize;
} Can_TxFrameType;

/*!
 * @brief Rx buffer frame structure
 */
typedef struct
{
    struct
    {
        uint32_t id : 29; //!< Frame id
        uint32_t rtr : 1; //!< Frame type(DATA or REMOTE)
        uint32_t xtd : 1; //!< Frame id type(STD or EXT)
        uint32_t esi : 1; //!< Frame error state
    }Can_RxFrameHead0;
    struct
    {
        uint32_t rxts : 16; //!< Rx Timestamp
        uint32_t dlc : 4;   //!< Data length
        uint32_t brs : 1;   //!< Bit rate switch
        uint32_t fdf : 1;   //!< CAN FD format
        uint32_t : 2;       //!< Reserved
        uint32_t fidx : 7;  //!< Filter index
        uint32_t anmf : 1;  //!< Accepted Non-matching Frame
    }Can_RxFrameHead1;
    uint8_t data[64];
} Can_RxFrameType;

/**
 * @brief    Tx Event FIFO Element 0
 */
typedef struct
{
    unsigned int id : 29;  //!< [28:0] Identifier (rwh)
    unsigned int rtr : 1;  //!< [29:29] Remote Transmission Request (rwh)
    unsigned int xtd : 1;  //!< [30:30] Extended Identifier (rwh)
    unsigned int esi : 1;  //!< [31:31] Error State Indicator (rwh)
}Can_TxEvtE0Type;

/**
 * @brief    Tx Event FIFO Element 1
 */
typedef struct
{
    unsigned int txts : 16;  //!< [15:0] Tx Timestamp (rwh)
    unsigned int dlc : 4;    //!< [19:16] Data Length Code (rwh)
    unsigned int brs : 1;    //!< [20:20] Bit Rate Switch (rwh)
    unsigned int fdf : 1;    //!< [21:21] FD Format (rwh)
    unsigned int et : 2;     //!< [23:22] Event Type (rwh)
    unsigned int mm : 8;     //!< [31:24] Message Marker (rwh)
}Can_TxEvtE1Type;

/**
 * @brief    Tx Event FIFO Element Structure
 */
typedef struct
{
    Can_TxEvtE0Type txEvtE0;
    Can_TxEvtE1Type txEvtE1;
}Can_TxEvtFifoEleType;

/*!
 * @brief Rx FIFO configuration type
 */
typedef struct
{
    uint32_t                address;                //!< FIFOx start address
    uint32_t                elementSize;            //!< FIFOx elements count
    uint32_t                watermark;              //!< FIFOx watermark level
    Can_RxFifoOperationType opmode;                 //!< FIFOx blocking/overwrite mode
    Can_DataSizeType        datafieldSize;          //!< Data field size
} Can_RxFifoConfigType;

/*!
 * @brief Rx buffer configuration type
 */
typedef struct
{
    uint32_t         address;                 //!< Rx Buffer start address
    Can_DataSizeType datafieldSize;           //!< Data field size
} Can_RxBufConfigType;

/*!
 * @brief Tx Event FIFO configuration type
 */
typedef struct
{
    uint32_t   address;     //!< Tx Event FIFO start address
    uint32_t   elementSize; //!< Tx Event FIFO element count
    uint32_t   watermark;   //!< Tx Event FIFO watermark level
} Can_TxFifoConfigType;

/*!
 * @brief Tx Buffer configuration type
 */
typedef struct
{
    uint32_t               address;            //!< Tx Buffers Start Address
    uint32_t               dedicatedSize;      //!< Number of Dedicated Transmit Buffers
    uint32_t               fifoQueCnt;         //!< Transmit FIFO/Queue count
    Can_TxBufQueorFifoType mode;               //!< Tx FIFO/Queue Mode
    Can_DataSizeType       datafieldSize;      //!< Data field size
} Can_TxBufConfigType;

/*!
 * @brief Standard Message ID Filter Element configuration type
 */
typedef struct
{
    uint32_t sfid2 : 11; //!< Filter ID 2
    uint32_t : 5;        //!< Reserved
    uint32_t sfid1 : 11; //!< Filter ID 1
    uint32_t sfec : 3;   //!< Filter Element operation
    uint32_t sft : 2;    //!< Filter mode
} Can_StdFilterEleConfigType;

/*!
 * @brief Extended Message ID Filter Element configuration type
 */
typedef struct 
{
    uint32_t efid1 : 29; //!< Filter ID 1
    uint32_t efec : 3;   //!< Filter Element operation
    uint32_t efid2 : 29; //!< Filter ID 2
    uint32_t : 1;        //!< Reserved
    uint32_t eft : 2;    //!< Filter mode
} Can_ExtFilterEleConfigType;

/*!
 * @brief Rx filter configuration type
 */
typedef struct
{
    uint32_t                 address;     //!< Filter start address
    uint32_t                 listSize;    //!< Filter list size
    Can_FrameIdType          idFormat;    //!< Frame id type
    Can_RemoteFrameTreatType remFrame;    //!< Remote frame treatment
    Can_NonMatchFramTreaTyp  nmFrame;     //!< Non-match frame treatment
} Can_FilterConfigType;

/*!
 * @brief Message RAM configuration type
 */
typedef struct
{
    Can_FilterConfigType *stdFilterMsgRamCfgPtr;    //!< Standard Frame ID Filter Configuration
    Can_FilterConfigType *extFilterMsgRamCfgPtr;    //!< Extended Frame ID Filter Configuration
    Can_RxFifoConfigType *rxFifo0MsgRamCfgPtr;      //!< Rx Fifo 0 configuration
    Can_RxFifoConfigType *rxFifo1MsgRamCfgPtr;      //!< Rx Fifo 1 configuration
    Can_RxBufConfigType  *rxDeBufMsgRamCfgPtr;      //!< Rx dedicated buffer configuration
    Can_TxFifoConfigType *txFifoMsgRamCfgPtr;       //!< Tx event Fifo configuration
    Can_TxBufConfigType  *txDeBufMsgRamCfgPtr;      //!< Tx dedicated buffer configuration
} Can_MessageRamConfig;

/*!
 * @brief Can transfer configuration type
 */
typedef struct
{
    Can_TxFrameType *txFramePtr;  //!< Buffer for CAN Messages to be Transferred
    Can_TxBufferIdType bufferIdx; //!< Tx buffer index
} Can_BufTransInfoType;

/*!
 * @brief Can Rx fifo tranfer configuration type
 */
typedef struct
{
    Can_RxFrameType *rxFramePtr; //!< Buffer for CAN Messages to be received
} Can_RxFifoTransInfoType;

typedef struct Can_HandlerDeal Can_HandleType;

/*!
 * @brief Can Rx/Tx callback type
 */
typedef void (*Can_RxTxCallbackType)(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_TransferStsType TranferSts, 
    uint32_t Result, uint8_t *InputParaPtr);

/* The callback function type of the interrupt */
typedef void (*Can_CallbackFunctionType)(CAN_T *ModulePtr, Can_HandleType *HandlePtr);

/*!
 * @brief Can baudrate configuration type
 */
typedef struct
{
    uint16_t  clkPsc;               //!< Nominal Clock Division
    uint8_t   resyncJumpWidth;      //!< Nominal Re-sync Jump Width
    uint8_t   phaseSeg1;            //!< Nominal Time Segment 1
    uint8_t   phaseSeg2;            //!< Nominal Time Segment 2
    uint16_t  dataClkPsc;           //!< Data Clock Division
    uint8_t   dataResyncJumpWidth;  //!< Data Re-sync Jump Width
    uint8_t   dataPhaseSeg1;        //!< Data Time Segment 1
    uint8_t   dataPhaseSeg2;        //!< Data Time Segment 2
} Can_BaudrateConfigType;

/*!
 * @brief Can module configuration type
 */
typedef struct
{
    uint32_t arbBaudrate;                       //!< Arbitration phase Baud rate 
    uint32_t dataBaudrate;                      //!< Data phase Baud rate
    uint8_t canfdNorEn;                         //!< Enable/Disable CAN FD
    uint8_t canfdBrsEn;                         //!< Enable/Disable baudrate switch
    uint8_t loopBackInterEn;                    //!< Enable/Disable Internal Loop Back
    uint8_t loopBackExtEn;                      //!< Enable/Disable External Loop Back
    uint8_t busMonEn;                           //!< Enable/Disable Bus Monitoring Mode
    Can_BaudrateConfigType baudrateConfig;      //!< Baudrate configuration
} Can_ConfigType;

/*!
 * @brief Can handle type
 */
struct Can_HandlerDeal
{
    Can_RxTxCallbackType callback;                    //!< Tx/Rx callback function
    uint8_t *InputParaPtr;                            //!< Callback input parameter
    Can_RxFrameType *volatile rxBufFrame[64U];        //!< Data Receiving Buffer
    Can_RxFrameType *volatile rxFifoFrameBuf[2U];     //!< Data Receiving Buffer for Rx FIFO
    volatile uint8_t txBufSts[32U];                    //!< TX Buffer transfer state
    volatile uint8_t rxBufSts[64U];                    //!< Rx Buffer transfer state
    volatile uint8_t rxFifoState[2U];                  //!< Rx FIFO transfer state
};

/**@} end of group CAN_Structures */
/*******************************************************************************
                            INLINE FUNCTIONS
*******************************************************************************/
/** @defgroup CAN_Functions Functions
  @{
*/

/*!
 * @brief Can switches to initialization mode
 *
 * @param ModulePtr  The pointer to the module
 */
static inline void Can_EnterInitMode(CAN_T *ModulePtr)
{
    ModulePtr->CCCR_R.CCCR_B.INIT = BIT_SET;

    while ((uint8_t)BIT_RESET == (ModulePtr->CCCR_R.CCCR_B.INIT))
    {
    }

    ModulePtr->CCCR_R.CCCR_B.CCE = BIT_SET;
}

/*!
 * @brief Can switches to normal mode
 *
 * @param ModulePtr  The pointer to the module
 */
static inline void Can_EnterNorMode(CAN_T *ModulePtr)
{
    ModulePtr->CCCR_R.CCCR_B.INIT = BIT_RESET;

    while ((uint8_t)BIT_SET != (ModulePtr->CCCR_R.CCCR_B.INIT))
    {
    }
}

/*!
 * @brief Read the interrupt flags
 *
 * @param ModulePtr  The pointer to the module
 * @param IntMask    The interrupt bit mask value
 * @return  The interrupt flag status
 */
static inline uint32_t Can_ReadIntFlag(const CAN_T *ModulePtr, uint32_t IntMask)
{
    return (ModulePtr->IR_R.IR & IntMask);
}

/*!
 * @brief Clear the interrupt flags
 *
 * @param ModulePtr  The pointer to the module
 * @param IntMask    The interrupt bit mask value
 */
static inline void Can_ClearIntFlag(CAN_T *ModulePtr, uint32_t IntMask)
{
    ModulePtr->IR_R.IR = IntMask;
}

/*!
 * @brief Read the new data flag of the Rx dedicate buffer
 *
 * @param ModulePtr  The pointer to the module
 * @param RxBufId    The Rx Buffer id
 * @return New data status flag
 */
static inline uint8_t Can_ReadRxBufNewDataFlg(const CAN_T *ModulePtr, Can_RxBufIdType RxBufId)
{
    uint8_t newDataFlg = 0U;

    if(RxBufId > 63U)
    {
        /* do nothing */
    }
    else
    {
        if (RxBufId <= 31U)
        {
            if(0U != (ModulePtr->NDAT1_R.NDAT1 & ((uint32_t)1U << RxBufId)))
            {
                newDataFlg = 1U;
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            if(0U != (ModulePtr->NDAT2_R.NDAT2 & ((uint32_t)1U << (RxBufId - 32U))))
            {
                newDataFlg = 1U;
            }
            else
            {
                /* do nothing */
            }
        }
    }

    return newDataFlg;
}

/*!
 * @brief Clear the new data flag of the Rx dedicate buffer
 *
 * @param ModulePtr  The pointer to the module
 * @param RxBufId    The Rx Buffer id
 */
static inline void Can_ClearRxBufNewDataFlg(CAN_T *ModulePtr, Can_RxBufIdType RxBufId)
{
    if(RxBufId > 63U)
    {
        /* do nothing */
    }
    else
    {
        if (RxBufId <= 31U)
        {
            ModulePtr->NDAT1_R.NDAT1 = ((uint32_t)1U << RxBufId);
        }
        else
        {
            ModulePtr->NDAT2_R.NDAT2 = ((uint32_t)1U << (RxBufId - 32U));
        }
    }
}

/*!
 * @brief Enable interrupts
 *
 * @param ModulePtr  The pointer to the module
 * @param LineId     The interrupt line id:0 or 1
 * @param IntMask    The interrupt bit mask
 */
static inline void Can_EnableInt(CAN_T *ModulePtr, uint32_t LineId, uint32_t IntMask)
{
    ModulePtr->ILE_R.ILE |= ((uint32_t)1U << LineId);

    if (0U == LineId)
    {
        ModulePtr->ILS_R.ILS &= ~IntMask;
    }
    else
    {
        ModulePtr->ILS_R.ILS |= IntMask;
    }

    ModulePtr->IE_R.IE |= IntMask;
}

/*!
 * @brief Disable interrupts
 *
 * @param ModulePtr  The pointer to the module
 * @param IntMask    The interrupt bit mask
 */
static inline void Can_DisableInt(CAN_T *ModulePtr, uint32_t IntMask)
{
    ModulePtr->IE_R.IE &= ~IntMask;
}

/*!
 * @brief Read the interrupts enabled status
 *
 * @param ModulePtr  The pointer to the module
 * @param IntMask    The interrupt bit mask
 */
static inline uint32_t Can_ReadIntEnStatus(const CAN_T *ModulePtr, uint32_t IntMask)
{
    return (ModulePtr->IE_R.IE & IntMask);
}

/*!
 * @brief Enable Tx buffer interrupts
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId    The Tx Buffer id
 */
static inline void Can_EnableTxBufInt(CAN_T *ModulePtr, Can_TxBufferIdType TxBufId)
{
    ModulePtr->TXBTIE_R.TXBTIE |= ((uint32_t)1U << TxBufId);
}

/*!
 * @brief Disable Tx buffer interrupts
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId    The Tx Buffer id
 */
static inline void Can_DisableTxBufInt(CAN_T *ModulePtr, Can_TxBufferIdType TxBufId)
{
    ModulePtr->TXBTIE_R.TXBTIE &= (~((uint32_t)1U << TxBufId));
}

/*!
 * @brief Request Tx buffer transmit
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId    The Tx Buffer id
 */
static inline void Can_TxAddReq(CAN_T *ModulePtr, Can_TxBufferIdType TxBufId)
{
    ModulePtr->TXBAR_R.TXBAR = ((uint32_t)1U << TxBufId);
}

/*!
 * @brief cancel sending
 *
 * @param ModulePtr  The pointer to the module
 * @param TxBufId    The Tx Buffer id
 */
static inline void Can_TxCancelReq(CAN_T *ModulePtr, Can_TxBufferIdType TxBufId)
{
    ModulePtr->TXBCR_R.TXBCR = ((uint32_t)1U << TxBufId);
}

/*!
 * @brief Get the BRS value from the Tx Event Fifo element
 *
 * @param TxEventFifoElePtr  The The pointer to the Tx Event Fifo element
 */
static inline uint8_t Can_GetBRSFromTxEventFifo(const Can_TxEvtFifoEleType *TxEventFifoElePtr)
{
    return ((uint8_t)TxEventFifoElePtr->txEvtE1.brs);
}

/*!
 * @brief Get the DLC value from the Tx Event Fifo element
 *
 * @param TxEventFifoElePtr  The The pointer to the Tx Event Fifo element
 */
static inline uint8_t Can_GetDLCFromTxEventFifo(const Can_TxEvtFifoEleType *TxEventFifoElePtr)
{
    return ((uint8_t)TxEventFifoElePtr->txEvtE1.dlc);
}

/*!
 * @brief Get the ESI value from the Tx Event Fifo element
 *
 * @param TxEventFifoElePtr  The The pointer to the Tx Event Fifo element
 */
static inline uint8_t Can_GetESIFromTxEventFifo(const Can_TxEvtFifoEleType *TxEventFifoElePtr)
{
    return ((uint8_t)TxEventFifoElePtr->txEvtE0.esi);
}

/*!
 * @brief Get the FDF value from the Tx Event Fifo element
 *
 * @param TxEventFifoElePtr  The The pointer to the Tx Event Fifo element
 */
static inline uint8_t Can_GetFDFFromTxEventFifo(const Can_TxEvtFifoEleType *TxEventFifoElePtr)
{
    return ((uint8_t)TxEventFifoElePtr->txEvtE1.fdf);
}

/*!
 * @brief Get the MM value from the Tx Event Fifo element
 *
 * @param TxEventFifoElePtr  The The pointer to the Tx Event Fifo element
 */
static inline uint8_t Can_GetMMFromTxEventFifo(const Can_TxEvtFifoEleType *TxEventFifoElePtr)
{
    return ((uint8_t)TxEventFifoElePtr->txEvtE1.mm);
}

/*!
 * @brief Get the RTR value from the Tx Event Fifo element
 *
 * @param TxEventFifoElePtr  The The pointer to the Tx Event Fifo element
 */
static inline uint8_t Can_GetRTRFromTxEventFifo(const Can_TxEvtFifoEleType *TxEventFifoElePtr)
{
    return ((uint8_t)TxEventFifoElePtr->txEvtE0.rtr);
}

/*!
 * @brief Get the TXTS value from the Tx Event Fifo element
 *
 * @param TxEventFifoElePtr  The The pointer to the Tx Event Fifo element
 */
static inline uint16_t Can_GetTXTSFromTxEventFifo(const Can_TxEvtFifoEleType *TxEventFifoElePtr)
{
    return ((uint16_t)TxEventFifoElePtr->txEvtE1.txts);
}

/*!
 * @brief Get the Tx Event Fifo Acknowledge Index
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetTxEventFifoAcknowledgeIndex(const CAN_T *ModulePtr)
{
    return ((uint8_t)ModulePtr->TXEFA_R.TXEFA_B.EFAI);
}

/*!
 * @brief Set the Tx Event Fifo Acknowledge Index
 *
 * @param ModulePtr  The pointer to the module
 * @param BufId      The buffer element id
 */
static inline void Can_SetTxEventFifoAcknowledgeIndex(CAN_T *ModulePtr, uint8_t BufId)
{
    ModulePtr->TXEFA_R.TXEFA_B.EFAI = BufId;
}

/*!
 * @brief Get the Tx Event Fifo Fill Level
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetTxEventFifoFillLevel(const CAN_T *ModulePtr)
{
    return ((uint8_t)ModulePtr->TXEFS_R.TXEFS_B.EFFL);
}

/*!
 * @brief Get the Tx Event Fifo Get Index
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetTxEventFifoGetIndex(const CAN_T *ModulePtr)
{
    return ((uint8_t)ModulePtr->TXEFS_R.TXEFS_B.EFGI);
}

/*!
 * @brief Get the Tx Event Fifo Put Index
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetTxEventFifoPutIndex(const CAN_T *ModulePtr)
{
    return ((uint8_t)ModulePtr->TXEFS_R.TXEFS_B.EFPI);
}

/*!
 * @brief Get the Tx Event Fifo Size
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetTxEventFifoSize(const CAN_T *ModulePtr)
{
    return ((uint8_t)ModulePtr->TXEFC_R.TXEFC_B.EFS);
}

/*!
 * @brief Get the Tx Event Fifo Watermark Level
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetTxEventFifoWatermarkLevel(const CAN_T *ModulePtr)
{
    return ((uint8_t)ModulePtr->TXEFC_R.TXEFC_B.EFWM);
}

/*!
 * @brief Get the Can last error status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetLastErroCodeStatus(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->PSR_R.PSR_B.LEC));
}

/*!
 * @brief Get the Can passive error status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_IsErrorPassive(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->PSR_R.PSR_B.EP));
}

/*!
 * @brief Get the Can error warning status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetWarningStatus(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->PSR_R.PSR_B.EW));
}

/*!
 * @brief Get the Can bus off status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetBusOffStatus(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->PSR_R.PSR_B.BO));
}

/*!
 * @brief Get the Can activity status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetActivityStatus(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->PSR_R.PSR_B.ACT));
}

/*!
 * @brief Get the Can data phase last error status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetDataPhaseLastErrorCode(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->PSR_R.PSR_B.DLEC));
}

/*!
 * @brief Get the Can Protocol Exception Event Occured status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_IsProtocolExceptionEventOccured(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->PSR_R.PSR_B.PXE));
}

/*!
 * @brief Set the Can FD frame type
 *
 * @param ModulePtr  The pointer to the module
 * @param FrameType  The Can FD frame type:0(ISO 11898), 1(BOSCH CAN FD)
 */
static inline void Can_SetFDFrameType(CAN_T *ModulePtr, uint8_t FrameType)
{
    ModulePtr->CCCR_R.CCCR_B.NISO = FrameType;
}

/*!
 * @brief Get the Can FD frame type
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetFDFrameType(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->CCCR_R.CCCR_B.NISO));
}

/*!
 * @brief Enable/Disable the Can Tx pause function
 *
 * @param ModulePtr  The pointer to the module
 * @param StsVal:    The value to be set
 */
static inline void Can_SetTxPause(CAN_T *ModulePtr, uint8_t StsVal)
{
    ModulePtr->CCCR_R.CCCR_B.TXP = StsVal;
}

/*!
 * @brief Get the Can Tx pause function status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetTxPause(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->CCCR_R.CCCR_B.TXP));
}

/*!
 * @brief Enable/Disable the Can Edge Filtering
 *
 * @param ModulePtr  The pointer to the module
 * @param StsVal:    The value to be set
 */
static inline void Can_SetEdgeFiltering(CAN_T *ModulePtr, uint8_t StsVal)
{
    ModulePtr->CCCR_R.CCCR_B.EFBI = StsVal;
}

/*!
 * @brief Get the Can Edge Filtering status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetEdgeFiltering(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->CCCR_R.CCCR_B.EFBI));
}

/*!
 * @brief Enable/Disable the Can Protocol Exception Handling
 *
 * @param ModulePtr  The pointer to the module
 * @param StsVal:    The value to be set
 */
static inline void Can_SetProtocolExcHandle(CAN_T *ModulePtr, uint8_t StsVal)
{
    ModulePtr->CCCR_R.CCCR_B.PXHD = StsVal;
}

/*!
 * @brief Get the Can Protocol Exception Handling status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetProtocolExcHandle(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->CCCR_R.CCCR_B.PXHD));
}

/*!
 * @brief Enable/Disable the Can Automatic Retransmission
 *
 * @param ModulePtr  The pointer to the module
 * @param StsVal:    The value to be set
 */
static inline void Can_SetAutomaticRetrans(CAN_T *ModulePtr, uint8_t StsVal)
{
    ModulePtr->CCCR_R.CCCR_B.DAR = StsVal;
}

/*!
 * @brief Get the Can Automatic Retransmission status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetAutomaticRetrans(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->CCCR_R.CCCR_B.DAR));
}

/*!
 * @brief Enable/Disable the Can Clock Stop Request
 *
 * @param ModulePtr  The pointer to the module
 * @param StsVal:    The value to be set
 */
static inline void Can_SetClkStopReq(CAN_T *ModulePtr, uint8_t StsVal)
{
    ModulePtr->CCCR_R.CCCR_B.CSR = StsVal;
}

/*!
 * @brief Get the Can Clock Stop Request status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetClkStopReq(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->CCCR_R.CCCR_B.CSR));
}

/*!
 * @brief Get the Can Clock Stop Acknowledge status
 *
 * @param ModulePtr  The pointer to the module
 */
static inline uint8_t Can_GetClkStopAck(const CAN_T *ModulePtr)
{
    return ((uint8_t)(ModulePtr->CCCR_R.CCCR_B.CSA));
}

/*!
 * @brief Enable Can Erm interrupts/event
 *
 * @param IntMask    The interrupt/event bit mask
 */
static inline void Can_EnableErmInt(uint32_t IntMask)
{
    CAN_ERM->CTRL_R.CTRL |= IntMask;
}

/*!
 * @brief Read the Erm status flags
 *
 * @param StsMask    The status bit mask value
 * @return  The status flag
 */
static inline uint32_t Can_ReadErmStsFlag(uint32_t StsMask)
{
    return (CAN_ERM->STS_R.STS & StsMask);
}

/*!
 * @brief Clear the Erm status flags
 *
 * @param StsMask    The status bit mask value
 */
static inline void Can_ClearErmStsFlag(uint32_t StsMask)
{
    CAN_ERM->STS_R.STS &= StsMask;
}

/*!
 * @brief Read the Erm Ecc error address
 * @return  The error address
 */
static inline uint32_t Can_ReadErmEccErrAddr(void)
{
    return (CAN_ERM->ECCLOG_R.ECCLOG_B.ERR_ADDR);
}


/*******************************************************************************
                            GLOBAL VARIABLES
*******************************************************************************/
 /* The Can handle array */
extern Can_HandleType *s_canHandleArr[CAN_MODULE_NUM];
/* The dlc region in the frame to the data bytes number mapping table */
extern uint8_t g_CanGblFdDlcConvDb[16];
/*******************************************************************************
                            FUNCTION DECLARATIONS
*******************************************************************************/
void Can_Init(CAN_T *ModulePtr, const Can_ConfigType *CanConfigPtr);
void Can_Deinit(CAN_T *ModulePtr);
void Can_SetArbTimConfig(CAN_T *ModulePtr, const Can_BaudrateConfigType *TimConfigPtr);

void Can_SetDataTimConfig(CAN_T *ModulePtr, const Can_BaudrateConfigType *TimConfigPtr);

void Can_SetRxFifo0Config(CAN_T *ModulePtr, const Can_RxFifoConfigType *RxFifoConfigPtr);
void Can_SetRxFifo1Config(CAN_T *ModulePtr, const Can_RxFifoConfigType *RxFifoConfigPtr);
void Can_SetRxBufferConfig(CAN_T *ModulePtr, const Can_RxBufConfigType *RxBufConfigPtr);
void Can_SetTxEvtFifoConfig(CAN_T *ModulePtr, const Can_TxFifoConfigType *TxEvtFifoConfigPtr);
void Can_SetTxBufConfig(CAN_T *ModulePtr, const Can_TxBufConfigType *TxBufConfigPtr);
void Can_SetFilterConfig(CAN_T *ModulePtr, const Can_FilterConfigType *FilterConfigPtr);
Can_ApiRetStsType Can_SetMsgRamConfig(CAN_T *ModulePtr, const Can_MessageRamConfig *MsgRamConfigPtr);

void Can_SetStdFilterEle(const Can_FilterConfigType *FilterConfigPtr,
                         const Can_StdFilterEleConfigType *StdFilterEleConfigPtr,
                         uint8_t FilterId);

void Can_SetExtFilterEle(const Can_FilterConfigType *FilterConfigPtr,
                         const Can_ExtFilterEleConfigType *ExtFilterEleConfigPtr,
                         uint8_t FilterId);

uint32_t Can_CheckTransReqPending(const CAN_T *ModulePtr, Can_TxBufferIdType TxBufId);
uint32_t Can_CheckTransOccurred(const CAN_T *ModulePtr, Can_TxBufferIdType TxBufId);

Can_ApiRetStsType Can_WriteTxBuffer(const CAN_T *ModulePtr, Can_TxBufferIdType TxBufId, const Can_TxFrameType *TxFramePtr);
Can_ApiRetStsType Can_ReadRxBuffer(const CAN_T *ModulePtr, Can_RxBufIdType RxBufId, Can_RxFrameType *RxFramePtr);
Can_ApiRetStsType Can_ReadRxFifo(CAN_T *ModulePtr, Can_RxFifoType FifoBlkId, Can_RxFrameType *RxFramePtr);

Can_ApiRetStsType Can_SendBlocking(CAN_T *ModulePtr, Can_TxBufferIdType TxBufId, const Can_TxFrameType *TxFramePtr);
Can_ApiRetStsType Can_ReceiveBlocking(CAN_T *ModulePtr, Can_RxBufIdType RxBufId, Can_RxFrameType *RxFramePtr);
Can_ApiRetStsType Can_ReceiveFifoBlocking(CAN_T *ModulePtr, Can_RxFifoType FifoBlkId, Can_RxFrameType *RxFramePtr);
void Can_CreateHandle(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_RxTxCallbackType Callback, 
    uint8_t *InputParaPtr);
Can_ApiRetStsType Can_SendNonBlocking(CAN_T *ModulePtr, Can_HandleType *HandlePtr, const Can_BufTransInfoType *TxInfoPtr);
Can_ApiRetStsType Can_ReceiveFifoNonBlocking(CAN_T *ModulePtr,
                                             Can_RxFifoType FifoBlkId,
                                             Can_HandleType *HandlePtr,
                                             const Can_RxFifoTransInfoType *RxFifoTranPtr);
Can_ApiRetStsType Can_ReceiveNonBlocking(CAN_T *ModulePtr,
                                         Can_RxBufIdType RxBufId,
                                         Can_HandleType *HandlePtr,
                                         Can_RxFrameType *RxBufFramePtr);
                                         
uint8_t Can_ConvertLenByteToDlc(uint8_t Len);
void Can_FinishDataTransfer(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_TxBufferIdType TxBufId);
void Can_FinishReceiveFifo(CAN_T *ModulePtr, Can_RxFifoType FifoBlkId, Can_HandleType *HandlePtr);
void Can_FinishReceiveBuf(CAN_T *ModulePtr, Can_HandleType *HandlePtr, Can_RxBufIdType RxBufId);
void Can_TransferHandleIsr(CAN_T *ModulePtr, Can_HandleType *HandlePtr);

#if defined(__cplusplus)
}
#endif

#endif /* G32A10xx_CAN_H */

/**@} end of group CAN_Functions */
/**@} end of group CAN_Driver */
/**@} end of group G32A10xx_StdPeriphDriver */
