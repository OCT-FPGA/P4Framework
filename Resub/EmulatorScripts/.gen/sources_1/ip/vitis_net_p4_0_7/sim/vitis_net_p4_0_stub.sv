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


//------------------------------------------------------------------------------------
// Filename:    vitis_net_p4_0_stub.sv
// Description: This HDL file is intended to be used with following simulators only:
//
//   Vivado Simulator (XSim)
//   Cadence Xcelium Simulator
//   Aldec Riviera-PRO Simulator
//
//------------------------------------------------------------------------------------
`timescale 1ps/1ps

`ifdef XILINX_SIMULATOR

`ifndef XILINX_SIMULATOR_BITASBOOL
`define XILINX_SIMULATOR_BITASBOOL
typedef bit bit_as_bool;
`endif

(* SC_MODULE_EXPORT *)
module vitis_net_p4_0 (
  input bit_as_bool s_axis_aclk,
  input bit_as_bool s_axis_aresetn,
  input bit_as_bool s_axi_aclk,
  input bit_as_bool s_axi_aresetn,
  input bit_as_bool cam_mem_aclk,
  input bit_as_bool cam_mem_aresetn,
  input bit [47 : 0] user_metadata_in,
  input bit_as_bool user_metadata_in_valid,
  output bit [47 : 0] user_metadata_out,
  output bit_as_bool user_metadata_out_valid,
  input bit [511 : 0] s_axis_tdata,
  input bit [63 : 0] s_axis_tkeep,
  input bit_as_bool s_axis_tlast,
  input bit_as_bool s_axis_tvalid,
  output bit_as_bool s_axis_tready,
  output bit [511 : 0] m_axis_tdata,
  output bit [63 : 0] m_axis_tkeep,
  output bit_as_bool m_axis_tlast,
  output bit_as_bool m_axis_tvalid,
  input bit_as_bool m_axis_tready,
  input bit [12 : 0] s_axi_araddr,
  output bit_as_bool s_axi_arready,
  input bit_as_bool s_axi_arvalid,
  input bit [12 : 0] s_axi_awaddr,
  output bit_as_bool s_axi_awready,
  input bit_as_bool s_axi_awvalid,
  input bit_as_bool s_axi_bready,
  output bit [1 : 0] s_axi_bresp,
  output bit_as_bool s_axi_bvalid,
  output bit [31 : 0] s_axi_rdata,
  input bit_as_bool s_axi_rready,
  output bit [1 : 0] s_axi_rresp,
  output bit_as_bool s_axi_rvalid,
  input bit [31 : 0] s_axi_wdata,
  output bit_as_bool s_axi_wready,
  input bit [3 : 0] s_axi_wstrb,
  input bit_as_bool s_axi_wvalid
);
endmodule
`endif

