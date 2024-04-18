#include "saveSrcDest.hpp"
#include "addr_types.h"

void saveSrcDest_hls_wrapper(src_dest_t *sd_in, src_dest_t *sd_out) {
	hls::stream<src_dest_t> in_stream;
	hls::stream<src_dest_t> out_stream;
	std::cout << "Entering wrapper" << std::endl;

	in_stream.write(*sd_in);

	saveSrcDest_kernel(in_stream, out_stream);
	*sd_out = out_stream.read();
}
