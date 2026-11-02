#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include "../Source/DSP/CoreDSP.h"
#include "../Source/DSP/SignalGrid.h"
#include "../Source/DSP/GridPlan.h"
#include "../Source/DSP/GridExecutor.h"
#include "../Source/DSP/ParallelGrid.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){
 vc::HighPassOnePole hp; hp.prepare(48000); hp.setCutoff(100);
 double sum=0; for(int i=0;i<48000;i++){float x=hp.process(1.f); assert(std::isfinite(x)); if(i>47000)sum+=std::abs(x);}
 assert(sum<0.02); // DC rejection
 hp.reset(); hp.setCutoff(100); float first=hp.process(1.f); assert(first>0.9f);
 for(int i=0;i<10000;i++) assert(std::isfinite(vc::saturate(std::sin(i*.01f),2.f,.1f)));
 assert(std::abs(vc::saturate(0.f,10.f,.3f))<1e-6f);
 // Verify DC rejection and saturation symmetry across representative sample rates.
 for (double rate : {44100.0, 48000.0, 96000.0}) {
  vc::HighPassOnePole f; f.prepare(rate); f.setCutoff(80.f);
  float output = 0.f;
  for (int i=0;i<static_cast<int>(rate);++i) output=f.process(0.75f);
  assert(std::abs(output)<0.0001f);
 }
 for (float x : {-1.f,-0.5f,0.f,0.5f,1.f}) {
  assert(std::abs(vc::saturate(x,3.f,0.f)+vc::saturate(-x,3.f,0.f))<0.00001f);
 }
 // Gain smoothing must converge, remain finite and avoid an instantaneous jump.
 vc::SmoothedGain smoother;
 smoother.prepare(48000, 12.f);
 smoother.reset(0.f);
 const float firstGain = smoother.process(1.f);
 assert(firstGain > 0.f && firstGain < 0.01f);
 for (int i=0; i<4800; ++i) assert(std::isfinite(smoother.process(1.f)));
 assert(smoother.value() > 0.99f && smoother.value() <= 1.f);
 smoother.process(NAN);
 assert(std::isfinite(smoother.value()));
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Gate,true}));
 assert(grid.set(0,1,{vc::BlockType::Amp,true}));
 assert(grid.swap(0,0,0,1));
 assert(grid.get(0,0)->type == vc::BlockType::Amp);
 assert(grid.bypass(0,0,true) && !grid.get(0,0)->enabled);
 assert(!grid.set(4,0,{vc::BlockType::Gate,true}));
 assert(!grid.set(0,8,{vc::BlockType::Gate,true}));
 assert(!grid.set(0,0,{static_cast<vc::BlockType>(255),true}));
 assert(!grid.swap(0,0,5,0));
 assert(grid.get(99,99)==nullptr);
 vc::GridPlan plan;
 assert(vc::compileRow(grid,0,plan));
 assert(plan.count==1 && plan.blocks[0]==vc::BlockType::Gate);
 assert(grid.set(0,2,{vc::BlockType::Gate,true}));
 assert(vc::compileRow(grid,0,plan)); // independent instances are allowed
 assert(grid.set(0,2,{vc::BlockType::Empty,true}));
 auto bytes=vc::encodeGrid(grid);
 vc::SignalGrid restored;
 assert(vc::decodeGrid(bytes,restored));
 assert(restored.get(0,0)->type==vc::BlockType::Amp);
 assert(!restored.get(0,0)->enabled);
 bytes[3]=0x7f;
 assert(!vc::decodeGrid(bytes,restored)); // malformed snapshot rejected
 assert(restored.get(0,0)->type==vc::BlockType::Amp); // transactional decode
 // Verify audible execution order changes when blocks are swapped.
 vc::SignalGrid routing;
 assert(routing.set(0,0,{vc::BlockType::Amp,true}));
 assert(routing.set(0,1,{vc::BlockType::EQ,true}));
 vc::GridPlan executionPlan;
 vc::GridExecutor executor;
 assert(vc::compileRow(routing,0,executionPlan));
 assert(executor.configure(executionPlan));
 const auto effect=[](vc::BlockType type,float x) noexcept {
  return type==vc::BlockType::Amp ? x*2.f : type==vc::BlockType::EQ ? x+1.f : x;
 };
 assert(std::abs(executor.processSample(1.f,effect)-3.f)<1.e-6f);
 assert(routing.swap(0,0,0,1));
 assert(vc::compileRow(routing,0,executionPlan));
 assert(executor.configure(executionPlan));
 assert(std::abs(executor.processSample(1.f,effect)-4.f)<1.e-6f);
 assert(routing.bypass(0,0,true));
 assert(vc::compileRow(routing,0,executionPlan));
 assert(executor.configure(executionPlan));
 assert(std::abs(executor.processSample(1.f,effect)-2.f)<1.e-6f);
 float samples[]={1.f,2.f,3.f};
 executor.processBuffer(samples,3,effect);
 assert(samples[0]==2.f && samples[1]==4.f && samples[2]==6.f);
 assert(executor.processSample(NAN,effect)==0.f);
 vc::GridPlan invalidPlan; invalidPlan.count=9;
 assert(!executor.configure(invalidPlan));
 assert(executor.plan().count==1);
 // Parallel rows receive independent copies and average at the output.
 vc::SignalGrid parallelLayout;
 assert(parallelLayout.set(0,0,{vc::BlockType::Amp,true}));
 assert(parallelLayout.set(1,0,{vc::BlockType::EQ,true}));
 vc::ParallelGrid parallel;
 assert(parallel.configure(parallelLayout,{{true,true,false,false}}));
 const auto laneEffect=[](std::size_t row,vc::BlockType type,float x) noexcept {
  return row==0 && type==vc::BlockType::Amp ? x*2.f :
         row==1 && type==vc::BlockType::EQ ? x+2.f : x;
 };
 assert(std::abs(parallel.processSample(2.f,laneEffect)-4.f)<1.e-6f);
 assert(parallel.configure(parallelLayout,{{true,false,false,false}}));
 assert(std::abs(parallel.processSample(2.f,laneEffect)-4.f)<1.e-6f);
 assert(parallel.configure(parallelLayout,{{false,false,false,false}}));
 assert(parallel.processSample(2.f,laneEffect)==2.f);
 assert(parallelLayout.set(0,1,{vc::BlockType::Amp,true}));
 assert(parallel.configure(parallelLayout,{{true,false,false,false}})); // repeated amp instances
 assert(parallel.configure(parallelLayout,{{false,true,false,false}}));
 float laneSamples[]={0.f,1.f,2.f};
 parallel.processBuffer(laneSamples,3,laneEffect);
 assert(laneSamples[0]==2.f && laneSamples[1]==3.f && laneSamples[2]==4.f);
 assert(std::isfinite(parallel.processSample(NAN,laneEffect)));
 // Real portable DSP dispatch through independently stateful lanes.
 vc::SignalGrid audioGrid;
 assert(audioGrid.set(0,0,{vc::BlockType::Amp,true}));
 assert(audioGrid.set(0,1,{vc::BlockType::Cabinet,true}));
 vc::PortableAudioEngine audio;
 assert(audio.configure(audioGrid,{{true,false,false,false}}));
 audio.prepare(48000);
 float wave[]={0.2f,0.2f,0.4f,0.4f};
 assert(audio.process(wave,2,2));
 assert(std::abs(wave[0]-wave[1])<1e-6f);
 assert(std::abs(wave[2]-wave[3])<1e-6f);
 assert(std::abs(wave[0]-0.2f)>0.01f);
 assert(audioGrid.swap(0,0,0,1));
 vc::PortableAudioEngine swapped;
 assert(swapped.configure(audioGrid,{{true,false,false,false}}));
 swapped.prepare(48000);
 float alternate[]={0.2f,0.2f,0.4f,0.4f};
 assert(swapped.process(alternate,2,2));
 assert(std::abs(wave[0]-alternate[0])>0.01f);
 assert(audioGrid.bypass(0,1,true));
 assert(audioGrid.bypass(0,0,true));
 vc::PortableAudioEngine bypassed;
 assert(bypassed.configure(audioGrid,{{true,false,false,false}}));
 bypassed.prepare(48000);
 float bypassInput[]={0.2f,0.2f};
 assert(bypassed.process(bypassInput,1,2));
 assert(std::abs(bypassInput[0]-0.2f)<0.01f);
 assert(!bypassed.process(bypassInput,1,3));
 vc::SignalGrid twoLanes;
 assert(twoLanes.set(0,0,{vc::BlockType::Delay,true}));
 assert(twoLanes.set(1,0,{vc::BlockType::Amp,true}));
 vc::PortableAudioEngine multi;
 assert(multi.configure(twoLanes,{{true,true,false,false}}));
 multi.prepare(44100);
 float multiInput[]={0.1f,0.2f};
 assert(multi.process(multiInput,2,1));
 assert(std::isfinite(multiInput[0]) && std::isfinite(multiInput[1]));
 // Verify duplicate delays have independent state: two serial delays must
 // produce different results from one delay and remain stereo-isolated.
 vc::SignalGrid doubleDelay;
 assert(doubleDelay.set(0,0,{vc::BlockType::Delay,true}));
 assert(doubleDelay.set(0,1,{vc::BlockType::Delay,true}));
 vc::GridPlan doublePlan;
 assert(vc::compileRow(doubleDelay,0,doublePlan));
 assert(doublePlan.count==2 && doublePlan.columns[0]==0 && doublePlan.columns[1]==1);
 vc::PortableAudioEngine doubleEngine;
 assert(doubleEngine.configure(doubleDelay,{{true,false,false,false}}));
 doubleEngine.prepare(8000,8);
 float impulse[32]{};impulse[0]=1.f;
 assert(doubleEngine.process(impulse,32,1));
 for(float x:impulse)assert(std::isfinite(x));
 assert(impulse[0]>0.f && impulse[0]<1.f);
 // Each duplicate delay must receive its own physical-column identity.
 // The indexed callback exposes both columns, including after bypass.
 vc::ParallelGrid indexed;
 assert(indexed.configure(doubleDelay,{{true,false,false,false}}));
 int hits[8]{};
 const float indexedOut=indexed.processSampleIndexed(1.f,[&](std::size_t row,std::size_t col,vc::BlockType type,float x) noexcept {
  assert(row==0 && type==vc::BlockType::Delay);
  ++hits[col];return x*0.5f;
 });
 assert(hits[0]==1 && hits[1]==1 && std::abs(indexedOut-0.25f)<1e-6f);
 assert(doubleDelay.bypass(0,0,true));
 assert(indexed.configure(doubleDelay,{{true,false,false,false}}));
 indexed.processSampleIndexed(1.f,[&](std::size_t,std::size_t col,vc::BlockType,float x) noexcept {++hits[col];return x;});
 assert(hits[0]==1 && hits[1]==2);
 // An invalid column index is rejected without mutating an existing executor.
 vc::GridPlan malformed=doublePlan;malformed.columns[1]=99;
 assert(!executor.configure(malformed));
 assert(executor.plan().count==1);
 // Per-slot parameters alter audio without sharing state with adjacent blocks.
 vc::SignalGrid ampGrid;
 assert(ampGrid.set(0,0,{vc::BlockType::Amp,true}));
 assert(ampGrid.set(0,1,{vc::BlockType::Amp,true}));
 vc::PortableAudioEngine paramEngine;
 assert(paramEngine.configure(ampGrid,{{true,false,false,false}}));
 paramEngine.prepare(48000);
 vc::SlotParameters low,high;
 low.drive=0.f; high.drive=1.f;
 assert(paramEngine.setSlotParameters(0,0,low));
 assert(paramEngine.setSlotParameters(0,1,high));
 assert(paramEngine.getSlotParameters(0,0)->drive==0.f);
 assert(paramEngine.getSlotParameters(0,1)->drive==1.f);
 assert(!paramEngine.setSlotParameters(4,0,high));
 high.drive=NAN;
 assert(!paramEngine.setSlotParameters(0,1,high));
 assert(paramEngine.getSlotParameters(0,1)->drive==1.f);
 float ampSample[]={0.2f};
 assert(paramEngine.process(ampSample,1,1));
 assert(ampSample[0]>0.2f && std::isfinite(ampSample[0]));
 vc::SignalGrid delayGrid;
 assert(delayGrid.set(0,0,{vc::BlockType::Delay,true}));
 vc::PortableAudioEngine dryDelay,wetDelay;
 assert(dryDelay.configure(delayGrid,{{true,false,false,false}}));
 assert(wetDelay.configure(delayGrid,{{true,false,false,false}}));
 dryDelay.prepare(8000,64); wetDelay.prepare(8000,64);
 vc::SlotParameters dry,wet;
 dry.mix=0.f;wet.mix=1.f; dry.time=0.f;wet.time=0.f;
 assert(dryDelay.setSlotParameters(0,0,dry));
 assert(wetDelay.setSlotParameters(0,0,wet));
 float dryImpulse[]={1.f,0.f}; float wetImpulse[]={1.f,0.f};
 assert(dryDelay.process(dryImpulse,2,1));
 assert(wetDelay.process(wetImpulse,2,1));
 assert(dryImpulse[0]==1.f && wetImpulse[0]==0.f);
 // Per-slot cabinet FIR: impulse response, stereo independence and rollback.
 {
  vc::SignalGrid cabGrid;
  assert(cabGrid.set(0,0,{vc::BlockType::Cabinet,true}));
  vc::PortableAudioEngine cabEngine;
  assert(cabEngine.configure(cabGrid,{{true,false,false,false}}));
  cabEngine.prepare(48000.0);
  const float impulse[]={0.5f,0.25f,-0.125f};
  assert(cabEngine.setCabinetIR(0,0,impulse,3));
  assert(cabEngine.cabinetIRLength(0,0)==3);
  // Let the initial fallback-to-IR crossfade finish before exact FIR impulse assertions.
  float settle[128]{};
  assert(cabEngine.process(settle,128,1));
  float stereo[]={1.f,0.f, 0.f,0.f, 0.f,0.f, 0.f,0.f};
  assert(cabEngine.process(stereo,4,2));
  for(int i=0;i<3;++i) { assert(std::abs(stereo[i*2]-impulse[i])<1e-5f); assert(stereo[i*2+1]==0.f); }
  assert(std::abs(stereo[6])<1e-5f);
  const float invalid[]={1.f,std::numeric_limits<float>::quiet_NaN()};
  assert(!cabEngine.setCabinetIR(0,0,invalid,2));
  assert(cabEngine.cabinetIRLength(0,0)==3);
  assert(!cabEngine.setCabinetIR(4,0,impulse,3));
  assert(!cabEngine.setCabinetIR(0,0,impulse,vc::PortableAudioEngine::maxIrTaps+1));
  assert(cabEngine.setCabinetIR(0,0,nullptr,0));
  assert(cabEngine.cabinetIRLength(0,0)==0);
 }
 std::cout<<"CoreDSP, per-slot parameters, grid and routing tests passed\n";
}
