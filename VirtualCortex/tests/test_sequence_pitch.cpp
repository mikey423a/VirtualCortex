#include "DSP/TrackingEffects.h"
#include <stdexcept>
#include <iostream>
#include <limits>
void check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int main(){try{
 for(double fs:{44100.,48000.,96000.,192000.})for(float interval:{-1.f,1.f}){
  vc::SequenceCrusher dsp;dsp.prepare(fs);vc::SequenceCrusher::Parameters p;p.enabled=true;p.pitch=true;p.mix=1;p.bits=24;p.rate=48000;p.cutoff=18000;p.steps.fill(interval);dsp.configure(p);
  int crossings=0;float previous=0;double energy=0;
  for(int i=0;i<int(fs*2);++i){const float x=.2f*float(std::sin(6.283185307179586*220*i/fs));auto out=dsp.process(x,0);check(std::isfinite(out[0]),"Nonfinite pitch output");check(std::abs(out[1])<1.e-8f,"Stereo pitch crosstalk");if(i>=int(fs)){if(previous<=0&&out[0]>0)++crossings;energy+=out[0]*out[0];}previous=out[0];}
  const int expected=interval<0?110:440;std::cout<<fs<<" shift "<<interval<<" crossings "<<crossings<<"\n";check(std::abs(crossings-expected)<12,"Octave pitch estimate wrong");check(energy>1,"Pitch output silent");
  p.pitch=false;p.mix=0;dsp.configure(p);for(int i=0;i<int(fs);++i)dsp.process(.1f,0);check(std::abs(dsp.process(.1f,0)[0]-.1f)<1.e-6f,"Pitch bypass did not settle dry");
  dsp.prepare(fs);p.enabled=true;p.pitch=true;p.mix=1;p.stutter=.8f;p.stepMs=25;p.steps={{-1,0,1,.5f,-.5f,0}};dsp.configure(p);
  for(int i=0;i<int(fs);++i){const auto out=dsp.process(.1f*std::sin(i*.1f),0);check(std::isfinite(out[0])&&std::abs(out[0])<=1.1f,"Sequence automation unstable");}
  dsp.prepare(fs);dsp.configure(p);for(int i=0;i<int(fs*.1);++i)check(dsp.process(0,0)[0]==0,"Pitch restart retained stale audio");
 }
 std::cout<<"PASS: pitch frequency, stereo isolation, bypass, sequencing and restart\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
