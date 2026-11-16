#include "DSP/StereoDelay.h"
#include "fixtures/LegacyStereoDelay.h"
#include <iostream>
#include <stdexcept>
int main(){try{
 for(double rate:{44100.,48000.,96000.}){
  vc::StereoDelay current;vc::LegacyStereoDelay old;
  for(int restart=0;restart<2;++restart){
   const double fs=restart?rate*.5:rate;current.prepare(fs);old.prepare(fs);
   vc::StereoDelayParameters p;vc::LegacyStereoDelayParameters q;
   for(int i=0;i<120000;++i){
    if(i%4096==0){
     const int step=i/4096;p.enabled=q.enabled=step%7!=6;
     p.lowCut=q.lowCut=20.f+float(step%6)*111.f;p.highCut=q.highCut=2000.f+float(step%9)*1700.f;
     p.timeL=q.timeL=13.f+step%5*11.f;p.timeR=q.timeR=19.f+step%7*7.f;
     p.mix=q.mix=.7f;p.feedback=q.feedback=.8f;p.diffusion=q.diffusion=.4f;
     p.modDepth=q.modDepth=1.2f;p.tape=q.tape=step%2!=0;current.configure(p);old.configure(q);
    }
    float l=.1f*std::sin(float(i)*.071f),r=.07f*std::cos(float(i)*.113f);
    const auto a=current.process(l,r),b=old.process(l,r);
    if(a!=b)throw std::runtime_error("Cached delay changed numerical output");
   }
  }
 }
 std::cout<<"PASS: exact legacy output across cutoff/time automation, bypass and prepare restarts\n";
}catch(const std::exception& e){std::cerr<<e.what();return 1;}}
