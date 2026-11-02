#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include <cassert>
#include <cmath>
#include <vector>
int main() {
 vc::PortableAudioEngine engine;
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Cabinet,true}));
 assert(engine.configure(grid,{true,false,false,false}));
 engine.prepare(48000);
 const float plus[]={1.f}, minus[]={-1.f};
 assert(engine.setCabinetIR(0,0,plus,1));
 std::vector<float> audio(256,0.5f);
 assert(engine.process(audio.data(),audio.size(),1));
 for(float v:audio) assert(std::isfinite(v));
 assert(engine.setCabinetIR(0,0,minus,1));
 std::vector<float> change(128,0.5f);
 assert(engine.process(change.data(),change.size(),1));
 assert(change.front()>0.45f); // starts close to old response
 assert(change[63]<0.02f && change[63]>-0.02f); // midpoint
 assert(change.back()<-0.49f); // reaches new response
 std::vector<float> steady(16,0.5f);
 assert(engine.process(steady.data(),steady.size(),1));
 for(float v:steady) assert(std::abs(v+0.5f)<1e-5f);
 // Adding the first IR must crossfade from the fallback cabinet filter.
 vc::PortableAudioEngine fresh;
 assert(fresh.configure(grid,{true,false,false,false}));
 fresh.prepare(48000);
 std::vector<float> warm(256,0.5f);
 assert(fresh.process(warm.data(),warm.size(),1));
 assert(fresh.setCabinetIR(0,0,minus,1));
 std::vector<float> first(128,0.5f);
 assert(fresh.process(first.data(),first.size(),1));
 assert(first.front()>0.3f); // close to fallback, not immediate negative IR
 assert(first.back()<-0.49f);
 // Clearing IR also fades rather than discontinuously replacing the FIR.
 assert(engine.setCabinetIR(0,0,nullptr,0));
 std::vector<float> clear(128,0.5f);
 assert(engine.process(clear.data(),clear.size(),1));
 assert(clear.front()<-0.45f);
 assert(clear.back()>0.4f);
}
