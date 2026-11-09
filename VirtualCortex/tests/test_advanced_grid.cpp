#include "DSP/PortableAudioEngine.h"
#include <iostream>
#include <stdexcept>
#include <limits>
using namespace vc;
static void check(bool value,const char* why){if(!value)throw std::runtime_error(why);}
int main(){try{
 for(int type=30;type<=36;++type){
  PortableAudioEngine moved,reference;moved.prepare(8000);reference.prepare(8000);
  SignalGrid g;g.set(0,0,{(BlockType)type,true});std::array<bool,4> enabled{true,false,false,false};
  moved.configure(g,enabled);reference.configure(g,enabled);
  moved.prepareAdvancedSlot(0,0,type);reference.prepareAdvancedSlot(0,0,type);
  auto v=advancedDefaults(type);check(validAdvanced(type,v),"Invalid defaults");
  auto invalid=v;invalid[0]=std::numeric_limits<float>::quiet_NaN();check(!validAdvanced(type,invalid),"NaN accepted");
  moved.configureAdvancedSlot(0,0,v,123,type==34?LayerLoom::Record:0);reference.configureAdvancedSlot(0,0,v,123,type==34?LayerLoom::Record:0);
  for(int i=0;i<4000;++i){
   if(i==1000&&type==34){moved.configureAdvancedSlot(0,0,v,123,LayerLoom::Record);reference.configureAdvancedSlot(0,0,v,123,LayerLoom::Record);check(moved.advancedStatus(0,0)==reference.advancedStatus(0,0),"Loop status diverged");}
   if(i==1500){check(moved.swapSlots(0,0,0,7),"Move failed");g.swap(0,0,0,7);moved.configure(g,enabled);moved.configureAdvancedSlot(0,7,v,123);}
   if(i==1600)moved.configureAdvancedSlot(0,7,invalid,123); // rejected without poisoning the live state
   const float x=i<1500?.15f*std::sin(i*.17f):0;float a[2]{x,-.5f*x},b[2]{x,-.5f*x};
   moved.process(a,1,2);reference.process(b,1,2);
   check(std::isfinite(a[0])&&std::isfinite(a[1]),"Nonfinite advanced output");
   check(a[0]==b[0]&&a[1]==b[1],"Moving effect changed its live DSP state");
  }
  GridBytes bytes=encodeGrid(g);SignalGrid decoded;check(decodeGrid(bytes,decoded)&&(int)decoded.get(0,7)->type==type,"Advanced type not serializable");
 }
 std::cout<<"PASS: all seven advanced types preserve sample-identical tails/state through moves\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
