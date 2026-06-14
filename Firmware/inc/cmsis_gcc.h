#ifndef __CMSIS_GCC_H
#define __CMSIS_GCC_H

/* Ignore semi-colon missing warnings in inline assembly macros */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"

#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((noreturn))
#endif
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED                               __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#endif

/* Core Register Access Functions */
__STATIC_INLINE uint32_t __get_CONTROL(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}

__STATIC_INLINE void __set_CONTROL(uint32_t control) {
  __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
}

__STATIC_INLINE uint32_t __get_IPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}

__STATIC_INLINE uint32_t __get_APSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}

__STATIC_INLINE uint32_t __get_xPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}

__STATIC_INLINE uint32_t __get_PSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, psp" : "=r" (result) );
  return(result);
}

__STATIC_INLINE void __set_PSP(uint32_t topOfProcStack) {
  __ASM volatile ("MSR psp, %0" : : "r" (topOfProcStack) : "memory");
}

__STATIC_INLINE uint32_t __get_MSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, msp" : "=r" (result) );
  return(result);
}

__STATIC_INLINE void __set_MSP(uint32_t topOfMainStack) {
  __ASM volatile ("MSR msp, %0" : : "r" (topOfMainStack) : "memory");
}

__STATIC_INLINE uint32_t __get_PRIMASK(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}

__STATIC_INLINE void __set_PRIMASK(uint32_t priMask) {
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

__STATIC_INLINE void __enable_irq(void) {
  __ASM volatile ("cpsie i" : : : "memory");
}

__STATIC_INLINE void __disable_irq(void) {
  __ASM volatile ("cpsid i" : : : "memory");
}

/* Instruction Barriers & Intrinsics */
__STATIC_INLINE void __NOP(void) { __ASM volatile ("nop"); }
__STATIC_INLINE void __WFI(void) { __ASM volatile ("wfi" : : : "memory"); }
__STATIC_INLINE void __WFE(void) { __ASM volatile ("wfe" : : : "memory"); }
__STATIC_INLINE void __SEV(void) { __ASM volatile ("sev"); }

__STATIC_INLINE void __ISB(void) { __ASM volatile ("isb 0xF":::"memory"); }
__STATIC_INLINE void __DSB(void) { __ASM volatile ("dsb 0xF":::"memory"); }
__STATIC_INLINE void __DMB(void) { __ASM volatile ("dmb 0xF":::"memory"); }

__STATIC_INLINE uint32_t __REV(uint32_t value) {
  return __builtin_bswap32(value);
}

__STATIC_INLINE uint32_t __REV16(uint32_t value) {
  uint32_t result;
  __ASM volatile ("rev16 %0, %1" : "=r" (result) : "r" (value) );
  return result;
}

__STATIC_INLINE int32_t __REVSH(int32_t value) {
  return (int16_t)__builtin_bswap16((uint16_t)value);
}

__STATIC_INLINE uint32_t __RBIT(uint32_t value) {
  uint32_t result;
  __ASM volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
  return result;
}

__STATIC_INLINE uint8_t __CLZ(uint32_t value) {
  if (value == 0U) return 32U;
  return (uint8_t)__builtin_clz(value);
}

#pragma GCC diagnostic pop
#endif /* __CMSIS_GCC_H */