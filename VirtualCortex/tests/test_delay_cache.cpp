#include "DSP/StereoDelay.h"
#include "fixtures/LegacyStereoDelay.h"
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <algorithm>
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

 // Informational paired benchmark: no flaky wall-clock pass/fail threshold.
 // Warm both engines, alternate order, retain output sums, report medians.
 {
  vc::StereoDelay current;vc::LegacyStereoDelay old;current.prepare(48000);old.prepare(48000);
  vc::StereoDelayParameters p;vc::LegacyStereoDelayParameters q;
  p.enabled=q.enabled=true;p.mix=q.mix=.7f;p.feedback=q.feedback=.75f;
  current.configure(p);old.configure(q);
  for(int i=0;i<48000;++i){current.process(.01f,-.01f);old.process(.01f,-.01f);}
  auto measure=[](auto& effect){
   double sum=0;const auto begin=std::chrono::steady_clock::now();
   for(int i=0;i<480000;++i){const float x=i%97==0?.1f:0.f;auto y=effect.process(x,-x);sum+=y[0]+2.*y[1];}
   const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();
   return std::array<double,2>{ms,sum};
  };
  std::array<double,6> cachedTimes{},legacyTimes{};
  for(int pass=0;pass<6;++pass){
   std::array<double,2> a,b;
   if(pass%2){b=measure(old);a=measure(current);}else{a=measure(current);b=measure(old);}
   if(a[1]!=b[1])throw std::runtime_error("Steady benchmark output differs");
   cachedTimes[pass]=a[0];legacyTimes[pass]=b[0];
  }
  std::sort(cachedTimes.begin(),cachedTimes.end());std::sort(legacyTimes.begin(),legacyTimes.end());
  std::cout<<"BENCHMARK 480k stereo frames at 48kHz, 6 alternating passes: cached median ms="<<(cachedTimes[2]+cachedTimes[3])*.5<<", legacy median ms="<<(legacyTimes[2]+legacyTimes[3])*.5<<"\n";
 }

 std::cout<<"PASS: exact legacy output across cutoff/time automation, bypass and prepare restarts\n";
}catch(const std::exception& e){std::cerr<<e.what();return 1;}}
