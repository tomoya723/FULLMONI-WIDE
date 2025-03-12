################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../aw/Source/Generated/APPWConf.c \
../aw/Source/Generated/ID_SCREEN_00.c \
../aw/Source/Generated/ID_SCREEN_01.c \
../aw/Source/Generated/ID_SCREEN_02.c \
../aw/Source/Generated/Resource.c 

LST += \
APPWConf.lst \
ID_SCREEN_00.lst \
ID_SCREEN_01.lst \
ID_SCREEN_02.lst \
Resource.lst 

C_DEPS += \
./aw/Source/Generated/APPWConf.d \
./aw/Source/Generated/ID_SCREEN_00.d \
./aw/Source/Generated/ID_SCREEN_01.d \
./aw/Source/Generated/ID_SCREEN_02.d \
./aw/Source/Generated/Resource.d 

OBJS += \
./aw/Source/Generated/APPWConf.o \
./aw/Source/Generated/ID_SCREEN_00.o \
./aw/Source/Generated/ID_SCREEN_01.o \
./aw/Source/Generated/ID_SCREEN_02.o \
./aw/Source/Generated/Resource.o 

MAP += \
FULLMONI_WIDE_official.map 


# Each subdirectory must supply rules for building sources it contributes
aw/Source/Generated/%.o: ../aw/Source/Generated/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fno-strict-aliasing -fdiagnostics-parseable-fixits -Wstack-usage=100 -g3 -mcpu=rx72t -misa=v3 -mtfu=intrinsic,mathlib -mlittle-endian-data -mtfu-version=v2 -std=gnu99 -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_pincfg" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\general" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_bsp" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_glcdc_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\Config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\GUI" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\aw\\Source" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_S12AD0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU3" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RTC" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU8" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_PORT" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_drw2d_rx\\inc\\tes" -DQE_DISPLAY_CONFIGURATION -DQE_EMWIN_CONFIGURATION -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<" -c -o "$@")
	@rx-elf-gcc @"$@.in"

