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
 
# table 'table0_tbl_InsertTimestamp' Impl constraints
 
# table 'table1_tbl_act' Impl constraints
 
# table 'table2_MyProcessing_FiveTuple' Impl constraints
set_property DONT_TOUCH TRUE [get_cells match_action_engine_inst/table2_MyProcessing_FiveTuple_inst/cam_inst/C*/A*/C[0].M[0].CL.C/R[0].M/R.R/KM[0].*RAM.R/xpm_memory_base_inst/gen_wr_a.gen_word_narrow.mem_reg_*ram_0]
set_property DONT_TOUCH TRUE [get_cells match_action_engine_inst/table2_MyProcessing_FiveTuple_inst/cam_inst/C*/A*/C[0].M[0].CL.C/R[0].M/R.R/KM[1].*RAM.R/xpm_memory_base_inst/gen_wr_a.gen_word_narrow.mem_reg_*ram_0]
create_macro table2_MyProcessing_FiveTuple_col0_mir0_row0
update_macro table2_MyProcessing_FiveTuple_col0_mir0_row0 {{match_action_engine_inst/table2_MyProcessing_FiveTuple_inst/cam_inst/C*/A*/C[0].M[0].CL.C/R[0].M/R.R/KM[0].*RAM.R/xpm_memory_base_inst/gen_wr_a.gen_word_narrow.mem_reg_*ram_0} X0Y0 {match_action_engine_inst/table2_MyProcessing_FiveTuple_inst/cam_inst/C*/A*/C[0].M[0].CL.C/R[0].M/R.R/KM[1].*RAM.R/xpm_memory_base_inst/gen_wr_a.gen_word_narrow.mem_reg_*ram_0} X0Y4}
 
# table 'table3_tbl_act_0' Impl constraints
 
# table 'table4_tbl_InsertTimestamplatest' Impl constraints
