################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex2/src/ex2.c 

OBJS += \
./ex2/src/ex2.o 

C_DEPS += \
./ex2/src/ex2.d 


# Each subdirectory must supply rules for building sources it contributes
ex2/src/%.o: ../ex2/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


