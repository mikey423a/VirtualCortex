#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
namespace vc {
inline constexpr std::array<const char*,18> stereoReverbIDs{{"srDecay","srSize","srPreDelay","srDiffusion","srEarly","srMix","srLowDecay","srHighDecay","srLowFreq","srHighFreq","srModRate","srModDepth","srWidth","srDucking","srRelease","srWetLevel","srInputHP","srInputLP"}};
struct StereoReverbParameters {
 float decay=2.5f,size=1,preDelay=20,diffusion=.7f,early=.2f,mix=.2f,lowDecay=1,highDecay=.5f,lowFreq=250,highFreq=4000,modRate=.35f,modDepth=.3f,width=1,ducking=0,release=300,wetLevel=0,inputHP=40,inputLP=16000;
 bool enabled=false,preGlide=true;
};
// Original 8-line FDN. Orthogonal normalized Hadamard feedback; contractive
// low/high shelves. Linear interpolation is a convex read, including modulation.
// No branded algorithm or convolution model. Allocates only in prepare().
class StereoReverb {
 struct Delay {
  std::vector<float> data;std::size_t index=0;
  void prepare(std::size_t size){data.assign(size,0);index=0;}
  float read(double delay)const noexcept {delay=std::clamp(delay,1.,(double)data.size()-2);double pos=(double)index-delay;if(pos<0)pos+=data.size();auto a=(std::size_t)pos;auto b=a+1==data.size()?0:a+1;float f=(float)(pos-a);return data[a]+f*(data[b]-data[a]);}
  void write(float x)noexcept {data[index]=std::abs(x)<1.e-20f?0.f:x;if(++index==data.size())index=0;}
 };
 struct Allpass {Delay delay;double length=1;float process(float x,float g)noexcept{float d=delay.read(length),y=d-g*x;delay.write(x+g*y);return y;}};
public:
 void prepare(double rate){
  fs=std::isfinite(rate)?std::clamp(rate,8000.,192000.):48000.;
  for(auto& d:tank)d.prepare((std::size_t)std::ceil(fs*.18)+8);
  for(auto& d:pre)d.prepare((std::size_t)std::ceil(fs*.7)+8);
  for(std::size_t ch=0;ch<2;++ch)for(std::size_t j=0;j<4;++j){auto& a=diffusers[ch][j];a.length=fs*(.0031+.0017*j+.00037*ch);a.delay.prepare((std::size_t)std::ceil(a.length)+4);}
  preCurrent=preTarget=1;preFade=0;preFading=false;preBlend=0;
  lowState={};highState={};inputLow={};inputHigh={};phase=0;envelope=0;idle=0;counter=0;awake=false;initialized=false;cfg={};smooth={};mix=0;wetGain=1;coefficient=std::exp(-1./(fs*.02));
 }
 bool active()const noexcept{return awake;}
 void configure(StereoReverbParameters p)noexcept{
  auto safe=[](float x,float lo,float hi,float fallback){return std::isfinite(x)?std::clamp(x,lo,hi):fallback;};
  p.decay=safe(p.decay,.2f,20,2.5f);p.size=safe(p.size,.5f,2,1);p.preDelay=safe(p.preDelay,0,500,20);p.diffusion=safe(p.diffusion,0,1,.7f);p.early=safe(p.early,0,1,.2f);p.mix=safe(p.mix,0,1,.2f);
  p.lowDecay=safe(p.lowDecay,.25f,2,1);p.highDecay=safe(p.highDecay,.1f,1,.5f);p.lowFreq=safe(p.lowFreq,80,1000,250);p.highFreq=safe(p.highFreq,1500,16000,4000);p.modRate=safe(p.modRate,.05f,4,.35f);p.modDepth=safe(p.modDepth,0,2,.3f);
  p.width=safe(p.width,0,1.5f,1);p.ducking=safe(p.ducking,0,1,0);p.release=safe(p.release,20,2000,300);p.wetLevel=safe(p.wetLevel,-24,12,0);p.inputHP=safe(p.inputHP,20,1000,40);p.inputLP=safe(p.inputLP,1000,20000,16000);
  cfg=p;attack=std::exp(-1./(fs*.005));release=std::exp(-1./(fs*.001*p.release));wetTarget=std::pow(10.f,p.wetLevel/20.f);
  if(p.enabled){awake=true;idle=0;}if(!initialized){smooth=p;mix=0;wetGain=wetTarget;preCurrent=preTarget=p.preDelay*.001*fs+1;initialized=true;updateCoefficients();}
 }
 std::array<float,2> process(float left,float right)noexcept{
  if(!std::isfinite(left))left=0;
  if(!std::isfinite(right))right=0;
  if(!awake||pre[0].data.empty())return {left,right};
  auto move=[&](float& x,float target){float next=(float)(target+coefficient*(x-target));x=(next==x||std::abs(next-target)<1.e-7f)?target:next;};
  move(mix,cfg.enabled?cfg.mix:0);move(wetGain,wetTarget);
  move(smooth.decay,cfg.decay);move(smooth.size,cfg.size);move(smooth.preDelay,cfg.preDelay);move(smooth.diffusion,cfg.diffusion);move(smooth.early,cfg.early);move(smooth.lowDecay,cfg.lowDecay);move(smooth.highDecay,cfg.highDecay);move(smooth.lowFreq,cfg.lowFreq);move(smooth.highFreq,cfg.highFreq);move(smooth.modRate,cfg.modRate);move(smooth.modDepth,cfg.modDepth);move(smooth.width,cfg.width);move(smooth.ducking,cfg.ducking);move(smooth.inputHP,cfg.inputHP);move(smooth.inputLP,cfg.inputLP);
  if(counter++%32==0)updateCoefficients();
  float magnitude=std::max(std::abs(left),std::abs(right));double envC=magnitude>envelope?attack:release;envelope=magnitude+envC*(envelope-magnitude);
  // Complete an in-flight crossfade before honoring a mode change or new target.
  // Rapid automation queues the latest target; neither read head moves mid-fade.
  const auto fadeLength=std::max<std::size_t>(1,(std::size_t)(fs*.02));
  if(!preFading){
   if(cfg.preGlide)preCurrent=smooth.preDelay*.001*fs+1;
   else {const double wanted=cfg.preDelay*.001*fs+1;if(std::abs(wanted-preCurrent)>.05){preTarget=wanted;preFade=0;preFading=true;}}
  }
  preBlend=preFading?(float)(preFade+1)/(float)fadeLength:0.f;
  std::array<float,2> input{{cfg.enabled?left:0.f,cfg.enabled?right:0.f}},early{},diffused{};
  for(std::size_t ch=0;ch<2;++ch){
   inputHigh[ch]=(1-inputHPC)*input[ch]+inputHPC*inputHigh[ch];float hp=input[ch]-inputHigh[ch];inputLow[ch]=(1-inputLPC)*hp+inputLPC*inputLow[ch];pre[ch].write(inputLow[ch]);
   // write advances the cursor: +1 aligns the configured predelay with input.
   float x=readPre(ch,0);
   const double taps[4]={.0073,.0137,.0239,.0371};for(int j=0;j<4;++j){float tap=readPre(ch,(taps[j]+ch*.0011)*smooth.size*fs);early[ch]+=tap*(j%2?-.22f:.3f);}
   for(auto& a:diffusers[ch])x=a.process(x,smooth.diffusion*.72f);
   diffused[ch]=x;
  }
  if(preFading&&++preFade>=fadeLength){preCurrent=preTarget;preFading=false;smooth.preDelay=(float)((preCurrent-1)*1000./fs);}
  std::array<float,8> feedback{};
  float wetL=0,wetR=0;
  for(std::size_t i=0;i<8;++i){double length=times[i]*smooth.size*fs+std::sin(phase+i*.897597901)*smooth.modDepth*.001*fs;float d=tank[i].read(length);
   wetL+=d*((i&4)?-.35355339f:.35355339f);wetR+=d*((i&2)?-.35355339f:.35355339f);
   lowState[i]=(1-lowC)*d+lowC*lowState[i];float lowShaped=gMid[i]*d+(gLow[i]-gMid[i])*lowState[i];
   highState[i]=(1-highC)*lowShaped+highC*highState[i];feedback[i]=highRatio[i]*lowShaped+(1-highRatio[i])*highState[i];
  }
  // H8/sqrt(8) is orthogonal: mixing adds no feedback energy.
  for(std::size_t stride=1;stride<8;stride*=2)for(std::size_t base=0;base<8;base+=2*stride)for(std::size_t j=0;j<stride;++j){auto a=feedback[base+j],b=feedback[base+j+stride];feedback[base+j]=a+b;feedback[base+j+stride]=a-b;}
  for(std::size_t i=0;i<8;++i){float injection=.35355339f*(diffused[0]+((i&1)?-diffused[1]:diffused[1]));tank[i].write(std::clamp(injection+feedback[i]*.35355339f,-16.f,16.f));}
  phase+=6.283185307179586*smooth.modRate/fs;if(phase>=6.283185307179586)phase-=6.283185307179586;
  wetL=wetL*(1-smooth.early)+early[0]*smooth.early;wetR=wetR*(1-smooth.early)+early[1]*smooth.early;
  float mid=(wetL+wetR)*.5f,side=(wetL-wetR)*.5f*smooth.width,duck=1.f/(1.f+8.f*smooth.ducking*(float)envelope);
  const float gain=mix*wetGain*duck;std::array<float,2> out{{left*(1-mix)+(mid+side)*gain,right*(1-mix)+(mid-side)*gain}};
  if(!cfg.enabled){if(++idle>fs*120.)awake=false;}else idle=0;
  for(auto& x:out)if(!std::isfinite(x))x=0;
  return out;
 }
private:
 float readPre(std::size_t channel,double offset)const noexcept{
  const float a=pre[channel].read(preCurrent+offset);
  return preFading?a*(1-preBlend)+pre[channel].read(preTarget+offset)*preBlend:a;
 }
 void updateCoefficients()noexcept{
  auto pole=[&](float hz){return (float)std::exp(-6.283185307179586*std::min((double)hz,fs*.45)/fs);};
  lowC=pole(smooth.lowFreq);highC=pole(smooth.highFreq);inputHPC=pole(smooth.inputHP);inputLPC=pole(smooth.inputLP);
  for(std::size_t i=0;i<8;++i){double t=times[i]*smooth.size;gMid[i]=(float)std::pow(.001,t/smooth.decay);gLow[i]=(float)std::pow(.001,t/(smooth.decay*smooth.lowDecay));float gHigh=(float)std::pow(.001,t/(smooth.decay*smooth.highDecay));highRatio[i]=gHigh/gMid[i];}
 }
 inline static constexpr std::array<double,8> times{{.0293,.0371,.0419,.0473,.0539,.0611,.0677,.0793}};
 std::array<Delay,8> tank;std::array<Delay,2> pre;std::array<std::array<Allpass,4>,2> diffusers;
 std::array<float,8> lowState{},highState{},gMid{},gLow{},highRatio{};std::array<float,2> inputHigh{},inputLow{};
 StereoReverbParameters cfg{},smooth{};double fs=48000,phase=0,envelope=0,coefficient=0,attack=0,release=0,idle=0;std::size_t counter=0;
 double preCurrent=1,preTarget=1;std::size_t preFade=0;float preBlend=0;bool preFading=false;
 float mix=0,wetGain=1,wetTarget=1,lowC=0,highC=0,inputHPC=0,inputLPC=0;bool awake=false,initialized=false;
};
}
