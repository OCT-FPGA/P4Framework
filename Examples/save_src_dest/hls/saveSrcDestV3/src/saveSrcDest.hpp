#include<hls_stream.h>
#include<iostream>
#include"addr_types.h"

void saveSrcDest_hls_wrapper(src_dest_t *sd_in, src_dest_t *sd_out);
void saveSrcDest_kernel(hls::stream<src_dest_t> &user_extern_in, hls::stream<src_dest_t> &user_extern_out);
