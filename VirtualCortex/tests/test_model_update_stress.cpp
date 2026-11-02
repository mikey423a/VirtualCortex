#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/PendingNeuralQueue.h"
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <array>

int main() {
 vc::PendingNeuralQueue queue;
 vc::PortableAudioEngine engine;
 vc::SignalGrid grid;
 for(std::size_t r=0;r<4;++r)
  for(std::size_t c=0;c<8;++c) {
   assert(grid.set(r,c,{vc::BlockType::Amp,true}));
   vc::NeuralAmpModel m;
   m.outputBias=static_cast<float>(r*8+c)/100.f;
   assert(queue.submit(r,c,&m));
  }
 assert(engine.configure(grid,{{true,true,true,true}}));
 engine.prepare(48000);
 std::array<bool,32> seen{};
 for(int i=0;i<32;++i) {
  assert(queue.consumeOne([&](std::size_t r,std::size_t c,const vc::NeuralAmpModel* m) noexcept {
   assert(m && m->outputBias==static_cast<float>(r*8+c)/100.f);
   assert(!seen[r*8+c]);seen[r*8+c]=true;
   assert(engine.setNeuralAmp(r,c,m));
  }));
 }
 for(bool v:seen) assert(v);
 assert(!queue.consumeOne([](std::size_t,std::size_t,const vc::NeuralAmpModel*) noexcept {assert(false);}));
 for(std::size_t r=0;r<4;++r) for(std::size_t c=0;c<8;++c) assert(engine.neuralAmpEnabled(r,c));
 // The queue must copy model data, not retain a pointer to a temporary.
 {
  vc::NeuralAmpModel temporary;
  temporary.outputBias=.45f;
  assert(queue.submit(0,0,&temporary));
 }
 assert(queue.consumeOne([&](std::size_t r,std::size_t c,const vc::NeuralAmpModel* m) noexcept {
  assert(r==0 && c==0 && m && m->outputBias==.45f);
  assert(engine.setNeuralAmp(r,c,m));
 }));
 std::array<float,512> audio{};
 for(std::size_t i=0;i<audio.size();++i)audio[i]=std::sin(static_cast<float>(i)*.017f)*.15f;
 assert(engine.process(audio.data(),256,2));
 for(float v:audio) assert(std::isfinite(v));
 for(std::size_t r=0;r<4;++r) for(std::size_t c=0;c<8;++c) assert(queue.submit(r,c,nullptr));
 for(int i=0;i<32;++i) assert(queue.consumeOne([&](std::size_t r,std::size_t c,const vc::NeuralAmpModel* m) noexcept {
  assert(m==nullptr);assert(engine.setNeuralAmp(r,c,m));
 }));
 for(std::size_t r=0;r<4;++r) for(std::size_t c=0;c<8;++c) assert(!engine.neuralAmpEnabled(r,c));
 assert(engine.process(audio.data(),256,2));
 for(float v:audio) assert(std::isfinite(v));
}
