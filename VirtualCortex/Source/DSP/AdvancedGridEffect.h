#pragma once
#include "AdvancedGridParameters.h"
#include "StereoDelay.h"
#include "StereoReverb.h"
#include "MultiHeadEcho.h"
#include "CreativeEffects.h"
#include "TrackingEffects.h"
#include "SequenceTempo.h"
#include <memory>
namespace vc {
// One independently owned effect per slot. Construction/prepare/destruction are
// control-thread operations with audio excluded. configure/process allocate nothing.
class AdvancedGridEffect {
public:
 explicit AdvancedGridEffect(int kind):type(kind){
  switch(type){case 30:delay=std::make_unique<StereoDelay>();break;case 31:reverb=std::make_unique<StereoReverb>();break;
   case 32:echo=std::make_unique<MultiHeadEcho>();break;case 33:pitch=std::make_unique<IntervalEcho>();break;
   case 34:loop=std::make_unique<LayerLoom>();break;case 35:synth=std::make_unique<TrackingSynth>();break;case 36:crusher=std::make_unique<SequenceCrusher>();break;}
 }
 int kind()const noexcept{return type;}
 void prepare(double rate){fs=rate;mix=0;note=-1;velocity=0;if(delay)delay->prepare(rate);if(reverb)reverb->prepare(rate);if(echo)echo->prepare(rate);if(pitch)pitch->prepare(rate);if(loop)loop->prepare(rate);if(synth)synth->prepare(rate);if(crusher)crusher->prepare(rate);}
 void configure(const AdvancedValues& v,float hostTempo=0) noexcept {
  values=v;auto tempo=[&](int i){return hostTempo>0?hostTempo:v[i];};
  switch(type){
   case 30:{StereoDelayParameters p;p.enabled=true;p.timeL=v[0];p.timeR=v[1];p.feedback=v[2];p.cross=v[3];p.lowCut=v[4];p.highCut=v[5];p.mix=v[6];p.modRate=v[7];p.modDepth=v[8];p.ducking=v[9];p.duckRelease=v[10];p.width=v[11];p.wetLevel=v[12];p.glide=v[13];p.filterMix=v[17];p.diffusion=v[18];p.pingPong=v[19]>.5f;p.tape=v[20]>.5f;
    if(v[21]>.5f){p.timeL=sequenceStepMilliseconds(v[0],tempo(14),v[15],true);p.timeR=sequenceStepMilliseconds(v[1],tempo(14),v[16],true);}delay->configure(p);break;}
   case 31:{StereoReverbParameters p;p.enabled=true;p.decay=v[0];p.size=v[1];p.preDelay=v[2];p.diffusion=v[3];p.early=v[4];p.mix=v[5];p.lowDecay=v[6];p.highDecay=v[7];p.lowFreq=v[8];p.highFreq=v[9];p.modRate=v[10];p.modDepth=v[11];p.width=v[12];p.ducking=v[13];p.release=v[14];p.wetLevel=v[15];p.inputHP=v[16];p.inputLP=v[17];p.preGlide=v[18]>.5f;reverb->configure(p);break;}
   case 32:{std::array<MultiHeadEcho::Head,4> heads;for(int i=0;i<4;++i)heads[i]={v[4*i],v[4*i+1],v[4*i+2],v[4*i+3]};echo->configure(heads);break;}
   case 33:{IntervalEcho::Parameters p;p.enabled=true;p.time=v[0];p.feedback=v[1];p.a=v[2];p.b=v[3];p.step=sequenceStepMilliseconds(v[4],tempo(7),v[8],v[9]>.5f);p.cutoff=v[5];p.mix=v[6];pitch->configure(p);break;}
   case 34:loop->configure(v[0],v[1],v[2],v[3]>.5f,true);break;
   case 35:{TrackingSynth::Parameters p;p.enabled=true;p.mix=v[0];p.transpose=v[1];p.detune=v[2];p.cutoff=v[3];p.resonance=v[4];p.attack=v[5];p.release=v[6];p.glide=v[7];p.wave=v[8];p.gate=v[9];synth->configure(p);break;}
   case 36:{SequenceCrusher::Parameters p;p.enabled=true;p.bits=v[0];p.rate=v[1];p.cutoff=v[2];p.stepMs=sequenceStepMilliseconds(v[3],tempo(15),v[16],v[17]>.5f);p.depth=v[4];p.mix=v[5];p.mode=v[6];p.stutterMs=v[7];p.stutter=v[8];for(int i=0;i<6;++i)p.steps[i]=v[9+i];p.pitch=v[18]>.5f;crusher->configure(p);break;}
  }
 }
 std::array<float,2> process(float l,float r) noexcept {
  if(delay)return delay->process(l,r);if(reverb)return reverb->process(l,r);
  if(echo){const auto wet=echo->process((l+r)*.5f);mix+=float(1.-std::exp(-1./(fs*.01)))*(values[16]-mix);return {l*(1-mix)+wet[0]*mix,r*(1-mix)+wet[1]*mix};}
  if(pitch)return pitch->process(l,r);if(loop)return loop->process(l,r);
  if(synth){const auto frame=synth->process(l,r);note=values[10]>.5f?frame.note:-1;velocity=frame.velocity;return frame.audio;}
  if(crusher)return crusher->process(l,r);return {l,r};
 }
 void command(unsigned bits)noexcept {if(loop)loop->command(bits);}
 unsigned status()const noexcept{return loop?unsigned(loop->layerCount())|(loop->isRecording()?256u:0u)|(loop->isPlaying()?512u:0u):0;}
 int midiNote()const noexcept{return note;}float midiVelocity()const noexcept{return velocity;}
private:
 int type,note=-1;float velocity=0,mix=0;double fs=48000;AdvancedValues values{};
 std::unique_ptr<StereoDelay>delay;std::unique_ptr<StereoReverb>reverb;std::unique_ptr<MultiHeadEcho>echo;
 std::unique_ptr<IntervalEcho>pitch;std::unique_ptr<LayerLoom>loop;std::unique_ptr<TrackingSynth>synth;std::unique_ptr<SequenceCrusher>crusher;
};
}
