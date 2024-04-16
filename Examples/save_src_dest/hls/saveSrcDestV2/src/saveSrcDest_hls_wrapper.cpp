#include "saveSrcDest.hpp"

void saveSrcDest_hls_wrapper(addr *src, addr *dest, addr *src_out, addr *dest_out) {
	hls::stream<addr> in1_stream;
	hls::stream<addr> in2_stream;
	hls::stream<addr> out1_stream;
	hls::stream<addr> out2_stream;
	std::cout << "Entering wrapper" << std::endl;

	in1_stream.write(*src);
	in2_stream.write(*dest);

	saveSrcDest_kernel(in1_stream, in2_stream, out1_stream, out2_stream);
	*src_out = out1_stream.read();
	*dest_out = out2_stream.read();
}
