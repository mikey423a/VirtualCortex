#pragma once
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
namespace vc {
// Original four-head mono-record/stereo-playback echo prototype.
// Call prepare while stopped; processing/configuration allocate no memory.
// Times crossfade between fixed heads over 20 ms; gain/send changes smooth over 10 ms.
class MultiHeadEcho {
public:
 struct Head {float milliseconds=100,level=0,pan=0,feedback=0;};
 void prepare(double rate){fs=std::isfinite(rate)?std::clamp(rate,8000.,192000.):48000.;buffer.assign((std::size_t)(fs*4)+2,0);cursor=0;initialized=false;fade.fill(0);levelL={};levelR={};send={};}
 void configure(std::array<Head,4> value) noexcept {
  auto safe=[](float x,float a,float b,float d){return std::isfinite(x)?std::clamp(x,a,b):d;};
  for(auto& h:value){h.milliseconds=safe(h.milliseconds,1,4000,100);h.level=safe(h.level,0,1,0);h.pan=safe(h.pan,-1,1,0);h.feedback=safe(h.feedback,0,.95f,0);}
  heads=value;float sum=0;for(auto& h:heads)sum+=h.feedback;
  feedbackScale=sum>.95f?.95f/sum:1.f;
  for(std::size_t i=0;i<4;++i){target[i]=heads[i].milliseconds*.001*fs;if(!initialized)distance[i]=target[i];gainL[i]=std::cos((heads[i].pan+1)*.7853981633974483)*heads[i].level;gainR[i]=std::sin((heads[i].pan+1)*.7853981633974483)*heads[i].level;}
  initialized=true;
 }
 // Wet-only output. Feedback level is independent of playback level/pan.
 std::array<float,2> process(float input) noexcept {
  if(buffer.empty())return {};
  if(!std::isfinite(input))input=0;
  std::array<float,2> result{};float feedback=0;
  for(std::size_t i=0;i<4;++i){if(!fade[i]&&std::abs(distance[i]-target[i])>.01){next[i]=target[i];fade[i]=1;}
   auto read=[&](double at){const double d=std::clamp(at,1.,(double)buffer.size()-2);const auto n=(std::size_t)d;const float f=(float)(d-n);
   const float a=buffer[(cursor+buffer.size()-n)%buffer.size()],b=buffer[(cursor+buffer.size()-n-1)%buffer.size()];return a+(b-a)*f;};
   float x=read(distance[i]);if(fade[i]){const auto length=(std::size_t)(fs*.02);const float t=(float)fade[i]/length;x=x*(1-t)+read(next[i])*t;if(++fade[i]>length){distance[i]=next[i];fade[i]=0;}}
   const float c=(float)std::exp(-1./(fs*.01));auto move=[&](float& v,float t){v=t+c*(v-t);};move(levelL[i],gainL[i]);move(levelR[i],gainR[i]);move(send[i],heads[i].feedback*feedbackScale);
   result[0]+=x*levelL[i];result[1]+=x*levelR[i];feedback+=x*send[i];
  }
  buffer[cursor]=std::clamp(input+feedback,-16.f,16.f);if(++cursor==buffer.size())cursor=0;
  return result;
 }
private:
 double fs=48000;std::vector<float> buffer;std::size_t cursor=0;
 std::array<Head,4> heads{};std::array<double,4> distance{{4800,4800,4800,4800}};
 std::array<float,4> gainL{},gainR{};float feedbackScale=1;bool initialized=false;std::array<double,4> target{},next{};std::array<std::size_t,4> fade{};std::array<float,4> levelL{},levelR{},send{};
};
}
