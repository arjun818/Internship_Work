################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/bcc/bcc.c \
../Sources/bcc/bcc_communication.c \
../Sources/bcc/bcc_spi.c \
../Sources/bcc/bcc_tpl.c 

OBJS += \
./Sources/bcc/bcc.o \
./Sources/bcc/bcc_communication.o \
./Sources/bcc/bcc_spi.o \
./Sources/bcc/bcc_tpl.o 

C_DEPS += \
./Sources/bcc/bcc.d \
./Sources/bcc/bcc_communication.d \
./Sources/bcc/bcc_spi.d \
./Sources/bcc/bcc_tpl.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/bcc/%.o: ../Sources/bcc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/bcc/bcc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


