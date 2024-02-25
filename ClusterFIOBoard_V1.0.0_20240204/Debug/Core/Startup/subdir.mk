################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32h723zgtx.s 

OBJS += \
./Core/Startup/startup_stm32h723zgtx.o 

S_DEPS += \
./Core/Startup/startup_stm32h723zgtx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -DDEBUG -c -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/IOLink" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Debug" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Filters" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PWM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PID" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalOut" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Encoder" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Modbus" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/pt-1.4" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500/W5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/EEPROM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalIn" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PCA9557" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/W25QXX" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Delay_us" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32h723zgtx.d ./Core/Startup/startup_stm32h723zgtx.o

.PHONY: clean-Core-2f-Startup

