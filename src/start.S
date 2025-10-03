    .section .text.boot
    .global _start
    .extern __stack_top
    .extern __bss_start
    .extern __bss_end
    .extern kernel_main

_start:
    // Set SP to the top of RAM
    ldr   x0, =__stack_top
    mov   sp, x0

    // Clear .bss
    ldr   x1, =__bss_start
    ldr   x2, =__bss_end
1:  cmp   x1, x2
    b.hs  2f
    str   xzr, [x1], #8
    b     1b
2:
    bl    kernel_main

hang:
    wfe
    b     hang
