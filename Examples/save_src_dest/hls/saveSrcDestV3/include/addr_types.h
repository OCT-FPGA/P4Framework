#ifndef ADDR_TYPES
#define ADDR_TYPES
#include<ap_int.h>
typedef ap_uint<32> addr;
typedef struct{
	addr src;
	addr dest;
}src_dest_t;
#define NUM_TRANS 300
#endif