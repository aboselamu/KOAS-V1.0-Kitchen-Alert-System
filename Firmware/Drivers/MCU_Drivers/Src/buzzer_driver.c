#include "buzzer_driver.h"
#include "stm32f1xx.h"

/*
 * Buzzer driven by TIM2 CH3 PWM on PA2.
 * Timer runs continuously — CC3E bit connects/disconnects output.
 * No LED, no timing, no FSM knowledge here.
 */

void Buzzer_On(void)
{
    TIM2->CCER |=  (0x1U << 8);
}

void Buzzer_Off(void)
{
    TIM2->CCER &= ~(0x1U << 8);
}

void Buzzer_Toggle(void)
{
    TIM2->CCER ^=  (0x1U << 8);
}
