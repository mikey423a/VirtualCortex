#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace vc {
struct RowRoute {
 // Bit 0 = input; bits 1..4 = outputs of rows 1..4. Only earlier rows allowed.
 unsigned sources=1;int channel=0;float gainDb=0,balance=0;bool invert=false,output=true;
};
struct RowRouting {
 std::array<RowRoute,4> rows{};bool average=true;
 static bool valid(const RowRouting& c)noexcept {
  for(unsigned r=0;r<4;++r){const auto& p=c.rows[r];if(p.sources&~((1u<<(r+1))-1u))return false;
   if(p.channel<0||p.channel>3||!std::isfinite(p.gainDb)||p.gainDb< -60||p.gainDb>12||!std::isfinite(p.balance)||p.balance< -1||p.balance>1)return false;
  }return true;
 }
};
}
