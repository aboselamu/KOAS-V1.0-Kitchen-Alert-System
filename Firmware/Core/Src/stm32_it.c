#include "stm32_it.h"
#include "exti_driver.h"

/*
 * Interrupt Service Routines
 * Zero application logic here — pure delegation to driver layer.
 * Flow: Hardware IRQ -> here -> EXTI_IRQHandling() -> callback -> FSM
 */

void EXTI0_IRQHandler(void)
{
    EXTI_IRQHandling(0);   /* PA0 — Silence button */
}

void EXTI1_IRQHandler(void)
{
    EXTI_IRQHandling(1);   /* PA1 — IR beam sensor */
}

void EXTI9_5_IRQHandler(void)
{
    for (int i = 5; i <= 9; i++)
        EXTI_IRQHandling(i);
}
