#include "DSP/CabinetIRWav.h"
#include "DSP/CoreDSP.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <cstdint>

VirtualCortexProcessor::VirtualCortexProcessor()
 : AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true)
                                  .withOutput("Output",juce::AudioChannelSet::stereo(),true)),
   parameters(*this,nullptr,"PARAMETERS",makeLayout()) {
 vc::SignalGrid grid;
 grid.set(0,0,{vc::BlockType::Gate,true});grid.set(0,1,{vc::BlockType::Amp,true});
 grid.set(0,2,{vc::BlockType::Cabinet,true});grid.set(1,0,{vc::BlockType::Amp,true});
 grid.set(1,1,{vc::BlockType::EQ,true});grid.set(1,2,{vc::BlockType::Delay,true});
 grid.set(1,3,{vc::BlockType::Reverb,true});
 const auto bytes=vc::encodeGrid(grid);
 for(std::size_t r=0;r<4;++r){std::uint64_t packed=0;
  for(std::size_t c=0;c<8;++c)packed|=std::uint64_t(bytes[2+r*8+c])<<(c*8);
  gridRows[r].store(packed);
 }
}

vc::SignalGrid VirtualCortexProcessor::getEditableGrid() const noexcept {
 vc::GridBytes bytes{};bytes[0]='V';bytes[1]=1;
 for(std::size_t r=0;r<4;++r){auto packed=gridRows[r].load(std::memory_order_acquire);
  for(std::size_t c=0;c<8;++c)bytes[2+r*8+c]=static_cast<std::uint8_t>(packed>>(c*8));
 }
 vc::SignalGrid grid;vc::decodeGrid(bytes,grid);return grid;
}
bool VirtualCortexProcessor::setGridBlock(std::size_t row,std::size_t col,vc::BlockSlot slot) noexcept {
 if(!vc::SignalGrid::valid(row,col)||!vc::SignalGrid::validType(slot.type))return false;
 const std::uint64_t shift=col*8;const std::uint64_t mask=std::uint64_t(255)<<shift;
 const std::uint64_t value=std::uint64_t(static_cast<std::uint8_t>(slot.type)|(slot.enabled?128:0))<<shift;
 auto previous=gridRows[row].load(std::memory_order_relaxed);
 while(!gridRows[row].compare_exchange_weak(previous,(previous&~mask)|value,std::memory_order_release,std::memory_order_relaxed)){}
 return true;
}
bool VirtualCortexProcessor::setGridLaneEnabled(std::size_t row,bool enabled) noexcept {
 if(row>=4)return false;const auto bit=static_cast<std::uint8_t>(1u<<row);
 if(enabled)gridLanes.fetch_or(bit);else gridLanes.fetch_and(static_cast<std::uint8_t>(~bit));
 return true;
}
bool VirtualCortexProcessor::isGridLaneEnabled(std::size_t row) const noexcept {
 return row<4 && (gridLanes.load()&(1u<<row))!=0;
}
bool VirtualCortexProcessor::setGridSlotParameters(std::size_t row,std::size_t col,const vc::SlotParameters& p) noexcept {
 if(!vc::SignalGrid::valid(row,col) || !vc::SlotParameters::valid(p)) return false;
 slotControls[row][col].store(p);return true;
}
vc::SlotParameters VirtualCortexProcessor::getGridSlotParameters(std::size_t row,std::size_t col) const noexcept {
 return vc::SignalGrid::valid(row,col) ? slotControls[row][col].load() : vc::SlotParameters{};
}
void VirtualCortexProcessor::updateGridRouting() noexcept {
 std::array<std::uint64_t,4> current{};bool changed=false;
 for(std::size_t r=0;r<4;++r){current[r]=gridRows[r].load(std::memory_order_acquire);changed|=current[r]!=appliedGridRows[r];}
 const auto lanes=gridLanes.load(std::memory_order_acquire);changed|=lanes!=appliedGridLanes;
 if(!changed)return;
 vc::GridBytes bytes{};bytes[0]='V';bytes[1]=1;
 for(std::size_t r=0;r<4;++r)for(std::size_t c=0;c<8;++c)bytes[2+r*8+c]=static_cast<std::uint8_t>(current[r]>>(c*8));
 vc::SignalGrid grid;
 if(!vc::decodeGrid(bytes,grid))return;
 std::array<bool,4> enabled{};for(std::size_t r=0;r<4;++r)enabled[r]=(lanes&(1u<<r))!=0;
 if(gridAudio.configure(grid,enabled)){appliedGridRows=current;appliedGridLanes=lanes;gridAudioReady=true;}
}


