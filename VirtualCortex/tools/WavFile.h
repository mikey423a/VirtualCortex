#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
namespace vcwav {
static std::uint16_t u16(const unsigned char* p) { return std::uint16_t(p[0]) | (std::uint16_t(p[1])<<8); }
static std::uint32_t u32(const unsigned char* p) { return std::uint32_t(p[0]) | (std::uint32_t(p[1])<<8) | (std::uint32_t(p[2])<<16) | (std::uint32_t(p[3])<<24); }
static void w16(std::ostream& o,std::uint16_t x){ for(int i=0;i<2;++i)o.put(char(x>>(8*i))); }
static void w32(std::ostream& o,std::uint32_t x){ for(int i=0;i<4;++i)o.put(char(x>>(8*i))); }
struct Wav { std::uint32_t rate=0; std::uint16_t channels=0, bits=16, formatTag=1; std::vector<float> samples; };
static Wav readWav(const std::filesystem::path& path){
 std::ifstream f(path,std::ios::binary);if(!f)throw std::runtime_error("Cannot open input WAV");
 std::vector<unsigned char> b((std::istreambuf_iterator<char>(f)),{});
 if(b.size()<44 || std::string(reinterpret_cast<char*>(b.data()),4)!="RIFF" || std::string(reinterpret_cast<char*>(b.data()+8),4)!="WAVE")throw std::runtime_error("Not a RIFF/WAVE file");
 Wav w;bool format=false,data=false;std::size_t dataPos=0,dataSize=0;
 for(std::size_t p=12;p+8<=b.size();){
  const auto n=static_cast<std::size_t>(u32(b.data()+p+4));const auto start=p+8;
  if(n>b.size()-start)throw std::runtime_error("Truncated WAV chunk");
  const std::string id(reinterpret_cast<char*>(b.data()+p),4);
  if(id=="fmt "){
   if(n<16)throw std::runtime_error("Invalid WAV format chunk");
   auto formatTag=u16(b.data()+start);
   w.bits=u16(b.data()+start+14);
   if(formatTag==0xfffe){
    if(n<40 || u16(b.data()+start+16)<22 || u16(b.data()+start+18)!=w.bits)
     throw std::runtime_error("Invalid extensible PCM WAV format");
    const unsigned char expectedGuidTail[14]={0,0,0,0,16,0,128,0,0,170,0,56,155,113};
    if(!std::equal(expectedGuidTail,expectedGuidTail+14,b.data()+start+26) || (u16(b.data()+start+24)!=1 && u16(b.data()+start+24)!=3))
     throw std::runtime_error("Unsupported extensible WAV subtype");
    formatTag=u16(b.data()+start+24);
   }
   if(!((formatTag==1 && (w.bits==16 || w.bits==24)) || (formatTag==3 && w.bits==32)))
    throw std::runtime_error("Only PCM16/PCM24 or IEEE float32 WAV supported");
   w.formatTag=formatTag;
   w.channels=u16(b.data()+start+2);w.rate=u32(b.data()+start+4);
   if((w.channels!=1&&w.channels!=2)||w.rate<8000||w.rate>192000||u16(b.data()+start+12)!=w.channels*(w.bits/8))throw std::runtime_error("Unsupported WAV channels/rate/block alignment");
   format=true;
  } else if(id=="data") {dataPos=start;dataSize=n;data=true;}
  p=start+n+(n&1);
 }
 if(!format||!data||dataSize%(w.channels*(w.bits/8)))throw std::runtime_error("Missing or malformed WAV data");
 w.samples.resize(dataSize/(w.bits/8));
 for(std::size_t i=0;i<w.samples.size();++i){
  const auto* p=b.data()+dataPos+i*(w.bits/8);
  if(w.formatTag==3) {const std::uint32_t bits=u32(p);float x;std::memcpy(&x,&bits,sizeof(x));w.samples[i]=std::isfinite(x)?x:0.f;}
  else if(w.bits==16) w.samples[i]=static_cast<std::int16_t>(u16(p))/32768.f;
  else {std::int32_t v=std::int32_t(p[0])|(std::int32_t(p[1])<<8)|(std::int32_t(p[2])<<16);if(v&0x800000)v|=~0xffffff;w.samples[i]=float(v)/8388608.f;}
 }
 return w;
}
static void writeWav(const std::string& path,const Wav& w){
 if(w.samples.size()>0x7fffffffU/(w.bits/8))throw std::runtime_error("Output too large");
 std::ofstream f(path,std::ios::binary);if(!f)throw std::runtime_error("Cannot create output WAV");
 const auto bytes=static_cast<std::uint32_t>(w.samples.size()*(w.bits/8));
 f.write("RIFF",4);w32(f,36+bytes);f.write("WAVEfmt ",8);w32(f,16);w16(f,w.formatTag);w16(f,w.channels);w32(f,w.rate);w32(f,w.rate*w.channels*(w.bits/8));w16(f,w.channels*(w.bits/8));w16(f,w.bits);
 f.write("data",4);w32(f,bytes);
 for(float x:w.samples){const float safe=std::isfinite(x)?std::clamp(x,-1.f,1.f):0.f;if(w.formatTag==3){std::uint32_t bits;std::memcpy(&bits,&safe,sizeof(bits));w32(f,bits);}
  else if(w.bits==16){const auto v=static_cast<std::int16_t>(std::lround(safe*32767.f));w16(f,static_cast<std::uint16_t>(v));}
  else {const auto v=static_cast<std::int32_t>(std::lround(safe*8388607.f));for(int j=0;j<3;++j)f.put(char((static_cast<std::uint32_t>(v)>>(8*j))&255));}}
 if(!f)throw std::runtime_error("Failed writing WAV");
}
}
