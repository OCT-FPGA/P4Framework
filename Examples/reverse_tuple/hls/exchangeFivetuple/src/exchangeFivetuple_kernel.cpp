#include "exchangeFivetuple.hpp"

void exchangeFivetuple_kernel(hls::stream<five_tuples_t> &user_extern_in, hls::stream<five_tuples_t> &user_extern_out)
{
#ifdef __VITIS_HLS__
#pragma HLS INTERFACE mode=axis port=user_extern_in
#pragma HLS INTERFACE mode=axis port=user_extern_out
#endif
//	while (1){
#ifdef __VITIS_HLS__
#pragma HLS PIPELINE  II=1
#endif
		five_tuples_t in_extern = user_extern_in.read();
		ap_uint<32> tmp_addr = in_extern.dst;
		ap_uint<16> tmp_port = in_extern.dstPort;
		in_extern.dst = in_extern.src;
		in_extern.src = tmp_addr;
		in_extern.dstPort = in_extern.srcPort;
		in_extern.srcPort = tmp_port;
		user_extern_out.write(in_extern);
//	}
}
