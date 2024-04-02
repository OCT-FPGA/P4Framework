#!/bin/bash
# build external object
export XILINX_VITIS=/opt/Xilinx/Vitis_HLS/2021.2
export XILINX_VIVADO=/opt/Xilinx/Vivado/2021.2

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
p4c-vitisnet ../reverse_tuple.p4 -o gen/reverse_tuple.json
run-p4bm-vitisnet -j gen/reverse_tuple.json --load-modules extern_lib_build/libadvan_calc_user_externs.so -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/reverse_tuple_in.user gen/behav_reverse_tuple_in.pcap
python3 ../../utility/convertPcap.py src/reverse_tuple_out.user gen/behav_reverse_tuple_out.pcap
rm -rf src/reverse_tuple_out*
