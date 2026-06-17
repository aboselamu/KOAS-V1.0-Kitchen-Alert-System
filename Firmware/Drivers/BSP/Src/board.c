#include "board.h"

/*
 * Board Support Package — STM32F103C8T6 Blue Pill
 *
 * This is the ONLY file that knows which pins and clocks to use.
 * Drivers call MspInit hooks — they never configure pins directly.
 *
 * F4 vs F1 key differences:
 *   GPIO clock : AHB1ENR (F4)  → APB2ENR (F1)
 *   Input mode : MODER+PUPDR   → CRL/CRH (0x4 = floating input)
 *   AF output  : MODER+AFR     → CRL/CRH (0xA = AF push-pull 10MHz)
 *   EXTI map   : SYSCFG->EXTICR → AFIO->EXTICR (needs AFIO clock bit 0)
 */

/*===========================================================================
 * EXTI MSP Init
 * Called by EXTI_Init() for each handle before configuring EXTI registers
 *==========================================================================*/
void EXTI_MspInit(EXTI_Handle_t *pHandle)
{
    /* 1. AFIO clock — required for EXTICR port-to-line mapping */
    RCC->APB2ENR |= (1U << 0);

    /* 2. GPIO clock for target port */
    if      (pHandle->pPort == GPIOA) RCC->APB2ENR |= (1U << 2);
    else if (pHandle->pPort == GPIOB) RCC->APB2ENR |= (1U << 3);
    else if (pHandle->pPort == GPIOC) RCC->APB2ENR |= (1U << 4);

    /* 3. Configure pin as floating input: CNF=01 MODE=00 → 0x4
     * Lines 0-7  use CRL, bit offset = line * 4
     * Lines 8-15 use CRH, bit offset = (line - 8) * 4            */
    if (pHandle->line < 8U)
    {
        uint32_t shift = pHandle->line * 4U;
        pHandle->pPort->CRL &= ~(0xFU << shift);
        pHandle->pPort->CRL |=  (0x4U << shift);
    }
    else
    {
        uint32_t shift = (pHandle->line - 8U) * 4U;
        pHandle->pPort->CRH &= ~(0xFU << shift);
        pHandle->pPort->CRH |=  (0x4U << shift);
    }

    /* 4. NVIC — lines 0-4 each have a dedicated IRQ vector */
    if (pHandle->line <= 4U)
    {
        NVIC_SetPriority((IRQn_Type)(EXTI0_IRQn + pHandle->line), 1U);
        NVIC_EnableIRQ((IRQn_Type)(EXTI0_IRQn + pHandle->line));
    }
    /* Lines 5-9 share EXTI9_5_IRQn — add here if needed in future */
}

/*===========================================================================
 * PWM MSP Init
 * Called by PWM_Init() before configuring timer registers
 *==========================================================================*/
void PWM_MspInit(PWM_HandleTypeDef *hpwm)
{
    if (hpwm->Instance == TIM2)
    {
        /* 1. TIM2 clock on APB1 */
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

        /* 2. GPIOA clock on APB2 */
        RCC->APB2ENR |= (1U << 2);

        /* 3. PA2: Alternate Function push-pull 10MHz
         * CRL bits [11:8] — pin 2 offset = 2 * 4 = 8
         * CNF=10 MODE=10 → 0xA                        */
        GPIOA->CRL &= ~(0xFU << 8U);
        GPIOA->CRL |=  (0xAU << 8U);
    }
}
