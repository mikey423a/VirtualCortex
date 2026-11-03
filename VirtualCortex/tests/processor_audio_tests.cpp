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
  // Host BPM wins over manual fallback; synchronized output matches equal manual ms.
  {
   struct TempoHead:juce::AudioPlayHead {juce::Optional<PositionInfo> getPosition() const override {PositionInfo p;p.setBpm(120.);return p;}} host;
   auto render=[&](bool sync,float beats,float bpm,bool useHost){auto p=std::make_unique<VirtualCortexProcessor>();layout(*p,1,2);grid(*p,vc::BlockType::Empty);prepare(*p);set(*p,"scOn",1);set(*p,"scMix",1);set(*p,"scMode",2);set(*p,"scBits",8);set(*p,"scStepMs",125);set(*p,"scSync",sync?1:0);set(*p,"scBpm",bpm);set(*p,"scBeats",beats);if(useHost)p->setPlayHead(&host);juce::AudioBuffer<float>b(2,128);juce::MidiBuffer midi;std::vector<float>out;out.reserve(25600);
    for(int block=0;block<200;++block){for(int i=0;i<128;++i)b.setSample(0,i,.2f*std::sin((block*128+i)*.057f));p->processBlock(b,midi);for(int i=0;i<128;++i)out.push_back(b.getSample(0,i));}p->setPlayHead(nullptr);return out;};
   const auto manual=render(false,.25f,120,false),fallback=render(true,.25f,120,false),hosted=render(true,.25f,60,true),slower=render(true,.5f,120,false);
   double difference=0;for(std::size_t i=0;i<manual.size();++i){check(std::abs(manual[i]-fallback[i])<1.e-6f,"Fallback sync timing differs");check(std::abs(manual[i]-hosted[i])<1.e-6f,"Host BPM did not override fallback");difference+=std::abs(manual[i]-slower[i]);}check(difference>1,"Beat division did not alter sequence");
  }
  // New pitch mode remains allocation-free with prepared buffers and recalls state.
  {
   auto p=std::make_unique<VirtualCortexProcessor>();layout(*p,1,2);grid(*p,vc::BlockType::Empty);prepare(*p);set(*p,"scOn",1);set(*p,"scPitch",1);set(*p,"scMix",1);set(*p,"scRate",48000);set(*p,"scBits",24);for(auto id:{"scStep1","scStep2","scStep3","scStep4","scStep5","scStep6"})set(*p,id,1);
   juce::AudioBuffer<float>b(2,128);juce::MidiBuffer midi;int crossings=0;float previous=0;
   for(int block=0;block<750;++block){for(int i=0;i<128;++i)b.setSample(0,i,.2f*std::sin(float(6.283185307179586*220*(block*128+i)/48000.)));allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Pitch sequencer allocated on audio thread");for(int i=0;i<128;++i){const float x=b.getSample(0,i);check(std::isfinite(x),"Pitch processor nonfinite");if(block>=375&&previous<=0&&x>0)++crossings;previous=x;}}
   check(std::abs(crossings-440)<12,"Native pitch processor did not shift octave");auto saved=state(*p);auto q=std::make_unique<VirtualCortexProcessor>();restore(*q,saved);check(q->parameters.getRawParameterValue("scPitch")->load()>.5f,"Pitch switch recall failed");
  }
  // Monophonic audio-to-MIDI is emitted at valid sample offsets, never a stuck note.
  {
   auto p=std::make_unique<VirtualCortexProcessor>();layout(*p,1,2);serialDry(*p);prepare(*p);set(*p,"syOn",1);set(*p,"syMidi",1);set(*p,"scOn",1);
   check(p->producesMidi(),"MIDI output capability absent");juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;midi.ensureSize(4096);int ons=0,offs=0;
   for(int block=0;block<200;++block){for(int i=0;i<128;++i)b.setSample(0,i,.2f*std::sin(float(6.283185307*220.*(block*128+i)/48000.)));
    allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Tracking DSP allocated with reserved MIDI buffer");
    for(const auto metadata:midi){const auto msg=metadata.getMessage();check(metadata.samplePosition>=0&&metadata.samplePosition<128,"MIDI offset invalid");if(msg.isNoteOn()){check(msg.getNoteNumber()==57,"Wrong tracked MIDI note");++ons;}if(msg.isNoteOff())++offs;}
   }
   check(ons==1&&offs==0,"Tracking retriggered stable note");set(*p,"bypass",1);p->processBlock(b,midi);for(const auto m:midi)if(m.getMessage().isNoteOff())++offs;check(offs==1,"Bypass left MIDI note active");
   auto saved=state(*p);auto q=std::make_unique<VirtualCortexProcessor>();restore(*q,saved);check(q->parameters.getRawParameterValue("syMidi")->load()>.5f,"Tracking state lost");
  }
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
  for(auto type:{vc::BlockType::Compressor,vc::BlockType::Overdrive,vc::BlockType::Distortion,vc::BlockType::Chorus,vc::BlockType::Flanger,vc::BlockType::Phaser,vc::BlockType::Wah,vc::BlockType::BitCrusher,vc::BlockType::Vibrato,vc::BlockType::RingModulator,vc::BlockType::Gain,vc::BlockType::LowHighCut,vc::BlockType::ToneEQ,vc::BlockType::RoomReverb}){
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,type);prepare(*p);vc::SlotParameters controls;controls.mix=1.f;controls.time=0.f;controls.depth=.3f;p->setGridSlotParameters(0,0,controls);settle(*p,.1f);
   auto recalled=std::make_unique<VirtualCortexProcessor>();prepare(*recalled);restore(*recalled,state(*p));check(recalled->getEditableGrid().get(0,0)->type==type,"New block type lost on state recall");check(std::abs(recalled->getGridSlotParameters(0,0).depth-.3f)<1.e-6f,"New block controls lost on state recall");
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
   for(int step=0;step<8;++step){controls.drive=step/8.f;p->setGridSlotParameters(0,0,controls);for(int ch=0;ch<2;++ch)for(int i=0;i<128;++i)b.setSample(ch,i,.1f*std::sin((i+step*128)*.1f));allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"New effect automation allocated in callback");for(int ch=0;ch<2;++ch)for(int i=0;i<128;++i)check(std::isfinite(b.getSample(ch,i)),"New effect produced non-finite samples");}
  }
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Delay);prepare(*p);vc::SlotParameters controls;controls.repeatFx=true;controls.mix=.5f;p->setGridSlotParameters(0,0,controls);settle(*p,.1f);
   auto recalled=std::make_unique<VirtualCortexProcessor>();prepare(*recalled);restore(*recalled,state(*p));check(recalled->getGridSlotParameters(0,0).repeatFx,"Repeat FX was lost on recall");
   auto oldPreset=state(*recalled);auto xml=juce::AudioProcessor::getXmlFromBinary(oldPreset.getData(),(int)oldPreset.getSize());check(xml!=nullptr,"Preset XML missing");xml->removeAttribute("slot_0_0_repeatFx");juce::MemoryBlock legacy;juce::AudioProcessor::copyXmlToBinary(*xml,legacy);restore(*recalled,legacy);check(!recalled->getGridSlotParameters(0,0).repeatFx,"Legacy preset did not disable Repeat FX");
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
   for(int step=0;step<10;++step){controls.time=step/10.f;controls.depth=1.f-step/10.f;p->setGridSlotParameters(0,0,controls);b.clear();allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Repeat FX allocated in the audio callback");}
  }
  {
   struct TempoHead final:juce::AudioPlayHead {juce::Optional<PositionInfo> getPosition() const override {PositionInfo p;p.setBpm(100.);return p;}} playhead;
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Empty);prepare(*p);p->setPlayHead(&playhead);set(*p,"stereoDelayOn",1);set(*p,"sdSync",1);set(*p,"sdBpm",120);set(*p,"sdBeatsL",.25f);set(*p,"sdBeatsR",.5f);set(*p,"sdMix",1);set(*p,"sdFeedback",0);set(*p,"sdFilterMix",0);settle(*p,0,120);
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;float peak=0;int peakIndex=-1;
   for(int block=0;block<125;++block){b.clear();if(block==0)b.setSample(0,0,.1f);p->processBlock(b,midi);for(int i=0;i<128;++i)if(std::abs(b.getSample(0,i))>peak){peak=std::abs(b.getSample(0,i));peakIndex=block*128+i;}}
   check(std::abs(peakIndex-7200)<=1&&std::abs(peak-.1f)<.0001f,"Host tempo did not control stereo delay timing");
   const auto saved=state(*p);auto restored=std::make_unique<VirtualCortexProcessor>();prepare(*restored);restore(*restored,saved);check(restored->parameters.getRawParameterValue("stereoDelayOn")->load()>.5f,"Stereo delay on-state lost on recall");check(std::abs(restored->parameters.getRawParameterValue("sdBeatsL")->load()-.25f)<.001,"Stereo tempo setting lost on recall");
   auto xml=juce::AudioProcessor::getXmlFromBinary(saved.getData(),(int)saved.getSize());auto tree=juce::ValueTree::fromXml(*xml);for(const auto* id:vc::stereoDelayIDs)tree.removeChild(tree.getChildWithProperty("id",id),nullptr);for(const auto* id:vc::stereoDelayFlagIDs)tree.removeChild(tree.getChildWithProperty("id",id),nullptr);auto legacyXml=tree.createXml();juce::MemoryBlock legacy;juce::AudioProcessor::copyXmlToBinary(*legacyXml,legacy);restore(*restored,legacy);
   check(restored->parameters.getRawParameterValue("stereoDelayOn")->load()<.5f,"Old preset retained enabled stereo delay");check(std::abs(restored->parameters.getRawParameterValue("sdBeatsL")->load()-1.f)<.001,"Old preset did not restore new-parameter defaults");
   set(*p,"sdSync",0);set(*p,"sdModDepth",2);set(*p,"sdDucking",.6f);
   for(int step=0;step<10;++step){set(*p,"sdTimeL",20.f+step*37);set(*p,"sdTape",(float)(step%2));b.clear();allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Stereo delay automation allocated in callback");for(int ch=0;ch<2;++ch)for(int i=0;i<128;++i)check(std::isfinite(b.getSample(ch,i)),"Stereo delay produced non-finite audio");}
   for(int mode=0;mode<2;++mode){if(mode)grid(*p,vc::BlockType::Empty);else{set(*p,"gridMode",0);serialDry(*p);}set(*p,"outputTrim",1);settle(*p,.1f,100);set(*p,"outputTrim",0);check(std::abs(settle(*p,.1f,200))<1.e-6f,"Global trim did not mute stereo delay wet output");}
   p->setPlayHead(nullptr);
  }
  // Exercise actual JUCE processor buffers at the common Anubis rate/buffer range.
  for(double rate:{44100.,48000.,96000.,192000.})for(int size:{32,64,128,256,512}){
   auto p=std::make_unique<VirtualCortexProcessor>();prepare(*p,rate,size);juce::AudioBuffer<float> b(2,size);juce::MidiBuffer midi;
   for(int mode=0;mode<2;++mode){set(*p,"gridMode",(float)mode);for(int block=0;block<4;++block){for(int ch=0;ch<2;++ch)for(int i=0;i<size;++i)b.setSample(ch,i,.15f*std::sin((i+block*size)*.15f));b.setSample(0,0,std::numeric_limits<float>::quiet_NaN());p->processBlock(b,midi);for(int ch=0;ch<2;++ch)for(int i=0;i<size;++i)check(std::isfinite(b.getSample(ch,i))&&std::abs(b.getSample(ch,i))<=1.f,"Processor produced invalid output at rate/buffer combination");}}
  }
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Empty);prepare(*p);set(*p,"stereoReverbOn",1);set(*p,"srDecay",4);set(*p,"srPreGlide",0);set(*p,"srMix",.5f);settle(*p,.1f,100);const auto saved=state(*p);auto restored=std::make_unique<VirtualCortexProcessor>();prepare(*restored);restore(*restored,saved);check(restored->parameters.getRawParameterValue("stereoReverbOn")->load()>.5f,"Reverb enable lost on recall");check(std::abs(restored->parameters.getRawParameterValue("srDecay")->load()-4)<.001f,"Reverb decay lost on recall");check(restored->parameters.getRawParameterValue("srPreGlide")->load()<.5f,"Clean pre-delay mode lost on recall");
   auto xml=juce::AudioProcessor::getXmlFromBinary(saved.getData(),(int)saved.getSize());auto tree=juce::ValueTree::fromXml(*xml);for(const auto* id:vc::stereoReverbIDs)tree.removeChild(tree.getChildWithProperty("id",id),nullptr);tree.removeChild(tree.getChildWithProperty("id","stereoReverbOn"),nullptr);tree.removeChild(tree.getChildWithProperty("id","srPreGlide"),nullptr);auto oldXml=tree.createXml();juce::MemoryBlock legacy;juce::AudioProcessor::copyXmlToBinary(*oldXml,legacy);restore(*restored,legacy);check(restored->parameters.getRawParameterValue("stereoReverbOn")->load()<.5f,"Old preset retained enabled reverb");check(std::abs(restored->parameters.getRawParameterValue("srDecay")->load()-2.5f)<.001f,"Old preset reverb defaults wrong");check(restored->parameters.getRawParameterValue("srPreGlide")->load()>.5f,"Old preset did not preserve glide behavior");
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;for(int step=0;step<20;++step){set(*p,"srPreGlide",step%3?0:1);set(*p,"srPreDelay",step%2?0:500);set(*p,"srSize",step%2?.5f:2);set(*p,"srDecay",.2f+step);b.clear();allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Reverb automation allocated in callback");for(int ch=0;ch<2;++ch)for(int i=0;i<128;++i)check(std::isfinite(b.getSample(ch,i)),"Reverb non-finite output");}
   for(int mode=0;mode<2;++mode){if(mode)grid(*p,vc::BlockType::Empty);else{set(*p,"gridMode",0);serialDry(*p);}set(*p,"outputTrim",1);settle(*p,.1f,100);set(*p,"outputTrim",0);check(std::abs(settle(*p,.1f,200))<1.e-6f,"Output trim did not mute reverb");}
  }
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Empty);prepare(*p);
   set(*p,"stereoDelayOn",1);set(*p,"sdDiffusion",.8f);set(*p,"sdFeedback",.8f);
   const auto saved=state(*p);auto restored=std::make_unique<VirtualCortexProcessor>();prepare(*restored);restore(*restored,saved);
   check(std::abs(restored->parameters.getRawParameterValue("sdDiffusion")->load()-.8f)<.001f,"Diffusion lost on recall");
   auto xml=juce::AudioProcessor::getXmlFromBinary(saved.getData(),(int)saved.getSize());auto tree=juce::ValueTree::fromXml(*xml);
   tree.removeChild(tree.getChildWithProperty("id","sdDiffusion"),nullptr);auto oldXml=tree.createXml();juce::MemoryBlock oldState;juce::AudioProcessor::copyXmlToBinary(*oldXml,oldState);restore(*restored,oldState);
   check(restored->parameters.getRawParameterValue("sdDiffusion")->load()==0.f,"Legacy preset retained diffusion");
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
   for(int step=0;step<100;++step){set(*p,"sdDiffusion",step%2?0:1);b.clear();b.setSample(0,0,.1f);allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;
    check(allocationProbe::count==0,"Diffusion automation allocated in callback");for(int i=0;i<128;++i)check(std::isfinite(b.getSample(0,i)),"Non-finite diffusion output");}
  }
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Empty);prepare(*p);set(*p,"mhOn",1);set(*p,"mhMix",.7f);set(*p,"mh4Time",789);
   auto saved=state(*p);auto restored=std::make_unique<VirtualCortexProcessor>();prepare(*restored);restore(*restored,saved);check(std::abs(restored->parameters.getRawParameterValue("mh4Time")->load()-789)<.01f,"Echo state lost");
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
   for(int step=0;step<100;++step){set(*p,"mh1Time",step%2?1:4000);set(*p,"mh1Pan",step%2?-1:1);set(*p,"mhOn",step%3?1:0);b.clear();b.setSample(0,0,.1f);allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Echo allocated in callback");for(int i=0;i<128;++i)check(std::isfinite(b.getSample(0,i)),"Echo non-finite output");}
  }
  {
   auto p=std::make_unique<VirtualCortexProcessor>();grid(*p,vc::BlockType::Empty);prepare(*p);set(*p,"llOn",1);set(*p,"llLevel",1);p->loopCommand(vc::LayerLoom::Record);
   juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
   for(int block=0;block<40;++block){for(int i=0;i<128;++i){b.setSample(0,i,.1f);b.setSample(1,i,-.1f);}allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Loop recording allocated");}
   p->loopCommand(vc::LayerLoom::Record);b.clear();p->processBlock(b,midi);check((p->loopStatus()&255)==1,"Processor loop recording failed");
   double energy=0;for(int block=0;block<20;++block){b.clear();p->processBlock(b,midi);for(int i=0;i<128;++i)energy+=std::abs(b.getSample(0,i));}check(energy>100,"Recorded loop is inaudible");
   p->loopCommand(vc::LayerLoom::Undo);b.clear();p->processBlock(b,midi);check((p->loopStatus()&255)==0,"Processor loop undo failed");p->loopCommand(vc::LayerLoom::Redo);b.clear();p->processBlock(b,midi);check((p->loopStatus()&255)==1,"Processor loop redo failed");
   set(*p,"ieOn",1);set(*p,"ieA",12);set(*p,"llRate",.5f);auto saved=state(*p);auto q=std::make_unique<VirtualCortexProcessor>();prepare(*q);restore(*q,saved);check(q->parameters.getRawParameterValue("ieA")->load()==12,"Interval recall failed");check(q->loopStatus()==0,"Session-only audio incorrectly reported restored");
   p->loopCommand(vc::LayerLoom::Clear);for(int block=0;block<50;++block){set(*p,"ieA",block%2?-12:12);set(*p,"ieTime",block%2?1:4000);b.clear();allocationProbe::count=0;allocationProbe::enabled=true;p->processBlock(b,midi);allocationProbe::enabled=false;check(allocationProbe::count==0,"Creative automation allocated");for(int i=0;i<128;++i)check(std::isfinite(b.getSample(0,i)),"Creative non-finite audio");}
  }
  {
   auto p=std::make_unique<VirtualCortexProcessor>();prepare(*p);grid(*p,vc::BlockType::Delay);
   check(!p->recallScene(0)&&!p->storeScene(-1)&&!p->hasScene(8),"Invalid or empty scene accepted");
   for(int i=0;i<8;++i){
    set(*p,"ieA",(float)i);set(*p,"scPitch",i%2);p->setGridLaneEnabled(1,i%2!=0);
    auto slot=p->getGridSlotParameters(0,0);slot.mix=.1f*(i+1);p->setGridSlotParameters(0,0,slot);
    auto block=*p->getEditableGrid().get(0,0);block.enabled=i%2!=0;p->setGridBlock(0,0,block);
    check(p->storeScene(i),"Scene store failed");
   }
   const auto saved=state(*p);auto q=std::make_unique<VirtualCortexProcessor>();prepare(*q);restore(*q,saved);
   check(q->selectedScene()==7,"Selected scene not persisted");
   juce::AudioBuffer<float> audio(2,128);juce::MidiBuffer midi;
   for(int i=0;i<8;++i){
    check(q->hasScene(i)&&q->recallScene(i),"Scene lost on state restore");
    check(q->parameters.getRawParameterValue("ieA")->load()==(float)i,"Scene parameter recall failed");
    check((q->parameters.getRawParameterValue("scPitch")->load()>.5f)==(i%2!=0),"Scene boolean recall failed");
    check(q->isGridLaneEnabled(1)==(i%2!=0),"Scene lane recall failed");
    check(q->getEditableGrid().get(0,0)->enabled==(i%2!=0),"Scene bypass recall failed");
    check(std::abs(q->getGridSlotParameters(0,0).mix-.1f*(i+1))<1.e-6f,"Scene slot recall failed");
    audio.clear();allocationProbe::count=0;allocationProbe::enabled=true;q->processBlock(audio,midi);allocationProbe::enabled=false;
    check(allocationProbe::count==0,"Post-scene audio callback allocated");
    for(int ch=0;ch<2;++ch)for(int j=0;j<128;++j)check(std::isfinite(audio.getSample(ch,j)),"Scene produced invalid audio");
   }
   auto replacement=*q->getEditableGrid().get(0,0);replacement.type=vc::BlockType::Amp;q->setGridBlock(0,0,replacement);
   auto slot=q->getGridSlotParameters(0,0);slot.mix=.37f;q->setGridSlotParameters(0,0,slot);q->recallScene(0);
   check(q->getEditableGrid().get(0,0)->type==vc::BlockType::Amp&&std::abs(q->getGridSlotParameters(0,0).mix-.37f)<1.e-6f,"Scene changed replacement device");
   auto xml=juce::AudioProcessor::getXmlFromBinary(saved.getData(),(int)saved.getSize());auto tree=juce::ValueTree::fromXml(*xml);
   tree.removeChild(tree.getChildWithName("ScenesV1"),nullptr);auto legacyXml=tree.createXml();juce::MemoryBlock legacy;
   juce::AudioProcessor::copyXmlToBinary(*legacyXml,legacy);restore(*q,legacy);
   check(q->selectedScene()==-1&&!q->hasScene(0)&&!q->hasScene(7),"Old preset retained unrelated scenes");
  }
  std::cout<<"PASS: mono/stereo, trim, callback allocations, UTF-8 assets, preset clearing, and rate/buffer matrix\n";return 0;
 }catch(const std::exception& e){allocationProbe::enabled=false;std::cerr<<e.what()<<'\n';return 1;}
}
