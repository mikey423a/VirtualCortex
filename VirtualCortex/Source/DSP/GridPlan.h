#pragma once
#include "SignalGrid.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace vc {
// Allocation-free immutable execution plan for a single grid row.
// A plan is prepared off the audio thread, then published at a safe boundary.
struct GridPlan {
 std::array<BlockType, SignalGrid::columns> blocks{};
 std::array<std::uint8_t, SignalGrid::columns> columns{};
 std::size_t count = 0;
 bool contains(BlockType t) const noexcept {
  for (std::size_t i=0;i<count;++i) if(blocks[i]==t) return true;
  return false;
 }
};
inline bool compileRow(const SignalGrid& grid, std::size_t row, GridPlan& out) noexcept {
 if (row >= SignalGrid::rows) return false;
 GridPlan result;
 for(std::size_t col=0;col<SignalGrid::columns;++col) {
  const auto* slot=grid.get(row,col);
  if(!slot || !SignalGrid::validType(slot->type)) return false;
  if(!slot->enabled || slot->type==BlockType::Empty) continue;
  result.columns[result.count]=static_cast<std::uint8_t>(col);
  result.blocks[result.count++]=slot->type;
 }
 out=result;
 return true;
}
// A compact, versioned grid snapshot. Explicit byte encoding avoids ABI padding.
using GridBytes=std::array<std::uint8_t, 2+SignalGrid::rows*SignalGrid::columns>;
inline GridBytes encodeGrid(const SignalGrid& grid) noexcept {
 GridBytes bytes{}; bytes[0]='V'; bytes[1]=1;
 for(std::size_t row=0;row<SignalGrid::rows;++row)
  for(std::size_t col=0;col<SignalGrid::columns;++col){
   const auto& s=*grid.get(row,col);
   if(static_cast<unsigned>(s.type)>15)bytes[1]=2;
   bytes[2+row*SignalGrid::columns+col]=static_cast<std::uint8_t>(s.type)|(s.enabled?0x80:0);
  }
 return bytes;
}
inline bool decodeGrid(const GridBytes& bytes, SignalGrid& out) noexcept {
 if(bytes[0]!='V' || (bytes[1]!=1 && bytes[1]!=2)) return false;
 SignalGrid next;
 for(std::size_t row=0;row<SignalGrid::rows;++row)
  for(std::size_t col=0;col<SignalGrid::columns;++col){
   auto raw=bytes[2+row*SignalGrid::columns+col];
   if(bytes[1]==1 && (raw&0x70)!=0) return false;
   BlockSlot slot{static_cast<BlockType>(raw&0x7f),(raw&0x80)!=0};
   if(!next.set(row,col,slot)) return false;
  }
 out=next; return true;
}
}
