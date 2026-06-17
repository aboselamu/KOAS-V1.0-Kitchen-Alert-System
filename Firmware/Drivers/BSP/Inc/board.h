#ifndef BOARD_H
#define BOARD_H

#include "stm32f1xx.h"
#include "exti_driver.h"
#include "pwm_driver.h"

/* MSP implementations — called by drivers during Init */
void EXTI_MspInit(EXTI_Handle_t *pHandle);
void PWM_MspInit(PWM_HandleTypeDef *hpwm);

#endif /* BOARD_H */
