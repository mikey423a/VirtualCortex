#pragma once
#include <algorithm>
#include <cmath>
namespace vc {
// Original feed-forward peak compressor. This is not a model of a branded unit.
class PeakCompressor {
public:
 float process(float x,float threshold,float ratio,float attack,float release,float makeup,float mix,float sampleRate) noexcept {
  if(threshold!=threshold_||ratio!=ratio_||attack!=attack_||release!=release_||makeup!=makeup_||sampleRate!=fs_){
   threshold_=threshold;ratio_=ratio;attack_=attack;release_=release;makeup_=makeup;fs_=sampleRate;
   thresholdGain_=std::pow(10.f,(-48.f+48.f*threshold)/20.f);inverseRatio_=1.f/(1.f+19.f*ratio);
   attackCoefficient_=std::exp(-1.f/(sampleRate*(.0001f+.0999f*attack)));
   releaseCoefficient_=std::exp(-1.f/(sampleRate*(.01f+.99f*release)));
   makeupGain_=std::pow(10.f,24.f*makeup/20.f);
  }
  const float peak=std::abs(x),coefficient=peak>envelope_?attackCoefficient_:releaseCoefficient_;
  envelope_=coefficient*envelope_+(1.f-coefficient)*peak;
  const float gain=envelope_>thresholdGain_?std::pow(thresholdGain_/envelope_,1.f-inverseRatio_):1.f;
  return x*((1.f-mix)+mix*gain*makeupGain_);
 }
private:
 float threshold_=-1,ratio_=-1,attack_=-1,release_=-1,makeup_=-1,fs_=-1;
 float thresholdGain_=1,inverseRatio_=1,attackCoefficient_=0,releaseCoefficient_=0,makeupGain_=1,envelope_=0;
};
// Two original clipping transfer functions, 2x interpolation, tone low-pass,
// and DC removal. No claim of analog circuit or branded pedal equivalence.
class ClippingDrive {
public:
 float process(float x,float drive,float tone,float level,float mix,float sampleRate,bool hard) noexcept {
  if(drive!=drive_||tone!=tone_||sampleRate!=fs_){
   drive_=drive;tone_=tone;fs_=sampleRate;gain_=std::pow(10.f,36.f*drive/20.f);
   toneCoefficient_=std::exp(-6.283185307f*(500.f+9500.f*tone)/(sampleRate*2.f));
   dcCoefficient_=std::exp(-6.283185307f*20.f/(sampleRate*2.f));
  }
  float total=0;
  for(int sub=0;sub<2;++sub){
   const float sample=(sub==0?(previousInput_+x)*.5f:x)*gain_;
   const float clipped=hard?std::clamp(sample,-1.f,1.f):std::tanh(sample);
   lowpass_=toneCoefficient_*lowpass_+(1.f-toneCoefficient_)*clipped;
   const float filtered=lowpass_-previousLowpass_+dcCoefficient_*dc_;previousLowpass_=lowpass_;dc_=filtered;total+=filtered;
  }
  previousInput_=x;return (1.f-mix)*x+mix*level*2.f*total*.5f;
 }
private:
 float drive_=-1,tone_=-1,fs_=-1,gain_=1,toneCoefficient_=0,dcCoefficient_=0;
 float previousInput_=0,lowpass_=0,previousLowpass_=0,dc_=0;
};
}
