`timescale 1 ns / 1 ps

module AESL_deadlock_kernel_monitor_top ( 
    input wire kernel_monitor_clock,
    input wire kernel_monitor_reset
);
wire [1:0] axis_block_sigs;
wire [0:0] inst_idle_sigs;
wire [0:0] inst_block_sigs;
wire kernel_block;

assign axis_block_sigs[0] = ~AESL_inst_saveSrcDest_kernel.user_extern_in_TDATA_blk_n;
assign axis_block_sigs[1] = ~AESL_inst_saveSrcDest_kernel.user_extern_out_TDATA_blk_n;

assign inst_block_sigs[0] = 1'b0;

assign inst_idle_sigs[0] = 1'b0;

AESL_deadlock_idx0_monitor AESL_deadlock_idx0_monitor_U (
    .clock(kernel_monitor_clock),
    .reset(kernel_monitor_reset),
    .axis_block_sigs(axis_block_sigs),
    .inst_idle_sigs(inst_idle_sigs),
    .inst_block_sigs(inst_block_sigs),
    .block(kernel_block)
);

// synthesis translate_off
// logic for axis port block diagnosis
reg [1:0] rpt_source_cnt;
initial begin : rpt_block_cnt
    rpt_source_cnt = 2'h0;
    wait(kernel_monitor_reset == 1'b1);
    repeat(10)
        @(posedge kernel_monitor_clock);
    while(!kernel_block || rpt_source_cnt != 2'h2)
    begin
        @(posedge kernel_monitor_clock);
        if (kernel_block)
            rpt_source_cnt = rpt_source_cnt + 2'h1;
    end
end

// synthesis translate_on

parameter RPT_IDLE = 4'b0001;
parameter RPT_COLLECT_DST = 4'b0010;
parameter RPT_COLLECT_SRC = 4'b0100;
parameter RPT_DONE = 4'b1000;
reg [3:0] rpt_cur_st;
reg [3:0] rpt_next_st;
reg rpt_enable;

always @(posedge kernel_monitor_clock) begin
    if (kernel_monitor_reset == 1'b1)
        rpt_enable <= 1'b1;
    else if (rpt_cur_st == RPT_IDLE && rpt_source_cnt == 2'h2)
        rpt_enable <= 1'b0;
    else ;
end

always @(posedge kernel_monitor_clock) begin
    if (kernel_monitor_reset == 1'b1)
        rpt_cur_st <= RPT_IDLE;
    else
        rpt_cur_st <= rpt_next_st;
end

always @(*) begin
    case(rpt_cur_st)
        RPT_IDLE: begin
            if (kernel_block == 1'b1 && rpt_enable == 1'b1)
                rpt_next_st = RPT_COLLECT_DST;
            else
                rpt_next_st = RPT_IDLE;
        end
        RPT_COLLECT_DST: rpt_next_st = RPT_COLLECT_SRC;
        RPT_COLLECT_SRC: rpt_next_st = RPT_DONE;
        RPT_DONE: rpt_next_st = RPT_IDLE;
        default: rpt_next_st = RPT_IDLE;
    endcase
end

reg [1:0] rpt_P0_block_info_dst;
reg [1:0] rpt_P0_block_info_src;

always @(posedge kernel_monitor_clock) begin
    if (kernel_monitor_reset == 1'b1)
        rpt_P0_block_info_dst <= 2'h0;
    else if (rpt_cur_st == RPT_COLLECT_DST)
        rpt_P0_block_info_dst <= ~axis_block_sigs;
end

always @(posedge kernel_monitor_clock) begin
    if (kernel_monitor_reset == 1'b1)
        rpt_P0_block_info_src <= 2'h0;
    else if (rpt_cur_st == RPT_COLLECT_SRC)
        rpt_P0_block_info_src <= axis_block_sigs;
end


// synthesis translate_off

initial begin : axis_deadlock_report
integer fp;
integer  axis_port_scan_flag;
integer  cnt;
    cnt = 0;
    fp = $fopen("kernel_deadlock_diagnosis.rpt","a");
    $fdisplay(fp,"//// start to dump deadlock path for kernel '%m' ////");
    wait(kernel_monitor_reset == 1'b1);
        axis_port_scan_flag = 0;
    repeat(10)
        @(posedge kernel_monitor_clock);
    while(!kernel_block || cnt != 2)
    begin
        @(posedge kernel_monitor_clock);
        if (kernel_block)
            cnt = cnt + 1;
    end

    $display("\n//////////////////////////////////////////////////////////////////////////////");
    $display("// Following axis ports can not be accessed by kernel 'saveSrcDest_kernel' ");
    $fdisplay(fp,"\n//////////////////////////////////////////////////////////////////////////////");
    $fdisplay(fp,"// Following axis ports can not be accessed by kernel 'saveSrcDest_kernel' ");
    // find all axis ports which have no outer block
    while (axis_port_scan_flag < 2) begin
        case(axis_port_scan_flag)
            0:
            begin
                if (axis_block_sigs[0] == 1'b0) begin
                    $display("//    port 'user_extern_in' can not be read");
                    $fdisplay(fp,"//    port 'user_extern_in' can not be read");
                end
            end
            1:
            begin
                if (axis_block_sigs[1] == 1'b0) begin
                    $display("//    port 'user_extern_out' can not be written");
                    $fdisplay(fp,"//    port 'user_extern_out' can not be written");
                end
            end
        endcase
        axis_port_scan_flag = axis_port_scan_flag + 1;
    end

    axis_port_scan_flag = 0;
                    $display("//");
                    $fdisplay(fp,"//");
    $display("//    Because the top pipeline FSM is blocked by following axis ports");
    $fdisplay(fp,"//    Because the top pipeline FSM is blocked by following axis ports");
    // find all axis ports which have outer block
    while (axis_port_scan_flag < 2) begin
        case(axis_port_scan_flag)
            0:
            begin
                if (axis_block_sigs[0] == 1'b1) begin
                    $display("//        port 'user_extern_in' does not have valid input data");
                    $fdisplay(fp,"//        port 'user_extern_in' does not have valid input data");
                end
            end
            1:
            begin
                if (axis_block_sigs[1] == 1'b1) begin
                    $display("//        port 'user_extern_out' is not ready");
                    $fdisplay(fp,"//        port 'user_extern_out' is not ready");
                end
            end
        endcase
        axis_port_scan_flag = axis_port_scan_flag + 1;
    end

    $display("//////////////////////////////////////////////////////////////////////////////\n");
    $fdisplay(fp,"//////////////////////////////////////////////////////////////////////////////\n");
    $fdisplay(fp,"//// finish dumping deadlock path for kernel '%m' ////\n");
    $fclose(fp);
end

// synthesis translate_on

initial begin : trigger_axis_deadlock
reg block_delay;
    block_delay = 0;
    while(1) begin
        @(posedge kernel_monitor_clock);
    if (kernel_block == 1'b1 && block_delay == 1'b0)
        block_delay = kernel_block;
    end
end

endmodule
