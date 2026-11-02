#pragma once
#include <array>
#include <cstddef>
#include <cmath>
#include <mutex>
#include "SignalGrid.h"

namespace vc {
// Non-blocking audio-thread consumer, bounded storage, last-write-wins per slot.
// Producer may block briefly on the UI thread; never call submit from audio thread.
template<std::size_t MaxTaps>
class PendingIRQueue {
public:
 struct Item {
  std::array<float, MaxTaps> taps{};
  std::size_t count=0;
  bool pending=false;
 };
 bool submit(std::size_t row, std::size_t col, const float* taps, std::size_t count) {
  if (!SignalGrid::valid(row,col) || count>MaxTaps || (count && !taps)) return false;
  for (std::size_t i=0;i<count;++i) if (!std::isfinite(taps[i])) return false;
  std::lock_guard<std::mutex> guard(mutex_);
  auto& item=items_[row][col];
  for (std::size_t i=0;i<count;++i) item.taps[i]=taps[i];
  item.count=count;
  item.pending=true;
  return true;
 }
 // Consumer is invoked at audio block boundaries. Handles at most one slot
 // per call to bound work. Callback runs under a try-locked mutex and MUST
 // be noexcept, non-blocking, and allocation-free.
 template<class Callback> bool consumeOne(Callback&& callback) noexcept {
  if (!mutex_.try_lock()) return false;
  bool consumed=false;
  for (std::size_t i=0;i<SignalGrid::rows*SignalGrid::columns;++i) {
   const auto index=(next_+i)%(SignalGrid::rows*SignalGrid::columns);
   const auto row=index/SignalGrid::columns, col=index%SignalGrid::columns;
   auto& item=items_[row][col];
   if (!item.pending) continue;
   callback(row,col,item.taps.data(),item.count);
   item.pending=false;
   next_=(index+1)%(SignalGrid::rows*SignalGrid::columns);
   consumed=true;
   break;
  }
  mutex_.unlock();
  return consumed;
 }
private:
 std::mutex mutex_;
 std::array<std::array<Item,SignalGrid::columns>,SignalGrid::rows> items_{};
 std::size_t next_=0;
};
}
