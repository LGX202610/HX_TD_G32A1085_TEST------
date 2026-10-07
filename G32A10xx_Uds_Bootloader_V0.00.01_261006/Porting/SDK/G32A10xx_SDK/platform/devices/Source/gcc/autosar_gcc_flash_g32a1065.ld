/* G32A10XX GNU ld file */

MEMORY {
    flash_rsvd1_region (rx)    : ORIGIN = 0x08000000, LENGTH = 0x08
    flash_rsvd2_region (rx)    : ORIGIN = 0x08000400, LENGTH = 0x10
    flash_rsvd_fls_region (rx) : ORIGIN = 0x08000410, LENGTH = 0x60
    flash_memory_region (rx)   : ORIGIN = 0x08000530, LENGTH = 0x1FFFF - 0x530

    iram_intvec_region (rwx)   : ORIGIN = 0x20000000, LENGTH = 0x20000200 - 0x20000000
    iram_main_region (rwx)     : ORIGIN = 0x20000200, LENGTH = 0x20003B60 - 0x20000200
    iram_rsvd_fls_region (rw)  : ORIGIN = 0x20003B60, LENGTH = 0x60
    iram_results_region (rw)   : ORIGIN = 0x20003BC0, LENGTH = 0x40
    iram_stack_region (rw)     : ORIGIN = 0x20003C00, LENGTH = 0x20004000 - 0x20003C00
}

ENTRY(Reset_Handler)

__STACK_SIZE = DEFINED(__STACK_SIZE) ? __STACK_SIZE : 1K;

SECTIONS {
    AppRCW : {
        KEEP(*(.rcw));
    } > flash_rsvd1_region

    AppFlashConfig : {
        KEEP(*(.flash_config));
    } > flash_rsvd2_region
    
    AppACFLSCode : {
        KEEP(*(.acfls_code_rom));
    } > flash_rsvd_fls_region
    
    AppCode : {
        *(.mcal_text*);
        *(.text*);
        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        /* section information for initial. */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
    } > flash_memory_region
    
    AppROData : {
    *(.mcal_const_cfg*);
    *(.mcal_const*);
    *(.rodata*);
    . = ALIGN(4);
    } > flash_memory_region
    
    /* Intrrupt table in flash */
    AppIntVec_Init : {
        KEEP(*(.intc_vector_init))
    } > flash_memory_region
    _flashivectors = LOADADDR(AppIntVec_Init); /* The address of the intrrupt table in flash */
    _endivectors = _flashivectors + SIZEOF(AppIntVec); /* The end address of the intrrupt table in flash */

    /* Intrrupt table in ram when running */
    AppIntVec : {
        _svectors = .;          /* Start address in RAM */
        KEEP(*(.intc_vector));
        _evectors = .;          /* End address in RAM */
    } > iram_intvec_region AT > flash_memory_region
    _ramivectors = ADDR(AppIntVec); /* Start address in RAM */

    AppStartup : {
        KEEP(*(.startup));
        KEEP(*(.systeminit));
        KEEP(*(.text.Reset_Handler));
    } > flash_memory_region
    
    AppData : ALIGN(4){
        _ramappdata = .;          
        KEEP(*(.mcal_data_init))  
        KEEP(*(.data_init))
        *(.mcal_data*);
        *(.data*);
        . = ALIGN(4);
        _edata = .;
    } > iram_main_region AT > flash_memory_region
     _flashappdata = LOADADDR(AppData);

    AppRAMCode_Init : {
        KEEP(*(.ramcode_init))
    } > flash_memory_region
    _flashramcode = LOADADDR(AppRAMCode_Init);
    _endramcode = _flashramcode + SIZEOF(AppRAMCode);

    AppRAMCode : {
        _sramcode = .;
        *(.ramcode*);
        . = ALIGN(4);
        _eramcode = .;
    } > iram_main_region AT > flash_memory_region
    _ramcode = ADDR(AppRAMCode);

    AppBSS_NO_CACHE (NOLOAD) : {
        _sbss_nc = .;
        *(.mcal_bss_no_cacheable*);
        . = ALIGN(4);
        _ebss_nc = .;
    } > iram_main_region

    AppData_NO_CACHE_Init : {
        _sidata_nc = .;
        KEEP(*(.mcal_data_no_cacheable_init));
    } > flash_memory_region
    _flashappdatanocache = LOADADDR(AppData_NO_CACHE_Init);
    _endappdatanocache = _flashappdatanocache + SIZEOF(AppData_NO_CACHE);

    AppData_NO_CACHE : {
        _sdata_nc = .;
        *(.mcal_data_no_cacheable*);
        . = ALIGN(4);
        _edata_nc = .;
    } > iram_main_region AT > flash_memory_region
    _ramappdatanocache = ADDR(AppData_NO_CACHE);

    AppBSS (NOLOAD) : {
        _sbss = .;
        __bss_start__ = .;
        *(.mcal_bss*);
        *(.bss*);
        *(COMMON);
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = .;
        __bss_end = .;
    } > iram_main_region

    AppSTACK (NOLOAD) : {
        . = ALIGN(4);
        . += __STACK_SIZE;
        *(.stack_main*);
        . = ALIGN(4);
        _estack = .;
    } > iram_stack_region

    .heap (NOLOAD) : {
        . = ALIGN(8);
        end = .;
        _end = .;
    } > iram_main_region
}

_Stack_end = ORIGIN(iram_stack_region);
_Stack_start = ORIGIN(iram_stack_region) + LENGTH(iram_stack_region);

INT_SRAM_START = ORIGIN(iram_main_region);
INT_SRAM_SIZE = (LENGTH(iram_main_region) + 
               LENGTH(iram_stack_region) + 
               LENGTH(iram_results_region) + 
               LENGTH(iram_rsvd_fls_region) + 3) / 4;
INT_SRAM_END = ORIGIN(iram_main_region) + 
              LENGTH(iram_main_region) + 
              LENGTH(iram_stack_region) + 
              LENGTH(iram_results_region) + 
              LENGTH(iram_rsvd_fls_region);

Fls_ACEraseRomStart = ORIGIN(flash_rsvd_fls_region);
Fls_ACEraseRomEnd = ORIGIN(flash_rsvd_fls_region) + LENGTH(flash_rsvd_fls_region);
Fls_ACEraseSize = (LENGTH(flash_rsvd_fls_region) + 3) / 4;

Fls_ACWriteRomStart = ORIGIN(flash_rsvd_fls_region);
Fls_ACWriteRomEnd = ORIGIN(flash_rsvd_fls_region) + LENGTH(flash_rsvd_fls_region);
Fls_ACWriteSize = (LENGTH(flash_rsvd_fls_region) + 3) / 4;

_ERASE_FUNC_ADDRESS_ = ORIGIN(iram_rsvd_fls_region);
_WRITE_FUNC_ADDRESS_ = ORIGIN(iram_rsvd_fls_region);

PROVIDE(_sbss_no_cache = _sbss_nc);
PROVIDE(_ebss_no_cache = _ebss_nc);
PROVIDE(_sdata_no_cache = _sdata_nc);
PROVIDE(_edata_no_cache = _edata_nc);
PROVIDE(__bss_start__ = _sbss);
PROVIDE(__bss_end__ = _ebss);

ASSERT(SIZEOF(AppBSS_NO_CACHE) <= (LENGTH(iram_main_region) - (_ebss - _sbss)), 
       "Error: AppBSS_NO_CACHE exceeds available RAM");
