cmake_minimum_required(VERSION 3.14)
message("CMAKE Version is ${CMAKE_VERSION}")
project(User_externs)

set(XILINX_VIVADO $ENV{XILINX_VIVADO})
set(XILINX_VITIS $ENV{XILINX_VITIS})
set(HLS_FUNC_FOLDER ${CMAKE_SOURCE_DIR}/../hls/saveSrcDestV3)
set(CMAKE_CXX_FLAGS "-DBMLOG_DEBUG_ON -DBMLOG_TRACE_ON -Wall -Wextra -pthread -g -O2 -fPIC")
set(SYS_INCLUDE_DIR ${XILINX_VIVADO}/tps/lnx64/gcc-6.2.0/include)
set(INCLUDE_DIR include include/spdlog ${XILINX_VIVADO}/include ${XILINX_VIVADO}/tps/boost_1_72_0 ${HLS_FUNC_FOLDER}/include ${XILINX_VITIS}/include)


file(GLOB SRC_FILE src/*.cpp)
set(TARGET_OBJECT advan_calc_user_externs)

# Target
add_subdirectory(${HLS_FUNC_FOLDER} saveSrcDest)
add_library(${TARGET_OBJECT} SHARED ${SRC_FILE})
target_include_directories(${TARGET_OBJECT} SYSTEM PUBLIC ${SYS_INCLUDE_DIR})
target_include_directories(${TARGET_OBJECT} PRIVATE ${INCLUDE_DIR})
target_link_libraries(${TARGET_OBJECT} saveSrcDest_hls_wrapper)
