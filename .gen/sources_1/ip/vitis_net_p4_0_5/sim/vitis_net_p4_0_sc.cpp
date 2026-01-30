// (c) Copyright 1995-2026 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.


#include "vitis_net_p4_0_sc.h"

#include "vitis_net_p4.h"

#include <map>
#include <string>

vitis_net_p4_0_sc::vitis_net_p4_0_sc(const sc_core::sc_module_name& nm) : sc_core::sc_module(nm), mp_impl(NULL)
{
  // configure connectivity manager
  xsc::utils::xsc_sim_manager::addInstance("vitis_net_p4_0", this);

  // initialize module
    xsc::common_cpp::properties model_param_props;
    model_param_props.addLong("TDATA_NUM_BYTES", "64");
    model_param_props.addLong("TID_WIDTH", "0");
    model_param_props.addLong("TDEST_WIDTH", "0");
    model_param_props.addLong("TUSER_WIDTH", "0");
    model_param_props.addLong("USER_META_DATA_WIDTH", "16");
    model_param_props.addLong("NUM_USER_EXTERNS", "1");
    model_param_props.addLong("USER_EXTERN_IN_WIDTH", "64");
    model_param_props.addLong("USER_EXTERN_OUT_WIDTH", "64");
    model_param_props.addLong("S_AXI_DATA_WIDTH", "32");
    model_param_props.addLong("S_AXI_ADDR_WIDTH", "14");
    model_param_props.addLong("M_AXI_HBM_DATA_WIDTH", "256");
    model_param_props.addLong("M_AXI_HBM_ADDR_WIDTH", "33");
    model_param_props.addLong("M_AXI_HBM_ID_WIDTH", "6");
    model_param_props.addLong("M_AXI_HBM_RESP_WIDTH", "2");
    model_param_props.addString("COMPONENT_NAME", "vitis_net_p4_0");

  mp_impl = new vitis_net_p4("inst", model_param_props);

  // initialize AXI sockets
  m_axis_tlm_axis_socket = mp_impl->m_axis_tlm_axis_socket;
  s_axis_tlm_axis_socket = mp_impl->s_axis_tlm_axis_socket;
}

vitis_net_p4_0_sc::~vitis_net_p4_0_sc()
{
  xsc::utils::xsc_sim_manager::clean();

  delete mp_impl;
}

