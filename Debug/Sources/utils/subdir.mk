################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/utils/nxp_console.c \
../Sources/utils/nxp_console_adapter.c \
../Sources/utils/nxp_helpers.c 

OBJS += \
./Sources/utils/nxp_console.o \
./Sources/utils/nxp_console_adapter.o \
./Sources/utils/nxp_helpers.o 

C_DEPS += \
./Sources/utils/nxp_console.d \
./Sources/utils/nxp_console_adapter.d \
./Sources/utils/nxp_helpers.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/utils/%.o: ../Sources/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/utils/nxp_console.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


