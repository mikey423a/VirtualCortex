#include "DSP/TrackingEffects.h"
#include <cassert>
#include <iostream>
int main(){
 for(double fs:{44100.,48000.,96000.,192000.}){
  for(double hz:{82.4069,110.,220.,440.,987.767}){
   vc::TrackingSynth synth;synth.prepare(fs);vc::TrackingSynth::Parameters p;p.enabled=true;synth.configure(p);int note=-1;double energy=0;
   for(int i=0;i<int(fs*.4);++i){const float x=.2f*float(std::sin(6.283185307*hz*i/fs)+.35*std::sin(12.566370614*hz*i/fs));const auto f=synth.process(x,x);note=f.note;energy+=f.audio[0]*f.audio[0];assert(std::isfinite(f.audio[0]));}
   const int expected=int(std::lround(69+12*std::log2(hz/440.)));if(note!=expected){std::cerr<<"tracking "<<fs<<" "<<hz<<" got "<<note<<" expected "<<expected<<"\n";return 1;}assert(energy>1);
   for(int i=0;i<int(fs*.3);++i)note=synth.process(0,0).note;assert(note==-1);
   p.enabled=false;synth.configure(p);assert(synth.process(.1f,.2f).note==-1);
  }
  vc::TrackingSynth synth;synth.prepare(fs);vc::TrackingSynth::Parameters sp;sp.enabled=true;synth.configure(sp);unsigned random=12345;
  for(int i=0;i<int(fs);++i){random=random*1664525u+1013904223u;const float x=(float(random>>8)/8388608.f-1)*.1f;assert(synth.process(x,x).note==-1);}
  vc::SequenceCrusher crusher;crusher.prepare(fs);vc::SequenceCrusher::Parameters p;p.enabled=true;p.mix=1;p.stutter=.7f;crusher.configure(p);double energy=0;
  for(int mode=0;mode<3;++mode){p.mode=float(mode);crusher.configure(p);for(int i=0;i<int(fs*.8);++i){auto f=crusher.process(.2f*float(std::sin(i*.05)),0);assert(std::isfinite(f[0])&&std::isfinite(f[1]));energy+=f[0]*f[0];assert(std::abs(f[1])<1.e-8f);}}
  assert(energy>1);assert(crusher.currentStep()>=0&&crusher.currentStep()<6);
 }
 std::cout<<"PASS: harmonic guitar notes, noise rejection, silence release, all rates, sequencer and stereo isolation\n";
}
