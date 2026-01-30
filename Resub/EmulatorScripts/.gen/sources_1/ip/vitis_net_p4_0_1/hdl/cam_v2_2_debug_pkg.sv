//////////////////////////////////////////////////////////////////////////////
// be767e8644eee50b2645307571242b99d62eea726bb276dae1cba7a07fa60690
//
// Copyright 2018 Xilinx, Inc. All rights reserved.
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
//     Description: Declaration of API error codes, and debug flags
//
//////////////////////////////////////////////////////////////////////////////


`ifndef cam_v2_2_2_DEBUG_PKG
`define cam_v2_2_2_DEBUG_PKG

package cam_v2_2_debug_pkg;

// Error codes
localparam int CAM_ERROR_FULL                          = 1;
localparam int CAM_ERROR_NO_OPEN                       = 2;
localparam int CAM_ERROR_WRONG_KEY_WIDTH               = 5;
localparam int CAM_ERROR_TOO_MANY_INSTANCES            = 6;
localparam int CAM_ERROR_WRONG_BIT_FIELD_MASK          = 7;
localparam int CAM_ERROR_WRONG_CONST_FIELD_MASK        = 8;
localparam int CAM_ERROR_WRONG_UNUSED_FIELD_MASK       = 9;
localparam int CAM_ERROR_INVALID_TERNARY_FIELD_LEN     = 10;
localparam int CAM_ERROR_WRONG_PRIO_WIDTH              = 11;
localparam int CAM_ERROR_WRONG_MAX                     = 12;
localparam int CAM_ERROR_DUPLICATE_FOUND               = 13;
localparam int CAM_ERROR_WRONG_PREFIX                  = 14;
localparam int CAM_ERROR_WRONG_PREFIX_MASK             = 15;
localparam int CAM_ERROR_WRONG_RANGE                   = 16;
localparam int CAM_ERROR_WRONG_RANGE_MASK              = 17;
localparam int CAM_ERROR_KEY_NOT_FOUND                 = 18;
localparam int CAM_ERROR_WRONG_MIN                     = 19;
localparam int CAM_ERROR_WRONG_PRIO                    = 20;
localparam int CAM_ERROR_WRONG_LIST_LENGTH             = 21;
localparam int CAM_ERROR_WRONG_NUMBER_OF_SLOTS         = 22;
localparam int CAM_ERROR_INVALID_MEM_TYPE              = 23;
localparam int CAM_ERROR_TOO_HIGH_FREQUENCY            = 24;
localparam int CAM_ERROR_WRONG_TERNARY_MASK            = 25;
localparam int CAM_ERROR_MASKED_KEY_BIT_IS_SET         = 26;
localparam int CAM_ERROR_INVALID_MODE                  = 28;
localparam int CAM_ERROR_WRONG_RESPONSE_WIDTH          = 29;
localparam int CAM_ERROR_FORMAT_SYNTAX                 = 30; // Syntax error in format
localparam int CAM_ERROR_TOO_MANY_FIELDS               = 31; // Too many fields
localparam int CAM_ERROR_TOO_MANY_RANGES               = 32; // Too many ranges
localparam int CAM_ERROR_INVALID_RANGE_LEN             = 33; // Invalid range len
localparam int CAM_ERROR_INVALID_RANGE_START           = 34; // Range start + len exceeds key len
localparam int CAM_ERROR_INVALID_PREFIX_LEN            = 35; // Invalid prefix len
localparam int CAM_ERROR_INVALID_PREFIX_START          = 36; // Prefix start + len exceeds key len
localparam int CAM_ERROR_INVALID_PREFIX_KEY            = 37; // Prefix key longer than prefix len
localparam int CAM_ERROR_INVALID_BIT_FIELD_LEN         = 38; // Invalid bit len
localparam int CAM_ERROR_INVALID_BIT_FIELD_START       = 39; // Bit start + len exceeds key len
localparam int CAM_ERROR_INVALID_CONST_FIELD_LEN       = 40; // Invalid const len
localparam int CAM_ERROR_INVALID_CONST_FIELD_START     = 41; // Const start + len exceeds key len
localparam int CAM_ERROR_INVALID_UNUSED_FIELD_LEN      = 42; // Invalid unused len
localparam int CAM_ERROR_INVALID_UNUSED_FIELD_START    = 43; // Unused start + len exceeds key len
localparam int CAM_ERROR_MAX_KEY_LEN_EXCEED            = 44; // Sum of all field lengths exceeds key len
localparam int CAM_ERROR_INVALID_PRIO_AND_INDEX_WIDTH  = 45; // Prio width is too narrow or too wide
localparam int CAM_ERROR_TOO_MANY_UNITS                = 46; // Too many logical or physical memory units
localparam int CAM_ERROR_NO_MASK                       = 47; // Error during tcam_insert. The entry has not been inserted since
                                                             // hardware resources are partially depleted. It is possible to
                                                             // continue using the instance, inserting entries using already
                                                             // allocated resources, updating entries, get entries or delete entries.
