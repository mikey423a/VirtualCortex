// Offline VCMLP1 inference verifier. NOT a NAM loader.
#include "DSP/NeuralAmp.h"
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
int main(int argc,char** argv){
 if(argc!=3){std::cerr<<"Usage: virtualcortex_neural_verify model.vcmlp input_samples.txt\n";return 2;}
 try {
  const auto model=vc::loadNeuralAmpModel(argv[1]);
  std::ifstream input(argv[2]);
  if(!input)throw std::runtime_error("Cannot open input samples");
  std::array<float,vc::NeuralAmpModel::inputs> history{};
  std::size_t index=0;
  std::cout<<std::setprecision(10);
  float sample=0;
  while(input>>sample){
   if(!std::isfinite(sample))throw std::runtime_error("Nonfinite input sample");
   history[index]=sample;
   std::cout<<model.infer(history,index)<<'\n';
   index=(index+1)%history.size();
  }
  if(!input.eof())throw std::runtime_error("Malformed input samples");
  return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
