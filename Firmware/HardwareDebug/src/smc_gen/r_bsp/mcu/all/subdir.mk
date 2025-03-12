################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/smc_gen/r_bsp/mcu/all/reset_program.S 

C_SRCS += \
../src/smc_gen/r_bsp/mcu/all/dbsct.c \
../src/smc_gen/r_bsp/mcu/all/lowlvl.c \
../src/smc_gen/r_bsp/mcu/all/lowsrc.c \
../src/smc_gen/r_bsp/mcu/all/mcu_locks.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_common.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_locking.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.c \
../src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.c \
../src/smc_gen/r_bsp/mcu/all/resetprg.c \
../src/smc_gen/r_bsp/mcu/all/sbrk.c 

LST += \
dbsct.lst \
lowlvl.lst \
lowsrc.lst \
mcu_locks.lst \
r_bsp_common.lst \
r_bsp_cpu.lst \
r_bsp_interrupts.lst \
r_bsp_locking.lst \
r_bsp_mcu_startup.lst \
r_bsp_software_interrupt.lst \
r_rx_intrinsic_functions.lst \
reset_program.lst \
resetprg.lst \
sbrk.lst 

C_DEPS += \
./src/smc_gen/r_bsp/mcu/all/dbsct.d \
./src/smc_gen/r_bsp/mcu/all/lowlvl.d \
./src/smc_gen/r_bsp/mcu/all/lowsrc.d \
./src/smc_gen/r_bsp/mcu/all/mcu_locks.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_common.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_locking.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.d \
./src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.d \
./src/smc_gen/r_bsp/mcu/all/resetprg.d \
./src/smc_gen/r_bsp/mcu/all/sbrk.d 

OBJS += \
./src/smc_gen/r_bsp/mcu/all/dbsct.o \
./src/smc_gen/r_bsp/mcu/all/lowlvl.o \
./src/smc_gen/r_bsp/mcu/all/lowsrc.o \
./src/smc_gen/r_bsp/mcu/all/mcu_locks.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_common.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_locking.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.o \
./src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.o \
./src/smc_gen/r_bsp/mcu/all/reset_program.o \
./src/smc_gen/r_bsp/mcu/all/resetprg.o \
./src/smc_gen/r_bsp/mcu/all/sbrk.o 

MAP += \
FULLMONI_WIDE_official.map 

S_UPPER_DEPS += \
./src/smc_gen/r_bsp/mcu/all/reset_program.d 


# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/r_bsp/mcu/all/%.o: ../src/smc_gen/r_bsp/mcu/all/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fno-strict-aliasing -fdiagnostics-parseable-fixits -Wstack-usage=100 -g3 -mcpu=rx72t -misa=v3 -mtfu=intrinsic,mathlib -mlittle-endian-data -mtfu-version=v2 -std=gnu99 -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_pincfg" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\general" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_bsp" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_glcdc_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\Config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\GUI" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\aw\\Source" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_S12AD0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU3" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RTC" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU8" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_PORT" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_drw2d_rx\\inc\\tes" -DQE_DISPLAY_CONFIGURATION -DQE_EMWIN_CONFIGURATION -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<" -c -o "$@")
	@rx-elf-gcc @"$@.in"
src/smc_gen/r_bsp/mcu/all/%.o: ../src/smc_gen/r_bsp/mcu/all/%.S
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fno-strict-aliasing -fdiagnostics-parseable-fixits -Wstack-usage=100 -g3 -mcpu=rx72t -misa=v3 -mtfu=intrinsic,mathlib -mlittle-endian-data -mtfu-version=v2 -x assembler-with-cpp -Wa,--gdwarf2 -I"D:/e2_studio/workspace/FULLMONI_WIDE/Firmware/src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_pincfg" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\general" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_bsp" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_glcdc_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_gpio_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_cmt_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\Config" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\GUI" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_dmaca_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx\\src" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_S12AD0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC0" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RIIC1" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_can_rx" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU3" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_RTC" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_MTU8" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\Config_PORT" -I"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_drw2d_rx\\inc\\tes" -Wa,-adlhn="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c "$<" -o "$@")
	@rx-elf-gcc @"$@.in"

