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

#include "vitis_net_p4_0.h"

#include "vitis_net_p4.h"

#include <map>
#include <string>





#ifdef XILINX_SIMULATOR
vitis_net_p4_0::vitis_net_p4_0(const sc_core::sc_module_name& nm) : vitis_net_p4_0_sc(nm), s_axis_aclk("s_axis_aclk"), s_axis_aresetn("s_axis_aresetn"), s_axi_aclk("s_axi_aclk"), s_axi_aresetn("s_axi_aresetn"), cam_mem_aclk("cam_mem_aclk"), cam_mem_aresetn("cam_mem_aresetn"), user_metadata_in("user_metadata_in"), user_metadata_in_valid("user_metadata_in_valid"), user_metadata_out("user_metadata_out"), user_metadata_out_valid("user_metadata_out_valid"), s_axis_tdata("s_axis_tdata"), s_axis_tkeep("s_axis_tkeep"), s_axis_tlast("s_axis_tlast"), s_axis_tvalid("s_axis_tvalid"), s_axis_tready("s_axis_tready"), m_axis_tdata("m_axis_tdata"), m_axis_tkeep("m_axis_tkeep"), m_axis_tlast("m_axis_tlast"), m_axis_tvalid("m_axis_tvalid"), m_axis_tready("m_axis_tready"), s_axi_araddr("s_axi_araddr"), s_axi_arready("s_axi_arready"), s_axi_arvalid("s_axi_arvalid"), s_axi_awaddr("s_axi_awaddr"), s_axi_awready("s_axi_awready"), s_axi_awvalid("s_axi_awvalid"), s_axi_bready("s_axi_bready"), s_axi_bresp("s_axi_bresp"), s_axi_bvalid("s_axi_bvalid"), s_axi_rdata("s_axi_rdata"), s_axi_rready("s_axi_rready"), s_axi_rresp("s_axi_rresp"), s_axi_rvalid("s_axi_rvalid"), s_axi_wdata("s_axi_wdata"), s_axi_wready("s_axi_wready"), s_axi_wstrb("s_axi_wstrb"), s_axi_wvalid("s_axi_wvalid")
{

  // initialize pins
  mp_impl->s_axis_aclk(s_axis_aclk);
  mp_impl->s_axis_aresetn(s_axis_aresetn);
  mp_impl->s_axi_aclk(s_axi_aclk);
  mp_impl->s_axi_aresetn(s_axi_aresetn);
  mp_impl->cam_mem_aclk(cam_mem_aclk);
  mp_impl->cam_mem_aresetn(cam_mem_aresetn);
  mp_impl->user_metadata_in(user_metadata_in);
  mp_impl->user_metadata_in_valid(user_metadata_in_valid);
  mp_impl->user_metadata_out(user_metadata_out);
  mp_impl->user_metadata_out_valid(user_metadata_out_valid);

  // initialize transactors
  mp_m_axis_transactor = NULL;
  mp_s_axis_transactor = NULL;
  mp_s_axi_transactor = NULL;

  // initialize socket stubs

}

