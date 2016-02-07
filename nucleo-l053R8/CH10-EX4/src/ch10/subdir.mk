################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ch10/main-ex4.c 

OBJS += \
./src/ch10/main-ex4.o 

C_DEPS += \
./src/ch10/main-ex4.d 


# Each subdirectory must supply rules for building sources it contributes
src/ch10/%.o: ../src/ch10/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wextra  -g3 -DDEBUG -DUART_RX -DOS_USE_SEMIHOSTING -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32L053xx -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l0xx" -I"../system/include/cmsis/device" -I"../Middlewares/RingBuffer" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


