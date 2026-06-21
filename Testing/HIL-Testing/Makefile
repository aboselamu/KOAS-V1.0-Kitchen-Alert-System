# --- Toolchain ---
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# --- Architecture Flags ---
ARCH_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

# --- C Compiler Flags ---
CFLAGS = $(ARCH_FLAGS) -Iinc/ -O0 -g3 -Wall -DSTM32F446xx -ffunction-sections -fdata-sections

# --- Linker Flags ---
LDFLAGS = $(ARCH_FLAGS) -T startup/STM32F446RETX_FLASH.ld --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections -Wl,-Map=main.map -static

# --- Targets ---
all: main.bin
	$(SIZE) main.elf

# 1. Compile C files (Your original working explicit rule)
src/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

# 2. Compile ASM files (Your original working explicit rule handling the name mismatch)
startup/startup.o: startup/startup_stm32f446xx.s
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

# 3. Link
main.elf: src/main.o startup/startup.o
	$(CC) $(LDFLAGS) $^ -o $@

main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f src/*.o startup/*.o *.elf *.bin *.map

# --- Flash Target ---
flash: main.elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program main.elf verify reset exit"