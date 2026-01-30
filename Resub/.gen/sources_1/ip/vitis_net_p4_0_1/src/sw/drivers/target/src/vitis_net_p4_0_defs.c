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

// Action 'operation_add' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_add =
{
    .NameStringPtr = "operation_add",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_sub' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_sub =
{
    .NameStringPtr = "operation_sub",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_mult' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_mult =
{
    .NameStringPtr = "operation_mult",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_div' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_div =
{
    .NameStringPtr = "operation_div",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_sqrt' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_sqrt =
{
    .NameStringPtr = "operation_sqrt",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_and' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_and =
{
    .NameStringPtr = "operation_and",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_or' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_or =
{
    .NameStringPtr = "operation_or",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_xor' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_xor =
{
    .NameStringPtr = "operation_xor",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Action 'operation_drop' definition
static XilVitisNetP4Action XilVitisNetP4Action_operation_drop =
{
    .NameStringPtr = "operation_drop",
    .ParamListSize = 0,
    .ParamListPtr  = NULL
};

// Table 'calculate' Action list
static XilVitisNetP4Action *XilVitisNetP4TableActions_calculate[] =
{
    &XilVitisNetP4Action_operation_add,
    &XilVitisNetP4Action_operation_sub,
    &XilVitisNetP4Action_operation_mult,
    &XilVitisNetP4Action_operation_div,
    &XilVitisNetP4Action_operation_sqrt,
    &XilVitisNetP4Action_operation_and,
    &XilVitisNetP4Action_operation_or,
    &XilVitisNetP4Action_operation_xor,
    &XilVitisNetP4Action_operation_drop
};

// Table 'calculate' definition
static XilVitisNetP4TargetTableConfig XilVitisNetP4TargetTableConfig_calculate =
{
    .NameStringPtr     = "calculate",
    .Config            = {
        .Endian            = XIL_VITIS_NET_P4_BIG_ENDIAN,
        .Mode              = XIL_VITIS_NET_P4_TABLE_MODE_DCAM,
        .KeySizeBits       = 8,
        .CamConfig         = {
            .BaseAddr           = XIL_VITIS_NET_P4_VITIS_NET_P4_0_CALCULATE_BASEADDR,
            .FormatStringPtr    = "8c",
            .NumEntries         = 256,
            .RamFrequencyHz     = 300000000,
            .LookupFrequencyHz  = 300000000,
            .LookupsPerSec      = 250000000,
            .ResponseSizeBits   = 4,
            .PrioritySizeBits   = XIL_VITIS_NET_P4_CAM_PRIORITY_SIZE_DEFAULT,
            .NumMasks           = 0,
            .Endian             = XIL_VITIS_NET_P4_BIG_ENDIAN,
            .MemType            = XIL_VITIS_NET_P4_CAM_MEM_AUTO,
            .RamSizeKbytes      = 0,
            .OptimizationType   = XIL_VITIS_NET_P4_CAM_OPTIMIZE_NONE
        },
        .ActionIdWidthBits = 4,
        .ActionListSize    = 9,
        .ActionListPtr     = &(XilVitisNetP4TableActions_calculate[0])
    }
};

// list of all tables defined in the design
static XilVitisNetP4TargetTableConfig *XilVitisNetP4TableList[] =
{
    &XilVitisNetP4TargetTableConfig_calculate
};

// Top Level VitisNetP4 Configuration
XilVitisNetP4TargetConfig XilVitisNetP4TargetConfig_vitis_net_p4_0 =
{
    .Endian        = XIL_VITIS_NET_P4_BIG_ENDIAN,
    .TableListSize = 1,
    .TableListPtr  = &(XilVitisNetP4TableList[0]),
    .BuildInfoPtr  = NULL,
    .InterruptPtr  = NULL,
    .CtrlConfigPtr = NULL
};

/**************************************************************************
 * Machine-generated file - do NOT modify by hand !
 * File created on 26 of January, 2026 @ 09:13:08
 * by VitisNetP4 IP, version v1.0 revision 2
 **************************************************************************/
