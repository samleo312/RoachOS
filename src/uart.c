#include <stdint.h>

// QEMU 'virt' machine: PL011 UART at 0x09000000
#define UART0_BASE   0x09000000UL
#define UART_DR      ((volatile uint32_t*)(UART0_BASE + 0x00))
#define UART_FR      ((volatile uint32_t*)(UART0_BASE + 0x18))
#define UART_IBRD    ((volatile uint32_t*)(UART0_BASE + 0x24))
#define UART_FBRD    ((volatile uint32_t*)(UART0_BASE + 0x28))
#define UART_LCRH    ((volatile uint32_t*)(UART0_BASE + 0x2C))
#define UART_CR      ((volatile uint32_t*)(UART0_BASE + 0x30))
#define UART_IMSC    ((volatile uint32_t*)(UART0_BASE + 0x38))
#define UART_ICR     ((volatile uint32_t*)(UART0_BASE + 0x44))

void uart_init(void) {
    *UART_CR = 0;        // disable
    *UART_ICR = 0x7FF;   // clear interrupts

    // Reasonable 115200-ish setup (QEMU isn't strict here)
    *UART_IBRD = 26;
    *UART_FBRD = 3;

    // 8N1 + FIFO
    *UART_LCRH = (1u << 4) | (3u << 5); // FEN + WLEN=8 bits
    *UART_IMSC = 0;                     // mask all
    *UART_CR   = (1u << 0) | (1u << 8) | (1u << 9); // UARTEN | TXE | RXE
}

void uart_putc(char c) {
    // Wait until TX FIFO not full (FR bit5 = TXFF)
    while (*UART_FR & (1u << 5)) { }
    *UART_DR = (uint32_t)c;
}

void uart_puts(const char* s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

// Wait until RX FIFO not empty (FR bit 4 = RXFE), then read a byte
char uart_getc(void) {
    while (*UART_FR & (1u << 4)) { /* spin */ }
    return (char)(*UART_DR & 0xFF);
}

// Optional non-blocking version: returns 1 if a byte was read, 0 otherwise
int uart_trygetc(char *out) {
    if (*UART_FR & (1u << 4)) return 0;       // RXFE set: empty
    *out = (char)(*UART_DR & 0xFF);
    return 1;
}

