################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex3/src/ex3.c 

OBJS += \
./ex3/src/ex3.o 

C_DEPS += \
./ex3/src/ex3.d 


# Each subdirectory must supply rules for building sources it contributes
ex3/src/%.o: ../ex3/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


