#include "exchangeFivetuple.hpp"

void exchangeFivetuple_hls_wrapper(five_tuples_t *tuple_in, five_tuples_t *tuple_out)
{
    hls::stream<five_tuples_t> in_stream;
    hls::stream<five_tuples_t> out_stream;
    std::cout << "Entering wrapper" << std::endl;
    
    in_stream.write(*tuple_in);

    exchangeFivetuple_kernel(in_stream, out_stream);
    *tuple_out=out_stream.read();

    
}