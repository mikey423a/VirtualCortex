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

// Informational one-octave-width RBJ bandpass analysis (Q=sqrt(2)).
// This is a measurement filter, not an IEC-certified octave filter bank.
std::vector<std::array<float,2>> band(const std::vector<std::array<float,2>>& in,double fs,double hz){
 const double w=6.283185307179586*hz/fs,alpha=std::sin(w)/(2*std::sqrt(2.));
 const double b0=alpha/(1+alpha),b2=-b0,a1=-2*std::cos(w)/(1+alpha),a2=(1-alpha)/(1+alpha);
 std::array<double,2> z1{},z2{};auto out=in;
 for(auto& f:out)for(int ch=0;ch<2;++ch){double x=f[ch],y=b0*x+z1[ch];z1[ch]=-a1*y+z2[ch];z2[ch]=b2*x-a2*y;f[ch]=(float)y;}
 return out;
}
int main(){
 for(float depth:{0.f,.8f})for(float damping:{1.f,.3f}){
  Params q;q.decay=2;q.lowDecay=1;q.highDecay=damping;q.modDepth=depth;q.preDelay=0;q.early=0;
  auto y=impulse(48000,q,8);
  for(double hz:{500.,2000.,8000.}){
   auto filtered=band(y,48000,hz);const double e=energy(filtered,0,filtered.size());assert(std::isfinite(e)&&e>1.e-12);
   const double measured=rt60(filtered,48000);assert(std::isfinite(measured)&&measured>.05&&measured<8);
   std::cout<<"BAND_DECAY center_hz="<<hz<<" depth_ms="<<depth<<" high_ratio="<<damping<<" measured="<<measured<<"\n";
  }
 }

 // Modulation decay characterization, with bounded output and nonzero tails.
 for(double rate:{44100.,96000.})for(float depth:{0.f,.8f,2.f}){
  Params q;q.decay=2;q.lowDecay=q.highDecay=1;q.modDepth=depth;q.preDelay=0;q.early=0;
  auto y=impulse(rate,q,8);double peak=0;for(auto frame:y)for(float x:frame){assert(std::isfinite(x));peak=std::max(peak,(double)std::abs(x));}
  const double measured=rt60(y,rate);
  std::cout<<"MOD_DECAY rate="<<rate<<" depth_ms="<<depth<<" measured="<<measured<<" peak="<<peak<<"\n";
  assert(peak>1.e-5&&peak<1&&measured>1&&measured<3);
 }

 // Characterize decay scaling across room sizes and sample rates.
 for(double rate:{44100.,96000.})for(float decay:{.5f,2.f,6.f})for(float size:{.5f,2.f}){
  Params q;q.decay=decay;q.size=size;q.lowDecay=q.highDecay=1;q.modDepth=0;q.preDelay=0;q.early=0;
  auto y=impulse(rate,q,decay*4.+1.);const double measured=rt60(y,rate);
  std::cout<<"DECAY_SWEEP rate="<<rate<<" size="<<size<<" requested="<<decay<<" measured="<<measured<<" ratio="<<measured/decay<<"\n";
  // Broad regression guard for the characterized unmodulated configuration.
  assert(std::isfinite(measured)&&measured>decay*.8&&measured<decay*1.1);
 }

 for(double fs:{44100.,48000.,96000.,192000.}){
  Params p;p.decay=1.2f;p.lowDecay=p.highDecay=1;p.modDepth=0;p.preDelay=0;p.early=0;p.diffusion=.7f;
  auto y=impulse(fs,p,4);double measured=rt60(y,fs);std::cout<<fs<<" Hz measured broadband T60 "<<measured<<" s\n";assert(measured>.9&&measured<1.5);
  double diff=0;for(auto x:y){for(float v:x)assert(std::isfinite(v)&&std::abs(v)<1);diff+=(x[0]-x[1])*(x[0]-x[1]);}assert(diff>1.e-5);assert(energy(y,(std::size_t)(fs*3),y.size())<energy(y,0,(std::size_t)fs)*1.e-10);
  p.width=0;auto mono=impulse(fs,p,.5);for(auto x:mono)assert(x[0]==x[1]);
  p.preDelay=100;p.diffusion=0;p.early=1;auto early=impulse(fs,p,.3);assert(energy(early,0,(std::size_t)(fs*.106))<1.e-15);assert(energy(early,(std::size_t)(fs*.107),(std::size_t)(fs*.15))>1.e-5);
 }
 // Clean pre-delay transitions equal a 20 ms blend of two fixed read heads.
 // Checking actual audio against independent reference engines catches moving-head
 // pitch sweeps, wrong endpoints, unequal early-tap timing and channel mistakes.
 for(double fs:{44100.,48000.,96000.,192000.}){
  vc::StereoReverb moving,oldHead,newHead;for(auto* e:{&moving,&oldHead,&newHead})e->prepare(fs);
  Params p;p.enabled=true;p.mix=1;p.early=1;p.modDepth=0;p.preGlide=false;p.preDelay=20;
  moving.configure(p);oldHead.configure(p);p.preDelay=180;newHead.configure(p);
  const int warmCount=(int)(fs*.6),fade=(int)(fs*.02);
  for(int i=0;i<warmCount;++i){float l=.1f*std::sin(i*.17),r=.07f*std::cos(i*.23);moving.process(l,r);oldHead.process(l,r);newHead.process(l,r);}
  moving.configure(p);
  for(int i=0;i<fade+100;++i){if(i==fade/2){p.preGlide=true;moving.configure(p);}int n=i+warmCount;float l=.1f*std::sin(n*.17),r=.07f*std::cos(n*.23);auto a=oldHead.process(l,r),b=newHead.process(l,r),y=moving.process(l,r);float w=std::min(1.f,(float)(i+1)/fade);for(int ch=0;ch<2;++ch)assert(std::abs(y[ch]-(a[ch]*(1-w)+b[ch]*w))<2.e-6f);}
  // A completed transition stays at the new head; repeated changes and mid-fade
  // mode switches must remain finite and bounded. The reference comparison above
  // also checks switching to glide halfway through a clean transition.
  for(int i=0;i<4000;++i){if(i%128==0){p.preDelay=i%256?500:0;p.preGlide=i%512==0;moving.configure(p);}auto y=moving.process(.05f,.05f);for(float v:y)assert(std::isfinite(v)&&std::abs(v)<1);}
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
 // Cached coefficients must be invalidated when preparing a different rate.
 for(double rate:{44100.,96000.,192000.}){
  vc::StereoReverb reused,fresh;Params q;q.enabled=true;q.mix=1;q.decay=4;q.lowDecay=1.5f;
  reused.prepare(48000);reused.configure(q);for(int i=0;i<24000;++i)reused.process(.1f,.05f);
  reused.prepare(rate);reused.configure(q);fresh.prepare(rate);fresh.configure(q);
  for(int i=0;i<24000;++i){float x=i==0?.1f:0.f;assert(reused.process(x,0)==fresh.process(x,0));}
 }
 std::cout<<"PASS stereo FDN decay, decorrelation, early timing, width, damping, ducking, automation and reset\n";
}
