############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project saveSrcDestV1
set_top saveSrcDest_kernel
add_files saveSrcDestV1/addr_types.h
add_files saveSrcDestV1/saveSrcDest.hpp
add_files saveSrcDestV1/saveSrcDest_hls_wrapper.cpp
add_files saveSrcDestV1/saveSrcDest_kernel.cpp
add_files -tb saveSrcDestV1/test1.cpp
open_solution "solution1" -flow_target vivado
set_part {xcu280-fsvh2892-2L-e}
create_clock -period 4 -name default
#source "./saveSrcDestV1/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
