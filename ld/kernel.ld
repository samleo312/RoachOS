ENTRY(_start)

/* Tell the linker where RAM is and how big it is.
   Use the same size you pass to QEMU with -m (default 128M). */
MEMORY {
  RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 128M
}

SECTIONS
{
  /* Place the image at the start of RAM */
  . = ORIGIN(RAM);

  /* Put the reset code (_start) here so it's first */
  .text : {
    KEEP(*(.text.boot))      /* e.g. start.S should use .section .text.boot */
    *(.text*)
    *(.rodata*)
  } > RAM

  .data : {
    *(.data*)
  } > RAM

  .bss : {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  } > RAM

  /* Heap begins right after BSS, aligned */
  __heap_start = ALIGN(__bss_end, 16);

  /* End of physical RAM and initial stack pointer */
  __ram_end   = ORIGIN(RAM) + LENGTH(RAM);
  __stack_top = __ram_end;    /* stack grows down from here */
}
