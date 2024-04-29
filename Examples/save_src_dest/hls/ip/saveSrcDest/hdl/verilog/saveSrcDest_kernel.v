// ==============================================================
// Generated by Vitis HLS v2023.1
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// ==============================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="saveSrcDest_kernel_saveSrcDest_kernel,hls_ip_2023_1,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcu280-fsvh2892-2L-e,HLS_INPUT_CLOCK=2.000000,HLS_INPUT_ARCH=pipeline,HLS_SYN_CLOCK=0.000000,HLS_SYN_LAT=1,HLS_SYN_TPT=1,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=2,HLS_SYN_LUT=26,HLS_VERSION=2023_1}" *)

module saveSrcDest_kernel (
        ap_clk,
        ap_rst_n,
        ap_start,
        ap_done,
        ap_idle,
        ap_ready,
        user_extern_in_TVALID,
        user_extern_out_TREADY,
        user_extern_in_TDATA,
        user_extern_in_TREADY,
        user_extern_out_TDATA,
        user_extern_out_TVALID
);

parameter    ap_ST_fsm_pp0_stage0 = 1'd1;

input   ap_clk;
input   ap_rst_n;
input   ap_start;
output   ap_done;
output   ap_idle;
output   ap_ready;
input   user_extern_in_TVALID;
input   user_extern_out_TREADY;
input  [63:0] user_extern_in_TDATA;
output   user_extern_in_TREADY;
output  [63:0] user_extern_out_TDATA;
output   user_extern_out_TVALID;

reg ap_done;
reg ap_idle;
reg ap_ready;

 reg    ap_rst_n_inv;
(* fsm_encoding = "none" *) reg   [0:0] ap_CS_fsm;
wire    ap_CS_fsm_pp0_stage0;
wire    ap_enable_reg_pp0_iter0;
reg    ap_enable_reg_pp0_iter1;
reg    ap_idle_pp0;
reg    ap_block_state1_pp0_stage0_iter0;
wire    regslice_both_user_extern_out_U_apdone_blk;
reg    ap_block_state2_pp0_stage0_iter1;
reg    ap_block_pp0_stage0_subdone;
reg    user_extern_in_TDATA_blk_n;
wire    ap_block_pp0_stage0;
reg    user_extern_out_TDATA_blk_n;
reg    ap_block_pp0_stage0_11001;
reg    ap_block_pp0_stage0_01001;
reg   [0:0] ap_NS_fsm;
reg    ap_idle_pp0_0to0;
reg    ap_reset_idle_pp0;
wire    ap_enable_pp0;
wire    regslice_both_user_extern_in_U_apdone_blk;
wire   [63:0] user_extern_in_TDATA_int_regslice;
wire    user_extern_in_TVALID_int_regslice;
reg    user_extern_in_TREADY_int_regslice;
wire    regslice_both_user_extern_in_U_ack_in;
reg    user_extern_out_TVALID_int_regslice;
wire    user_extern_out_TREADY_int_regslice;
wire    regslice_both_user_extern_out_U_vld_out;
wire    ap_ce_reg;

// power-on initialization
initial begin
#0 ap_CS_fsm = 1'd1;
#0 ap_enable_reg_pp0_iter1 = 1'b0;
end

saveSrcDest_kernel_regslice_both #(
    .DataWidth( 64 ))
regslice_both_user_extern_in_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(user_extern_in_TDATA),
    .vld_in(user_extern_in_TVALID),
    .ack_in(regslice_both_user_extern_in_U_ack_in),
    .data_out(user_extern_in_TDATA_int_regslice),
    .vld_out(user_extern_in_TVALID_int_regslice),
    .ack_out(user_extern_in_TREADY_int_regslice),
    .apdone_blk(regslice_both_user_extern_in_U_apdone_blk)
);

saveSrcDest_kernel_regslice_both #(
    .DataWidth( 64 ))
