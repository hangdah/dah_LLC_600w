################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DSP2833x_Libraries/DSP2833x_common/source/%.obj: ../DSP2833x_Libraries/DSP2833x_common/source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w" --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w/APP/APP_Libraries" --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w/DSP2833x_Libraries/DSP2833x_common/include" --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w/DSP2833x_Libraries/DSP2833x_headers/include" --include_path="D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="DSP2833x_Libraries/DSP2833x_common/source/$(basename $(<F)).d_raw" --obj_directory="DSP2833x_Libraries/DSP2833x_common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP2833x_Libraries/DSP2833x_common/source/%.obj: ../DSP2833x_Libraries/DSP2833x_common/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w" --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w/APP/APP_Libraries" --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w/DSP2833x_Libraries/DSP2833x_common/include" --include_path="D:/AAAstudy/DSPF28335_Program/microinverter_projects/dah_LLC_600w/DSP2833x_Libraries/DSP2833x_headers/include" --include_path="D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="D:/apps/ccstudio/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="DSP2833x_Libraries/DSP2833x_common/source/$(basename $(<F)).d_raw" --obj_directory="DSP2833x_Libraries/DSP2833x_common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


