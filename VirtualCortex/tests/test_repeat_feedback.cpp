#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
std::vector<float> render(bool fx,float filterMix){auto engine=std::make_unique<vc::PortableAudioEngine>();engine->prepare(48000);vc::SignalGrid g;g.set(0,0,{vc::BlockType::Delay,true});engine->configure(g,{true,false,false,false});vc::SlotParameters p;p.repeatFx=fx;p.time=0;p.feedback=.5f;p.mix=1;p.drive=0;p.depth=0;p.rate=filterMix;engine->setSlotParameters(0,0,p);std::vector<float> data(960*8);data[0]=.1f;engine->process(data.data(),data.size(),1);return data;}
int main(){
 const auto legacy=render(false,1),filtered=render(true,1),plain=render(true,0);
 // Dry injection returns first unfiltered; the second repeat passes through filters.
 assert(std::abs(filtered[960]-.1f)<.0001f);
 assert(std::abs(legacy[1920]-.045f)<.0001f);
 assert(filtered[1920]<legacy[1920]*.1f);
 // Blending out loop filtering retains ordinary repeats (fractional read tolerance).
 for(std::size_t i=0;i<plain.size();++i)assert(std::abs(plain[i]-legacy[i])<.0001f);
 // The low-pass spreads the second impulse rather than erasing the whole echo.
 double second=0;for(int i=1918;i<2100;++i)second+=filtered[i]*filtered[i];assert(second>1.e-6);
 // With feedback at zero, even an aggressive loop filter cannot affect the first repeat.
 for(float fs:{44100.f,48000.f,96000.f,192000.f}){
  vc::RepeatFeedback loop;std::vector<float> buffer((std::size_t)(fs*.6f));std::size_t index=0;double e=0;
  for(int i=0;i<(int)fs;++i){float y=loop.process(i==0?.1f:0.f,buffer,index,.1f,0,1,0,1,1,fs);assert(std::isfinite(y));e+=y*y;}assert(e>0);
  // Full dry mix is exact, and large automated timing/cutoff jumps stay bounded.
  vc::RepeatFeedback automated;std::vector<float> b((std::size_t)(fs*.6f));std::size_t j=0;
  for(int i=0;i<24000;++i){const float v=(i/128)%2?1.f:0.f;float x=.1f*std::sin(i*.13f),y=automated.process(x,b,j,v,1,v,1-v,v,0,fs);assert(y==x);}
 }
 vc::RepeatFeedback small;std::vector<float> b(2);std::size_t i=0;for(int n=0;n<100;++n)assert(std::isfinite(small.process(.1f,b,i,1,1,0,1,1,1,48000)));
 std::cout<<"PASS: first repeat, filtered regeneration, loop blend, dry mix, timing automation and rate matrix\n";
}
