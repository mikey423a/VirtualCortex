#pragma once
#include "UtilitySpatialEffects.h"
namespace vc {
// Optional filtered repeat-return path. The first repeat and dry input are not
// filtered; each additional regeneration passes through the loop filters again.
// Third-order Lagrange reading and smoothed time/cutoff changes; no allocation.
class RepeatFeedback {
public:
 float process(float input,std::vector<float>& buffer,std::size_t& index,float time,float feedback,float highpass,float lowpass,float filterMix,float mix,float fs) noexcept {
  const auto n=buffer.size();if(n<2)return input;
  const float wanted=std::clamp(fs*(.02f+.48f*time),n>=8?2.f:1.f,(float)(n>=8?n-3:n-1));
  const float hpTarget=20.f+980.f*highpass,lpTarget=1000.f+19000.f*lowpass;
  if(!initialized_||fs!=fs_){fs_=fs;coefficient_=std::exp(-1.f/(fs*.02f));delay_=wanted;hp_=hpTarget;lp_=lpTarget;blend_=filterMix;feedback_=feedback;mix_=mix;initialized_=true;counter_=0;}
  auto smooth=[&](float& current,float target){current=target+coefficient_*(current-target);};
  smooth(delay_,wanted);smooth(hp_,hpTarget);smooth(lp_,lpTarget);smooth(blend_,filterMix);smooth(feedback_,feedback);smooth(mix_,mix);
  if(counter_++%16==0){high_.cut(fs,hp_,true);low_.cut(fs,lp_,false);}
  const auto whole=(std::size_t)delay_;const float f=delay_-(float)whole;
  const auto at=[&](std::size_t distance){return buffer[(index+n-distance)%n];};
  float delayed=at(whole);
  if(n>=8)delayed=at(whole-1)*(-f*(f-1.f)*(f-2.f)/6.f)+at(whole)*((f+1.f)*(f-1.f)*(f-2.f)*.5f)+at(whole+1)*(-(f+1.f)*f*(f-2.f)*.5f)+at(whole+2)*((f+1.f)*f*(f-1.f)/6.f);
  const float filtered=low_.process(high_.process(delayed));
  const float returned=(1.f-blend_)*delayed+blend_*filtered;
  buffer[index]=std::clamp(input+.9f*feedback_*returned,-2.f,2.f);index=(index+1)%n;
  return (1.f-mix_)*input+mix_*delayed;
 }
private:
 bool initialized_=false;unsigned counter_=0;float fs_=0,coefficient_=0,delay_=0,hp_=20,lp_=20000,blend_=1,feedback_=0,mix_=0;
 Biquad high_,low_;
};
}
