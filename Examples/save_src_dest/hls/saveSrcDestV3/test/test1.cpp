#include "saveSrcDest.hpp"

int main () {
	//initial return value
	int retval = 0;

	addr a[NUM_TRANS], b[NUM_TRANS];
	addr c_e[NUM_TRANS], d_e[NUM_TRANS];

	int ret = 0;
	addr in_s, in_d;
	src_dest_t ein;
	src_dest_t eout;

	for (int i = 0; i < NUM_TRANS; i++) {
		in_s = rand();
		in_d = rand();
		a[i] = in_s;
		b[i] = in_d;
		ein.src = in_s;
		ein.dest = in_d;
		saveSrcDest_hls_wrapper(&ein, &eout);
		c_e[i] = eout.src;
		d_e[i] = eout.dest;
	}

	for (int i = 0; i < NUM_TRANS; i++) {
		if (i == 0) {
			continue;
		}
		if (a[i-1] != c_e[i-1]){
			std::cout << "wrong a values!" << std::endl;
			retval = 1;
		}
		std::cout << "src:\t" << a[i-1] << "\tmatches returned src:\t" << c_e[i] << std::endl;
		if (b[i-1] != d_e[i-1]){
			std::cout << "wrong b values!" << std::endl;
			retval = 1;
		}
		std::cout << "dest:\t" << b[i-1] << "\tmatches returned dest:\t" << d_e[i] << std::endl;
	}
	return retval;
}
