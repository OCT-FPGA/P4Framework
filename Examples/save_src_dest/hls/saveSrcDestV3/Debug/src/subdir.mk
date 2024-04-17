################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OBJS += \
./src/saveSrcDest_hls_wrapper.o \
./src/saveSrcDest_kernel.o 

CPP_DEPS += \
./src/saveSrcDest_hls_wrapper.d \
./src/saveSrcDest_kernel.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DAESL_TB -D__llvm__ -D__llvm__ -I/home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/hls -I/tools/Xilinx/Vitis_HLS/2023.1/include/etc -I/tools/Xilinx/Vitis_HLS/2023.1/include/ap_sysc -I/tools/Xilinx/Vitis_HLS/2023.1/include -I/tools/Xilinx/Vitis_HLS/2023.1/lnx64/tools/auto_cc/include -I/tools/Xilinx/Vitis_HLS/2023.1/lnx64/tools/systemc/include -I/home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/include/ -I/home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/hls/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


