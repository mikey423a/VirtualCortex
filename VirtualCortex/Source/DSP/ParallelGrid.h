#pragma once
#include "GridExecutor.h"
#include "RowRouting.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <cstddef>

namespace vc {
// Four independent parallel lanes. Each enabled lane receives the same input.
// Caller owns per-lane/per-channel DSP state and provides a noexcept callback.
// Compile/configure on the control thread; publish at an audio-safe boundary.
class ParallelGrid {
public:
 bool configure(const SignalGrid& grid, const std::array<bool, SignalGrid::rows>& enabled) noexcept {
  std::array<GridExecutor, SignalGrid::rows> next{};
  for (std::size_t row=0; row<SignalGrid::rows; ++row) {
   if (!enabled[row]) continue;
   GridPlan plan;
   if (!compileRow(grid,row,plan) || !next[row].configure(plan)) return false;
  }
  lanes_=next;
  enabled_=enabled;
  return true;
 }
 template<class DSP>
 float processSample(float input, DSP&& dsp) const noexcept {
  const float dry=std::isfinite(input)?input:0.f;
  float sum=0.f;
  std::size_t active=0;
  for (std::size_t row=0; row<SignalGrid::rows; ++row) {
   if (!enabled_[row]) continue;
   ++active;
   float x=dry;
   const auto& plan=lanes_[row].plan();
   for (std::size_t i=0; i<plan.count; ++i) {
    x=dsp(row,plan.blocks[i],x);
    if (!std::isfinite(x)) { x=0.f; break; }
   }
   sum+=x;
  }
  if (active==0) return dry;
  const float result=sum/static_cast<float>(active);
  return std::isfinite(result)?result:0.f;
 }
 // Callback receives the physical grid column, allowing truly independent
 // DSP state for repeated effect types in the same row.
 template<class DSP>
 float processSampleIndexed(float input, DSP&& dsp) const noexcept {
  const float dry=std::isfinite(input)?input:0.f;
  float sum=0.f;
  std::size_t active=0;
  for (std::size_t row=0;row<SignalGrid::rows;++row) {
   if (!enabled_[row]) continue;
   ++active;
   float x=dry;
   const auto& plan=lanes_[row].plan();
   for (std::size_t i=0;i<plan.count;++i) {
    x=dsp(row,static_cast<std::size_t>(plan.columns[i]),plan.blocks[i],x);
    if (!std::isfinite(x)) {x=0.f;break;}
   }
   sum+=x;
  }
  return active==0?dry:(std::isfinite(sum)?sum/static_cast<float>(active):0.f);
 }
 enum class Mode { Parallel, Series, DualMono, Custom };
 bool setRowRouting(const RowRouting& c)noexcept {
  if(!RowRouting::valid(c))return false;custom_=c;
  for(int r=0;r<4;++r){const auto& p=c.rows[r];const float gain=std::pow(10.f,p.gainDb/20.f)*(p.invert?-1.f:1.f);
   targets_[r]={gain*(p.balance>0?1-p.balance:1),gain*(p.balance<0?1+p.balance:1)};
  }return true;
 }
 void prepareMixer(double rate)noexcept {mixerCoefficient_=std::exp(-1./((std::isfinite(rate)?std::clamp(rate,8000.,192000.):48000.)*.01));gains_=targets_;}
 template<class DSP>
 std::array<float,2> processCustom(std::array<float,2> input,DSP&& dsp)noexcept {
  for(auto& x:input)if(!std::isfinite(x))x=0;
  std::array<std::array<float,2>,4> bus{};std::array<double,2> sum{};unsigned outputs=0;
  for(unsigned r=0;r<4;++r){
   for(int ch=0;ch<2;++ch){const float t=targets_[r][ch];gains_[r][ch]=(float)(t+mixerCoefficient_*(gains_[r][ch]-t));if(std::abs(gains_[r][ch]-t)<1.e-7f)gains_[r][ch]=t;}
   if(!enabled_[r])continue;const auto& route=custom_.rows[r];std::array<double,2> incoming{};unsigned count=0;
   if(route.sources&1u){incoming={input[0],input[1]};++count;}
   for(unsigned from=0;from<r;++from)if((route.sources&(2u<<from))&&enabled_[from]){for(int ch=0;ch<2;++ch)incoming[ch]+=bus[from][ch];++count;}
   std::array<float,2> frame{};for(int ch=0;ch<2;++ch)frame[ch]=(float)(incoming[ch]/(custom_.average&&count?count:1));
   if(route.channel){const float mono=route.channel==1?frame[0]:route.channel==2?frame[1]:(frame[0]+frame[1])*.5f;frame={mono,mono};}
   const auto& plan=lanes_[r].plan();for(std::size_t i=0;i<plan.count;++i){frame=dsp(r,(std::size_t)plan.columns[i],plan.blocks[i],frame);for(auto& x:frame)if(!std::isfinite(x))x=0;}
   for(int ch=0;ch<2;++ch){bus[r][ch]=frame[ch]*gains_[r][ch];if(!std::isfinite(bus[r][ch]))bus[r][ch]=0;}
   if(route.output){++outputs;for(int ch=0;ch<2;++ch)sum[ch]+=bus[r][ch];}
  }
  std::array<float,2> out{};for(int ch=0;ch<2;++ch){out[ch]=(float)(sum[ch]/(custom_.average&&outputs?outputs:1));if(!std::isfinite(out[ch]))out[ch]=0;}return out;
 }
 // DualMono: input/output L uses rows 1->2; R uses rows 3->4.
 // Callback carries channel and physical slot; no signal crosses chains.
 template<class DSP>
 std::array<float,2> processFrame(std::array<float,2> input,Mode mode,DSP&& dsp) noexcept {
  if(mode==Mode::Custom)return processCustom(input,[&](std::size_t r,std::size_t c,BlockType b,std::array<float,2>x){return std::array<float,2>{dsp(0,r,c,b,x[0]),dsp(1,r,c,b,x[1])};});
  for(auto& x:input)if(!std::isfinite(x))x=0.f;
  std::array<float,2> output{};
  for(std::size_t channel=0;channel<2;++channel){
   if(mode==Mode::Parallel){
    output[channel]=processSampleIndexed(input[channel],[&](std::size_t r,std::size_t c,BlockType b,float x) noexcept {return dsp(channel,r,c,b,x);});
    continue;
   }
   float x=input[channel];
   const std::size_t first=mode==Mode::DualMono?channel*2:0;
   const std::size_t end=mode==Mode::DualMono?first+2:4;
   for(std::size_t row=first;row<end;++row){
    if(!enabled_[row])continue;
    const auto& plan=lanes_[row].plan();
    for(std::size_t i=0;i<plan.count;++i){
     x=dsp(channel,row,plan.columns[i],plan.blocks[i],x);
     if(!std::isfinite(x)){x=0.f;break;}
    }
   }
   output[channel]=x;
  }
  return output;
 }
 // Stereo devices are invoked once per frame, not once per channel.
 // DualMono deliberately remains on processFrame until each device has an
 // explicit mono policy; calling this method in that mode is dry passthrough.
 template<class DSP>
 std::array<float,2> processStereoFrame(std::array<float,2> input,Mode mode,DSP&& dsp) noexcept {
  if(mode==Mode::Custom)return processCustom(input,dsp);
  for(auto& x:input)if(!std::isfinite(x))x=0.f;
  if(mode==Mode::DualMono)return input;
  auto output=input;std::array<double,2> sum{};std::size_t active=0;
  for(std::size_t row=0;row<SignalGrid::rows;++row){
   if(!enabled_[row])continue;
   ++active;auto frame=mode==Mode::Series?output:input;
   const auto& plan=lanes_[row].plan();
   for(std::size_t i=0;i<plan.count;++i){
    frame=dsp(row,static_cast<std::size_t>(plan.columns[i]),plan.blocks[i],frame);
    for(auto& x:frame)if(!std::isfinite(x))x=0.f;
   }
   if(mode==Mode::Series)output=frame;
   else for(std::size_t c=0;c<2;++c)sum[c]+=frame[c];
  }
  if(mode==Mode::Parallel&&active)for(std::size_t c=0;c<2;++c)output[c]=static_cast<float>(sum[c]/active);
  for(auto& x:output)if(!std::isfinite(x))x=0.f;
  return output;
 }
 template<class DSP>
 void processBuffer(float* data,std::size_t count,DSP&& dsp) const noexcept {
  if (!data) return;
  for (std::size_t i=0;i<count;++i) data[i]=processSample(data[i],dsp);
 }
 bool laneEnabled(std::size_t row) const noexcept { return row<SignalGrid::rows && enabled_[row]; }
private:
 RowRouting custom_{};
 std::array<std::array<float,2>,4> targets_{{{1,1},{1,1},{1,1},{1,1}}},gains_=targets_;
 double mixerCoefficient_=0;
 std::array<GridExecutor, SignalGrid::rows> lanes_{};
 std::array<bool, SignalGrid::rows> enabled_{};
};
}
