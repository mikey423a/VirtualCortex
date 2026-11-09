#include "DSP/LiveAmp.h"
#include "DSP/PortableAudioEngine.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <new>
#include <stdexcept>
#if defined(_MSC_VER)
#include <malloc.h>
#endif
namespace probe {thread_local bool enabled=false;thread_local std::size_t allocations=0;}
void* operator new(std::size_t n){if(probe::enabled)++probe::allocations;if(auto* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
void* operator new(std::size_t n,std::align_val_t a){if(probe::enabled)++probe::allocations;
#if defined(_MSC_VER)
 if(auto* p=_aligned_malloc(n?n:1,(std::size_t)a))return p;
#else
 void* p=nullptr;if(posix_memalign(&p,(std::size_t)a,n?n:1)==0)return p;
#endif
 throw std::bad_alloc();}
void* operator new[](std::size_t n,std::align_val_t a){return ::operator new(n,a);}
void operator delete(void* p,std::align_val_t)noexcept{
#if defined(_MSC_VER)
 _aligned_free(p);
#else
 std::free(p);
#endif
}
void operator delete[](void* p,std::align_val_t a)noexcept{::operator delete(p,a);}
void operator delete(void* p,std::size_t,std::align_val_t a)noexcept{::operator delete(p,a);}
void operator delete[](void* p,std::size_t,std::align_val_t a)noexcept{::operator delete(p,a);}
static void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(int argc,char** argv){try{
 if(argc>1){
  auto model=vc::loadLiveNamAmp(argv[1],48000,true);const double rate=model->expectedRate()>0?model->expectedRate():48000;
  model->prepare(rate);check(model->status()==vc::LiveAmpStatus::Ready,"Upstream model not ready");
  double best=1000;float peak=0;
  for(int pass=0;pass<3;++pass){
   const auto start=std::chrono::steady_clock::now();probe::allocations=0;probe::enabled=true;
   for(int i=0;i<(int)rate;++i){float x=.01f*std::sin(float(i*.028));peak=std::max(peak,std::abs(model->process(0,x)));model->process(1,-x);}
   probe::enabled=false;best=std::min(best,std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count());
   check(probe::allocations==0,"Prepared model used C++ heap allocation while processing");
   check(model->status()==vc::LiveAmpStatus::Ready,"Upstream model faulted");
  }
  std::cout<<"NAM stereo one-sample processing: "<<best*1000<<" ms per second of audio, peak="<<peak<<"\n";
  check(best<1.,"Reference model slower than real-time on this runner");return 0;
 }
 const auto root=std::filesystem::temp_directory_path()/("vc-live-nam-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
 check(std::filesystem::create_directory(root),"Cannot create fixture directory");
 struct Cleanup{std::filesystem::path p;~Cleanup(){std::error_code e;std::filesystem::remove_all(p,e);}} cleanup{root};
 const auto path=root/"linear.nam";
 {std::ofstream file(path);file<<R"({"version":"0.5.4","architecture":"Linear","config":{"receptive_field":2,"bias":false},"weights":[0.5,0.25],"sample_rate":48000,"metadata":{}})";}
 auto model=vc::loadLiveNamAmp(path.string(),48000);float previous=0;
 for(int i=0;i<1025;++i){const float x=.2f*std::sin(i*.1f);probe::allocations=0;probe::enabled=true;const float y=model->process(0,x),right=model->process(1,0);probe::enabled=false;
  check(probe::allocations==0,"Linear live model allocated");check(std::abs(y-(.5f*x+.25f*previous))<2.e-6f,"Live NAM differs from independent FIR oracle or adds latency");check(right==0,"NAM stereo crosstalk");previous=x;
 }
 model->prepare(44100);check(model->status()==vc::LiveAmpStatus::RateMismatch&&model->process(0,.1f)==0,"Rate mismatch did not mute");
 model->prepare(48000);check(model->status()==vc::LiveAmpStatus::Ready,"Matching rate did not recover");
 vc::PortableAudioEngine engine;engine.prepare(48000);vc::SignalGrid grid;grid.set(0,0,{vc::BlockType::Amp,true});engine.configure(grid,{true,false,false,false});
 engine.setLiveAmp(0,0,std::move(model));vc::SlotParameters settings;settings.drive=settings.mix=.5f;engine.setSlotParameters(0,0,settings);
 float frame[2]{.2f,0};engine.process(frame,1,2);check(std::abs(frame[0]-.1f)<1.e-6f,"NAM not integrated into graph");
 engine.swapSlots(0,0,0,7);grid.swap(0,0,0,7);engine.configure(grid,{true,false,false,false});frame[0]=frame[1]=0;engine.process(frame,1,2);check(std::abs(frame[0]-.05f)<1.e-6f,"Move lost NAM history");
 std::cout<<"PASS: actual live NAM FIR oracle, channel isolation, no adapter latency, movement and sample-rate recovery\n";
}catch(const std::exception& e){probe::enabled=false;std::cerr<<e.what()<<'\n';return 1;}}
