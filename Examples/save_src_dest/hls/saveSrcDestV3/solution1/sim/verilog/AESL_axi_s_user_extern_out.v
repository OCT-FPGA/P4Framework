// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.1 (64-bit)
// Tool Version Limit: 2023.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================

`timescale 1 ns / 1 ps

`define TV_OUT_user_extern_out_TDATA "../tv/rtldatafile/rtl.saveSrcDest_kernel.autotvout_user_extern_out.dat"

`define AUTOTB_TRANSACTION_NUM 300

module AESL_axi_s_user_extern_out (
    input clk,
    input reset,
    input [64 - 1:0] TRAN_user_extern_out_TDATA,
    input TRAN_user_extern_out_TVALID,
    output TRAN_user_extern_out_TREADY,
    input ready,
    input done,
    output [31:0] transaction);

    wire TRAN_user_extern_out_TVALID_temp;
    wire user_extern_out_TDATA_full;
    wire user_extern_out_TDATA_empty;
    reg user_extern_out_TDATA_write_en;
    reg [64 - 1:0] user_extern_out_TDATA_write_data;
    reg user_extern_out_TDATA_read_en;
    wire [64 - 1:0] user_extern_out_TDATA_read_data;
    
    fifo #(1, 64) fifo_user_extern_out_TDATA (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(user_extern_out_TDATA_write_en),
        .write_data(user_extern_out_TDATA_write_data),
        .read_clock(clk),
        .read_en(user_extern_out_TDATA_read_en),
        .read_data(user_extern_out_TDATA_read_data),
        .full(user_extern_out_TDATA_full),
        .empty(user_extern_out_TDATA_empty));
    
    always @ (*) begin
        user_extern_out_TDATA_write_en <= TRAN_user_extern_out_TVALID;
        user_extern_out_TDATA_write_data <= TRAN_user_extern_out_TDATA;
        user_extern_out_TDATA_read_en <= 0;
    end
    assign TRAN_user_extern_out_TVALID = TRAN_user_extern_out_TVALID_temp;

    
    assign TRAN_user_extern_out_TREADY = ~(user_extern_out_TDATA_full);
    
    function is_blank_char(input [7:0] in_char);
        if (in_char == " " || in_char == "\011" || in_char == "\012" || in_char == "\015") begin
            is_blank_char = 1;
        end else begin
            is_blank_char = 0;
        end
    endfunction
    
    function [199:0] read_token(input integer fp);
        integer ret;
        begin
            read_token = "";
                    ret = 0;
                    ret = $fscanf(fp,"%s",read_token);
        end
    endfunction
    
    function [199:0] rm_0x(input [199:0] token);
        reg [199:0] token_tmp;
        integer i;
        begin
            token_tmp = "";
            for (i = 0; token[15:0] != "0x"; token = token >> 8) begin
                token_tmp = (token[7:0] << (8 * i)) | token_tmp;
                i = i + 1;
            end
            rm_0x = token_tmp;
        end
    endfunction
    
    reg done_1;
    
    always @ (posedge clk or reset) begin
        if (~reset) begin
            done_1 <= 0;
        end else begin
            done_1 <= done;
        end
    end
    
    reg [31:0] transaction_save_user_extern_out_TDATA;
    
    assign transaction = transaction_save_user_extern_out_TDATA;
    
    initial begin : AXI_stream_receiver_user_extern_out_TDATA
        integer fp;
        reg [64 - 1:0] data;
        reg [8 * 5:1] str;
        
        transaction_save_user_extern_out_TDATA = 0;
        fifo_user_extern_out_TDATA.clear();
        wait (reset === 1);
        forever begin
            @ (negedge clk);
            if (done_1 == 1) begin
                fp = $fopen(`TV_OUT_user_extern_out_TDATA, "a");
                if (fp == 0) begin // Failed to open file
                    $display("ERROR: Failed to open file \"%s\"!", `TV_OUT_user_extern_out_TDATA);
                    $finish;
                end
                $fdisplay(fp, "[[transaction]] %d", transaction_save_user_extern_out_TDATA);
                while (~fifo_user_extern_out_TDATA.empty) begin
                    fifo_user_extern_out_TDATA.pop(data);
                    $fdisplay(fp, "0x%x", data);
                end
                $fdisplay(fp, "[[/transaction]]");
                transaction_save_user_extern_out_TDATA = transaction_save_user_extern_out_TDATA + 1;
                fifo_user_extern_out_TDATA.clear();
                $fclose(fp);
            end
        end
    end

endmodule
