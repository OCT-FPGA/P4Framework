#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../fiveTuple.p4 -o gen/fiveTuple.json
run-p4bm-vitisnet -j gen/fiveTuple.json --load-modules src/new_extern.so -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/traffic_in.user gen/behav_traffic_in.pcap
python3 ../../utility/convertPcap.py src/traffic_out.user gen/behav_traffic_out.pcap
rm -rf src/traffic_out.user
rm -rf src/traffic_out.meta
