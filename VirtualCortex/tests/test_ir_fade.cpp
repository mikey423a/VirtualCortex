#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <vector>
#include <memory>
#include <limits>
#include <iostream>
int main() {
 vc::PortableAudioEngine engine;
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Cabinet,true}));
 assert(engine.configure(grid,{true,false,false,false}));
 engine.prepare(48000);
 const float plus[]={1.f}, minus[]={-1.f};
 assert(engine.setCabinetIR(0,0,plus,1));
 std::vector<float> audio(256,0.5f);
 assert(engine.process(audio.data(),audio.size(),1));
 for(float v:audio) assert(std::isfinite(v));
 assert(engine.setCabinetIR(0,0,minus,1));
 std::vector<float> change(128,0.5f);
 assert(engine.process(change.data(),change.size(),1));
 assert(change.front()>0.45f); // starts close to old response
 assert(change[63]<0.02f && change[63]>-0.02f); // midpoint
 assert(change.back()<-0.49f); // reaches new response
 std::vector<float> steady(16,0.5f);
 assert(engine.process(steady.data(),steady.size(),1));
 for(float v:steady) assert(std::abs(v+0.5f)<1e-5f);
 // Adding the first IR must crossfade from the fallback cabinet filter.
 vc::PortableAudioEngine fresh;
 assert(fresh.configure(grid,{true,false,false,false}));
 fresh.prepare(48000);
 std::vector<float> warm(256,0.5f);
 assert(fresh.process(warm.data(),warm.size(),1));
 assert(fresh.setCabinetIR(0,0,minus,1));
 std::vector<float> first(128,0.5f);
 assert(fresh.process(first.data(),first.size(),1));
 assert(first.front()>0.3f); // close to fallback, not immediate negative IR
 assert(first.back()<-0.49f);
 // Clearing IR also fades rather than discontinuously replacing the FIR.
 assert(engine.setCabinetIR(0,0,nullptr,0));
 std::vector<float> clear(128,0.5f);
 assert(engine.process(clear.data(),clear.size(),1));
 assert(clear.front()<-0.45f);
 assert(clear.back()>0.4f);

 for(double rate:{44100.,48000.,96000.})
 for(auto mode:{vc::ParallelGrid::Mode::Parallel,vc::ParallelGrid::Mode::Series,
                vc::ParallelGrid::Mode::DualMono,vc::ParallelGrid::Mode::Custom}){
  auto e=std::make_unique<vc::PortableAudioEngine>();
  assert(e->configure(grid,{true,false,false,false}));
  e->prepare(rate,64);assert(e->setCabinetIR(0,0,plus,1));e->prepare(rate,64);
  auto tick=[&](float expected){
   float frame[2]{.2f,-.15f};assert(e->process(frame,1,2,mode));
   assert(std::abs(frame[0]-.2f*expected)<2e-6f);
   const float right=mode==vc::ParallelGrid::Mode::DualMono?-.15f:-.15f*expected;
   assert(std::abs(frame[1]-right)<2e-6f);
  };
  assert(e->setCabinetIR(0,0,minus,1));
  for(int i=0;i<128;++i){
   if(i==31){const float intermediate[]={.25f};assert(e->setCabinetIR(0,0,intermediate,1));}
   if(i==47){const float latest[]={.5f,0.f};assert(e->setCabinetIR(0,0,latest,2));assert(e->cabinetIRLength(0,0)==2);}
   if(i==55){const float bad[]={std::numeric_limits<float>::quiet_NaN()};assert(!e->setCabinetIR(0,0,bad,1));}
   tick(1.f-2.f*(i+1)/128.f);
  }
  for(int i=0;i<128;++i)tick(-1.f+1.5f*(i+1)/128.f);
  tick(.5f);
  // Returning to the current target cancels a queued replacement.
  assert(e->setCabinetIR(0,0,minus,1));
  for(int i=0;i<128;++i){
   if(i==17)assert(e->setCabinetIR(0,0,plus,1));
   if(i==37)assert(e->setCabinetIR(0,0,minus,1));
   tick(.5f-1.5f*(i+1)/128.f);
  }
  for(int i=0;i<150;++i)tick(-1.f);
  // Restart commits the latest requested target, with obsolete fades discarded.
  assert(e->setCabinetIR(0,0,plus,1));tick(-1.f+2.f/128.f);
  const float latest[]={.3f,0.f};assert(e->setCabinetIR(0,0,latest,2));
  e->prepare(rate,64);tick(.3f);
  // A queued removal persists across restart, matching an ordinary clean fallback.
  assert(e->setCabinetIR(0,0,minus,1));tick(.3f-1.3f/128.f);
  assert(e->setCabinetIR(0,0,nullptr,0));assert(e->cabinetIRLength(0,0)==0);
  e->prepare(rate,64);
  auto clean=std::make_unique<vc::PortableAudioEngine>();clean->prepare(rate,64);assert(clean->configure(grid,{true,false,false,false}));
  for(int i=0;i<200;++i){float a[2]{.2f,-.15f},b[2]{.2f,-.15f};assert(e->process(a,1,2,mode));assert(clean->process(b,1,2,mode));assert(a[0]==b[0]&&a[1]==b[1]);}
 }
 std::cout<<"PASS: queued IR transition continuity, latest target, cancellation and restart across rates/routing modes\n";
}
