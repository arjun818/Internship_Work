################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Sources/utils/subdir.mk
-include Sources/bcc_s32k144/subdir.mk
-include Sources/bcc/subdir.mk
-include Sources/subdir.mk
-include SDK/rtos/osif/subdir.mk
-include SDK/platform/drivers/src/pins/subdir.mk
-include SDK/platform/drivers/src/lpuart/subdir.mk
-include SDK/platform/drivers/src/lpspi/subdir.mk
-include SDK/platform/drivers/src/interrupt/subdir.mk
-include SDK/platform/drivers/src/edma/subdir.mk
-include SDK/platform/drivers/src/clock/S32K1xx/subdir.mk
-include SDK/platform/devices/S32K144/startup/subdir.mk
-include SDK/platform/devices/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include Generated_Code/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
BCC_S32K144_Monitoring.siz \


# All Target
all: BCC_S32K144_Monitoring.elf secondary-outputs

# Tool invocations
BCC_S32K144_Monitoring.elf: $(OBJS) G:/EMBEDDED-SW-BCC/BCC_SDK_SW/S32DS_Examples/BCC_S32K144_FOR_DRONE/Project_Settings/Linker_Files/S32K144_64_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "BCC_S32K144_Monitoring.elf" "@BCC_S32K144_Monitoring.args"  $(USER_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

BCC_S32K144_Monitoring.siz: BCC_S32K144_Monitoring.elf
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley BCC_S32K144_Monitoring.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
