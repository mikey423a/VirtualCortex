#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/PortableAudioEngine.h"
#include "DSP/CabinetIRWav.h"
#include <cassert>
#include <cmath>
#include <vector>
#include <fstream>
#include <cstdint>
#include <cstdio>

static void u16(std::ofstream& f, unsigned n) { char b[2]={char(n),char(n>>8)};f.write(b,2); }
static void u32(std::ofstream& f, unsigned n) { char b[4]={char(n),char(n>>8),char(n>>16),char(n>>24)};f.write(b,4); }
int main() {
 constexpr std::size_t n=vc::PortableAudioEngine::maxIrTaps;
 static_assert(n==4096,"long IR capacity");
 vc::PortableAudioEngine e;
 vc::SignalGrid grid;
 assert(grid.set(0,0,{vc::BlockType::Cabinet,true}));
 assert(e.configure(grid,{true,false,false,false}));
 e.prepare(48000);
 std::vector<float> ir(n,0.f); ir[n-1]=0.5f;
 assert(e.setCabinetIR(0,0,ir.data(),ir.size()));
 assert(e.cabinetIRLength(0,0)==n);
 std::vector<float> impulse(n+256,0.f); impulse[0]=1.f;
 assert(e.process(impulse.data(),impulse.size(),1));
 assert(std::abs(impulse[n-1]-0.5f)<0.00001f);
 assert(std::abs(impulse[n-2])<0.00001f);
 assert(std::abs(impulse[n])<0.00001f);
 assert(!e.setCabinetIR(0,0,ir.data(),n+1));
 assert(e.cabinetIRLength(0,0)==n);
 // Validate the actual WAV loader at the expanded capacity.
 const char* name="vc_long_ir_test.wav";
 {
  std::ofstream f(name,std::ios::binary); assert(f);
  f.write("RIFF",4);u32(f,36+unsigned(n*2));f.write("WAVEfmt ",8);
  u32(f,16);u16(f,1);u16(f,1);u32(f,48000);u32(f,96000);u16(f,2);u16(f,16);
  f.write("data",4);u32(f,unsigned(n*2));
  for(std::size_t i=0;i<n;++i)u16(f,i==n-1?16384:0);
 }
 auto loaded=vc::loadCabinetIRWav(name,48000,n);
 assert(loaded.taps.size()==n);
 assert(std::abs(loaded.taps.back()-0.5f)<0.0001f);
 bool rejected=false;
 try { (void)vc::loadCabinetIRWav(name,48000,1024); }
 catch(const std::exception&) { rejected=true; }
 assert(rejected);
 std::remove(name);
}
