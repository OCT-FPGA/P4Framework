#!/bin/bash
# build external object
export XILINX_VITIS=/tools/Xilinx/Vitis_HLS/2021.2
export XILINX_VIVADO=/tools/Xilinx/Vivado/2021.2

if [ -z "$variable" ]; then
    echo "Variable is unset or null"
else
    echo "Variable is set to: $variable"
fi

rm -rf extern_lib_build
mkdir extern_lib_build
cd extern_lib_build
cmake ../../user_externs/
make
cd ..

source $XILINX_VIVADO/settings64.sh
echo $XILINX_VIVADO
mkdir gen
p4c-vitisnet ../save_src_dest.p4 -o gen/save_src_dest.json
run-p4bm-vitisnet -j gen/save_src_dest.json --load-modules extern_lib_build/libadvan_calc_user_externs.so -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/save_src_dest_in.user gen/behav_save_src_dest_in.pcap
python3 ../../utility/convertPcap.py src/save_src_dest_out.user gen/behav_save_src_dest_out.pcap
rm -rf src/save_src_dest_out*
