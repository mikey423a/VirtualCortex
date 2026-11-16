#include "DSP/PortableAudioEngine.h"
#include <iostream>
#include <stdexcept>
static void check(bool b,const char* m){if(!b)throw std::runtime_error(m);}
int main(){try{
 for(double sampleRate:{44100.,48000.,96000.}) for(auto type:{vc::BlockType::GridStereoDelay,vc::BlockType::GridStereoReverb}){
  auto routed=std::make_unique<vc::PortableAudioEngine>();
  auto reference=std::make_unique<vc::PortableAudioEngine>();
  vc::SignalGrid grid;grid.set(0,0,{type,true});vc::RowRouting routes;
  for(auto* e:{routed.get(),reference.get()}){
   e->prepare(sampleRate,64);
   if(type==vc::BlockType::GridStereoDelay)e->prepareStereoSlot(0,0);else e->prepareReverbSlot(0,0);
   check(e->configure(grid,{true,false,false,false}),"configure");
   check(e->setRowRouting(routes),"routes");
   vc::SlotParameters p;p.mix=1;p.time=9.f/3999;p.rate=19.f/3999;p.feedback=.5f;p.drive=.2f;p.depth=.5f;
   check(e->setSlotParameters(0,0,p),"parameters");
  }
  auto tick=[](vc::PortableAudioEngine& e,float l,float r){std::array<float,2> f{l,r};check(e.process(f.data(),1,2,vc::ParallelGrid::Mode::Custom),"process");return f;};
  auto compare=[](auto a,auto b){for(int c=0;c<2;++c)check(std::isfinite(a[c])&&std::abs(a[c]-b[c])<1e-6f,"Routing transition changed effect state");};
  for(int i=0;i<6000;++i)compare(tick(*routed,0,0),tick(*reference,0,0));
  compare(tick(*routed,.25f,-.125f),tick(*reference,.25f,-.125f));
  routes.rows[0].sources=0;check(routed->setRowRouting(routes),"disconnect");
  double tail=0;
  for(int i=0;i<2400;++i){auto a=tick(*routed,.7f,-.6f);compare(a,tick(*reference,0,0));tail+=std::abs(a[0])+std::abs(a[1]);}
  check(tail>.001,"Disconnected input lost tail");
  routes.rows[0].entryColumn=1;check(routed->setRowRouting(routes),"skip");
  for(int i=0;i<777;++i)check(tick(*routed,.7f,-.6f)==std::array<float,2>{0,0},"Skipped disconnected slot not silent");
  routes.rows[0].entryColumn=0;check(routed->setRowRouting(routes),"restore");
  double restoredTail=0;
  for(int i=0;i<2400;++i){auto a=tick(*routed,.7f,-.6f);compare(a,tick(*reference,0,0));restoredTail+=std::abs(a[0])+std::abs(a[1]);}
  check(restoredTail>.00001,"Restored effect lost retained tail");
  routes.rows[0].sources=1;check(routed->setRowRouting(routes),"reconnect");
  for(int i=0;i<2400;++i){float l=i==0?.1f:0.f,r=i==0?-.2f:0.f;compare(tick(*routed,l,r),tick(*reference,l,r));}
 }
 std::cout<<"PASS: actual stereo delay/reverb disconnected tails, skipped state and reconnection\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
