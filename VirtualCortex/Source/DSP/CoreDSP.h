#pragma once
#include <cmath>
#include <algorithm>
namespace vc {
// No allocation, no JUCE dependency. Sample-by-sample high-pass filter.
class HighPassOnePole {
public:
 void prepare(double sampleRate) noexcept { fs = std::max(8000.0, sampleRate); reset(); }
 void setCutoff(float hz) noexcept { const auto f = std::clamp((double)hz, 1.0, fs * 0.45); alpha = (float)std::exp(-2.0 * 3.14159265358979323846 * f / fs); }
 float process(float x) noexcept { const float y = alpha * (previousOutput + x - previousInput); previousInput = x; previousOutput = std::isfinite(y) ? y : 0.0f; return previousOutput; }
 void reset() noexcept { previousInput = previousOutput = 0.0f; }
private:
 double fs = 48000.0;
 float alpha = 0.99f, previousInput = 0.0f, previousOutput = 0.0f;
};
// One-pole smoothing for click-reduced gain automation. No allocations in process().
class SmoothedGain {
public:
 void prepare(double sampleRate, float milliseconds = 10.f) noexcept {
  const double fs = std::max(8000.0, sampleRate);
  const double seconds = std::max(0.001, static_cast<double>(milliseconds) * 0.001);
  coefficient = static_cast<float>(std::exp(-1.0 / (fs * seconds)));
 }
 void reset(float initial) noexcept { current = std::isfinite(initial) ? initial : 1.f; }
 float process(float target) noexcept {
  if (!std::isfinite(target)) target = current;
  current = coefficient * current + (1.f - coefficient) * target;
  return current;
 }
 float value() const noexcept { return current; }
private:
 float current = 1.f, coefficient = 0.998f;
};
inline float saturate(float input, float gain, float bias) noexcept {
 const float x = std::tanh(input * gain + bias) - std::tanh(bias);
 return std::isfinite(x) ? x : 0.0f;
}
}
