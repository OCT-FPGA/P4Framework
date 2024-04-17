################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OBJS += \
./testbench/test1.o 

CPP_DEPS += \
./testbench/test1.d 


# Each subdirectory must supply rules for building sources it contributes
testbench/test1.o: /home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/hls/saveSrcDestV3/test/test1.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DAESL_TB -D__llvm__ -D__llvm__ -I/home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/hls -I/tools/Xilinx/Vitis_HLS/2023.1/include/etc -I/tools/Xilinx/Vitis_HLS/2023.1/include/ap_sysc -I/tools/Xilinx/Vitis_HLS/2023.1/include -I/tools/Xilinx/Vitis_HLS/2023.1/lnx64/tools/auto_cc/include -I/tools/Xilinx/Vitis_HLS/2023.1/lnx64/tools/systemc/include -I/home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/include/ -I/home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/hls/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


