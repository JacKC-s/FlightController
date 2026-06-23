######################################################################
# Makefile — STM32F446RE (Nucleo-64) with FreeRTOS
######################################################################

# Generated

# Project name (output filenames)
TARGET = flight_controller

# Toolchain
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-gcc -x assembler-with-cpp
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE    = arm-none-eabi-size

# MCU flags  (Cortex-M4 with hardware FPU)
CPU   = -mcpu=cortex-m4
FPU   = -mfpu=fpv4-sp-d16
FLOAT = -mfloat-abi=hard
MCU   = $(CPU) -mthumb $(FPU) $(FLOAT)

######################################################################
# Source files
######################################################################

# C sources
C_SOURCES  = app/main.c
C_SOURCES += hal/gpio.c
C_SOURCES += cmsis/system_stm32f4xx.c
C_SOURCES += freertos/src/tasks.c
C_SOURCES += freertos/src/queue.c
C_SOURCES += freertos/src/list.c
C_SOURCES += freertos/src/timers.c
C_SOURCES += freertos/src/event_groups.c
C_SOURCES += freertos/src/stream_buffer.c
C_SOURCES += freertos/src/croutine.c
C_SOURCES += freertos/port/port.c
C_SOURCES += freertos/heap/heap_4.c

# Assembly sources
ASM_SOURCES = cmsis/startup_stm32f446xx.s

# Linker script
LDSCRIPT = linker/link.ld

######################################################################
# Include paths
######################################################################

C_INCLUDES  = -Icmsis
C_INCLUDES += -Iconfig
C_INCLUDES += -Ihal
C_INCLUDES += -Ifreertos/include
C_INCLUDES += -Ifreertos/port

######################################################################
# Compiler / Linker flags
######################################################################

# C defines — tells CMSIS which chip we are targeting
C_DEFS = -DSTM32F446xx

# Compiler flags
CFLAGS  = $(MCU) $(C_DEFS) $(C_INCLUDES)
CFLAGS += -Wall -Wextra
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -std=gnu11
CFLAGS += -g -gdwarf-2          # debug info for GDB

# Linker flags
LDFLAGS  = $(MCU)
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -Wl,--gc-sections    # remove unused sections
LDFLAGS += -Wl,-Map=build/$(TARGET).map,--cref
LDFLAGS += --specs=nano.specs   # use newlib-nano (smaller libc)
LDFLAGS += --specs=nosys.specs  # no semihosting
LDFLAGS += -lc -lm -lnosys

######################################################################
# Build directory
######################################################################

BUILD_DIR = build

######################################################################
# Object files
######################################################################

OBJECTS  = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))

# Create list of needed directories
OBJ_DIRS = $(sort $(dir $(OBJECTS)))

######################################################################
# Targets
######################################################################

.PHONY: all clean flash erase size

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex size

# ── Compile C files ────────────────────────────────────────────────
$(BUILD_DIR)/%.o: %.c | $(OBJ_DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# ── Assemble .s files ─────────────────────────────────────────────
$(BUILD_DIR)/%.o: %.s | $(OBJ_DIRS)
	$(AS) $(MCU) -c $< -o $@

# ── Link ───────────────────────────────────────────────────────────
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# ── Generate binary (for flashing) ────────────────────────────────
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# ── Generate Intel HEX (alternative flash format) ─────────────────
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# ── Create build directories ──────────────────────────────────────
$(OBJ_DIRS):
	mkdir -p $@

# ── Print size info ────────────────────────────────────────────────
size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<

# ── Flash via ST-Link (USB) ────────────────────────────────────────
flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000

# ── Erase chip ─────────────────────────────────────────────────────
erase:
	st-flash erase

# ── Clean build artifacts ──────────────────────────────────────────
clean:
	rm -rf $(BUILD_DIR)
