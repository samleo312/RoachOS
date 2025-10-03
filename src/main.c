#include "uart.h"

#define LINE_MAX 128

static void echo_backspace(void) {
    uart_putc('\b'); uart_putc(' '); uart_putc('\b');
}

static int is_newline(char c) {
    return (c == '\r') || (c == '\n');
}

static int uart_readline(char *buf, int maxlen) {
    int i = 0;
    for (;;) {
        char c = uart_getc();

        // Handle Enter (CR, LF, or CRLF)
        if (is_newline(c)) {
            uart_putc('\r'); uart_putc('\n');

            buf[i] = '\0';
            return i;                 // line complete
        }

        // Handle backspace (DEL 0x7F or BS 0x08)
        if (c == 0x7F || c == 0x08) {
            if (i > 0) {
                i--; 
                echo_backspace(); 
            }
            continue;
        }

        // Ignore other control chars
        if ((unsigned char)c < 0x20) {
            continue;
        }

        // Add normal char if space remains
        if (i < maxlen - 1) {
            buf[i++] = c;
            uart_putc(c);            // echo
        } else {
            // Optional: bell on overflow
            uart_putc('\a');
        }
    }
}

static void echo_char(char c) {
    if (c == '\r' || c == '\n') {
        // Normalize newlines to CRLF so terminals behave
        uart_putc('\r');
        uart_putc('\n');
    } else if (c == 0x7F || c == 0x08) {
        // Backspace or DEL: move cursor left, erase, move left again
        uart_putc('\b');
        uart_putc(' ');
        uart_putc('\b');
    } else {
        // Normal character
        uart_putc(c);
    }
}

void kernel_main(void) {
    uart_init();
    uart_puts("\n-- RoachOS v0.1 --\n");

    char line[LINE_MAX];
    for (;;) {
        uart_puts("> ");
        int n = uart_readline(line, LINE_MAX);
        uart_puts("You typed: ");
        uart_puts(line);
        uart_putc('\r'); uart_putc('\n');
    }
}

