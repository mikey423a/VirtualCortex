#pragma once
#include "NeuralAmp.h"
#include "SignalGrid.h"
#include <array>
#include <mutex>
namespace vc {
// UI producer, audio-thread try-lock consumer. Latest update wins per slot.
// Model data is copied only under the mutex; audio callback never waits.
class PendingNeuralQueue {
 struct Item { NeuralAmpModel model{}; bool enabled=false, pending=false; };
 std::mutex mutex_;
 std::array<std::array<Item,SignalGrid::columns>,SignalGrid::rows> slots_{};
 std::size_t next_=0;
public:
 bool submit(std::size_t row,std::size_t col,const NeuralAmpModel* model) {
  if(!SignalGrid::valid(row,col) || (model && !model->valid())) return false;
  std::lock_guard<std::mutex> lock(mutex_);
  auto& item=slots_[row][col];
  if(model) item.model=*model;
  item.enabled=model!=nullptr;item.pending=true;return true;
 }
 template<class Callback> bool consumeOne(Callback&& callback) noexcept {
  if(!mutex_.try_lock()) return false;
  bool result=false;
  for(std::size_t i=0;i<SignalGrid::rows*SignalGrid::columns;++i) {
   const auto index=(next_+i)%(SignalGrid::rows*SignalGrid::columns);
   auto& item=slots_[index/SignalGrid::columns][index%SignalGrid::columns];
   if(!item.pending) continue;
   callback(index/SignalGrid::columns,index%SignalGrid::columns,item.enabled?&item.model:nullptr);
   item.pending=false;next_=(index+1)%(SignalGrid::rows*SignalGrid::columns);
   result=true;break;
  }
  mutex_.unlock();return result;
 }
};
}
