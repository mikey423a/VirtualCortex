#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include "StereoSequencePitch.h"

namespace vc {
// Original implementations. No Meris firmware, models, or factory presets.
inline float trackingLimit(float x,float lo,float hi,float fallback) noexcept {
 return std::isfinite(x)?std::clamp(x,lo,hi):fallback;
}
class MonoTracker {
public:
 void prepare(double rate) noexcept {
  fs=rate;divisor=std::max(1,(int)std::lround(fs/12000.));lowRate=fs/divisor;
  ring.fill(0);difference.fill(0);write=filled=decimation=hop=0;lp=0;note=candidate=-1;stable=missing=0;frequency=0;
 }
 // Bounded YIN difference search on a decimated signal; one analysis per 128 samples.
 bool process(float x,float gate) noexcept {
  lp+=float(1.-std::exp(-6.283185307*3000./fs))*(x-lp);
  if(++decimation<divisor)return false;decimation=0;
  ring[write]=lp;write=(write+1)%512;filled=std::min(filled+1,512);
  if(++hop<128||filled<512)return false;hop=0;
  float energy=0;for(int i=0;i<256;++i){const float a=at(i);energy+=a*a;}
  const int minLag=std::max(2,int(lowRate/1400.)),maxLag=std::min(250,int(lowRate/55.));
  int best=-1;
  if(energy/256>gate*gate){
   double sum=0;difference[0]=1;
   for(int lag=1;lag<=maxLag;++lag){double d=0;for(int i=0;i<256;++i){const double a=at(i)-at(i+lag);d+=a*a;}sum+=d;difference[lag]=sum>1.e-15?float(d*lag/sum):1.f;}
   for(int lag=minLag;lag<maxLag;++lag)if(difference[lag]<.18f){best=lag;while(best+1<=maxLag&&difference[best+1]<difference[best])++best;break;}
  }
  if(best>=0){
   float lag=float(best);if(best>1&&best<maxLag){const float a=difference[best-1],b=difference[best],c=difference[best+1],den=a-2*b+c;if(std::abs(den)>1.e-9f)lag+=std::clamp(.5f*(a-c)/den,-.5f,.5f);}
   const float hz=float(lowRate/lag);const int n=std::clamp(int(std::lround(69.+12.*std::log2(hz/440.))),0,127);
   if(n==candidate)++stable;else{candidate=n;stable=1;}
   missing=0;if(stable>=2){note=n;frequency=hz;}
  }else if(++missing>=4){note=candidate=-1;stable=0;frequency=0;}
  return true;
 }
 int currentNote()const noexcept{return note;}
 float hz()const noexcept{return frequency;}
 void stop()noexcept{note=candidate=-1;stable=missing=0;frequency=0;ring.fill(0);filled=hop=0;}
private:
 float at(int index)const noexcept{return ring[(write+index)%512];}
 std::array<float,512>ring{},difference{};double fs=48000,lowRate=12000;int divisor=4,write=0,filled=0,decimation=0,hop=0,note=-1,candidate=-1,stable=0,missing=0;float lp=0,frequency=0;
};
class TrackingSynth {
public:
 struct Parameters {float mix=1,transpose=0,detune=7,cutoff=1800,resonance=1,attack=8,release=100,glide=25,wave=1,gate=-45;bool enabled=false;};
 struct Frame {std::array<float,2>audio;int note=-1;float velocity=0;};
 void prepare(double rate){fs=std::isfinite(rate)?std::clamp(rate,8000.,192000.):48000.;tracker.prepare(fs);phaseA=phaseB=0;frequency=220;envelope=level=ic1=ic2=mix=0;wasEnabled=false;}
 void configure(Parameters p) noexcept {
  cfg=p;cfg.mix=trackingLimit(p.mix,0,1,1);cfg.transpose=std::round(trackingLimit(p.transpose,-24,24,0));cfg.detune=trackingLimit(p.detune,0,30,7);cfg.cutoff=trackingLimit(p.cutoff,60,18000,1800);cfg.resonance=trackingLimit(p.resonance,.5f,8,1);cfg.attack=trackingLimit(p.attack,1,500,8);cfg.release=trackingLimit(p.release,10,2000,100);cfg.glide=trackingLimit(p.glide,1,500,25);cfg.wave=trackingLimit(p.wave,0,2,1);cfg.gate=trackingLimit(p.gate,-70,-15,-45);
  attack=std::exp(-1.f/float(fs*.001*cfg.attack));release=std::exp(-1.f/float(fs*.001*cfg.release));glide=std::exp(-1.f/float(fs*.001*cfg.glide));gate=std::pow(10.f,cfg.gate/20);g=std::tan(float(3.141592653589793*std::min(double(cfg.cutoff),fs*.4)/fs));k=1/cfg.resonance;
  if(wasEnabled&&!cfg.enabled)tracker.stop();wasEnabled=cfg.enabled;
 }
 Frame process(float l,float r) noexcept {
  l=std::isfinite(l)?l:0;r=std::isfinite(r)?r:0;
  if(!cfg.enabled&&mix<1.e-7f){mix=0;return {{l,r},-1,0};}
  // Clean left input tracks before amplifier/distortion/reverb; synth is dual mono.
  if(cfg.enabled)tracker.process(l,gate);
  const int raw=cfg.enabled?tracker.currentNote():-1;const int note=raw<0?-1:std::clamp(raw+int(cfg.transpose),0,127);
  const float amp=std::min(1.f,std::abs(l)*2.f);envelope=amp+(amp>envelope?attack:release)*(envelope-amp);
  const float target=note>=0?envelope:0;level=target+(target>level?attack:release)*(level-target);
  const float hz=note>=0?float(tracker.hz()*std::pow(2.,cfg.transpose/12.)):frequency;frequency=hz+glide*(frequency-hz);
  const double da=std::clamp(double(frequency)/fs,0.,.4),db=std::clamp(da*std::pow(2.,cfg.detune/1200.),0.,.4);
  phaseA+=da;phaseA-=std::floor(phaseA);phaseB+=db;phaseB-=std::floor(phaseB);
  const float osc=.5f*(wave(phaseA,da)+wave(phaseB,db))*level;
  const float v1=(ic1+g*(osc-ic2))/(1+g*(g+k)),v2=ic2+g*v1;ic1=2*v1-ic1;ic2=2*v2-ic2;
  const float wet=std::tanh(v2);const float targetMix=cfg.enabled?cfg.mix:0;mix+=float(1.-std::exp(-1./(fs*.01)))*(targetMix-mix);
  return {{l*(1-mix)+wet*mix,r*(1-mix)+wet*mix},note,envelope};
 }
private:
 static double blep(double t,double dt)noexcept {if(dt<=0)return 0;if(t<dt){t/=dt;return t+t-t*t-1;}if(t>1-dt){t=(t-1)/dt;return t*t+t+t+1;}return 0;}
 float wave(double t,double dt)const noexcept {
  const double sine=std::sin(6.283185307179586*t),saw=2*t-1-blep(t,dt);double p=t+.5;p-=std::floor(p);const double square=(t<.5?1.:-1.)+blep(t,dt)-blep(p,dt);
  return float(cfg.wave<=1?sine+(saw-sine)*cfg.wave:saw+(square-saw)*(cfg.wave-1));
 }
 MonoTracker tracker;Parameters cfg;double fs=48000,phaseA=0,phaseB=0;float frequency=220,envelope=0,level=0,ic1=0,ic2=0,mix=0,attack=0,release=0,glide=0,gate=.005f,g=.1f,k=1;bool wasEnabled=false;
};
class SequenceCrusher {
public:
 struct Parameters {float bits=8,rate=8000,cutoff=6000,stepMs=125,depth=1,mix=.5f,mode=0,stutterMs=60,stutter=0;std::array<float,6>steps{{0,.5f,-.5f,1,0,-1}};bool enabled=false,pitch=false;};
 void prepare(double rate){fs=std::isfinite(rate)?std::clamp(rate,8000.,192000.):48000.;pitchShift.prepare(fs);for(auto& b:history)b.assign(std::size_t(fs*.3)+4,0);write=0;holdPhase=stepPhase=0;step=0;held.fill(0);filtered.fill(0);mix=0;stutterPhase=0;captured=0;previousEnabled=false;}
 void configure(Parameters p)noexcept{cfg=p;cfg.bits=trackingLimit(p.bits,1,24,8);cfg.rate=trackingLimit(p.rate,48,48000,8000);cfg.cutoff=trackingLimit(p.cutoff,60,18000,6000);cfg.stepMs=trackingLimit(p.stepMs,25,12000,125);cfg.depth=trackingLimit(p.depth,0,1,1);cfg.mix=trackingLimit(p.mix,0,1,.5f);cfg.mode=std::round(trackingLimit(p.mode,0,2,0));cfg.stutterMs=trackingLimit(p.stutterMs,5,250,60);cfg.stutter=trackingLimit(p.stutter,0,1,0);for(auto& s:cfg.steps)s=trackingLimit(s,-1,1,0);if(cfg.enabled&&!previousEnabled){step=0;stepPhase=0;captured=0;stutterPhase=0;}previousEnabled=cfg.enabled;}
 std::array<float,2> process(float l,float r)noexcept{
  std::array<float,2>out{{std::isfinite(l)?l:0,std::isfinite(r)?r:0}};
  if(history[0].empty()||(!cfg.enabled&&mix<1.e-7f)){mix=0;return out;}
  const auto length=std::clamp(std::size_t(fs*cfg.stutterMs*.001),std::size_t(2),history[0].size()-1);
  const float modulation=cfg.steps[step]*cfg.depth;
  const float rate=std::clamp(cfg.rate*(cfg.mode==0&&!cfg.pitch?std::pow(2.f,modulation*4):1.f),48.f,float(fs));
  const float cutoff=std::clamp(cfg.cutoff*(cfg.mode==1&&!cfg.pitch?std::pow(2.f,modulation*4):1.f),60.f,float(fs*.45));
  const int bits=std::clamp(int(std::lround(cfg.bits+(cfg.mode==2&&!cfg.pitch?modulation*12:0))),1,24);
  const float levels=std::pow(2.f,float(bits))-1,alpha=float(1.-std::exp(-6.283185307179586*cutoff/fs));
  holdPhase+=rate/fs;const bool update=holdPhase>=1;if(update)holdPhase-=std::floor(holdPhase);
  mix+=float(1.-std::exp(-1./(fs*.01)))*((cfg.enabled?cfg.mix:0)-mix);
  // Capture before pitch, then shift the repeated slice before quantization.
  const auto dry=out;std::array<float,2>slice=out;
  for(int ch=0;ch<2;++ch){if(captured<length)history[ch][captured]=slice[ch];else slice[ch]=slice[ch]*(1-cfg.stutter)+history[ch][stutterPhase%length]*cfg.stutter;}
  slice=pitchShift.process(slice[0],slice[1],modulation*12,cfg.pitch);
  for(int ch=0;ch<2;++ch){if(update){const float bounded=std::clamp(slice[ch],-1.f,1.f);held[ch]=bounded==0?0:2*std::round((bounded+1)*.5f*levels)/levels-1;}
   filtered[ch]+=alpha*(held[ch]-filtered[ch]);out[ch]=dry[ch]*(1-mix)+filtered[ch]*mix;
  }
  if(captured<length)++captured;else stutterPhase=(stutterPhase+1)%length;
  stepPhase+=1000./(fs*cfg.stepMs);if(stepPhase>=1){stepPhase-=std::floor(stepPhase);step=(step+1)%6;captured=stutterPhase=0;}
  return out;
 }
 int currentStep()const noexcept{return step;}
private:
 StereoSequencePitch pitchShift;Parameters cfg;double fs=48000,holdPhase=0,stepPhase=0;int step=0;std::array<std::vector<float>,2>history;std::size_t write=0,captured=0,stutterPhase=0;std::array<float,2>held{},filtered{};float mix=0;bool previousEnabled=false;
};
} // namespace vc
