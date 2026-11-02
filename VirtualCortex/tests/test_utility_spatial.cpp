#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
template<class Process>double sineEnergy(float fs,float hz,Process&& process){double e=0;for(int i=0;i<(int)fs/2;++i){float y=process(.01f*std::sin(6.283185307f*hz*i/fs));assert(std::isfinite(y));if(i>(int)fs/4)e+=y*y;}return e;}
int main(){
 // Preserve the original encoding for old device IDs; expanded IDs use V2.
 vc::SignalGrid old;old.set(0,0,{vc::BlockType::Amp,false});auto legacy=vc::encodeGrid(old);assert(legacy[1]==1);vc::SignalGrid recalled;assert(vc::decodeGrid(legacy,recalled));assert(!recalled.get(0,0)->enabled);
 vc::SignalGrid expanded;expanded.set(3,7,{vc::BlockType::RoomReverb,true});auto modern=vc::encodeGrid(expanded);assert(modern[1]==2);assert(vc::decodeGrid(modern,recalled));assert(recalled.get(3,7)->type==vc::BlockType::RoomReverb);
 modern[1]=1;assert(!vc::decodeGrid(modern,recalled));assert(recalled.get(3,7)->type==vc::BlockType::RoomReverb);modern[1]=3;assert(!vc::decodeGrid(modern,recalled));
 for(float fs:{44100.f,48000.f,96000.f,192000.f}){
  vc::GainUtility gain;float y=0;for(int i=0;i<(int)fs/2;++i)y=gain.process(.1f,60.f/72.f,0,fs);assert(std::abs(y-.1f)<1.e-5f);
  for(int i=0;i<(int)fs/2;++i)y=gain.process(.1f,54.f/72.f,0,fs);assert(std::abs(y-.0501187f)<1.e-5f);
  for(int i=0;i<(int)fs/2;++i)y=gain.process(.1f,60.f/72.f,1,fs);assert(std::abs(y+.1f)<1.e-5f);
  // Band limiting rejects bass and treble while retaining the midrange.
  auto cutEnergy=[&](float frequency){vc::CutFilters cuts;return sineEnergy(fs,frequency,[&](float x){return cuts.process(x,(300.f-20.f)/980.f,(3000.f-1000.f)/19000.f,1,fs);});};
  assert(cutEnergy(1000)>cutEnergy(30)*500);assert(cutEnergy(1000)>cutEnergy(15000)*50);
  vc::ToneEQ flat;const double unity=sineEnergy(fs,1000,[&](float x){return flat.process(x,.5f,.5f,.5f,900.f/3900.f,.189189f,1,fs);});
  vc::ToneEQ boost;const double boosted=sineEnergy(fs,1000,[&](float x){return boost.process(x,.5f,.75f,.5f,900.f/3900.f,.189189f,1,fs);});
  assert(std::abs(boosted/unity-std::pow(10.,6./10.))<.02);
  vc::ToneEQ lowShelf;const double low=sineEnergy(fs,20,[&](float x){return lowShelf.process(x,.75f,.5f,.5f,.2f,.2f,1,fs);});
  vc::ToneEQ lowFlat;const double reference=sineEnergy(fs,20,[&](float x){return lowFlat.process(x,.5f,.5f,.5f,.2f,.2f,1,fs);});assert(low/reference>3.5);
  assert(flat.process(.123f,0,1,0,.5f,.5f,0,fs)==.123f);
  for(auto type:{vc::BlockType::Vibrato,vc::BlockType::RingModulator,vc::BlockType::Gain,vc::BlockType::LowHighCut,vc::BlockType::ToneEQ,vc::BlockType::RoomReverb}){
   auto engine=std::make_unique<vc::PortableAudioEngine>();engine->prepare(fs);vc::SignalGrid g;g.set(0,0,{type,true});assert(engine->configure(g,{true,false,false,false}));vc::SlotParameters p;p.mix=1;p.feedback=.2f;p.time=.2f;p.rate=.2f;p.depth=.8f;engine->setSlotParameters(0,0,p);
   std::vector<float> input((std::size_t)(fs*.2f)*2);for(std::size_t i=0;i<input.size();++i)input[i]=.1f*std::sin(i*.1f);assert(engine->process(input.data(),input.size()/2,2));double e=0;for(float v:input){assert(std::isfinite(v)&&std::abs(v)<=1);e+=v*v;}assert(e>0);
  }
 }
 // The room has no immediate wet feedthrough; a longer decay leaves more late energy.
 auto roomEnergy=[](float time){vc::RoomReverb room;room.prepare(48000);double early=0,late=0;for(int i=0;i<144000;++i){float y=room.process(i==0?.1f:0.f,time,.1f,.6f);assert(std::isfinite(y));if(i<1200)early+=y*y;if(i>24000)late+=y*y;}assert(early==0);return late;};assert(roomEnergy(.7f)>roomEnergy(0)*100);
 // A 1 kHz tone multiplied by a 100 Hz carrier yields sidebands at 900/1100 Hz.
 {auto engine=std::make_unique<vc::PortableAudioEngine>();engine->prepare(48000);vc::SignalGrid g;g.set(0,0,{vc::BlockType::RingModulator,true});engine->configure(g,{true,false,false,false});vc::SlotParameters p;p.rate=std::log(10.f)/std::log(200.f);p.depth=1;p.mix=1;engine->setSlotParameters(0,0,p);std::vector<float> x(48000);for(int i=0;i<48000;++i)x[i]=.1f*std::sin(6.283185307f*1000*i/48000);engine->process(x.data(),x.size(),1);
  auto magnitude=[&](float hz){double a=0,b=0;for(int i=0;i<48000;++i){const double w=6.283185307179586*hz*i/48000.;a+=x[i]*std::cos(w);b+=x[i]*std::sin(w);}return std::sqrt(a*a+b*b);};assert(magnitude(900)>magnitude(1000)*100);assert(magnitude(1100)>magnitude(1000)*100);}
 std::cout<<"PASS: legacy/V2 state, gain/polarity, cut response, EQ gain, room decay, ring sidebands and rate matrix\n";
}
