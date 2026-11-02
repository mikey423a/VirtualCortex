#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/StereoDelay.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
void warm(vc::StereoDelay& d,int samples=24000){for(int i=0;i<samples;++i)d.process(0,0);}
int main(){
 for(double fs:{44100.,48000.,96000.,192000.}){
  vc::StereoDelay d;d.prepare(fs);vc::StereoDelayParameters p;p.enabled=true;p.timeL=100;p.timeR=150;p.feedback=0;p.mix=1;p.filterMix=0;d.configure(p);warm(d,(int)(fs*.5));
  // Independent channels, no accidental stereo crosstalk; wet impulse timing.
  double rightEnergy=0;float peak=0;int peakIndex=0;
  for(int i=0;i<(int)(fs*.3);++i){auto y=d.process(i==0?.1f:0.f,0);rightEnergy+=y[1]*y[1];if(std::abs(y[0])>peak){peak=std::abs(y[0]);peakIndex=i;}}
  assert(rightEnergy==0);assert(std::abs(peakIndex-(int)(fs*.1))<=1);assert(std::abs(peak-.1f)<.0001);
  // Ping-pong actually transfers repeats from L to R, including mono guitar input.
  d.prepare(fs);p.feedback=.5f;p.pingPong=true;d.configure(p);warm(d,(int)(fs*.5));float first=0,second=0,third=0;
  for(int i=0;i<(int)(fs*.5);++i){auto y=d.process(i==0?.1f:0.f,i==0?.1f:0.f);if(std::abs(i-(int)(fs*.1))<2)first=std::max(first,std::abs(y[0]));if(std::abs(i-(int)(fs*.25))<2)second=std::max(second,std::abs(y[1]));if(std::abs(i-(int)(fs*.35))<2)third=std::max(third,std::abs(y[0]));}
  assert(std::abs(first-.1f)<.0001);assert(std::abs(second-.05f)<.0002);assert(std::abs(third-.025f)<.0002);
  // Automated time/routing changes and extreme valid feedback remain finite.
  p.modDepth=10;p.ducking=1;p.feedback=.95f;p.mix=.7f;p.width=1.5f;
  for(int block=0;block<50;++block){p.timeL=block%2?1:16000;p.timeR=block%3?300:12000;p.tape=block%2;p.highCut=block%2?1000:20000;d.configure(p);for(int i=0;i<128;++i){auto y=d.process(.1f*std::sin((block*128+i)*.1),.07f);for(float v:y)assert(std::isfinite(v)&&std::abs(v)<10);}}
 }
 // Ducking lowers wet energy while loud input is present.
 auto energy=[](float duck){vc::StereoDelay d;d.prepare(48000);vc::StereoDelayParameters p;p.enabled=true;p.timeL=20;p.timeR=30;p.feedback=.4f;p.mix=1;p.ducking=duck;p.filterMix=0;d.configure(p);warm(d);double e=0;for(int i=0;i<48000;++i){auto y=d.process(.3f*std::sin(i*.1f),.3f*std::sin(i*.1f));if(i>24000)e+=y[0]*y[0]+y[1]*y[1];}return e;};assert(energy(1)<energy(0)*.4);
 // Fully disabled module is transparent; malformed controls are sanitized.
 vc::StereoDelay d;d.prepare(48000);assert(d.process(.123f,-.234f)==(std::array<float,2>{.123f,-.234f}));vc::StereoDelayParameters p;p.enabled=true;p.timeL=std::numeric_limits<float>::quiet_NaN();p.feedback=std::numeric_limits<float>::infinity();d.configure(p);for(int i=0;i<1000;++i){auto y=d.process(.1f,0);assert(std::isfinite(y[0])&&std::isfinite(y[1]));}
 // Fractional-delay output at 1 kHz retains amplitude within 0.1 dB.
 d.prepare(48000);p={};p.enabled=true;p.timeL=100.123f;p.mix=1;p.feedback=0;p.filterMix=0;d.configure(p);warm(d);double in=0,out=0;for(int i=0;i<48000;++i){float x=.1f*std::sin(6.283185307f*1000*i/48000);auto y=d.process(x,0);if(i>24000){in+=x*x;out+=y[0]*y[0];}}assert(std::abs(10*std::log10(out/in))<.1);
 // Disabling drains rather than freezing old audio forever.
 p.enabled=false;d.configure(p);for(int i=0;i<800000;++i)d.process(0,0);assert(!d.active());assert(d.process(.123f,-.2f)==(std::array<float,2>{.123f,-.2f}));
 std::cout<<"PASS: stereo isolation/timing, ping-pong, ducking, automation, interpolation and bypass drain\n";
}
