#include "LiveAmp.h"
#include <NAM/get_dsp.h>
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <stdexcept>
namespace vc {
namespace {
class NamAmp final:public LiveAmp {
 std::array<std::unique_ptr<nam::DSP>,2> models;
 std::atomic<LiveAmpStatus> state{LiveAmpStatus::None};
 double expected=0;
 float inputTarget=1,outputTarget=1,coefficient=0;
 std::array<float,2> inputGain{{1,1}},outputGain{{1,1}};
public:
 NamAmp(const std::string& path,double rate,bool allowRateMismatch){
  const auto file=std::filesystem::u8path(path);
  if(std::filesystem::file_size(file)>64u*1024u*1024u)throw std::runtime_error("NAM model exceeds 64 MiB");
  for(auto& model:models){
   nam::DspLoadOptions options;options.prewarm=false;model=nam::get_dsp(file,options);
   if(!model||model->NumInputChannels()!=1||model->NumOutputChannels()!=1)throw std::runtime_error("NAM needs a mono-input, mono-output model");
   const double sr=model->GetExpectedSampleRate();if(!std::isfinite(sr))throw std::runtime_error("NAM model has invalid sample rate");
   expected=sr>0?sr:0;
  }
  prepare(rate);
  if(status()==LiveAmpStatus::RateMismatch&&!allowRateMismatch)throw std::runtime_error("NAM model expects "+std::to_string((int)expected)+" Hz; set the audio device/DAW to that sample rate");
  if(status()!=LiveAmpStatus::Ready&&status()!=LiveAmpStatus::RateMismatch)throw std::runtime_error("NAM model could not be prepared");
 }
 void prepare(double rate)noexcept override {
  if(!std::isfinite(rate)||rate<8000||rate>192000||(expected>0&&std::abs(rate-expected)>.5)){state=LiveAmpStatus::RateMismatch;return;}
  try{
   // Processing one sample adds no block-adapter latency. All model Reset /
   // prewarm allocations occur here, never in the normal processing callback.
   for(auto& model:models){model->SetPrewarmOnReset(true);model->Reset(rate,1);}
   coefficient=(float)std::exp(-1./(rate*.01));inputGain.fill(inputTarget);outputGain.fill(outputTarget);state=LiveAmpStatus::Ready;
  }catch(...){state=LiveAmpStatus::Fault;}
 }
 void setGains(float in,float out)noexcept override {
  if(std::isfinite(in))inputTarget=std::pow(10.f,std::clamp(in,-24.f,24.f)/20.f);
  if(std::isfinite(out))outputTarget=std::pow(10.f,std::clamp(out,-24.f,24.f)/20.f);
 }
 float process(std::size_t channel,float input)noexcept override {
  if(channel>=2||state.load(std::memory_order_relaxed)!=LiveAmpStatus::Ready)return 0;
  inputGain[channel]=inputTarget+coefficient*(inputGain[channel]-inputTarget);
  outputGain[channel]=outputTarget+coefficient*(outputGain[channel]-outputTarget);
  float in=(std::isfinite(input)?input:0)*inputGain[channel],out=0;float* ip=&in;float* op=&out;
  try{models[channel]->process(&ip,&op,1);}catch(...){state=LiveAmpStatus::Fault;return 0;}
  out*=outputGain[channel];if(!std::isfinite(out)){state=LiveAmpStatus::Fault;return 0;}return out;
 }
 LiveAmpStatus status()const noexcept override{return state.load(std::memory_order_relaxed);}
 double expectedRate()const noexcept override{return expected;}
};
}
std::unique_ptr<LiveAmp> loadLiveNamAmp(const std::string& path,double rate,bool allowRateMismatch){return std::make_unique<NamAmp>(path,rate,allowRateMismatch);}
}
