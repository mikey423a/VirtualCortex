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
 // Measure the fractional read against a continuous-time sinusoid.
 for(float fs:{44100.f,48000.f,96000.f}) {
  std::vector<float> legacy(4096),detail(4096);
  std::size_t legacyIndex=0,detailIndex=0;
  float legacyPhase=0,detailPhase=0;
  const float distance=std::floor(fs*.005f)+.5f;
  const float time=(distance/(fs*.001f)-5.f)/20.f;
  constexpr double omega=6.283185307179586/6.;
  double oldError=0,newError=0;
  for(int i=0;i<10000;++i) {
   const float input=.2f*static_cast<float>(std::sin(omega*i));
   const float a=vc::modulatedDelay(input,legacy,legacyIndex,legacyPhase,1.f,time,0.f,0.f,0.f,fs,false);
   const float b=vc::modulatedDelay(input,detail,detailIndex,detailPhase,1.f,time,0.f,0.f,0.f,fs,false,true);
   if(i<2000)continue;
   const double expected=.2*std::sin(omega*(static_cast<double>(i)-distance));
   oldError+=(a-expected)*(a-expected);
   newError+=(b-expected)*(b-expected);
  }
  assert(newError<.05*oldError);
  // The optional output read must not change the feedback history.
  for(int i=0;i<12000;++i) {
   const float input=i%71==0?.6f:0.f;
   vc::modulatedDelay(input,legacy,legacyIndex,legacyPhase,1.f,time,1.f,.6f,1.f,fs,true);
   const float y=vc::modulatedDelay(input,detail,detailIndex,detailPhase,1.f,time,1.f,.6f,1.f,fs,true,true);
   assert(std::isfinite(y));
  }
  assert(legacy==detail&&legacyIndex==detailIndex&&legacyPhase==detailPhase);
  std::cout<<"MOD_DETAIL rate="<<fs<<" squared_error_ratio="<<newError/oldError<<"\n";
 }
 // Compare moving reads with a band-limited source evaluated at the current
 // fractional position. This reference includes the intentional pitch sweep.
 for(float fs:{44100.f,48000.f,96000.f}) for(bool flanger:{false,true}) {
  for(float hz:{1000.f,6500.f}) {
   std::vector<float> linear(4096),cubic(4096);
   std::size_t linearIndex=0,cubicIndex=0;
   float linearPhase=0,cubicPhase=0;
   constexpr float time=.23f,rate=.37f,depth=.81f;
   const double omega=6.283185307179586*hz/fs;
   double linearError=0,cubicError=0,linearEnergy=0,cubicEnergy=0,referenceEnergy=0;
   for(int i=0;i<static_cast<int>(fs);++i) {
    const float x=static_cast<float>(.2*std::sin(omega*i));
    const float lfo=.5f+.5f*std::sin(linearPhase);
    const float ms=flanger ? .2f+5.f*time+3.f*depth*lfo : 5.f+20.f*time+10.f*depth*lfo;
    const float distance=std::clamp(fs*.001f*ms,1.f,4095.f);
    const float a=vc::modulatedDelay(x,linear,linearIndex,linearPhase,1.f,time,0.f,rate,depth,fs,flanger);
    const float b=vc::modulatedDelay(x,cubic,cubicIndex,cubicPhase,1.f,time,0.f,rate,depth,fs,flanger,true);
    if(i<4096)continue;
    const double expected=.2*std::sin(omega*(i-static_cast<double>(distance)));
    linearError+=(a-expected)*(a-expected);
    cubicError+=(b-expected)*(b-expected);
    linearEnergy+=a*a;
    cubicEnergy+=b*b;
    referenceEnergy+=expected*expected;
   }
   assert(linearPhase==cubicPhase&&linear==cubic);
   assert(cubicError<.5*linearError);
   assert(std::abs(cubicEnergy/referenceEnergy-1.)<std::abs(linearEnergy/referenceEnergy-1.));
   std::cout<<"MOD_SWEEP rate="<<fs<<" flanger="<<flanger<<" hz="<<hz
            <<" error_ratio="<<cubicError/linearError
            <<" linear_db="<<10.*std::log10(linearEnergy/referenceEnergy)
            <<" detail_db="<<10.*std::log10(cubicEnergy/referenceEnergy)<<"\n";
  }
 }
 // Exercise live toggles through the real grid with continuously running
 // feedback. Fixed-mode engines provide both endpoints for each sample.
 for(double fs:{44100.,48000.,96000.}) {
  for(auto type:{vc::BlockType::Chorus,vc::BlockType::Flanger,vc::BlockType::Vibrato}) {
   auto moving=std::make_unique<vc::PortableAudioEngine>();
   auto linear=std::make_unique<vc::PortableAudioEngine>();
   auto cubic=std::make_unique<vc::PortableAudioEngine>();
   vc::SignalGrid grid;
   assert(grid.set(0,0,{type,true}));
   vc::SlotParameters controls;
   controls.mix=1.f;controls.feedback=.8f;controls.depth=.8f;controls.rate=.4f;
   for(auto* engine:{moving.get(),linear.get(),cubic.get()}) {
    engine->prepare(fs,4096);
    assert(engine->configure(grid,{true,false,false,false}));
    controls.detailedModulation=engine==cubic.get();
    assert(engine->setSlotParameters(0,0,controls));
   }
   const int duration=static_cast<int>(std::lround(fs*.005));
   const int first=5000,reverse=first+duration/2,enableAgain=first+3*duration;
   int weightFrames=0;
   double firstStep=0,hardStep=0;
   for(int i=0;i<enableAgain+2*duration;++i) {
    const bool enabled=(i>=first&&i<reverse)||i>=enableAgain;
    if(i==first||i==reverse||i==enableAgain){
     controls.detailedModulation=enabled;
     assert(moving->setSlotParameters(0,0,controls));
    }
    if(enabled)weightFrames=std::min(duration,weightFrames+1);
    else weightFrames=std::max(0,weightFrames-1);
    const float weight=static_cast<float>(weightFrames)/duration;
    std::array<float,2> a{.2f*std::sin(i*.87f),.15f*std::cos(i*.69f)},b=a,y=a;
    assert(linear->process(a.data(),1,2));
    assert(cubic->process(b.data(),1,2));
    assert(moving->process(y.data(),1,2));
    for(int ch=0;ch<2;++ch){
     const float expected=weightFrames==duration ? b[ch] : a[ch]+weight*(b[ch]-a[ch]);
     assert(std::abs(y[ch]-expected)<2.e-6f);
     if(weightFrames==0||weightFrames==duration)assert(y[ch]==expected);
     if(i==first) {
      firstStep=std::max(firstStep,static_cast<double>(std::abs(y[ch]-a[ch])));
      hardStep=std::max(hardStep,static_cast<double>(std::abs(b[ch]-a[ch])));
     }
    }
   }
   assert(hardStep>1.e-5&&firstStep<hardStep*.02);
   std::cout<<"MOD_TOGGLE rate="<<fs<<" block="<<int(type)
            <<" immediate_residual="<<hardStep<<" ramp_residual="<<firstStep<<"\n";
  }
 }
 std::cout<<"PASS: chorus/flanger delay, phaser magnitude, wah response, quantization/hold, dry mix and rate/grid matrix\n";
}
