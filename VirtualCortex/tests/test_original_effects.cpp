#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <memory>
int main(){
 // Steady-state 4:1 compression must follow the amplitude power law.
 vc::PeakCompressor comp;float output=0,first=0;
 for(int i=0;i<96000;++i){output=comp.process(.5f,.5f,3.f/19.f,0.f,.1f,0.f,1.f,48000.f);if(i==0)first=output;}
 const float threshold=std::pow(10.f,-24.f/20.f),expected=threshold*std::pow(.5f/threshold,.25f);
 assert(std::abs(output-expected)<.0001f);assert(first>output*2.f);
 vc::PeakCompressor unity;
 for(int i=0;i<1000;++i)assert(std::abs(unity.process(.4f,.1f,0.f,.1f,.1f,0.f,1.f,48000)-.4f)<1.e-6f);
 assert(comp.process(.123f,.5f,.8f,.2f,.2f,.5f,0.f,48000)==.123f);
 // Release follows envelope memory rather than instantaneously boosting quiet input.
 float quietFirst=comp.process(.03f,.5f,3.f/19.f,0.f,.2f,0.f,1.f,48000);
 for(int i=0;i<96000;++i)output=comp.process(.03f,.5f,3.f/19.f,0.f,.2f,0.f,1.f,48000);
 assert(output>quietFirst*2.f);assert(std::abs(output-.03f)<.0001f);
 // Hard/soft transfer functions must produce different audio, reject DC at wet output,
 // and bypass exactly with dry mix. Validate across supported sample rates.
 for(float fs:{44100.f,48000.f,96000.f,192000.f}){
  vc::ClippingDrive soft,hard;double delta=0,wetEnergy=0;
  for(int i=0;i<(int)fs;++i){float x=.15f*std::sin(6.283185307f*440.f*i/fs);float a=soft.process(x,.8f,.5f,.3f,1.f,fs,false),b=hard.process(x,.8f,.5f,.3f,1.f,fs,true);assert(std::isfinite(a)&&std::isfinite(b));delta+=std::abs(a-b);wetEnergy+=a*a;}
  assert(delta>5);assert(wetEnergy>1);
  assert(soft.process(.123f,.8f,.2f,.5f,0.f,fs,false)==.123f);
  for(int i=0;i<(int)fs*2;++i)output=hard.process(.2f,.8f,.5f,.3f,1.f,fs,true);
  assert(std::abs(output)<.001f);
 }
 // New enum values survive preset grid encoding and run in the portable engine.
 vc::SignalGrid grid;grid.set(3,0,{vc::BlockType::Compressor,true});grid.set(3,1,{vc::BlockType::Overdrive,true});grid.set(3,2,{vc::BlockType::Distortion,true});
 vc::SignalGrid recalled;assert(vc::decodeGrid(vc::encodeGrid(grid),recalled));assert(recalled.get(3,2)->type==vc::BlockType::Distortion);
 auto engine=std::make_unique<vc::PortableAudioEngine>();engine->prepare(48000);assert(engine->configure(recalled,{false,false,false,true}));
 std::vector<float> audio(256,.1f);audio[0]=std::numeric_limits<float>::quiet_NaN();assert(engine->process(audio.data(),audio.size(),1));for(float v:audio)assert(std::isfinite(v)&&std::abs(v)<=1);
 std::cout<<"PASS: compressor ratio/attack/release, drive distinction/DC removal/dry mix, rates and new grid types\n";
}
