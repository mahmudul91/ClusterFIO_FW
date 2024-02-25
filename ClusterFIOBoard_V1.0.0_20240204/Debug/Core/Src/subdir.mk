################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ADC.c \
../Core/Src/AnalogIO.c \
../Core/Src/Callbacks.c \
../Core/Src/Diagnosis.c \
../Core/Src/EEPROM_StorageExt.c \
../Core/Src/Flash_Storage.c \
../Core/Src/Flash_StorageExt.c \
../Core/Src/IRQHandler.c \
../Core/Src/Instances.c \
../Core/Src/Scheduler.c \
../Core/Src/Settings.c \
../Core/Src/Task_Handler.c \
../Core/Src/TimeStamp.c \
../Core/Src/Tools.c \
../Core/Src/main.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c \
../Core/Src/w25Qxx_Interface.c 

OBJS += \
./Core/Src/ADC.o \
./Core/Src/AnalogIO.o \
./Core/Src/Callbacks.o \
./Core/Src/Diagnosis.o \
./Core/Src/EEPROM_StorageExt.o \
./Core/Src/Flash_Storage.o \
./Core/Src/Flash_StorageExt.o \
./Core/Src/IRQHandler.o \
./Core/Src/Instances.o \
./Core/Src/Scheduler.o \
./Core/Src/Settings.o \
./Core/Src/Task_Handler.o \
./Core/Src/TimeStamp.o \
./Core/Src/Tools.o \
./Core/Src/main.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o \
./Core/Src/w25Qxx_Interface.o 

C_DEPS += \
./Core/Src/ADC.d \
./Core/Src/AnalogIO.d \
./Core/Src/Callbacks.d \
./Core/Src/Diagnosis.d \
./Core/Src/EEPROM_StorageExt.d \
./Core/Src/Flash_Storage.d \
./Core/Src/Flash_StorageExt.d \
./Core/Src/IRQHandler.d \
./Core/Src/Instances.d \
./Core/Src/Scheduler.d \
./Core/Src/Settings.d \
./Core/Src/Task_Handler.d \
./Core/Src/TimeStamp.d \
./Core/Src/Tools.d \
./Core/Src/main.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d \
./Core/Src/w25Qxx_Interface.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/IOLink" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Debug" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Filters" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PWM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PID" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalOut" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Encoder" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Modbus" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/pt-1.4" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/w5500/W5500" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/EEPROM" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/DigitalIn" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/PCA9557" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/W25QXX" -I"D:/Work/AClusterLLC/Projects/ClusterFIOBoard/FW/ClusterFIOBoard_V1.0.0_20240204/Core/Src/Delay_us" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ADC.d ./Core/Src/ADC.o ./Core/Src/AnalogIO.d ./Core/Src/AnalogIO.o ./Core/Src/Callbacks.d ./Core/Src/Callbacks.o ./Core/Src/Diagnosis.d ./Core/Src/Diagnosis.o ./Core/Src/EEPROM_StorageExt.d ./Core/Src/EEPROM_StorageExt.o ./Core/Src/Flash_Storage.d ./Core/Src/Flash_Storage.o ./Core/Src/Flash_StorageExt.d ./Core/Src/Flash_StorageExt.o ./Core/Src/IRQHandler.d ./Core/Src/IRQHandler.o ./Core/Src/Instances.d ./Core/Src/Instances.o ./Core/Src/Scheduler.d ./Core/Src/Scheduler.o ./Core/Src/Settings.d ./Core/Src/Settings.o ./Core/Src/Task_Handler.d ./Core/Src/Task_Handler.o ./Core/Src/TimeStamp.d ./Core/Src/TimeStamp.o ./Core/Src/Tools.d ./Core/Src/Tools.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/w25Qxx_Interface.d ./Core/Src/w25Qxx_Interface.o

.PHONY: clean-Core-2f-Src

