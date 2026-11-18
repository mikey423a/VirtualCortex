#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
int main(){
 vc::PortableAudioEngine e;
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Amp,true}));
 assert(e.configure(grid,{true,false,false,false}));
 e.prepare(48000);
 vc::NeuralAmpModel a,b;
 a.outputBias=0.2f;b.outputBias=0.8f;
 assert(e.setNeuralAmp(0,0,&a));
 float stereo[2]={0,0};
 assert(e.process(stereo,1,2));
 assert(std::abs(stereo[0]-0.2f)<1e-6f && std::abs(stereo[1]-0.2f)<1e-6f);
 assert(e.setNeuralAmp(0,0,&b));
 for(std::size_t i=0;i<vc::PortableAudioEngine::neuralFadeSamples;++i){
  stereo[0]=stereo[1]=0;
  assert(e.process(stereo,1,2));
  const float expected=0.2f+0.6f*float(i+1)/float(vc::PortableAudioEngine::neuralFadeSamples);
  assert(std::abs(stereo[0]-expected)<1e-5f);
  assert(std::abs(stereo[1]-expected)<1e-5f);
 }
 stereo[0]=stereo[1]=0;
 assert(e.process(stereo,1,2));
 assert(std::abs(stereo[0]-0.8f)<1e-6f);
 // Replacing with the basic amplifier also fades rather than jumping.
 assert(e.setNeuralAmp(0,0,nullptr));
 stereo[0]=stereo[1]=0;
 assert(e.process(stereo,1,2));
 assert(stereo[0]>0.79f);
 for(std::size_t i=1;i<vc::PortableAudioEngine::neuralFadeSamples;++i){
  stereo[0]=stereo[1]=0;
  assert(e.process(stereo,1,2));
 }
 assert(std::abs(stereo[0])<1e-6f);

 // Duplicate selections must be exactly transparent in every routing mode,
 // during a fade and after it settles, including temporal (non-constant) models.
 for(double rate:{44100.,48000.,96000.})
 for(auto mode:{vc::ParallelGrid::Mode::Parallel,vc::ParallelGrid::Mode::Series,
                vc::ParallelGrid::Mode::DualMono,vc::ParallelGrid::Mode::Custom}){
  auto changed=std::make_unique<vc::PortableAudioEngine>();
  auto reference=std::make_unique<vc::PortableAudioEngine>();
  vc::SignalGrid g;assert(g.set(0,0,{vc::BlockType::Amp,true}));
  vc::NeuralAmpModel first,second;
  first.weights[0][0]=.7f;first.weights[0][7]=.4f;first.bias[0]=.03f;
  first.outputWeights[0]=.5f;first.outputBias=-.02f;
  second=first;second.weights[1][3]=-.6f;second.bias[1]=.04f;
  second.outputWeights[1]=.3f;second.outputBias=.01f;
  for(auto* engine:{changed.get(),reference.get()}){
   engine->prepare(rate,64);assert(engine->configure(g,{true,false,false,false}));
   assert(engine->setNeuralAmp(0,0,&first));
  }
  int sample=0;
  auto render=[&](int count){
   for(int n=0;n<count;++n,++sample){
    const float l=.15f*std::sin(.071f*sample),r=.12f*std::cos(.053f*sample);
    float a[2]{l,r},b[2]{l,r};
    assert(changed->process(a,1,2,mode));assert(reference->process(b,1,2,mode));
    assert(std::isfinite(a[0])&&std::isfinite(a[1]));
    assert(a[0]==b[0]&&a[1]==b[1]);
   }
  };
  render(257);
  // A separate copy of the same weights is still the same selected model.
  auto copy=first;assert(changed->setNeuralAmp(0,0,&copy));render(173);
  // All four model fields must trigger a genuine replacement when different.
  for(int field=0;field<4;++field){
   auto next=first;
   if(field==0)next.weights[0][7]+=.1f;
   if(field==1)next.bias[0]+=.1f;
   if(field==2)next.outputWeights[0]+=.1f;
   if(field==3)next.outputBias+=.1f;
   assert(changed->setNeuralAmp(0,0,&next));
   assert(reference->setNeuralAmp(0,0,&next));
   render(37);
   copy=next;
   for(int repeat=0;repeat<4;++repeat){assert(changed->setNeuralAmp(0,0,&copy));render(11);}
   render(193);
  }
  assert(changed->setNeuralAmp(0,0,&second));
  assert(reference->setNeuralAmp(0,0,&second));render(29);
  copy=second;assert(changed->setNeuralAmp(0,0,&copy));render(211);
  assert(changed->setNeuralAmp(0,0,nullptr));assert(reference->setNeuralAmp(0,0,nullptr));
  render(31);
  for(int repeat=0;repeat<5;++repeat){assert(changed->setNeuralAmp(0,0,nullptr));render(13);}
  render(197);
  assert(changed->setNeuralAmp(0,0,nullptr));render(151);
  // Invalid commands remain rejected without disturbing the selected model.
  auto invalid=second;invalid.bias[0]=std::numeric_limits<float>::quiet_NaN();
  assert(!changed->setNeuralAmp(0,0,&invalid));
  assert(!changed->setNeuralAmp(4,0,&second));render(137);
  // Device restart still clears history/transition and keeps the selected model.
  assert(changed->setNeuralAmp(0,0,&second));assert(reference->setNeuralAmp(0,0,&second));
  render(19);
  changed->prepare(rate,64);reference->prepare(rate,64);
  copy=second;assert(changed->setNeuralAmp(0,0,&copy));render(257);
 }
 std::cout<<"PASS: duplicate model/clear commands preserve temporal history and fades at three rates/four routing modes\n";
}
