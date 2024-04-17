#include<hls_stream.h>
#include<iostream>
#include"addr_types.h"

void saveSrcDest_hls_wrapper(addr *src, addr *dest, addr *src_out, addr *dest_out);
void saveSrcDest_kernel(hls::stream<addr> &src, hls::stream<addr> &dest,
		hls::stream<addr> &src_out, hls::stream<addr> &dest_out);
