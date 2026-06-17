#include "led_driver.h"
#include "stm32f1xx.h"

/* PC13 active-low: LOW = ON, HIGH = OFF */

void LED_On(void)
{
    GPIOC->ODR &= ~(1U << 13);
}

void LED_Off(void)
{
    GPIOC->ODR |=  (1U << 13);
}

void LED_Toggle(void)
{
    GPIOC->ODR ^=  (1U << 13);
}
