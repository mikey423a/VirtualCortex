#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/CreativeEffects.h"
#include <cassert>
int main(){
 for(double rate:{44100.,48000.,96000.,192000.}){
  vc::IntervalEcho d;d.prepare(rate);vc::IntervalEcho::Parameters p;p.enabled=true;p.a=p.b=12;p.step=4000;p.feedback=0;p.mix=1;p.cutoff=18000;d.configure(p);int crossings=0;float previous=0;
  for(int i=0;i<(int)(rate*5.5);++i){auto y=d.process(.1f*std::sin(6.283185307179586*440*i/rate),0);assert(std::isfinite(y[0])&&y[1]==0);if(i>(int)(rate*4.5)&&previous<=0&&y[0]>0)++crossings;previous=y[0];}assert(crossings>860&&crossings<900);
  vc::LayerLoom loop;loop.prepare(rate);loop.configure(1,1,18000,false,true);loop.command(vc::LayerLoom::Record);
  for(int i=0;i<4096;++i)loop.process(.1f,-.1f);loop.command(vc::LayerLoom::Record);assert(loop.frames()==4096&&loop.layerCount()==1&&!loop.isRecording());
  std::array<float,2> y{};for(int i=0;i<512;++i)y=loop.process(0,0);assert(y[0]>.09f&&y[1]<-.09f);
  loop.command(vc::LayerLoom::Record);for(int i=0;i<4096;++i)loop.process(.2f,-.2f);assert(loop.layerCount()==2&&!loop.isRecording());for(int i=0;i<512;++i)y=loop.process(0,0);assert(y[0]>.29f);
  loop.command(vc::LayerLoom::Undo);for(int i=0;i<512;++i)y=loop.process(0,0);assert(loop.layerCount()==1&&y[0]<.11f);
  loop.command(vc::LayerLoom::Redo);assert(loop.layerCount()==2);loop.configure(1,.5f,1000,true,true);for(int i=0;i<4096;++i){y=loop.process(0,0);assert(std::isfinite(y[0]));}
  loop.command(vc::LayerLoom::Clear);assert(loop.frames()==0&&loop.layerCount()==0);loop.prepare(rate);assert(loop.frames()==0);
 }
}
