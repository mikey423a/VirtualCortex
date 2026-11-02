#pragma once
#include "DSP/PortableAudioEngine.h"
#include <array>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace vc {
struct OfflineGridConfig {
 SignalGrid grid;
 std::array<bool,4> lanes{{false,false,false,false}};
 std::array<std::array<SlotParameters,8>,4> params{};
 std::array<std::array<std::string,8>,4> neuralPaths{};
};
inline BlockType parseBlockName(const std::string& name) {
 if(name=="Gate") return BlockType::Gate;
 if(name=="Amp") return BlockType::Amp;
 if(name=="Cabinet") return BlockType::Cabinet;
 if(name=="EQ") return BlockType::EQ;
 if(name=="Delay") return BlockType::Delay;
 if(name=="Reverb") return BlockType::Reverb;
 if(name=="Tremolo") return BlockType::Tremolo;
 if(name=="Compressor") return BlockType::Compressor;
 if(name=="Overdrive") return BlockType::Overdrive;
 if(name=="Distortion") return BlockType::Distortion;
 if(name=="Chorus") return BlockType::Chorus;
 if(name=="Flanger") return BlockType::Flanger;
 if(name=="Phaser") return BlockType::Phaser;
 if(name=="Wah") return BlockType::Wah;
 if(name=="BitCrusher") return BlockType::BitCrusher;
 if(name=="Vibrato") return BlockType::Vibrato;
 if(name=="RingModulator") return BlockType::RingModulator;
 if(name=="Gain") return BlockType::Gain;
 if(name=="LowHighCut") return BlockType::LowHighCut;
 if(name=="ToneEQ") return BlockType::ToneEQ;
 if(name=="RoomReverb") return BlockType::RoomReverb;
 if(name=="Empty") return BlockType::Empty;
 throw std::runtime_error("Unknown effect: "+name);
}
// Text format: lane <0-3> <on|off>; slot <row> <column> <effect> <on|off>;
// param <row> <column> <drive|mix|time|feedback|rate|depth> <0..1>.
// repeatFx is an optional param accepting 0 or 1.
// Comments start with '#'. Parsing is strict and transactional.
inline OfflineGridConfig loadOfflineGridConfig(const std::string& path) {
 std::ifstream in(path);
 if(!in) throw std::runtime_error("Cannot open grid configuration: "+path);
 OfflineGridConfig result;
 std::string line;
 std::size_t lineNumber=0;
 auto fail=[&](const std::string& msg) -> void { throw std::runtime_error("Grid config line "+std::to_string(lineNumber)+": "+msg); };
 while(std::getline(in,line)) {
  ++lineNumber;
  const auto comment=line.find('#');
  if(comment!=std::string::npos) line.erase(comment);
  std::istringstream stream(line);
  std::string command;
  if(!(stream>>command)) continue;
  std::size_t row=0,col=0;
  std::string arg,extra;
  if(command=="lane") {
   if(!(stream>>row>>arg) || (stream>>extra) || row>=4 || (arg!="on"&&arg!="off")) fail("expected lane ROW on|off");
   result.lanes[row]=(arg=="on");
  } else if(command=="slot") {
   std::string state;
   if(!(stream>>row>>col>>arg>>state) || (stream>>extra) || !SignalGrid::valid(row,col) || (state!="on"&&state!="off")) fail("expected slot ROW COL EFFECT on|off");
   try { result.grid.set(row,col,{parseBlockName(arg),state=="on"}); }
   catch(const std::exception& e) { fail(e.what()); }
  } else if(command=="neural") {
   if(!(stream>>row>>col>>arg) || (stream>>extra) || !SignalGrid::valid(row,col)) fail("expected neural ROW COL MODEL_PATH");
   const auto modelPath=std::filesystem::path(arg);
   result.neuralPaths[row][col]=(modelPath.is_absolute()?modelPath:std::filesystem::absolute(std::filesystem::path(path)).parent_path()/modelPath).lexically_normal().string();
  } else if(command=="param") {
   float value=0.f;
   if(!(stream>>row>>col>>arg>>value) || (stream>>extra) || !SignalGrid::valid(row,col) || !std::isfinite(value) || value<0.f || value>1.f) fail("expected param ROW COL NAME VALUE (0..1)");
   auto& p=result.params[row][col];
   if(arg=="drive") p.drive=value;
   else if(arg=="mix") p.mix=value;
   else if(arg=="time") p.time=value;
   else if(arg=="feedback") p.feedback=value;
   else if(arg=="rate") p.rate=value;
   else if(arg=="depth") p.depth=value;
   else if(arg=="repeatFx" && (value==0.f||value==1.f)) p.repeatFx=value==1.f;
   else fail("unknown parameter: "+arg);
  } else fail("unknown command: "+command);
 }
 if(!in.eof() && in.fail()) throw std::runtime_error("Error reading grid configuration");
 for(std::size_t r=0;r<4;++r)for(std::size_t c=0;c<8;++c)
  if(!result.neuralPaths[r][c].empty() && (result.grid.get(r,c)->type!=BlockType::Amp || !result.grid.get(r,c)->enabled))
   throw std::runtime_error("Neural model requires an enabled Amp block at row "+std::to_string(r)+" column "+std::to_string(c));
 return result;
}
}
