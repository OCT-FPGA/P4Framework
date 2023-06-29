/*
-- (c) Copyright 2018 Xilinx, Inc. All rights reserved.
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
-- Description : Header file defining the random extern object for Xilinx SDNet
--
--
--------------------------------------------------------------------------------
*/

#ifndef BM_BM_SIM_RANDOM_H_
#define BM_BM_SIM_RANDOM_H_

#include <string>
#include "data.h"
#include "named_p4object.h"

namespace bm {

class Random : public NamedP4Object {
 public:
  enum RandomErrorCode {
    SUCCESS = 0,
    INVALID_RANDOM_NAME,
    INVALID_SEED,
    ERROR
  };

  Random(const std::string &name, p4object_id_t id, const std::string &max,
         const std::string &min);

  RandomErrorCode reset_state();
  RandomErrorCode set_seed(const Data &seed);
  RandomErrorCode get_seed(Data &seed);
  void get_random(Data &value);

 private:
  unsigned poly[4];
  unsigned lfsr_width{0};
  unsigned output_width{0};
  Data seed{0};
  Data lfsr{0};
  Data one{1};
  Data mask{0};
  Data max{0};
  Data min{0};
};

}

#endif  // BM_BM_SIM_RANDOM_H_