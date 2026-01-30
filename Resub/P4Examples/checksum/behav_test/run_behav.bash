#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../checksum.p4 -o gen/checksum.json
run-p4bm-vitisnet -j gen/checksum.json -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/checksum_in.user gen/behav_checksum_in.pcap
python3 ../../utility/convertPcap.py src/checksum_out.user gen/behav_checksum_out.pcap
rm -rf src/checksum_out.user
rm -rf src/checksum_out.meta
