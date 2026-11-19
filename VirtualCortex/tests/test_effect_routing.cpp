#include "DSP/PortableAudioEngine.h"
#include <iostream>
#include <stdexcept>
static void check(bool b,const char* m){if(!b)throw std::runtime_error(m);}
static void checkRowOutputBlending(double rate,bool average){
 using Mode=vc::ParallelGrid::Mode;
 auto instant=std::make_unique<vc::PortableAudioEngine>();
 auto smooth=std::make_unique<vc::PortableAudioEngine>();
 auto delayOnly=std::make_unique<vc::PortableAudioEngine>();
 auto reverbOnly=std::make_unique<vc::PortableAudioEngine>();
 vc::SignalGrid grid;
 grid.set(0,0,{vc::BlockType::GridStereoDelay,true});
 grid.set(1,0,{vc::BlockType::GridStereoReverb,true});
 vc::RowRouting routes;
 routes.average=average;
 routes.rows[1].sources=2; // The muted delay must still feed the downstream reverb.
 for(auto* engine:{instant.get(),smooth.get(),delayOnly.get(),reverbOnly.get()}){
  engine->prepare(rate,64);
  engine->prepareStereoSlot(0,0);
  engine->prepareReverbSlot(1,0);
  check(engine->configure(grid,{true,true,false,false}),"Output blend fixture grid");
  vc::SlotParameters p;
  p.mix=1.f;
  p.time=.003f;
  p.rate=.02f;
  p.feedback=.7f;
  check(engine->setSlotParameters(0,0,p)&&engine->setSlotParameters(1,0,p),"Output blend fixture controls");
  auto initial=routes;
  initial.smoothRowOutputs=engine==smooth.get();
  initial.rows[0].output=engine!=reverbOnly.get();
  initial.rows[1].output=engine!=delayOnly.get();
  check(engine->setRowRouting(initial),"Output blend fixture routing");
  engine->enableRowOutputBlending(true);
 }
 const int start=static_cast<int>(rate/2),duration=static_cast<int>(std::lround(rate*.005));
 std::array<int,2> ramp{duration,duration};
 std::array<float,2> previousRaw{},previousSmooth{};
 double rawStep=0,blendedStep=0,tail=0;
 for(int i=0;i<start+4*duration+5000;++i){
  // Reverse one OUT command mid-fade, then turn both off and restore just reverb.
  const int offset=i-start;
  const bool edit=offset==0||offset==duration/2||offset==duration||offset==3*duration;
  if(edit){
   routes.rows[0].output=offset==duration/2;
   routes.rows[1].output=offset!=duration;
   check(instant->setRowRouting(routes),"Immediate output edit");
   auto blended=routes;
   blended.smoothRowOutputs=true;
   check(smooth->setRowRouting(blended),"Smoothed output edit");
  }
  const float x=offset<duration?.13f*std::cos(offset*.0341f):0.f;
  std::array<float,2> raw{x,-.53f*x},y=raw,a=raw,b=raw;
  check(instant->process(raw.data(),1,2,Mode::Custom)&&smooth->process(y.data(),1,2,Mode::Custom)
     &&delayOnly->process(a.data(),1,2,Mode::Custom)&&reverbOnly->process(b.data(),1,2,Mode::Custom),"Output blend render");
  for(int r=0;r<2;++r)ramp[r]+=routes.rows[r].output?(ramp[r]<duration):-(ramp[r]>0);
  const double w0=static_cast<double>(ramp[0])/duration,w1=static_cast<double>(ramp[1])/duration;
  for(int ch=0;ch<2;++ch){
   const auto expected=(w0*a[ch]+w1*b[ch])/(average?std::max(1.,w0+w1):1.);
   check(std::abs(y[ch]-expected)<2.e-6,"Weighted row output reference");
   if(offset==0){
    rawStep=std::max(rawStep,double(std::abs(raw[ch]-previousRaw[ch])));
    blendedStep=std::max(blendedStep,double(std::abs(y[ch]-previousSmooth[ch])));
   }
   if(offset>=4*duration){check(y[ch]==raw[ch],"Output fade altered the settled reverb history");tail+=y[ch]*y[ch];}
  }
  previousRaw=raw;
  previousSmooth=y;
 }
 check(rawStep>.005&&blendedStep<rawStep*.35&&tail>.0001,"Output blend needs a measured step reduction and retained tail");
 std::cout<<"ROW_OUTPUT rate="<<rate<<" average="<<average<<" immediate_step="<<rawStep
          <<" blended_step="<<blendedStep<<" restored_tail_energy="<<tail<<"\n";
}
static void checkConnectionBlending(double rate,bool average){
 using Mode=vc::ParallelGrid::Mode;
 auto instant=std::make_unique<vc::PortableAudioEngine>();
 auto smooth=std::make_unique<vc::PortableAudioEngine>();
 auto delay=std::make_unique<vc::PortableAudioEngine>();
 auto reference=std::make_unique<vc::PortableAudioEngine>();
 vc::SignalGrid grid,delayGrid,reverbGrid;
 grid.set(0,0,{vc::BlockType::GridStereoDelay,true});
 grid.set(1,0,{vc::BlockType::GridStereoReverb,true});
 delayGrid.set(0,0,*grid.get(0,0));
 reverbGrid.set(1,0,*grid.get(1,0));
 vc::RowRouting routes;
 routes.average=average;
 routes.rows[0].output=false;
 routes.rows[1].sources=3;
 for(auto* engine:{instant.get(),smooth.get(),delay.get(),reference.get()}){
  const bool delayOnly=engine==delay.get(), reverbOnly=engine==reference.get();
  engine->prepare(rate,64);
  if(!reverbOnly)engine->prepareStereoSlot(0,0);
  if(!delayOnly)engine->prepareReverbSlot(1,0);
  check(engine->configure(delayOnly?delayGrid:reverbOnly?reverbGrid:grid,{!reverbOnly,!delayOnly,false,false}),"Connection fixture grid");
  vc::SlotParameters p;
  p.mix=.83f;p.time=.003f;p.rate=.018f;p.feedback=.65f;
  check(engine->setSlotParameters(0,0,p)&&engine->setSlotParameters(1,0,p),"Connection fixture controls");
  auto initial=routes;
  initial.smoothSources=engine==smooth.get();
  if(delayOnly)initial.rows[0].output=true;
  if(reverbOnly)initial.rows[1].sources=1;
  check(engine->setRowRouting(initial),"Connection fixture routing");
  engine->enableSourceConnectionBlending(true);
 }
 const int start=static_cast<int>(rate/2),duration=static_cast<int>(std::lround(rate*.005));
 std::array<int,2> ramp{duration,duration};
 std::array<float,2> previousRaw{},previousSmooth{};
 double rawStep=0,blendStep=0,tail=0;
 for(int i=0;i<start+5*duration+6000;++i){
  const int offset=i-start;
  const bool edit=offset==0||offset==duration/2||offset==duration||offset==3*duration;
  if(edit){
   // Remove delay, reconnect mid-fade, then remove both sources and restore IN.
   routes.rows[1].sources=offset==0||offset==3*duration?1:offset==duration/2?3:0;
   check(instant->setRowRouting(routes),"Immediate connection edit");
   auto blended=routes;blended.smoothSources=true;
   check(smooth->setRowRouting(blended),"Smoothed connection edit");
  }
  const float x=offset<2*duration?.13f*std::cos(offset*.0341f):0.f;
  std::array<float,2> raw{x,-.53f*x},y=raw,d=raw,expected{};
  check(delay->process(d.data(),1,2,Mode::Custom),"Render independent source delay");
  for(unsigned source=0;source<2;++source)
   ramp[source]=std::clamp(ramp[source]+((routes.rows[1].sources&(1u<<source))?1:-1),0,duration);
  const double direct=double(ramp[0])/duration,wet=double(ramp[1])/duration;
  for(int ch=0;ch<2;++ch)expected[ch]=static_cast<float>((direct*raw[ch]+wet*d[ch])/(average?std::max(1.,direct+wet):1.));
  check(reference->process(expected.data(),1,2,Mode::Custom)&&instant->process(raw.data(),1,2,Mode::Custom)
      &&smooth->process(y.data(),1,2,Mode::Custom),"Render downstream reverb");
  for(int ch=0;ch<2;++ch){
   check(std::abs(y[ch]-expected[ch])<2.e-6f,"Downstream reverb disagrees with weighted source reference");
   if(offset==0){
    rawStep=std::max(rawStep,double(std::abs(raw[ch]-previousRaw[ch])));
    blendStep=std::max(blendStep,double(std::abs(y[ch]-previousSmooth[ch])));
   }
   if(offset>=4*duration)tail+=y[ch]*y[ch];
  }
  previousRaw=raw;previousSmooth=y;
 }
 check(rawStep>.001&&tail>.00001,"Connection fixture needs a step and live reverb tail");
 if(rate==48000.)check(blendStep<rawStep*.5,"48 kHz connection fade discontinuity not reduced");
 std::cout<<"SOURCE_CONNECTION rate="<<rate<<" average="<<average<<" immediate_step="<<rawStep
          <<" blended_step="<<blendStep<<" retained_tail_energy="<<tail<<"\n";
}
int main(){try{
 for(double rate:{44100.,48000.,96000.})for(bool average:{false,true})checkConnectionBlending(rate,average);
 for(double rate:{44100.,48000.,96000.})for(bool average:{false,true})checkRowOutputBlending(rate,average);
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
 for(bool entryEdit:{false,true}) for(double rate:{44100.,48000.,96000.}) for(auto type:{vc::BlockType::GridStereoDelay,vc::BlockType::GridStereoReverb}){
  auto moving=std::make_unique<vc::PortableAudioEngine>();
  auto fixed=std::make_unique<vc::PortableAudioEngine>();
  vc::SignalGrid grid;
  grid.set(0,0,{type,true});
  vc::RowRouting routing;
  routing.rows[0].output=entryEdit;
  routing.rows[1].output=!entryEdit;
  routing.rows[1].sources=2;
  routing.rows[1].sourceTaps[0]=8;
  for(auto* engine:{moving.get(),fixed.get()}){
   engine->prepare(rate,64);
   if(type==vc::BlockType::GridStereoDelay) engine->prepareStereoSlot(0,0);
   else engine->prepareReverbSlot(0,0);
   check(engine->configure(grid,{true,true,false,false}),"Configure two-row test");
   check(engine->setRowRouting(routing),"Apply fixed route");
   vc::SlotParameters params;
   params.mix=1;params.time=.003f;params.rate=.005f;params.feedback=.5f;
   check(engine->setSlotParameters(0,0,params),"Configure effect");
  }
  routing.smoothSourceTaps=!entryEdit;
  routing.smoothRowEntries=entryEdit;
  if(!entryEdit) routing.rows[1].sourceTaps[0]=0;
  moving->setRowRouting(routing);
  moving->enableSourceTapBlending(true);
  moving->enableRowEntryBlending(true);
  const int change=int(rate/2),length=int(std::lround(rate*.005));
  const int resume=change+length+500;
  double tail=0,hardStep=0,softStep=0,resumedTail=0;
  float previous=0;
  for(int i=0;i<change+length+8000;++i){
   const float input=i<change+length?.2f*std::cos((i-change)*.043f):0.f;
   std::array<float,2> dry{input,-.73f*input},wet=dry,output=dry;
   if(i==change){
    if(entryEdit) routing.rows[0].entryColumn=1;
    else routing.rows[1].sourceTaps[0]=8;
    moving->setRowRouting(routing);
   }
   if(entryEdit&&i==resume){
    routing.rows[0].entryColumn=0;
    moving->setRowRouting(routing);
   }
   // Hold the reference effect at the same frame where the blend reaches dry.
   if(!entryEdit||i<change+length-1||i>=resume)
    check(fixed->process(wet.data(),1,2,vc::ParallelGrid::Mode::Custom),"Fixed render");
   check(moving->process(output.data(),1,2,vc::ParallelGrid::Mode::Custom),"Moving render");
   const float progress=std::clamp(float(i-change+1)/length,0.f,1.f);
   const float reentry=std::clamp(float(i-resume+1)/length,0.f,1.f);
   const float w=entryEdit ? (i<resume ? 1.f-progress : reentry) : progress;
   for(int ch=0;ch<2;++ch)check(std::abs(output[ch]-(dry[ch]*(1-w)+wet[ch]*w))<2.e-6f,"Actual tap fade reference");
   if(i==change){hardStep=std::abs((entryEdit?dry[0]:wet[0])-previous);softStep=std::abs(output[0]-previous);}
   previous=output[0];
   if(i>=change+length&&(!entryEdit||i<resume))tail+=std::abs(output[0])+std::abs(output[1]);
   if(entryEdit&&i>=resume)resumedTail+=std::abs(output[0])+std::abs(output[1]);
  }
  if(entryEdit){check(tail==0,"Fully skipped effect should be silent after input ends");check(resumedTail>.001,"Reentered effect lost its retained tail");}
  else check(tail>.001,"Lost effect tail after tap edit");
  std::cout<<(entryEdit?"ENTRY_EFFECT rate=":"TAP_EFFECT rate=")<<rate<<" type="<<int(type)<<" hard_step="<<hardStep<<" blend_step="<<softStep<<" tail_abs_sum="<<tail<<" resumed_tail="<<resumedTail<<"\n";
 }
 for(double rate:{44100.,48000.,96000.}) {
  using Mode=vc::ParallelGrid::Mode;
  auto classic=std::make_unique<vc::PortableAudioEngine>();
  auto smoothed=std::make_unique<vc::PortableAudioEngine>();
  vc::SignalGrid grid;
  grid.set(0,0,{vc::BlockType::GridStereoDelay,true});
  grid.set(1,0,{vc::BlockType::GridStereoReverb,true});
  vc::RowRouting routes;
  routes.rows[1].sources=2;
  routes.rows[0].output=false;
  for(auto* engine:{classic.get(),smoothed.get()}) {
   engine->prepare(rate,64);
   engine->prepareStereoSlot(0,0);
   engine->prepareReverbSlot(1,0);
   check(engine->configure(grid,{true,true,false,false}),"Mode fixture grid");
   routes.smoothRoutingModes=engine==smoothed.get();
   check(engine->setRowRouting(routes),"Mode fixture routing");
   engine->enableRoutingModeSmoothing(true);
   vc::SlotParameters controls;
   controls.mix=1.f;
   controls.feedback=.5f;
   controls.time=.005f;
   check(engine->setSlotParameters(0,0,controls)&&engine->setSlotParameters(1,0,controls),"Mode fixture effects");
  }
  const int change=static_cast<int>(rate/2);
  const int duration=static_cast<int>(std::lround(rate*.005));
  std::array<float,2> previous{},previousRaw{};
  int fadeEnd=0;
  double step=0,tail=0;
  for(int i=0;i<change+8000;++i) {
   const bool first=i==change;
   const bool second=i==change+duration/2;
   const auto mode=i<change ? Mode::Parallel : i<change+duration/2 ? Mode::Series : Mode::Custom;
   const float input=i<change+duration?.2f*std::cos((i-change)*.043f):0.f;
   std::array<float,2> raw{input,-.73f*input},smoothedFrame=raw;
   check(classic->process(raw.data(),1,2,mode),"Immediate mode render");
   check(smoothed->process(smoothedFrame.data(),1,2,mode),"Smoothed mode render");
   if(first||second) {
    fadeEnd=i+duration;
    check(smoothedFrame==previous,"Mode change discontinuity");
    step=std::max(step,static_cast<double>(std::abs(raw[0]-previousRaw[0])));
   } else if(i>=fadeEnd) {
    check(smoothedFrame==raw,"Mode blend changed the steady effect history");
    if(i>change+2*duration)tail+=std::abs(raw[0])+std::abs(raw[1]);
   }
   previous=smoothedFrame;
   previousRaw=raw;
  }
  check(step>.001&&tail>.001,"Mode fixture must exercise a discontinuity and retain an effect tail");
  std::cout<<"MODE_EFFECT rate="<<rate<<" immediate_peak_switch_step="<<step<<" smooth_switch_step=0 tail_abs_sum="<<tail<<"\n";
 }
 std::cout<<"PASS: actual stereo delay/reverb disconnected tails, skipped state and reconnection\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
