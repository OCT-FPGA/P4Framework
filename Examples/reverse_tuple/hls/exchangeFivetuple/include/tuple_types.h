#ifndef TUPLE_TYPES
#define TUPLE_TYPES
#include<ap_int.h>
typedef struct{
    ap_uint<32> src;
    ap_uint<32> dst;
    ap_uint<16> srcPort;
    ap_uint<16> dstPort;
    ap_uint<8>  proto;
}five_tuples_t;
#endif