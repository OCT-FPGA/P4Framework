############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project saveSrcDestV2
set_top saveSrcDest_kernel
add_files src/saveSrcDest.hpp
add_files src/saveSrcDest_hls_wrapper.cpp
add_files src/saveSrcDest_kernel.cpp
add_files -tb test/test1.cpp
open_solution "solution1" -flow_target vivado
set_part {xcu280-fsvh2892-2L-e}
create_clock -period 2 -name default
source "./saveSrcDestV2/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
