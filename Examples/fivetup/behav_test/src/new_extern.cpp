#include <bm/bm_sim/extern.h>
#include <bm/bm_sim/data.h>
#include <atomic>

using bm::ExternType;
using bm::Data;

class ts_now : public ExternType {
 public:
  BM_EXTERN_ATTRIBUTES {}

  // Ignore input; output a monotonically increasing 64-bit counter.

//void apply(cst Data & /*in*/, Data &out) {
  //  static std::atomic<uint64_t> ctr{0};
   // uint64_t v = ++ctr;          // start at 1
   // out.set(v)  }

  void apply(const Data & /*in*/, Data &out) {
       out.set(0xaaaaaaaa55555555ULL);
	}

};

BM_REGISTER_EXTERN(ts_now);
BM_REGISTER_EXTERN_METHOD(ts_now, apply, const Data&, Data&);
