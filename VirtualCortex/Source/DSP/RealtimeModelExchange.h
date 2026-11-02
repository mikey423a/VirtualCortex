#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <memory>

namespace vc {
// Ownership bridge for dynamically allocated DSP engines (including NAM).
// Exactly one loader/control thread calls submit()/reclaim(). Exactly one audio
// thread calls advance()/get(). Stop BOTH threads before destroying this object.
// Prepare/prewarm models before submit. The audio thread never destroys models.
// This is transport only: sample-rate validity, fades and DSP safety are caller duties.
template<class Model, std::size_t Capacity = 4>
class RealtimeModelExchange {
 static_assert(Capacity >= 2, "Need at least two queue positions");
 static_assert(std::atomic<std::size_t>::is_always_lock_free,
               "Audio ownership queues require lock-free indices");
 struct Queue {
  std::array<Model*, Capacity> values{};
  std::atomic<std::size_t> head{0}, tail{0};
  bool full() const noexcept {
   return (head.load(std::memory_order_relaxed)+1)%Capacity == tail.load(std::memory_order_acquire);
  }
  bool push(Model* value) noexcept {
   const auto h=head.load(std::memory_order_relaxed);
   const auto next=(h+1)%Capacity;
   if(next==tail.load(std::memory_order_acquire)) return false;
   values[h]=value; head.store(next,std::memory_order_release); return true;
  }
  bool pop(Model*& value) noexcept {
   const auto t=tail.load(std::memory_order_relaxed);
   if(t==head.load(std::memory_order_acquire)) return false;
   value=values[t];tail.store((t+1)%Capacity,std::memory_order_release);return true;
  }
 } pending, retired;
 Model* active=nullptr;
public:
 RealtimeModelExchange()=default;
 RealtimeModelExchange(const RealtimeModelExchange&)=delete;
 RealtimeModelExchange& operator=(const RealtimeModelExchange&)=delete;
 ~RealtimeModelExchange() {
  // Shutdown only, after callbacks and loader have stopped.
  reclaim(); Model* value=nullptr;
  while(pending.pop(value)) delete value;
  delete active;
 }
 // Loader thread: false retains caller ownership; null is an explicit unload.
 bool submit(std::unique_ptr<Model>& model) noexcept {
  if(!pending.push(model.get())) return false;
  model.release(); return true;
 }
 // Loader thread: run periodically even when no new files are being loaded.
 void reclaim() noexcept {
  Model* value=nullptr;while(retired.pop(value)) delete value;
 }
 // Audio block boundary. Backpressure keeps the current model playing unchanged.
 // Any pointer from get() expires on the next successful advance().
 bool advance() noexcept {
  if(active && retired.full()) return false;
  Model* next=nullptr;
  if(!pending.pop(next)) return false;
  if(active) retired.push(active); // sole producer; consumer can only free space
  active=next;return true;
 }
 Model* get() const noexcept { return active; } // audio thread only
};
}
