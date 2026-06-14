/**
 * startup_stm32f103xb.s
 * Minimal startup file for STM32F103C8T6 (Blue Pill)
 * Cortex-M3, 64KB Flash, 20KB RAM
 */

  .syntax unified
  .cpu cortex-m3
  .thumb

/* _estack is defined in the linker script as the top of SRAM (0x20005000)  */
/* Using it directly here avoids the .stack section address-zero problem    */
  .extern _estack

/* Vector Table */
  .section .isr_vector, "a", %progbits
  .type g_pfnVectors, %object

g_pfnVectors:
  /* Core Exceptions */
  .word  _estack                      /* 0:  Initial Stack Pointer (top of SRAM) */
  .word  Reset_Handler                /* 1:  Reset Handler                */
  .word  NMI_Handler                  /* 2:  NMI Handler                  */
  .word  HardFault_Handler            /* 3:  Hard Fault Handler           */
  .word  MemManage_Handler            /* 4:  MPU Fault Handler            */
  .word  BusFault_Handler             /* 5:  Bus Fault Handler            */
  .word  UsageFault_Handler           /* 6:  Usage Fault Handler          */
  .word  0                            /* 7:  Reserved                     */
  .word  0                            /* 8:  Reserved                     */
  .word  0                            /* 9:  Reserved                     */
  .word  0                            /* 10: Reserved                     */
  .word  SVC_Handler                  /* 11: SVCall Handler               */
  .word  DebugMon_Handler             /* 12: Debug Monitor Handler        */
  .word  0                            /* 13: Reserved                     */
  .word  PendSV_Handler               /* 14: PendSV Handler               */
  .word  SysTick_Handler              /* 15: SysTick Handler              */

  /* STM32F103 External Interrupts (IRQ0 - IRQ59) */
  .word  WWDG_IRQHandler              /* 0:  Window Watchdog              */
  .word  PVD_IRQHandler               /* 1:  PVD through EXTI Line detect */
  .word  TAMPER_IRQHandler            /* 2:  Tamper                       */
  .word  RTC_IRQHandler               /* 3:  RTC                          */
  .word  FLASH_IRQHandler             /* 4:  Flash                        */
  .word  RCC_IRQHandler               /* 5:  RCC                          */
  .word  EXTI0_IRQHandler             /* 6:  EXTI Line 0  <-- YOUR BUTTON */
  .word  EXTI1_IRQHandler             /* 7:  EXTI Line 1                  */
  .word  EXTI2_IRQHandler             /* 8:  EXTI Line 2                  */
  .word  EXTI3_IRQHandler             /* 9:  EXTI Line 3                  */
  .word  EXTI4_IRQHandler             /* 10: EXTI Line 4                  */
  .word  DMA1_Channel1_IRQHandler     /* 11: DMA1 Channel 1               */
  .word  DMA1_Channel2_IRQHandler     /* 12: DMA1 Channel 2               */
  .word  DMA1_Channel3_IRQHandler     /* 13: DMA1 Channel 3               */
  .word  DMA1_Channel4_IRQHandler     /* 14: DMA1 Channel 4               */
  .word  DMA1_Channel5_IRQHandler     /* 15: DMA1 Channel 5               */
  .word  DMA1_Channel6_IRQHandler     /* 16: DMA1 Channel 6               */
  .word  DMA1_Channel7_IRQHandler     /* 17: DMA1 Channel 7               */
  .word  ADC1_2_IRQHandler            /* 18: ADC1 & ADC2                  */
  .word  USB_HP_CAN1_TX_IRQHandler    /* 19: USB High / CAN1 TX           */
  .word  USB_LP_CAN1_RX0_IRQHandler   /* 20: USB Low / CAN1 RX0           */
  .word  CAN1_RX1_IRQHandler          /* 21: CAN1 RX1                     */
  .word  CAN1_SCE_IRQHandler          /* 22: CAN1 SCE                     */
  .word  EXTI9_5_IRQHandler           /* 23: EXTI Lines 9..5              */
  .word  TIM1_BRK_IRQHandler          /* 24: TIM1 Break                   */
  .word  TIM1_UP_IRQHandler           /* 25: TIM1 Update                  */
  .word  TIM1_TRG_COM_IRQHandler      /* 26: TIM1 Trigger & Commutation   */
  .word  TIM1_CC_IRQHandler           /* 27: TIM1 Capture Compare         */
  .word  TIM2_IRQHandler              /* 28: TIM2                         */
  .word  TIM3_IRQHandler              /* 29: TIM3                         */
  .word  TIM4_IRQHandler              /* 30: TIM4                         */
  .word  I2C1_EV_IRQHandler           /* 31: I2C1 Event                   */
  .word  I2C1_ER_IRQHandler           /* 32: I2C1 Error                   */
  .word  I2C2_EV_IRQHandler           /* 33: I2C2 Event                   */
  .word  I2C2_ER_IRQHandler           /* 34: I2C2 Error                   */
  .word  SPI1_IRQHandler              /* 35: SPI1                         */
  .word  SPI2_IRQHandler              /* 36: SPI2                         */
  .word  USART1_IRQHandler            /* 37: USART1                       */
  .word  USART2_IRQHandler            /* 38: USART2                       */
  .word  USART3_IRQHandler            /* 39: USART3                       */
  .word  EXTI15_10_IRQHandler         /* 40: EXTI Lines 15..10            */
  .word  RTC_Alarm_IRQHandler         /* 41: RTC Alarm through EXTI Line  */
  .word  USBWakeUp_IRQHandler         /* 42: USB Wakeup from suspend      */
  .word  TIM8_BRK_IRQHandler          /* 43: TIM8 Break                   */
  .word  TIM8_UP_IRQHandler           /* 44: TIM8 Update                  */
  .word  TIM8_TRG_COM_IRQHandler      /* 45: TIM8 Trigger & Commutation   */
  .word  TIM8_CC_IRQHandler           /* 46: TIM8 Capture Compare         */
  .word  ADC3_IRQHandler              /* 47: ADC3                         */
  .word  FSMC_IRQHandler              /* 48: FSMC                         */
  .word  SDIO_IRQHandler              /* 49: SDIO                         */
  .word  TIM5_IRQHandler              /* 50: TIM5                         */
  .word  SPI3_IRQHandler              /* 51: SPI3                         */
  .word  UART4_IRQHandler             /* 52: UART4                        */
  .word  UART5_IRQHandler             /* 53: UART5                        */
  .word  TIM6_IRQHandler              /* 54: TIM6                         */
  .word  TIM7_IRQHandler              /* 55: TIM7                         */
  .word  DMA2_Channel1_IRQHandler     /* 56: DMA2 Channel 1               */
  .word  DMA2_Channel2_IRQHandler     /* 57: DMA2 Channel 2               */
  .word  DMA2_Channel3_IRQHandler     /* 58: DMA2 Channel 3               */
  .word  DMA2_Channel4_5_IRQHandler   /* 59: DMA2 Channel 4 & 5           */

