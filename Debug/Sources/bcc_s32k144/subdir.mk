################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/bcc_s32k144/bcc_peripheries.c \
../Sources/bcc_s32k144/bcc_wait.c 

OBJS += \
./Sources/bcc_s32k144/bcc_peripheries.o \
./Sources/bcc_s32k144/bcc_wait.o 

C_DEPS += \
./Sources/bcc_s32k144/bcc_peripheries.d \
./Sources/bcc_s32k144/bcc_wait.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/bcc_s32k144/%.o: ../Sources/bcc_s32k144/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/bcc_s32k144/bcc_peripheries.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


