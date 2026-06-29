#!/bin/bash
#source $VIVADO_ROOT/settings64.sh
source /share/Xilinx/Vivado/2023.1/settings64.sh
echo "PATH=$PATH"
echo "p4c-vitisnet => $(command -v p4c-vitisnet || echo NOT_FOUND)"
echo "run-p4bm-vitisnet => $(command -v run-p4bm-vitisnet || echo NOT_FOUND)"
type -a p4c-vitisnet || true
type -a run-p4bm-vitisnet || true

rm -rf gen
mkdir gen
p4c-vitisnet ../noop.p4 -o gen/noop.json
run-p4bm-vitisnet -j gen/forward.json -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/forward_in.user gen/behav_forward_in.pcap
python3 ../../utility/convertPcap.py src/forward_out.user gen/behav_forward_out.pcap
rm -rf src/forward_out.user
rm -rf src/forward_out.meta
