################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main_s1_pg1_efr.c 

OBJS += \
./src/main_s1_pg1_efr.o 

C_DEPS += \
./src/main_s1_pg1_efr.d 


# Each subdirectory must supply rules for building sources it contributes
src/main_s1_pg1_efr.o: ../src/main_s1_pg1_efr.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -std=c99 '-DNDEBUG=1' -Os -Wall -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"src/main_s1_pg1_efr.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


