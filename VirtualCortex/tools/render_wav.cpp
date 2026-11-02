// Offline validation utility: PCM16/PCM24/float32 little-endian mono/stereo WAV -> processed WAV.
// No JUCE dependency. Not a replacement for a Windows VST3/ASIO test.
#include "DSP/PortableAudioEngine.h"
#include "DSP/CabinetIRWav.h"
#include "OfflineGridConfig.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "WavFile.h"
using namespace vcwav;
int main(int argc,char** argv){
 if(argc<3 || argc>5){std::cerr<<"Usage: virtualcortex_render input.wav output.wav [cabinet-ir.wav] [--grid grid.txt]\n       virtualcortex_render input.wav output.wav --grid grid.txt\n";return 2;}
 try{
  std::string irPath,gridPath;
  for(int i=3;i<argc;++i) {
   if(std::string(argv[i])=="--grid") {if(++i>=argc || !gridPath.empty())throw std::runtime_error("--grid requires exactly one file");gridPath=argv[i];}
   else if(irPath.empty()) irPath=argv[i];
   else throw std::runtime_error("Unexpected argument");
  }
  auto w=readWav(argv[1]);vc::SignalGrid grid;
  grid.set(0,0,{vc::BlockType::Gate,true});grid.set(0,1,{vc::BlockType::Amp,true});grid.set(0,2,{vc::BlockType::Cabinet,true});grid.set(0,3,{vc::BlockType::EQ,true});
  grid.set(1,0,{vc::BlockType::Amp,true});grid.set(1,1,{vc::BlockType::Delay,true});grid.set(1,2,{vc::BlockType::Reverb,true});
  vc::PortableAudioEngine engine;
  if(!gridPath.empty()) {
   const auto config=vc::loadOfflineGridConfig(gridPath);
   if(!engine.configure(config.grid,config.lanes))throw std::runtime_error("Invalid custom grid");
   for(std::size_t row=0;row<4;++row)for(std::size_t col=0;col<8;++col) {
    if(!engine.setSlotParameters(row,col,config.params[row][col]))throw std::runtime_error("Invalid grid parameters");
    if(!config.neuralPaths[row][col].empty()) {
     const auto model=vc::loadNeuralAmpModel(config.neuralPaths[row][col]);
     if(!engine.setNeuralAmp(row,col,&model))throw std::runtime_error("Cannot assign neural model");
     std::cout<<"Loaded neural model for row "<<row<<" column "<<col<<"\n";
    }
   }
  } else if(!engine.configure(grid,{{true,true,false,false}}))throw std::runtime_error("Invalid grid");
  engine.prepare(w.rate);
  if(!irPath.empty()) {
   const auto ir=vc::loadCabinetIRWav(irPath,w.rate,vc::PortableAudioEngine::maxIrTaps);
   if(!engine.setCabinetIR(0,2,ir.taps.data(),ir.taps.size())) throw std::runtime_error("Cannot install cabinet IR");
   std::cout<<"Loaded cabinet IR: "<<ir.taps.size()<<" taps ("<<ir.sourceRate<<" -> "<<ir.targetRate<<" Hz)\n";
  }
  constexpr std::size_t framesPerChunk=256;
  for(std::size_t offset=0;offset<w.samples.size();offset+=framesPerChunk*w.channels){
   const auto count=std::min(w.samples.size()-offset,framesPerChunk*w.channels);
   if(!engine.process(w.samples.data()+offset,count/w.channels,w.channels))throw std::runtime_error("DSP processing failed");
  }
  writeWav(argv[2],w);
  std::cout<<"Rendered "<<w.samples.size()/w.channels<<" frames at "<<w.rate<<" Hz ("<<w.channels<<" channels)\n";
 }catch(const std::exception& e){std::cerr<<"Render error: "<<e.what()<<'\n';return 1;}
}
