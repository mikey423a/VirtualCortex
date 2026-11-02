#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <limits>
int main() {
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Cabinet,true}));
 assert(grid.set(0,1,{vc::BlockType::Delay,true}));
 vc::PortableAudioEngine engine;
 assert(engine.configure(grid,{true,false,false,false}));
 engine.prepare(std::numeric_limits<double>::quiet_NaN());
 float signal[8]={0.2f,std::numeric_limits<float>::quiet_NaN(),0.2f,
  std::numeric_limits<float>::infinity(),0.2f,-std::numeric_limits<float>::infinity(),0.2f,0.2f};
 assert(engine.process(signal,8,1));
 for(float x:signal) assert(std::isfinite(x));
 float following[256]; for(float& x:following)x=0.2f;
 assert(engine.process(following,256,1));
 for(float x:following) assert(std::isfinite(x));
}
