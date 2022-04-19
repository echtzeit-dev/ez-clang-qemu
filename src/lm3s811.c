#include "device.h"

//
// Device specific defines for the LM3S811
//

// Base addresses of memories and peripherals
#define FLASH_BASE              0x00000000  // FLASH memory
#define SRAM_BASE               0x20000000  // SRAM memory
#define SRAM_SIZE               0x00002000  // 8KB
#define WATCHDOG0_BASE          0x40000000  // Watchdog0
#define WATCHDOG1_BASE          0x40001000  // Watchdog1
#define GPIO_PORTA_BASE         0x40004000  // GPIO Port A
#define GPIO_PORTB_BASE         0x40005000  // GPIO Port B
#define GPIO_PORTC_BASE         0x40006000  // GPIO Port C
#define GPIO_PORTD_BASE         0x40007000  // GPIO Port D
#define SSI0_BASE               0x40008000  // SSI0
#define SSI1_BASE               0x40009000  // SSI1
#define UART0_BASE              0x4000C000  // UART0
#define UART1_BASE              0x4000D000  // UART1
#define UART2_BASE              0x4000E000  // UART2
#define I2C0_MASTER_BASE        0x40020000  // I2C0 Master
#define I2C0_SLAVE_BASE         0x40020800  // I2C0 Slave
#define I2C1_MASTER_BASE        0x40021000  // I2C1 Master
#define I2C1_SLAVE_BASE         0x40021800  // I2C1 Slave
#define GPIO_PORTE_BASE         0x40024000  // GPIO Port E
#define GPIO_PORTF_BASE         0x40025000  // GPIO Port F
#define GPIO_PORTG_BASE         0x40026000  // GPIO Port G
#define GPIO_PORTH_BASE         0x40027000  // GPIO Port H
#define PWM_BASE                0x40028000  // PWM
#define QEI0_BASE               0x4002C000  // QEI0
#define QEI1_BASE               0x4002D000  // QEI1
#define TIMER0_BASE             0x40030000  // Timer0
#define TIMER1_BASE             0x40031000  // Timer1
#define TIMER2_BASE             0x40032000  // Timer2
#define TIMER3_BASE             0x40033000  // Timer3
#define ADC0_BASE               0x40038000  // ADC0
#define ADC1_BASE               0x40039000  // ADC1
#define COMP_BASE               0x4003C000  // Analog comparators
#define GPIO_PORTJ_BASE         0x4003D000  // GPIO Port J
#define CAN0_BASE               0x40040000  // CAN0
#define CAN1_BASE               0x40041000  // CAN1
#define CAN2_BASE               0x40042000  // CAN2
#define ETH_BASE                0x40048000  // Ethernet
#define MAC_BASE                0x40048000  // Ethernet
#define USB0_BASE               0x40050000  // USB 0 Controller
#define I2S0_BASE               0x40054000  // I2S0
#define GPIO_PORTA_AHB_BASE     0x40058000  // GPIO Port A (high speed)
#define GPIO_PORTB_AHB_BASE     0x40059000  // GPIO Port B (high speed)
#define GPIO_PORTC_AHB_BASE     0x4005A000  // GPIO Port C (high speed)
#define GPIO_PORTD_AHB_BASE     0x4005B000  // GPIO Port D (high speed)
#define GPIO_PORTE_AHB_BASE     0x4005C000  // GPIO Port E (high speed)
#define GPIO_PORTF_AHB_BASE     0x4005D000  // GPIO Port F (high speed)
#define GPIO_PORTG_AHB_BASE     0x4005E000  // GPIO Port G (high speed)
#define GPIO_PORTH_AHB_BASE     0x4005F000  // GPIO Port H (high speed)
#define GPIO_PORTJ_AHB_BASE     0x40060000  // GPIO Port J (high speed)
#define EPI0_BASE               0x400D0000  // EPI0
#define HIB_BASE                0x400FC000  // Hibernation Module
#define FLASH_CTRL_BASE         0x400FD000  // FLASH Controller
#define SYSCTL_BASE             0x400FE000  // System Control
#define UDMA_BASE               0x400FF000  // uDMA Controller
#define ITM_BASE                0xE0000000  // Instrumentation Trace Macrocell
#define DWT_BASE                0xE0001000  // Data Watchpoint and Trace
#define FPB_BASE                0xE0002000  // FLASH Patch and Breakpoint
#define NVIC_BASE               0xE000E000  // Nested Vectored Interrupt Ctrl
#define TPIU_BASE               0xE0040000  // Trace Port Interface Unit

