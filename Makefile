# Default to Clang + LLD cross build. Override CC to GCC cross if you want:
#   make CC=aarch64-none-elf-gcc
CC ?= clang

# Common compile flags (AArch64, freestanding)
CFLAGS  := -Wall -Wextra -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic \
           --target=aarch64-unknown-elf -mcpu=cortex-a53 -mstrict-align

# Linker flags (note: target + lld are needed at LINK time!)
LDFLAGS := -nostdlib -nostartfiles -static -Wl,-T,ld/kernel.ld -fuse-ld=lld --target=aarch64-unknown-elf
# Optional: map file for debugging
LDFLAGS += -Wl,-Map,kernel.map

SRC_C   := src/main.c src/uart.c
SRC_S   := src/start.S
OBJ     := $(SRC_C:.c=.o) $(SRC_S:.S=.o)

all: kernel.elf

# Compile C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile ASM
%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

# Link
kernel.elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

clean:
	rm -f $(OBJ) kernel.elf kernel.map
