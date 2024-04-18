
#include "saveSrcDest.hpp"

addr saved_addresses[256];
ap_uint<8> curr = 0;
ap_uint<8> prev = 0;
bool first = true;


void saveSrcDest_kernel(hls::stream<src_dest_t> &user_extern_in, hls::stream<src_dest_t> &user_extern_out) {
#ifdef __VITIS_HLS__
#pragma HLS INTERFACE mode=axis port=user_extern_in
#pragma HLS INTERFACE mode=axis port=user_extern_out
#endif
	// while (1) {
#ifdef __VITIS_HLS__
#pragma HLS PIPELINE II=1
#endif
	src_dest_t in_extern = user_extern_in.read();
	addr in_src = in_extern.src;
	addr in_dest = in_extern.dest;
	saved_addresses[curr] = in_src;
	saved_addresses[curr+1] = in_dest;
	if (first) {
		first = false;
		in_extern.src = 0;
		in_extern.dest = 0;
		curr += 2;
		user_extern_out.write(in_extern);
	}else {
		in_extern.src = saved_addresses[prev];
		in_extern.dest = saved_addresses[prev+1];
		curr += 2;
		prev += 2;
		user_extern_out.write(in_extern);
	}
}
