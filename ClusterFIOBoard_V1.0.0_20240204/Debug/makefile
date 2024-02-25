################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32H7xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/w5500/W5500/subdir.mk
-include Core/Src/w5500/subdir.mk
-include Core/Src/W25QXX/subdir.mk
-include Core/Src/PWM/subdir.mk
-include Core/Src/PID/subdir.mk
-include Core/Src/PCA9557/subdir.mk
-include Core/Src/Modbus/subdir.mk
-include Core/Src/MAC/subdir.mk
-include Core/Src/IOLink/subdir.mk
-include Core/Src/Filters/subdir.mk
-include Core/Src/Encoder/subdir.mk
-include Core/Src/EEPROM/subdir.mk
-include Core/Src/DigitalOut/subdir.mk
-include Core/Src/DigitalIn/subdir.mk
-include Core/Src/Delay_us/subdir.mk
-include Core/Src/Debug/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ClusterFIOBoard_V1.0.0_20240204
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
ClusterFIOBoard_V1.0.0_20240204.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
ClusterFIOBoard_V1.0.0_20240204.list \

OBJCOPY_HEX += \
ClusterFIOBoard_V1.0.0_20240204.hex \

OBJCOPY_BIN += \
ClusterFIOBoard_V1.0.0_20240204.bin \


# All Target
all: main-build

# Main-build Target
main-build: ClusterFIOBoard_V1.0.0_20240204.elf secondary-outputs

# Tool invocations
ClusterFIOBoard_V1.0.0_20240204.elf: $(OBJS) $(USER_OBJS) D:\Work\AClusterLLC\Projects\ClusterFIOBoard\FW\ClusterFIOBoard_V1.0.0_20240204\STM32H723ZGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "ClusterFIOBoard_V1.0.0_20240204.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"D:\Work\AClusterLLC\Projects\ClusterFIOBoard\FW\ClusterFIOBoard_V1.0.0_20240204\STM32H723ZGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="ClusterFIOBoard_V1.0.0_20240204.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

ClusterFIOBoard_V1.0.0_20240204.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "ClusterFIOBoard_V1.0.0_20240204.list"
	@echo 'Finished building: $@'
	@echo ' '

ClusterFIOBoard_V1.0.0_20240204.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "ClusterFIOBoard_V1.0.0_20240204.hex"
	@echo 'Finished building: $@'
	@echo ' '

ClusterFIOBoard_V1.0.0_20240204.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "ClusterFIOBoard_V1.0.0_20240204.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ClusterFIOBoard_V1.0.0_20240204.bin ClusterFIOBoard_V1.0.0_20240204.elf ClusterFIOBoard_V1.0.0_20240204.hex ClusterFIOBoard_V1.0.0_20240204.list default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
