################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../aw/Resource/Image/Back2.c \
../aw/Resource/Image/Marker_red_bar_80x7s.c \
../aw/Resource/Image/fuel1.c \
../aw/Resource/Image/fuel2.c \
../aw/Resource/Image/motec.c \
../aw/Resource/Image/w_back.c \
../aw/Resource/Image/ws_batterywarning.c \
../aw/Resource/Image/ws_beltwarning.c \
../aw/Resource/Image/ws_breakwarning.c \
../aw/Resource/Image/ws_exhaustwarning.c \
../aw/Resource/Image/ws_fuelcheck.c \
../aw/Resource/Image/ws_masterwarning.c \
../aw/Resource/Image/ws_oilpresswarning.c \
../aw/Resource/Image/ws_watarcool.c \
../aw/Resource/Image/ws_waterwarning.c 

LST += \
Back2.lst \
Marker_red_bar_80x7s.lst \
fuel1.lst \
fuel2.lst \
motec.lst \
w_back.lst \
ws_batterywarning.lst \
ws_beltwarning.lst \
ws_breakwarning.lst \
ws_exhaustwarning.lst \
ws_fuelcheck.lst \
ws_masterwarning.lst \
ws_oilpresswarning.lst \
ws_watarcool.lst \
ws_waterwarning.lst 

C_DEPS += \
./aw/Resource/Image/Back2.d \
./aw/Resource/Image/Marker_red_bar_80x7s.d \
./aw/Resource/Image/fuel1.d \
./aw/Resource/Image/fuel2.d \
./aw/Resource/Image/motec.d \
./aw/Resource/Image/w_back.d \
./aw/Resource/Image/ws_batterywarning.d \
./aw/Resource/Image/ws_beltwarning.d \
./aw/Resource/Image/ws_breakwarning.d \
./aw/Resource/Image/ws_exhaustwarning.d \
./aw/Resource/Image/ws_fuelcheck.d \
./aw/Resource/Image/ws_masterwarning.d \
./aw/Resource/Image/ws_oilpresswarning.d \
./aw/Resource/Image/ws_watarcool.d \
./aw/Resource/Image/ws_waterwarning.d 

OBJS += \
./aw/Resource/Image/Back2.o \
./aw/Resource/Image/Marker_red_bar_80x7s.o \
./aw/Resource/Image/fuel1.o \
./aw/Resource/Image/fuel2.o \
./aw/Resource/Image/motec.o \
./aw/Resource/Image/w_back.o \
./aw/Resource/Image/ws_batterywarning.o \
./aw/Resource/Image/ws_beltwarning.o \
./aw/Resource/Image/ws_breakwarning.o \
./aw/Resource/Image/ws_exhaustwarning.o \
./aw/Resource/Image/ws_fuelcheck.o \
./aw/Resource/Image/ws_masterwarning.o \
./aw/Resource/Image/ws_oilpresswarning.o \
./aw/Resource/Image/ws_watarcool.o \
./aw/Resource/Image/ws_waterwarning.o 

MAP += \
FULLMONI_WIDE_official.map 


# Each subdirectory must supply rules for building sources it contributes
aw/Resource/Image/%.o: ../aw/Resource/Image/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fno-strict-aliasing -fdiagnostics-parseable-fixits -Wstack-usage=100 -g3 -mcpu=rx72t -misa=v3 -mtfu=intrinsic,mathlib -mlittle-endian-data -mtfu-version=v2 -std=gnu99 -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_pincfg" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\general" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_bsp" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_glcdc_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\Config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\GUI" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\aw\\Source" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_S12AD0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU3" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RTC" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU8" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_PORT" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_drw2d_rx\\inc\\tes" -DQE_DISPLAY_CONFIGURATION -DQE_EMWIN_CONFIGURATION -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<" -c -o "$@")
	@rx-elf-gcc @"$@.in"

