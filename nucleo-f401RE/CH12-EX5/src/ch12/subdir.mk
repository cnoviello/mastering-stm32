################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ch12/main-ex5.c 

OBJS += \
./src/ch12/main-ex5.o 

C_DEPS += \
./src/ch12/main-ex5.d 


# Each subdirectory must supply rules for building sources it contributes
src/ch12/%.o: ../src/ch12/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wextra  -g3 -DDEBUG -DUART_RX -DOS_USE_SEMIHOSTING -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F401xE -I/Users/cnoviello/STM32Toolchain/mastering-stm32/nucleo-f401RE/include -I/Users/cnoviello/STM32Toolchain/mastering-stm32/nucleo-f401RE/system/include -I/Users/cnoviello/STM32Toolchain/mastering-stm32/nucleo-f401RE/system/include/cmsis -I/Users/cnoviello/STM32Toolchain/mastering-stm32/nucleo-f401RE/system/include/stm32f4xx -I/Users/cnoviello/STM32Toolchain/mastering-stm32/nucleo-f401RE/system/include/cmsis/device -I"../Middlewares/RingBuffer" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


