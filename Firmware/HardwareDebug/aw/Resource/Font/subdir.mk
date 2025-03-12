################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../aw/Resource/Font/51_20_Normal_EXT.c \
../aw/Resource/Font/51_30_Normal_EXT.c \
../aw/Resource/Font/51_40_Normal_EXT.c \
../aw/Resource/Font/Arial_16_Bold_EXT.c \
../aw/Resource/Font/Arial_19_Normal_EXT.c \
../aw/Resource/Font/SuiGenerisRg_120_Bold_EXT.c 

LST += \
51_20_Normal_EXT.lst \
51_30_Normal_EXT.lst \
51_40_Normal_EXT.lst \
Arial_16_Bold_EXT.lst \
Arial_19_Normal_EXT.lst \
SuiGenerisRg_120_Bold_EXT.lst 

C_DEPS += \
./aw/Resource/Font/51_20_Normal_EXT.d \
./aw/Resource/Font/51_30_Normal_EXT.d \
./aw/Resource/Font/51_40_Normal_EXT.d \
./aw/Resource/Font/Arial_16_Bold_EXT.d \
./aw/Resource/Font/Arial_19_Normal_EXT.d \
./aw/Resource/Font/SuiGenerisRg_120_Bold_EXT.d 

OBJS += \
./aw/Resource/Font/51_20_Normal_EXT.o \
./aw/Resource/Font/51_30_Normal_EXT.o \
./aw/Resource/Font/51_40_Normal_EXT.o \
./aw/Resource/Font/Arial_16_Bold_EXT.o \
./aw/Resource/Font/Arial_19_Normal_EXT.o \
./aw/Resource/Font/SuiGenerisRg_120_Bold_EXT.o 

MAP += \
FULLMONI_WIDE_official.map 


# Each subdirectory must supply rules for building sources it contributes
aw/Resource/Font/%.o: ../aw/Resource/Font/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fno-strict-aliasing -fdiagnostics-parseable-fixits -Wstack-usage=100 -g3 -mcpu=rx72t -misa=v3 -mtfu=intrinsic,mathlib -mlittle-endian-data -mtfu-version=v2 -std=gnu99 -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_pincfg" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\general" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_bsp" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_glcdc_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\Config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\GUI" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\aw\\Source" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_S12AD0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU3" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RTC" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU8" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_PORT" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_drw2d_rx\\inc\\tes" -DQE_DISPLAY_CONFIGURATION -DQE_EMWIN_CONFIGURATION -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<" -c -o "$@")
	@rx-elf-gcc @"$@.in"