`ifdef XCELIUM
(* XMSC_MODULE_EXPORT *)
module vitis_net_p4_0 (s_axis_aclk,s_axis_aresetn,s_axi_aclk,s_axi_aresetn,cam_mem_aclk,cam_mem_aresetn,user_metadata_in,user_metadata_in_valid,user_metadata_out,user_metadata_out_valid,s_axis_tdata,s_axis_tkeep,s_axis_tlast,s_axis_tvalid,s_axis_tready,m_axis_tdata,m_axis_tkeep,m_axis_tlast,m_axis_tvalid,m_axis_tready,s_axi_araddr,s_axi_arready,s_axi_arvalid,s_axi_awaddr,s_axi_awready,s_axi_awvalid,s_axi_bready,s_axi_bresp,s_axi_bvalid,s_axi_rdata,s_axi_rready,s_axi_rresp,s_axi_rvalid,s_axi_wdata,s_axi_wready,s_axi_wstrb,s_axi_wvalid)
(* integer foreign = "SystemC";
*);
  input bit s_axis_aclk;
  input bit s_axis_aresetn;
  input bit s_axi_aclk;
  input bit s_axi_aresetn;
  input bit cam_mem_aclk;
  input bit cam_mem_aresetn;
  input bit [47 : 0] user_metadata_in;
  input bit user_metadata_in_valid;
  output wire [47 : 0] user_metadata_out;
  output wire user_metadata_out_valid;
  input bit [511 : 0] s_axis_tdata;
  input bit [63 : 0] s_axis_tkeep;
  input bit s_axis_tlast;
  input bit s_axis_tvalid;
  output wire s_axis_tready;
  output wire [511 : 0] m_axis_tdata;
  output wire [63 : 0] m_axis_tkeep;
  output wire m_axis_tlast;
  output wire m_axis_tvalid;
  input bit m_axis_tready;
  input bit [12 : 0] s_axi_araddr;
  output wire s_axi_arready;
  input bit s_axi_arvalid;
  input bit [12 : 0] s_axi_awaddr;
  output wire s_axi_awready;
  input bit s_axi_awvalid;
  input bit s_axi_bready;
  output wire [1 : 0] s_axi_bresp;
  output wire s_axi_bvalid;
  output wire [31 : 0] s_axi_rdata;
  input bit s_axi_rready;
  output wire [1 : 0] s_axi_rresp;
  output wire s_axi_rvalid;
  input bit [31 : 0] s_axi_wdata;
  output wire s_axi_wready;
  input bit [3 : 0] s_axi_wstrb;
  input bit s_axi_wvalid;
endmodule
`endif

`ifdef RIVIERA
(* SC_MODULE_EXPORT *)
module vitis_net_p4_0 (s_axis_aclk,s_axis_aresetn,s_axi_aclk,s_axi_aresetn,cam_mem_aclk,cam_mem_aresetn,user_metadata_in,user_metadata_in_valid,user_metadata_out,user_metadata_out_valid,s_axis_tdata,s_axis_tkeep,s_axis_tlast,s_axis_tvalid,s_axis_tready,m_axis_tdata,m_axis_tkeep,m_axis_tlast,m_axis_tvalid,m_axis_tready,s_axi_araddr,s_axi_arready,s_axi_arvalid,s_axi_awaddr,s_axi_awready,s_axi_awvalid,s_axi_bready,s_axi_bresp,s_axi_bvalid,s_axi_rdata,s_axi_rready,s_axi_rresp,s_axi_rvalid,s_axi_wdata,s_axi_wready,s_axi_wstrb,s_axi_wvalid)
  input bit s_axis_aclk;
  input bit s_axis_aresetn;
  input bit s_axi_aclk;
  input bit s_axi_aresetn;
  input bit cam_mem_aclk;
  input bit cam_mem_aresetn;
  input bit [47 : 0] user_metadata_in;
  input bit user_metadata_in_valid;
  output wire [47 : 0] user_metadata_out;
  output wire user_metadata_out_valid;
  input bit [511 : 0] s_axis_tdata;
  input bit [63 : 0] s_axis_tkeep;
  input bit s_axis_tlast;
  input bit s_axis_tvalid;
  output wire s_axis_tready;
  output wire [511 : 0] m_axis_tdata;
  output wire [63 : 0] m_axis_tkeep;
  output wire m_axis_tlast;
  output wire m_axis_tvalid;
  input bit m_axis_tready;
  input bit [12 : 0] s_axi_araddr;
  output wire s_axi_arready;
  input bit s_axi_arvalid;
  input bit [12 : 0] s_axi_awaddr;
  output wire s_axi_awready;
  input bit s_axi_awvalid;
  input bit s_axi_bready;
  output wire [1 : 0] s_axi_bresp;
  output wire s_axi_bvalid;
  output wire [31 : 0] s_axi_rdata;
  input bit s_axi_rready;
  output wire [1 : 0] s_axi_rresp;
  output wire s_axi_rvalid;
  input bit [31 : 0] s_axi_wdata;
  output wire s_axi_wready;
  input bit [3 : 0] s_axi_wstrb;
  input bit s_axi_wvalid;
endmodule
`endif
