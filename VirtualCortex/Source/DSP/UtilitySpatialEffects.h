#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>
namespace vc {
class Biquad {
public:
 void coefficients(double b0,double b1,double b2,double a0,double a1,double a2) noexcept {b0_=b0/a0;b1_=b1/a0;b2_=b2/a0;a1_=a1/a0;a2_=a2/a0;}
 void cut(float fs,float frequency,bool highpass) noexcept {
  const double w=6.283185307179586*std::clamp(frequency,10.f,fs*.45f)/fs,c=std::cos(w),alpha=std::sin(w)*.7071067811865476;
  const double b=highpass?(1+c)*.5:(1-c)*.5;coefficients(b,highpass?-2*b:2*b,b,1+alpha,-2*c,1-alpha);
 }
 void peak(float fs,float frequency,float q,float db) noexcept {
  const double a=std::pow(10.,db/40.),w=6.283185307179586*std::clamp(frequency,10.f,fs*.45f)/fs,c=std::cos(w),alpha=std::sin(w)/(2*q);
  coefficients(1+alpha*a,-2*c,1-alpha*a,1+alpha/a,-2*c,1-alpha/a);
 }
 void shelf(float fs,float frequency,float db,bool high) noexcept {
  const double a=std::pow(10.,db/40.),w=6.283185307179586*std::clamp(frequency,10.f,fs*.45f)/fs,c=std::cos(w),beta=2*std::sqrt(a)*std::sin(w)*.7071067811865476;
  if(high)coefficients(a*((a+1)+(a-1)*c+beta),-2*a*((a-1)+(a+1)*c),a*((a+1)+(a-1)*c-beta),(a+1)-(a-1)*c+beta,2*((a-1)-(a+1)*c),(a+1)-(a-1)*c-beta);
  else coefficients(a*((a+1)-(a-1)*c+beta),2*a*((a-1)-(a+1)*c),a*((a+1)-(a-1)*c-beta),(a+1)+(a-1)*c+beta,-2*((a-1)+(a+1)*c),(a+1)+(a-1)*c-beta);
 }
 float process(float x) noexcept {const double y=b0_*x+z1_;z1_=b1_*x-a1_*y+z2_;z2_=b2_*x-a2_*y;return (float)y;}
private:double b0_=1,b1_=0,b2_=0,a1_=0,a2_=0,z1_=0,z2_=0;
};
class CutFilters {
public:
 float process(float x,float highpass,float lowpass,float mix,float fs) noexcept {
  if(highpass!=hp_||lowpass!=lp_||fs!=fs_){hp_=highpass;lp_=lowpass;fs_=fs;high_.cut(fs,20.f+980.f*hp_,true);low_.cut(fs,1000.f+19000.f*lp_,false);}
  return (1.f-mix)*x+mix*low_.process(high_.process(x));
 }
private:float hp_=-1,lp_=-1,fs_=-1;Biquad high_,low_;
};
class ToneEQ {
public:
 float process(float x,float low,float mid,float high,float frequency,float q,float mix,float fs) noexcept {
  if(low!=low_||mid!=mid_||high!=high_||frequency!=frequency_||q!=q_||fs!=fs_){low_=low;mid_=mid;high_=high;frequency_=frequency;q_=q;fs_=fs;lowShelf_.shelf(fs,120.f,-12.f+24.f*low,false);midPeak_.peak(fs,100.f+3900.f*frequency,.3f+3.7f*q,-12.f+24.f*mid);highShelf_.shelf(fs,4000.f,-12.f+24.f*high,true);}
  return (1.f-mix)*x+mix*highShelf_.process(midPeak_.process(lowShelf_.process(x)));
 }
private:float low_=-1,mid_=-1,high_=-1,frequency_=-1,q_=-1,fs_=-1;Biquad lowShelf_,midPeak_,highShelf_;
};
class GainUtility {
public:
 float process(float x,float db,float invert,float fs) noexcept {
  if(fs!=fs_){fs_=fs;coefficient_=std::exp(-1.f/(fs*.01f));}
  if(db!=db_||invert!=invert_){db_=db;invert_=invert;target_=std::pow(10.f,(-60.f+72.f*db)/20.f)*(invert>=.5f?-1.f:1.f);}
  gain_=target_+coefficient_*(gain_-target_);return x*gain_;
 }
private:float fs_=-1,db_=-1,invert_=-1,coefficient_=0,target_=1,gain_=1;
};
// Original mono Schroeder-style room. Each channel/slot owns its delay state.
// Buffers are prepared while audio is stopped; all subsequent processing is allocation-free.
class RoomReverb {
public:
 void prepare(float fs){
  fs_=fs;const float combSeconds[]={.0271f,.0337f,.0391f,.0433f},allpassSeconds[]={.0053f,.0017f};
  for(std::size_t i=0;i<4;++i){combs_[i].assign(std::max<std::size_t>(2,(std::size_t)std::round(fs*combSeconds[i])),0.f);combIndex_[i]=0;dampingState_[i]=0;}
  for(std::size_t i=0;i<2;++i){allpasses_[i].assign(std::max<std::size_t>(2,(std::size_t)std::round(fs*allpassSeconds[i])),0.f);allpassIndex_[i]=0;}
  lastTime_=-1;
 }
 float process(float x,float time,float damping,float diffusion) noexcept {
  if(combs_[0].empty())return 0.f;
  if(time!=lastTime_){lastTime_=time;const float rt60=.2f+7.8f*time;for(std::size_t i=0;i<4;++i)feedback_[i]=std::pow(10.f,-3.f*(float)combs_[i].size()/(fs_*rt60));}
  const float damp=.05f+.9f*damping;float wet=0;
  for(std::size_t i=0;i<4;++i){auto& buffer=combs_[i];auto& index=combIndex_[i];const float delayed=buffer[index];dampingState_[i]=(1.f-damp)*delayed+damp*dampingState_[i];buffer[index]=std::clamp(x*.5f+feedback_[i]*dampingState_[i],-4.f,4.f);if(++index==buffer.size())index=0;wet+=delayed*.25f;}
  const float g=.1f+.65f*diffusion;
  for(std::size_t i=0;i<2;++i){auto& buffer=allpasses_[i];auto& index=allpassIndex_[i];const float y=buffer[index]-g*wet;buffer[index]=wet+g*y;if(++index==buffer.size())index=0;wet=y;}
  return wet;
 }
private:
 float fs_=48000,lastTime_=-1;std::array<std::vector<float>,4> combs_;std::array<std::vector<float>,2> allpasses_;
 std::array<std::size_t,4> combIndex_{};std::array<std::size_t,2> allpassIndex_{};
 std::array<float,4> feedback_{},dampingState_{};
};
}
