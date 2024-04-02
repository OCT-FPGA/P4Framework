cmake_minimum_required(VERSION 3.14)
project(exchangeFivetuple_hls_wrapper)

message("$ENV{XILINX_VITIS}")
set(XILINX_VITIS "$ENV{XILINX_VITIS}")
set(g++_VER 6.2.0)
set(CMAKE_CXX_COMPILER ${XILINX_VITIS}/tps/lnx64/gcc-${g++_VER}/bin/g++)
set(CMAKE_CXX_FLAGS "-DBMLOG_DEBUG_ON -DBMLOG_TRACE_ON -Wall -Wextra -pthread -g -O2 -fPIC")

set(SYS_INCLUDE_DIR ${XILINX_VITIS}/tps/lnx64/gcc-${g++_VER}/include)
set(INCLUDE_DIR include ${XILINX_VITIS}/lnx64/tools/autocc ${XILINX_VITIS}/include) 


file(GLOB SRC_FILE src/*.cpp)
set(TARGET_OBJECT exchangeFivetuple_hls_wrapper)

# Target
add_library(${TARGET_OBJECT} OBJECT ${SRC_FILE})
target_include_directories(${TARGET_OBJECT} SYSTEM PUBLIC ${SYS_INCLUDE_DIR})
target_include_directories(${TARGET_OBJECT} PRIVATE ${INCLUDE_DIR})
