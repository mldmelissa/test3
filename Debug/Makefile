################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../LSCI\ libraries \
../include \
../build \
../port \
../source \
../third\ party


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../build/startup_sam4e.c \
../build/syscalls.c \
../build/system_sam4e.c \
../port/HAL.cpp \
../source/android_comm_interface_manager.cpp \
../source/calibration.cpp \
../source/command_manager.cpp \
../source/main.cpp \
../source/output_control.cpp \
../source/settings_manager.cpp \
../source/sine_wave.cpp \
../source/sources_command_callbacks.cpp \
../source/sources_settings_callbacks.cpp \
../source/utility_functions.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
build/startup_sam4e.o \
build/syscalls.o \
build/system_sam4e.o \
port/HAL.o \
source/android_comm_interface_manager.o \
source/calibration.o \
source/command_manager.o \
source/main.o \
source/output_control.o \
source/settings_manager.o \
source/sine_wave.o \
source/sources_command_callbacks.o \
source/sources_settings_callbacks.o \
source/utility_functions.o

OBJS_AS_ARGS +=  \
build/startup_sam4e.o \
build/syscalls.o \
build/system_sam4e.o \
port/HAL.o \
source/android_comm_interface_manager.o \
source/calibration.o \
source/command_manager.o \
source/main.o \
source/output_control.o \
source/settings_manager.o \
source/sine_wave.o \
source/sources_command_callbacks.o \
source/sources_settings_callbacks.o \
source/utility_functions.o

C_DEPS +=  \
build/startup_sam4e.d \
build/syscalls.d \
build/system_sam4e.d \
port/HAL.d \
source/android_comm_interface_manager.d \
source/calibration.d \
source/command_manager.d \
source/main.d \
source/output_control.d \
source/settings_manager.d \
source/sine_wave.d \
source/sources_command_callbacks.d \
source/sources_settings_callbacks.d \
source/utility_functions.d

C_DEPS_AS_ARGS +=  \
build/startup_sam4e.d \
build/syscalls.d \
build/system_sam4e.d \
port/HAL.d \
source/android_comm_interface_manager.d \
source/calibration.d \
source/command_manager.d \
source/main.d \
source/output_control.d \
source/settings_manager.d \
source/sine_wave.d \
source/sources_command_callbacks.d \
source/sources_settings_callbacks.d \
source/utility_functions.d

OUTPUT_FILE_PATH +=Sources.elf

OUTPUT_FILE_PATH_AS_ARGS +=Sources.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+=  \
../build/sam4e16e_flash.ld \
../build/sam4e16e_sram.ld \
../build/sam4e_flash.ld \
../build/sam4e_sram.ld


# AVR32/GNU C Compiler





























build/%.o: ../build/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 5.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -x c -mthumb -D__SAM4E16E__ -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAM4E_DFP\1.1.45\ic.sam4e\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include" -I"../build" -I"../include" -I"../LSCI libraries" -I"../source" -I"../third party"  -O3 -ffunction-sections -funsafe-math-optimizations -ffast-math -mlong-calls -g3 -Wall -mcpu=cortex-m4 -c -std=gnu99 -Wno-unknown-pragmas -pipe -fno-strict-aliasing -Wall -Wextra -ffunction-sections -fdata-sections --param max-inline-insns-single=500 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

port/%.o: ../port/%.cpp
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 5.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-g++.exe$(QUOTE) -mthumb -D__SAM4E16E__ -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAM4E_DFP\1.1.45\ic.sam4e\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include" -I"../build" -I"../include" -I"../LSCI libraries" -I"../port" -I"../source" -I"../third party"  -O3 -ffunction-sections -funsafe-math-optimizations -ffast-math -fno-rtti -fno-exceptions -mlong-calls -g3 -Wall -mcpu=cortex-m4 -c -Wno-unknown-pragmas -pipe -fno-strict-aliasing -Wall -Wextra -ffunction-sections -fdata-sections --param max-inline-insns-single=500 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

source/%.o: ../source/%.cpp
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 5.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-g++.exe$(QUOTE) -mthumb -D__SAM4E16E__ -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAM4E_DFP\1.1.45\ic.sam4e\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include" -I"../build" -I"../include" -I"../LSCI libraries" -I"../port" -I"../source" -I"../third party"  -O3 -ffunction-sections -funsafe-math-optimizations -ffast-math -fno-rtti -fno-exceptions -mlong-calls -g3 -Wall -mcpu=cortex-m4 -c -Wno-unknown-pragmas -pipe -fno-strict-aliasing -Wall -Wextra -ffunction-sections -fdata-sections --param max-inline-insns-single=500 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker : 5.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -mthumb -Wl,-Map="Sources.map" --specs=nosys.specs -Wl,--start-group -lm -lserial_circular_buffer_service -lLSCP_service  -Wl,--end-group -L"../build" -L"../LSCI libraries" -L"../../../Main/LSCI libraries" -L"../../../../Libraries/serial_circular_buffer_service/Main/serial_circular_buffer_service/Debug" -L"../../../../Libraries/LSCP_Service/Main/LSCP_Service/Debug" -L"../../../Libraries/LSCP_Service/Main/LSCP_Service/Debug" -L"../../../Libraries/serial_circular_buffer_service/Main/serial_circular_buffer_service/Debug"  -Wl,--gc-sections -mcpu=cortex-m4 -Tsam4e16e_flash.ld  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O binary "Sources.elf" "Sources.bin"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "Sources.elf" "Sources.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary "Sources.elf" "Sources.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objdump.exe" -h -S "Sources.elf" > "Sources.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "Sources.elf" "Sources.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-size.exe" "Sources.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Sources.elf" "Sources.a" "Sources.hex" "Sources.bin" "Sources.lss" "Sources.eep" "Sources.map" "Sources.srec"
	