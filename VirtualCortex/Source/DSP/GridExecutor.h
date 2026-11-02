#pragma once
#include "GridPlan.h"
#include <array>
#include <cstddef>
#include <cmath>

namespace vc {
// Portable offline executor. Each callback implements one block's DSP for a
// single sample. The host must supply distinct state for every channel/instance.
// No allocations, locks or exceptions on the sample-processing path.
class GridExecutor {
public:
 bool configure(const GridPlan& plan) noexcept {
  if (plan.count > SignalGrid::columns) return false;
  for (std::size_t i=0; i<plan.count; ++i)
   if (!SignalGrid::validType(plan.blocks[i]) || plan.blocks[i]==BlockType::Empty) return false;
  for (std::size_t i=0;i<plan.count;++i)
   if (plan.columns[i] >= SignalGrid::columns) return false;
  plan_ = plan;
  return true;
 }
 template<class DSP>
 float processSample(float input, DSP&& dsp) const noexcept {
  float value = std::isfinite(input) ? input : 0.f;
  for (std::size_t i=0; i<plan_.count; ++i) {
   value = dsp(plan_.blocks[i], value);
   if (!std::isfinite(value)) return 0.f;
  }
  return value;
 }
 template<class DSP>
 void processBuffer(float* samples, std::size_t count, DSP&& dsp) const noexcept {
  if (!samples) return;
  for (std::size_t i=0; i<count; ++i)
   samples[i]=processSample(samples[i], dsp);
 }
 const GridPlan& plan() const noexcept { return plan_; }
private:
 GridPlan plan_{};
};
}
