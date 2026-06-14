#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

#include <stdint.h>

/* ARM Compiler 4/5 */
#if   defined ( __CC_ARM )
  #include "cmsis_armcc.h"

/* ARM Compiler 6 (Arm Clang) */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #include "cmsis_armclang.h"

/* GNU Compiler (gcc) */
#elif defined ( __GNUC__ )
  #include "cmsis_gcc.h"

/* IAR Compiler */
#elif defined ( __ICCARM__ )
  #include "cmsis_iccarm.h"

/* TI Arm Compiler */
#elif defined ( __TI_ARM__ )
  #include "cmsis_ccs.h"

/* TASKING Compiler */
#elif defined ( __TASKING__ )
  #include "cmsis_tasking.h"

/* COSMIC Compiler */
#elif defined ( __CSMC__ )
  #include "cmsis_cosmic.h"

#else
  #error Unknown compiler.
#endif

#endif /* __CMSIS_COMPILER_H */