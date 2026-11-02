#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/RealtimeModelExchange.h"
#include <cassert>
#include <cstdlib>
#include <new>
#include <thread>

static thread_local bool audio=false;
static std::atomic<int> alive{0}, destroyed{0};
void* operator new(std::size_t n) { assert(!audio);if(auto* p=std::malloc(n?n:1))return p;throw std::bad_alloc(); }
void operator delete(void* p) noexcept { assert(!audio);std::free(p); }
void operator delete(void* p,std::size_t) noexcept { assert(!audio);std::free(p); }
struct Model {
 int sequence;
 explicit Model(int n):sequence(n){++alive;}
 ~Model(){assert(!audio);--alive;++destroyed;}
};
int main(){
 {
  vc::RealtimeModelExchange<Model,2> q;
  auto a=std::make_unique<Model>(1),b=std::make_unique<Model>(2);
  assert(q.submit(a)&&!a);assert(!q.submit(b)&&b);
  audio=true;assert(q.advance()&&q.get()->sequence==1);audio=false;
  assert(q.submit(b)&&!b);
  audio=true;assert(q.advance()&&q.get()->sequence==2);audio=false;
  std::unique_ptr<Model> unload;
  assert(q.submit(unload));
  audio=true;assert(!q.advance()&&q.get()->sequence==2);audio=false;
  q.reclaim();assert(alive==1);
  audio=true;assert(q.advance()&&!q.get());assert(!q.advance());audio=false;
  q.reclaim();assert(alive==0);
  auto c=std::make_unique<Model>(3);assert(q.submit(c)); // shutdown with pending
 }
 assert(alive==0&&destroyed==3);
 {
  vc::RealtimeModelExchange<Model,8> q;
  constexpr int count=100000;
  std::atomic<bool> complete{false};
  std::thread consumer([&]{
   audio=true;int last=0;
   while(last<count){
    if(q.advance()){assert(q.get()&&q.get()->sequence==last+1);last=q.get()->sequence;}
   }
   audio=false;complete.store(true,std::memory_order_release);
  });
  for(int i=1;i<=count;++i){
   auto model=std::make_unique<Model>(i);
   while(!q.submit(model))q.reclaim();
   q.reclaim();
  }
  while(!complete.load(std::memory_order_acquire))q.reclaim();
  consumer.join();q.reclaim();assert(alive==1);
 }
 assert(alive==0&&destroyed==100003);
}
