//////////////////////////////////////////////////////////////////////////////
// be767e8644eee50b2645307571242b99d62eea726bb276dae1cba7a07fa60690
// Proprietary Note:
// XILINX CONFIDENTIAL
//
// Copyright 2015 Xilinx, Inc. All rights reserved.
// This file contains confidential and proprietary information of Xilinx, Inc.
// and is protected under U.S. and international copyright and other
// intellectual property laws.
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
//////////////////////////////////////////////////////////////////////////////

module vitis_net_p4_0 #(
   parameter TDATA_NUM_BYTES         = 64,
   parameter TUSER_WIDTH             = 1,
   parameter TID_WIDTH               = 1,
   parameter TDEST_WIDTH             = 1,
   parameter USER_META_DATA_WIDTH    = 48,
   parameter NUM_USER_EXTERNS        = 1,
   parameter USER_EXTERN_IN_WIDTH    = 1,
   parameter USER_EXTERN_OUT_WIDTH   = 1,
   parameter S_AXI_DATA_WIDTH        = 32,
   parameter S_AXI_ADDR_WIDTH        = 13,
   parameter M_AXI_HBM_DATA_WIDTH    = 256,
   parameter M_AXI_HBM_ADDR_WIDTH    = 33,
   parameter M_AXI_HBM_ID_WIDTH      = 6,
   parameter M_AXI_HBM_RESP_WIDTH    = 2
) (
   // clocks & resets
   input  logic                              s_axis_aclk,
   input  logic                              s_axis_aresetn,
   input  logic                              s_axi_aclk,
   input  logic                              s_axi_aresetn,
   input  logic                              cam_mem_aclk,
   input  logic                              cam_mem_aresetn,
   // Metadata
   input  logic [USER_META_DATA_WIDTH-1:0]   user_metadata_in,
   input  logic                              user_metadata_in_valid,
   output logic [USER_META_DATA_WIDTH-1:0]   user_metadata_out,
   output logic                              user_metadata_out_valid,
   // AXI4-lite interface                   
   input  logic [S_AXI_ADDR_WIDTH-1:0]       s_axi_awaddr,
   input  logic                              s_axi_awvalid,
   output logic                              s_axi_awready,
   input  logic [S_AXI_DATA_WIDTH-1:0]       s_axi_wdata,
   input  logic [S_AXI_DATA_WIDTH/8-1:0]     s_axi_wstrb,
   input  logic                              s_axi_wvalid,
   output logic                              s_axi_wready,
   output logic [1:0]                        s_axi_bresp,
   output logic                              s_axi_bvalid,
   input  logic                              s_axi_bready,
   input  logic [S_AXI_ADDR_WIDTH-1:0]       s_axi_araddr,
   input  logic                              s_axi_arvalid,
   output logic                              s_axi_arready,
   output logic [S_AXI_DATA_WIDTH-1:0]       s_axi_rdata,
   output logic                              s_axi_rvalid,
   input  logic                              s_axi_rready,
   output logic [1:0]                        s_axi_rresp,
   // AXI Master port                        
   output logic [TDATA_NUM_BYTES*8-1:0]      m_axis_tdata,
   output logic [TDATA_NUM_BYTES-1:0]        m_axis_tkeep,
   output logic                              m_axis_tvalid,
   output logic                              m_axis_tlast,
   input  logic                              m_axis_tready,
   // AXI Slave port
   input  logic [TDATA_NUM_BYTES*8-1:0]      s_axis_tdata,
   input  logic [TDATA_NUM_BYTES-1:0]        s_axis_tkeep,
   input  logic                              s_axis_tvalid,
   input  logic                              s_axis_tlast,
   output logic                              s_axis_tready
);

  vitis_net_p4_0_top inst(.*);

endmodule
