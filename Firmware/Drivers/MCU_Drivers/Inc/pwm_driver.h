#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include "stm32f1xx.h"   /* F1 header — replaces stm32f4xx.h */
#include <stdint.h>
#include <stddef.h>

/* Status codes */
typedef enum {
    PWM_STATUS_OK    = 0x00,
    PWM_STATUS_ERROR = 0x01
} PWM_Status_t;

/* Configuration */
typedef struct {
    uint32_t prescaler;   /* PSC register value                  */
    uint32_t period;      /* ARR register value                  */
    uint8_t  polarity;    /* 0 = Active High, 1 = Active Low     */
} PWM_Config_t;

/* Handle */
typedef struct {
    TIM_TypeDef  *Instance;   /* TIM2, TIM3, etc.  */
    uint8_t       channel;    /* Channel 1–4        */
    PWM_Config_t  Config;
} PWM_HandleTypeDef;

/* API */
PWM_Status_t PWM_Init(PWM_HandleTypeDef *hpwm);
PWM_Status_t PWM_SetDuty(PWM_HandleTypeDef *hpwm, uint32_t duty);

/* MSP Hook — implemented in board.c */
void PWM_MspInit(PWM_HandleTypeDef *hpwm);

#endif /* PWM_DRIVER_H */
