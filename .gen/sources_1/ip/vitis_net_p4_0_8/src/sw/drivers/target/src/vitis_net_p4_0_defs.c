/***************************************************************************
* (c) Copyright 2026 Xilinx, Inc. All rights reserved.
* 
* This file contains confidential and proprietary information
* of Xilinx, Inc. and is protected under U.S. and
* international copyright and other intellectual property
* laws.
* 
* DISCLAIMER
* This disclaimer is not a license and does not grant any
* rights to the materials distributed herewith. Except as
* otherwise provided in a valid license issued to you by
* Xilinx, and to the maximum extent permitted by applicable
* law: (1) THESE MATERIALS ARE MADE AVAILABLE 'AS IS' AND
* WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
* AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
* BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
* INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
* (2) Xilinx shall not be liable (whether in contract or tort,
* including negligence, or under any other theory of
* liability) for any loss or damage of any kind or nature
* related to, arising under or in connection with these
* materials, including for any direct, or any indirect,
* special, incidental, or consequential loss or damage
* (including loss of data, profits, goodwill, or any type of
* loss or damage suffered as a result of any action brought
* by a third party) even if such damage or loss was
* reasonably foreseeable or Xilinx had been advised of the
* possibility of the same.
* 
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-
* safe, or for use in any application requiring fail-safe
* performance, such as life-support or safety devices or
* systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any
* other applications that could lead to death, personal
* injury, or severe property or environmental damage
* (individually and collectively, 'Critical
* Applications'). Customer assumes the sole risk and
* liability of any use of Xilinx products in Critical
* Applications, subject only to applicable laws and
* regulations governing limitations on product liability.
* 
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
* PART OF THIS FILE AT ALL TIMES. 
**************************************************************************/

/*
 * Public control plane definitions for design vitis_net_p4_0
 */

/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/

#include "vitis_net_p4_0_defs.h"

/****************************************************************************************************************************************************/
/* SECTION: Constants */
/****************************************************************************************************************************************************/

// Action 'forwardPacket' definition
static XilVitisNetP4Action XilVitisNetP4Action_forwardPacket =
{
    .NameStringPtr = "forwardPacket",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'dropPacket' definition
static XilVitisNetP4Action XilVitisNetP4Action_dropPacket =
{
    .NameStringPtr = "dropPacket",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Table 'forwardIPv4' Action list
static XilVitisNetP4Action *XilVitisNetP4TableActions_forwardIPv4[] =
{
    &XilVitisNetP4Action_forwardPacket,
    &XilVitisNetP4Action_dropPacket
};

// Table 'forwardIPv4' definition
static XilVitisNetP4TargetTableConfig XilVitisNetP4TargetTableConfig_forwardIPv4 =
{
    .NameStringPtr     = "forwardIPv4",
    .Config            = {
        .Endian            = XIL_VITIS_NET_P4_BIG_ENDIAN,
        .Mode              = XIL_VITIS_NET_P4_TABLE_MODE_STCAM,
        .KeySizeBits       = 32,
        .CamConfig         = {
            .BaseAddr           = XIL_VITIS_NET_P4_VITIS_NET_P4_0_FORWARDIPV4_BASEADDR,
            .FormatStringPtr    = "32p",
            .NumEntries         = 1024,
            .RamFrequencyHz     = 300000000,
            .LookupFrequencyHz  = 300000000,
            .LookupsPerSec      = 196000000,
            .ResponseSizeBits   = 1,
            .PrioritySizeBits   = XIL_VITIS_NET_P4_CAM_PRIORITY_SIZE_DEFAULT,
            .NumMasks           = 64,
            .Endian             = XIL_VITIS_NET_P4_BIG_ENDIAN,
            .MemType            = XIL_VITIS_NET_P4_CAM_MEM_BRAM,
            .RamSizeKbytes      = 0,
            .OptimizationType   = XIL_VITIS_NET_P4_CAM_OPTIMIZE_NONE
        },
        .ActionIdWidthBits = 1,
        .ActionListSize    = 2,
        .ActionListPtr     = &(XilVitisNetP4TableActions_forwardIPv4[0])
    }
};

// Table 'forwardIPv6' Action list
static XilVitisNetP4Action *XilVitisNetP4TableActions_forwardIPv6[] =
{
    &XilVitisNetP4Action_forwardPacket,
    &XilVitisNetP4Action_dropPacket
};

// Table 'forwardIPv6' definition
static XilVitisNetP4TargetTableConfig XilVitisNetP4TargetTableConfig_forwardIPv6 =
{
    .NameStringPtr     = "forwardIPv6",
    .Config            = {
        .Endian            = XIL_VITIS_NET_P4_BIG_ENDIAN,
        .Mode              = XIL_VITIS_NET_P4_TABLE_MODE_TCAM,
        .KeySizeBits       = 128,
        .CamConfig         = {
            .BaseAddr           = XIL_VITIS_NET_P4_VITIS_NET_P4_0_FORWARDIPV6_BASEADDR,
            .FormatStringPtr    = "128p",
            .NumEntries         = 1024,
            .RamFrequencyHz     = 300000000,
            .LookupFrequencyHz  = 300000000,
            .LookupsPerSec      = 196000000,
            .ResponseSizeBits   = 1,
            .PrioritySizeBits   = XIL_VITIS_NET_P4_CAM_PRIORITY_SIZE_DEFAULT,
            .NumMasks           = 0,
            .Endian             = XIL_VITIS_NET_P4_BIG_ENDIAN,
            .MemType            = XIL_VITIS_NET_P4_CAM_MEM_BRAM,
            .RamSizeKbytes      = 0,
            .OptimizationType   = XIL_VITIS_NET_P4_CAM_OPTIMIZE_NONE
        },
        .ActionIdWidthBits = 1,
        .ActionListSize    = 2,
        .ActionListPtr     = &(XilVitisNetP4TableActions_forwardIPv6[0])
    }
};

// list of all tables defined in the design
static XilVitisNetP4TargetTableConfig *XilVitisNetP4TableList[] =
{
    &XilVitisNetP4TargetTableConfig_forwardIPv4,
    &XilVitisNetP4TargetTableConfig_forwardIPv6
};

// Top Level VitisNetP4 Configuration
XilVitisNetP4TargetConfig XilVitisNetP4TargetConfig_vitis_net_p4_0 =
{
    .Endian        = XIL_VITIS_NET_P4_BIG_ENDIAN,
    .TableListSize = 2,
    .TableListPtr  = &(XilVitisNetP4TableList[0]),
    .BuildInfoPtr  = NULL,
    .InterruptPtr  = NULL,
    .CtrlConfigPtr = NULL
};

/**************************************************************************
 * Machine-generated file - do NOT modify by hand !
 * File created on 10 of January, 2026 @ 08:25:45
 * by VitisNetP4 IP, version v1.0 revision 2
 **************************************************************************/
