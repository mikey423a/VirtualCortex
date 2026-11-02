#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <cstdio>
int main(){
 vc::NeuralAmpModel model;
 // A known model: one tanh neuron driven by the current sample.
 model.weights[0][0]=1.f; model.outputWeights[0]=1.f;
 assert(model.valid());
 vc::PortableAudioEngine engine;
 vc::SignalGrid grid; assert(grid.set(0,0,{vc::BlockType::Amp,true}));
 assert(engine.configure(grid,{true,false,false,false}));
 engine.prepare(48000);
 assert(engine.setNeuralAmp(0,0,&model));
 assert(engine.neuralAmpEnabled(0,0));
 float samples[]={0.5f,-0.5f,0.f};
 assert(engine.process(samples,3,1));
 assert(std::abs(samples[0]-std::tanh(0.5f))<1e-6f);
 assert(std::abs(samples[1]-std::tanh(-0.5f))<1e-6f);
 assert(std::abs(samples[2])<1e-6f);
 assert(!engine.setNeuralAmp(4,0,&model));
 auto bad=model;bad.outputBias=INFINITY;
 assert(!engine.setNeuralAmp(0,0,&bad));
 assert(engine.neuralAmpEnabled(0,0));
 const char* filename="vc_neural_test.txt";
 {std::ofstream out(filename);out<<"VCMLP1\n";
  for(auto& row:model.weights) for(float w:row) out<<w<<" ";
  for(float b:model.bias)out<<b<<" ";
  for(float w:model.outputWeights)out<<w<<" ";
  out<<model.outputBias<<"\n";}
 auto loaded=vc::loadNeuralAmpModel(filename);
 assert(loaded.weights[0][0]==1.f && loaded.outputWeights[0]==1.f);
 std::remove(filename);
 assert(engine.setNeuralAmp(0,0,nullptr));
 assert(!engine.neuralAmpEnabled(0,0));
 float dry=0.5f;assert(engine.process(&dry,1,1));
 assert(std::abs(dry-std::tanh(0.5f))<0.01f);
 for(int i=0;i<128;++i){float x=0.5f;assert(engine.process(&x,1,1));dry=x;}
 assert(std::abs(dry-std::tanh(8.5f*0.5f)/std::tanh(8.5f))<1e-6f);
}
