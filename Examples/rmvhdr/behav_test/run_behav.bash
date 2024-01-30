#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../rmvhdr.p4 -o gen/rmvhdr.json
run-p4bm-vitisnet -j gen/rmvhdr.json -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/rmvhdr_in.user gen/behav_rmvhdr_in.pcap
python3 ../../utility/convertPcap.py src/rmvhdr_out.user gen/behav_rmvhdr_out.pcap
rm -rf src/rmvhdr_out.user
rm -rf src/rmvhdr_out.meta
