#include "stm32f4xx.h"  // Core CMSIS Device Header (gives access to register definitions)
void SystemInit(void)
{
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
}// Function Prototypes
void UART2_Init(void);
void UART2_Write(int ch);
char UART2_Read(void);

/*

*/

int main(void) {
    // 1. Initialize the USART2 hardware
    UART2_Init();
    
    // 2. Enable Clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // 3. Configure PA0 (Pin A0) and PA1 (Pin A1) as OUTPUTS (01)
    GPIOA->MODER &= ~((3U << (0 * 2)) | (3U << (1 * 2))); // Clear bits
    GPIOA->MODER |= ((1U << (0 * 2)) | (1U << (1 * 2)));  // Set to Output

    // 4. Configure PA4 (Pin A2) as an INPUT (00) to read the Buzzer
    GPIOA->MODER &= ~(3U << (4 * 2)); // Clear bits to make it an Input

    // Enable the internal Pull-Down resistor on PA4 so it defaults to '0'
    GPIOA->PUPDR &= ~(3U << (4 * 2)); // Clear existing pull-up/pull-down settings
    GPIOA->PUPDR |= (2U << (4 * 2));  // Set to 10 (Pull-Down mode)
    
    // 5. CRITICAL FIX: Keep outputs HIGH by default (Unbroken Beam, Unpressed Button)
    GPIOA->ODR |= ((1U << 0) | (1U << 1));

    // 6. Transmit the initial boot message
    char boot_msg[] = "Nucleo Ready\r\n";
    for(int i = 0; boot_msg[i] != '\0'; i++) {
        UART2_Write(boot_msg[i]);
    }

    /* Infinite loop */
    while (1) {
        char rx_byte = UART2_Read();
        
        // PING: Python asks if we are awake
        if (rx_byte == '?') {
            char boot_msg[] = "Nucleo Ready\r\n";
            for(int i = 0; boot_msg[i] != '\0'; i++) {
                UART2_Write(boot_msg[i]);
            }
        }
        // QUERY: Read the Blue Pill's Buzzer (Pin A2 / PA4)
        else if (rx_byte == 'Q') {
            char val = (GPIOA->IDR & (1U << 4)) ? '1' : '0'; 
            UART2_Write(val);
            UART2_Write('\n'); 
        }
        // BREAK BEAM: Set PA0 LOW (Pin A0) - Simulate physical break
        else if (rx_byte == 'B') {
            GPIOA->ODR &= ~(1U << 0); 
        }
        // RESTORE BEAM: Set PA0 HIGH (Pin A0) - Simulate physical restore
        else if (rx_byte == 'R') {
            GPIOA->ODR |= (1U << 0); 
        }
        // SILENCE BUTTON PRESS: Set PA1 LOW (Pin A1)
        else if (rx_byte == 'S') {
            GPIOA->ODR &= ~(1U << 1); 
        }
        // SILENCE BUTTON RELEASE: Set PA1 HIGH (Pin A1)
        else if (rx_byte == 'U') {
            GPIOA->ODR |= (1U << 1); 
        }
    }
}

/* --- USART2 Setup Code --- */
void UART2_Init(void) {
    // 1. Enable Clock Access to GPIOA and USART2 peripherals
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // GPIOA Clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // USART2 Clock

    // 2. Configure PA2 (TX) and PA3 (RX) for Alternate Function mode
    GPIOA->MODER &= ~(3U << (2 * 2)); // Clear PA2
    GPIOA->MODER |=  (2U << (2 * 2)); // Set PA2 to Alternate Function
    
    GPIOA->MODER &= ~(3U << (3 * 2)); // Clear PA3
    GPIOA->MODER |=  (2U << (3 * 2)); // Set PA3 to Alternate Function

    // 3. Set the specific alternate function to AF7 (USART2) for both pins
    GPIOA->AFR[0] &= ~(0xF << (2 * 4)); // Clear PA2 AF
    GPIOA->AFR[0] |=  (0x7 << (2 * 4)); // Set PA2 to AF7
    
    GPIOA->AFR[0] &= ~(0xF << (3 * 4)); // Clear PA3 AF
    GPIOA->AFR[0] |=  (0x7 << (3 * 4)); // Set PA3 to AF7

    // 4. Set the Baud Rate to 115200 (Clock: 16 MHz / Baud: 115200 = 139 = 0x8B)
    USART2->BRR = 0x008B; 

    // 5. Enable Transmitter, Receiver, and the overall USART2 module
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; //
}

/* --- Function to Transmit Data Over UART --- */
void UART2_Write(int ch) {
    // Wait until the Transmit Data Register is empty (TXE bit = 1)
    while (!(USART2->SR & USART_SR_TXE)) {}
    // Write character byte to data register
    USART2->DR = (ch & 0xFF); 
}

/* --- Function to Receive Data Over UART --- */
char UART2_Read(void) {
    // Wait until the Read Data Register Not Empty flag is tripped (RXNE bit = 1)
    while (!(USART2->SR & USART_SR_RXNE)) {}
    // Return data register value (automatically clears RXNE flag)
    return (char)(USART2->DR & 0xFF); 
}
