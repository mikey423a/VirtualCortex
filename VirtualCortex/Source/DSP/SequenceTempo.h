#pragma once
#include <algorithm>
#include <cmath>
namespace vc {
// Tempo follows BPM only; does not reset phase or follow transport position.
inline float sequenceStepMilliseconds(float manual,float bpm,float beats,bool sync) noexcept {
 if(!sync)return manual;
 const float tempo=std::isfinite(bpm)&&bpm>0?std::clamp(bpm,20.f,300.f):120.f;
 const float division=std::isfinite(beats)?std::clamp(beats,.125f,4.f):1.f;
 return 60000.f/tempo*division;
}
}