juce::AudioProcessorValueTreeState::ParameterLayout VirtualCortexProcessor::makeLayout(){
 juce::AudioProcessorValueTreeState::ParameterLayout p;
 auto addFloat=[&](const char* id,const char* label,float min,float max,float initial){
  p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},label,juce::NormalisableRange<float>(min,max),initial));
 };
 auto addBool=[&](const char* id,const char* label,bool initial){
  p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{id,1},label,initial));
 };
 addFloat("gate", "Gate threshold (dB)",-80.f,-20.f,-62.f);
 addFloat("drive","Drive",0.f,1.f,0.35f);
 addFloat("ampBias","Amp bias",-0.5f,0.5f,0.f);
 addFloat("inputHP","Input high-pass (Hz)",20.f,200.f,55.f);
 addFloat("tone","Tone",0.f,1.f,0.6f);
 addFloat("level","Output",0.f,1.f,0.65f);
 addFloat("cab","Cabinet mix",0.f,1.f,1.f);
 addFloat("cabCut","Cabinet low-pass (Hz)",1800.f,11000.f,4700.f);
 addFloat("delayTime","Delay time (ms)",60.f,800.f,320.f);
 addFloat("delayFeedback","Delay feedback",0.f,0.8f,0.3f);
 addFloat("delayMix","Delay mix",0.f,0.65f,0.f);
 addFloat("reverbMix","Reverb mix",0.f,0.55f,0.f);
 addFloat("tremRate","Tremolo rate (Hz)",0.5f,12.f,4.f);
 addFloat("tremDepth","Tremolo depth",0.f,1.f,0.4f);
 addFloat("outputTrim","Output trim",0.f,2.f,1.f);
 addFloat("eqLow","Post-cab bass (dB)",-12.f,12.f,0.f);
 addFloat("eqMid","Post-cab mid (dB)",-12.f,12.f,0.f);
 addFloat("eqHigh","Post-cab treble (dB)",-12.f,12.f,0.f);
 addBool("gateOn","Gate",true);
 addBool("ampOn","Amp",true);
 addBool("cabOn","Cabinet",true);
 addBool("delayOn","Delay",false);
 addBool("reverbOn","Reverb",false);
 addBool("tremOn","Tremolo",false);
 addBool("eqOn","Post-cab EQ",true);
 addBool("bypass","Global bypass",false);
 addBool("irOn","Cabinet IR",false);
 addBool("gridMode","Parallel grid (experimental)",false);
 return p;
}

bool VirtualCortexProcessor::isBusesLayoutSupported(const BusesLayout& layout) const {
 const auto in=layout.getMainInputChannelSet(),out=layout.getMainOutputChannelSet();
 return (in==juce::AudioChannelSet::mono() || in==juce::AudioChannelSet::stereo()) &&
        (out==juce::AudioChannelSet::mono() || out==juce::AudioChannelSet::stereo()) && in==out;
}

void VirtualCortexProcessor::prepareToPlay(double sampleRate,int samplesPerBlock){
 fs=juce::jmax(8000.0,sampleRate);
 // Routing changes are published through atomic row snapshots.
 updateGridRouting();
 gridAudio.prepare(fs);
 cabinetConvolution.prepare({fs, (juce::uint32)juce::jmax(1, samplesPerBlock), (juce::uint32)juce::jmax(1,getTotalNumOutputChannels())});
 channels.clear();
 channels.resize((size_t)juce::jmax(1,getTotalNumOutputChannels()));
 for(auto& s:channels){
  s.lowEQ.coefficients=juce::dsp::IIR::Coefficients<float>::makeLowShelf(fs,140.0,0.707,1.0);
  s.midEQ.coefficients=juce::dsp::IIR::Coefficients<float>::makePeakFilter(fs,850.0,0.85,1.0);
  s.highEQ.coefficients=juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs,3500.0,0.707,1.0);
  s.lowEQ.reset();s.midEQ.reset();s.highEQ.reset();
  s.inputFilter.prepare(fs);
  s.outputSmoother.prepare(fs, 12.f);
  s.outputSmoother.reset(parameters.getRawParameterValue("outputTrim")->load());
  s.inputFilter.setCutoff(55.f);
  s.delay.assign((size_t)(fs*0.85)+2,0.f);
  s.combA.assign((size_t)(fs*0.031)+1,0.f);
  s.combB.assign((size_t)(fs*0.043)+1,0.f);
  s.gateEnvelope=s.cabLP=s.cabHP=s.toneLP=s.tremPhase=0.f;
  s.delayIndex=s.indexA=s.indexB=0;
 }
}

void VirtualCortexProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&){
 juce::ScopedNoDenormals noDenormals;
 const auto param=[this](const char* id){return parameters.getRawParameterValue(id)->load();};
 if (b.getNumChannels() == 0 || b.getNumSamples() == 0 || channels.empty()) return;
 // Process only the supported main bus channels; clear unexpected extra channels.
 for (int ch = getTotalNumInputChannels(); ch < b.getNumChannels(); ++ch)
  b.clear(ch, 0, b.getNumSamples());
 // Apply staged changes even while bypassed or in the serial engine.
 // Otherwise model/IR updates can remain indefinitely pending until the user
 // re-enables grid processing, and the first unbypassed block may be stale.
 updateGridRouting();
 commitPendingGridIR();
 commitPendingNeuralModels();
 if(param("bypass")>0.5f)return; // Dry passthrough.
 if (param("gridMode")>0.5f) {
  // No heap allocation: snapshot controls before processing this audio block.
  for(std::size_t row=0;row<4;++row) for(std::size_t col=0;col<8;++col)
   gridAudio.setSlotParameters(row,col,slotControls[row][col].load());
 }
 if (param("gridMode")>0.5f && gridAudioReady) {
  // JUCE provides planar audio; portable engine expects interleaved frames.
  // Fixed-size stack chunks keep the audio callback allocation-free.
  constexpr int chunkFrames=128;
  std::array<float,chunkFrames*2> interleaved{};
  const int count=juce::jmin(b.getNumChannels(),2);
  for(int offset=0;offset<b.getNumSamples();offset+=chunkFrames){
   const int frames=juce::jmin(chunkFrames,b.getNumSamples()-offset);
   for(int i=0;i<frames;++i)
    for(int ch=0;ch<count;++ch)
     interleaved[static_cast<size_t>(i*count+ch)]=b.getSample(ch,offset+i);
   if(!gridAudio.process(interleaved.data(),static_cast<size_t>(frames),static_cast<size_t>(count))){
    b.clear();return;
   }
   for(int i=0;i<frames;++i)
    for(int ch=0;ch<count;++ch)
     b.setSample(ch,offset+i,interleaved[static_cast<size_t>(i*count+ch)]);
  }
  return;
 }
 const float gateThreshold=juce::Decibels::decibelsToGain(param("gate"));
 const float drive=param("drive"),tone=param("tone"),level=param("level"),cabMix=param("cab");
 const float bias=param("ampBias"),tremDepth=param("tremDepth"),tremRate=param("tremRate");
 const float feedback=param("delayFeedback"),delayMix=param("delayMix"),reverbMix=param("reverbMix");
 const float outputTrim=param("outputTrim");
 const bool gateOn=param("gateOn")>0.5f,ampOn=param("ampOn")>0.5f,cabOn=param("cabOn")>0.5f;
 const bool eqOn=param("eqOn")>0.5f,irOn=param("irOn")>0.5f;
 const bool delayOn=param("delayOn")>0.5f,reverbOn=param("reverbOn")>0.5f,tremOn=param("tremOn")>0.5f;
 const bool useIR = cabOn && irOn && cabinetConvolution.getCurrentIRSize() > 0;
 const float gain=1.f+drive*38.f;
 const float toneCoeff=std::exp(-2.f*juce::MathConstants<float>::pi*(400.f+tone*10500.f)/(float)fs);
 const float cabLowCoeff=std::exp(-2.f*juce::MathConstants<float>::pi*param("cabCut")/(float)fs);
 const float cabHighCoeff=std::exp(-2.f*juce::MathConstants<float>::pi*95.f/(float)fs);
 const float inputCutoff=param("inputHP");
 const float gateAttack=std::exp(-1.f/(0.002f*(float)fs));
 const float gateRelease=std::exp(-1.f/(0.075f*(float)fs));
 const size_t delaySamples=(size_t)juce::jlimit(1,(int)channels.front().delay.size()-1,(int)(fs*param("delayTime")*0.001));
 const float eqLow=param("eqLow"),eqMid=param("eqMid"),eqHigh=param("eqHigh");
 // PRE-IR: gate -> amp -> cabinet filter (if no IR). EQ is applied post-IR.
 for(int ch=0;ch<b.getNumChannels();++ch){
  if((size_t)ch>=channels.size()){b.clear(ch,0,b.getNumSamples());continue;}
  auto& s=channels[(size_t)ch];auto* data=b.getWritePointer(ch);
  if (s.lastInputCutoff != inputCutoff) { s.inputFilter.setCutoff(inputCutoff); s.lastInputCutoff = inputCutoff; }
  if (s.lastEqLow != eqLow) { *s.lowEQ.coefficients=*juce::dsp::IIR::Coefficients<float>::makeLowShelf(fs,140.0,0.707,juce::Decibels::decibelsToGain(eqLow)); s.lastEqLow=eqLow; }
  if (s.lastEqMid != eqMid) { *s.midEQ.coefficients=*juce::dsp::IIR::Coefficients<float>::makePeakFilter(fs,850.0,0.85,juce::Decibels::decibelsToGain(eqMid)); s.lastEqMid=eqMid; }
  if (s.lastEqHigh != eqHigh) { *s.highEQ.coefficients=*juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs,3500.0,0.707,juce::Decibels::decibelsToGain(eqHigh)); s.lastEqHigh=eqHigh; }
  for(int i=0;i<b.getNumSamples();++i){
   float x=data[i];
   x=s.inputFilter.process(x);
   if(gateOn){
    const float absX=std::abs(x),c=absX>s.gateEnvelope?gateAttack:gateRelease;
    s.gateEnvelope=c*s.gateEnvelope+(1.f-c)*absX;
    x*=juce::jlimit(0.f,1.f,(s.gateEnvelope-gateThreshold*0.5f)/(gateThreshold*0.5f+1.e-9f));
   }
   if(ampOn){
    x=vc::saturate(x,gain,bias);
    s.toneLP=(1.f-toneCoeff)*x+toneCoeff*s.toneLP;
    x=s.toneLP*level*1.4f;
   }
   if(cabOn && !useIR){
    s.cabLP=(1.f-cabLowCoeff)*x+cabLowCoeff*s.cabLP;
    s.cabHP=(1.f-cabHighCoeff)*s.cabLP+cabHighCoeff*s.cabHP;
    x=x*(1.f-cabMix)+(s.cabLP-s.cabHP)*1.4f*cabMix;
   }
   data[i]=std::isfinite(x)?x:0.f;
  }
 }
 // Cabinet convolution is before time-based effects, rather than after them.
 if(useIR){
  juce::dsp::AudioBlock<float> block(b);
  juce::dsp::ProcessContextReplacing<float> context(block);
  cabinetConvolution.process(context);
 }
 // POST-IR: EQ -> delay -> ambience -> tremolo -> output.
 for(int ch=0;ch<b.getNumChannels();++ch){
  if((size_t)ch>=channels.size())continue;
  auto& s=channels[(size_t)ch];auto* data=b.getWritePointer(ch);
  for(int i=0;i<b.getNumSamples();++i){
   float x=data[i];
   if(eqOn){x=s.lowEQ.processSample(x);x=s.midEQ.processSample(x);x=s.highEQ.processSample(x);}
   const size_t read=(s.delayIndex+s.delay.size()-delaySamples)%s.delay.size();
   const float delayed=s.delay[read];
   s.delay[s.delayIndex]=delayOn ? juce::jlimit(-4.f,4.f,x+delayed*feedback) : 0.f;
   s.delayIndex=(s.delayIndex+1)%s.delay.size();
   if(delayOn)x=x*(1.f-delayMix)+delayed*delayMix;
   const float a=s.combA[s.indexA],c=s.combB[s.indexB];
   s.combA[s.indexA]=reverbOn ? juce::jlimit(-4.f,4.f,x+a*0.69f) : 0.f;
   s.combB[s.indexB]=reverbOn ? juce::jlimit(-4.f,4.f,x+c*0.63f) : 0.f;
   s.indexA=(s.indexA+1)%s.combA.size();s.indexB=(s.indexB+1)%s.combB.size();
   if(reverbOn)x=x*(1.f-reverbMix)+0.28f*(a+c)*reverbMix;
   if(tremOn){
    x*=1.f-tremDepth*0.5f*(1.f+std::sin(s.tremPhase));
    s.tremPhase+=2.f*juce::MathConstants<float>::pi*tremRate/(float)fs;
    if(s.tremPhase>2.f*juce::MathConstants<float>::pi)s.tremPhase-=2.f*juce::MathConstants<float>::pi;
   }
   data[i]=std::isfinite(x)?juce::jlimit(-1.f,1.f,x*s.outputSmoother.process(outputTrim)):0.f;
  }
 }
}
void VirtualCortexProcessor::commitPendingGridIR() noexcept {
 pendingGridIRs.consumeOne([this](std::size_t row,std::size_t col,
                                    const float* taps,std::size_t count) noexcept {
  gridAudio.setCabinetIR(row,col,taps,count);
 });
}
void VirtualCortexProcessor::commitPendingNeuralModels() noexcept {
 pendingNeuralModels.consumeOne([this](std::size_t row,std::size_t col,
                                       const vc::NeuralAmpModel* model) noexcept {
  gridAudio.setNeuralAmp(row,col,model);
 });
}
bool VirtualCortexProcessor::loadGridNeuralModel(const juce::File& file,
                                                std::size_t row,std::size_t col) {
 if(!vc::SignalGrid::valid(row,col) || !file.existsAsFile()) return false;
 const auto grid=getEditableGrid();
 const auto slot=grid.get(row,col);
 if(!slot || !slot->enabled || slot->type!=vc::BlockType::Amp) return false;
 vc::NeuralAmpModel model;
 try { model=vc::loadNeuralAmpModel(file.getFullPathName().toStdString()); }
 catch(const std::exception&) { return false; }
 if(!pendingNeuralModels.submit(row,col,&model)) return false;
 gridNeuralPaths[row][col]=file.getFullPathName();
 return true;
}
juce::String VirtualCortexProcessor::getGridNeuralModelName(std::size_t row,std::size_t col) const {
 if(!vc::SignalGrid::valid(row,col)) return "Invalid slot";
 const auto& path=gridNeuralPaths[row][col];
 return path.isEmpty()?"No neural model":juce::File(path).getFileName();
}
bool VirtualCortexProcessor::loadGridCabinetIR(const juce::File& file,
                                                std::size_t row, std::size_t col) {
 if (!vc::SignalGrid::valid(row,col) || !file.existsAsFile() ||
     !file.hasFileExtension("wav")) return false;
 // Decode/resample outside the audio callback. Oversized IRs are rejected.
 vc::LoadedCabinetIR decoded;
 try {
  decoded=vc::loadCabinetIRWav(file.getFullPathName().toStdString(),
                        static_cast<unsigned>(fs), vc::PortableAudioEngine::maxIrTaps);
 } catch (const std::exception&) { return false; }
 if (decoded.taps.empty()) return false;
 if (!pendingGridIRs.submit(row,col,decoded.taps.data(),decoded.taps.size())) return false;
 gridIRPaths[row][col]=file.getFullPathName();
 return true;
}
juce::String VirtualCortexProcessor::getGridCabinetIRName(std::size_t row,
                                                           std::size_t col) const {
 if (!vc::SignalGrid::valid(row,col)) return "Invalid slot";
 const auto& path=gridIRPaths[row][col];
 return path.isEmpty() ? "No grid IR" : juce::File(path).getFileName();
}
bool VirtualCortexProcessor::loadCabinetIR(const juce::File& file){
 if(!file.existsAsFile() || !file.hasFileExtension("wav;aiff;aif")) return false;
 cabinetConvolution.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
                                      juce::dsp::Convolution::Trim::yes, 0,
                                      juce::dsp::Convolution::Normalise::yes);
 cabinetIRName=file.getFileName();
 cabinetIRPath=file.getFullPathName();
 return true;
}
void VirtualCortexProcessor::getStateInformation(juce::MemoryBlock& dest){
 auto state=parameters.copyState();
 state.setProperty("cabinetIRPath",cabinetIRPath, nullptr);
 const auto bytes=vc::encodeGrid(getEditableGrid());
 state.setProperty("gridV1",juce::String::toHexString(bytes.data(),static_cast<int>(bytes.size()),0),nullptr);
 state.setProperty("gridLanes",static_cast<int>(gridLanes.load()),nullptr);
 for(std::size_t row=0;row<4;++row)for(std::size_t col=0;col<8;++col){
  const auto p=getGridSlotParameters(row,col);
  const auto key="slot_"+juce::String(static_cast<int>(row))+"_"+juce::String(static_cast<int>(col))+"_";
  state.setProperty(key+"drive",p.drive,nullptr);state.setProperty(key+"mix",p.mix,nullptr);
  state.setProperty(key+"time",p.time,nullptr);state.setProperty(key+"feedback",p.feedback,nullptr);
  state.setProperty(key+"rate",p.rate,nullptr);state.setProperty(key+"depth",p.depth,nullptr);
  state.setProperty(key+"irPath",gridIRPaths[row][col],nullptr);
  state.setProperty(key+"neuralPath",gridNeuralPaths[row][col],nullptr);
 }
 std::unique_ptr<juce::XmlElement> xml(state.createXml());copyXmlToBinary(*xml,dest);
}
void VirtualCortexProcessor::setStateInformation(const void* data,int size){
 std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data,size));
 if(xml&&xml->hasTagName(parameters.state.getType())){
  auto restored=juce::ValueTree::fromXml(*xml);
  const auto path=restored.getProperty("cabinetIRPath").toString();
  const auto gridHex=restored.getProperty("gridV1").toString();
  if(gridHex.length()==static_cast<int>(vc::GridBytes{}.size()*2)){
   vc::GridBytes bytes{};bool valid=true;
   for(std::size_t i=0;i<bytes.size();++i){
    const auto hi=juce::CharacterFunctions::getHexDigitValue(gridHex[static_cast<int>(i*2)]);
    const auto lo=juce::CharacterFunctions::getHexDigitValue(gridHex[static_cast<int>(i*2+1)]);
    if(hi<0||lo<0){valid=false;break;}bytes[i]=static_cast<std::uint8_t>((hi<<4)|lo);
   }
   vc::SignalGrid grid;
   if(valid&&vc::decodeGrid(bytes,grid)){
    for(std::size_t r=0;r<4;++r)for(std::size_t c=0;c<8;++c)setGridBlock(r,c,*grid.get(r,c));
    gridLanes.store(static_cast<std::uint8_t>(juce::jlimit(0,15,static_cast<int>(restored.getProperty("gridLanes",3)))));
   }
  }
  for(std::size_t row=0;row<4;++row)for(std::size_t col=0;col<8;++col){
   auto p=getGridSlotParameters(row,col);
   const auto key="slot_"+juce::String(static_cast<int>(row))+"_"+juce::String(static_cast<int>(col))+"_";
   if(restored.hasProperty(key+"drive"))p.drive=static_cast<float>(restored.getProperty(key+"drive"));
   if(restored.hasProperty(key+"mix"))p.mix=static_cast<float>(restored.getProperty(key+"mix"));
   if(restored.hasProperty(key+"time"))p.time=static_cast<float>(restored.getProperty(key+"time"));
   if(restored.hasProperty(key+"feedback"))p.feedback=static_cast<float>(restored.getProperty(key+"feedback"));
   if(restored.hasProperty(key+"rate"))p.rate=static_cast<float>(restored.getProperty(key+"rate"));
   if(restored.hasProperty(key+"depth"))p.depth=static_cast<float>(restored.getProperty(key+"depth"));
   setGridSlotParameters(row,col,p);
   const auto neuralPath=restored.getProperty(key+"neuralPath").toString();
   if(neuralPath.isNotEmpty() && juce::File(neuralPath).existsAsFile())
    loadGridNeuralModel(juce::File(neuralPath),row,col);
   const auto gridPath=restored.getProperty(key+"irPath").toString();
   if(gridPath.isNotEmpty() && juce::File(gridPath).existsAsFile())
    loadGridCabinetIR(juce::File(gridPath),row,col);
  }
  parameters.replaceState(restored);
  if(path.isNotEmpty() && juce::File(path).existsAsFile()) loadCabinetIR(juce::File(path));
  else { cabinetIRPath.clear(); cabinetIRName = "No IR loaded"; }
 }
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new VirtualCortexProcessor();}
