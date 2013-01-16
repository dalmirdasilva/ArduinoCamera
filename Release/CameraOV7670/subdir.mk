################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CameraOV7670/CameraOV7670.cpp 

OBJS += \
./CameraOV7670/CameraOV7670.o 

CPP_DEPS += \
./CameraOV7670/CameraOV7670.d 


# Each subdirectory must supply rules for building sources it contributes
CameraOV7670/%.o: ../CameraOV7670/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/storage/microcontroller/arduino/library/io/OutputStream" -I"/storage/microcontroller/arduino/library/io/Closeable" -I/usr/share/arduino/libraries/Wire -I/usr/share/arduino/hardware/arduino/variants/mega -I/usr/share/arduino/hardware/arduino/cores/arduino -I"/storage/microcontroller/arduino/driver/camera/Camera" -I"/storage/microcontroller/arduino/driver/camera/CameraAL422B" -I"/storage/microcontroller/arduino/driver/camera/CameraOV7670" -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