void vitis_net_p4_0::before_end_of_elaboration()
{
  // configure 'm_axis' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "m_axis_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 'm_axis' transactor parameters
    xsc::common_cpp::properties m_axis_transactor_param_props;
    m_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    m_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    m_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    m_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    m_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    m_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    m_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    m_axis_transactor_param_props.addLong("HAS_RESET", "1");
    m_axis_transactor_param_props.addFloat("PHASE", "0.0");
    m_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    m_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_m_axis_transactor = new xtlm::xaxis_xtlm2pin_t<64,1,1,1,1,1>("m_axis_transactor", m_axis_transactor_param_props);

    // m_axis' transactor ports

    mp_m_axis_transactor->TDATA(m_axis_tdata);
    mp_m_axis_transactor->TKEEP(m_axis_tkeep);
    mp_m_axis_transactor->TLAST(m_axis_tlast);
    mp_m_axis_transactor->TREADY(m_axis_tready);
    mp_m_axis_transactor->TVALID(m_axis_tvalid);
    mp_m_axis_transactor->CLK(s_axis_aclk);
    mp_m_axis_transactor->RST(s_axis_aresetn);

    // m_axis' transactor sockets

    mp_impl->m_axis_tlm_axis_socket->bind(*(mp_m_axis_transactor->socket));
  }
  else
  {
  }

  // configure 's_axis' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axis_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 's_axis' transactor parameters
    xsc::common_cpp::properties s_axis_transactor_param_props;
    s_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    s_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    s_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    s_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    s_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    s_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    s_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axis_transactor_param_props.addLong("HAS_RESET", "1");
    s_axis_transactor_param_props.addFloat("PHASE", "0.0");
    s_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    s_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_s_axis_transactor = new xtlm::xaxis_pin2xtlm_t<64,1,1,1,1,1>("s_axis_transactor", s_axis_transactor_param_props);

    // s_axis' transactor ports

    mp_s_axis_transactor->TDATA(s_axis_tdata);
    mp_s_axis_transactor->TKEEP(s_axis_tkeep);
    mp_s_axis_transactor->TLAST(s_axis_tlast);
    mp_s_axis_transactor->TREADY(s_axis_tready);
    mp_s_axis_transactor->TVALID(s_axis_tvalid);
    mp_s_axis_transactor->CLK(s_axis_aclk);
    mp_s_axis_transactor->RST(s_axis_aresetn);

    // s_axis' transactor sockets

    mp_impl->s_axis_tlm_axis_socket->bind(*(mp_s_axis_transactor->socket));
  }
  else
  {
  }

  // configure 's_axi' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axi_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 's_axi' transactor parameters
    xsc::common_cpp::properties s_axi_transactor_param_props;
    s_axi_transactor_param_props.addLong("DATA_WIDTH", "32");
    s_axi_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axi_transactor_param_props.addLong("ID_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ADDR_WIDTH", "14");
    s_axi_transactor_param_props.addLong("AWUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ARUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("WUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("RUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("BUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("HAS_BURST", "0");
    s_axi_transactor_param_props.addLong("HAS_LOCK", "0");
    s_axi_transactor_param_props.addLong("HAS_PROT", "0");
    s_axi_transactor_param_props.addLong("HAS_CACHE", "0");
    s_axi_transactor_param_props.addLong("HAS_QOS", "0");
    s_axi_transactor_param_props.addLong("HAS_REGION", "0");
    s_axi_transactor_param_props.addLong("HAS_WSTRB", "1");
    s_axi_transactor_param_props.addLong("HAS_BRESP", "1");
    s_axi_transactor_param_props.addLong("HAS_RRESP", "1");
    s_axi_transactor_param_props.addLong("SUPPORTS_NARROW_BURST", "0");
    s_axi_transactor_param_props.addLong("NUM_READ_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("MAX_BURST_LENGTH", "1");
    s_axi_transactor_param_props.addLong("NUM_READ_THREADS", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_THREADS", "1");
    s_axi_transactor_param_props.addLong("RUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("WUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("HAS_SIZE", "0");
    s_axi_transactor_param_props.addLong("HAS_RESET", "1");
    s_axi_transactor_param_props.addFloat("PHASE", "0.0");
    s_axi_transactor_param_props.addString("PROTOCOL", "AXI4LITE");
    s_axi_transactor_param_props.addString("READ_WRITE_MODE", "READ_WRITE");
    s_axi_transactor_param_props.addString("CLK_DOMAIN", "");

    mp_s_axi_transactor = new xtlm::xaximm_pin2xtlm_t<32,14,1,1,1,1,1,1>("s_axi_transactor", s_axi_transactor_param_props);

    // s_axi' transactor ports

    mp_s_axi_transactor->ARADDR(s_axi_araddr);
    mp_s_axi_transactor->ARREADY(s_axi_arready);
    mp_s_axi_transactor->ARVALID(s_axi_arvalid);
    mp_s_axi_transactor->AWADDR(s_axi_awaddr);
    mp_s_axi_transactor->AWREADY(s_axi_awready);
    mp_s_axi_transactor->AWVALID(s_axi_awvalid);
    mp_s_axi_transactor->BREADY(s_axi_bready);
    mp_s_axi_transactor->BRESP(s_axi_bresp);
    mp_s_axi_transactor->BVALID(s_axi_bvalid);
    mp_s_axi_transactor->RDATA(s_axi_rdata);
    mp_s_axi_transactor->RREADY(s_axi_rready);
    mp_s_axi_transactor->RRESP(s_axi_rresp);
    mp_s_axi_transactor->RVALID(s_axi_rvalid);
    mp_s_axi_transactor->WDATA(s_axi_wdata);
    mp_s_axi_transactor->WREADY(s_axi_wready);
    mp_s_axi_transactor->WSTRB(s_axi_wstrb);
    mp_s_axi_transactor->WVALID(s_axi_wvalid);
    mp_s_axi_transactor->CLK(s_axi_aclk);
    mp_s_axi_transactor->RST(s_axi_aresetn);

    // s_axi' transactor sockets

  }
  else
  {
  }

}

#endif // XILINX_SIMULATOR




#ifdef XM_SYSTEMC
vitis_net_p4_0::vitis_net_p4_0(const sc_core::sc_module_name& nm) : vitis_net_p4_0_sc(nm), s_axis_aclk("s_axis_aclk"), s_axis_aresetn("s_axis_aresetn"), s_axi_aclk("s_axi_aclk"), s_axi_aresetn("s_axi_aresetn"), cam_mem_aclk("cam_mem_aclk"), cam_mem_aresetn("cam_mem_aresetn"), user_metadata_in("user_metadata_in"), user_metadata_in_valid("user_metadata_in_valid"), user_metadata_out("user_metadata_out"), user_metadata_out_valid("user_metadata_out_valid"), s_axis_tdata("s_axis_tdata"), s_axis_tkeep("s_axis_tkeep"), s_axis_tlast("s_axis_tlast"), s_axis_tvalid("s_axis_tvalid"), s_axis_tready("s_axis_tready"), m_axis_tdata("m_axis_tdata"), m_axis_tkeep("m_axis_tkeep"), m_axis_tlast("m_axis_tlast"), m_axis_tvalid("m_axis_tvalid"), m_axis_tready("m_axis_tready"), s_axi_araddr("s_axi_araddr"), s_axi_arready("s_axi_arready"), s_axi_arvalid("s_axi_arvalid"), s_axi_awaddr("s_axi_awaddr"), s_axi_awready("s_axi_awready"), s_axi_awvalid("s_axi_awvalid"), s_axi_bready("s_axi_bready"), s_axi_bresp("s_axi_bresp"), s_axi_bvalid("s_axi_bvalid"), s_axi_rdata("s_axi_rdata"), s_axi_rready("s_axi_rready"), s_axi_rresp("s_axi_rresp"), s_axi_rvalid("s_axi_rvalid"), s_axi_wdata("s_axi_wdata"), s_axi_wready("s_axi_wready"), s_axi_wstrb("s_axi_wstrb"), s_axi_wvalid("s_axi_wvalid")
{

  // initialize pins
  mp_impl->s_axis_aclk(s_axis_aclk);
  mp_impl->s_axis_aresetn(s_axis_aresetn);
  mp_impl->s_axi_aclk(s_axi_aclk);
  mp_impl->s_axi_aresetn(s_axi_aresetn);
  mp_impl->cam_mem_aclk(cam_mem_aclk);
  mp_impl->cam_mem_aresetn(cam_mem_aresetn);
  mp_impl->user_metadata_in(user_metadata_in);
  mp_impl->user_metadata_in_valid(user_metadata_in_valid);
  mp_impl->user_metadata_out(user_metadata_out);
  mp_impl->user_metadata_out_valid(user_metadata_out_valid);

  // initialize transactors
  mp_m_axis_transactor = NULL;
  mp_s_axis_transactor = NULL;
  mp_s_axi_transactor = NULL;

  // initialize socket stubs

}

void vitis_net_p4_0::before_end_of_elaboration()
{
  // configure 'm_axis' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "m_axis_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 'm_axis' transactor parameters
    xsc::common_cpp::properties m_axis_transactor_param_props;
    m_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    m_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    m_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    m_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    m_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    m_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    m_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    m_axis_transactor_param_props.addLong("HAS_RESET", "1");
    m_axis_transactor_param_props.addFloat("PHASE", "0.0");
    m_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    m_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_m_axis_transactor = new xtlm::xaxis_xtlm2pin_t<64,1,1,1,1,1>("m_axis_transactor", m_axis_transactor_param_props);

    // m_axis' transactor ports

    mp_m_axis_transactor->TDATA(m_axis_tdata);
    mp_m_axis_transactor->TKEEP(m_axis_tkeep);
    mp_m_axis_transactor->TLAST(m_axis_tlast);
    mp_m_axis_transactor->TREADY(m_axis_tready);
    mp_m_axis_transactor->TVALID(m_axis_tvalid);
    mp_m_axis_transactor->CLK(s_axis_aclk);
    mp_m_axis_transactor->RST(s_axis_aresetn);

    // m_axis' transactor sockets

    mp_impl->m_axis_tlm_axis_socket->bind(*(mp_m_axis_transactor->socket));
  }
  else
  {
  }

  // configure 's_axis' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axis_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 's_axis' transactor parameters
    xsc::common_cpp::properties s_axis_transactor_param_props;
    s_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    s_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    s_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    s_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    s_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    s_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    s_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axis_transactor_param_props.addLong("HAS_RESET", "1");
    s_axis_transactor_param_props.addFloat("PHASE", "0.0");
    s_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    s_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_s_axis_transactor = new xtlm::xaxis_pin2xtlm_t<64,1,1,1,1,1>("s_axis_transactor", s_axis_transactor_param_props);

    // s_axis' transactor ports

    mp_s_axis_transactor->TDATA(s_axis_tdata);
    mp_s_axis_transactor->TKEEP(s_axis_tkeep);
    mp_s_axis_transactor->TLAST(s_axis_tlast);
    mp_s_axis_transactor->TREADY(s_axis_tready);
    mp_s_axis_transactor->TVALID(s_axis_tvalid);
    mp_s_axis_transactor->CLK(s_axis_aclk);
    mp_s_axis_transactor->RST(s_axis_aresetn);

    // s_axis' transactor sockets

    mp_impl->s_axis_tlm_axis_socket->bind(*(mp_s_axis_transactor->socket));
  }
  else
  {
  }

  // configure 's_axi' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axi_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 's_axi' transactor parameters
    xsc::common_cpp::properties s_axi_transactor_param_props;
    s_axi_transactor_param_props.addLong("DATA_WIDTH", "32");
    s_axi_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axi_transactor_param_props.addLong("ID_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ADDR_WIDTH", "14");
    s_axi_transactor_param_props.addLong("AWUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ARUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("WUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("RUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("BUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("HAS_BURST", "0");
    s_axi_transactor_param_props.addLong("HAS_LOCK", "0");
    s_axi_transactor_param_props.addLong("HAS_PROT", "0");
    s_axi_transactor_param_props.addLong("HAS_CACHE", "0");
    s_axi_transactor_param_props.addLong("HAS_QOS", "0");
    s_axi_transactor_param_props.addLong("HAS_REGION", "0");
    s_axi_transactor_param_props.addLong("HAS_WSTRB", "1");
    s_axi_transactor_param_props.addLong("HAS_BRESP", "1");
    s_axi_transactor_param_props.addLong("HAS_RRESP", "1");
    s_axi_transactor_param_props.addLong("SUPPORTS_NARROW_BURST", "0");
    s_axi_transactor_param_props.addLong("NUM_READ_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("MAX_BURST_LENGTH", "1");
    s_axi_transactor_param_props.addLong("NUM_READ_THREADS", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_THREADS", "1");
    s_axi_transactor_param_props.addLong("RUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("WUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("HAS_SIZE", "0");
    s_axi_transactor_param_props.addLong("HAS_RESET", "1");
    s_axi_transactor_param_props.addFloat("PHASE", "0.0");
    s_axi_transactor_param_props.addString("PROTOCOL", "AXI4LITE");
    s_axi_transactor_param_props.addString("READ_WRITE_MODE", "READ_WRITE");
    s_axi_transactor_param_props.addString("CLK_DOMAIN", "");

    mp_s_axi_transactor = new xtlm::xaximm_pin2xtlm_t<32,14,1,1,1,1,1,1>("s_axi_transactor", s_axi_transactor_param_props);

    // s_axi' transactor ports

    mp_s_axi_transactor->ARADDR(s_axi_araddr);
    mp_s_axi_transactor->ARREADY(s_axi_arready);
    mp_s_axi_transactor->ARVALID(s_axi_arvalid);
    mp_s_axi_transactor->AWADDR(s_axi_awaddr);
    mp_s_axi_transactor->AWREADY(s_axi_awready);
    mp_s_axi_transactor->AWVALID(s_axi_awvalid);
    mp_s_axi_transactor->BREADY(s_axi_bready);
    mp_s_axi_transactor->BRESP(s_axi_bresp);
    mp_s_axi_transactor->BVALID(s_axi_bvalid);
    mp_s_axi_transactor->RDATA(s_axi_rdata);
    mp_s_axi_transactor->RREADY(s_axi_rready);
    mp_s_axi_transactor->RRESP(s_axi_rresp);
    mp_s_axi_transactor->RVALID(s_axi_rvalid);
    mp_s_axi_transactor->WDATA(s_axi_wdata);
    mp_s_axi_transactor->WREADY(s_axi_wready);
    mp_s_axi_transactor->WSTRB(s_axi_wstrb);
    mp_s_axi_transactor->WVALID(s_axi_wvalid);
    mp_s_axi_transactor->CLK(s_axi_aclk);
    mp_s_axi_transactor->RST(s_axi_aresetn);

    // s_axi' transactor sockets

  }
  else
  {
  }

}

#endif // XM_SYSTEMC




#ifdef RIVIERA
vitis_net_p4_0::vitis_net_p4_0(const sc_core::sc_module_name& nm) : vitis_net_p4_0_sc(nm), s_axis_aclk("s_axis_aclk"), s_axis_aresetn("s_axis_aresetn"), s_axi_aclk("s_axi_aclk"), s_axi_aresetn("s_axi_aresetn"), cam_mem_aclk("cam_mem_aclk"), cam_mem_aresetn("cam_mem_aresetn"), user_metadata_in("user_metadata_in"), user_metadata_in_valid("user_metadata_in_valid"), user_metadata_out("user_metadata_out"), user_metadata_out_valid("user_metadata_out_valid"), s_axis_tdata("s_axis_tdata"), s_axis_tkeep("s_axis_tkeep"), s_axis_tlast("s_axis_tlast"), s_axis_tvalid("s_axis_tvalid"), s_axis_tready("s_axis_tready"), m_axis_tdata("m_axis_tdata"), m_axis_tkeep("m_axis_tkeep"), m_axis_tlast("m_axis_tlast"), m_axis_tvalid("m_axis_tvalid"), m_axis_tready("m_axis_tready"), s_axi_araddr("s_axi_araddr"), s_axi_arready("s_axi_arready"), s_axi_arvalid("s_axi_arvalid"), s_axi_awaddr("s_axi_awaddr"), s_axi_awready("s_axi_awready"), s_axi_awvalid("s_axi_awvalid"), s_axi_bready("s_axi_bready"), s_axi_bresp("s_axi_bresp"), s_axi_bvalid("s_axi_bvalid"), s_axi_rdata("s_axi_rdata"), s_axi_rready("s_axi_rready"), s_axi_rresp("s_axi_rresp"), s_axi_rvalid("s_axi_rvalid"), s_axi_wdata("s_axi_wdata"), s_axi_wready("s_axi_wready"), s_axi_wstrb("s_axi_wstrb"), s_axi_wvalid("s_axi_wvalid")
{

  // initialize pins
  mp_impl->s_axis_aclk(s_axis_aclk);
  mp_impl->s_axis_aresetn(s_axis_aresetn);
  mp_impl->s_axi_aclk(s_axi_aclk);
  mp_impl->s_axi_aresetn(s_axi_aresetn);
  mp_impl->cam_mem_aclk(cam_mem_aclk);
  mp_impl->cam_mem_aresetn(cam_mem_aresetn);
  mp_impl->user_metadata_in(user_metadata_in);
  mp_impl->user_metadata_in_valid(user_metadata_in_valid);
  mp_impl->user_metadata_out(user_metadata_out);
  mp_impl->user_metadata_out_valid(user_metadata_out_valid);

  // initialize transactors
  mp_m_axis_transactor = NULL;
  mp_s_axis_transactor = NULL;
  mp_s_axi_transactor = NULL;

  // initialize socket stubs

}

void vitis_net_p4_0::before_end_of_elaboration()
{
  // configure 'm_axis' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "m_axis_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 'm_axis' transactor parameters
    xsc::common_cpp::properties m_axis_transactor_param_props;
    m_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    m_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    m_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    m_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    m_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    m_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    m_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    m_axis_transactor_param_props.addLong("HAS_RESET", "1");
    m_axis_transactor_param_props.addFloat("PHASE", "0.0");
    m_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    m_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_m_axis_transactor = new xtlm::xaxis_xtlm2pin_t<64,1,1,1,1,1>("m_axis_transactor", m_axis_transactor_param_props);

    // m_axis' transactor ports

    mp_m_axis_transactor->TDATA(m_axis_tdata);
    mp_m_axis_transactor->TKEEP(m_axis_tkeep);
    mp_m_axis_transactor->TLAST(m_axis_tlast);
    mp_m_axis_transactor->TREADY(m_axis_tready);
    mp_m_axis_transactor->TVALID(m_axis_tvalid);
    mp_m_axis_transactor->CLK(s_axis_aclk);
    mp_m_axis_transactor->RST(s_axis_aresetn);

    // m_axis' transactor sockets

    mp_impl->m_axis_tlm_axis_socket->bind(*(mp_m_axis_transactor->socket));
  }
  else
  {
  }

  // configure 's_axis' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axis_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 's_axis' transactor parameters
    xsc::common_cpp::properties s_axis_transactor_param_props;
    s_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    s_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    s_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    s_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    s_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    s_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    s_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axis_transactor_param_props.addLong("HAS_RESET", "1");
    s_axis_transactor_param_props.addFloat("PHASE", "0.0");
    s_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    s_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_s_axis_transactor = new xtlm::xaxis_pin2xtlm_t<64,1,1,1,1,1>("s_axis_transactor", s_axis_transactor_param_props);

    // s_axis' transactor ports

    mp_s_axis_transactor->TDATA(s_axis_tdata);
    mp_s_axis_transactor->TKEEP(s_axis_tkeep);
    mp_s_axis_transactor->TLAST(s_axis_tlast);
    mp_s_axis_transactor->TREADY(s_axis_tready);
    mp_s_axis_transactor->TVALID(s_axis_tvalid);
    mp_s_axis_transactor->CLK(s_axis_aclk);
    mp_s_axis_transactor->RST(s_axis_aresetn);

    // s_axis' transactor sockets

    mp_impl->s_axis_tlm_axis_socket->bind(*(mp_s_axis_transactor->socket));
  }
  else
  {
  }

  // configure 's_axi' transactor

  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axi_TLM_MODE") != 1)
  {
    // Instantiate Socket Stubs

  // 's_axi' transactor parameters
    xsc::common_cpp::properties s_axi_transactor_param_props;
    s_axi_transactor_param_props.addLong("DATA_WIDTH", "32");
    s_axi_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axi_transactor_param_props.addLong("ID_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ADDR_WIDTH", "14");
    s_axi_transactor_param_props.addLong("AWUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ARUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("WUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("RUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("BUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("HAS_BURST", "0");
    s_axi_transactor_param_props.addLong("HAS_LOCK", "0");
    s_axi_transactor_param_props.addLong("HAS_PROT", "0");
    s_axi_transactor_param_props.addLong("HAS_CACHE", "0");
    s_axi_transactor_param_props.addLong("HAS_QOS", "0");
    s_axi_transactor_param_props.addLong("HAS_REGION", "0");
    s_axi_transactor_param_props.addLong("HAS_WSTRB", "1");
    s_axi_transactor_param_props.addLong("HAS_BRESP", "1");
    s_axi_transactor_param_props.addLong("HAS_RRESP", "1");
    s_axi_transactor_param_props.addLong("SUPPORTS_NARROW_BURST", "0");
    s_axi_transactor_param_props.addLong("NUM_READ_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("MAX_BURST_LENGTH", "1");
    s_axi_transactor_param_props.addLong("NUM_READ_THREADS", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_THREADS", "1");
    s_axi_transactor_param_props.addLong("RUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("WUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("HAS_SIZE", "0");
    s_axi_transactor_param_props.addLong("HAS_RESET", "1");
    s_axi_transactor_param_props.addFloat("PHASE", "0.0");
    s_axi_transactor_param_props.addString("PROTOCOL", "AXI4LITE");
    s_axi_transactor_param_props.addString("READ_WRITE_MODE", "READ_WRITE");
    s_axi_transactor_param_props.addString("CLK_DOMAIN", "");

    mp_s_axi_transactor = new xtlm::xaximm_pin2xtlm_t<32,14,1,1,1,1,1,1>("s_axi_transactor", s_axi_transactor_param_props);

    // s_axi' transactor ports

    mp_s_axi_transactor->ARADDR(s_axi_araddr);
    mp_s_axi_transactor->ARREADY(s_axi_arready);
    mp_s_axi_transactor->ARVALID(s_axi_arvalid);
    mp_s_axi_transactor->AWADDR(s_axi_awaddr);
    mp_s_axi_transactor->AWREADY(s_axi_awready);
    mp_s_axi_transactor->AWVALID(s_axi_awvalid);
    mp_s_axi_transactor->BREADY(s_axi_bready);
    mp_s_axi_transactor->BRESP(s_axi_bresp);
    mp_s_axi_transactor->BVALID(s_axi_bvalid);
    mp_s_axi_transactor->RDATA(s_axi_rdata);
    mp_s_axi_transactor->RREADY(s_axi_rready);
    mp_s_axi_transactor->RRESP(s_axi_rresp);
    mp_s_axi_transactor->RVALID(s_axi_rvalid);
    mp_s_axi_transactor->WDATA(s_axi_wdata);
    mp_s_axi_transactor->WREADY(s_axi_wready);
    mp_s_axi_transactor->WSTRB(s_axi_wstrb);
    mp_s_axi_transactor->WVALID(s_axi_wvalid);
    mp_s_axi_transactor->CLK(s_axi_aclk);
    mp_s_axi_transactor->RST(s_axi_aresetn);

    // s_axi' transactor sockets

  }
  else
  {
  }

}

#endif // RIVIERA




#ifdef VCSSYSTEMC
vitis_net_p4_0::vitis_net_p4_0(const sc_core::sc_module_name& nm) : vitis_net_p4_0_sc(nm),  s_axis_aclk("s_axis_aclk"), s_axis_aresetn("s_axis_aresetn"), s_axi_aclk("s_axi_aclk"), s_axi_aresetn("s_axi_aresetn"), cam_mem_aclk("cam_mem_aclk"), cam_mem_aresetn("cam_mem_aresetn"), user_metadata_in("user_metadata_in"), user_metadata_in_valid("user_metadata_in_valid"), user_metadata_out("user_metadata_out"), user_metadata_out_valid("user_metadata_out_valid"), s_axis_tdata("s_axis_tdata"), s_axis_tkeep("s_axis_tkeep"), s_axis_tlast("s_axis_tlast"), s_axis_tvalid("s_axis_tvalid"), s_axis_tready("s_axis_tready"), m_axis_tdata("m_axis_tdata"), m_axis_tkeep("m_axis_tkeep"), m_axis_tlast("m_axis_tlast"), m_axis_tvalid("m_axis_tvalid"), m_axis_tready("m_axis_tready"), s_axi_araddr("s_axi_araddr"), s_axi_arready("s_axi_arready"), s_axi_arvalid("s_axi_arvalid"), s_axi_awaddr("s_axi_awaddr"), s_axi_awready("s_axi_awready"), s_axi_awvalid("s_axi_awvalid"), s_axi_bready("s_axi_bready"), s_axi_bresp("s_axi_bresp"), s_axi_bvalid("s_axi_bvalid"), s_axi_rdata("s_axi_rdata"), s_axi_rready("s_axi_rready"), s_axi_rresp("s_axi_rresp"), s_axi_rvalid("s_axi_rvalid"), s_axi_wdata("s_axi_wdata"), s_axi_wready("s_axi_wready"), s_axi_wstrb("s_axi_wstrb"), s_axi_wvalid("s_axi_wvalid")
{
  // initialize pins
  mp_impl->s_axis_aclk(s_axis_aclk);
  mp_impl->s_axis_aresetn(s_axis_aresetn);
  mp_impl->s_axi_aclk(s_axi_aclk);
  mp_impl->s_axi_aresetn(s_axi_aresetn);
  mp_impl->cam_mem_aclk(cam_mem_aclk);
  mp_impl->cam_mem_aresetn(cam_mem_aresetn);
  mp_impl->user_metadata_in(user_metadata_in);
  mp_impl->user_metadata_in_valid(user_metadata_in_valid);
  mp_impl->user_metadata_out(user_metadata_out);
  mp_impl->user_metadata_out_valid(user_metadata_out_valid);

  // initialize transactors
  mp_m_axis_transactor = NULL;
  mp_s_axis_transactor = NULL;
  mp_s_axi_transactor = NULL;

  // Instantiate Socket Stubs

  // configure m_axis_transactor
    xsc::common_cpp::properties m_axis_transactor_param_props;
    m_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    m_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    m_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    m_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    m_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    m_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    m_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    m_axis_transactor_param_props.addLong("HAS_RESET", "1");
    m_axis_transactor_param_props.addFloat("PHASE", "0.0");
    m_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    m_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_m_axis_transactor = new xtlm::xaxis_xtlm2pin_t<64,1,1,1,1,1>("m_axis_transactor", m_axis_transactor_param_props);
  mp_m_axis_transactor->TDATA(m_axis_tdata);
  mp_m_axis_transactor->TKEEP(m_axis_tkeep);
  mp_m_axis_transactor->TLAST(m_axis_tlast);
  mp_m_axis_transactor->TREADY(m_axis_tready);
  mp_m_axis_transactor->TVALID(m_axis_tvalid);
  mp_m_axis_transactor->CLK(s_axis_aclk);
  mp_m_axis_transactor->RST(s_axis_aresetn);
  // configure s_axis_transactor
    xsc::common_cpp::properties s_axis_transactor_param_props;
    s_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    s_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    s_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    s_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    s_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    s_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    s_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axis_transactor_param_props.addLong("HAS_RESET", "1");
    s_axis_transactor_param_props.addFloat("PHASE", "0.0");
    s_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    s_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_s_axis_transactor = new xtlm::xaxis_pin2xtlm_t<64,1,1,1,1,1>("s_axis_transactor", s_axis_transactor_param_props);
  mp_s_axis_transactor->TDATA(s_axis_tdata);
  mp_s_axis_transactor->TKEEP(s_axis_tkeep);
  mp_s_axis_transactor->TLAST(s_axis_tlast);
  mp_s_axis_transactor->TREADY(s_axis_tready);
  mp_s_axis_transactor->TVALID(s_axis_tvalid);
  mp_s_axis_transactor->CLK(s_axis_aclk);
  mp_s_axis_transactor->RST(s_axis_aresetn);
  // configure s_axi_transactor
    xsc::common_cpp::properties s_axi_transactor_param_props;
    s_axi_transactor_param_props.addLong("DATA_WIDTH", "32");
    s_axi_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axi_transactor_param_props.addLong("ID_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ADDR_WIDTH", "14");
    s_axi_transactor_param_props.addLong("AWUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ARUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("WUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("RUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("BUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("HAS_BURST", "0");
    s_axi_transactor_param_props.addLong("HAS_LOCK", "0");
    s_axi_transactor_param_props.addLong("HAS_PROT", "0");
    s_axi_transactor_param_props.addLong("HAS_CACHE", "0");
    s_axi_transactor_param_props.addLong("HAS_QOS", "0");
    s_axi_transactor_param_props.addLong("HAS_REGION", "0");
    s_axi_transactor_param_props.addLong("HAS_WSTRB", "1");
    s_axi_transactor_param_props.addLong("HAS_BRESP", "1");
    s_axi_transactor_param_props.addLong("HAS_RRESP", "1");
    s_axi_transactor_param_props.addLong("SUPPORTS_NARROW_BURST", "0");
    s_axi_transactor_param_props.addLong("NUM_READ_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("MAX_BURST_LENGTH", "1");
    s_axi_transactor_param_props.addLong("NUM_READ_THREADS", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_THREADS", "1");
    s_axi_transactor_param_props.addLong("RUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("WUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("HAS_SIZE", "0");
    s_axi_transactor_param_props.addLong("HAS_RESET", "1");
    s_axi_transactor_param_props.addFloat("PHASE", "0.0");
    s_axi_transactor_param_props.addString("PROTOCOL", "AXI4LITE");
    s_axi_transactor_param_props.addString("READ_WRITE_MODE", "READ_WRITE");
    s_axi_transactor_param_props.addString("CLK_DOMAIN", "");

    mp_s_axi_transactor = new xtlm::xaximm_pin2xtlm_t<32,14,1,1,1,1,1,1>("s_axi_transactor", s_axi_transactor_param_props);
  mp_s_axi_transactor->ARADDR(s_axi_araddr);
  mp_s_axi_transactor->ARREADY(s_axi_arready);
  mp_s_axi_transactor->ARVALID(s_axi_arvalid);
  mp_s_axi_transactor->AWADDR(s_axi_awaddr);
  mp_s_axi_transactor->AWREADY(s_axi_awready);
  mp_s_axi_transactor->AWVALID(s_axi_awvalid);
  mp_s_axi_transactor->BREADY(s_axi_bready);
  mp_s_axi_transactor->BRESP(s_axi_bresp);
  mp_s_axi_transactor->BVALID(s_axi_bvalid);
  mp_s_axi_transactor->RDATA(s_axi_rdata);
  mp_s_axi_transactor->RREADY(s_axi_rready);
  mp_s_axi_transactor->RRESP(s_axi_rresp);
  mp_s_axi_transactor->RVALID(s_axi_rvalid);
  mp_s_axi_transactor->WDATA(s_axi_wdata);
  mp_s_axi_transactor->WREADY(s_axi_wready);
  mp_s_axi_transactor->WSTRB(s_axi_wstrb);
  mp_s_axi_transactor->WVALID(s_axi_wvalid);
  mp_s_axi_transactor->CLK(s_axi_aclk);
  mp_s_axi_transactor->RST(s_axi_aresetn);

  // initialize transactors stubs
  m_axis_transactor_initiator_socket_stub = nullptr;
  s_axis_transactor_target_socket_stub = nullptr;
  s_axi_transactor_target_wr_socket_stub = nullptr;
  s_axi_transactor_target_rd_socket_stub = nullptr;

}

void vitis_net_p4_0::before_end_of_elaboration()
{
  // configure 'm_axis' transactor
  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "m_axis_TLM_MODE") != 1)
  {
    mp_impl->m_axis_tlm_axis_socket->bind(*(mp_m_axis_transactor->socket));
  
  }
  else
  {
    m_axis_transactor_initiator_socket_stub = new xtlm::xtlm_axis_initiator_stub("socket",0);
    m_axis_transactor_initiator_socket_stub->bind(*(mp_m_axis_transactor->socket));
    mp_m_axis_transactor->disable_transactor();
  }

  // configure 's_axis' transactor
  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axis_TLM_MODE") != 1)
  {
    mp_impl->s_axis_tlm_axis_socket->bind(*(mp_s_axis_transactor->socket));
  
  }
  else
  {
    s_axis_transactor_target_socket_stub = new xtlm::xtlm_axis_target_stub("socket",0);
    s_axis_transactor_target_socket_stub->bind(*(mp_s_axis_transactor->socket));
    mp_s_axis_transactor->disable_transactor();
  }

  // configure 's_axi' transactor
  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axi_TLM_MODE") != 1)
  {
  
  }
  else
  {
    s_axi_transactor_target_wr_socket_stub = new xtlm::xtlm_aximm_target_stub("wr_socket",0);
    s_axi_transactor_target_wr_socket_stub->bind(*(mp_s_axi_transactor->wr_socket));
    s_axi_transactor_target_rd_socket_stub = new xtlm::xtlm_aximm_target_stub("rd_socket",0);
    s_axi_transactor_target_rd_socket_stub->bind(*(mp_s_axi_transactor->rd_socket));
    mp_s_axi_transactor->disable_transactor();
  }

}

#endif // VCSSYSTEMC




#ifdef MTI_SYSTEMC
vitis_net_p4_0::vitis_net_p4_0(const sc_core::sc_module_name& nm) : vitis_net_p4_0_sc(nm),  s_axis_aclk("s_axis_aclk"), s_axis_aresetn("s_axis_aresetn"), s_axi_aclk("s_axi_aclk"), s_axi_aresetn("s_axi_aresetn"), cam_mem_aclk("cam_mem_aclk"), cam_mem_aresetn("cam_mem_aresetn"), user_metadata_in("user_metadata_in"), user_metadata_in_valid("user_metadata_in_valid"), user_metadata_out("user_metadata_out"), user_metadata_out_valid("user_metadata_out_valid"), s_axis_tdata("s_axis_tdata"), s_axis_tkeep("s_axis_tkeep"), s_axis_tlast("s_axis_tlast"), s_axis_tvalid("s_axis_tvalid"), s_axis_tready("s_axis_tready"), m_axis_tdata("m_axis_tdata"), m_axis_tkeep("m_axis_tkeep"), m_axis_tlast("m_axis_tlast"), m_axis_tvalid("m_axis_tvalid"), m_axis_tready("m_axis_tready"), s_axi_araddr("s_axi_araddr"), s_axi_arready("s_axi_arready"), s_axi_arvalid("s_axi_arvalid"), s_axi_awaddr("s_axi_awaddr"), s_axi_awready("s_axi_awready"), s_axi_awvalid("s_axi_awvalid"), s_axi_bready("s_axi_bready"), s_axi_bresp("s_axi_bresp"), s_axi_bvalid("s_axi_bvalid"), s_axi_rdata("s_axi_rdata"), s_axi_rready("s_axi_rready"), s_axi_rresp("s_axi_rresp"), s_axi_rvalid("s_axi_rvalid"), s_axi_wdata("s_axi_wdata"), s_axi_wready("s_axi_wready"), s_axi_wstrb("s_axi_wstrb"), s_axi_wvalid("s_axi_wvalid")
{
  // initialize pins
  mp_impl->s_axis_aclk(s_axis_aclk);
  mp_impl->s_axis_aresetn(s_axis_aresetn);
  mp_impl->s_axi_aclk(s_axi_aclk);
  mp_impl->s_axi_aresetn(s_axi_aresetn);
  mp_impl->cam_mem_aclk(cam_mem_aclk);
  mp_impl->cam_mem_aresetn(cam_mem_aresetn);
  mp_impl->user_metadata_in(user_metadata_in);
  mp_impl->user_metadata_in_valid(user_metadata_in_valid);
  mp_impl->user_metadata_out(user_metadata_out);
  mp_impl->user_metadata_out_valid(user_metadata_out_valid);

  // initialize transactors
  mp_m_axis_transactor = NULL;
  mp_s_axis_transactor = NULL;
  mp_s_axi_transactor = NULL;

  // Instantiate Socket Stubs

  // configure m_axis_transactor
    xsc::common_cpp::properties m_axis_transactor_param_props;
    m_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    m_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    m_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    m_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    m_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    m_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    m_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    m_axis_transactor_param_props.addLong("HAS_RESET", "1");
    m_axis_transactor_param_props.addFloat("PHASE", "0.0");
    m_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    m_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    m_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_m_axis_transactor = new xtlm::xaxis_xtlm2pin_t<64,1,1,1,1,1>("m_axis_transactor", m_axis_transactor_param_props);
  mp_m_axis_transactor->TDATA(m_axis_tdata);
  mp_m_axis_transactor->TKEEP(m_axis_tkeep);
  mp_m_axis_transactor->TLAST(m_axis_tlast);
  mp_m_axis_transactor->TREADY(m_axis_tready);
  mp_m_axis_transactor->TVALID(m_axis_tvalid);
  mp_m_axis_transactor->CLK(s_axis_aclk);
  mp_m_axis_transactor->RST(s_axis_aresetn);
  // configure s_axis_transactor
    xsc::common_cpp::properties s_axis_transactor_param_props;
    s_axis_transactor_param_props.addLong("TDATA_NUM_BYTES", "64");
    s_axis_transactor_param_props.addLong("TDEST_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TID_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TUSER_WIDTH", "0");
    s_axis_transactor_param_props.addLong("HAS_TREADY", "1");
    s_axis_transactor_param_props.addLong("HAS_TSTRB", "0");
    s_axis_transactor_param_props.addLong("HAS_TKEEP", "1");
    s_axis_transactor_param_props.addLong("HAS_TLAST", "1");
    s_axis_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axis_transactor_param_props.addLong("HAS_RESET", "1");
    s_axis_transactor_param_props.addFloat("PHASE", "0.0");
    s_axis_transactor_param_props.addString("CLK_DOMAIN", "");
    s_axis_transactor_param_props.addString("LAYERED_METADATA", "undef");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_1_WIDTH", "0");
    s_axis_transactor_param_props.addLong("TSIDE_BAND_2_WIDTH", "0");

    mp_s_axis_transactor = new xtlm::xaxis_pin2xtlm_t<64,1,1,1,1,1>("s_axis_transactor", s_axis_transactor_param_props);
  mp_s_axis_transactor->TDATA(s_axis_tdata);
  mp_s_axis_transactor->TKEEP(s_axis_tkeep);
  mp_s_axis_transactor->TLAST(s_axis_tlast);
  mp_s_axis_transactor->TREADY(s_axis_tready);
  mp_s_axis_transactor->TVALID(s_axis_tvalid);
  mp_s_axis_transactor->CLK(s_axis_aclk);
  mp_s_axis_transactor->RST(s_axis_aresetn);
  // configure s_axi_transactor
    xsc::common_cpp::properties s_axi_transactor_param_props;
    s_axi_transactor_param_props.addLong("DATA_WIDTH", "32");
    s_axi_transactor_param_props.addLong("FREQ_HZ", "100000000");
    s_axi_transactor_param_props.addLong("ID_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ADDR_WIDTH", "14");
    s_axi_transactor_param_props.addLong("AWUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("ARUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("WUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("RUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("BUSER_WIDTH", "0");
    s_axi_transactor_param_props.addLong("HAS_BURST", "0");
    s_axi_transactor_param_props.addLong("HAS_LOCK", "0");
    s_axi_transactor_param_props.addLong("HAS_PROT", "0");
    s_axi_transactor_param_props.addLong("HAS_CACHE", "0");
    s_axi_transactor_param_props.addLong("HAS_QOS", "0");
    s_axi_transactor_param_props.addLong("HAS_REGION", "0");
    s_axi_transactor_param_props.addLong("HAS_WSTRB", "1");
    s_axi_transactor_param_props.addLong("HAS_BRESP", "1");
    s_axi_transactor_param_props.addLong("HAS_RRESP", "1");
    s_axi_transactor_param_props.addLong("SUPPORTS_NARROW_BURST", "0");
    s_axi_transactor_param_props.addLong("NUM_READ_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_OUTSTANDING", "1");
    s_axi_transactor_param_props.addLong("MAX_BURST_LENGTH", "1");
    s_axi_transactor_param_props.addLong("NUM_READ_THREADS", "1");
    s_axi_transactor_param_props.addLong("NUM_WRITE_THREADS", "1");
    s_axi_transactor_param_props.addLong("RUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("WUSER_BITS_PER_BYTE", "0");
    s_axi_transactor_param_props.addLong("HAS_SIZE", "0");
    s_axi_transactor_param_props.addLong("HAS_RESET", "1");
    s_axi_transactor_param_props.addFloat("PHASE", "0.0");
    s_axi_transactor_param_props.addString("PROTOCOL", "AXI4LITE");
    s_axi_transactor_param_props.addString("READ_WRITE_MODE", "READ_WRITE");
    s_axi_transactor_param_props.addString("CLK_DOMAIN", "");

    mp_s_axi_transactor = new xtlm::xaximm_pin2xtlm_t<32,14,1,1,1,1,1,1>("s_axi_transactor", s_axi_transactor_param_props);
  mp_s_axi_transactor->ARADDR(s_axi_araddr);
  mp_s_axi_transactor->ARREADY(s_axi_arready);
  mp_s_axi_transactor->ARVALID(s_axi_arvalid);
  mp_s_axi_transactor->AWADDR(s_axi_awaddr);
  mp_s_axi_transactor->AWREADY(s_axi_awready);
  mp_s_axi_transactor->AWVALID(s_axi_awvalid);
  mp_s_axi_transactor->BREADY(s_axi_bready);
  mp_s_axi_transactor->BRESP(s_axi_bresp);
  mp_s_axi_transactor->BVALID(s_axi_bvalid);
  mp_s_axi_transactor->RDATA(s_axi_rdata);
  mp_s_axi_transactor->RREADY(s_axi_rready);
  mp_s_axi_transactor->RRESP(s_axi_rresp);
  mp_s_axi_transactor->RVALID(s_axi_rvalid);
  mp_s_axi_transactor->WDATA(s_axi_wdata);
  mp_s_axi_transactor->WREADY(s_axi_wready);
  mp_s_axi_transactor->WSTRB(s_axi_wstrb);
  mp_s_axi_transactor->WVALID(s_axi_wvalid);
  mp_s_axi_transactor->CLK(s_axi_aclk);
  mp_s_axi_transactor->RST(s_axi_aresetn);

  // initialize transactors stubs
  m_axis_transactor_initiator_socket_stub = nullptr;
  s_axis_transactor_target_socket_stub = nullptr;
  s_axi_transactor_target_wr_socket_stub = nullptr;
  s_axi_transactor_target_rd_socket_stub = nullptr;

}

void vitis_net_p4_0::before_end_of_elaboration()
{
  // configure 'm_axis' transactor
  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "m_axis_TLM_MODE") != 1)
  {
    mp_impl->m_axis_tlm_axis_socket->bind(*(mp_m_axis_transactor->socket));
  
  }
  else
  {
    m_axis_transactor_initiator_socket_stub = new xtlm::xtlm_axis_initiator_stub("socket",0);
    m_axis_transactor_initiator_socket_stub->bind(*(mp_m_axis_transactor->socket));
    mp_m_axis_transactor->disable_transactor();
  }

  // configure 's_axis' transactor
  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axis_TLM_MODE") != 1)
  {
    mp_impl->s_axis_tlm_axis_socket->bind(*(mp_s_axis_transactor->socket));
  
  }
  else
  {
    s_axis_transactor_target_socket_stub = new xtlm::xtlm_axis_target_stub("socket",0);
    s_axis_transactor_target_socket_stub->bind(*(mp_s_axis_transactor->socket));
    mp_s_axis_transactor->disable_transactor();
  }

  // configure 's_axi' transactor
  if (xsc::utils::xsc_sim_manager::getInstanceParameterInt("vitis_net_p4_0", "s_axi_TLM_MODE") != 1)
  {
  
  }
  else
  {
    s_axi_transactor_target_wr_socket_stub = new xtlm::xtlm_aximm_target_stub("wr_socket",0);
    s_axi_transactor_target_wr_socket_stub->bind(*(mp_s_axi_transactor->wr_socket));
    s_axi_transactor_target_rd_socket_stub = new xtlm::xtlm_aximm_target_stub("rd_socket",0);
    s_axi_transactor_target_rd_socket_stub->bind(*(mp_s_axi_transactor->rd_socket));
    mp_s_axi_transactor->disable_transactor();
  }

}

#endif // MTI_SYSTEMC




vitis_net_p4_0::~vitis_net_p4_0()
{
  delete mp_m_axis_transactor;

  delete mp_s_axis_transactor;

  delete mp_s_axi_transactor;

}

#ifdef MTI_SYSTEMC
SC_MODULE_EXPORT(vitis_net_p4_0);
#endif

#ifdef XM_SYSTEMC
XMSC_MODULE_EXPORT(vitis_net_p4_0);
#endif

#ifdef RIVIERA
SC_MODULE_EXPORT(vitis_net_p4_0);
SC_REGISTER_BV(512);
#endif

