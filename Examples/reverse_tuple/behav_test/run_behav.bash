#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../reverse_tuple.p4 -o gen/sketch.json
run-p4bm-vitisnet -j gen/sketch.json -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/sketch_in.user gen/behav_forward_in.pcap
python3 ../../utility/convertPcap.py src/sketch_out.user gen/behav_forward_out.pcap
rm -rf src/sketch_out.user
rm -rf src/sketch_out.meta
