#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/StereoReverb.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
using Params=vc::StereoReverbParameters;
void warm(vc::StereoReverb& r,double fs){for(int i=0;i<(int)(fs*.5);++i)r.process(0,0);}
std::vector<std::array<float,2>> impulse(double fs,Params p,double seconds){vc::StereoReverb r;r.prepare(fs);p.enabled=true;p.mix=1;r.configure(p);warm(r,fs);std::vector<std::array<float,2>> y((std::size_t)(seconds*fs));for(std::size_t i=0;i<y.size();++i)y[i]=r.process(i==0?.1f:0.f,0);return y;}
double energy(const std::vector<std::array<float,2>>& y,std::size_t begin,std::size_t end){double e=0;for(std::size_t i=begin;i<std::min(end,y.size());++i)for(float x:y[i])e+=x*x;return e;}
double rt60(const std::vector<std::array<float,2>>& y,double fs){std::vector<double> e(y.size()+1);for(std::size_t i=y.size();i-->0;)e[i]=e[i+1]+y[i][0]*y[i][0]+y[i][1]*y[i][1];std::size_t a=0,b=0;for(std::size_t i=0;i<y.size();++i){double db=10*std::log10(std::max(1.e-30,e[i]/e[0]));if(!a&&db<=-5)a=i;if(db<=-25){b=i;break;}}assert(a&&b>a);return (b-a)/fs*3;}
int main(){
 for(double fs:{44100.,48000.,96000.,192000.}){
  Params p;p.decay=1.2f;p.lowDecay=p.highDecay=1;p.modDepth=0;p.preDelay=0;p.early=0;p.diffusion=.7f;
  auto y=impulse(fs,p,4);double measured=rt60(y,fs);std::cout<<fs<<" Hz measured broadband T60 "<<measured<<" s\n";assert(measured>.9&&measured<1.5);
  double diff=0;for(auto x:y){for(float v:x)assert(std::isfinite(v)&&std::abs(v)<1);diff+=(x[0]-x[1])*(x[0]-x[1]);}assert(diff>1.e-5);assert(energy(y,(std::size_t)(fs*3),y.size())<energy(y,0,(std::size_t)fs)*1.e-10);
  p.width=0;auto mono=impulse(fs,p,.5);for(auto x:mono)assert(x[0]==x[1]);
  p.preDelay=100;p.diffusion=0;p.early=1;auto early=impulse(fs,p,.3);assert(energy(early,0,(std::size_t)(fs*.106))<1.e-15);assert(energy(early,(std::size_t)(fs*.107),(std::size_t)(fs*.15))>1.e-5);
 }
 // High-frequency decay control reduces late high-frequency burst energy.
 auto burst=[](float ratio){vc::StereoReverb r;r.prepare(48000);Params p;p.enabled=true;p.mix=1;p.decay=2;p.lowDecay=1;p.highDecay=ratio;p.highFreq=2000;p.modDepth=0;p.early=0;r.configure(p);warm(r,48000);double e=0;for(int i=0;i<144000;++i){float x=i<4800?.1f*std::sin(i*6.2831853f*8000/48000):0;auto y=r.process(x,x);if(i>48000)for(float v:y)e+=v*v;}return e;};assert(burst(.2f)<burst(1)*.05);
 auto sustained=[](float duck){vc::StereoReverb r;r.prepare(48000);Params p;p.enabled=true;p.mix=1;p.ducking=duck;r.configure(p);double e=0;for(int i=0;i<96000;++i){auto y=r.process(.3f*std::sin(i*.13f),.3f*std::sin(i*.13f));if(i>48000)for(float v:y)e+=v*v;}return e;};assert(sustained(1)<sustained(0)*.4);
 vc::StereoReverb r;r.prepare(48000);assert(r.process(.1f,-.2f)[0]==.1f);Params p;p.enabled=true;p.mix=1;p.decay=20;p.lowDecay=2;p.highDecay=1;p.size=2;p.modDepth=2;r.configure(p);
 for(int block=0;block<300;++block){p.preDelay=block%2?500:0;p.size=block%2?.5f:2;p.diffusion=block%2?1:0;r.configure(p);for(int i=0;i<256;++i){auto y=r.process(.1f*std::sin((block*256+i)*.1f),-.07f);for(float v:y)assert(std::isfinite(v)&&std::abs(v)<8);}}
 p.decay=std::numeric_limits<float>::quiet_NaN();p.size=std::numeric_limits<float>::infinity();p.lowFreq=-100;p.highFreq=0;r.configure(p);for(int i=0;i<20000;++i)for(float x:r.process(0,0))assert(std::isfinite(x));
 p.enabled=false;r.configure(p);for(int i=0;i<48000;++i)r.process(.1f,-.2f);auto dry=r.process(.1f,-.2f);assert(dry[0]==.1f&&dry[1]==-.2f);
 // Preparing again clears the tank, diffusers and envelope.
 r.prepare(48000);p={};p.enabled=true;p.mix=1;r.configure(p);for(int i=0;i<48000;++i)for(float x:r.process(0,0))assert(x==0);
 std::cout<<"PASS stereo FDN decay, decorrelation, early timing, width, damping, ducking, automation and reset\n";
}
