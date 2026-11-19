#include "DSP/PortableAudioEngine.h"
#include <iostream>
#include <stdexcept>
#include <limits>
using namespace vc;
static void check(bool value,const char* why){if(!value)throw std::runtime_error(why);}

static void checkGridTailDetail(double rate){
 constexpr int detail=19;
 auto settings=advancedDefaults(31);
 check(settings[detail]==0.f,"Grid Tail Detail must default off");
 settings[5]=1.f;
 settings[11]=1.f;

 // Three real grid renders keep independent histories through the same feed.
 PortableAudioEngine linear,cubic,moving;
 SignalGrid grid;
 grid.set(0,0,{BlockType::GridStereoReverb,true});
 for(auto* engine:{&linear,&cubic,&moving}){
  engine->prepare(rate,64);
  check(engine->configure(grid,{true,false,false,false}),"Configure grid reverb");
  engine->prepareAdvancedSlot(0,0,31);
  settings[detail]=engine==&cubic ? 1.f : 0.f;
  engine->configureAdvancedSlot(0,0,settings,120.f);
 }
 const auto fade=static_cast<int>(std::lround(rate*.005));
 const auto stop=static_cast<int>(rate*.25);
 double difference=0.,tailEnergy=0.;
 for(int frame=0;frame<stop+4*fade+4000;++frame){
  if(frame==stop||frame==stop+2*fade){
   settings[detail]=frame==stop ? 1.f : 0.f;
   moving.configureAdvancedSlot(0,0,settings,120.f);
  }
  const std::array<float,2> input=frame<stop
      ? std::array<float,2>{.12f*std::sin(frame*.61f),.08f*std::cos(frame*.37f)}
      : std::array<float,2>{};
  auto a=input,b=input,y=input;
  check(linear.process(a.data(),1,2)&&cubic.process(b.data(),1,2)&&moving.process(y.data(),1,2),
        "Render Grid Stereo Reverb");
  const int weightFrames=frame<stop+2*fade ? std::clamp(frame-stop+1,0,fade)
      : std::clamp(stop+3*fade-frame-1,0,fade);
  const float weight=static_cast<float>(weightFrames)/fade;
  for(int ch=0;ch<2;++ch){
   check(std::abs(y[ch]-(a[ch]+weight*(b[ch]-a[ch])))<2.e-6f,"Grid reverb detail fade");
   if(weightFrames==0)check(y[ch]==a[ch],"Grid reverb failed to restore the linear tail");
   if(weightFrames==fade){
    check(y[ch]==b[ch],"Grid reverb did not reach the cubic tail");
    difference+=std::abs(b[ch]-a[ch]);
   }
   if(frame>=stop+3*fade)tailEnergy+=y[ch]*y[ch];
  }
 }
 check(difference>1.e-5&&tailEnergy>1.e-5,"Grid reverb detail fixture needs a distinct surviving tail");
 std::cout<<"GRID_REVERB_DETAIL rate="<<rate<<" abs_difference_sum="<<difference
          <<" retained_tail_energy="<<tailEnergy<<"\n";
}
int main(){try{
 for(int type=30;type<=36;++type){
  PortableAudioEngine moved,reference;moved.prepare(8000);reference.prepare(8000);
  SignalGrid g;g.set(0,0,{(BlockType)type,true});std::array<bool,4> enabled{true,false,false,false};
  moved.configure(g,enabled);reference.configure(g,enabled);
  moved.prepareAdvancedSlot(0,0,type);reference.prepareAdvancedSlot(0,0,type);
  auto v=advancedDefaults(type);if(type==31)v[19]=1.f;check(validAdvanced(type,v),"Invalid defaults");
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
 for(double rate:{44100.,48000.,96000.})checkGridTailDetail(rate);
 std::cout<<"PASS: all seven advanced types preserve sample-identical tails/state through moves\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
