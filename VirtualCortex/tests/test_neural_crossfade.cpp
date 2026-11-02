#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
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
}
