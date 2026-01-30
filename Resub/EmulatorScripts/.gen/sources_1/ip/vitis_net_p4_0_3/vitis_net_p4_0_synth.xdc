# ############################################################
# (c) Copyright 2017 Xilinx, Inc. All rights reserved.
#
# This file contains confidential and proprietary information
# of Xilinx, Inc. and is protected under U.S. and
# international copyright and other intellectual property
# laws.
#
# DISCLAIMER
# This disclaimer is not a license and does not grant any
# rights to the materials distributed herewith. Except as
# otherwise provided in a valid license issued to you by
# Xilinx, and to the maximum extent permitted by applicable
# law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
# WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
# AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
# BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
# INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
# (2) Xilinx shall not be liable (whether in contract or tort,
# including negligence, or under any other theory of
# liability) for any loss or damage of any kind or nature
# related to, arising under or in connection with these
# materials, including for any direct, or any indirect,
# special, incidental, or consequential loss or damage
# (including loss of data, profits, goodwill, or any type of
# loss or damage suffered as a result of any action brought
# by a third party) even if such damage or loss was
# reasonably foreseeable or Xilinx had been advised of the
# possibility of the same.
#
# CRITICAL APPLICATIONS
# Xilinx products are not designed or intended to be fail-
# safe, or for use in any application requiring fail-safe
# performance, such as life-support or safety devices or
# systems, Class III medical devices, nuclear facilities,
# applications related to the deployment of airbags, or any
# other applications that could lead to death, personal
# injury, or severe property or environmental damage
# (individually and collectively, "Critical
# Applications"). Customer assumes the sole risk and
# liability of any use of Xilinx products in Critical
# Applications, subject only to applicable laws and
# regulations governing limitations on product liability.
#
# THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
# PART OF THIS FILE AT ALL TIMES.
# ############################################################
 
# table 'table0_tbl_act' Synth constraints
 
# table 'table1_tbl_dropPacket' Synth constraints
 
# table 'table2_tbl_act_0' Synth constraints
 
# table 'table3_tbl_act_1' Synth constraints
 
# table 'table4_tbl_act_2' Synth constraints
 
# table 'table5_tbl_act_3' Synth constraints
 
# table 'table6_MyProcessing_forwardIPv4' Synth constraints
set_property BLOCK_SYNTH.RETIMING {0} [get_cells match_action_engine_inst/table6_MyProcessing_forwardIPv4_inst/cam_inst]
set_property BLOCK_SYNTH.KEEP_EQUIVALENT_REGISTER {1} [get_cells match_action_engine_inst/table6_MyProcessing_forwardIPv4_inst/cam_inst]
set_property BLOCK_SYNTH.STRATEGY {PERFORMANCE_OPTIMIZED} [get_cells -hierarchical -filter {  REF_NAME == hcam_compcam_fsm || ORIG_REF_NAME == hcam_compcam_fsm ||  REF_NAME == hcam_mem_unit    || ORIG_REF_NAME == hcam_mem_unit ||  REF_NAME == hcam_hwui_cdc    || ORIG_REF_NAME == hcam_hwui_cdc ||  REF_NAME == hcam_cuckoo_reg  || ORIG_REF_NAME == hcam_cuckoo_reg ||  REF_NAME == xpm_fifo_async   || ORIG_REF_NAME == xpm_fifo_async}]
 
# table 'table7_MyProcessing_forwardIPv6' Synth constraints
set_property BLOCK_SYNTH.RETIMING {0} [get_cells match_action_engine_inst/table7_MyProcessing_forwardIPv6_inst/cam_inst]
set_property BLOCK_SYNTH.KEEP_EQUIVALENT_REGISTER {1} [get_cells match_action_engine_inst/table7_MyProcessing_forwardIPv6_inst/cam_inst]
set_property BLOCK_SYNTH.STRATEGY {PERFORMANCE_OPTIMIZED} [get_cells -hierarchical -filter {  REF_NAME == hcam_compcam_fsm || ORIG_REF_NAME == hcam_compcam_fsm ||  REF_NAME == hcam_mem_unit    || ORIG_REF_NAME == hcam_mem_unit ||  REF_NAME == hcam_hwui_cdc    || ORIG_REF_NAME == hcam_hwui_cdc ||  REF_NAME == hcam_cuckoo_reg  || ORIG_REF_NAME == hcam_cuckoo_reg ||  REF_NAME == xpm_fifo_async   || ORIG_REF_NAME == xpm_fifo_async}]
 
# table 'table8_tbl_forwardPacket' Synth constraints
