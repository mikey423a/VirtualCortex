#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
int main() {
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Cabinet,true}));
 vc::PortableAudioEngine reference, repeated;
 assert(reference.configure(grid,{true,false,false,false}));
 assert(repeated.configure(grid,{true,false,false,false}));
 reference.prepare(48000); repeated.prepare(48000);
 const float a[]={0.8f,0.2f};
 const float b[]={0.3f,0.4f};
 assert(reference.setCabinetIR(0,0,a,2));
 assert(repeated.setCabinetIR(0,0,a,2));
 for(int i=0;i<250;++i) {
  float x= i==0?0.5f:0.1f, y=x;
  if(i==80) {
   assert(reference.setCabinetIR(0,0,b,2));
   assert(repeated.setCabinetIR(0,0,b,2));
  }
  // Redundant GUI updates must not restart the crossfade.
  if(i>=80 && i<170) assert(repeated.setCabinetIR(0,0,b,2));
  assert(reference.process(&x,1,1));
  assert(repeated.process(&y,1,1));
  assert(std::abs(x-y)<1e-6f);
 }
 // Removing an IR repeatedly is also a no-op.
 assert(reference.setCabinetIR(0,0,nullptr,0));
 assert(repeated.setCabinetIR(0,0,nullptr,0));
 for(int i=0;i<150;++i) {
  float x=0.2f,y=x;
  assert(repeated.setCabinetIR(0,0,nullptr,0));
  assert(reference.process(&x,1,1));
  assert(repeated.process(&y,1,1));
  assert(std::abs(x-y)<1e-6f);
 }
}
