################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./APP/adc.obj" \
"./APP/control_algorithm.obj" \
"./APP/control_config.obj" \
"./APP/control_pwm.obj" \
"./APP/control_sampling.obj" \
"./APP/control_state.obj" \
"./APP/epwm.obj" \
"./APP/exit.obj" \
"./APP/key.obj" \
"./APP/led.obj" \
"./APP/llc_app.obj" \
"./APP/mppt.obj" \
"./APP/relay.obj" \
"./APP/timer0.obj" \
"./APP/timer1.obj" \
"./DSP2833x_Librsries/DSP2833x_ADC_cal.obj" \
"./DSP2833x_Librsries/DSP2833x_Adc.obj" \
"./DSP2833x_Librsries/DSP2833x_CodeStartBranch.obj" \
"./DSP2833x_Librsries/DSP2833x_CpuTimers.obj" \
"./DSP2833x_Librsries/DSP2833x_DefaultIsr.obj" \
"./DSP2833x_Librsries/DSP2833x_EPwm.obj" \
"./DSP2833x_Librsries/DSP2833x_GlobalVariableDefs.obj" \
"./DSP2833x_Librsries/DSP2833x_Gpio.obj" \
"./DSP2833x_Librsries/DSP2833x_PieCtrl.obj" \
"./DSP2833x_Librsries/DSP2833x_PieVect.obj" \
"./DSP2833x_Librsries/DSP2833x_Sci.obj" \
"./DSP2833x_Librsries/DSP2833x_SysCtrl.obj" \
"./DSP2833x_Librsries/DSP2833x_usDelay.obj" \
"../DSP2833x_Librsries/DSP2833x_Headers_nonBIOS.cmd" \
"../DSP2833x_Librsries/F28335.cmd" \
"../DSP2833x_Librsries/IQmath.lib" \
"../Libraries/lib/SFO_TI_Build_V5B_fpu.lib" \
"../Libraries/lib/rts2800_fpu32_fast_supplement.lib" \
"../Libraries/lib/rts2800_fpu32_fast_supplement_coff.lib" \
$(GEN_CMDS__FLAG) \
-lrts2800_fpu32.lib \
-lSFO_TI_Build_V5B_fpu.lib \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include APP/subdir_vars.mk
-include DSP2833x_Librsries/subdir_vars.mk
-include Libraries/lib/subdir_vars.mk
-include subdir_rules.mk
-include APP/subdir_rules.mk
-include DSP2833x_Librsries/subdir_rules.mk
-include Libraries/lib/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
dah_LLC_600w.out 

EXE_OUTPUTS__QUOTED += \
"dah_LLC_600w.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "dah_LLC_600w.out"

# Tool invocations
dah_LLC_600w.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"dah_LLC_600w.map" --stack_size=0x300 --warn_sections -i"D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/lib" -i"D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" -i"D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w/Libraries/lib" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="dah_LLC_600w_linkInfo.xml" --rom_model -o "dah_LLC_600w.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "APP\adc.obj" "APP\control_algorithm.obj" "APP\control_config.obj" "APP\control_pwm.obj" "APP\control_sampling.obj" "APP\control_state.obj" "APP\epwm.obj" "APP\exit.obj" "APP\key.obj" "APP\led.obj" "APP\llc_app.obj" "APP\mppt.obj" "APP\relay.obj" "APP\timer0.obj" "APP\timer1.obj" "DSP2833x_Librsries\DSP2833x_ADC_cal.obj" "DSP2833x_Librsries\DSP2833x_Adc.obj" "DSP2833x_Librsries\DSP2833x_CodeStartBranch.obj" "DSP2833x_Librsries\DSP2833x_CpuTimers.obj" "DSP2833x_Librsries\DSP2833x_DefaultIsr.obj" "DSP2833x_Librsries\DSP2833x_EPwm.obj" "DSP2833x_Librsries\DSP2833x_GlobalVariableDefs.obj" "DSP2833x_Librsries\DSP2833x_Gpio.obj" "DSP2833x_Librsries\DSP2833x_PieCtrl.obj" "DSP2833x_Librsries\DSP2833x_PieVect.obj" "DSP2833x_Librsries\DSP2833x_Sci.obj" "DSP2833x_Librsries\DSP2833x_SysCtrl.obj" "DSP2833x_Librsries\DSP2833x_usDelay.obj" 
	-$(RM) "main.d" "APP\adc.d" "APP\control_algorithm.d" "APP\control_config.d" "APP\control_pwm.d" "APP\control_sampling.d" "APP\control_state.d" "APP\epwm.d" "APP\exit.d" "APP\key.d" "APP\led.d" "APP\llc_app.d" "APP\mppt.d" "APP\relay.d" "APP\timer0.d" "APP\timer1.d" "DSP2833x_Librsries\DSP2833x_Adc.d" "DSP2833x_Librsries\DSP2833x_CpuTimers.d" "DSP2833x_Librsries\DSP2833x_DefaultIsr.d" "DSP2833x_Librsries\DSP2833x_EPwm.d" "DSP2833x_Librsries\DSP2833x_GlobalVariableDefs.d" "DSP2833x_Librsries\DSP2833x_Gpio.d" "DSP2833x_Librsries\DSP2833x_PieCtrl.d" "DSP2833x_Librsries\DSP2833x_PieVect.d" "DSP2833x_Librsries\DSP2833x_Sci.d" "DSP2833x_Librsries\DSP2833x_SysCtrl.d" 
	-$(RM) "DSP2833x_Librsries\DSP2833x_ADC_cal.d" "DSP2833x_Librsries\DSP2833x_CodeStartBranch.d" "DSP2833x_Librsries\DSP2833x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

