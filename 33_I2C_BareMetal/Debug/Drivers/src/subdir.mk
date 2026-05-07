################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/src/stm32l476xx_gpio.c \
../Drivers/src/stm32l476xx_i2c.c \
../Drivers/src/stm32l476xx_spi.c 

OBJS += \
./Drivers/src/stm32l476xx_gpio.o \
./Drivers/src/stm32l476xx_i2c.o \
./Drivers/src/stm32l476xx_spi.o 

C_DEPS += \
./Drivers/src/stm32l476xx_gpio.d \
./Drivers/src/stm32l476xx_i2c.d \
./Drivers/src/stm32l476xx_spi.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/src/%.o Drivers/src/%.su Drivers/src/%.cyclo: ../Drivers/src/%.c Drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32L4 -DSTM32 -DSTM32L476RGTx -c -I../Inc -I"D:/STM32/33_I2C_BareMetal/Drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-src

clean-Drivers-2f-src:
	-$(RM) ./Drivers/src/stm32l476xx_gpio.cyclo ./Drivers/src/stm32l476xx_gpio.d ./Drivers/src/stm32l476xx_gpio.o ./Drivers/src/stm32l476xx_gpio.su ./Drivers/src/stm32l476xx_i2c.cyclo ./Drivers/src/stm32l476xx_i2c.d ./Drivers/src/stm32l476xx_i2c.o ./Drivers/src/stm32l476xx_i2c.su ./Drivers/src/stm32l476xx_spi.cyclo ./Drivers/src/stm32l476xx_spi.d ./Drivers/src/stm32l476xx_spi.o ./Drivers/src/stm32l476xx_spi.su

.PHONY: clean-Drivers-2f-src

