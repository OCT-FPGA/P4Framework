#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../addhdr.p4 -o gen/addhdr.json
run-p4bm-vitisnet -j gen/addhdr.json -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/addhdr_in.user gen/behav_addhdr_in.pcap
python3 ../../utility/convertPcap.py src/addhdr_out.user gen/behav_addhdr_out.pcap
rm -rf src/addhdr_out.user
rm -rf src/addhdr_out.meta
