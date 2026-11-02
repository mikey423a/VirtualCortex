#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PendingIRQueue.h"
#include <cassert>
#include <limits>
int main() {
 vc::PendingIRQueue<16> q;
 float a[]={0.1f,0.2f}, b[]={0.4f};
 assert(q.submit(0,0,a,2));
 assert(q.submit(3,7,b,1));
 assert(q.submit(0,0,b,1)); // coalesces, does not discard other slot
 assert(!q.submit(4,0,b,1));
 assert(!q.submit(0,0,b,17));
 float nan=std::numeric_limits<float>::quiet_NaN();
 assert(!q.submit(0,0,&nan,1));
 int count=0;
 auto take=[&](std::size_t r,std::size_t c,const float* taps,std::size_t n) noexcept {
  if (r==0 && c==0) {assert(n==1 && taps[0]==0.4f);}
  else {assert(r==3 && c==7 && n==1 && taps[0]==0.4f);}
  ++count;
 };
 assert(q.consumeOne(take));
 assert(q.consumeOne(take));
 assert(!q.consumeOne(take));
 assert(count==2);
 assert(q.submit(1,2,nullptr,0)); // clearing a slot
 assert(q.consumeOne([&](std::size_t r,std::size_t c,const float*,std::size_t n) noexcept {
  assert(r==1 && c==2 && n==0);
 }));
}
