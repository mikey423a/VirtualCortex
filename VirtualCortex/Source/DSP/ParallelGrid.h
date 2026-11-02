#pragma once
#include "GridExecutor.h"
#include <array>
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
 template<class DSP>
 void processBuffer(float* data,std::size_t count,DSP&& dsp) const noexcept {
  if (!data) return;
  for (std::size_t i=0;i<count;++i) data[i]=processSample(data[i],dsp);
 }
 bool laneEnabled(std::size_t row) const noexcept { return row<SignalGrid::rows && enabled_[row]; }
private:
 std::array<GridExecutor, SignalGrid::rows> lanes_{};
 std::array<bool, SignalGrid::rows> enabled_{};
};
}
