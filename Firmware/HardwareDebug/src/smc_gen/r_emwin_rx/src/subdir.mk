################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/smc_gen/r_emwin_rx/src/r_emwin_rx_if.c \
../src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_iic_if.c \
../src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_spi_if.c \
../src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_user_if.c 

LST += \
r_emwin_rx_if.lst \
r_emwin_rx_pid_iic_if.lst \
r_emwin_rx_pid_spi_if.lst \
r_emwin_rx_pid_user_if.lst 

C_DEPS += \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_if.d \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_iic_if.d \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_spi_if.d \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_user_if.d 

OBJS += \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_if.o \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_iic_if.o \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_spi_if.o \
./src/smc_gen/r_emwin_rx/src/r_emwin_rx_pid_user_if.o 

MAP += \
FULLMONI_WIDE_official.map 


# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/r_emwin_rx/src/%.o: ../src/smc_gen/r_emwin_rx/src/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fno-strict-aliasing -fdiagnostics-parseable-fixits -Wstack-usage=100 -g3 -mcpu=rx72t -misa=v3 -mtfu=intrinsic,mathlib -mlittle-endian-data -mtfu-version=v2 -std=gnu99 -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_pincfg" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\general" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_bsp" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_glcdc_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\Config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\GUI" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\aw\\Source" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_S12AD0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU3" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RTC" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU8" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_PORT" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_drw2d_rx\\inc\\tes" -DQE_DISPLAY_CONFIGURATION -DQE_EMWIN_CONFIGURATION -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<" -c -o "$@")
	@rx-elf-gcc @"$@.in"

