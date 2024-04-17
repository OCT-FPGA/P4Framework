#include"saveSrcDest.hpp"

int main () {
	//initial return value
	int retval = 0;

	addr a[NUM_TRANS], b[NUM_TRANS];
	addr c_e[NUM_TRANS], d_e[NUM_TRANS];

	int ret = 0;
	addr in_s, in_d, out_s, out_d;

	for (int i = 0; i < NUM_TRANS; i++) {
		in_s = rand();
		in_d = rand();
		a[i] = in_s;
		b[i] = in_d;
		saveSrcDest_hls_wrapper(&in_s, &in_d, &out_s, &out_d);
		c_e[i] = out_s;
		d_e[i] = out_d;
	}

	//call to the wrapper
	//saveSrcDest_hls_wrapper(&a, &b, &c_e, &d_e);

	for (int i = 0; i < NUM_TRANS; i++) {
		if (i == 0) {
			continue;
		}
		if (a[i-1] != c_e[i]){
			std::cout << "wrong a values!" << std::endl;
			retval = 1;
		}
		std::cout << "src:\t" << a[i-1] << "\tmatches returned src:\t" << c_e[i] << std::endl;
		if (b[i-1] != d_e[i]){
			std::cout << "wrong b values!" << std::endl;
			retval = 1;
		}
		std::cout << "dest:\t" << b[i-1] << "\tmatches returned dest:\t" << d_e[i] << std::endl;
	}
	return retval;
}
