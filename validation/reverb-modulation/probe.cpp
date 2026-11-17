#include "DSP/StereoReverb.h"
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
// Offline characterization only. Fixed 48 kHz, 1 kHz input, 2 Hz modulation.
int main(int argc,char** argv){
 if(argc!=2){std::cerr<<"Usage: characterize_reverb_modulation OUTPUT_DIRECTORY\n";return 2;}
 const std::filesystem::path output(argv[1]);std::filesystem::create_directories(output);
 for(int depth=0;depth<3;++depth){
  vc::StereoReverb r;r.prepare(48000);vc::StereoReverbParameters p;
  p.enabled=true;p.mix=1;p.decay=1;p.lowDecay=p.highDecay=1;
  p.modRate=2;p.modDepth=depth==0?0.f:depth==1?.8f:2.f;
  p.preDelay=0;p.early=0;r.configure(p);
  std::ofstream file(output/("mod-"+std::to_string(depth)+".f32"),std::ios::binary);
  if(!file)return 3;
  for(int i=0;i<48000*12;++i){
   float x=(float)(.1*std::sin(6.283185307179586*1000*i/48000.));auto y=r.process(x,0);
   if(i>=48000*4)file.write(reinterpret_cast<const char*>(y.data()),2*sizeof(float));
  }
  file.close();if(!file)return 4;
 }
}
