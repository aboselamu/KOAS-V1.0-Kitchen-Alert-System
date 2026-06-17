#include "pwm_driver.h"

/* Default weak MspInit — overridden by board.c */
__attribute__((weak)) void PWM_MspInit(PWM_HandleTypeDef *hpwm)
{
    (void)hpwm;
}

PWM_Status_t PWM_Init(PWM_HandleTypeDef *hpwm)
{
    if (hpwm == NULL || hpwm->Instance == NULL) return PWM_STATUS_ERROR;
    if (hpwm->channel < 1 || hpwm->channel > 4) return PWM_STATUS_ERROR;

    /* Board-level clock and pin setup */
    PWM_MspInit(hpwm);

    /* Configure time base */
    hpwm->Instance->PSC  = hpwm->Config.prescaler;
    hpwm->Instance->ARR  = hpwm->Config.period;
    hpwm->Instance->EGR |= TIM_EGR_UG;     /* Force-load PSC/ARR shadow registers */
    hpwm->Instance->CR1 |= TIM_CR1_ARPE;   /* Auto-reload preload enable           */

    /* Configure channel output mode — PWM Mode 1 */
    uint8_t ch    = hpwm->channel - 1U;
    uint8_t shift = (ch & 1U) ? 12U : 4U;
    volatile uint32_t *ccmr = (ch < 2U) ?
                               &hpwm->Instance->CCMR1 :
                               &hpwm->Instance->CCMR2;

    uint32_t ccmr_tmp  = *ccmr;
    ccmr_tmp &= ~(0x7U << shift);          /* Clear OC mode bits     */
    ccmr_tmp &= ~(1U   << (shift - 1U));   /* Clear preload bit      */
    ccmr_tmp |=  (0x6U << shift);          /* PWM Mode 1             */
    ccmr_tmp |=  (1U   << (shift - 1U));   /* Output compare preload */
    *ccmr = ccmr_tmp;

    /* Configure polarity — intentionally leave CCxE=0 (output OFF)
     * Buzzer_On() / Buzzer_Off() control the output at runtime      */
    uint8_t  ccer_shift = ch * 4U;
    uint32_t ccer_tmp   = hpwm->Instance->CCER;
    ccer_tmp &= ~((1U << ccer_shift) | (1U << (ccer_shift + 1U)));
    if (hpwm->Config.polarity)
        ccer_tmp |= (1U << (ccer_shift + 1U));   /* Active Low */
    hpwm->Instance->CCER = ccer_tmp;

    /* Advanced timer guard — TIM1 only on F103 (F103 has no TIM8) */
    if (hpwm->Instance == TIM1)
        hpwm->Instance->BDTR |= TIM_BDTR_MOE;

    /* Start counter */
    hpwm->Instance->CR1 |= TIM_CR1_CEN;

    return PWM_STATUS_OK;
}

PWM_Status_t PWM_SetDuty(PWM_HandleTypeDef *hpwm, uint32_t duty)
{
    if (hpwm == NULL || hpwm->Instance == NULL) return PWM_STATUS_ERROR;
    if (hpwm->channel < 1 || hpwm->channel > 4) return PWM_STATUS_ERROR;

    if (duty > hpwm->Config.period)
        duty = hpwm->Config.period;

    switch (hpwm->channel)
    {
        case 1: hpwm->Instance->CCR1 = duty; break;
        case 2: hpwm->Instance->CCR2 = duty; break;
        case 3: hpwm->Instance->CCR3 = duty; break;
        case 4: hpwm->Instance->CCR4 = duty; break;
        default: return PWM_STATUS_ERROR;
    }

    return PWM_STATUS_OK;
}