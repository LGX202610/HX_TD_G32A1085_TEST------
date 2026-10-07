;/*!
; * @file       startup_g32a10xx_mdk.s
; *
; * @brief      CMSIS Cortex-M0 PLUS based Core Device Startup File for Device startup_g32a10xx
; *
; * @version    V1.0.0
; *
; * @date       2024-07-31
; *
; * @attention
; *
; *  Copyright (C) 2024 Geehy Semiconductor
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

; <h> Stack Configuration
;  <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000800

                AREA    |.stack_main|, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                   ; Top of Stack
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
                DCD     RTC_IRQHandler                 ; RTC through EINT Line
                DCD     FLASH_IRQHandler               ; FLASH
                DCD     RCM_IRQHandler                 ; RCM
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
                DCD     TMR2_IRQHandler                ; TMR2
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

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler routine
Reset_Handler   PROC
                EXPORT  Reset_Handler                 [WEAK]
                IMPORT  __main
                IMPORT  SystemInit
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                    [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler              [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                    [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                 [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  PVD_IRQHandler                 [WEAK]
                EXPORT  RTC_IRQHandler                 [WEAK]
                EXPORT  FLASH_IRQHandler               [WEAK]
                EXPORT  RCM_IRQHandler                 [WEAK]
                EXPORT  EINT0_1_IRQHandler             [WEAK]
                EXPORT  EINT2_3_IRQHandler             [WEAK]
                EXPORT  EINT4_15_IRQHandler            [WEAK]
                EXPORT  SERM_IRQHandler                [WEAK]
                EXPORT  DMA_CH1_IRQHandler             [WEAK]
                EXPORT  DMA_CH2_3_IRQHandler           [WEAK]
                EXPORT  DMA_CH4_5_IRQHandler           [WEAK]
                EXPORT  ADC_IRQHandler                 [WEAK]
                EXPORT  TMR1_BRK_UP_TRG_COM_IRQHandler [WEAK]
                EXPORT  TMR1_CC_IRQHandler             [WEAK]
                EXPORT  TMR2_IRQHandler                [WEAK]
                EXPORT  TMR3_IRQHandler                [WEAK]
                EXPORT  TMR6_IRQHandler                [WEAK]
                EXPORT  TMR7_IRQHandler                [WEAK]
                EXPORT  TMR4_IRQHandler                [WEAK]
                EXPORT  SHA256_IRQHandler              [WEAK]
                EXPORT  AES256_IRQHandler              [WEAK]
                EXPORT  FDCAN_IT0_IRQHandler           [WEAK]
                EXPORT  FDCAN_IT1_IRQHandler           [WEAK]
                EXPORT  SPI_IRQHandler                 [WEAK]
                EXPORT  USART1_IRQHandler              [WEAK]
                EXPORT  USART2_IRQHandler              [WEAK]
                EXPORT  RNG_IRQHandler                 [WEAK]
                EXPORT  TMR8_IRQHandler                [WEAK]
                EXPORT  FDCAN_SERM_IRQHandler          [WEAK]


PVD_IRQHandler
RTC_IRQHandler
FLASH_IRQHandler
RCM_IRQHandler
EINT0_1_IRQHandler
EINT2_3_IRQHandler
EINT4_15_IRQHandler
SERM_IRQHandler
DMA_CH1_IRQHandler
DMA_CH2_3_IRQHandler
DMA_CH4_5_IRQHandler
ADC_IRQHandler
TMR1_BRK_UP_TRG_COM_IRQHandler
TMR1_CC_IRQHandler
TMR2_IRQHandler
TMR3_IRQHandler
TMR6_IRQHandler
TMR7_IRQHandler
TMR4_IRQHandler
SHA256_IRQHandler
AES256_IRQHandler
FDCAN_IT0_IRQHandler
FDCAN_IT1_IRQHandler
SPI_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
RNG_IRQHandler
TMR8_IRQHandler
FDCAN_SERM_IRQHandler

                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END
