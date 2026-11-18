#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include <cstdlib>
#include <new>
static bool watchAllocations=false;
static unsigned allocations=0;
void* operator new(std::size_t n){if(watchAllocations)++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
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

 // Compare queued/moved/block rendering with a stationary single-frame renderer
 // receiving each replacement only after the previous fade completes.
 for(double rate:{44100.,48000.,96000.})
 for(auto mode:{vc::ParallelGrid::Mode::Parallel,vc::ParallelGrid::Mode::Series,
                vc::ParallelGrid::Mode::DualMono,vc::ParallelGrid::Mode::Custom}){
  auto moved=std::make_unique<vc::PortableAudioEngine>();
  auto reference=std::make_unique<vc::PortableAudioEngine>();
  std::array<float,vc::PortableAudioEngine::maxIrTaps> a{},b{},c{};
  a[0]=.4f;a[127]=.15f;a.back()=-.07f;
  b[0]=-.25f;b[53]=.2f;b.back()=.1f;
  c[0]=.3f;c[911]=-.12f;c.back()=.06f;
  for(auto* e:{moved.get(),reference.get()}){
   e->prepare(rate,64);assert(e->configure(grid,{true,false,false,false}));
   assert(e->setCabinetIR(0,0,a.data(),a.size()));e->prepare(rate,64);
  }
  unsigned time=0;int column=0;
  auto render=[&](int frames){
   std::vector<float> actual(frames*2),expected(frames*2);
   for(int i=0;i<frames;++i,++time){actual[2*i]=.17f*std::sin(.017f*time);actual[2*i+1]=.13f*std::cos(.029f*time);}
   expected=actual;
   allocations=0;watchAllocations=true;
   assert(moved->process(actual.data(),frames,2,mode));
   for(int i=0;i<frames;++i)assert(reference->process(expected.data()+2*i,1,2,mode));
   watchAllocations=false;assert(allocations==0);
   assert(actual==expected);
  };
  render(4200); // Populate history through the last supported tap.
  for(auto* e:{moved.get(),reference.get()})assert(e->setCabinetIR(0,0,b.data(),b.size()));
  render(17);
  assert(moved->setCabinetIR(0,0,c.data(),c.size()));render(47);
  // Move all FIRs, pending target, fade counter and input history together.
  assert(moved->swapSlots(0,0,0,7));
  vc::SignalGrid shifted;assert(shifted.set(0,7,{vc::BlockType::Cabinet,true}));
  assert(moved->configure(shifted,{true,false,false,false}));column=7;
  assert(moved->cabinetIRLength(0,column)==c.size());render(64);
  assert(reference->setCabinetIR(0,0,c.data(),c.size()));
  render(1);render(63);render(64);render(137);
  // Queue removal during another replacement and let it complete without restart.
  assert(moved->setCabinetIR(0,column,a.data(),a.size()));
  assert(reference->setCabinetIR(0,0,a.data(),a.size()));render(19);
  assert(moved->setCabinetIR(0,column,nullptr,0));assert(moved->cabinetIRLength(0,column)==0);
  render(109);assert(reference->setCabinetIR(0,0,nullptr,0));
  render(127);render(1);render(257);
 }
 std::cout<<"PASS: full-length IR history, queued removal, slot move, block partition and process allocation guards\n";
}
