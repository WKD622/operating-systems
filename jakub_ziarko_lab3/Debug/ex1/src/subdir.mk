################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex1/src/ex1.c 

OBJS += \
./ex1/src/ex1.o 

C_DEPS += \
./ex1/src/ex1.d 


# Each subdirectory must supply rules for building sources it contributes
ex1/src/%.o: ../ex1/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


