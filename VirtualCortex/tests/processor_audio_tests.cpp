#include "PluginProcessor.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#if defined(_MSC_VER)
 #include <malloc.h>
#endif

namespace allocationProbe { thread_local bool enabled=false; thread_local std::size_t count=0; }
void* operator new(std::size_t n){if(allocationProbe::enabled)++allocationProbe::count;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}
void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
void* operator new(std::size_t n,std::align_val_t alignment){
 if(allocationProbe::enabled)++allocationProbe::count;
 #if defined(_MSC_VER)
  if(void* p=_aligned_malloc(n?n:1,(std::size_t)alignment))return p;
 #else
  void* p=nullptr;if(posix_memalign(&p,(std::size_t)alignment,n?n:1)==0)return p;
 #endif
 throw std::bad_alloc();
}
void* operator new[](std::size_t n,std::align_val_t a){return ::operator new(n,a);}
void operator delete(void* p,std::align_val_t) noexcept{
 #if defined(_MSC_VER)
  _aligned_free(p);
 #else
  std::free(p);
 #endif
}
void operator delete[](void* p,std::align_val_t a) noexcept{::operator delete(p,a);}
void operator delete(void* p,std::size_t,std::align_val_t a) noexcept{::operator delete(p,a);}
void operator delete[](void* p,std::size_t,std::align_val_t a) noexcept{::operator delete(p,a);}

