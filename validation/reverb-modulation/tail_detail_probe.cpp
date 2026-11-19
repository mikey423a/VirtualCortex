#include "DSP/StereoReverb.h"
#include <cmath>
#include <iostream>

int main() {
 const double tau=6.283185307179586;
 for(double rate:{48000.,96000.})for(double hz:{1000.,4000.,8000.,12000.})for(float depth:{0.f,.8f}){
  vc::StereoReverb linear,cubic;
  vc::StereoReverbParameters controls;
  controls.enabled=true;
  controls.mix=1.f;
  controls.early=0.f;
  controls.modRate=2.f;
  controls.modDepth=depth;
  controls.preDelay=0.f;
  linear.prepare(rate);
  cubic.prepare(rate);
  linear.configure(controls);
  controls.detailRead=true;
  cubic.configure(controls);
  double sums[2]{};
  for(int i=0;i<rate*6;++i){
   const float input=static_cast<float>(.1*std::sin(tau*hz*i/rate));
   const auto a=linear.process(input,0.f),b=cubic.process(input,0.f);
   if(i>=rate*4)for(int ch=0;ch<2;++ch){sums[0]+=double(a[ch])*a[ch];sums[1]+=double(b[ch])*b[ch];}
  }
  std::cout<<rate<<' '<<hz<<' '<<depth<<' '<<10*std::log10(sums[1]/sums[0])<<'\n';
 }
}