localparam int CAM_ERROR_INVALID_MEMORY_WIDTH          = 48; // The memory structure defined by the combined key and response width is too wide
localparam int CAM_ERROR_UNSUPPORTED_COMMAND           = 49; // Unsupported Computational CAM command
localparam int CAM_ERROR_ENVIRONMENT                   = 50; // Computational CAM environment error at transport layer or IP non responsive
localparam int CAM_ERROR_UNSUPPORTED_CAM_TYPE          = 51; // API request for an unsupported cam type
localparam int CAM_ERROR_NULL_POINTER                  = 52; // Null pointer
localparam int CAM_ERROR_TOO_MANY_PCS                  = 53; // Too many pseudo-channels (PCs) are used
localparam int CAM_ERROR_CONFIGURATION                 = 54; // Hardware configuration parameters do not match software setttings
localparam int CAM_ERROR_ENVIRONMENT_FSMBUSY           = 55; // Computational BCAM FSM reporting busy
localparam int CAM_ERROR_ENVIRONMENT_POLLED_OUT        = 56; // Computational BCAM has polled stutus register beyond the max allowed
localparam int CAM_ERROR_SEGMENT_COUNT_ZERO            = 57; // Virtual BCAM has zero segments
localparam int CAM_ERROR_VC_UNSUPPORTED_COMMAND        = 58; // Unsupported Virtual container command
localparam int CAM_ERROR_WRONG_VIRTUAL_CAM_ID          = 59; // Wrong virtual BCAM identifier
localparam int CAM_ERROR_WRONG_SEGMENT_NUMBER          = 60; // Wrong segment number
localparam int CAM_ERROR_TOO_MANY_RAMS                 = 61; // Too many RAM instances are used
localparam int CAM_ERROR_FOUND_IN_SHADOW_BUT_NOT_IN_HW = 62; // Shadow and HW does not match
localparam int CAM_ERROR_TOO_SMALL_HEAP                = 63; // The provided max heap is not sufficiently large
localparam int CAM_ERROR_RAM_TOO_SMALL                 = 64; // The RAM is not large enough to be divided into virtual segments
localparam int CAM_ERROR_NOMEM                         = 199;
localparam int CAM_ERROR_MALLOC_FAILED                 = 198;



// Debug flags
localparam int CAM_DEBUG_NONE               = 32'h0000_0000;  // Quiet, only software error messages are printed
localparam int CAM_DEBUG_NO_ERROR_MSG       = 32'h0000_0001;  // Disable software error messages printout
localparam int CAM_DEBUG_ARGS               = 32'h0000_0002;  // Software echoes input arguments
localparam int CAM_DEBUG_VERIFY_WR          = 32'h0000_0004;  // Software immediately reads back written data from
                                                              // hardware for verification. Mismatching data is
                                                              // reported with error message.
localparam int CAM_DEBUG_VERIFY_SHADOW      = 32'h0000_0008;  // 'get_by_response' and 'get_by_key' operations read
                                                              // data from hardware and verifies software shadow
                                                              // data
localparam int CAM_DEBUG_VERBOSE_VERIFY     = 32'h0000_0010;  // Enables printout of all verify operations,
                                                              // including correct compare. Applies to
                                                              // CAM_DEBUG_VERIFY_WR, CAM_DEBUG_VERIFY_SHADOW,
                                                              // and CAM_DEBUG_VERIFY_CONFIG.
localparam int CAM_DEBUG_SKIP_MEM_INIT      = 32'h0000_0020;  // Skip memory initialization, useful to speedup
                                                              // Verilog simulation
localparam int CAM_DEBUG_CONFIG             = 32'h0000_0040;  // Enable printout of configuration parameters for both
                                                              // software and Verilog hardware simulation
localparam int CAM_DEBUG_VERIFY_CONFIG      = 32'h0000_0080;  // DEPRECATED: Configuration parameters are automatically
                                                              // read from hardware for verification
localparam int CAM_DEBUG_KEY_MASKING        = 32'h0000_0100;  // Enable error check for the presence of key value '1'
                                                              // in masked (wildcarded) bit position.
                                                              // Applies stcam and tcam input arguments.
localparam int CAM_DEBUG_STATS              = 32'h0000_0200;  // Enable statistics printout in the 'bcam_destroy' and
                                                              // 'stcam_destroy' functions.
localparam int CAM_DEBUG_SEGMENTS           = 32'h0000_0400;  // Software prints segment map for virtualized bcam (vbcam)
localparam int CAM_DEBUG_SKIP_VERIFY_CONFIG = 32'h0000_0800;  // Skip verification of hardware/software configuration 
                                                              // parameters
localparam int CAM_DEBUG_SET_COVER          = 32'h0000_1000;  // Software prints heap and set cover information for tcam
localparam int CAM_DEBUG_CONFIG_ARGS        = 32'h0000_2000;  // Software prints cam_arg values before creating the instance

localparam int CAM_DEBUG_HW_WR              = 32'h0001_0000;  // Verilog hardware prints write operations
localparam int CAM_DEBUG_HW_RD              = 32'h0002_0000;  // Verilog hardware prints read operations
localparam int CAM_DEBUG_HW_LOOKUP          = 32'h0004_0000;  // Verilog hardware prints lookup operations



// Debug status port bit vector
localparam int CAM_WR_ADDR_DECODE_FAIL      = 0;
localparam int CAM_RD_ADDR_DECODE_FAIL      = 1;
localparam int CAM_RD_DATA_OVERFLOW         = 2;
localparam int CAM_WR_RESP_OVERFLOW         = 3;
localparam int CAM_RDWR_REQ_OVERFLOW        = 4;
localparam int CAM_LOOKUP_REQ_OVERFLOW      = 5;
localparam int CAM_LOOKUP_RESP_OVERFLOW     = 6;
localparam int CAM_RDWR_BUSY                = 8;


// Hardware update interface (HWUI)
// Supported 'hwui_op' port operations
localparam logic[1:0] HWUI_INSERT = 2'b00;
localparam logic[1:0] HWUI_UPDATE = 2'b01;
localparam logic[1:0] HWUI_DELETE = 2'b10;
localparam logic[1:0] HWUI_LOOKUP = 2'b11;

endpackage

`endif

