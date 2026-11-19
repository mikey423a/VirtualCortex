#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace vc {
struct RowRoute {
 // Bit 0 = input; bits 1..4 = outputs of rows 1..4. Only earlier rows allowed.
 // Entry 0..7 = slot A..H. Taps 0 = before A, 1..8 = after A..H.
 int entryColumn=0;std::array<int,4> sourceTaps{{8,8,8,8}};
 unsigned sources=1;int channel=0;float gainDb=0,balance=0;bool invert=false,output=true;
};
struct RowRouting {
 std::array<RowRoute,4> rows{};bool average=true;
 bool smoothSourceTaps=false;
 bool smoothRowEntries=false;
 bool smoothRoutingModes=false;
 bool smoothRowOutputs=false;
 static bool valid(const RowRouting& c)noexcept {
  for(unsigned r=0;r<4;++r){const auto& p=c.rows[r];if(p.sources&~((1u<<(r+1))-1u))return false;
   if(p.entryColumn<0||p.entryColumn>7)return false;for(auto tap:p.sourceTaps)if(tap<0||tap>8)return false;
   if(p.channel<0||p.channel>3||!std::isfinite(p.gainDb)||p.gainDb< -60||p.gainDb>12||!std::isfinite(p.balance)||p.balance< -1||p.balance>1)return false;
  }return true;
 }
};
}
