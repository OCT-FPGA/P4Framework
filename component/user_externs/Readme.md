# UserExtern Component Developent (Inherited from VitisNetP4 Exampl)
## Run
set XILINX\_VIVADO and XILINX\_VITIS as Vivado and Vitis_HLS installation path. For example:

 ``` 
    export XILINX_VIVADO=/opt/Xilinx/Vivado/2021.2
    export XILINX_VITIS=/opt/Xilinx/Vitis_HLS/2021.2 
 ``` 

And then run the following commands:
```
export CXX=$XILINX\_VIVADO/tps/lnx64/gcc-6.2.0/bin/g++
cmake . -B build
cd build && make 
```


