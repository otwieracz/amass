######################################
## Basic configuration
TARGET_NAME = stm32f3+freertos
BUILD_DIR   = build
# Flash offset 
FLASH       = 0x8000000
# GDB listen port
GDB_PORT    = 4242
# DEBUG flag
DEBUG       = 1
# Optimization
OPT         = -O0


#######################################
# Toolchain configuration
CROSS      = /usr/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-
CC         = $(CROSS)gcc
AS         = $(CROSS)gcc -x assembler-with-cpp
CP         = $(CROSS)objcopy
AR         = $(CROSS)ar
SZ         = $(CROSS)size
GDB        = $(CROSS)gdb
HEX        = $(CP) -O ihex
BIN        = $(CP) -O binary
STFLASH    = st-flash
STUTIL     = st-util
START_STOP = start-stop-daemon

######################################
## Toolchain flags
ASFLAGS   = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -Wall -fdata-sections -ffunction-sections
CFLAGS    = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -Wall -fdata-sections -ffunction-sections
# Generate dependency information
CFLAGS   += -MD -MP -MF .dep/$(@F).d

# Linker config
# ARM linker script
LDSCRIPT  = arm-gcc-link.ld
LIBS      = -lc -lm -lnosys
LIBDIR    =
LDFLAGS   = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET_NAME).map,--cref -Wl,--gc-sections

######################################
# Source configuration
SRCS = \
  Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_cortex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dma.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_uart.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim_ex.c \
  Drivers/BSP/STM32F3-Discovery/stm32f3_discovery.c \
  Drivers/BSP/STM32F3-Discovery/stm32f3_discovery_gyroscope.c \
  Drivers/BSP/STM32F3-Discovery/stm32f3_discovery_accelerometer.c \
  Drivers/BSP/Components/l3gd20/l3gd20.c \
  Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.c \
  Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
  Middlewares/Third_Party/FreeRTOS/Source/list.c \
  Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
  Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_1.c \
  Middlewares/Third_Party/FreeRTOS/Source/queue.c \
  Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
  Middlewares/Third_Party/FreeRTOS/Source/timers.c \
  Middlewares/Third_Party/FreeRTOS-Plus-CLI/FreeRTOS_CLI.c \
  Src/hardware/stm32f3xx_hal_msp.c \
  Src/hardware/stm32f3xx_it.c \
  Src/hardware/hardware.c \
  Src/hardware/uart.c \
  Src/task/heartbeat.c \
  Src/task/cli.c \
  Src/error.c \
  Src/main.c \

ASM_SOURCES = \
  Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s

######################################
# Include directories
AS_INCLUDES = -I../../Inc
C_INCLUDES  = -ISrc
C_INCLUDES += -IInc
C_INCLUDES += -ISrc/hardware
C_INCLUDES += -IDrivers/STM32F3xx_HAL_Driver/Inc
C_INCLUDES += -IDrivers/BSP/STM32F3-Discovery
C_INCLUDES += -IDrivers/BSP/Components
C_INCLUDES += -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
C_INCLUDES += -IMiddlewares/Third_Party/FreeRTOS/Source/include
C_INCLUDES += -IMiddlewares/Third_Party/FreeRTOS-Plus-CLI
C_INCLUDES += -IDrivers/CMSIS/Include
C_INCLUDES += -IDrivers/CMSIS/Device/ST/STM32F3xx/Include

## Source code modules
# Enable UART
CFLAGS   += -DHAL_UART_MODULE_ENABLED

#######################################
# End of user configuration.
# You probably do not want to change
# anything more.
#######################################

# macros for gcc
AS_DEFS  =
C_DEFS   = -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F303xC

# Debug support
ifeq ($(DEBUG), 1)
CFLAGS  += -g -gdwarf-2
endif

# final CFLAGS
ASFLAGS += $(AS_DEFS) $(AS_INCLUDES) $(OPT)
CFLAGS  += $(C_DEFS) $(C_INCLUDES) $(OPT)

# default action: build all
all: $(BUILD_DIR)/$(TARGET_NAME).elf $(BUILD_DIR)/$(TARGET_NAME).hex $(BUILD_DIR)/$(TARGET_NAME).bin

#######################################
# Build rules
OBJS  = $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.c=.o)))
vpath %.c $(sort $(dir $(SRCS)))
OBJS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET_NAME).elf: $(OBJS) Makefile
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	

$(BUILD_DIR):
	mkdir -p $@		

# Device flashing
flash: $(BUILD_DIR)/$(TARGET_NAME).bin
	$(STFLASH) write $< $(FLASH)

burn: flash

# Debugging
gdb: $(BUILD_DIR)/$(TARGET_NAME).elf
	rm -f $(BUILD_DIR)/st-util.pid
	$(START_STOP) -b -m -p $(BUILD_DIR)/st-util.pid $(STUTIL)
	$(GDB) -ex "target remote localhost:$(GDB_PORT)" -ex "load" $<; $(START_STOP) -K -p $(BUILD_DIR)/st-util.pid; echo "st-util should be stopped"

#  Clean up
clean:
	-rm -fR .dep $(BUILD_DIR)

# Dependencies
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

