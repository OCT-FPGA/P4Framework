############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project saveSrcDestV3
set_top saveSrcDest_kernel
add_files saveSrcDestV3/src/saveSrcDest_kernel.cpp
add_files saveSrcDestV3/src/saveSrcDest_hls_wrapper.cpp
add_files saveSrcDestV3/src/saveSrcDest.hpp
add_files saveSrcDestV3/include/addr_types.h
add_files -tb saveSrcDestV3/test/test1.cpp -cflags "-IsaveSrcDestV3/include -IsaveSrcDestV3/src -IsaveSrcDestV3/include -IsaveSrcDestV3/src -Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xcu280-fsvh2892-2L-e}
create_clock -period 2 -name default
config_export -format ip_catalog -output /home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/hls/ip -rtl verilog
config_cosim -O -tool xsim
source "./saveSrcDestV3/solution1/directives.tcl"
csim_design -clean
csynth_design
cosim_design -O
export_design -rtl verilog -format ip_catalog -output /home/abriasco/P4OpenNIC_Public/P4Framework/Examples/save_src_dest/hls/ip
