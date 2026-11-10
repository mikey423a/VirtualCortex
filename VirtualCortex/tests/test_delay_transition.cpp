#include "DSP/StereoDelay.h"
#include <stdexcept>
#include <iostream>
int main(){try{
 for(double fs:{44100.,48000.,96000.,192000.}){
  vc::StereoDelay moving,oldHead,newHead;for(auto* p:{&moving,&oldHead,&newHead})p->prepare(fs);
  vc::StereoDelayParameters p;p.enabled=true;p.mix=1;p.feedback=0;p.filterMix=0;p.timeL=20;p.timeR=37;
  moving.configure(p);oldHead.configure(p);p.timeL=180;p.timeR=240;newHead.configure(p);
  const int warm=(int)(fs*.6),fade=(int)(fs*.02);
  for(int i=0;i<warm;++i){float l=.1f*std::sin(i*.17),r=.08f*std::cos(i*.23);moving.process(l,r);oldHead.process(l,r);newHead.process(l,r);}
  moving.configure(p);
  for(int i=0;i<fade+200;++i){if(i==fade/2){p.tape=true;moving.configure(p);}int n=i+warm;float l=.1f*std::sin(n*.17),r=.08f*std::cos(n*.23);auto a=oldHead.process(l,r),b=newHead.process(l,r),y=moving.process(l,r);float blend=std::min(1.f,(float)(i+1)/fade);for(int c=0;c<2;++c)if(std::abs(y[c]-(a[c]*(1-blend)+b[c]*blend))>2.e-6f)throw std::runtime_error("Tape switch broke clean crossfade");}
 }
 std::cout<<"PASS: delay clean-to-tape transitions match independent read-head crossfade at four sample rates\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
