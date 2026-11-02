#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PendingNeuralQueue.h"
#include <cassert>
#include <cmath>
int main(){
 vc::PendingNeuralQueue queue;
 vc::NeuralAmpModel a,b;
 a.outputBias=0.25f;b.outputBias=0.75f;
 assert(queue.submit(0,0,&a));
 assert(queue.submit(1,7,&b));
 assert(queue.submit(0,0,&b)); // last-write-wins
 assert(!queue.submit(4,0,&a));
 auto bad=a;bad.outputBias=INFINITY;
 assert(!queue.submit(0,0,&bad));
 int count=0;
 assert(queue.consumeOne([&](std::size_t r,std::size_t c,const vc::NeuralAmpModel* m) noexcept {
  assert(r==0 && c==0 && m && m->outputBias==0.75f);++count;
 }));
 assert(queue.consumeOne([&](std::size_t r,std::size_t c,const vc::NeuralAmpModel* m) noexcept {
  assert(r==1 && c==7 && m && m->outputBias==0.75f);++count;
 }));
 assert(!queue.consumeOne([&](std::size_t,std::size_t,const vc::NeuralAmpModel*) noexcept {assert(false);}));
 assert(queue.submit(0,0,nullptr));
 assert(queue.consumeOne([&](std::size_t,std::size_t,const vc::NeuralAmpModel* m) noexcept {assert(m==nullptr);++count;}));
 assert(count==3);
}
