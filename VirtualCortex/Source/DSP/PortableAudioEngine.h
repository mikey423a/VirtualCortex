#pragma once
#include "ParallelGrid.h"
#include "OriginalEffects.h"
#include "ModulationEffects.h"
#include "NeuralAmp.h"
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>
#include <memory>
#include <algorithm>

namespace vc {
// Independently stateful portable audio renderer for the four-row grid.
// Configure and prepare OFF the audio thread. process() allocates nothing.
// This is an original, basic DSP engine; it does not implement NAM or QC models.
// Per-position controls are normalized to [0,1]. Configure from the control
// thread; never mutate them concurrently with process().
struct SlotParameters {
 float drive=0.5f, mix=0.2f, time=0.5f, feedback=0.35f, rate=0.5f, depth=0.5f;
 static bool valid(const SlotParameters& p) noexcept {
  const float values[]={p.drive,p.mix,p.time,p.feedback,p.rate,p.depth};
  for (float v:values) if (!std::isfinite(v) || v<0.f || v>1.f) return false;
  return true;
 }
};
class PortableAudioEngine {
public:
 PortableAudioEngine() : irs_(std::make_unique<IRStorage>()), state_(std::make_unique<StateStorage>()), neural_(std::make_unique<NeuralStorage>()) {}
 static constexpr std::size_t maxIrTaps = 4096;
 // May be called at a block boundary by the audio callback. Coefficients are
 // copied into fixed-size storage; both old and new FIRs run for 128 samples.
 // Never call concurrently with process(); use the pending update queue.
 // An update arriving mid-transition starts from the currently selected FIR.
 static constexpr std::size_t irFadeSamples = 128;
 static constexpr std::size_t neuralFadeSamples = 128;
 bool setCabinetIR(std::size_t row,std::size_t col,const float* taps,std::size_t count) noexcept {
  if (!SignalGrid::valid(row,col) || count>maxIrTaps || (count && !taps)) return false;
  for(std::size_t i=0;i<count;++i) if(!std::isfinite(taps[i])) return false;
  auto& ir=(*irs_)[row][col];
  // Re-sending the selected IR must not restart a crossfade or overwrite
  // the previous FIR during an in-flight transition.
  if (ir.current.count == count &&
      (count == 0 || std::equal(taps, taps + count, ir.current.taps.begin()))) return true;
  ir.previous=ir.current;
  ir.current.count=count;
  for(std::size_t i=0;i<count;++i) ir.current.taps[i]=taps[i];
  ir.fade=0;
  // Fade both when adding the first IR and when replacing/removing one.
  ir.transition=(ir.previous.count!=0 || ir.current.count!=0);
  return true;
 }
 std::size_t cabinetIRLength(std::size_t row,std::size_t col) const noexcept {
  return SignalGrid::valid(row,col) ? (*irs_)[row][col].current.count : 0;
 }

public:
 // Call only while processing is stopped. A loaded model replaces the basic
 // tanh amplifier for the specified Amp slot; nullptr restores the basic amp.
 bool setNeuralAmp(std::size_t row,std::size_t col,const NeuralAmpModel* model) noexcept {
  if(!SignalGrid::valid(row,col) || (model && !model->valid())) return false;
  auto& slot=(*neural_)[row][col];
  slot.previousModel=slot.model;
  slot.previousEnabled=slot.enabled;
  if(model) slot.model=*model;
  slot.enabled=(model!=nullptr);
  slot.transition=hasProcessed_;
  slot.fade=0;
  for(auto& channel:(*state_)[row][col]) {
   channel.previousNeuralHistory=channel.neuralHistory;
   channel.previousNeuralIndex=channel.neuralIndex;
   channel.neuralHistory.fill(0.f);channel.neuralIndex=0;
  }
  return true;
 }
 bool neuralAmpEnabled(std::size_t row,std::size_t col) const noexcept {
  return SignalGrid::valid(row,col) && (*neural_)[row][col].enabled;
 }
 bool configure(const SignalGrid& grid, const std::array<bool,4>& enabled) noexcept {
  return routing_.configure(grid,enabled);
 }
 bool setSlotParameters(std::size_t row,std::size_t col,const SlotParameters& p) noexcept {
  if (!SignalGrid::valid(row,col) || !SlotParameters::valid(p)) return false;
  parameters_[row][col]=p;
  return true;
 }
 const SlotParameters* getSlotParameters(std::size_t row,std::size_t col) const noexcept {
  return SignalGrid::valid(row,col) ? &parameters_[row][col] : nullptr;
 }
 void prepare(double sampleRate, std::size_t maxDelaySamples = 0) {
  hasProcessed_=false;
  // A device restart or sample-rate change must not resume an unfinished
  // transition from the previous stream. Retain the selected model/IR but
  // discard obsolete crossfade state and previous FIR coefficients.
  for (auto& lane:*irs_) for (auto& ir:lane) {
   ir.transition=false;
   ir.fade=0;
   ir.previous.count=0;
  }
  for (auto& lane:*neural_) for (auto& slot:lane) {
   slot.transition=false;
   slot.fade=0;
   slot.previousEnabled=false;
  }
  rate_ = std::isfinite(sampleRate) ? std::clamp(sampleRate,8000.0,192000.0) : 48000.0;
  const auto n = maxDelaySamples ? maxDelaySamples : static_cast<std::size_t>(rate_ * 0.6);
  for (auto& lane: *state_) for (auto& slot: lane) for (auto& ch: slot) {
   ch = Channel{};
   ch.delay.assign(std::max<std::size_t>(n,2),0.f);
  }
 }
 // Interleaved audio. Channels 1 or 2, prepared before calling process.
 bool process(float* samples, std::size_t frames, std::size_t channels) noexcept {
  if (!samples || (channels != 1 && channels != 2) || !ready()) return false;
  for (std::size_t i=0;i<frames;++i) {
   for(std::size_t c=0;c<channels;++c) {
   auto& sample=samples[i*channels+c];
   // Sanitize before stateful DSP so NaN/Inf cannot poison filter histories.
   if (!std::isfinite(sample)) sample=0.f;
   sample=routing_.processSampleIndexed(sample,[&](std::size_t row,std::size_t col,BlockType block,float x) noexcept {
    return apply((*state_)[row][col][c],parameters_[row][col],(*irs_)[row][col],(*neural_)[row][col],block,x);
   });
   if (!std::isfinite(sample)) sample=0.f;
   sample=std::clamp(sample,-1.f,1.f);
   }
   for (auto& lane:*neural_) for (auto& slot:lane) if(slot.transition) {
    if(++slot.fade>=neuralFadeSamples) slot.transition=false;
   }
   for (auto& lane:*irs_) for (auto& ir:lane) if (ir.transition) {
    if (++ir.fade >= irFadeSamples) { ir.transition=false; ir.previous.count=0; }
   }
  }
  if(frames) hasProcessed_=true;
  return true;
 }
private:
 struct FIR { std::array<float,maxIrTaps> taps{}; std::size_t count=0; };
 struct CabinetIR { FIR current{}, previous{}; std::size_t fade=0; bool transition=false; };
 struct Channel {
  SixStagePhaser phaser;
  EnvelopeWah wah;
  BitCrusher crusher;
  PeakCompressor compressor;
  ClippingDrive clippingDrive;
  std::array<float,maxIrTaps> irHistory{};
  std::array<float,NeuralAmpModel::inputs> neuralHistory{}, previousNeuralHistory{};
  std::size_t previousNeuralIndex=0;
  std::size_t neuralIndex=0;
  std::size_t irPosition=0;
  float gate=0.f, cab=0.f, eq=0.f, reverb=0.f, phase=0.f;
  std::vector<float> delay;
  std::size_t index=0;
 };
 struct NeuralSlot { NeuralAmpModel model{}, previousModel{}; bool enabled=false,previousEnabled=false,transition=false; std::size_t fade=0; };
 bool ready() const noexcept { return !(*state_)[0][0][0].delay.empty(); }
 float apply(Channel& s,const SlotParameters& p,const CabinetIR& ir,const NeuralSlot& neural,BlockType b,float x) noexcept {
  const float pi=3.14159265358979323846f;
  const float fs=static_cast<float>(rate_);
  switch(b) {
   case BlockType::Gate: {
    s.gate=0.995f*s.gate+0.005f*std::abs(x);
    return s.gate<0.006f ? x*0.05f : x;
   }
   case BlockType::Amp: {
    // Only evaluate the fallback amp if it contributes to this sample.
    // Previously basic() was evaluated unconditionally, even when the neural
    // model completely replaced it. That incurred unnecessary tanh calls.
    const auto basic=[&]() noexcept {
     const float drive=1.f+15.f*p.drive;
     return std::tanh(drive*x)/std::tanh(drive);
    };
    float newer=0.f;
    if(neural.enabled) {
     s.neuralHistory[s.neuralIndex]=x;
     newer=neural.model.infer(s.neuralHistory,s.neuralIndex);
     s.neuralIndex=(s.neuralIndex+1)%NeuralAmpModel::inputs;
    } else {
     newer=basic();
    }
    if(!neural.transition) return newer;
    float older=0.f;
    if(neural.previousEnabled) {
     s.previousNeuralHistory[s.previousNeuralIndex]=x;
     older=neural.previousModel.infer(s.previousNeuralHistory,s.previousNeuralIndex);
     s.previousNeuralIndex=(s.previousNeuralIndex+1)%NeuralAmpModel::inputs;
    } else {
     older=basic();
    }
    const float t=static_cast<float>(neural.fade+1)/static_cast<float>(neuralFadeSamples);
    return older*(1.f-t)+newer*t;
   }
   case BlockType::Cabinet: {
    // Shared input history means both FIRs receive exactly the same samples.
    s.irHistory[s.irPosition]=x;
    const auto convolve=[&](const FIR& fir) noexcept {
     if (!fir.count) {
      // The fallback cab is a one-pole filter, whose state is advanced below.
      return s.cab;
     }
     float y=0.f;
     std::size_t read=s.irPosition;
     for(std::size_t i=0;i<fir.count;++i) {
      y+=fir.taps[i]*s.irHistory[read];
      read=read ? read-1 : maxIrTaps-1;
     }
     return std::isfinite(y) ? y : 0.f;
    };
    const float a=std::exp(-2.f*pi*4200.f/fs);
    s.cab=(1.f-a)*x+a*s.cab;
    const float newer=convolve(ir.current);
    const float older=ir.transition ? convolve(ir.previous) : newer;
    s.irPosition=(s.irPosition+1)%maxIrTaps;
    if (!ir.transition) return newer;
    const float t=static_cast<float>(ir.fade+1)/static_cast<float>(irFadeSamples);
    return older*(1.f-t)+newer*t;
   }
   case BlockType::EQ: {
    const float a=std::exp(-2.f*pi*700.f/fs);
    s.eq=(1.f-a)*x+a*s.eq;
    return 0.8f*x+0.2f*s.eq;
   }
   case BlockType::Delay: {
    const std::size_t n=s.delay.size();
    const std::size_t distance=std::min(n-1,std::max<std::size_t>(1,static_cast<std::size_t>(fs*(0.02f+0.48f*p.time))));
    const std::size_t read=(s.index+n-distance)%n;
    const float delayed=s.delay[read];
    s.delay[s.index]=std::clamp(x+delayed*(0.9f*p.feedback),-2.f,2.f);
    s.index=(s.index+1)%n;
    return (1.f-p.mix)*x+p.mix*delayed;
   }
   case BlockType::Reverb: {
    const float prior=s.reverb;
    s.reverb=0.86f*s.reverb+0.14f*x;
    return (1.f-p.mix)*x+p.mix*prior;
   }
   case BlockType::Tremolo: {
    const float depth=p.depth;
    const float gain=1.f-depth*0.5f+depth*0.5f*std::sin(s.phase);
    s.phase+=2.f*pi*(0.1f+9.9f*p.rate)/fs;
    if(s.phase>=2.f*pi)s.phase-=2.f*pi;
    return x*gain;
   }
   case BlockType::Compressor:return s.compressor.process(x,p.drive,p.feedback,p.rate,p.depth,p.time,p.mix,fs);
   case BlockType::Overdrive:case BlockType::Distortion:return s.clippingDrive.process(x,p.drive,p.rate,p.depth,p.mix,fs,b==BlockType::Distortion);
   case BlockType::Chorus:case BlockType::Flanger:return modulatedDelay(x,s.delay,s.index,s.phase,p.mix,p.time,p.feedback,p.rate,p.depth,fs,b==BlockType::Flanger);
   case BlockType::Phaser:return s.phaser.process(x,p.mix,p.time,p.feedback,p.rate,p.depth,fs);
   case BlockType::Wah:return s.wah.process(x,p.drive,p.mix,p.time,p.feedback,p.rate,p.depth,fs);
   case BlockType::BitCrusher:return s.crusher.process(x,p.drive,p.mix,p.time,p.depth);
   default: return x;
  }
 }
 ParallelGrid routing_;
 bool hasProcessed_=false;
 double rate_=48000.0;
 std::array<std::array<SlotParameters,SignalGrid::columns>,SignalGrid::rows> parameters_{};
 using IRStorage = std::array<std::array<CabinetIR,SignalGrid::columns>,SignalGrid::rows>;
 std::unique_ptr<IRStorage> irs_;
 using NeuralStorage = std::array<std::array<NeuralSlot,SignalGrid::columns>,SignalGrid::rows>;
 std::unique_ptr<NeuralStorage> neural_;
 using StateStorage = std::array<std::array<std::array<Channel,2>,SignalGrid::columns>,SignalGrid::rows>;
 std::unique_ptr<StateStorage> state_;
};
}
