# Getting started with building P4 bitstreams on OCT

Some initial setup is required on OCT machines before building bitstreams as mentioned in [P4Framework](https://github.com/OCT-FPGA/P4Framework/tree/master).


## Verify vitis_net ip

Run `ls -d /fpga/Xilinx/Vivado/2023.1/data/ip/xilinx/vitis_net_p4* 2>/dev/null` 

This should output something like:

`/fpga/Xilinx/Vivado/2023.1/data/ip/xilinx/vitis_net_p4_v1_3` indicating VitisNetIP is present for the Vivado version 2023.1.

## Setup Vivado environment

Run `source /fpga/Xilinx/Vivado/2023.1/settings64.sh` 

Verify Vivado version 2023.1.

`which vivado`

`vivado -version | head`

## Update the license 

Run `export XILINXD_LICENSE_FILE=2100@octlm` 

## Verify that the Makefile in P4Framework has the right Vivado version:

`VIVADO_TARGET_VER=2023.1`

## To get BMv2 working do the following installations ([Reference](https://github.com/OCT-FPGA/P4Framework/tree/master/Examples/forward/behav_test))

`sudo apt-get update && sudo apt-get install libgmpxx4ldbl`

`find /fpga/Xilinx/Vivado/2023.1 -name "libthrift*" 2>/dev/null`

`echo "/fpga/Xilinx/Vivado/2023.1/lib/lnx64.o/Ubuntu/20" | sudo tee /etc/ld.so.conf.d/vivado-thrift.conf`

`sudo ldconfig`

`ldconfig -p | grep thrift`

`wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1f-1ubuntu2.24_amd64.deb`

`sudo dpkg -i libssl1.1_1.1.1f-1ubuntu2.24_amd64.deb`

`sudo apt install wireshark`



Now the steps mentioned in [P4Framework](https://github.com/OCT-FPGA/P4Framework/tree/master) can be followed to build P4 bitstreams.
