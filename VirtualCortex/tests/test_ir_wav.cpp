#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "DSP/CabinetIRWav.h"
#include "DSP/GridIRLoader.h"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <string>
#include <cstdio>
static void w16(std::ostream& o,unsigned v){o.put(char(v));o.put(char(v>>8));}
static void w32(std::ostream& o,unsigned v){w16(o,v);w16(o,v>>16);}
static void write(const std::string& p,unsigned rate,unsigned channels,unsigned frames,unsigned bits=16){
 std::ofstream f(p,std::ios::binary);const unsigned bytes=frames*channels*(bits/8);
 f.write("RIFF",4);w32(f,36+bytes);f.write("WAVEfmt ",8);w32(f,16);w16(f,1);w16(f,channels);w32(f,rate);w32(f,rate*channels*(bits/8));w16(f,channels*(bits/8));w16(f,bits);f.write("data",4);w32(f,bytes);
 for(unsigned i=0;i<frames;++i)for(unsigned c=0;c<channels;++c){if(bits==16)w16(f,i==0?16384:0);else {const unsigned v=i==0?4194304:0;f.put(char(v));f.put(char(v>>8));f.put(char(v>>16));}}
}
static void writeExtensible(const std::string& p, unsigned subtype) {
 std::ofstream f(p,std::ios::binary);
 f.write("RIFF",4);w32(f,36+24+12);f.write("WAVEfmt ",8);w32(f,40);
 w16(f,0xfffe);w16(f,1);w32(f,48000);w32(f,144000);w16(f,3);w16(f,24);
 w16(f,22);w16(f,24);w32(f,0);
 w32(f,subtype);const unsigned char tail[]={0,0,16,0,128,0,0,170,0,56,155,113};
 f.write(reinterpret_cast<const char*>(tail),12);
 f.write("data",4);w32(f,12);
 for(int i=0;i<4;++i){unsigned v=i==0?4194304:0;f.put(char(v));f.put(char(v>>8));f.put(char(v>>16));}
}
int main(){
 const std::string p="test_cab_ir.wav";
 write(p,48000,2,4);
 auto a=vc::loadCabinetIRWav(p,48000);assert(a.taps.size()==4);assert(std::abs(a.taps[0]-0.5f)<0.001f);assert(a.taps[1]==0.f);
 auto b=vc::loadCabinetIRWav(p,96000);assert(b.taps.size()==8);assert(std::abs(b.taps[1]-0.25f)<0.001f);
 bool rejected=false;try{(void)vc::loadCabinetIRWav(p,96000,7);}catch(const std::runtime_error&){rejected=true;}assert(rejected);
 write(p,48000,2,4,24);auto c=vc::loadCabinetIRWav(p,48000);assert(c.taps.size()==4);assert(std::abs(c.taps[0]-0.5f)<0.001f);
 write(p,48000,1,4097);rejected=false;try{(void)vc::loadCabinetIRWav(p,48000);}catch(const std::runtime_error&){rejected=true;}assert(rejected);
 writeExtensible(p,1);auto ext=vc::loadCabinetIRWav(p,48000);assert(ext.taps.size()==4);assert(std::abs(ext.taps[0]-0.5f)<0.001f);
 writeExtensible(p,99);rejected=false;try{(void)vc::loadCabinetIRWav(p,48000);}catch(const std::runtime_error&){rejected=true;}assert(rejected);
 // End-to-end assignment of a 24-bit / 48 kHz WAV to a single grid slot.
 write(p,48000,1,4,24);
 vc::PortableAudioEngine engine;
 auto installed=vc::loadGridCabinetIR(engine,1,3,p,48000);
 assert(installed.ok && installed.taps==4 && installed.sourceRate==48000);
 assert(engine.cabinetIRLength(1,3)==4);
 assert(engine.cabinetIRLength(1,4)==0);
 auto badSlot=vc::loadGridCabinetIR(engine,4,3,p,48000);
 assert(!badSlot.ok && engine.cabinetIRLength(1,3)==4);
 auto missing=vc::loadGridCabinetIR(engine,1,3,"nonexistent-ir.wav",48000);
 assert(!missing.ok && engine.cabinetIRLength(1,3)==4);
 auto invalidRate=vc::loadGridCabinetIR(engine,1,3,p,0);
 assert(!invalidRate.ok && engine.cabinetIRLength(1,3)==4);
 std::remove(p.c_str());
}
