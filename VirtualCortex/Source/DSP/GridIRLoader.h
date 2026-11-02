#pragma once
// Control-thread-only cabinet IR file assignment. Call while audio is stopped.
#include "CabinetIRWav.h"
#include "PortableAudioEngine.h"
#include <string>

namespace vc {
struct GridIRLoadResult {
 bool ok = false;
 std::string message;
 std::size_t taps = 0;
 unsigned sourceRate = 0;
};
inline GridIRLoadResult loadGridCabinetIR(PortableAudioEngine& engine,
    std::size_t row, std::size_t col, const std::string& path, unsigned sessionRate) {
 if (!SignalGrid::valid(row,col)) return {false,"Invalid grid slot",0,0};
 try {
  auto ir=loadCabinetIRWav(path,sessionRate,PortableAudioEngine::maxIrTaps);
  if (!engine.setCabinetIR(row,col,ir.taps.data(),ir.taps.size()))
   return {false,"Unable to install cabinet IR",0,ir.sourceRate};
  return {true,"Loaded cabinet IR",ir.taps.size(),ir.sourceRate};
 } catch(const std::exception& e) { return {false,e.what(),0,0}; }
}
}
