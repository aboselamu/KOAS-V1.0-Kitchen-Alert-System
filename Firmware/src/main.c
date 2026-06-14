
#include "stm32f1xx.h"
#include <stdint.h>

volatile uint32_t tick_ms = 0;
volatile uint8_t buzzer_on = 0;


static void systick_init(void)
{
    SysTick_Config(8000);  /* 8MHz / 8000 = 1ms tick */
}

static void clock_init(void)
{
    RCC->APB2ENR |= (1U << 0);
    RCC->APB2ENR |= (1U << 2);
    RCC->APB2ENR |= (1U << 4);
    RCC->APB1ENR |= (1U << 0); 
}

static void gpio_init(void)
{
    GPIOA->CRL &= ~(0xFU << 0);
    GPIOA->CRL |=  (0x4U << 0);

    GPIOC->CRH &= ~(0xFU << 20);
    GPIOC->CRH |=  (0x2U << 20);
}

static void exti_init(void)
{
    AFIO->EXTICR[0] &= ~(0xFU << 0);
    AFIO->EXTICR[0] |=  (0x0U << 0);

    EXTI->IMR  |=  (1U << 0);
    EXTI->FTSR |=  (1U << 0);
    EXTI->RTSR &= ~(1U << 0);

    NVIC_SetPriority(EXTI0_IRQn, 1U);
    NVIC_EnableIRQ(EXTI0_IRQn);
}


static void pwm_init(void){

    GPIOA->CRL &= ~(0XF << 2*4); 
    GPIOA->CRL |= (0XA<< 2*4); 

    TIM2->PSC = 7; 
    TIM2->ARR = 499; 
    TIM2->CCR3 = 249;

    TIM2->CCMR2 &= ~(0x3 << 0);   // CC3S = 00, output mode 
    TIM2->CCMR2 &= ~(0x7 << 4);   // clear OC3M bits first 
    TIM2->CCMR2 |=  (0x6 << 4);   // OC3M = 110, PWM mode 1 
    TIM2->CCER  &=  ~(0x1 << 8);   // CC3E = 1, channel 3 output enabled 
    TIM2->CR1   |= ((0x1 << 0) | (0x1 << 7));  // CEN=1 start counter, ARPE=1 preload 
}

static void sensor_init(){
    GPIOA->CRL &= ~(0xFU << (1*4));
    GPIOA->CRL |= (0X4U << (1*4));
}

static void exti1_init(void)
{
    AFIO->EXTICR[0] &= ~(0xFU << 4);
    AFIO->EXTICR[0] |=  (0x0U << 4);
    EXTI->IMR  |=  (1U << 1);
    EXTI->FTSR |=  (1U << 1);
    EXTI->RTSR &= ~(1U << 1);
    NVIC_SetPriority(EXTI1_IRQn, 1U);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

static void iwdg_init(void)
{
    IWDG->KR  = 0xCCCC;   /* start watchdog */
    IWDG->KR  = 0x5555;   /* unlock PR and RLR */
    IWDG->PR  = 0x3U;     /* prescaler 32 */
    IWDG->RLR = 1248U;    /* 1 second timeout */
    IWDG->KR  = 0xAAAA;   /* reload counter */
}
static void startup_behav(void)
{
        /* power-on confirmation */
    TIM2->CCER |=  (0x1U << 8);     /* buzzer ON  */
    GPIOC->ODR &= ~(1U   << 13);    /* LED ON     */

    uint32_t start = tick_ms;
    while ((tick_ms - start) < 200U) /* wait 200ms */
        {
        IWDG->KR = 0xAAAAU;          /* kick watchdog while waiting */
        }

    TIM2->CCER &= ~(0x1U << 8);     /* buzzer OFF */
    // GPIOC->ODR |=  (1U   << 13);    /* LED OFF    */
    while((GPIOA->IDR & (1U << 1)) ==0)
    // if ()  /* beam clear — ticket removed */
            {
                // buzzer_on = 0U;
                TIM2->CCER ^=  (0x1U << 8);   /* toggle buzzer */
                GPIOC->ODR ^=  (1U   << 13);  /* toggle LED    */
            }
}

int main(void)
{
    systick_init();
    clock_init();
    gpio_init();
    sensor_init();
    exti_init();
    exti1_init();
    pwm_init();
    iwdg_init();   

    startup_behav();
    

    while (1)
    {
        IWDG->KR = 0xAAAAU;

        if (buzzer_on)
     {
            static uint32_t last_buzz = 0;

            if ((tick_ms - last_buzz) >= 900U)
            {
                last_buzz = tick_ms;
                TIM2->CCER ^=  (0x1U << 8);   /* toggle buzzer */
                GPIOC->ODR ^=  (1U   << 13);  /* toggle LED    */
            }
     }
     else
        {
            TIM2->CCER &= ~(0x1U << 8);   /* buzzer OFF */
            GPIOC->ODR |=  (1U   << 13);  /* LED OFF    */
        }
    }
    }
void SysTick_Handler(void)
{
    tick_ms++;
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1U << 0))
    {
        EXTI->PR = (1U << 0);
        __DSB();

        static uint32_t last_tick = 0;

        if ((tick_ms - last_tick) >= 20)
        {
            last_tick = tick_ms;

            if (GPIOA->IDR & (1U << 1))  /* beam clear — ticket removed */
            {
                buzzer_on = 0U;
            }
        }
    }
}

void EXTI1_IRQHandler(void){
  if(EXTI->PR & (1U << 1)){
    EXTI->PR = (1U << 1);
    __DSB();
    buzzer_on = 1;
  }

}

/*
Power on → all peripherals init → watchdog armed

Normal idle:
    main loop kicks watchdog every cycle
    beam clear, buzzer silent

Order arrives (beam breaks):
    EXTI1 fires → buzzer ON

Staff acknowledges:
    button pressed + beam clear → buzzer OFF
    button pressed + beam blocked → ignored

Firmware crash (any reason):
    watchdog not kicked within 1 second → auto reset → system restarts clean
*/