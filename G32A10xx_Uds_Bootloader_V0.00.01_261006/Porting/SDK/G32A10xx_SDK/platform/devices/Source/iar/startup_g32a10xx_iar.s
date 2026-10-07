;/*!
; * @file       startup_g32a10xx_iar.s
; *
; * @brief      g32a10xx devices vector table for EWARM toolchain.
; *
; * @version    V1.0.0
; *
; * @date       2026-01-19
; *
; * @attention
; *
; *  Copyright (C) 2026 Geehy Semiconductor
; *
; *  You may not use this file except in compliance with the
; *  GEEHY COPYRIGHT NOTICE (Geehy Semiconductor Software License Agreement).
; *
; *  The program is only for reference, which is distributed in the hope
; *  that it will be useful and instructional for customers to develop
; *  their software. Unless required by applicable law or agreed to in
; *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
; *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
; *  See the Geehy Semiconductor Software License Agreement for the governing permissions
; *  and limitations under the License.
; */

        MODULE  ?cstartup

        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler                  ; Reset Handler

        DCD     NMI_Handler                    ; NMI Handler
        DCD     HardFault_Handler              ; Hard Fault Handler
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     SVC_Handler                    ; SVCall Handler
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     PendSV_Handler                 ; PendSV Handler
        DCD     SysTick_Handler                ; SysTick Handler

        ; External Interrupts
        DCD     0                              ; Reserved
        DCD     PVD_IRQHandler                 ; PVD
        DCD     RTC_IRQHandler                 ; RTC through EXTI Line
        DCD     FLASH_IRQHandler               ; FLASH
        DCD     RCM_IRQHandler                 ; RCC
        DCD     EINT0_1_IRQHandler             ; EINT Line 0 and 1
        DCD     EINT2_3_IRQHandler             ; EINT Line 2 and 3
        DCD     EINT4_15_IRQHandler            ; EINT Line 4 to 15
        DCD     SERM_IRQHandler                ; ERP
        DCD     DMA_CH1_IRQHandler             ; DMA1 Channel 1
        DCD     DMA_CH2_3_IRQHandler           ; DMA1 Channel 2 and Channel 3
        DCD     DMA_CH4_5_IRQHandler           ; DMA1 Channel 4 and Channel 5
        DCD     ADC_IRQHandler                 ; ADC1
        DCD     TMR1_BRK_UP_TRG_COM_IRQHandler ; TMR1 Break, Update, Trigger and Commutation
        DCD     TMR1_CC_IRQHandler             ; TMR1 Capture Compare
        DCD     TMR2_IRQHandler                ; Reserved
        DCD     TMR3_IRQHandler                ; TMR3
        DCD     TMR6_IRQHandler                ; TMR6
        DCD     TMR7_IRQHandler                ; TMR7
        DCD     TMR4_IRQHandler                ; TMR4
        DCD     0                              ; Reserved
        DCD     SHA256_IRQHandler              ; SHA256
        DCD     AES256_IRQHandler              ; AES256
        DCD     FDCAN_IT0_IRQHandler           ; FDCAN_IT0
        DCD     FDCAN_IT1_IRQHandler           ; FDCAN_IT1
        DCD     SPI_IRQHandler                 ; SPI1
        DCD     0                              ; Reserved
        DCD     USART1_IRQHandler              ; USART1
        DCD     USART2_IRQHandler              ; USART2
        DCD     RNG_IRQHandler                 ; RNG
        DCD     TMR8_IRQHandler                ; TMR8
        DCD     FDCAN_SERM_IRQHandler          ; FDCAN_SERM

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler

        LDR     R0, =sfe(CSTACK)          ; set stack pointer
        MSR     MSP, R0

        LDR R0,=0x00000004
        LDR R1, [R0]
        LSRS R1, R1, #24
        LDR R2,=0x1F
        CMP R1, R2

        BNE ApplicationStart

        LDR R0,=0x40021018
        LDR R1,=0x00000001
        STR R1, [R0]

        LDR R0,=0x40010000
        LDR R1,=0x00000000
        STR R1, [R0]
ApplicationStart
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK PVD_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
PVD_IRQHandler
        B PVD_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RTC_IRQHandler
        B RTC_IRQHandler

        PUBWEAK FLASH_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FLASH_IRQHandler
        B FLASH_IRQHandler

        PUBWEAK RCM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RCM_IRQHandler
        B RCM_IRQHandler

        PUBWEAK EINT0_1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EINT0_1_IRQHandler
        B EINT0_1_IRQHandler

        PUBWEAK EINT2_3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EINT2_3_IRQHandler
        B EINT2_3_IRQHandler

        PUBWEAK EINT4_15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EINT4_15_IRQHandler
        B EINT4_15_IRQHandler

        PUBWEAK SERM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SERM_IRQHandler
        B SERM_IRQHandler

        PUBWEAK DMA_CH1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA_CH1_IRQHandler
        B DMA_CH1_IRQHandler

        PUBWEAK DMA_CH2_3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA_CH2_3_IRQHandler
        B DMA_CH2_3_IRQHandler

        PUBWEAK DMA_CH4_5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA_CH4_5_IRQHandler
        B DMA_CH4_5_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ADC_IRQHandler
        B ADC_IRQHandler

        PUBWEAK TMR1_BRK_UP_TRG_COM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR1_BRK_UP_TRG_COM_IRQHandler
        B TMR1_BRK_UP_TRG_COM_IRQHandler

        PUBWEAK TMR1_CC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR1_CC_IRQHandler
        B TMR1_CC_IRQHandler

        PUBWEAK TMR2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR2_IRQHandler
        B TMR2_IRQHandler

        PUBWEAK TMR3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR3_IRQHandler
        B TMR3_IRQHandler

        PUBWEAK TMR6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR6_IRQHandler
        B TMR6_IRQHandler

        PUBWEAK TMR7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR7_IRQHandler
        B TMR7_IRQHandler

        PUBWEAK TMR4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR4_IRQHandler
        B TMR4_IRQHandler

        PUBWEAK SHA256_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SHA256_IRQHandler
        B SHA256_IRQHandler

        PUBWEAK AES256_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
AES256_IRQHandler
        B AES256_IRQHandler

        PUBWEAK FDCAN_IT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN_IT0_IRQHandler
        B FDCAN_IT0_IRQHandler

        PUBWEAK FDCAN_IT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN_IT1_IRQHandler
        B FDCAN_IT1_IRQHandler

        PUBWEAK SPI_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI_IRQHandler
        B SPI_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART1_IRQHandler
        B USART1_IRQHandler

        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART2_IRQHandler
        B USART2_IRQHandler

        PUBWEAK RNG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RNG_IRQHandler
        B RNG_IRQHandler

        PUBWEAK TMR8_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TMR8_IRQHandler
        B TMR8_IRQHandler

        PUBWEAK FDCAN_SERM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN_SERM_IRQHandler
        B FDCAN_SERM_IRQHandler

        END
