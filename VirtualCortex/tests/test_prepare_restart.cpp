#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
int main() {
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Cabinet,true}));
 assert(grid.set(0,1,{vc::BlockType::Amp,true}));
 vc::PortableAudioEngine engine;
 assert(engine.configure(grid,{true,false,false,false}));
 engine.prepare(48000);
 float warm[4]={0.2f,0.2f,0.2f,0.2f};
 assert(engine.process(warm,4,1));
 const float impulse[1]={1.f};
 assert(engine.setCabinetIR(0,0,impulse,1));
 vc::NeuralAmpModel model;
 model.outputBias=0.125f;
 assert(engine.setNeuralAmp(0,1,&model));
 // Restart while both crossfades are in progress.
 engine.prepare(96000);
 float first[1]={0.5f};
 assert(engine.process(first,1,1));
 // The active IR is identity and the active neural model is constant.
 // If old fades leaked across prepare(), output would differ.
 assert(std::abs(first[0]-0.125f)<1e-5f);
 // Restart with neural disabled and IR removed: both selected states persist.
 assert(engine.setNeuralAmp(0,1,nullptr));
 assert(engine.setCabinetIR(0,0,nullptr,0));
 engine.prepare(44100);
 float x[1]={0.5f};
 assert(engine.process(x,1,1));
 assert(std::isfinite(x[0]));
 assert(x[0]>0.f);
}
