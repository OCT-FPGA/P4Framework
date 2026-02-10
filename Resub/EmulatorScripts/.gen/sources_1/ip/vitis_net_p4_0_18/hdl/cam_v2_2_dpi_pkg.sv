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
// Declarations of the software API for Verilog DPI simulation
// Descriptions of the software functions are available in the software header files:
//   sw/cam_src/inc/cam.h
//   sw/cam_src/inc/bcam.h
//   sw/cam_src/inc/tcam.h
//   sw/cam_src/inc/stcam.h
//   sw/cam_src/inc/vbcam.h
//
//////////////////////////////////////////////////////////////////////////////


`ifndef cam_v2_2_2_DPI_PKG
`define cam_v2_2_2_DPI_PKG

package cam_v2_2_dpi_pkg;

`ifndef SYNTHESIS

// CAM API
// The functions are described in the file cam.h
import "DPI-C" context function int  cam_arg_create(inout chandle out_cam_arg);

import "DPI-C" context function void cam_arg_init(inout chandle out_cam_arg);

import "DPI-C" context function void cam_arg_destroy(chandle cam_arg);

import "DPI-C" context function int  cam_arg_set_format(chandle cam_arg, string format);

import "DPI-C" context function void cam_arg_set_inst_ctx(chandle cam_arg, chandle inst_ctx);

import "DPI-C" context function void cam_arg_set_hw_write_function(chandle cam_arg,
                                                                   chandle hw_write_function);

import "DPI-C" context function void cam_arg_set_hw_read_function(chandle cam_arg,
                                                                  chandle hw_read_function);

import "DPI-C" context function void cam_arg_set_num_entries(chandle cam_arg,
                                                             int unsigned num_entries);

import "DPI-C" context function void cam_arg_set_hw_assist(chandle cam_arg, int hw_assist);

import "DPI-C" context function void cam_arg_set_engine_lookup_rate_hz(chandle cam_arg,
                                                                       int unsigned lookup_rate_hz);

import "DPI-C" context function void cam_arg_set_num_banks(chandle cam_arg,
                                                                       int unsigned num_banks);

import "DPI-C" context function void cam_arg_set_lookup_interface_freq_hz(chandle cam_arg,
                                                                          int unsigned freq_hz);

import "DPI-C" context function void cam_arg_set_ram_freq_hz(chandle cam_arg, int unsigned freq_hz);

import "DPI-C" context function void cam_arg_set_response_width(chandle cam_arg,
                                                                shortint unsigned response_width);

import "DPI-C" context function void cam_arg_force_prio_width(chandle cam_arg, byte prio);

import "DPI-C" context function void cam_arg_set_key_width(chandle cam_arg,
                                                           shortint unsigned key_width);

import "DPI-C" context function void cam_arg_set_num_masks(chandle cam_arg, byte num_masks);

import "DPI-C" context function void cam_arg_force_num_units(chandle cam_arg,
                                                             shortint unsigned num_units);

import "DPI-C" context function void cam_arg_force_mem_type(chandle cam_arg, int uram);

import "DPI-C" context function void cam_arg_force_hbm(chandle cam_arg);

import "DPI-C" context function void cam_arg_force_dram(chandle cam_arg);

import "DPI-C" context function void cam_arg_force_asic_ram(chandle cam_arg);

import "DPI-C" context function void cam_arg_set_ram_size(chandle cam_arg, int unsigned size);

import "DPI-C" context function void cam_arg_set_channel_width(chandle cam_arg, shortint unsigned width);

import "DPI-C" context function void cam_arg_set_narrow_key_width(chandle cam_arg,
                                                                  shortint unsigned narrow_key_width);

import "DPI-C" context function void cam_arg_set_max_hw_writes(chandle cam_arg,
                                                               shortint unsigned max_hw_writes);

import "DPI-C" context function void cam_arg_set_flow_control(chandle cam_arg, int flow_control);

import "DPI-C" context function void cam_arg_set_default_response(chandle cam_arg,
                                                                  chandle default_response);

import "DPI-C" context function void cam_arg_set_narrow_default_response(chandle cam_arg,
                                                                         chandle narrow_default_response);

import "DPI-C" context function void cam_arg_set_vcid(chandle cam_arg, byte vcid);

import "DPI-C" context function void cam_arg_set_seg(chandle cam_arg, shortint unsigned segment_id);

import "DPI-C" context function void cam_arg_set_hw_write_sequence_functions(chandle cam_arg,
                                                                             chandle start_function,
                                                                             chandle end_function);

import "DPI-C" context function void cam_arg_set_debug_flags(chandle cam_arg,
                                                             int unsigned debug_flags);

import "DPI-C" context function void cam_arg_set_optimization_flags(chandle cam_arg,
                                                                    int unsigned optimization_flags);

import "DPI-C" context function void cam_arg_enable_options(chandle cam_arg);

import "DPI-C" context function void cam_arg_set_error_print_function(chandle cam_arg,
                                                                      chandle error_print_function);

import "DPI-C" context function void cam_arg_set_info_print_function(chandle cam_arg,
                                                                     chandle info_print_function);

import "DPI-C" context function void cam_arg_set_malloc_function(chandle cam_arg,
                                                                 chandle malloc_function);

import "DPI-C" context function void cam_arg_set_calloc_function(chandle cam_arg,
                                                                 chandle calloc_function);

import "DPI-C" context function void cam_arg_set_free_function(chandle cam_arg,
                                                               chandle free_function);

import "DPI-C" context function string cam_error_string(int error_code);

import "DPI-C" context function void cam_print_error(int error_code);

import "DPI-C" context function int cam_options_create(inout chandle out_cam_options);

import "DPI-C" context function int unsigned cam_options_size();

import "DPI-C" context function void cam_options_init(inout chandle out_options);

import "DPI-C" context function void cam_options_destroy(chandle cam_options);

import "DPI-C" context function void cam_options_set_narrow_key(chandle cam_options, int narrow);

import "DPI-C" context function void cam_options_set_precomputed_hash_value(chandle cam_options,
                                                                            chandle hash_value);

import "DPI-C" context function void cam_options_clear_precomputed_hash_value(chandle cam_options);




// VCAM API
// The functions are described in the file vbcam.h
import "DPI-C" context task vbcam_create(chandle cam_arg, inout chandle out_vbcam);
import "DPI-C" context task vbcam_create_dpi(chandle cam_arg, inout chandle out_vbcam, inout int status);

import "DPI-C" context task vbcam_insert(chandle vbcam, chandle key, chandle response, chandle options);
import "DPI-C" context task vbcam_insert_dpi(chandle vbcam, chandle key, chandle response, chandle options,
                                             inout int status);

import "DPI-C" context task vbcam_update(chandle vbcam, chandle key, chandle response, chandle options);
import "DPI-C" context task vbcam_update_dpi(chandle vbcam, chandle key, chandle response, chandle options,
                                             inout int status);

import "DPI-C" context task vbcam_get_by_response(chandle vbcam, chandle response, chandle response_mask,
                                                  chandle pos, chandle out_key, chandle options);
import "DPI-C" context task vbcam_get_by_response_dpi(chandle vbcam, chandle response, chandle response_mask,
                                                      chandle pos, chandle out_key, chandle options,
                                                      inout int status);

import "DPI-C" context task vbcam_get_by_key(chandle vbcam, chandle key, chandle response, chandle options);
import "DPI-C" context task vbcam_get_by_key_dpi(chandle vbcam, chandle key, chandle response, chandle options,
                                                 inout int status);

import "DPI-C" context task vbcam_delete(chandle vbcam, chandle key, chandle options);
import "DPI-C" context task vbcam_delete_dpi(chandle vbcam, chandle key, chandle options, inout int status);

import "DPI-C" context function int vbcam_compute_hash_value(chandle vbcam, chandle key,
                                                             chandle hash_value, chandle options);

import "DPI-C" context function int vbcam_get_segment(chandle vbcam, chandle hash_value,
                                                      inout shortint unsigned segment);

import "DPI-C" context task vbcam_destroy(chandle vbcam);
import "DPI-C" context task vbcam_destroy_dpi(chandle vbcam, inout int status);

import "DPI-C" context task vbcam_read_and_clear_ecc_counters(chandle vbcam,
                                                              chandle corrected_single_bit_errors,
                                                              chandle detected_double_bit_errors);
import "DPI-C" context task vbcam_read_and_clear_ecc_addresses(chandle vbcam,
                                                               chandle failing_address_single_bit_error,
                                                               chandle failing_address_double_bit_error);
import "DPI-C" context task vbcam_set_ecc_test(chandle vbcam,
                                               int inject_single_bit_errors,
                                               int inject_double_bit_errors);



// BCAM API
// The functions are described in the file bcam.h
import "DPI-C" context task bcam_create(chandle cam_arg, inout chandle out_bcam);
import "DPI-C" context task bcam_create_dpi(chandle cam_arg, inout chandle out_bcam, inout int status);

import "DPI-C" context task bcam_destroy(chandle bcam);
import "DPI-C" context task bcam_destroy_dpi(chandle bcam, inout int status);

import "DPI-C" context task bcam_insert(chandle bcam, chandle key, chandle response, chandle options);
import "DPI-C" context task bcam_insert_dpi(chandle bcam, chandle key, chandle response, chandle options,
                                            inout int status);

import "DPI-C" context task bcam_update(chandle bcam, chandle key, chandle response, chandle options);
import "DPI-C" context task bcam_update_dpi(chandle bcam, chandle key, chandle response, chandle options,
                                            inout int status);

import "DPI-C" context task bcam_delete(chandle bcam, chandle key, chandle options);
import "DPI-C" context task bcam_delete_dpi(chandle bcam, chandle key, chandle options, inout int status);

import "DPI-C" context task bcam_get_by_key(chandle bcam, chandle key, chandle response, chandle options);
import "DPI-C" context task bcam_get_by_key_dpi(chandle bcam, chandle key, chandle response, chandle options,
                                                inout int status);

import "DPI-C" context task bcam_get_by_response(chandle bcam, chandle response, chandle response_mask,
                                                 chandle pos, chandle out_key, chandle options);
import "DPI-C" context task bcam_get_by_response_dpi(chandle bcam, chandle response,
                                                     chandle response_mask, chandle pos,
                                                     chandle out_key, chandle options, inout int status);

//import "DPI-C" context function int bcam_set_option_flags(chandle bcam, int unsigned option_flags);

//import "DPI-C" context function int unsigned bcam_get_option_flags(chandle bcam);

import "DPI-C" context task bcam_read_and_clear_ecc_counters(chandle bcam,
                                                             chandle corrected_single_bit_errors,
                                                             chandle detected_double_bit_errors);

import "DPI-C" context task bcam_read_and_clear_ecc_addresses(chandle bcam,
                                                              chandle failing_address_single_bit_error,
                                                              chandle failing_address_double_bit_error);

import "DPI-C" context task bcam_set_ecc_test(chandle bcam,
                                              int inject_single_bit_errors,
                                              int inject_double_bit_errors);



// STCAM API
// The functions are described in the file stcam.h
import "DPI-C" context task stcam_create(chandle cam_arg, inout chandle out_stcam);
import "DPI-C" context task stcam_create_dpi(chandle cam_arg, inout chandle out_stcam, inout int status);

import "DPI-C" context function void stcam_destroy(chandle stcam);

import "DPI-C" context task stcam_insert(chandle stcam, chandle key, chandle mask,
                                         int unsigned prio, chandle response);
import "DPI-C" context task stcam_insert_dpi(chandle stcam, chandle key, chandle mask,
                                             int unsigned prio, chandle response, inout int status);

import "DPI-C" context task stcam_update(chandle stcam, chandle key, chandle mask, chandle response);
import "DPI-C" context task stcam_update_dpi(chandle stcam, chandle key, chandle mask, chandle response,
                                             inout int status);

import "DPI-C" context task stcam_delete(chandle stcam, chandle key, chandle mask);
import "DPI-C" context task stcam_delete_dpi(chandle stcam, chandle key, chandle mask, inout int status);

import "DPI-C" context function int stcam_lookup(chandle stcam, chandle key, chandle response);
import "DPI-C" context task stcam_lookup_dpi(chandle stcam, chandle key, chandle response,
                                             inout int status);

import "DPI-C" context task stcam_get_by_key(chandle stcam, chandle key, chandle mask,
                                             chandle prio, chandle response);
import "DPI-C" context task stcam_get_by_key_dpi(chandle stcam, chandle key, chandle mask,
                                                 chandle prio, chandle response, inout int status);

import "DPI-C" context task stcam_get_by_response(chandle stcam, chandle response,
                                                  chandle response_mask, chandle pos,
                                                  chandle out_key, chandle out_mask);
import "DPI-C" context task stcam_get_by_response_dpi(chandle stcam, chandle response,
                                                      chandle response_mask, chandle pos,
                                                      chandle out_key, chandle out_mask,
                                                      inout int status);

import "DPI-C" context task stcam_read_and_clear_ecc_counters(chandle stcam,
                                                              chandle corrected_single_bit_errors,
                                                              chandle detected_double_bit_errors);

import "DPI-C" context task stcam_read_and_clear_ecc_addresses(chandle stcam,
                                                               chandle failing_address_single_bit_error,
                                                               chandle failing_address_double_bit_error);
import "DPI-C" context task stcam_set_ecc_test(chandle stcam,
                                               int inject_single_bit_errors,
                                               int inject_double_bit_errors);



// TCAM API
// The functions are described in the file tcam.h
import "DPI-C" context task tcam_create(chandle cam_arg, inout chandle out_tcam);
import "DPI-C" context task tcam_create_dpi(chandle cam_arg, inout chandle out_tcam, inout int status);

import "DPI-C" context task tcam_destroy(chandle tcam);
import "DPI-C" context task tcam_destroy_dpi(chandle tcam, inout int status);

import "DPI-C" context task tcam_insert(chandle tcam, chandle key, chandle mask,
                                        int unsigned prio, chandle response);
import "DPI-C" context task tcam_insert_dpi(chandle tcam, chandle key, chandle mask,
                                            int unsigned prio, chandle response, inout int status);

import "DPI-C" context task tcam_update(chandle tcam, chandle key, chandle mask, chandle response);
import "DPI-C" context task tcam_update_dpi(chandle tcam, chandle key, chandle mask, chandle response,
                                            inout int status);

import "DPI-C" context task tcam_delete(chandle tcam, chandle key, chandle mask);
import "DPI-C" context task tcam_delete_dpi(chandle tcam, chandle key, chandle mask, inout int status);

import "DPI-C" context function int tcam_lookup(chandle tcam, chandle key, chandle response);
import "DPI-C" context task tcam_lookup_dpi(chandle tcam, chandle key, chandle response,
                                            inout int status);

import "DPI-C" context task tcam_get_by_key(chandle tcam, chandle key, chandle mask,
                                            chandle prio, chandle response);
import "DPI-C" context task tcam_get_by_key_dpi(chandle tcam, chandle key, chandle mask,
                                                chandle prio, chandle response, inout int status);

import "DPI-C" context task tcam_get_by_response(chandle tcam, chandle response,
                                                 chandle response_mask, chandle pos,
                                                 chandle out_key, chandle out_mask);
import "DPI-C" context task tcam_get_by_response_dpi(chandle tcam, chandle response,
                                                     chandle response_mask, chandle pos,
                                                     chandle out_key, chandle out_mask,
                                                     inout int status);

import "DPI-C" context task tcam_read_and_clear_ecc_counters(chandle tcam,
                                                             chandle corrected_single_bit_errors,
                                                             chandle detected_double_bit_errors);

import "DPI-C" context task tcam_read_and_clear_ecc_addresses(chandle tcam,
                                                              chandle failing_address_single_bit_error,
                                                              chandle failing_address_double_bit_error);

import "DPI-C" context task tcam_set_ecc_test(chandle tcam,
                                              int inject_single_bit_errors,
                                              int inject_double_bit_errors);



// Utilities
// The functions are described in the file cam_util.[ch]
import "DPI-C" context function shortint unsigned get_key_len(string format);

import "DPI-C" context function chandle byte_arr_create(shortint unsigned bit_len);

import "DPI-C" context function void byte_arr_destroy(chandle byte_arr);

import "DPI-C" context function void str2byte_arr(shortint unsigned bit_len,
                                                  string src, chandle dest);

import "DPI-C" context function void byte_arr2str(shortint unsigned bit_len,
                                                  chandle src, inout string dst);

import "DPI-C" context function void byte_arr2bit_arr(shortint unsigned bit_len, chandle byte_arr,
                                                      inout bit[1024-1:0] bit_arr);

import "DPI-C" context function void bit_arr2byte_arr(shortint unsigned len, bit[1024-1:0] bit_arr,
                                                      chandle byte_arr);

import "DPI-C" context function chandle uint32_create();

import "DPI-C" context function void uint32_destroy(chandle num);

import "DPI-C" context function void uint32_set(chandle dst, int value);



// Helper functions used for specification of rd/wr operations.
// The function returns a pointer to the software wrapper used to call the Verilog task doing
// hardware rd/wr.
// The functions are defined in the file cam_dpi.[ch]
import "DPI-C" context function chandle get_hw_wr_function();
import "DPI-C" context function chandle get_hw_wr_function_vbcam();
import "DPI-C" context function chandle get_hw_rd_function();
import "DPI-C" context function chandle get_hw_rd_function_vbcam();
import "DPI-C" context function chandle get_hw_wr_start_function();
import "DPI-C" context function chandle get_hw_wr_stop_function();


// Optimization flags
localparam int CAM_OPTIMIZE_NONE    = 32'h00000000; // No optimizations
localparam int CAM_OPTIMIZE_RAM     = 32'h00000001; // The default hw generation balances logic and ram.
                                                    // Use this flag to reduce ram cost with a potentially higher logic cost.
                                                    // Reducing both ram and logic is not possible. Not supported for hbm
localparam int CAM_OPTIMIZE_LOGIC   = 32'h00000002; // The default hw generation balances logic and ram.
                                                    // Use this flag to reduce logic cost with a potentially higher ram cost.
                                                    // Reducing both ram and logic is not possible. Not supported for hbm
localparam int CAM_OPTIMIZE_ENTRIES = 32'h00000004; // This flag is only valid for stcam and bcam.
                                                    // The default behavior is to disallow more than num_entries to be
                                                    // inserted. This flag allows additional insertions if there is spare memory.
                                                    // When memory was allocated for num_entries, quantification effects might
                                                    // have created spare memory. For stcam, inserting excessive entries might
                                                    // compromise the number of masks that can be inserted. The stcam
                                                    // dimensioning algorithm uses a worst case distribution of entries over
                                                    // masks. The actual number of entries that can be inserted might
                                                    // be much higher than num_entries if the distribution is favorable.
localparam int CAM_OPTIMIZE_MASKS   = 32'h00000008; // This flag is only valid for stcam. The default behavior is to disallow more than num_masks to be
                                                    // inserted. This flag allows
                                                    // additional mask insertions. Inserting excessive masks may compromise
                                                    // how many entries that can be inserted.The stcam
                                                    // dimensioning algorithm uses a worst case distribution of entries over
                                                    // masks. The actual number of masks that can be inserted might
                                                    // be much higher than num_masks if the distribution is favorable.

`endif

endpackage

`endif

