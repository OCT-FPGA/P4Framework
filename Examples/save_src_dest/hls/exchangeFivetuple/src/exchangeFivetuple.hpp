#include<hls_stream.h>
#include<iostream>
#include"tuple_types.h"
void exchangeFivetuple_hls_wrapper(five_tuples_t *tuple_in, five_tuples_t *tuple_out);
void exchangeFivetuple_kernel(hls::stream<five_tuples_t> &user_extern_in, hls::stream<five_tuples_t> &user_extern_out);