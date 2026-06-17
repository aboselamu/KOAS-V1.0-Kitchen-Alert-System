# KOAS-V1.0 | Kitchen Order Alert System Hardware Platform


An industrial-grade, ultra-low-latency BARE-METAL embedded controller engineered to manage high-volume food service delivery windows. Running directly on raw hardware registers without an RTOS or vendor abstract frameworks (HAL/LL), KOAS-V1.0 features asynchronous dual-vector interrupt loops, real-world electrical noise filtering, a self-diagnostic startup safety matrix, and an independent watchdog shield.

---

## 📸 System Enclosure Branding
Below is the commercial graphic overlay and technical specification panel designed for the physical product case, matching industrial manufacturing and regulatory requirements:

<p align="center">
  <img src="Hardware/Enclosure_Sticker.jpg" alt="DEV360S KOAS-V1.0 Front Panel Label" width="550"/>
</p>

---

## 🛠️ Core Engineering & Production Highlights
* **Zero-Abstraction Register Control:** Maintained absolute memory and execution control by bypassing heavy libraries, programming the ARM Cortex-M3 clock trees (`RCC`), general-purpose timers (`TIM2`), and input/output registers directly via raw bitmasks.
* **Dual-Vector Interleaved Interrupt Architecture:** Implemented asynchronous edge-triggered interrupt service routines across separate lanes (`EXTI0` and `EXTI1`). Order arrivals are captured instantly on falling edges, keeping the main processing loop completely un-blocked.
* **Defensive Silicon Isolation (Cross-Vendor Safety):** Embedded data synchronization barriers (`__DSB()`) directly following interrupt clears. This acts as a hardware memory fence, blocking internal instruction pipelining race conditions across cheaper alternative silicon clones (CKS32/CS32) found in real-world supply chains.
* **Industrial Electrical Noise Mitigation:** Handled severe kitchen electromagnetic interference (EMI) via an external 2.2 kΩ / 10 kΩ stripboard step-down voltage divider combined with a 10 kΩ pull-up resistor and a 100 nF ceramic capacitor filter for full hardware button debouncing.
* **Self-Healing Watchdog Architecture (IWDG):** Enabled the internal low-speed independent hardware watchdog clock (40 kHz). The system automatically performs a full-chip power reset within 1.0 second if an industrial power surge freezes the main processing line.
* **Custom Toolchain Compilation Pipeline:** Formulated a lightweight manual `Makefile` execution structure using the standard cross-compiler toolchain to compile clean machine instruction binaries without specialized IDE dependencies.


                            3.3V ──┬── 10kΩ ──┬── PA0
                                │          │
                                │        [Button]
                                │          │
                                GND ────────┘

---

## 📊 Finite State Machine (FSM) State Rules
The application loop operates a predictable, deterministic state layout to manage kitchen order delivery flows:

| Operating State | Hardware Trigger | Peripheral Reaction Profile |
| :--- | :--- | :--- |
| **`STATE_IDLE`** | IR Sensor Beam Clear (`PA1` High) | `TIM2` PWM Output Deactivated / `PC13` Onboard User LED Turned OFF |
| **`STATE_ALARM`** | IR Sensor Beam Broken (`PA1` Falls Low) | `TIM2` Pulse Channel Energised at 2.0 kHz / `PC13` Flashed at 900ms Intervals |
| **`STATE_MUTED`** | ACK Button Pressed + Beam Clear (`PA0`) | `TIM2` Audio Output Silenced Instantly / `PC13` Held Solid ON as a Silent Visual Reminder |

*Note: The system contains a physical logic interlock loop. If the ACK button is pressed while a ticket is still physically blocking the optical path (`PA1` is Low), the system forcefully ignores the mute request, safeguarding the order tracking integrity.*

---

## 📂 Repository File Allocation Map
```text
📁 KOAS-V1.0-Kitchen-Alert-System-main/
├── 📁 Firmware/
│   ├── 📁 startup/                        # Low-level system assembly boot vectors and linker script
│   │   ├── 📄 startup_stm32f103xb.s       # ARM Cortex-M3 reset handler and interrupt vector table
│   │   └── 📄 stm32f103c8tx_flash.ld      # Linker script defining memory regions and section layout
│   ├── 📁 Core/                           # Platform entry point and interrupt service routines
│   │   ├── 📁 Inc/
│   │   │   └── 📄 stm32_it.h             # ISR function declarations
│   │   └── 📁 Src/
│   │       ├── 📄 main.c                  # Main execution entry point and system initialization
│   │       └── 📄 stm32_it.c             # Interrupt service routine implementations
│   ├── 📁 App/                            # Application-layer FSM logic
│   │   ├── 📁 Inc/
│   │   │   └── 📄 order_alert_fsm.h      # FSM state and event type declarations
│   │   └── 📁 Src/
│   │       └── 📄 order_alert_fsm.c      # FSM transition logic and alert sequencing
│   ├── 📁 Drivers/                        # Hardware abstraction and CMSIS peripheral drivers
│   │   ├── 📁 MCU_Drivers/               # Custom peripheral driver layer
│   │   │   ├── 📁 Inc/                   # Driver header files (buzzer, EXTI, IR sensor, LED, PWM)
│   │   │   └── 📁 Src/                   # Driver source implementations
│   │   ├── 📁 BSP/                       # Board support package
│   │   │   ├── 📁 Inc/
│   │   │   └── 📁 Src/
│   │   └── 📁 CMSIS/                     # ARM CMSIS core and STM32F1xx device headers
│   │       ├── 📁 Include/               # Core Cortex-M3 headers (core_cm3.h, cmsis_gcc.h, etc.)
│   │       └── 📁 Device/               # STM32F1xx device-specific register definitions
│   ├── 📁 build/                          # Compiled output artifacts (binary, ELF, map file)
│   └── 📄 Makefile                        # Toolchain cross-compiler compilation instruction script
├── 📁 Hardware/
│   └── 🖼️ Enclosure_Sticker_koas_v1.jpg  # Production case cover sticker branding graphic
└── 📁 Documentation/
    ├── 📄 Engineering_Manual.md            # High-density architectural manual, mathematical analysis, and configurations
    ├── 📄 Firmware Architecture - Kitchen Order Alert System FSM.pdf  # FSM architecture slide deck
    └── 🖼️ FSM diagrams & flow images       # D-flow.png, FSM-Theory, FSM-Diagram, FSM-Implementation slides
```

---

## ⚡ Technical Specifications & Electrical Envelope
* **Logic Controller Core:** STMicroelectronics STM32F103C8T6 (Genuine Core ID: `0x1BA01477` / Device ID: `0x410`)
* **Primary System Clock Speed:** 8.0 MHz (Internal RC High-Speed Oscillator Setup)
* **Regulated DC System Rail:** 3.24V - 3.3V (Stepped down from 5.0V Buck infrastructure via internal LDO link)
* **Acoustic Alerting Frequency:** $2.0\text{ kHz}$ Edge-Aligned Pulse-Width Modulation (PWM Mode 1 via `TIM2_CH3`)
* **Operating Temperature Window:** $-20^\circ\text{C}$ to $+85^\circ\text{C}$ (Industrial Grade Envelope)
* **Mechanical Debounce Delay Constraint:** 30ms Synchronous Delta Protection Check

