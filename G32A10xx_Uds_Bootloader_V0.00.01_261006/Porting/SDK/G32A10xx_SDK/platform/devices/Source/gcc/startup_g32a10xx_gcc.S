.syntax unified
.cpu cortex-m0plus
.thumb

/* ----------------------------------------------------------------------------
   外部符号声明 (必须与 ld 文件中的符号完全一致)
   ---------------------------------------------------------------------------- */
.extern _estack
.extern _flashivectors, _ramivectors, _evectors          /* 向量表 */
.extern _flashappdata, _ramappdata, _edata                /* Data段 */
.extern _flashramcode, _ramcode, _eramcode                /* RAM代码 */
.extern _flashappdatanocache, _ramappdatanocache, _edata_nc /* 非缓存Data */
.extern _sbss, _ebss                                      /* BSS */
.extern _sbss_nc, _ebss_nc                                /* 非缓存BSS */
.extern SystemInit
.extern main
.type main, %function  

/* ----------------------------------------------------------------------------
   1. 启动配置 (RCW / Boot Header)
   必须链接到 Flash 的起始地址 (通常是 0x08000000)
   ---------------------------------------------------------------------------- */
.section .rcw, "a", %progbits
.align 2
    .word _estack            /* 0x00: 初始堆栈指针 */
    .word Reset_Handler      /* 0x04: 复位入口地址 */

/* ----------------------------------------------------------------------------
   2. 运行时的向量表 (VTABLE)
   该部分会被搬运到 RAM (_ramivectors) 中使用
   ---------------------------------------------------------------------------- */
.section .intc_vector, "a", %progbits
.align 4
.global VTABLE
VTABLE:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word 0, 0, 0, 0, 0, 0, 0 /* Reserved */
    .word SVC_Handler
    .word 0, 0                /* Reserved */
    .word PendSV_Handler
    .word SysTick_Handler

    /* 外部中断 (External Interrupts) */
    .word 0                                 /* IRQ0: Reserved */
    .word PVD_IRQHandler                    /* IRQ1: PVD */
    .word RTC_IRQHandler                    /* IRQ2: RTC */
    .word FLASH_IRQHandler                  /* IRQ3: FLASH */
    .word RCM_IRQHandler                    /* IRQ4: RCM */
    .word EINT0_1_IRQHandler                /* IRQ5: EINT 0-1 */
    .word EINT2_3_IRQHandler                /* IRQ6: EINT 2-3 */
    .word EINT4_15_IRQHandler               /* IRQ7: EINT 4-15 */
    .word SERM_IRQHandler                   /* IRQ8: ERP */
    .word DMA_CH1_IRQHandler                /* IRQ9: DMA Channel 1 */
    .word DMA_CH2_3_IRQHandler              /* IRQ10: DMA Channel 2-3 */
    .word DMA_CH4_5_IRQHandler              /* IRQ11: DMA Channel 4-5 */
    .word ADC_IRQHandler                    /* IRQ12: ADC1 */
    .word TMR1_BRK_UP_TRG_COM_IRQHandler    /* IRQ13: TMR1 Break/Up/Trg/Com */
    .word TMR1_CC_IRQHandler                /* IRQ14: TMR1 Capture Compare */
    .word TMR2_IRQHandler                   /* IRQ15: TMR2 */
    .word TMR3_IRQHandler                   /* IRQ16: TMR3 */
    .word TMR6_IRQHandler                   /* IRQ17: TMR6 */
    .word TMR7_IRQHandler                   /* IRQ18: TMR7 */
    .word TMR4_IRQHandler                   /* IRQ19: TMR4 */
    .word 0                                 /* IRQ20: Reserved */
    .word SHA256_IRQHandler                 /* IRQ21: SHA256 */
    .word AES256_IRQHandler                 /* IRQ22: AES256 */
    .word FDCAN_IT0_IRQHandler              /* IRQ23: FDCAN_IT0 */
    .word FDCAN_IT1_IRQHandler              /* IRQ24: FDCAN_IT1 */
    .word SPI_IRQHandler                    /* IRQ25: SPI1 */
    .word 0                                 /* IRQ26: Reserved */
    .word USART1_IRQHandler                 /* IRQ27: USART1 */
    .word USART2_IRQHandler                 /* IRQ28: USART2 */
    .word RNG_IRQHandler                    /* IRQ29: RNG */
    .word TMR8_IRQHandler                   /* IRQ30: TMR8 */
    .word FDCAN_SERM_IRQHandler             /* IRQ31: FDCAN_SERM */

/* ----------------------------------------------------------------------------
   3. 复位处理程序 (Reset_Handler)
   ---------------------------------------------------------------------------- */
.section .text.Reset_Handler, "ax", %progbits
.global Reset_Handler
.thumb_func
.type Reset_Handler, %function

