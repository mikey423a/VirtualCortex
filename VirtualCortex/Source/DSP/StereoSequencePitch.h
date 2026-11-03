#pragma once
#include <array>
#include <complex>
#include <algorithm>
#include <cmath>
namespace vc {
// Original stereo phase-vocoder shifter: 2048-point STFT, 4x overlap.
// Fixed storage, independent channel phases, no proprietary firmware.
class StereoSequencePitch {
 static constexpr int n=2048,hop=512,latency=n-hop,bins=n/2+1;
 using Complex=std::complex<float>;
 struct Channel {std::array<float,n>input{},accum{};std::array<float,hop>output{};std::array<float,bins>previous{},phase{};};
public:
 void prepare(double rate){fs=rate;state={};rover=latency;blend=0;wasEnabled=false;for(int i=0;i<n;++i)window[i]=float(.5-.5*std::cos(6.283185307179586*i/n));}
 std::array<float,2> process(float l,float r,float semitones,bool enabled) noexcept {
  std::array<float,2>out{{l,r}};
  if(!enabled&&!wasEnabled&&blend<1.e-7f){blend=0;return out;}
  if(enabled&&!wasEnabled&&blend<1.e-7f){state={};rover=latency;}wasEnabled=enabled;
  const double ratio=std::pow(2.,enabled?double(semitones)/12.:0.);
  blend+=float(1.-std::exp(-1./(fs*.01)))*((enabled?1.f:0.f)-blend);
  for(int ch=0;ch<2;++ch){auto& s=state[ch];s.input[rover]=out[ch];out[ch]+=blend*(s.output[rover-latency]-out[ch]);}
  if(++rover==n){rover=latency;for(auto& s:state)transform(s,ratio);}
  return out;
 }
private:
 static void fft(std::array<Complex,n>& a,bool inverse)noexcept {
  for(int i=1,j=0;i<n;++i){int bit=n>>1;for(;j&bit;bit>>=1)j^=bit;j^=bit;if(i<j)std::swap(a[i],a[j]);}
  for(int len=2;len<=n;len<<=1){const double angle=(inverse?1.:-1.)*6.283185307179586/len;const Complex increment(float(std::cos(angle)),float(std::sin(angle)));
   for(int i=0;i<n;i+=len){Complex w(1,0);for(int j=0;j<len/2;++j){const Complex u=a[i+j],v=a[i+j+len/2]*w;a[i+j]=u+v;a[i+j+len/2]=u-v;w*=increment;}}}
  if(inverse)for(auto& x:a)x/=float(n);
 }
 void transform(Channel& s,double ratio)noexcept {
  for(int i=0;i<n;++i)spectrum[i]=Complex(s.input[i]*window[i],0);fft(spectrum,false);magnitude.fill(0);frequency.fill(0);
  constexpr double tau=6.283185307179586;
  for(int k=0;k<bins;++k){const float mag=std::abs(spectrum[k]);const float phase=std::arg(spectrum[k]);const double expected=tau*k*hop/n;double delta=phase-s.previous[k]-expected;s.previous[k]=phase;delta-=tau*std::round(delta/tau);const double advance=(expected+delta)*ratio;const int destination=int(std::lround(k*ratio));if(destination<bins){magnitude[destination]+=mag;frequency[destination]+=float(advance*mag);}}
  spectrum.fill(Complex{});
  for(int k=0;k<bins;++k){if(magnitude[k]>1.e-12f)s.phase[k]=float(std::remainder(s.phase[k]+frequency[k]/magnitude[k],tau));spectrum[k]=std::polar(magnitude[k],s.phase[k]);if(k>0&&k<n/2)spectrum[n-k]=std::conj(spectrum[k]);}
  spectrum[0]=Complex(spectrum[0].real(),0);spectrum[n/2]=Complex(spectrum[n/2].real(),0);fft(spectrum,true);
  for(int i=0;i<n;++i)s.accum[i]+=spectrum[i].real()*window[i]*(2.f/3.f);
  for(int i=0;i<hop;++i)s.output[i]=s.accum[i];for(int i=0;i<n-hop;++i){s.accum[i]=s.accum[i+hop];s.input[i]=s.input[i+hop];}for(int i=n-hop;i<n;++i)s.accum[i]=0;
 }
 double fs=48000;int rover=latency;float blend=0;bool wasEnabled=false;std::array<Channel,2>state{};std::array<float,n>window{};std::array<Complex,n>spectrum{};std::array<float,bins>magnitude{},frequency{};
};
}
