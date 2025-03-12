################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/smc_gen/r_pincfg/subdir.mk
-include src/smc_gen/r_gpio_rx/src/targets/rx72n/subdir.mk
-include src/smc_gen/r_gpio_rx/src/subdir.mk
-include src/smc_gen/r_glcdc_rx/src/targets/rx72n/subdir.mk
-include src/smc_gen/r_glcdc_rx/src/subdir.mk
-include src/smc_gen/r_emwin_rx/src/subdir.mk
-include src/smc_gen/r_emwin_rx/lib/Config/subdir.mk
-include src/smc_gen/r_drw2d_rx/src/tes/subdir.mk
-include src/smc_gen/r_drw2d_rx/src/rx/subdir.mk
-include src/smc_gen/r_dmaca_rx/src/targets/rx72n/subdir.mk
-include src/smc_gen/r_dmaca_rx/src/subdir.mk
-include src/smc_gen/r_cmt_rx/src/subdir.mk
-include src/smc_gen/r_can_rx/src/subdir.mk
-include src/smc_gen/r_bsp/mcu/rx72n/subdir.mk
-include src/smc_gen/r_bsp/mcu/all/subdir.mk
-include src/smc_gen/r_bsp/board/generic_rx72n/subdir.mk
-include src/smc_gen/general/subdir.mk
-include src/smc_gen/Config_S12AD0/subdir.mk
-include src/smc_gen/Config_RTC/subdir.mk
-include src/smc_gen/Config_RIIC1/subdir.mk
-include src/smc_gen/Config_RIIC0/subdir.mk
-include src/smc_gen/Config_PORT/subdir.mk
-include src/smc_gen/Config_MTU8/subdir.mk
-include src/smc_gen/Config_MTU3/subdir.mk
-include src/smc_gen/Config_MTU1/subdir.mk
-include src/smc_gen/Config_MTU0/subdir.mk
-include src/subdir.mk
-include aw/Source/Generated/subdir.mk
-include aw/Source/CustomCode/subdir.mk
-include aw/Source/subdir.mk
-include aw/Resource/Text/subdir.mk
-include aw/Resource/Image/subdir.mk
-include aw/Resource/Font/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS += \

OBJCOPY += \
FULLMONI_WIDE_official.mot \

PRINT_SIZE += \
FULLMONI_WIDE_official.siz \


# All Target
all: FULLMONI_WIDE_official.elf secondary-outputs

# Tool invocations
FULLMONI_WIDE_official.elf: $(OBJS) $(USER_OBJS) $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS) $(LINKER_SCRIPT)
	$(file > FULLMONI_WIDE_official.elf.in,-Og -ffunction-sections -fdata-sections -fno-strict-aliasing -fdiagnostics-parseable-fixits -Wstack-usage=100 -g3 -mcpu=rx72t -misa=v3 -mtfu=intrinsic,mathlib -mlittle-endian-data -mtfu-version=v2 -o "FULLMONI_WIDE_official.elf" $(OBJS) $(USER_OBJS) $(LIBS) -T "D:/e2_studio/workspace/FULLMONI_WIDE/Firmware/src/linker_script.ld" -L"D:\\e2_studio\\workspace\\FULLMONI_WIDE\\Firmware\\src\\smc_gen\\r_emwin_rx\\lib\\GUI" -Wl,--start-group -lm -lc -lgcc -lemWinLib_RXv3_GCC -Wl,--end-group -nostartfiles -Wl,-e_PowerON_Reset -Wl,--Map=FULLMONI_WIDE_official.map)
	@echo Building target: $@ && rx-elf-gcc @"FULLMONI_WIDE_official.elf.in"
: 
	rx-elf-libgen  --select-lib=newlib --compiler-options=-Og,-ffunction-sections,-fdata-sections,-fno-strict-aliasing,-mcpu=rx72t,-misa=v3,-mlittle-endian-data --assembler-options=-mlittle-endian-data -o
FULLMONI_WIDE_official.mot: FULLMONI_WIDE_official.elf
	rx-elf-objcopy "FULLMONI_WIDE_official.elf" -O srec -I elf32-rx-be-ns  "FULLMONI_WIDE_official.mot"
FULLMONI_WIDE_official.siz: FULLMONI_WIDE_official.elf
	rx-elf-size --format=berkeley "FULLMONI_WIDE_official.elf"
# Other Targets
clean:
	-$(file > clean_file.tmp,  $(PRINT_SIZE)  $(JMP_DEPS)  $(FSY_DEPS)  $(C_UPPER_DEPS)  $(SRC_UPPER_DEPS)  $(P_UPPER_DEPS)  $(LST)  $(S_DEPS)  $(PP_UPPER_DEPS)  $(P_DEPS)  $(FSY_UPPER_DEPS)  $(C_DEPS)  $(SRC_DEPS)  $(OBJCOPY)  $(JMP_UPPER_DEPS)  $(PP_DEPS)  $(SX_DEPS)  $(ASM_UPPER_DEPS)  $(OBJS)  $(SX_UPPER_DEPS)  $(ASM_DEPS)  $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS)  $(MAP)  $(S_UPPER_DEPS) )
	-@ xargs -P 2 -s 32000 -t -a clean_file.tmp $(RM)
	-@$(RM) clean_file.tmp
	-$(RM)  FULLMONI_WIDE_official.elf
	-@echo ' '

secondary-outputs: $(OBJCOPY) $(PRINT_SIZE)

.PHONY: all clean dependents 

-include ../makefile.targets