Reset_Handler:
    /* 1. 屏蔽中断，确保启动过程不被干扰 */
    cpsid i

    /* 2. 初始化堆栈指针 (虽然硬件自动加载，但手动加载更安全) */
    ldr   r0, =_estack
    mov   sp, r0

    /* 3. 调用基础硬件初始化 (如关闭看门狗、切时钟) */
    ldr   r0, =SystemInit
    blx   r0

    /* 4. 搬运向量表 (Flash -> RAM 0x20000000) */
    ldr   r0, =_flashivectors
    ldr   r1, =_ramivectors
    ldr   r2, =_evectors
    bl    .L_copy_loop

    /* 5. 设置 VTOR 重定向向量表 
       G32A10xx (Cortex-M0+) 的 VTOR 寄存器地址为 0xE000ED08
    */
    ldr   r0, =0xE000ED08
    ldr   r1, =_ramivectors
    str   r1, [r0]

    /* 6. 搬运 Data 段 (已初始化的变量) */
    ldr   r0, =_flashappdata
    ldr   r1, =_ramappdata
    ldr   r2, =_edata
    bl    .L_copy_loop

    /* 7. 搬运 RAM Code 段 (在 RAM 中运行的函数) */
    ldr   r0, =_flashramcode
    ldr   r1, =_ramcode
    ldr   r2, =_eramcode
    bl    .L_copy_loop

    /* 8. 搬运 Non-Cacheable Data 段 */
    ldr   r0, =_flashappdatanocache
    ldr   r1, =_ramappdatanocache
    ldr   r2, =_edata_nc
    bl    .L_copy_loop

    /* 9. 清零 BSS 段 (未初始化的变量) */
    ldr   r1, =_sbss
    ldr   r2, =_ebss
    bl    .L_zero_loop

    /* 10. 清零 Non-Cacheable BSS 段 */
    ldr   r1, =_sbss_nc
    ldr   r2, =_ebss_nc
    bl    .L_zero_loop
    
    /* 11. 恢复中断，进入 main */
    cpsie i
#ifdef USING_RT_THREAD
    ldr   r0, =entry
#else
    ldr   r0, =main
#endif
    bx    r0

/* ----------------------------------------------------------------------------
   辅助子程序 (针对 M0+ 优化)
   ---------------------------------------------------------------------------- */
.thumb_func
.L_copy_loop:
    cmp   r1, r2
    bhs   .L_done
    ldr   r3, [r0]
    str   r3, [r1]
    adds  r0, r0, #4
    adds  r1, r1, #4
    b     .L_copy_loop

.thumb_func
.L_zero_loop:
    movs  r3, #0
.L_zero_inner:
    cmp   r1, r2
    bhs   .L_done
    str   r3, [r1]
    adds  r1, r1, #4
    b     .L_zero_inner

.L_done:
    bx    lr

.size Reset_Handler, . - Reset_Handler

/* ----------------------------------------------------------------------------
   4. 默认中断处理程序 (Default_Handler)
   ---------------------------------------------------------------------------- */
.section .text.Default_Handler, "ax", %progbits
.thumb_func
.type Default_Handler, %function
Default_Handler:
    b .      /* 死循环，方便调试时发现未定义的中断 */
.size Default_Handler, . - Default_Handler
/* 定义弱符号宏 */
.macro IRQ_DEF name
    .weak \name
    .thumb_set \name, Default_Handler
.endm

/* 系统异常 */
IRQ_DEF NMI_Handler
IRQ_DEF HardFault_Handler
IRQ_DEF SVC_Handler
IRQ_DEF PendSV_Handler
IRQ_DEF SysTick_Handler

/* 外部中断 */
IRQ_DEF PVD_IRQHandler
IRQ_DEF RTC_IRQHandler
IRQ_DEF FLASH_IRQHandler
IRQ_DEF RCM_IRQHandler
IRQ_DEF EINT0_1_IRQHandler
IRQ_DEF EINT2_3_IRQHandler
IRQ_DEF EINT4_15_IRQHandler
IRQ_DEF SERM_IRQHandler
IRQ_DEF DMA_CH1_IRQHandler
IRQ_DEF DMA_CH2_3_IRQHandler
IRQ_DEF DMA_CH4_5_IRQHandler
IRQ_DEF ADC_IRQHandler
IRQ_DEF TMR1_BRK_UP_TRG_COM_IRQHandler
IRQ_DEF TMR1_CC_IRQHandler
IRQ_DEF TMR2_IRQHandler
IRQ_DEF TMR3_IRQHandler
IRQ_DEF TMR6_IRQHandler
IRQ_DEF TMR7_IRQHandler
IRQ_DEF TMR4_IRQHandler
IRQ_DEF SHA256_IRQHandler
IRQ_DEF AES256_IRQHandler
IRQ_DEF FDCAN_IT0_IRQHandler
IRQ_DEF FDCAN_IT1_IRQHandler
IRQ_DEF SPI_IRQHandler
IRQ_DEF USART1_IRQHandler
IRQ_DEF USART2_IRQHandler
IRQ_DEF RNG_IRQHandler
IRQ_DEF TMR8_IRQHandler
IRQ_DEF FDCAN_SERM_IRQHandler

