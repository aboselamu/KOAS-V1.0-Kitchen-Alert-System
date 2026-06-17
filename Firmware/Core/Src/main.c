/**
 *******************************************************************************
 * @file    main.c
 * @brief   Kitchen Order Alert System — KOAS-V1.0
 * @target  STM32F103C8T6 (Blue Pill) — Device ID 0x410
 * @clock   8MHz HSI internal oscillator
 *******************************************************************************
 *
 * Hardware Map:
 *   PA0  — Silence button    (10kΩ pull-up, 100nF cap to GND)
 *   PA1  — HUAXING IR sensor (NPN, 2.2kΩ/10kΩ voltage divider)
 *   PA2  — TIM2 CH3 PWM      (2kHz buzzer)
 *   PC13 — Onboard LED       (active-low)
 *
 * FSM States:
 *   IDLE      — quiet, waiting for order
 *   ALARMING  — ticket present, buzzer pulsing
 *   WAIT_ACK  — ticket removed, buzzer still pulsing, waiting for button
 *
 * Interrupt flow:
 *   Hardware IRQ → stm32_it.c → EXTI_IRQHandling() → callback → FSM
 */

#include "stm32f1xx.h"
#include <stdint.h>

#include "ir_sensor_driver.h"
#include "exti_driver.h"
#include "pwm_driver.h"
#include "buzzer_driver.h"
#include "led_driver.h"
#include "order_alert_fsm.h"

/*===========================================================================
 * System tick
 *==========================================================================*/
volatile uint32_t tick_ms = 0U;

void SysTick_Handler(void)
{
    tick_ms++;
}

static void systick_init(void)
{
    SysTick_Config(8000U);   /* 8MHz / 8000 = 1ms tick */
}

/*===========================================================================
 * LED GPIO init
 * PA0, PA1 handled by EXTI_MspInit
 * PA2     handled by PWM_MspInit
 * PC13    handled here — not owned by any driver
 *==========================================================================*/
static void led_gpio_init(void)
{
    RCC->APB2ENR |= (1U << 4);       /* GPIOC clock           */
    GPIOC->CRH   &= ~(0xFU << 20);   /* clear PC13 bits       */
    GPIOC->CRH   |=  (0x2U << 20);   /* push-pull 2MHz output */
    GPIOC->ODR   |=  (1U   << 13);   /* active-low: start OFF */
}

/*===========================================================================
 * IWDG — ~1 second timeout, always init last
 *==========================================================================*/
static void iwdg_init(void)
{
    IWDG->KR  = 0xCCCCU;   /* start on LSI clock */
    IWDG->KR  = 0x5555U;   /* unlock PR + RLR    */
    IWDG->PR  = 0x3U;      /* prescaler /32       */
    IWDG->RLR = 1248U;     /* ~1 second timeout   */
    IWDG->KR  = 0xAAAAU;   /* initial reload      */
}

/*===========================================================================
 * Power-on confirmation — 200ms beep + LED flash
 *==========================================================================*/
static void startup_confirmation(void)
{
    /* 1. Turn on indicators */
    Buzzer_On();
    LED_On();

    /* 2. Wait 200ms for the "beep" */
    uint32_t start = tick_ms;
    while ((tick_ms - start) < 200U)
    {
        IWDG->KR = 0xAAAAU;   /* kick watchdog while waiting */
    }

    /* 3. Wait for the sensor to be cleared */
    // We don't need to keep calling Buzzer_On(), it is already running.
    while (IR_Sensor_IsBeamBroken())
    {
        IWDG->KR = 0xAAAAU;   /* CRITICAL: Must feed watchdog here too! */
    }

    /* 4. Turn off indicators and proceed to main loop */
    Buzzer_Off();
    LED_Off();
}

/*===========================================================================
 * Application Callbacks
 * Only place where hardware events are translated into FSM events.
 * Flow: stm32_it.c -> EXTI_IRQHandling() -> callback -> FSM_ProcessEvent()
 *==========================================================================*/

/* PA0 — Silence button, falling edge */
void Button_Callback(uint8_t line)
{
    (void)line;
    FSM_ProcessEvent(BUTTON_PRESSED);
}

/* PA1 — IR sensor, both edges
 * IR_Sensor_IsBeamBroken() reads IDR here — FSM never touches hardware
 */
void IR_Sensor_Callback(uint8_t line)
{
    (void)line;
    if (IR_Sensor_IsBeamBroken())
        FSM_ProcessEvent(TICKET_DETECTED);
    else
        FSM_ProcessEvent(TICKET_REMOVED);
}

/*===========================================================================
 * Main
 *==========================================================================*/
int main(void)
{
    /* 1. System tick — first so all timing works immediately */
    systick_init();

    /* 2. LED GPIO */
    led_gpio_init();

    /* 3. PWM — timer running, output OFF until Buzzer_On() */
    PWM_HandleTypeDef hpwm    = {0};
    hpwm.Instance             = TIM2;
    hpwm.channel              = 3U;
    hpwm.Config.prescaler     = 7U;    /* 8MHz / 8 = 1MHz         */
    hpwm.Config.period        = 499U;  /* 1MHz / 500 = 2kHz       */
    hpwm.Config.polarity      = 0U;    /* active high              */
    PWM_Init(&hpwm);
    PWM_SetDuty(&hpwm, 249U);          /* 50% duty cycle           */

    /* 4. EXTI — silence button on PA0, falling edge */
    EXTI_Handle_t h_button              = {0};
    h_button.line                       = 0U;
    h_button.trigger                    = EXTI_TRIGGER_FALLING;
    h_button.pPort                      = GPIOA;
    h_button.pApplicationCallback       = Button_Callback;
    EXTI_Init(&h_button);

    /* 5. EXTI — IR sensor on PA1, both edges */
    EXTI_Handle_t h_sensor              = {0};
    h_sensor.line                       = 1U;
    h_sensor.trigger                    = EXTI_TRIGGER_BOTH;
    h_sensor.pPort                      = GPIOA;
    h_sensor.pApplicationCallback       = IR_Sensor_Callback;
    EXTI_Init(&h_sensor);

    /* 6. FSM — set IDLE, buzzer off, LED off */
    FSM_Init();

    /* 7. IWDG — always last */
    iwdg_init();

    /* 8. Power-on confirmation */
    startup_confirmation();

    /*=========================================================================
     * Main loop
     *
     * FSM handles all state transitions via interrupt callbacks.
     * Main loop responsibilities:
     *   1. Kick watchdog every cycle
     *   2. Drive 900ms buzzer/LED pulse in ALARMING and WAIT_ACK states
     *
     * Why pulse in WAIT_ACK too?
     *   When ticket is removed, state moves ALARMING → WAIT_ACK.
     *   Buzzer must keep pulsing until button is pressed.
     *   If we only pulsed in ALARMING, the alarm would go silent
     *   the moment the ticket is lifted — defeating the purpose.
     *========================================================================*/
    while (1)
    {
        IWDG->KR = 0xAAAAU;   /* kick watchdog */

        static uint32_t last_buzz = 0U;
        SystemState_t   state     = FSM_GetState();

        if (state == STATE_ALARMING || state == STATE_WAIT_ACK)
        {
            if ((tick_ms - last_buzz) >= 900U)
            {
                last_buzz = tick_ms;
                Buzzer_Toggle();
                LED_Toggle();
            }
        }
        else
        {
            /* IDLE — ensure outputs are off
             * Reset last_buzz so first alarm beep fires immediately */
            Buzzer_Off();
            LED_Off();
            last_buzz = tick_ms - 900U;
        }
    }
}
