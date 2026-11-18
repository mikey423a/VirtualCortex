#pragma once
#include "LegacyStereoReverb.h"
#include <cstring>
// Frozen 6.40 renderer is independent of the optimized production header.
inline vc_legacy::StereoReverbParameters legacyParams(const Params& p) {
 vc_legacy::StereoReverbParameters q;
 q.decay=p.decay;
 q.size=p.size;
 q.preDelay=p.preDelay;
 q.diffusion=p.diffusion;
 q.early=p.early;
 q.mix=p.mix;
 q.lowDecay=p.lowDecay;
 q.highDecay=p.highDecay;
 q.lowFreq=p.lowFreq;
 q.highFreq=p.highFreq;
 q.modRate=p.modRate;
 q.modDepth=p.modDepth;
 q.width=p.width;
 q.ducking=p.ducking;
 q.release=p.release;
 q.wetLevel=p.wetLevel;
 q.inputHP=p.inputHP;
 q.inputLP=p.inputLP;
 q.enabled=p.enabled;
 q.preGlide=p.preGlide;
 return q;
}
inline void reverbControlEquivalence() {
 vc::StereoReverb current;vc_legacy::StereoReverb legacy;
 for(double rate:{48000.,44100.,96000.,192000.,8000.,48000.}) {
  current.prepare(rate);legacy.prepare(rate);
  Params p;p.enabled=true;p.mix=.8f;p.ducking=.7f;
  current.configure(p);legacy.configure(legacyParams(p));
  for(int i=0;i<96000;++i) {
   if(i%127==0) {
    const int block=i/127;
    // Repeated settings, changes, bypass and invalid values exercise the
    // sanitized cache keys, smoothing, predelay queue and envelope state.
    if(block%11==0) {
     p.release=block%2?20.f:2000.f;p.wetLevel=block%3?12.f:-24.f;
     p.preDelay=block%2?0.f:180.f;p.preGlide=block%3==0;
     p.size=block%2?.5f:2.f;p.decay=block%2?.2f:20.f;
     p.modRate=block%2?.05f:4.f;p.modDepth=block%2?0.f:2.f;
     p.inputHP=block%2?20.f:1000.f;p.inputLP=block%2?1000.f:20000.f;
     p.lowDecay=block%2?.25f:2.f;p.highDecay=block%2?.1f:1.f;
     p.mix=block%2?0.f:1.f;p.enabled=block%5!=0;
    }
    if(block%37==0){p.release=std::numeric_limits<float>::quiet_NaN();p.wetLevel=std::numeric_limits<float>::infinity();}
    current.configure(p);legacy.configure(legacyParams(p));
   }
   const float l=.1f*std::sin(i*.137f),r=.07f*std::cos(i*.081f);
   const auto a=current.process(l,r),b=legacy.process(l,r);
   assert(std::memcmp(a.data(),b.data(),sizeof(float)*2)==0);
  }
 }
 // Fully settled controls and long bypass/rest activation must also match.
 current.prepare(8000);legacy.prepare(8000);Params p;p.enabled=true;p.mix=1;
 current.configure(p);legacy.configure(legacyParams(p));
 for(int i=0;i<1000000;++i){
  if(i==4000)p.enabled=false;if(i==990000)p.enabled=true;
  if(i%64==0){current.configure(p);legacy.configure(legacyParams(p));}
  const auto a=current.process(i%101==0?.1f:0.f,0),b=legacy.process(i%101==0?.1f:0.f,0);
  assert(std::memcmp(a.data(),b.data(),sizeof(float)*2)==0);
 }
 std::cout<<"PASS: reverb control optimization bit-exact legacy automation, sanitization, sleep/wake and rate restart\n";
}
template<class Engine,class P> double reverbControlTiming(bool automate,double& checksum) {
 Engine r;r.prepare(48000);P p;p.enabled=true;p.mix=1;r.configure(p);
 for(int i=0;i<48000;++i)r.process(0,0);
 const auto start=std::chrono::steady_clock::now();double sum=0;
 for(int i=0;i<480000;++i){
  if(i%64==0){if(automate){p.release=(i/64)%2?20.f:2000.f;p.wetLevel=(i/64)%2?-6.f:3.f;p.size=(i/64)%2?.5f:2.f;}r.configure(p);}
  auto y=r.process(i%97==0?.1f:0.f,i%131==0?-.07f:0.f);sum+=y[0]+y[1];
 }
 checksum=sum;return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
}
inline void reverbControlBenchmark(){
 for(bool automate:{false,true}){
  std::vector<double> optimized,old;double a=0,b=0;
  for(int pass=0;pass<6;++pass){
   if(pass%2){old.push_back(reverbControlTiming<vc_legacy::StereoReverb,vc_legacy::StereoReverbParameters>(automate,b));optimized.push_back(reverbControlTiming<vc::StereoReverb,Params>(automate,a));}
   else{optimized.push_back(reverbControlTiming<vc::StereoReverb,Params>(automate,a));old.push_back(reverbControlTiming<vc_legacy::StereoReverb,vc_legacy::StereoReverbParameters>(automate,b));}
   assert(a==b&&std::isfinite(a));
  }
  std::sort(optimized.begin(),optimized.end());std::sort(old.begin(),old.end());
  std::cout<<"REVERB_CONTROL_CPU automated="<<automate<<" frames=480000 block=64 rate=48000 optimized_ms="<<(optimized[2]+optimized[3])*.5<<" legacy_ms="<<(old[2]+old[3])*.5<<" checksum="<<a<<"\n";
 }
}
