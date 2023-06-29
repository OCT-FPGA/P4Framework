#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../reverse_tuple.p4 -o gen/reverse_tuple.json
run-p4bm-vitisnet -j gen/reverse_tuple.json --load-modules src/libadvan_calc_user_externs.so -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/reverse_tuple_in.user gen/behav_reverse_tuple_in.pcap
python3 ../../utility/convertPcap.py src/reverse_tuple_out.user gen/behav_reverse_tuple_out.pcap
rm -rf src/reverse_tuple_out*
