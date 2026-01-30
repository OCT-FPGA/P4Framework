#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../forward.p4 -o gen/forward.json
run-p4bm-vitisnet -j gen/forward.json -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/forward_in.user gen/behav_forward_in.pcap
python3 ../../utility/convertPcap.py src/forward_out.user gen/behav_forward_out.pcap
rm -rf src/forward_out.user
rm -rf src/forward_out.meta
