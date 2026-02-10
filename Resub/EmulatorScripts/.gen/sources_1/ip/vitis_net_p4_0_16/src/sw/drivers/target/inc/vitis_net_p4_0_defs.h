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
/* SECTION: Header guards */
/****************************************************************************************************************************************************/

#ifndef XIL_VITIS_NET_P4_VITIS_NET_P4_0_DEFS_H
#define XIL_VITIS_NET_P4_VITIS_NET_P4_0_DEFS_H

/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "vitisnetp4_target.h"

/****************************************************************************************************************************************************/
/* SECTION: Constants */
/****************************************************************************************************************************************************/

extern XilVitisNetP4TargetConfig XilVitisNetP4TargetConfig_vitis_net_p4_0;

/* Address map */

// Table FiveTuple
#define XIL_VITIS_NET_P4_VITIS_NET_P4_0_FIVETUPLE_BASEADDR (0x00000000)
#define XIL_VITIS_NET_P4_VITIS_NET_P4_0_FIVETUPLE_HIGHADDR (0x00001FFF)

/****************************************************************************************************************************************************/
/* SECTION: End */
/****************************************************************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************
 * Machine-generated file - do NOT modify by hand !
 * File created on 02 of February, 2026 @ 04:01:19
 * by VitisNetP4 IP, version v1.0 revision 2
 **************************************************************************/
