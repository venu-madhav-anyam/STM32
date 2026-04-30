################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim.c \
../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim_ex.c 

OBJS += \
./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim.o \
./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim_ex.o 

C_DEPS += \
./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim.d \
./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim_ex.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32L4xx_HAL_Driver/Inc/%.o Drivers/STM32L4xx_HAL_Driver/Inc/%.su Drivers/STM32L4xx_HAL_Driver/Inc/%.cyclo: ../Drivers/STM32L4xx_HAL_Driver/Inc/%.c Drivers/STM32L4xx_HAL_Driver/Inc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Inc -I"D:/STM32/17_HAL_RTC/Drivers/CMSIS/Include" -I"D:/STM32/17_HAL_RTC/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"D:/STM32/17_HAL_RTC/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-STM32L4xx_HAL_Driver-2f-Inc

clean-Drivers-2f-STM32L4xx_HAL_Driver-2f-Inc:
	-$(RM) ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim.cyclo ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim.d ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim.o ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim.su ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim_ex.cyclo ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim_ex.d ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim_ex.o ./Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_tim_ex.su

.PHONY: clean-Drivers-2f-STM32L4xx_HAL_Driver-2f-Inc

