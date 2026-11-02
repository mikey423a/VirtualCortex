#pragma once
#include "UtilitySpatialEffects.h"
#include <array>
#include <vector>
namespace vc {
inline constexpr std::array<const char*,19> stereoDelayIDs{{"sdTimeL","sdTimeR","sdFeedback","sdCross","sdLowCut","sdHighCut","sdMix","sdModRate","sdModDepth","sdDucking","sdDuckRelease","sdWidth","sdWetLevel","sdGlide","sdBpm","sdBeatsL","sdBeatsR","sdFilterMix","sdDiffusion"}};
inline constexpr std::array<const char*,4> stereoDelayFlagIDs{{"stereoDelayOn","sdPingPong","sdTape","sdSync"}};
struct StereoDelayParameters {
 float timeL=320,timeR=480,feedback=.35f,cross=0,lowCut=40,highCut=16000,mix=.2f,modRate=.5f,modDepth=0,ducking=0,duckRelease=250,width=1,wetLevel=0,glide=50,filterMix=1,diffusion=0;
 bool enabled=false,pingPong=false,tape=false;
};
// Original stereo post-chain delay, not a model of a proprietary device.
// prepare allocates while stopped; configure/process do not allocate or lock.
class StereoDelay {
public:
 void prepare(double rate){
  fs_=std::isfinite(rate)?std::clamp(rate,8000.,192000.):48000.;const auto size=(std::size_t)std::ceil(fs_*16.02)+8;
  for(auto& b:buffer_)b.assign(size,0.f);index_=0;phase_=0;envelope_=0;initialized_=false;dormant_=true;idle_=0;counter_=0;
  for(int ch=0;ch<2;++ch)for(int stage=0;stage<4;++stage){const double ms[]={3.7,7.3,11.3,17.9};auto& d=diffusers_[(std::size_t)ch][(std::size_t)stage];d.data.assign((std::size_t)std::max(1.,std::round(fs_*.001*(ms[stage]+ch*.71))),0.f);d.index=0;}
  heads_={};high_={};low_={};smooth_=StereoDelayParameters{};smooth_.mix=0;smooth_.feedback=0;cfg_={};coefficient_=std::exp(-1./(fs_*.01));
 }
 bool active() const noexcept {return !dormant_;}
 void configure(StereoDelayParameters p) noexcept {
  auto safe=[](float v,float lo,float hi,float fallback){return std::isfinite(v)?std::clamp(v,lo,hi):fallback;};
  p.timeL=safe(p.timeL,1,16000,320);p.timeR=safe(p.timeR,1,16000,480);p.feedback=safe(p.feedback,0,.95f,.35f);p.cross=safe(p.cross,0,1,0);
  p.lowCut=safe(p.lowCut,20,1000,40);p.highCut=safe(p.highCut,1000,20000,16000);p.mix=safe(p.mix,0,1,.2f);p.modRate=safe(p.modRate,.05f,8,.5f);p.modDepth=safe(p.modDepth,0,10,0);
  p.ducking=safe(p.ducking,0,1,0);p.duckRelease=safe(p.duckRelease,20,2000,250);p.width=safe(p.width,0,1.5f,1);p.wetLevel=safe(p.wetLevel,-24,12,0);p.glide=safe(p.glide,5,500,50);p.filterMix=safe(p.filterMix,0,1,1);
  p.diffusion=safe(p.diffusion,0,1,0);
  cfg_=p;release_=std::exp(-1./(fs_*.001*p.duckRelease));attack_=std::exp(-1./(fs_*.005));glideCoefficient_=std::exp(-1./(fs_*.001*p.glide));wetTarget_=std::pow(10.f,p.wetLevel/20.f);
  if(p.enabled&&dormant_){dormant_=false;initialized_=false;idle_=0;}
 }
 std::array<float,2> process(float left,float right) noexcept {
  if(!std::isfinite(left))left=0;if(!std::isfinite(right))right=0;
  if(dormant_||buffer_[0].empty())return {left,right};
  const std::array<float,2> dry{{left,right}};
  if(!initialized_){smooth_=cfg_;smooth_.mix=0;smooth_.feedback=cfg_.enabled?cfg_.feedback:0;wetGain_=wetTarget_;heads_[0].current=cfg_.timeL*fs_*.001;heads_[1].current=cfg_.timeR*fs_*.001;initialized_=true;counter_=0;}
  auto move=[&](float& x,float target){const float next=(float)(target+coefficient_*(x-target));x=(next==x||std::abs(next-target)<1.e-7f)?target:next;};
  move(smooth_.mix,cfg_.enabled?cfg_.mix:0);move(smooth_.feedback,cfg_.enabled?cfg_.feedback:0);move(smooth_.cross,cfg_.pingPong?1.f:cfg_.cross);move(smooth_.lowCut,cfg_.lowCut);move(smooth_.highCut,cfg_.highCut);
  move(smooth_.modRate,cfg_.modRate);move(smooth_.modDepth,cfg_.modDepth);move(smooth_.ducking,cfg_.ducking);move(smooth_.width,cfg_.width);move(smooth_.filterMix,cfg_.filterMix);move(wetGain_,wetTarget_);move(smooth_.diffusion,cfg_.diffusion);
  if(counter_++%16==0)for(int ch=0;ch<2;++ch){high_[(std::size_t)ch].cut((float)fs_,smooth_.lowCut,true);low_[(std::size_t)ch].cut((float)fs_,smooth_.highCut,false);}
  const double magnitude=std::max(std::abs(left),std::abs(right)),envCoefficient=magnitude>envelope_?attack_:release_;envelope_=magnitude+envCoefficient*(envelope_-magnitude);
  std::array<float,2> wet{},returned{};
  for(int ch=0;ch<2;++ch){
   const double wanted=(ch==0?cfg_.timeL:cfg_.timeR)*fs_*.001;
   const double modulation=smooth_.modDepth*.001*fs_*std::sin(phase_+ch*1.5707963267948966);
   wet[(std::size_t)ch]=readHead(ch,wanted,modulation);
   const float filtered=low_[(std::size_t)ch].process(high_[(std::size_t)ch].process(wet[(std::size_t)ch]));returned[(std::size_t)ch]=(1-smooth_.filterMix)*wet[(std::size_t)ch]+smooth_.filterMix*filtered;
  }
  // Diffuse only the feedback return: the first repeat stays intact, subsequent
  // repeats disperse into a tail. Four independent allpasses per channel.
  // Keep their history running even at zero amount; zero takes the exact old path.
  for(std::size_t ch=0;ch<2;++ch){float scattered=returned[ch];for(auto& d:diffusers_[ch])scattered=d.process(scattered);
   if(smooth_.diffusion>0) returned[ch]=(1-smooth_.diffusion)*returned[ch]+smooth_.diffusion*scattered;
  }
  float inL=cfg_.enabled?left:0,inR=cfg_.enabled?right:0;if(cfg_.enabled&&cfg_.pingPong){inL=(left+right)*.5f;inR=0;}
  buffer_[0][index_]=std::clamp(inL+smooth_.feedback*((1-smooth_.cross)*returned[0]+smooth_.cross*returned[1]),-16.f,16.f);
  buffer_[1][index_]=std::clamp(inR+smooth_.feedback*((1-smooth_.cross)*returned[1]+smooth_.cross*returned[0]),-16.f,16.f);
  if(++index_==buffer_[0].size())index_=0;phase_+=6.283185307179586*smooth_.modRate/fs_;if(phase_>=6.283185307179586)phase_-=6.283185307179586;
  const float mid=(wet[0]+wet[1])*.5f,side=(wet[0]-wet[1])*.5f*smooth_.width;wet[0]=mid+side;wet[1]=mid-side;
  const float duck=1.f/(1.f+8.f*smooth_.ducking*(float)envelope_);
  std::array<float,2> output{};for(int ch=0;ch<2;++ch){const float y=(1-smooth_.mix)*dry[(std::size_t)ch]+smooth_.mix*wet[(std::size_t)ch]*wetGain_*duck;output[(std::size_t)ch]=std::isfinite(y)?y:0;}
  if(cfg_.enabled)idle_=0;else if(++idle_>buffer_[0].size()+(std::size_t)(fs_*.2)&&smooth_.mix<1.e-7f)dormant_=true;
  return output;
 }
private:
 struct Diffuser {std::vector<float> data;std::size_t index=0;float process(float x) noexcept {const float delayed=data[index],y=delayed-.6f*x;data[index]=x+.6f*y;if(++index==data.size())index=0;return y;}};
 std::array<std::array<Diffuser,4>,2> diffusers_;
 struct Head {double current=0,target=0;std::size_t fade=0;bool fading=false;};
 float read(int channel,double distance) const noexcept {
  const auto& b=buffer_[(std::size_t)channel];const auto n=b.size();distance=std::clamp(distance,2.,(double)n-3);const auto whole=(std::size_t)distance;const double f=distance-whole;
  auto at=[&](std::size_t d){return b[(index_+n-d)%n];};
  return (float)(at(whole-1)*(-f*(f-1)*(f-2)/6)+at(whole)*((f+1)*(f-1)*(f-2)*.5)+at(whole+1)*(-(f+1)*f*(f-2)*.5)+at(whole+2)*((f+1)*f*(f-1)/6));
 }
 float readHead(int channel,double wanted,double modulation) noexcept {
  auto& h=heads_[(std::size_t)channel];
  if(cfg_.tape){h.current=wanted+glideCoefficient_*(h.current-wanted);h.fading=false;return read(channel,h.current+modulation);}
  if(!h.fading&&std::abs(wanted-h.current)>.05){h.target=wanted;h.fade=0;h.fading=true;}
  if(!h.fading)return read(channel,h.current+modulation);
  const auto length=std::max<std::size_t>(1,(std::size_t)(fs_*.02));const float t=(float)(h.fade+1)/(float)length;
  const float y=read(channel,h.current+modulation)*(1-t)+read(channel,h.target+modulation)*t;
  if(++h.fade>=length){h.current=h.target;h.fading=false;}return y;
 }
 double fs_=48000,phase_=0,envelope_=0,coefficient_=0,release_=0,attack_=0,glideCoefficient_=0;
 std::array<std::vector<float>,2> buffer_;std::array<Head,2> heads_{};std::array<Biquad,2> high_,low_;
 StereoDelayParameters cfg_,smooth_;std::size_t index_=0,idle_=0;unsigned counter_=0;float wetTarget_=1,wetGain_=1;bool initialized_=false,dormant_=true;
};
}
