################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ch12/main-ex1.c \
../src/ch12/nucleo_hal_bsp.c \
../src/ch12/stm32f1xx_hal_msp.c \
../src/ch12/stm32f1xx_it.c 

OBJS += \
./src/ch12/main-ex1.o \
./src/ch12/nucleo_hal_bsp.o \
./src/ch12/stm32f1xx_hal_msp.o \
./src/ch12/stm32f1xx_it.o 

C_DEPS += \
./src/ch12/main-ex1.d \
./src/ch12/nucleo_hal_bsp.d \
./src/ch12/stm32f1xx_hal_msp.d \
./src/ch12/stm32f1xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
src/ch12/%.o: ../src/ch12/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUART_RX -DOS_USE_SEMIHOSTING -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F103xB -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1xx" -I"../system/include/cmsis/device" -I"../Middlewares/RingBuffer" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


