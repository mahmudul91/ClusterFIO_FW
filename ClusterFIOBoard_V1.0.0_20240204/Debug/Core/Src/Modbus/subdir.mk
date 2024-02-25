################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Modbus/MB_ACL.c \
../Core/Src/Modbus/MB_CRC.c \
../Core/Src/Modbus/MB_Custom.c \
../Core/Src/Modbus/MB_Diagnostics.c \
../Core/Src/Modbus/MB_Gateway.c \
../Core/Src/Modbus/MB_Handler.c \
../Core/Src/Modbus/MB_MsgQ.c \
../Core/Src/Modbus/MB_RTUMaster.c \
../Core/Src/Modbus/MB_RTUSlave.c \
../Core/Src/Modbus/MB_Register.c \
../Core/Src/Modbus/MB_Serial.c \
../Core/Src/Modbus/MB_TCP.c \
../Core/Src/Modbus/MB_TCPClient.c \
../Core/Src/Modbus/MB_TCPServer.c \
../Core/Src/Modbus/MB_Tools.c \
../Core/Src/Modbus/MB_UART.c 

OBJS += \
./Core/Src/Modbus/MB_ACL.o \
./Core/Src/Modbus/MB_CRC.o \
./Core/Src/Modbus/MB_Custom.o \
./Core/Src/Modbus/MB_Diagnostics.o \
./Core/Src/Modbus/MB_Gateway.o \
./Core/Src/Modbus/MB_Handler.o \
./Core/Src/Modbus/MB_MsgQ.o \
./Core/Src/Modbus/MB_RTUMaster.o \
./Core/Src/Modbus/MB_RTUSlave.o \
./Core/Src/Modbus/MB_Register.o \
./Core/Src/Modbus/MB_Serial.o \
./Core/Src/Modbus/MB_TCP.o \
./Core/Src/Modbus/MB_TCPClient.o \
./Core/Src/Modbus/MB_TCPServer.o \
./Core/Src/Modbus/MB_Tools.o \
./Core/Src/Modbus/MB_UART.o 

C_DEPS += \
./Core/Src/Modbus/MB_ACL.d \
./Core/Src/Modbus/MB_CRC.d \
./Core/Src/Modbus/MB_Custom.d \
./Core/Src/Modbus/MB_Diagnostics.d \
./Core/Src/Modbus/MB_Gateway.d \
./Core/Src/Modbus/MB_Handler.d \
./Core/Src/Modbus/MB_MsgQ.d \
./Core/Src/Modbus/MB_RTUMaster.d \
./Core/Src/Modbus/MB_RTUSlave.d \
./Core/Src/Modbus/MB_Register.d \
./Core/Src/Modbus/MB_Serial.d \
./Core/Src/Modbus/MB_TCP.d \
./Core/Src/Modbus/MB_TCPClient.d \
./Core/Src/Modbus/MB_TCPServer.d \
./Core/Src/Modbus/MB_Tools.d \
./Core/Src/Modbus/MB_UART.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Modbus/%.o: ../Core/Src/Modbus/%.c Core/Src/Modbus/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/IOLink" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Debug" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Filters" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PWM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PID" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalOut" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Encoder" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Modbus" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/pt-1.4" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500/W5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/EEPROM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalIn" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PCA9557" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/W25QXX" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Delay_us" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Modbus

clean-Core-2f-Src-2f-Modbus:
	-$(RM) ./Core/Src/Modbus/MB_ACL.d ./Core/Src/Modbus/MB_ACL.o ./Core/Src/Modbus/MB_CRC.d ./Core/Src/Modbus/MB_CRC.o ./Core/Src/Modbus/MB_Custom.d ./Core/Src/Modbus/MB_Custom.o ./Core/Src/Modbus/MB_Diagnostics.d ./Core/Src/Modbus/MB_Diagnostics.o ./Core/Src/Modbus/MB_Gateway.d ./Core/Src/Modbus/MB_Gateway.o ./Core/Src/Modbus/MB_Handler.d ./Core/Src/Modbus/MB_Handler.o ./Core/Src/Modbus/MB_MsgQ.d ./Core/Src/Modbus/MB_MsgQ.o ./Core/Src/Modbus/MB_RTUMaster.d ./Core/Src/Modbus/MB_RTUMaster.o ./Core/Src/Modbus/MB_RTUSlave.d ./Core/Src/Modbus/MB_RTUSlave.o ./Core/Src/Modbus/MB_Register.d ./Core/Src/Modbus/MB_Register.o ./Core/Src/Modbus/MB_Serial.d ./Core/Src/Modbus/MB_Serial.o ./Core/Src/Modbus/MB_TCP.d ./Core/Src/Modbus/MB_TCP.o ./Core/Src/Modbus/MB_TCPClient.d ./Core/Src/Modbus/MB_TCPClient.o ./Core/Src/Modbus/MB_TCPServer.d ./Core/Src/Modbus/MB_TCPServer.o ./Core/Src/Modbus/MB_Tools.d ./Core/Src/Modbus/MB_Tools.o ./Core/Src/Modbus/MB_UART.d ./Core/Src/Modbus/MB_UART.o

.PHONY: clean-Core-2f-Src-2f-Modbus

