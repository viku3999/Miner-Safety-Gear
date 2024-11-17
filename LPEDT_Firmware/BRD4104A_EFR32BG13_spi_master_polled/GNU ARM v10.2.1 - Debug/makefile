################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/EFR32BG13P/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := BRD4104A_EFR32BG13_spi_master_polled
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: BRD4104A_EFR32BG13_spi_master_polled.axf

# Tool invocations
BRD4104A_EFR32BG13_spi_master_polled.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "BRD4104A_EFR32BG13_spi_master_polled.ld" -Wl,--no-warn-rwx-segments -Xlinker --gc-sections -Xlinker -Map="BRD4104A_EFR32BG13_spi_master_polled.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o BRD4104A_EFR32BG13_spi_master_polled.axf "./CMSIS/EFR32BG13P/startup_efr32bg13p.o" "./CMSIS/EFR32BG13P/system_efr32bg13p.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_gpio.o" "./emlib/em_system.o" "./emlib/em_usart.o" "./src/main_s1_pg1_efr.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building out file: BRD4104A_EFR32BG13_spi_master_polled.out'
	arm-none-eabi-objcopy "BRD4104A_EFR32BG13_spi_master_polled.axf" "BRD4104A_EFR32BG13_spi_master_polled.out"
	@echo ' '

	@echo 'Building hex file: BRD4104A_EFR32BG13_spi_master_polled.hex'
	arm-none-eabi-objcopy -O ihex "BRD4104A_EFR32BG13_spi_master_polled.axf" "BRD4104A_EFR32BG13_spi_master_polled.hex"
	@echo ' '

	@echo 'Building bin file: BRD4104A_EFR32BG13_spi_master_polled.bin'
	arm-none-eabi-objcopy -O binary "BRD4104A_EFR32BG13_spi_master_polled.axf" "BRD4104A_EFR32BG13_spi_master_polled.bin"
	@echo ' '

	@echo 'Building s37 file: BRD4104A_EFR32BG13_spi_master_polled.s37'
	arm-none-eabi-objcopy -O srec "BRD4104A_EFR32BG13_spi_master_polled.axf" "BRD4104A_EFR32BG13_spi_master_polled.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "BRD4104A_EFR32BG13_spi_master_polled.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) BRD4104A_EFR32BG13_spi_master_polled.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
