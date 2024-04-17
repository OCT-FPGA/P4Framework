
#include "saveSrcDest.hpp"

addr saved_addresses[256];
ap_uint<8> curr = 0;
ap_uint<8> prev = 0;
bool first = true;


void saveSrcDest_kernel(hls::stream<addr> &src, hls::stream<addr> &dest, hls::stream<addr> &src_out, hls::stream<addr> &dest_out) {
#ifdef __VITIS_HLS__
#pragma HLS INTERFACE mode=axis port=src
#pragma HLS INTERFACE mode=axis port=dest
#pragma HLS INTERFACE mode=axis port=src_out
#pragma HLS INTERFACE mode=axis port=dest_out
#endif
	// while (1) {
#ifdef __VITIS_HLS__
#pragma HLS PIPELINE II=1
#endif
	addr in_src = src.read();
	addr in_dest = dest.read();
	saved_addresses[curr] = in_src;
	saved_addresses[curr+1] = in_dest;
	if (first) {
		first = false;
		src_out.write(0);
		dest_out.write(0);
		curr += 2;
	}else {
		src_out.write(saved_addresses[prev]);
		dest_out.write(saved_addresses[prev+1]);
		curr += 2;
		prev += 2;
	}
}
