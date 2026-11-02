#pragma once
// Offline/control-thread WAV impulse-response decoder. No JUCE dependency.
// PCM16/PCM24 and IEEE float32 mono/stereo, RIFF/WAVE, with linear resampling.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace vc {
struct LoadedCabinetIR { std::vector<float> taps; unsigned sourceRate=0, targetRate=0; };
inline std::uint16_t irU16(const unsigned char* p) { return std::uint16_t(p[0]) | (std::uint16_t(p[1])<<8); }
inline std::uint32_t irU32(const unsigned char* p) { return std::uint32_t(p[0]) | (std::uint32_t(p[1])<<8) | (std::uint32_t(p[2])<<16) | (std::uint32_t(p[3])<<24); }
inline LoadedCabinetIR loadCabinetIRWav(const std::string& path, unsigned targetRate, std::size_t maxTaps=4096) {
 if(targetRate<8000 || targetRate>192000 || maxTaps==0) throw std::runtime_error("Invalid IR target sample rate or capacity");
 std::ifstream f(path,std::ios::binary);
 if(!f) throw std::runtime_error("Cannot open cabinet IR WAV");
 std::vector<unsigned char> b((std::istreambuf_iterator<char>(f)),{});
 if(b.size()<12 || std::memcmp(b.data(),"RIFF",4) || std::memcmp(b.data()+8,"WAVE",4)) throw std::runtime_error("Invalid RIFF/WAVE IR");
 unsigned format=0,channels=0,rate=0,bits=0,alignment=0,validBits=0;
 std::size_t dataPos=0,dataSize=0; bool fmtFound=false,dataFound=false;
 for(std::size_t p=12;p+8<=b.size();) {
  const std::size_t n=irU32(b.data()+p+4),start=p+8;
  if(n>b.size()-start) throw std::runtime_error("Truncated IR WAV chunk");
  if(!std::memcmp(b.data()+p,"fmt ",4)) {
   if(n<16) throw std::runtime_error("Malformed IR format");
   format=irU16(b.data()+start);channels=irU16(b.data()+start+2);rate=irU32(b.data()+start+4);
   alignment=irU16(b.data()+start+12);bits=irU16(b.data()+start+14);validBits=bits;
   // WAVE_FORMAT_EXTENSIBLE: support standard PCM and IEEE_FLOAT subformats.
   if(format==0xfffe) {
    if(n<40 || irU16(b.data()+start+16)<22) throw std::runtime_error("Malformed extensible IR format");
    validBits=irU16(b.data()+start+18);
    static constexpr unsigned char tail[12]={0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71};
    const auto* guid=b.data()+start+24;
    if(guid[2]!=0 || guid[3]!=0 || std::memcmp(guid+4,tail,12)!=0) throw std::runtime_error("Unsupported extensible IR subtype");
    format=irU16(guid);
   }
   fmtFound=true;
  } else if(!std::memcmp(b.data()+p,"data",4)) { dataPos=start;dataSize=n;dataFound=true; }
  p=start+n; if((n&1) && p<b.size()) ++p;
 }
 if(!fmtFound || !dataFound || (channels!=1 && channels!=2) || rate<8000 || rate>192000 ||
    !((format==1 && (bits==16 || bits==24))||(format==3 && bits==32)) || alignment!=channels*(bits/8) || validBits!=bits ||
    dataSize==0 || dataSize%alignment) throw std::runtime_error("Unsupported IR WAV format");
 const std::size_t frames=dataSize/alignment;
 if(frames>10000000) throw std::runtime_error("IR file exceeds safety frame limit");
 // Prevent silent truncation: a source IR longer than capacity after resampling is rejected.
 const auto outputFrames=static_cast<std::size_t>(std::ceil(static_cast<double>(frames)*targetRate/rate));
 if(outputFrames==0 || outputFrames>maxTaps) throw std::runtime_error("IR too long for current FIR engine (maximum taps configured by caller)");
 std::vector<float> mono(frames);
 for(std::size_t i=0;i<frames;++i) {
  double sum=0;
  for(unsigned c=0;c<channels;++c) {
   const auto* p=b.data()+dataPos+i*alignment+c*(bits/8);
   float x=0;
   if(format==1 && bits==16) { const auto raw=irU16(p); x=static_cast<std::int16_t>(raw)/32768.f; }
   else if(format==1 && bits==24) { std::int32_t raw=std::int32_t(p[0]) | (std::int32_t(p[1])<<8) | (std::int32_t(p[2])<<16); if(raw & 0x800000) raw |= ~0xffffff; x=static_cast<float>(raw)/8388608.f; }
   else { const auto raw=irU32(p); std::memcpy(&x,&raw,sizeof(x)); }
   if(!std::isfinite(x)) throw std::runtime_error("IR contains nonfinite samples");
   sum+=x;
  }
  mono[i]=static_cast<float>(sum/channels);
 }
 LoadedCabinetIR out;out.sourceRate=rate;out.targetRate=targetRate;out.taps.resize(outputFrames);
 for(std::size_t i=0;i<outputFrames;++i) {
  const double pos=static_cast<double>(i)*rate/targetRate;
  const auto lo=std::min(static_cast<std::size_t>(pos),frames-1);
  const auto hi=std::min(lo+1,frames-1);
  const float t=static_cast<float>(std::clamp(pos-static_cast<double>(lo),0.0,1.0));
  out.taps[i]=mono[lo]+t*(mono[hi]-mono[lo]);
 }
 return out;
}
}
