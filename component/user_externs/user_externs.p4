/*
-- (c) Copyright 2019 Xilinx, Inc. All rights reserved.
--
-- This file contains confidential and proprietary information
-- of Xilinx, Inc. and is protected under U.S. and
-- international copyright and other intellectual property
-- laws.
--
-- DISCLAIMER
-- This disclaimer is not a license and does not grant any
-- rights to the materials distributed herewith. Except as
-- otherwise provided in a valid license issued to you by
-- Xilinx, and to the maximum extent permitted by applicable
-- law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
-- WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
-- AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
-- BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
-- INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
-- (2) Xilinx shall not be liable (whether in contract or tort,
-- including negligence, or under any other theory of
-- liability) for any loss or damage of any kind or nature
-- related to, arising under or in connection with these
-- materials, including for any direct, or any indirect,
-- special, incidental, or consequential loss or damage
-- (including loss of data, profits, goodwill, or any type of
-- loss or damage suffered as a result of any action brought
-- by a third party) even if such damage or loss was
-- reasonably foreseeable or Xilinx had been advised of the
-- possibility of the same.
--
-- CRITICAL APPLICATIONS
-- Xilinx products are not designed or intended to be fail-
-- safe, or for use in any application requiring fail-safe
-- performance, such as life-support or safety devices or
-- systems, Class III medical devices, nuclear facilities,
-- applications related to the deployment of airbags, or any
-- other applications that could lead to death, personal
-- injury, or severe property or environmental damage
-- (individually and collectively, "Critical
-- Applications"). Customer assumes the sole risk and
-- liability of any use of Xilinx products in Critical
-- Applications, subject only to applicable laws and
-- regulations governing limitations on product liability.
--
-- THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
-- PART OF THIS FILE AT ALL TIMES. 
--------------------------------------------------------------------------------
--
-- Vendor         : Xilinx
-- Revision       : $Revision: #1 $
-- Date           : $DateTime: 2022/07/11 18:35:00 $
-- Last Author    : $Author: xbuild $
--
--------------------------------------------------------------------------------
-- Description : Minimal user extern example
--
--------------------------------------------------------------------------------
*/

#include <core.p4>
#include <xsa.p4>

struct headers_t {
}

struct switch_metadata_t {
    bit<12> input;
    bool match;
}

parser MyParser(packet_in pkt,
                out headers_t hdr,
                inout switch_metadata_t ctrl,
                inout standard_metadata_t standard_metadata) {
    state start {
        transition accept;
    }
}

control Pipeline(inout headers_t hdr,
                 inout switch_metadata_t ctrl,
                 inout standard_metadata_t standard_metadata) {
    
    // Here we declare an instance of the user extern.  In declaring the
    // instance, we specify:
    //  - The type of input data, in this case bit<12>
    //  - The type of output data, in this case bit<12>
    //  - The number of clock cycles of latency of the hardware, in this case 16
    //  - The name of the instance, which must match the name of the C++ model
    UserExtern<bit<12>,bit<12>>(16) minimal_user_extern_example;
    bit<12> input;
    bit<12> output;

    apply {
        // This assignment is unnecessary, just for done to simplify the
        // subsequent statement where the extern is used
        input = ctrl.input;
        // Here, we utilize the user extern instance.  Per the definition of
        // UserExtern in xsa.p4, the extern has only one method called apply().
        // The method may be invoked only once in the program, however a program
        // may contain multiple instances of UserExtern.  In order to model the
        // function performed by the extern in software, a shared object must be
        // compiled and loaded into the behavioural model at runtime.  Example
        // code for creating such a shared object is provided alongside this
        // program.  Note that in this instance, the parameters passed to this
        // method are both variables and because of this, the parameters to the
        // apply() method defined in the software model must be of type "Field".
        // If, for instance, the first parameter passed below was a constant,
        // then the first parameter of the apply() method in the software model
        // would have to be changed to type "Data".
    	minimal_user_extern_example.apply(input, output);
        if (input == output)
            ctrl.match = true;
        else
            ctrl.match = false;
    }
}

control MyDeparser(packet_out pkt,
                   in headers_t hdr,
                   inout switch_metadata_t ctrl,
                   inout standard_metadata_t standard_metadata) {
    apply {
    }
}

XilinxPipeline(MyParser(), Pipeline(), MyDeparser()) main;
