#pragma once
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
namespace vc {
class IntervalEcho {
public:
 struct Parameters {float time=350,feedback=.4f,a=7,b=-5,step=500,cutoff=5000,mix=.3f;bool enabled=false;};
 void prepare(double rate){fs=std::clamp(rate,8000.,192000.);for(auto& v:buffer)v.assign((std::size_t)(fs*4.1)+4,0);index=0;drain=0;phase=0;clock=0;stage=0;low={};mix=0;ratio=1;delay=350;feedback=0;smooth=std::exp(-1./(fs*.02));}
 bool active()const noexcept{return cfg.enabled||drain>0;}
 void configure(Parameters p){auto safe=[](float x,float lo,float hi,float d){return std::isfinite(x)?std::clamp(x,lo,hi):d;};p.time=safe(p.time,1,4000,350);p.feedback=safe(p.feedback,0,.9f,.4f);p.a=safe(p.a,-12,12,7);p.b=safe(p.b,-12,12,-5);p.step=safe(p.step,100,4000,500);p.cutoff=safe(p.cutoff,200,18000,5000);p.mix=safe(p.mix,0,1,.3f);cfg=p;filter=std::exp(-6.283185307179586*std::min((double)p.cutoff,fs*.45)/fs);}
 std::array<float,2> process(float l,float r){if(!active()||buffer[0].empty())return {l,r};
  if(cfg.enabled)drain=(std::size_t)(fs*4.3);else if(drain)--drain;
  if(++clock>=fs*cfg.step*.001){clock=0;stage=(stage+1)%3;}
  const double target=std::pow(2.,(stage==0?0:stage==1?cfg.a:cfg.b)/12.);ratio=target+smooth*(ratio-target);delay=cfg.time+smooth*(delay-cfg.time);feedback=(cfg.enabled?cfg.feedback:0)+smooth*(feedback-(cfg.enabled?cfg.feedback:0));mix=(cfg.enabled?cfg.mix:0)+smooth*(mix-(cfg.enabled?cfg.mix:0));if(mix<1e-8)mix=0;
  const double second=phase<.5?phase+.5:phase-.5;const double w=.5-.5*std::cos(6.283185307179586*phase);
  std::array<float,2> out{},in{{std::isfinite(l)?l:0,std::isfinite(r)?r:0}};
  for(std::size_t ch=0;ch<2;++ch){auto read=[&](double d){d=std::clamp(d,1.,(double)buffer[ch].size()-2);const auto n=(std::size_t)d;const float f=(float)(d-n);const auto len=buffer[ch].size();float a=buffer[ch][(index+len-n)%len],b=buffer[ch][(index+len-n-1)%len];return a+(b-a)*f;};
   const float wet=(float)(w*read(fs*(delay*.001+phase*.06))+(1-w)*read(fs*(delay*.001+second*.06)));low[ch]=(float)(wet+filter*(low[ch]-wet));buffer[ch][index]=std::clamp((cfg.enabled?in[ch]:0)+(float)feedback*low[ch],-8.f,8.f);out[ch]=(float)((1-mix)*in[ch]+mix*low[ch]);}
  if(++index==buffer[0].size())index=0;phase+=(1-ratio)/(fs*.06);phase-=std::floor(phase);return out;
 }
private:
 Parameters cfg;double fs=48000,smooth=0,filter=0,phase=0,clock=0,ratio=1,delay=350,mix=0,feedback=0;int stage=0;std::size_t index=0,drain=0;std::array<std::vector<float>,2> buffer;std::array<float,2> low{};
};
// Eight independent stereo layers; fixed capacity allocated while stopped.
// Commands/process belong to the audio thread. Audio is session-only.
class LayerLoom {
public:
 enum Command {Record=1,Stop=2,Play=4,Undo=8,Redo=16,Clear=32};
 void prepare(double rate){fs=std::clamp(rate,8000.,192000.);capacity=(std::size_t)(fs*16);for(auto& l:layers)for(auto& c:l)c.assign(capacity,0);clear();mix=0;speed=1;low={};}
 void clear()noexcept {length=0;count=visible=0;written.fill(0);recording=false;playing=false;position=0;write=0;}
 void command(unsigned bits)noexcept {
  if(bits&Clear){clear();return;}
  if(bits&Stop){finish();playing=false;}
  if(bits&Undo){finish();if(visible>0)--visible;}
  if(bits&Redo){finish();if(visible<count)++visible;}
  if(bits&Play){playing=length>0;}
  if(bits&Record){if(recording){finish();return;}if(visible>=8)return;count=visible;written[count]=0;write=0;position=0;recording=true;playing=true;}
 }
 void configure(float level,float rate,float cutoff,bool reverse,bool enabled)noexcept {targetMix=std::isfinite(level)?std::clamp(level,0.f,1.f):0;targetSpeed=std::isfinite(rate)?std::clamp(rate,.5f,2.f):1;backward=reverse;on=enabled;filter=std::exp(-6.283185307179586*std::clamp(std::isfinite(cutoff)?(double)cutoff:8000.,200.,fs*.45)/fs);}
 std::array<float,2> process(float l,float r)noexcept {
  std::array<float,2> input{{std::isfinite(l)?l:0,std::isfinite(r)?r:0}},wet{};
  if(capacity==0||(!on&&!playing&&!recording&&mix==0))return input;
  const float c=(float)std::exp(-1./(fs*.01));mix=(on?targetMix:0)+c*(mix-(on?targetMix:0));if(mix<1e-8)mix=0;speed=targetSpeed+c*(speed-targetSpeed);
  if(recording){for(std::size_t ch=0;ch<2;++ch)layers[count][ch][write]=input[ch];written[count]=write+1;++write;if((length&&write>=length)||write>=capacity)finish();}
  if(playing&&length){for(std::size_t ch=0;ch<2;++ch){const auto a=(std::size_t)position,b=(a+1)%length;const float t=(float)(position-a);for(std::size_t layer=0;layer<visible;++layer){float x=a<written[layer]?layers[layer][ch][a]:0,y=b<written[layer]?layers[layer][ch][b]:0;wet[ch]+=x+(y-x)*t;}
    // Short endpoint fades suppress loop-boundary discontinuities.
    const double edge=std::max(1.,std::min(fs*.003,(double)length*.1));wet[ch]*=(float)std::min(1.,std::min(position,(double)length-1-position)/edge);
   }
   position+=recording?1:(backward?-speed:speed);while(position<0)position+=length;while(position>=length)position-=length;
  }
  for(std::size_t ch=0;ch<2;++ch){low[ch]=(float)(wet[ch]+filter*(low[ch]-wet[ch]));input[ch]=input[ch]+mix*low[ch];}return input;
 }
 std::size_t layerCount()const noexcept{return visible;}
 std::size_t frames()const noexcept{return length;}
 bool isRecording()const noexcept{return recording;}
 bool isPlaying()const noexcept{return playing;}
private:
 void finish()noexcept {if(!recording)return;recording=false;if(write==0)return;if(!length)length=write;count++;visible=count;position=0;playing=true;}
 std::array<std::array<std::vector<float>,2>,8> layers;std::array<std::size_t,8> written{};
 std::size_t capacity=0,length=0,count=0,visible=0,write=0;double fs=48000,position=0,filter=0;float mix=0,targetMix=.5f,speed=1,targetSpeed=1;std::array<float,2> low{};bool recording=false,playing=false,backward=false,on=false;
};
}
