# KOAS-V1.0 Bare-Metal Engineering Manual
**Document Ref:** DEV360S-ENG-M002  
**Target Hardware:** STM32F103C8T6 (ARM Cortex-M3 Core Architecture)  
**Compilation Target:** GNU arm-none-eabi-gcc v15.2.rel1  
**Author:** DEV360S Advanced Technologies Engineering Team  

---

## 1. System Architectural Overview
The Kitchen Order Alert System (KOAS-V1.0) is a high-reliability, zero-overhead event-driven controller. By eliminating vendor HAL frameworks and operating system layers, the system guarantees deterministic, microsecond-level interrupt responses and precise peripheral timing through direct ARM Cortex-M3 register control.

### 1.1 Firmware Boot and Execution Pipeline
1. **Power-On Initialization:** System setup configurations are sequentially executed: `systick_init()`, `clock_init()`, `gpio_init()`, `sensor_init()`, `exti_init()`, `exti1_init()`, `pwm_init()`, and `iwdg_init()`.
2. **Diagnostic Boot Sequence (`startup_behav`):** Fires the buzzer and status LED for 200ms to visually and audibly confirm device health. If the infrared beam is blocked during startup, the firmware traps execution in a defensive toggling loop, refusing to clear until the ticket delivery lane is physically opened.
3. **Main Event Loop (`while(1)`):** Continuously refreshes the Independent Watchdog token (`0xAAAA`). If the global flag `buzzer_on == 1`, a non-blocking background time comparison triggers an asynchronous 900ms audio-visual flash pattern. If `buzzer_on == 0`, the timer output is forcefully squelched.

---

## 2. Event-Driven State & Interrupt Architecture
To achieve optimal latency and free up main loop processing overhead, responsibilities are split across two dedicated, priority-interleaved External Interrupt (EXTI) channels:

### 2.1 The Arrival Vector: EXTI1 (PA1 / IR Sensor Core)
* **Trigger Mechanics:** Configured via `AFIO->EXTICR` to track Port A, listening exclusively for a **Falling Edge** transition (`EXTI->FTSR`).
* **Operational Flow:** The moment a kitchen ticket breaks the optical path, the sensor output drops to 0V. This instantly fires `EXTI1_IRQHandler`, setting the global status flag `buzzer_on = 1`.

### 2.2 The Clearance Vector: EXTI0 (PA0 / ACK Button)
* **Trigger Mechanics:** Mapped to Port A Pin 0, tracking a **Falling Edge** layout.
* **The Logic Interlock:** To prevent chefs from muting the system while an active ticket is still blocking the window, the ISR reads the input data register: `if (GPIOA->IDR & (1U << 1))`. The alarm flag is only cleared (`buzzer_on = 0`) if the optical beam path is **fully clear**.
* **Software Debouncing:** Includes a non-blocking time-delta comparison wrapper (`tick_ms - last_tick >= 20`) to filter out high-frequency mechanical contact noise.

---

## 3. Peripheral Hardware Control Register Configurations

### 3.1 Advanced Power and Input Profiles
To save power and isolate internal buses, four separate peripheral paths are energized simultaneously inside `clock_init()` using the Reset and Clock Control register (`RCC->APB2ENR` and `RCC->APB1ENR`):

```c
/* Enable Alternate Function IO (AFIO), GPIOA, GPIOC, and TIM2 Peripherals */
RCC->APB2ENR |= (1U << 0) | (1U << 2) | (1U << 4);
RCC->APB1ENR |= (1U << 0);
```

* **PA0 (Button Input):** Set to Floating Input (`0x4`), removing internal pull resistors so it interacts perfectly with your external 10 kΩ stripboard array.
* **PA1 (IR Input):** Set to Floating Input (`0x4`), keeping the microcontroller completely clear of your external hardware step-down protection network.
* **PC13 (User LED):** Set to General Purpose Output Push-Pull at 2MHz (`0x2`) to handle the onboard Active-Low debugging element.

### 3.2 Acoustic PWM Generator Engine (`TIM2_CH3`)
To push a crisp 2.0 kHz alarm through noisy commercial kitchen settings, Timer 2 Channel 3 is mapped directly to Pin `PA2` via Alternate Function Push-Pull mode (`0xA`). 

The counter frequency calculation matches the following mathematical configuration matrix:

$$\text{Frequency} = \frac{f_{CLK}}{(\text{PSC} + 1) \times (\text{ARR} + 1)} = \frac{8,000,000}{(7 + 1) \times (499 + 1)} = 2000\text{ Hz}$$

```c
TIM2->PSC  = 7;      /* Pre-scaler divides 8MHz internal clock to 1MHz */
TIM2->ARR  = 499;    /* Count resets every 500 ticks to fix 2.0kHz frequency */
TIM2->CCR3 = 249;    /* Fixed 50% Duty Cycle for optimal acoustic resonance */
```

---

## 4. Production-Grade Safety, Diagnostics & Memory Fencing

### 4.1 Memory Fencing via Data Synchronization Barriers (`__DSB()`)
To ensure cross-compatibility with both authentic ST chips and third-party alternative silicon (like CKS32/CS32 clones), both interrupt service routines execute a hardware memory fence command (`__DSB()`) immediately after clearing the pending register:

```c
EXTI->PR = (1U << 0); // Clear the EXTI channel pending bit
__DSB();              // Force CPU core to wait until register write physically clears
```
**Why this matters for recruiters:** Without this barrier, the high-speed ARM pipeline could exit the interrupt before the slower physical silicon bus finishes clearing the hardware flag, causing the CPU to loop back into the interrupt indefinitely.

### 4.2 The Independent Watchdog Safe-Fail Shield (IWDG)
To make your system reliable enough to run indefinitely without maintenance, the Independent Watchdog peripheral runs on a dedicated internal low-speed oscillator (40 kHz). If a voltage spike freezes the main execution loop, the watchdog stops receiving its refresh code and resets the microcontroller within 1.0 second:

```c
IWDG->KR  = 0xCCCC;  /* Wake up and arm the IWDG module */
IWDG->KR  = 0x5555;  /* Access key to unlock RLR register writes */
IWDG->PR  = 0x3U;    /* Divide oscillator by 32 (1.25 kHz counting speed) */
IWDG->RLR = 1248U;   /* 1248 ticks / 1250Hz = ~1.0 Second Reset Window */
IWDG->KR  = 0xAAAA;  /* Start counting / Refresh command macro */
```
