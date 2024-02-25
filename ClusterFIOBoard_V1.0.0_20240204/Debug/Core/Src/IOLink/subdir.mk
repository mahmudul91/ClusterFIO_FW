################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/IOLink/IOLink.c \
../Core/Src/IOLink/IOLink_AL.c \
../Core/Src/IOLink/IOLink_DL.c \
../Core/Src/IOLink/IOLink_MA.c \
../Core/Src/IOLink/IOLink_PL.c \
../Core/Src/IOLink/IOLink_SM.c \
../Core/Src/IOLink/IOLink_SMI.c \
../Core/Src/IOLink/IOLink_Stm32Hw.c \
../Core/Src/IOLink/IOLink_Time.c \
../Core/Src/IOLink/IOLink_Utility.c \
../Core/Src/IOLink/Timer.c \
../Core/Src/IOLink/l6360.c 

OBJS += \
./Core/Src/IOLink/IOLink.o \
./Core/Src/IOLink/IOLink_AL.o \
./Core/Src/IOLink/IOLink_DL.o \
./Core/Src/IOLink/IOLink_MA.o \
./Core/Src/IOLink/IOLink_PL.o \
./Core/Src/IOLink/IOLink_SM.o \
./Core/Src/IOLink/IOLink_SMI.o \
./Core/Src/IOLink/IOLink_Stm32Hw.o \
./Core/Src/IOLink/IOLink_Time.o \
./Core/Src/IOLink/IOLink_Utility.o \
./Core/Src/IOLink/Timer.o \
./Core/Src/IOLink/l6360.o 

C_DEPS += \
./Core/Src/IOLink/IOLink.d \
./Core/Src/IOLink/IOLink_AL.d \
./Core/Src/IOLink/IOLink_DL.d \
./Core/Src/IOLink/IOLink_MA.d \
./Core/Src/IOLink/IOLink_PL.d \
./Core/Src/IOLink/IOLink_SM.d \
./Core/Src/IOLink/IOLink_SMI.d \
./Core/Src/IOLink/IOLink_Stm32Hw.d \
./Core/Src/IOLink/IOLink_Time.d \
./Core/Src/IOLink/IOLink_Utility.d \
./Core/Src/IOLink/Timer.d \
./Core/Src/IOLink/l6360.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/IOLink/%.o: ../Core/Src/IOLink/%.c Core/Src/IOLink/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/IOLink" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Debug" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Filters" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PWM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PID" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalOut" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Encoder" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Modbus" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/pt-1.4" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500/W5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/EEPROM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalIn" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PCA9557" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/W25QXX" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Delay_us" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-IOLink

clean-Core-2f-Src-2f-IOLink:
	-$(RM) ./Core/Src/IOLink/IOLink.d ./Core/Src/IOLink/IOLink.o ./Core/Src/IOLink/IOLink_AL.d ./Core/Src/IOLink/IOLink_AL.o ./Core/Src/IOLink/IOLink_DL.d ./Core/Src/IOLink/IOLink_DL.o ./Core/Src/IOLink/IOLink_MA.d ./Core/Src/IOLink/IOLink_MA.o ./Core/Src/IOLink/IOLink_PL.d ./Core/Src/IOLink/IOLink_PL.o ./Core/Src/IOLink/IOLink_SM.d ./Core/Src/IOLink/IOLink_SM.o ./Core/Src/IOLink/IOLink_SMI.d ./Core/Src/IOLink/IOLink_SMI.o ./Core/Src/IOLink/IOLink_Stm32Hw.d ./Core/Src/IOLink/IOLink_Stm32Hw.o ./Core/Src/IOLink/IOLink_Time.d ./Core/Src/IOLink/IOLink_Time.o ./Core/Src/IOLink/IOLink_Utility.d ./Core/Src/IOLink/IOLink_Utility.o ./Core/Src/IOLink/Timer.d ./Core/Src/IOLink/Timer.o ./Core/Src/IOLink/l6360.d ./Core/Src/IOLink/l6360.o

.PHONY: clean-Core-2f-Src-2f-IOLink

