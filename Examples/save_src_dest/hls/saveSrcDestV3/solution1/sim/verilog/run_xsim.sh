
/tools/Xilinx/Vivado/2023.1/bin/xelab xil_defaultlib.apatb_saveSrcDest_kernel_top glbl -Oenable_linking_all_libraries  -prj saveSrcDest_kernel.prj -L smartconnect_v1_0 -L axi_protocol_checker_v1_1_12 -L axi_protocol_checker_v1_1_13 -L axis_protocol_checker_v1_1_11 -L axis_protocol_checker_v1_1_12 -L xil_defaultlib -L unisims_ver -L xpm  -L floating_point_v7_1_15 -L floating_point_v7_0_20 --lib "ieee_proposed=./ieee_proposed" -s saveSrcDest_kernel 
/tools/Xilinx/Vivado/2023.1/bin/xsim --noieeewarnings saveSrcDest_kernel -tclbatch saveSrcDest_kernel.tcl 