/*---------------------------------------------------------------------------
 * Reset Handler — runs immediately after power-on or reset
 *--------------------------------------------------------------------------*/
  .section .text.Reset_Handler
  .weak   Reset_Handler
  .type   Reset_Handler, %function
Reset_Handler:
  /* Copy .data section from Flash to SRAM */
  ldr   r0, =_sdata         /* destination start (SRAM) */
  ldr   r1, =_edata         /* destination end   (SRAM) */
  ldr   r2, =_sidata        /* source start      (Flash) */
  movs  r3, #0
  b     CopyDataLoop

CopyDataBody:
  ldr   r4, [r2, r3]
  str   r4, [r0, r3]
  adds  r3, r3, #4

CopyDataLoop:
  adds  r4, r0, r3
  cmp   r4, r1
  bcc   CopyDataBody

  /* Zero .bss section in SRAM */
  ldr   r2, =_sbss
  ldr   r4, =_ebss
  movs  r3, #0
  b     ZeroBssLoop

ZeroBssBody:
  str   r3, [r2]
  adds  r2, r2, #4

ZeroBssLoop:
  cmp   r2, r4
  bcc   ZeroBssBody

  /* Jump to main() */
  bl    main
  bx    lr

/*---------------------------------------------------------------------------
 * Default weak handlers — all unimplemented interrupts loop here
 * Your handlers (EXTI0_IRQHandler etc.) override these automatically
 *--------------------------------------------------------------------------*/
  .macro WEAK_HANDLER name
  .weak \name
  .thumb_set \name, Default_Handler
  .endm

  .section .text.Default_Handler, "ax", %progbits
