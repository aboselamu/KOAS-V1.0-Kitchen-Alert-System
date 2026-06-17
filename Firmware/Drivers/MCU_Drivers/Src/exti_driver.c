#include <stdint.h>
#include <stddef.h>
#include "exti_driver.h"
#include "board.h"

/* Private handle storage — used by ISR to find the right callback */
static EXTI_Handle_t *gpEXTI_Handles[16];

EXTI_Status_t EXTI_Init(EXTI_Handle_t *pHandle)
{
    /* Defensive validation */
    if (pHandle == NULL)        return EXTI_STATUS_ERROR;
    if (pHandle->line > 15)     return EXTI_STATUS_ERROR;

    /* Guard: prevent double-init on same line */
    if (gpEXTI_Handles[pHandle->line] != NULL)
        return EXTI_STATUS_ERROR;

    /* 1. Board-specific clock and pin setup */
    EXTI_MspInit(pHandle);

    /* 2. Store handle for ISR */
    gpEXTI_Handles[pHandle->line] = pHandle;

    /* 3. Map GPIO port to EXTI line via AFIO (F1 uses AFIO, not SYSCFG) */
    uint8_t reg_idx   = pHandle->line / 4U;
    uint8_t shift     = (pHandle->line % 4U) * 4U;
    uint8_t port_code = (uint8_t)(((uint32_t)pHandle->pPort - GPIOA_BASE) / 0x400U);

    AFIO->EXTICR[reg_idx] &= ~(0xFU << shift);
    AFIO->EXTICR[reg_idx] |=  (port_code << shift);

    /* 4. Clear both edge triggers before setting */
    EXTI->RTSR &= ~(1U << pHandle->line);
    EXTI->FTSR &= ~(1U << pHandle->line);

    if (pHandle->trigger == EXTI_TRIGGER_RISING ||
        pHandle->trigger == EXTI_TRIGGER_BOTH)
        EXTI->RTSR |= (1U << pHandle->line);

    if (pHandle->trigger == EXTI_TRIGGER_FALLING ||
        pHandle->trigger == EXTI_TRIGGER_BOTH)
        EXTI->FTSR |= (1U << pHandle->line);

    /* 5. Unmask interrupt line */
    EXTI->IMR |= (1U << pHandle->line);

    return EXTI_STATUS_OK;
}

void EXTI_IRQHandling(uint8_t line)
{
    if (EXTI->PR & (1U << line))
    {
        /* Clear pending flag — write 1 directly, never use |= */
        EXTI->PR = (1U << line);

        /* Barrier: ensure flag clear completes before callback runs */
        __DSB();

        /* Fire application callback */
        if (gpEXTI_Handles[line] != NULL &&
            gpEXTI_Handles[line]->pApplicationCallback != NULL)
        {
            gpEXTI_Handles[line]->pApplicationCallback(line);
        }
    }
}
