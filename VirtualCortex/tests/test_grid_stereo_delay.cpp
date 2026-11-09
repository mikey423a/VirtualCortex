#include "DSP/PortableAudioEngine.h"
#include <stdexcept>
#include <iostream>
static void check(bool b){if(!b)throw std::runtime_error("Grid stereo delay failed");}
int main(){try{
 auto e=std::make_unique<vc::PortableAudioEngine>();e->prepare(48000,64);e->prepareStereoSlot(0,0);
 vc::SignalGrid g;g.set(0,0,{vc::BlockType::GridStereoDelay,true});check(e->configure(g,{true,false,false,false}));
 vc::SlotParameters p;p.time=9.f/3999;p.rate=19.f/3999;p.mix=1;p.feedback=0;p.drive=0;p.depth=0;e->setSlotParameters(0,0,p);
 float f[2]{};for(int i=0;i<5000;++i){f[0]=f[1]=0;check(e->process(f,1,2));}
 int left=-1,right=-1;float lp=0,rp=0;
 for(int i=0;i<2000;++i){f[0]=f[1]=i==0?.1f:0.f;e->process(f,1,2);if(std::abs(f[0])>lp){lp=std::abs(f[0]);left=i;}if(std::abs(f[1])>rp){rp=std::abs(f[1]);right=i;}}
 check(left==480&&right==960&&lp>.09&&rp>.09);
 check(e->swapSlots(0,0,2,7));g.swap(0,0,2,7);check(e->configure(g,{false,false,true,false}));
 check(e->getSlotParameters(2,7)->time==p.time);auto bytes=vc::encodeGrid(g);vc::SignalGrid saved;check(vc::decodeGrid(bytes,saved)&&saved.get(2,7)->type==vc::BlockType::GridStereoDelay);
 std::cout<<"PASS: separate stereo times, movable state and grid serialization\n";
}catch(const std::exception& e){std::cerr<<e.what();return 1;}}