Default_Handler:
  b Default_Handler         /* Infinite loop — lets debugger catch it */

  WEAK_HANDLER NMI_Handler
  WEAK_HANDLER HardFault_Handler
  WEAK_HANDLER MemManage_Handler
  WEAK_HANDLER BusFault_Handler
  WEAK_HANDLER UsageFault_Handler
  WEAK_HANDLER SVC_Handler
  WEAK_HANDLER DebugMon_Handler
  WEAK_HANDLER PendSV_Handler
  WEAK_HANDLER SysTick_Handler
  WEAK_HANDLER WWDG_IRQHandler
  WEAK_HANDLER PVD_IRQHandler
  WEAK_HANDLER TAMPER_IRQHandler
  WEAK_HANDLER RTC_IRQHandler
  WEAK_HANDLER FLASH_IRQHandler
  WEAK_HANDLER RCC_IRQHandler
  WEAK_HANDLER EXTI0_IRQHandler
  WEAK_HANDLER EXTI1_IRQHandler
  WEAK_HANDLER EXTI2_IRQHandler
  WEAK_HANDLER EXTI3_IRQHandler
  WEAK_HANDLER EXTI4_IRQHandler
  WEAK_HANDLER DMA1_Channel1_IRQHandler
  WEAK_HANDLER DMA1_Channel2_IRQHandler
  WEAK_HANDLER DMA1_Channel3_IRQHandler
  WEAK_HANDLER DMA1_Channel4_IRQHandler
  WEAK_HANDLER DMA1_Channel5_IRQHandler
  WEAK_HANDLER DMA1_Channel6_IRQHandler
  WEAK_HANDLER DMA1_Channel7_IRQHandler
  WEAK_HANDLER ADC1_2_IRQHandler
  WEAK_HANDLER USB_HP_CAN1_TX_IRQHandler
  WEAK_HANDLER USB_LP_CAN1_RX0_IRQHandler
  WEAK_HANDLER CAN1_RX1_IRQHandler
  WEAK_HANDLER CAN1_SCE_IRQHandler
  WEAK_HANDLER EXTI9_5_IRQHandler
  WEAK_HANDLER TIM1_BRK_IRQHandler
  WEAK_HANDLER TIM1_UP_IRQHandler
  WEAK_HANDLER TIM1_TRG_COM_IRQHandler
  WEAK_HANDLER TIM1_CC_IRQHandler
  WEAK_HANDLER TIM2_IRQHandler
  WEAK_HANDLER TIM3_IRQHandler
  WEAK_HANDLER TIM4_IRQHandler
  WEAK_HANDLER I2C1_EV_IRQHandler
  WEAK_HANDLER I2C1_ER_IRQHandler
  WEAK_HANDLER I2C2_EV_IRQHandler
  WEAK_HANDLER I2C2_ER_IRQHandler
  WEAK_HANDLER SPI1_IRQHandler
  WEAK_HANDLER SPI2_IRQHandler
  WEAK_HANDLER USART1_IRQHandler
  WEAK_HANDLER USART2_IRQHandler
  WEAK_HANDLER USART3_IRQHandler
  WEAK_HANDLER EXTI15_10_IRQHandler
  WEAK_HANDLER RTC_Alarm_IRQHandler
  WEAK_HANDLER USBWakeUp_IRQHandler
  WEAK_HANDLER TIM8_BRK_IRQHandler
  WEAK_HANDLER TIM8_UP_IRQHandler
  WEAK_HANDLER TIM8_TRG_COM_IRQHandler
  WEAK_HANDLER TIM8_CC_IRQHandler
  WEAK_HANDLER ADC3_IRQHandler
  WEAK_HANDLER FSMC_IRQHandler
  WEAK_HANDLER SDIO_IRQHandler
  WEAK_HANDLER TIM5_IRQHandler
  WEAK_HANDLER SPI3_IRQHandler
  WEAK_HANDLER UART4_IRQHandler
  WEAK_HANDLER UART5_IRQHandler
  WEAK_HANDLER TIM6_IRQHandler
  WEAK_HANDLER TIM7_IRQHandler
  WEAK_HANDLER DMA2_Channel1_IRQHandler
  WEAK_HANDLER DMA2_Channel2_IRQHandler
  WEAK_HANDLER DMA2_Channel3_IRQHandler
  WEAK_HANDLER DMA2_Channel4_5_IRQHandler

  .end
