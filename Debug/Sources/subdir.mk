################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/common.c \
../Sources/main.c \
../Sources/monitoring.c 

OBJS += \
./Sources/common.o \
./Sources/main.o \
./Sources/monitoring.o 

C_DEPS += \
./Sources/common.d \
./Sources/main.d \
./Sources/monitoring.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/common.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