// Bit fields in SYSCTL_RCGC1 register
#define SYSCTL_RCGC1_EPI0       0x40000000  // EPI0 Clock Gating
#define SYSCTL_RCGC1_I2S0       0x10000000  // I2S0 Clock Gating Control
#define SYSCTL_RCGC1_COMP2      0x04000000  // Analog Comparator 2 Clock Gating
#define SYSCTL_RCGC1_COMP1      0x02000000  // Analog Comparator 1 Clock Gating
#define SYSCTL_RCGC1_COMP0      0x01000000  // Analog Comparator 0 Clock Gating
#define SYSCTL_RCGC1_TIMER3     0x00080000  // Timer 3 Clock Gating Control
#define SYSCTL_RCGC1_TIMER2     0x00040000  // Timer 2 Clock Gating Control
#define SYSCTL_RCGC1_TIMER1     0x00020000  // Timer 1 Clock Gating Control
#define SYSCTL_RCGC1_TIMER0     0x00010000  // Timer 0 Clock Gating Control
#define SYSCTL_RCGC1_I2C1       0x00004000  // I2C1 Clock Gating Control
#define SYSCTL_RCGC1_I2C0       0x00001000  // I2C0 Clock Gating Control
#define SYSCTL_RCGC1_QEI1       0x00000200  // QEI1 Clock Gating Control
#define SYSCTL_RCGC1_QEI0       0x00000100  // QEI0 Clock Gating Control
#define SYSCTL_RCGC1_SSI1       0x00000020  // SSI1 Clock Gating Control
#define SYSCTL_RCGC1_SSI0       0x00000010  // SSI0 Clock Gating Control
#define SYSCTL_RCGC1_UART2      0x00000004  // UART2 Clock Gating Control
#define SYSCTL_RCGC1_UART1      0x00000002  // UART1 Clock Gating Control
#define SYSCTL_RCGC1_UART0      0x00000001  // UART0 Clock Gating Control

// Bit fields in SYSCTL_RCGC2 register
#define SYSCTL_RCGC2_EPHY0      0x40000000  // PHY0 Clock Gating Control
#define SYSCTL_RCGC2_EMAC0      0x10000000  // MAC0 Clock Gating Control
#define SYSCTL_RCGC2_USB0       0x00010000  // USB0 Clock Gating Control
#define SYSCTL_RCGC2_UDMA       0x00002000  // UDMA Clock Gating Control
#define SYSCTL_RCGC2_GPIOJ      0x00000100  // Port J Clock Gating Control
#define SYSCTL_RCGC2_GPIOH      0x00000080  // Port H Clock Gating Control
#define SYSCTL_RCGC2_GPIOG      0x00000040  // Port G Clock Gating Control
#define SYSCTL_RCGC2_GPIOF      0x00000020  // Port F Clock Gating Control
#define SYSCTL_RCGC2_GPIOE      0x00000010  // Port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOD      0x00000008  // Port D Clock Gating Control
#define SYSCTL_RCGC2_GPIOC      0x00000004  // Port C Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // Port B Clock Gating Control
#define SYSCTL_RCGC2_GPIOA      0x00000001  // Port A Clock Gating Control

// UART Register offsets
#define UART_O_DR               0x00000000  // Data Register
#define UART_O_RSR              0x00000004  // Receive Status Register (read)
#define UART_O_ECR              0x00000004  // Error Clear Register (write)
#define UART_O_FR               0x00000018  // Flag Register (read only)
#define UART_O_ILPR             0x00000020  // UART IrDA Low-Power Register
#define UART_O_IBRD             0x00000024  // Integer Baud Rate Divisor Reg
#define UART_O_FBRD             0x00000028  // Fractional Baud Rate Divisor Reg
#define UART_O_LCRH             0x0000002C  // UART Line Control
#define UART_O_CTL              0x00000030  // Control Register
#define UART_O_IFLS             0x00000034  // Interrupt FIFO Level Select Reg
#define UART_O_IM               0x00000038  // Interrupt Mask Set/Clear Reg
#define UART_O_RIS              0x0000003C  // Raw Interrupt Status Register
#define UART_O_MIS              0x00000040  // Masked Interrupt Status Register
#define UART_O_ICR              0x00000044  // Interrupt Clear Register
#define UART_O_DMACTL           0x00000048  // UART DMA Control
#define UART_O_LCTL             0x00000090  // UART LIN Control
#define UART_O_LSS              0x00000094  // UART LIN Snap Shot
#define UART_O_LTIM             0x00000098  // UART LIN Timer

