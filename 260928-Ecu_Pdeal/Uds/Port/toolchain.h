#ifndef TOOLCHAIN_H_
#define TOOLCHAIN_H_

#if defined __IAR_SYSTEMS_ICC__
    #define INLINE              inline
    #define INTERRUPT_FUNC      __interrupt
    #define ASM_KEYWORD         __asm volatile
    #define PACKED_STRUCT_END
    #define PACKED_STRUCT_FIELD(x)  x
    #define PACKED_STRUCT_BEGIN     __packed
    #define STRINGIZE(X) #X
    #define ALIGNEDXB(n)            _Pragma(STRINGIZE(data_alignment=n))

#elif defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__clang__)
    #define ASM_KEYWORD         __asm
    #define PACKED_STRUCT_END   __attribute__((packed))
    #define PACKED_STRUCT_FIELD(x) x __attribute__((packed))
    #define PACKED_STRUCT_BEGIN
    #define ALIGNEDXB(x)        __attribute__((aligned(x)))

#else
    #error "Unknown toolchain"
#endif

#endif