namespace {
void check(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
void set(VirtualCortexProcessor& p,const char* id,float value){auto* parameter=p.parameters.getParameter(id);check(parameter!=nullptr,"Missing test parameter");parameter->setValueNotifyingHost(parameter->convertTo0to1(value));}
void prepare(VirtualCortexProcessor& p,double rate=48000,int block=128){p.setRateAndBufferSizeDetails(rate,block);p.prepareToPlay(rate,block);}
void layout(VirtualCortexProcessor& p,int inputs,int outputs){auto b=p.getBusesLayout();b.inputBuses.set(0,inputs==1?juce::AudioChannelSet::mono():juce::AudioChannelSet::stereo());b.outputBuses.set(0,outputs==1?juce::AudioChannelSet::mono():juce::AudioChannelSet::stereo());check(p.setBusesLayout(b),"Requested mono/stereo layout rejected");}
void grid(VirtualCortexProcessor& p,vc::BlockType type){for(std::size_t r=0;r<4;++r){p.setGridLaneEnabled(r,r==0);for(std::size_t c=0;c<8;++c)p.setGridBlock(r,c,{vc::BlockType::Empty,true});}p.setGridBlock(0,0,{type,true});set(p,"gridMode",1);}
float settle(VirtualCortexProcessor& p,float input,int blocks=80){juce::AudioBuffer<float> b(p.getTotalNumOutputChannels(),128);juce::MidiBuffer midi;for(int block=0;block<blocks;++block){for(int ch=0;ch<b.getNumChannels();++ch)for(int i=0;i<128;++i)b.setSample(ch,i,input);p.processBlock(b,midi);}return b.getSample(0,127);}
juce::MemoryBlock state(VirtualCortexProcessor& p){juce::MemoryBlock b;p.getStateInformation(b);return b;}
void restore(VirtualCortexProcessor& p,const juce::MemoryBlock& b){p.setStateInformation(b.getData(),(int)b.getSize());}
void saveModel(const juce::File& f){std::unique_ptr<juce::FileOutputStream> out(f.createOutputStream());check(out!=nullptr,"Model fixture write failed");*out<<"VCMLP1\n";for(int i=0;i<80;++i)*out<<"0 ";*out<<"0.25\n";}
void saveIR(const juce::File& f){juce::WavAudioFormat format;auto* stream=f.createOutputStream().release();check(stream!=nullptr,"IR fixture write failed");std::unique_ptr<juce::AudioFormatWriter> writer(format.createWriterFor(stream,48000.,1,16,{},0));if(!writer){delete stream;throw std::runtime_error("IR fixture encoder failed");}juce::AudioBuffer<float> ir(1,1);ir.setSample(0,0,.5f);check(writer->writeFromAudioSampleBuffer(ir,0,1),"IR fixture output failed");}
void serialDry(VirtualCortexProcessor& p){for(const char* id:{"gateOn","ampOn","cabOn","eqOn","delayOn","reverbOn","tremOn"})set(p,id,0);}
}
int runProcessorAudioTests(){
 try {
  auto folder=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VirtualCortexAudio",{},false);
  check(folder.createDirectory().wasOk(),"Fixture directory failed");
  struct Cleanup {juce::File dir;~Cleanup(){dir.deleteRecursively();}} cleanup{folder};
  // A 1-in/2-out guitar route must produce matching dry audio on both outputs.
  {
   auto p=std::make_unique<VirtualCortexProcessor>();layout(*p,1,2);set(*p,"bypass",1);prepare(*p);
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;b.clear();
   for(int i=0;i<128;++i)b.setSample(0,i,.25f*std::sin(i*.1f));
   b.setSample(0,5,std::numeric_limits<float>::quiet_NaN());b.setSample(0,6,std::numeric_limits<float>::infinity());
   p->processBlock(b,midi);
   for(int i=0;i<128;++i){check(std::isfinite(b.getSample(0,i))&&std::isfinite(b.getSample(1,i)),"Bypass passed non-finite input");check(b.getSample(0,i)==b.getSample(1,i),"Mono guitar was not copied to stereo outputs");}
  }
  // Output trim must work in Grid mode and remain smoothed after changes.
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Empty);prepare(*p);
   check(std::abs(settle(*p,.25f)-.25f)<1.e-5f,"Grid unity gain changed");
   set(*p,"outputTrim",0);check(std::abs(settle(*p,.25f,160))<1.e-6f,"Grid output trim did not mute");
   set(*p,"outputTrim",2);check(std::abs(settle(*p,.25f,160)-.5f)<1.e-5f,"Grid output trim gain is wrong");
   set(*p,"outputTrim",1);set(*p,"gridMode",0);serialDry(*p);prepare(*p);set(*p,"outputTrim",0);check(std::abs(settle(*p,.25f,160))<1.e-6f,"Serial trim mute regressed");
  }
  // Host EQ automation must not allocate C++ objects in the processing callback.
  {
   auto p=std::make_unique<VirtualCortexProcessor>();prepare(*p);settle(*p,.01f,2);
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;b.clear();
   for(int i=0;i<12;++i){set(*p,"eqLow",(float)(i-6));set(*p,"eqMid",(float)(6-i));set(*p,"eqHigh",(float)(i-3));allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"EQ automation allocated C++ objects in audio callback");}
  }
  // UTF-8 filenames, bypassed model recall, empty presets and missing assets.
  const auto model=folder.getChildFile(juce::String::fromUTF8("amplifier-\xc3\xa9.vcmlp"));saveModel(model);
  {
   auto parallel=std::make_unique<VirtualCortexProcessor>();grid(*parallel,vc::BlockType::Empty);
   for(std::size_t row=0;row<4;++row)parallel->setGridLaneEnabled(row,true);
   parallel->setGridBlock(3,0,{vc::BlockType::Amp,true});prepare(*parallel);
   check(parallel->loadGridNeuralModel(model,3,0),"Fourth parallel row model failed to load");
   check(std::abs(settle(*parallel,0)-.0625f)<1.e-5f,"Four enabled rows were not averaged");
   for(std::size_t row=0;row<3;++row)parallel->setGridLaneEnabled(row,false);
   check(std::abs(settle(*parallel,0)-.25f)<1.e-5f,"Fourth row could not process independently");
   parallel->setGridLaneEnabled(3,false);check(std::abs(settle(*parallel,.1f)-.1f)<1.e-5f,"All rows disabled did not return dry input");
  }
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Amp);prepare(*p);
   check(p->loadGridNeuralModel(model,0,0),"Unicode model path did not load");check(std::abs(settle(*p,0)-.25f)<1.e-5f,"Loaded model did not reach audio");
   p->setGridBlock(0,0,{vc::BlockType::Amp,false});const auto disabledState=state(*p);
   auto restored=std::make_unique<VirtualCortexProcessor>();prepare(*restored);restore(*restored,disabledState);check(restored->getGridNeuralModelName(0,0)==model.getFileName(),"Bypassed amp model was lost on recall");restored->setGridBlock(0,0,{vc::BlockType::Amp,true});check(std::abs(settle(*restored,0)-.25f)<1.e-5f,"Re-enabled recalled amp did not use its model");
   auto fresh=std::make_unique<VirtualCortexProcessor>();grid(*fresh,vc::BlockType::Amp);auto noModel=state(*fresh);restore(*p,noModel);check(p->getGridNeuralModelName(0,0)=="No neural model","Empty preset kept model name");check(std::abs(settle(*p,0))<1.e-6f,"Empty preset kept stale neural audio");
   p->setGridBlock(0,0,{vc::BlockType::Amp,true});check(p->loadGridNeuralModel(model,0,0),"Model reload failed");settle(*p,0);const auto withModel=state(*p);model.deleteFile();restore(*p,withModel);check(p->getGridNeuralModelName(0,0)=="No neural model","Missing model name was retained");check(std::abs(settle(*p,0))<1.e-6f,"Missing model kept stale audio");
  }
  const auto ir=folder.getChildFile(juce::String::fromUTF8("cabinet-\xc3\xa9.wav"));saveIR(ir);
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Cabinet);prepare(*p);
   check(p->loadGridCabinetIR(ir,0,0),"Unicode cabinet path did not load");check(std::abs(settle(*p,.2f)-.1f)<1.e-4f,"Cabinet fixture was not applied");
   auto fresh=std::make_unique<VirtualCortexProcessor>();grid(*fresh,vc::BlockType::Cabinet);restore(*p,state(*fresh));check(p->getGridCabinetIRName(0,0)=="No grid IR","Empty preset kept cabinet name");check(std::abs(settle(*p,.2f)-.2f)<1.e-4f,"Empty preset kept stale IR audio");
   check(p->loadGridCabinetIR(ir,0,0),"IR reload failed");settle(*p,.2f);const auto withIR=state(*p);ir.replaceWithText("invalid WAV");restore(*p,withIR);check(p->getGridCabinetIRName(0,0)=="No grid IR","Corrupt IR was reported loaded");check(std::abs(settle(*p,.2f)-.2f)<1.e-4f,"Corrupt IR kept stale audio");
   check(!p->loadCabinetIR(ir),"Serial cabinet accepted invalid WAV");
  }
  // The new blocks must survive JUCE state recall and stay allocation-free while automated.
  for(auto type:{vc::BlockType::Compressor,vc::BlockType::Overdrive,vc::BlockType::Distortion}){
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,type);prepare(*p);vc::SlotParameters controls;controls.mix=1.f;controls.time=0.f;controls.depth=.3f;p->setGridSlotParameters(0,0,controls);settle(*p,.1f);
   auto recalled=std::make_unique<VirtualCortexProcessor>();prepare(*recalled);restore(*recalled,state(*p));check(recalled->getEditableGrid().get(0,0)->type==type,"New block type lost on state recall");check(std::abs(recalled->getGridSlotParameters(0,0).depth-.3f)<1.e-6f,"New block controls lost on state recall");
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
   for(int step=0;step<8;++step){controls.drive=step/8.f;p->setGridSlotParameters(0,0,controls);for(int ch=0;ch<2;++ch)for(int i=0;i<128;++i)b.setSample(ch,i,.1f*std::sin((i+step*128)*.1f));allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"New effect automation allocated in callback");for(int ch=0;ch<2;++ch)for(int i=0;i<128;++i)check(std::isfinite(b.getSample(ch,i)),"New effect produced non-finite samples");}
  }
  // Exercise actual JUCE processor buffers at the common Anubis rate/buffer range.
  for(double rate:{44100.,48000.,96000.,192000.})for(int size:{32,64,128,256,512}){
   auto p=std::make_unique<VirtualCortexProcessor>();prepare(*p,rate,size);juce::AudioBuffer<float> b(2,size);juce::MidiBuffer midi;
   for(int mode=0;mode<2;++mode){set(*p,"gridMode",(float)mode);for(int block=0;block<4;++block){for(int ch=0;ch<2;++ch)for(int i=0;i<size;++i)b.setSample(ch,i,.15f*std::sin((i+block*size)*.15f));b.setSample(0,0,std::numeric_limits<float>::quiet_NaN());p->processBlock(b,midi);for(int ch=0;ch<2;++ch)for(int i=0;i<size;++i)check(std::isfinite(b.getSample(ch,i))&&std::abs(b.getSample(ch,i))<=1.f,"Processor produced invalid output at rate/buffer combination");}}
  }
  std::cout<<"PASS: mono/stereo, trim, callback allocations, UTF-8 assets, preset clearing, and rate/buffer matrix\n";return 0;
 }catch(const std::exception& e){allocationProbe::enabled=false;std::cerr<<e.what()<<'\n';return 1;}
}