regslice_both_user_extern_out_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(user_extern_in_TDATA_int_regslice),
    .vld_in(user_extern_out_TVALID_int_regslice),
    .ack_in(user_extern_out_TREADY_int_regslice),
    .data_out(user_extern_out_TDATA),
    .vld_out(regslice_both_user_extern_out_U_vld_out),
    .ack_out(user_extern_out_TREADY),
    .apdone_blk(regslice_both_user_extern_out_U_apdone_blk)
);

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_pp0_stage0;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        ap_enable_reg_pp0_iter1 <= 1'b0;
    end else begin
        if (((1'b0 == ap_block_pp0_stage0_subdone) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
            ap_enable_reg_pp0_iter1 <= ap_start;
        end
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_subdone) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        ap_done = 1'b1;
    end else begin
        ap_done = 1'b0;
    end
end

always @ (*) begin
    if (((ap_start == 1'b0) & (ap_idle_pp0 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        ap_idle = 1'b1;
    end else begin
        ap_idle = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b0) & (ap_enable_reg_pp0_iter0 == 1'b0))) begin
        ap_idle_pp0 = 1'b1;
    end else begin
        ap_idle_pp0 = 1'b0;
    end
end

always @ (*) begin
    if ((ap_enable_reg_pp0_iter0 == 1'b0)) begin
        ap_idle_pp0_0to0 = 1'b1;
    end else begin
        ap_idle_pp0_0to0 = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_subdone) & (ap_start == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        ap_ready = 1'b1;
    end else begin
        ap_ready = 1'b0;
    end
end

always @ (*) begin
    if (((ap_start == 1'b0) & (ap_idle_pp0_0to0 == 1'b1))) begin
        ap_reset_idle_pp0 = 1'b1;
    end else begin
        ap_reset_idle_pp0 = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0) & (ap_start == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        user_extern_in_TDATA_blk_n = user_extern_in_TVALID_int_regslice;
    end else begin
        user_extern_in_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_start == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        user_extern_in_TREADY_int_regslice = 1'b1;
    end else begin
        user_extern_in_TREADY_int_regslice = 1'b0;
    end
end

always @ (*) begin
    if ((((1'b0 == ap_block_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((1'b0 == ap_block_pp0_stage0) & (ap_start == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)))) begin
        user_extern_out_TDATA_blk_n = user_extern_out_TREADY_int_regslice;
    end else begin
        user_extern_out_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_start == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        user_extern_out_TVALID_int_regslice = 1'b1;
    end else begin
        user_extern_out_TVALID_int_regslice = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_pp0_stage0 : begin
            ap_NS_fsm = ap_ST_fsm_pp0_stage0;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign ap_CS_fsm_pp0_stage0 = ap_CS_fsm[32'd0];

assign ap_block_pp0_stage0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_pp0_stage0_01001 = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((regslice_both_user_extern_out_U_apdone_blk == 1'b1) | (user_extern_out_TREADY_int_regslice == 1'b0))) | ((ap_start == 1'b1) & ((user_extern_out_TREADY_int_regslice == 1'b0) | (user_extern_in_TVALID_int_regslice == 1'b0))));
end

always @ (*) begin
    ap_block_pp0_stage0_11001 = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((regslice_both_user_extern_out_U_apdone_blk == 1'b1) | (user_extern_out_TREADY_int_regslice == 1'b0))) | ((ap_start == 1'b1) & ((user_extern_out_TREADY_int_regslice == 1'b0) | (user_extern_in_TVALID_int_regslice == 1'b0))));
end

always @ (*) begin
    ap_block_pp0_stage0_subdone = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((regslice_both_user_extern_out_U_apdone_blk == 1'b1) | (user_extern_out_TREADY_int_regslice == 1'b0))) | ((ap_start == 1'b1) & ((user_extern_out_TREADY_int_regslice == 1'b0) | (user_extern_in_TVALID_int_regslice == 1'b0))));
end

always @ (*) begin
    ap_block_state1_pp0_stage0_iter0 = ((user_extern_out_TREADY_int_regslice == 1'b0) | (user_extern_in_TVALID_int_regslice == 1'b0));
end

always @ (*) begin
    ap_block_state2_pp0_stage0_iter1 = ((regslice_both_user_extern_out_U_apdone_blk == 1'b1) | (user_extern_out_TREADY_int_regslice == 1'b0));
end

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

assign ap_enable_reg_pp0_iter0 = ap_start;

always @ (*) begin
    ap_rst_n_inv = ~ap_rst_n;
end

assign user_extern_in_TREADY = regslice_both_user_extern_in_U_ack_in;

assign user_extern_out_TVALID = regslice_both_user_extern_out_U_vld_out;


reg find_kernel_block = 0;
// synthesis translate_off
`include "saveSrcDest_kernel_hls_deadlock_kernel_monitor_top.vh"
// synthesis translate_on

endmodule //saveSrcDest_kernel

