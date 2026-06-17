#ifndef EXTI_DRIVER_H
#define EXTI_DRIVER_H

#include <stdint.h>
#include "stm32f1xx.h"

/* Trigger options */
#define EXTI_TRIGGER_RISING   0
#define EXTI_TRIGGER_FALLING  1
#define EXTI_TRIGGER_BOTH     2

/* Status codes */
typedef enum {
    EXTI_STATUS_OK    = 0x00,
    EXTI_STATUS_ERROR = 0x01
} EXTI_Status_t;

/* Handle */
typedef struct {
    uint8_t        line;                           /* EXTI line 0-15          */
    uint8_t        trigger;                        /* RISING, FALLING, BOTH   */
    GPIO_TypeDef  *pPort;                          /* GPIOA, GPIOB, etc.      */
    void         (*pApplicationCallback)(uint8_t); /* Application callback    */
} EXTI_Handle_t;

/* API */
EXTI_Status_t EXTI_Init(EXTI_Handle_t *pHandle);
void          EXTI_IRQHandling(uint8_t line);

/* MSP Hook — implemented in board.c */
void EXTI_MspInit(EXTI_Handle_t *pHandle);

#endif /* EXTI_DRIVER_H */
