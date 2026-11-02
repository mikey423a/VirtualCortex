#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
static double energy(const std::vector<float>& x){double e=0;for(float v:x){assert(std::isfinite(v));e+=v*v;}return e;}
int main(){
 // Integer base delay: first wet chorus impulse must occur exactly 5 ms later.
 {std::vector<float> delay(30000,0);std::size_t index=0;float phase=0;
  for(int i=0;i<1000;++i){float y=vc::modulatedDelay(i==0?1.f:0.f,delay,index,phase,1,0,0,0,0,48000,false);assert(std::abs(y-(i==240?1.f:0.f))<1.e-4f);}}
 // Fractional flanger delay: a 9.6 sample delay distributes energy to samples 9/10.
 {std::vector<float> delay(30000,0);std::size_t index=0;float phase=0;float sum=0;
  for(int i=0;i<100;++i){float y=vc::modulatedDelay(i==0?1.f:0.f,delay,index,phase,1,0,0,0,0,48000,true);if(i!=9&&i!=10)assert(std::abs(y)<1.e-6f);sum+=y;}assert(std::abs(sum-1)<1.e-6f);}
 for(float fs:{44100.f,48000.f,96000.f,192000.f}){
  // Six allpass stages preserve steady-state sinusoidal energy without feedback.
  vc::SixStagePhaser phaser;double inputEnergy=0,outputEnergy=0;
  for(int i=0;i<(int)fs;++i){float x=.1f*std::sin(6.283185307f*440.f*i/fs);float y=phaser.process(x,1,.3f,0,.2f,0,fs);assert(std::isfinite(y));if(i>(int)fs/2){inputEnergy+=x*x;outputEnergy+=y*y;}}
  assert(std::abs(outputEnergy/inputEnergy-1.)<.01);
  // Manual wah: 300 Hz bandpass attenuates a 6 kHz tone much more than 300 Hz.
  double low=0,high=0;vc::EnvelopeWah lowWah,highWah;
  for(int i=0;i<(int)fs/2;++i){const float a=lowWah.process(.1f*std::sin(6.283185307f*300.f*i/fs),0,1,0,.4f,.1f,.1f,fs);const float b=highWah.process(.1f*std::sin(6.283185307f*6000.f*i/fs),0,1,0,.4f,.1f,.1f,fs);if(i>(int)fs/4){low+=a*a;high+=b*b;}}
  assert(low>high*20);
  // Fully dry mix is exact for every new algorithm.
  assert(phaser.process(.123f,0,.5f,.5f,.5f,.5f,fs)==.123f);
  assert(lowWah.process(.123f,.5f,0,.5f,.5f,.5f,.5f,fs)==.123f);
  for(auto type:{vc::BlockType::Chorus,vc::BlockType::Flanger,vc::BlockType::Phaser,vc::BlockType::Wah,vc::BlockType::BitCrusher}){
   vc::SignalGrid grid;grid.set(3,7,{type,true});vc::SignalGrid restored;assert(vc::decodeGrid(vc::encodeGrid(grid),restored));assert(restored.get(3,7)->type==type);
   auto engine=std::make_unique<vc::PortableAudioEngine>();engine->prepare(fs);assert(engine->configure(restored,{false,false,false,true}));
   vc::SlotParameters p;p.mix=1;p.feedback=.99f;p.depth=1;assert(engine->setSlotParameters(3,7,p));
   std::vector<float> input((std::size_t)(fs*.08f)*2);for(std::size_t i=0;i<input.size();++i)input[i]=.1f*std::sin(i*.13f);assert(engine->process(input.data(),input.size()/2,2));assert(energy(input)>0);
   for(float y:input)assert(std::abs(y)<=1);
  }
 }
 // Sample hold is exact; low-resolution quantization has larger error than high resolution.
 {vc::BitCrusher low,high,hold;double lowError=0,highError=0;float held=0;
  for(int i=0;i<1000;++i){const float x=.5f*std::sin(i*.13f);float a=low.process(x,0,1,0,.5f),b=high.process(x,1,1,0,.5f);lowError+=(x-a)*(x-a);highError+=(x-b)*(x-b);
   float y=hold.process(x,.5f,1,3.f/63.f,.5f);if(i%4==0)held=y;else assert(y==held);}
  assert(lowError>highError*1000);assert(hold.process(.123f,0,0,0,.5f)==.123f);}
 // Modulation is audible compared with the stationary version for each delay mode.
 for(bool flange:{false,true}){std::vector<float> a(30000),b(30000);std::size_t ai=0,bi=0;float ap=0,bp=0;double difference=0;
  for(int i=0;i<48000;++i){float x=.1f*std::sin(i*.13f);difference+=std::abs(vc::modulatedDelay(x,a,ai,ap,.5f,.5f,.2f,.3f,.8f,48000,flange)-vc::modulatedDelay(x,b,bi,bp,.5f,.5f,.2f,.3f,0,48000,flange));}assert(difference>100);}
 std::cout<<"PASS: chorus/flanger delay, phaser magnitude, wah response, quantization/hold, dry mix and rate/grid matrix\n";
}
