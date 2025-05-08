# *************************************************************************
#
# Copyright 2020 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# *************************************************************************
#set p4_dir $user_plugin
set p4_dir [file normalize .] 
set p4_src rmvhdr.p4

read_verilog -quiet -sv p2p_250mhz.sv
# Create VitisNetP4 IP with p4 src code
create_ip -name vitis_net_p4 -vendor xilinx.com -library ip -module_name vitis_net_p4_0
set_property -dict [list CONFIG.P4_FILE "$p4_dir/$p4_src"] [get_ips vitis_net_p4_0]
generate_target all [get_ips vitis_net_p4_0]

# Modify AXI_CROSSBAR to fit Table register size
# The BASE ADDR can be found in the P4 table header file vitis_net_p4_0_defs.h
# For custom P4 codes, refer this file to configure them
set ADDR_WIDTH 14
set BASE_ADDR 0x4000
set_property -dict [list CONFIG.M00_A00_ADDR_WIDTH "$ADDR_WIDTH" CONFIG.M01_A00_BASE_ADDR "$BASE_ADDR"] [get_ips box_250mhz_axi_crossbar] 

# Set generic for width.
set cur_generic [get_property generic [current_fileset]]
lappend cur_generic "C_DUMMY_BASE_ADDR=$BASE_ADDR"
set_property -name generic -value $cur_generic -object [current_fileset]
set ccur_generic [get_property generic [current_fileset]]
