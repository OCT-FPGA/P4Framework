############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
############################################################
open_project exchangeFivetuple
set_top exchangeFivetuple_kernel
add_files exchangeFivetuple/src/tuple_types.h
add_files exchangeFivetuple/src/exchangeFivetuple_kernel.cpp
add_files exchangeFivetuple/src/exchangeFivetuple_hls_wrapper.cpp
add_files exchangeFivetuple/src/exchangeFivetuple.hpp
open_solution "solution1" -flow_target vivado
set_part {xcu280-fsvh2892-2L-e}
create_clock -period 4 -name default
config_export -format ip_catalog -output /home/function47/Desktop/Workspace/P4Framework/Examples/reverse_tuple/hls -rtl verilog -vivado_clock 4
source "./exchangeFivetuple/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -rtl verilog -format ip_catalog -output /home/function47/Desktop/Workspace/P4Framework/Examples/reverse_tuple/hls
