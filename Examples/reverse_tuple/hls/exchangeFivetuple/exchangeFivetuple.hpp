#include<ap_int.h>
#include<hls_stream.h>
#include<iostream>

typedef struct{
    ap_uint<32> src;
    ap_uint<32> dst;
    ap_uint<16> srcPort;
    ap_uint<16> dstPort;
    ap_uint<8>  proto;
}five_tuples_t;
void exchangeFivetuple(hls::stream<five_tuples_t> &user_extern_in, hls::stream<five_tuples_t> &user_extern_out);