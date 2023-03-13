# P4-OpenNIC Framework
This framework integrates the VitisNetP4 with Xilinx's OpenNIC shell. This framework will utilize Xilinx's VitisNetP4 compiler to compile the P4 codes into hardware IP. This hardware IP will then be instantiated as a box_plugin for the OpenNIC shell. 

In addition, we provide three examples to demonstrate the usage of this framework.

Note: Use `git submodule update --init` to update the submodule open-nic shell

## Examples 
We provide three examples: forward, calc and advCalc. These examples are all inherited from VitisNetP4 Examples.

Make sure your Vivado version is 2021.2. Or you can modify the version in the  `Makefile`. However, this may raise other IP version issues. 

Make sure you have sdnet_p4 and Tcam licenses available for your Vivado. 

Run `source $(YOUR_VIVADO_ROOT)/Vivado/2021.2/bin/settings64.sh` and make sure your Vivado is runnable by `vivado -version`

Run `make forward` or `make calc` or `make advCalc` to build the examples. 



### Options for build
You can set arguments `impl=0 synth_ip=0 post_impl=0` to only create the design but not build it. 

### Option for JTAG or PCIe programming
The generated bitstreams can either be programmed through PCIe bus or JTAG. 

Use argument `qspi_support=0` to enable JTAG programming and `qspi_support=1` for PCIe programming. The default is `qspi_support=1`.  


## template Plugin 

Use `templatePlugin` to develop your own P4 plugin. 

The `build_box_250mhz.tcl` file is used for instructing Vivado to generate the VitisNetP4 IP from the P4 code.

The `p2p_250mhz.sv` is to define the interface between opennic shell and VitisNetP4 IP. It doesn't need to change if your IP doesn't have the extern functions. 

Use `make shell custom_plguin=templatePlugin` to build. 
