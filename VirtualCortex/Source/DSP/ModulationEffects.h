#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>
namespace vc {
// Original fractional-delay modulation. Storage is allocated by prepare(), never process().
inline float modulatedDelay(float x,std::vector<float>& buffer,std::size_t& index,float& phase,float mix,float time,float feedback,float rate,float depth,float fs,bool flanger) noexcept {
 const float lfo=.5f+.5f*std::sin(phase);
 const float milliseconds=flanger?(.2f+5.f*time+3.f*depth*lfo):(5.f+20.f*time+10.f*depth*lfo);
 const float distance=std::clamp(fs*.001f*milliseconds,1.f,(float)(buffer.size()-1));
 const auto whole=(std::size_t)distance;const float fraction=distance-(float)whole;
 const auto read=(index+buffer.size()-whole)%buffer.size(),older=(read+buffer.size()-1)%buffer.size();
 const float delayed=buffer[read]*(1.f-fraction)+buffer[older]*fraction;
 buffer[index]=std::clamp(x+feedback*(flanger?.85f:.7f)*delayed,-4.f,4.f);index=(index+1)%buffer.size();
 phase+=6.283185307f*(flanger?(.05f+4.95f*rate):(.1f+9.9f*rate))/fs;
 if(phase>=6.283185307f)phase-=6.283185307f;
 return (1.f-mix)*x+mix*delayed;
}
class SixStagePhaser {
public:
 float process(float x,float mix,float time,float feedback,float rate,float depth,float fs) noexcept {
  const float cutoff=std::clamp((150.f+1350.f*time)*std::pow(2.f,2.f*depth*std::sin(phase_)),40.f,std::min(12000.f,fs*.4f));
  const float g=std::tan(3.141592654f*cutoff/fs),a=(1.f-g)/(1.f+g);
  float wet=std::clamp(x+lastOutput_*feedback*.85f,-4.f,4.f);
  for(auto& state:state_){const float y=-a*wet+state;state=wet+a*y;wet=y;}
  lastOutput_=wet;phase_+=6.283185307f*(.05f+4.95f*rate)/fs;if(phase_>=6.283185307f)phase_-=6.283185307f;
  return (1.f-mix)*x+mix*wet;
 }
private:std::array<float,6> state_{};float lastOutput_=0,phase_=0;
};
class EnvelopeWah {
public:
 float process(float x,float sensitivity,float mix,float position,float resonance,float attack,float release,float fs) noexcept {
  if(attack!=attack_||release!=release_||fs!=fs_){attack_=attack;release_=release;fs_=fs;attackCoefficient_=std::exp(-1.f/(fs*(.002f+.098f*attack)));releaseCoefficient_=std::exp(-1.f/(fs*(.05f+.95f*release)));}
  const float magnitude=std::abs(x),coefficient=magnitude>envelope_?attackCoefficient_:releaseCoefficient_;
  envelope_=coefficient*envelope_+(1.f-coefficient)*magnitude;
  const float sweep=std::clamp(position+envelope_*sensitivity*4.f,0.f,1.f);
  const float cutoff=std::min(300.f*std::pow(10.f,sweep),fs*.35f);
  const float g=std::tan(3.141592654f*cutoff/fs),k=1.f/(.5f+4.5f*resonance);
  const float v3=x-ic2_,v1=(ic1_+g*v3)/(1.f+g*(g+k)),v2=ic2_+g*v1;
  ic1_=2.f*v1-ic1_;ic2_=2.f*v2-ic2_;
  return (1.f-mix)*x+mix*k*v1;
 }
private:float attack_=-1,release_=-1,fs_=-1,attackCoefficient_=0,releaseCoefficient_=0,envelope_=0,ic1_=0,ic2_=0;
};
class BitCrusher {
public:
 float process(float x,float bits,float mix,float hold,float level) noexcept {
  const int period=1+(int)std::round(63.f*hold);
  if(counter_==0){const int bitCount=4+(int)std::round(12.f*bits);const float levels=(float)((1<<(bitCount-1))-1);held_=std::round(std::clamp(x,-1.f,1.f)*levels)/levels;}
  if(++counter_>=period)counter_=0;
  return (1.f-mix)*x+mix*held_*level*2.f;
 }
private:int counter_=0;float held_=0;
};
}
