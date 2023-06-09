#!/bin/bash
source $VIVADO_ROOT/settings64.sh
rm -rf gen
mkdir gen
p4c-vitisnet ../advCalc.p4 -o gen/advCalc.json
run-p4bm-vitisnet -j gen/advCalc.json --load-modules src/advan_calc_user_externs.so -s src/cli_commands.txt
python3 ../../utility/convertPcap.py src/advCalc_in.user gen/behav_advCalc_in.pcap
python3 ../../utility/convertPcap.py src/advCalc_out.user gen/behav_advCalc_out.pcap
rm -rf src/advCalc_out.user
rm -rf src/advCalc_out.meta
