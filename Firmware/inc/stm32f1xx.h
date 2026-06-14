/**
  ******************************************************************************
  * @file    stm32f1xx.h
  * @brief   CMSIS STM32F1xx Device Peripheral Access Layer Header File.
  ******************************************************************************
  */

#ifndef __STM32F1xx_H
#define __STM32F1xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#if !defined (STM32F1)
#define STM32F1
#endif /* STM32F1 */

/* The Makefile defines -DSTM32F103xB, which will activate this block */
#if defined(STM32F103xB)
  #include "stm32f103xb.h"
#else
  #error "Please select first the target STM32F1xx device used in your application"
#endif

typedef enum
{
  RESET = 0U,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0U,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  SUCCESS = 0U,
  ERROR = !SUCCESS
} ErrorStatus;

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F1xx_H */