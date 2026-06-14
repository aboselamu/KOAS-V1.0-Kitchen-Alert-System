# =============================================================================
# Makefile — STM32F103C8T6 Blue Pill Bare-Metal
# Stage 1: Button EXTI Test
# =============================================================================

# --- Toolchain ---
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# --- Architecture flags for STM32F103 (Cortex-M3, no FPU) ---
ARCH_FLAGS = -mcpu=cortex-m3 -mthumb

# --- Compiler flags ---
CFLAGS = $(ARCH_FLAGS)       \
         -Iinc/              \
         -Itemp_cmsis/       \
         -O0                 \
         -g3                 \
         -Wall               \
         -Wextra             \
         -DSTM32F103xB

# --- Linker flags ---
LDFLAGS = $(ARCH_FLAGS)                          \
          -T startup/stm32f103c8tx_flash.ld      \
          --specs=nosys.specs                    \
          -Wl,--gc-sections                      \
          -static

# --- Source and object files ---
C_SRCS   = src/main.c
ASM_SRCS = startup/startup_stm32f103xb.s

C_OBJS   = src/main.o
ASM_OBJS = startup/startup.o

# =============================================================================
# Build targets
# =============================================================================

all: main.bin size

# Compile C source
src/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile startup assembly
startup/startup.o: startup/startup_stm32f103xb.s
	$(CC) $(ARCH_FLAGS) -x assembler-with-cpp -c $< -o $@

# Link
main.elf: $(C_OBJS) $(ASM_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Convert to binary
main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

# Print size of sections
size: main.elf
	$(SIZE) main.elf

# =============================================================================
# Flash target (ST-Link via OpenOCD)
# =============================================================================
flash: main.elf
	openocd -f interface/stlink.cfg \
	        -f target/stm32f1x.cfg  \
	        -c "program main.elf verify reset exit"

# =============================================================================
# Clean
# =============================================================================
clean:
	rm -f src/*.o startup/*.o *.elf *.bin