// Bit fields in UART_O_LCRH register
#define UART_LCRH_SPS           0x00000080  // UART Stick Parity Select
#define UART_LCRH_WLEN_M        0x00000060  // UART Word Length
#define UART_LCRH_WLEN_5        0x00000000  // 5 bits (default)
#define UART_LCRH_WLEN_6        0x00000020  // 6 bits
#define UART_LCRH_WLEN_7        0x00000040  // 7 bits
#define UART_LCRH_WLEN_8        0x00000060  // 8 bits
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_LCRH_STP2          0x00000008  // UART Two Stop Bits Select
#define UART_LCRH_EPS           0x00000004  // UART Even Parity Select
#define UART_LCRH_PEN           0x00000002  // UART Parity Enable
#define UART_LCRH_BRK           0x00000001  // UART Send Break

// Bit fields in UART_O_FR register
#define UART_FR_RI              0x00000100  // Ring Indicator
#define UART_FR_TXFE            0x00000080  // TX FIFO Empty
#define UART_FR_RXFF            0x00000040  // RX FIFO Full
#define UART_FR_TXFF            0x00000020  // TX FIFO Full
#define UART_FR_RXFE            0x00000010  // RX FIFO Empty
#define UART_FR_BUSY            0x00000008  // UART Busy
#define UART_FR_DCD             0x00000004  // Data Carrier Detect
#define UART_FR_DSR             0x00000002  // Data Set Ready
#define UART_FR_CTS             0x00000001  // Clear To Send

// Helpers for handling addresses as volatile pointers
#define REG8(Base, Offset) ((volatile uint8_t *const)(Base + Offset))
#define REG32(Base, Offset) ((volatile uint32_t *const)(Base + Offset))

// UART Data, Flag and Line Control Registers
#define UART_DR REG8(UART0_BASE, UART_O_DR)
#define UART_FR REG32(UART0_BASE, UART_O_FR)
#define UART_LCRH REG32(UART0_BASE, UART_O_LCRH)

// Run Mode Clock Gating Control Registers
#define SYSCTRL_RCGC1 REG32(SYSCTL_BASE, 0x104)
#define SYSCTRL_RCGC2 REG32(SYSCTL_BASE, 0x108)

//
// Implemention of the device interface
//
void setupSendReceive() {
  // Enable clocks for UART0 and GPIO A
  *SYSCTRL_RCGC1 |= SYSCTL_RCGC1_UART0;
  *SYSCTRL_RCGC2 |= SYSCTL_RCGC2_GPIOA;

  // Set UART package payload 8bit and enable FIFO mode
  *UART_LCRH = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

  // Wait for FIFO while UART busy, then discard any existing data
  while ((*UART_FR & UART_FR_BUSY) != 0);
  for (uint32_t i = 0; i < 10000; i += 1)
    *UART_DR;
}

void sendBytes(const char *Buffer, uint32_t Size) {
  // Wait for FIFO while UART busy
  while ((*UART_FR & UART_FR_BUSY) != 0);
  for (uint32_t i = 0; i < Size; i += 1) {
    // Wait for capacity while FIFO full
    while ((*UART_FR & UART_FR_TXFF) != 0);
    *UART_DR = Buffer[i];
  }
}

bool receiveBytes(char Buffer[], uint32_t Size) {
  // Wait for FIFO while UART busy
  while ((*UART_FR & UART_FR_BUSY) != 0);
  for (uint32_t i = 0; i < Size; i += 1) {
    // Wait for data while FIFO empty
    while ((*UART_FR & UART_FR_RXFE) != 0);
    Buffer[i] = *UART_DR;
  }
  return (bool)1;
}

//
// Device startup code for the LM3S811
//
extern int main();

extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

void ResetHandler(void) {
  // Copy the data segment initializers from flash to SRAM. All addresses
  // provided from linker scripts are 4-byte aligned.
  unsigned long *Src = &_etext;
  unsigned long *Dst = &_data;
  while (Dst < &_edata)
    *Dst++ = *Src++;

  // Zero fill bss segment
  __asm("    ldr     r0, =_bss\n"
        "    ldr     r1, =_ebss\n"
        "    mov     r2, #0\n"
        "    .thumb_func\n"
        "zero_loop:\n"
        "        cmp     r0, r1\n"
        "        it      lt\n"
        "        strlt   r2, [r0], #4\n"
        "        blt     zero_loop");

  // Call main entry point
  main();
}

// Non-maskable interrupts are typically used for debugging
static void NmiHandler() {
  while (1)
    ;
}

// Hard faults are errors like permission violations, nullptr dereference, etc.
static void HardFaultHandler() {
  hardFault();
}

// Application code can override this
 __attribute__((weak))
void hardFault() {
  while (1)
    ;
}

// End up at physical address 0x0000.0000
__attribute__ ((section(".isr_vector")))
void (* const VectorTable[])(void) = {
  (void (*)(void))(SRAM_BASE + SRAM_SIZE),  // Initial stack pointer
  ResetHandler,
  NmiHandler,
  HardFaultHandler,
};
