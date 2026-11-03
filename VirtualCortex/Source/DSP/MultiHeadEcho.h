#pragma once
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
namespace vc {
// Original four-head mono-record/stereo-playback echo prototype.
// Call prepare while stopped; processing/configuration allocate no memory.
// Timing changes must currently be made while stopped (no head crossfade yet).
class MultiHeadEcho {
public:
 struct Head {float milliseconds=100,level=0,pan=0,feedback=0;};
 void prepare(double rate){fs=std::isfinite(rate)?std::clamp(rate,8000.,192000.):48000.;buffer.assign((std::size_t)(fs*4)+2,0);cursor=0;}
 void configure(std::array<Head,4> value) noexcept {
  auto safe=[](float x,float a,float b,float d){return std::isfinite(x)?std::clamp(x,a,b):d;};
  for(auto& h:value){h.milliseconds=safe(h.milliseconds,1,4000,100);h.level=safe(h.level,0,1,0);h.pan=safe(h.pan,-1,1,0);h.feedback=safe(h.feedback,0,.95f,0);}
  heads=value;float sum=0;for(auto& h:heads)sum+=h.feedback;
  feedbackScale=sum>.95f?.95f/sum:1.f;
  for(std::size_t i=0;i<4;++i){distance[i]=heads[i].milliseconds*.001*fs;gainL[i]=std::cos((heads[i].pan+1)*.7853981633974483)*heads[i].level;gainR[i]=std::sin((heads[i].pan+1)*.7853981633974483)*heads[i].level;}
 }
 // Wet-only output. Feedback level is independent of playback level/pan.
 std::array<float,2> process(float input) noexcept {
  if(buffer.empty())return {};
  if(!std::isfinite(input))input=0;
  std::array<float,2> result{};float feedback=0;
  for(std::size_t i=0;i<4;++i){const double d=std::clamp(distance[i],1.,(double)buffer.size()-2);const auto n=(std::size_t)d;const float f=(float)(d-n);
   const float a=buffer[(cursor+buffer.size()-n)%buffer.size()],b=buffer[(cursor+buffer.size()-n-1)%buffer.size()];const float x=a+(b-a)*f;
   result[0]+=x*gainL[i];result[1]+=x*gainR[i];feedback+=x*heads[i].feedback;
  }
  buffer[cursor]=std::clamp(input+feedback*feedbackScale,-16.f,16.f);if(++cursor==buffer.size())cursor=0;
  return result;
 }
private:
 double fs=48000;std::vector<float> buffer;std::size_t cursor=0;
 std::array<Head,4> heads{};std::array<double,4> distance{{4800,4800,4800,4800}};
 std::array<float,4> gainL{},gainR{};float feedbackScale=1;
};
}
