################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/w5500/Client.c \
../Core/Src/w5500/Server.c \
../Core/Src/w5500/SocketHandler.c \
../Core/Src/w5500/socket.c \
../Core/Src/w5500/w5500_chip.c \
../Core/Src/w5500/wizchip_conf.c 

OBJS += \
./Core/Src/w5500/Client.o \
./Core/Src/w5500/Server.o \
./Core/Src/w5500/SocketHandler.o \
./Core/Src/w5500/socket.o \
./Core/Src/w5500/w5500_chip.o \
./Core/Src/w5500/wizchip_conf.o 

C_DEPS += \
./Core/Src/w5500/Client.d \
./Core/Src/w5500/Server.d \
./Core/Src/w5500/SocketHandler.d \
./Core/Src/w5500/socket.d \
./Core/Src/w5500/w5500_chip.d \
./Core/Src/w5500/wizchip_conf.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/w5500/%.o: ../Core/Src/w5500/%.c Core/Src/w5500/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/IOLink" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Debug" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Filters" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PWM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PID" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalOut" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Encoder" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Modbus" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/pt-1.4" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500/W5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/EEPROM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalIn" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PCA9557" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/W25QXX" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Delay_us" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-w5500

clean-Core-2f-Src-2f-w5500:
	-$(RM) ./Core/Src/w5500/Client.d ./Core/Src/w5500/Client.o ./Core/Src/w5500/Server.d ./Core/Src/w5500/Server.o ./Core/Src/w5500/SocketHandler.d ./Core/Src/w5500/SocketHandler.o ./Core/Src/w5500/socket.d ./Core/Src/w5500/socket.o ./Core/Src/w5500/w5500_chip.d ./Core/Src/w5500/w5500_chip.o ./Core/Src/w5500/wizchip_conf.d ./Core/Src/w5500/wizchip_conf.o

.PHONY: clean-Core-2f-Src-2f-w5500

