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
 cached.gate=parameters.getRawParameterValue("gate");
 cached.drive=parameters.getRawParameterValue("drive");
 cached.ampBias=parameters.getRawParameterValue("ampBias");
 cached.inputHP=parameters.getRawParameterValue("inputHP");
 cached.tone=parameters.getRawParameterValue("tone");
 cached.level=parameters.getRawParameterValue("level");
 cached.cab=parameters.getRawParameterValue("cab");
 cached.cabCut=parameters.getRawParameterValue("cabCut");
 cached.delayTime=parameters.getRawParameterValue("delayTime");
 cached.delayFeedback=parameters.getRawParameterValue("delayFeedback");
 cached.delayMix=parameters.getRawParameterValue("delayMix");
 cached.reverbMix=parameters.getRawParameterValue("reverbMix");
 cached.tremRate=parameters.getRawParameterValue("tremRate");
 cached.tremDepth=parameters.getRawParameterValue("tremDepth");
 cached.outputTrim=parameters.getRawParameterValue("outputTrim");
 cached.eqLow=parameters.getRawParameterValue("eqLow");
 cached.eqMid=parameters.getRawParameterValue("eqMid");
 cached.eqHigh=parameters.getRawParameterValue("eqHigh");
 cached.gateOn=parameters.getRawParameterValue("gateOn");
 cached.ampOn=parameters.getRawParameterValue("ampOn");
 cached.cabOn=parameters.getRawParameterValue("cabOn");
 cached.delayOn=parameters.getRawParameterValue("delayOn");
 cached.reverbOn=parameters.getRawParameterValue("reverbOn");
 cached.tremOn=parameters.getRawParameterValue("tremOn");
 cached.eqOn=parameters.getRawParameterValue("eqOn");
 cached.bypass=parameters.getRawParameterValue("bypass");
 cached.irOn=parameters.getRawParameterValue("irOn");
 cached.gridMode=parameters.getRawParameterValue("gridMode");
 for(std::size_t i=0;i<stereoParameters.size();++i)stereoParameters[i]=parameters.getRawParameterValue(vc::stereoDelayIDs[i]);
 for(std::size_t i=0;i<stereoFlags.size();++i)stereoFlags[i]=parameters.getRawParameterValue(vc::stereoDelayFlagIDs[i]);
 for(std::size_t i=0;i<reverbParameters.size();++i)reverbParameters[i]=parameters.getRawParameterValue(vc::stereoReverbIDs[i]);
 const char* mhIDs[]={"mh1Time","mh1Level","mh1Pan","mh1Feedback","mh2Time","mh2Level","mh2Pan","mh2Feedback","mh3Time","mh3Level","mh3Pan","mh3Feedback","mh4Time","mh4Level","mh4Pan","mh4Feedback","mhMix"};for(std::size_t i=0;i<17;++i)mhParameters[i]=parameters.getRawParameterValue(mhIDs[i]);mhEnabled=parameters.getRawParameterValue("mhOn");
 reverbEnabled=parameters.getRawParameterValue("stereoReverbOn");
 reverbPreGlide=parameters.getRawParameterValue("srPreGlide");

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
 vc::GridBytes bytes{};bytes[0]='V';bytes[1]=2;
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
 vc::GridBytes bytes{};bytes[0]='V';bytes[1]=2;
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
 addFloat("sdTimeL","Stereo delay left (ms)",1.0f,16000.0f,320.0f);
 addFloat("sdTimeR","Stereo delay right (ms)",1.0f,16000.0f,480.0f);
 addFloat("sdFeedback","Stereo delay feedback",0.0f,0.95f,0.35f);
 addFloat("sdCross","Stereo delay cross feedback",0.0f,1.0f,0.0f);
 addFloat("sdLowCut","Stereo delay high-pass (Hz)",20.0f,1000.0f,40.0f);
 addFloat("sdHighCut","Stereo delay low-pass (Hz)",1000.0f,20000.0f,16000.0f);
 addFloat("sdMix","Stereo delay mix",0.0f,1.0f,0.2f);
 addFloat("sdModRate","Stereo delay modulation (Hz)",0.05f,8.0f,0.5f);
 addFloat("sdModDepth","Stereo delay modulation depth (ms)",0.0f,10.0f,0.0f);
 addFloat("sdDucking","Stereo delay ducking",0.0f,1.0f,0.0f);
 addFloat("sdDuckRelease","Stereo delay duck release (ms)",20.0f,2000.0f,250.0f);
 addFloat("sdWidth","Stereo delay width",0.0f,1.5f,1.0f);
 addFloat("sdWetLevel","Stereo delay wet level (dB)",-24.0f,12.0f,0.0f);
 addFloat("sdGlide","Stereo delay glide (ms)",5.0f,500.0f,50.0f);
 addFloat("sdBpm","Stereo delay tempo",20.0f,300.0f,120.0f);
 addFloat("sdBeatsL","Stereo delay left beats",0.125f,4.0f,1.0f);
 addFloat("sdBeatsR","Stereo delay right beats",0.125f,4.0f,1.5f);
 addFloat("sdFilterMix","Stereo delay loop filter mix",0.0f,1.0f,1.0f);
 addBool("stereoDelayOn","Post-chain stereo delay",false);
 addBool("sdPingPong","Stereo delay ping-pong",false);
 addBool("sdTape","Stereo delay pitch glide",false);
 addBool("sdSync","Stereo delay tempo sync",false);
 addFloat("srDecay","Stereo reverb Decay (s)",0.2f,20.0f,2.5f);
 addFloat("srSize","Stereo reverb Size",0.5f,2.0f,1.0f);
 addFloat("srPreDelay","Stereo reverb Pre-delay (ms)",0.0f,500.0f,20.0f);
 addFloat("srDiffusion","Stereo reverb Diffusion",0.0f,1.0f,0.7f);
 addFloat("srEarly","Stereo reverb Early balance",0.0f,1.0f,0.2f);
 addFloat("srMix","Stereo reverb Mix",0.0f,1.0f,0.2f);
 addFloat("srLowDecay","Stereo reverb Low decay ratio",0.25f,2.0f,1.0f);
 addFloat("srHighDecay","Stereo reverb High decay ratio",0.1f,1.0f,0.5f);
 addFloat("srLowFreq","Stereo reverb Low crossover (Hz)",80.0f,1000.0f,250.0f);
 addFloat("srHighFreq","Stereo reverb High crossover (Hz)",1500.0f,16000.0f,4000.0f);
 addFloat("srModRate","Stereo reverb Modulation rate (Hz)",0.05f,4.0f,0.35f);
 addFloat("srModDepth","Stereo reverb Modulation depth (ms)",0.0f,2.0f,0.3f);
 addFloat("srWidth","Stereo reverb Width",0.0f,1.5f,1.0f);
 addFloat("srDucking","Stereo reverb Ducking",0.0f,1.0f,0.0f);
 addFloat("srRelease","Stereo reverb Duck release (ms)",20.0f,2000.0f,300.0f);
 addFloat("srWetLevel","Stereo reverb Wet level (dB)",-24.0f,12.0f,0.0f);
 addFloat("srInputHP","Stereo reverb Input high-pass (Hz)",20.0f,1000.0f,40.0f);
 addFloat("srInputLP","Stereo reverb Input low-pass (Hz)",1000.0f,20000.0f,16000.0f);
 addBool("stereoReverbOn","Post-chain stereo reverb",false);
 addBool("srPreGlide","Reverb pre-delay pitch glide",true);
 addFloat("sdDiffusion","Delay loop diffusion",0.0f,1.0f,0.0f);
 addFloat("mh1Time","mh1Time",1.0f,4000.0f,100.0f);
 addFloat("mh1Level","mh1Level",0.0f,1.0f,0.5f);
 addFloat("mh1Pan","mh1Pan",-1.0f,1.0f,0.0f);
 addFloat("mh1Feedback","mh1Feedback",0.0f,0.95f,0.15f);
 addFloat("mh2Time","mh2Time",1.0f,4000.0f,200.0f);
 addFloat("mh2Level","mh2Level",0.0f,1.0f,0.5f);
 addFloat("mh2Pan","mh2Pan",-1.0f,1.0f,0.0f);
 addFloat("mh2Feedback","mh2Feedback",0.0f,0.95f,0.15f);
 addFloat("mh3Time","mh3Time",1.0f,4000.0f,300.0f);
 addFloat("mh3Level","mh3Level",0.0f,1.0f,0.5f);
 addFloat("mh3Pan","mh3Pan",-1.0f,1.0f,0.0f);
 addFloat("mh3Feedback","mh3Feedback",0.0f,0.95f,0.15f);
 addFloat("mh4Time","mh4Time",1.0f,4000.0f,400.0f);
 addFloat("mh4Level","mh4Level",0.0f,1.0f,0.5f);
 addFloat("mh4Pan","mh4Pan",-1.0f,1.0f,0.0f);
 addFloat("mh4Feedback","mh4Feedback",0.0f,0.95f,0.15f);
 addFloat("mhMix","mhMix",0.0f,1.0f,0.3f);
 addBool("mhOn","Four-head echo",false);
 return p;
}

bool VirtualCortexProcessor::isBusesLayoutSupported(const BusesLayout& layout) const {
 const auto in=layout.getMainInputChannelSet(),out=layout.getMainOutputChannelSet();
 return (in==juce::AudioChannelSet::mono() || in==juce::AudioChannelSet::stereo()) &&
        (out==juce::AudioChannelSet::mono() || out==juce::AudioChannelSet::stereo()) &&
        (in==out || (in==juce::AudioChannelSet::mono() && out==juce::AudioChannelSet::stereo()));
}

void VirtualCortexProcessor::prepareToPlay(double sampleRate,int samplesPerBlock){
 fs=std::isfinite(sampleRate)?juce::jmax(8000.0,sampleRate):48000.0;
 assetSampleRate.store(fs,std::memory_order_release);
 // Routing changes are published through atomic row snapshots.
 updateGridRouting();
 gridAudio.prepare(fs);
 multiHead.prepare(fs);mhMix=0;mhDrain=0;
 stereoDelay.prepare(fs);
 stereoReverb.prepare(fs);
 cabinetConvolution.prepare({fs, (juce::uint32)juce::jmax(1, samplesPerBlock), (juce::uint32)juce::jmax(1,getTotalNumOutputChannels())});
 channels.clear();
 channels.resize((size_t)juce::jmax(1,getTotalNumOutputChannels()));
 for(auto& s:channels){
  s.lowEQ.coefficients=juce::dsp::IIR::Coefficients<float>::makeLowShelf(fs,140.f,0.707f,1.f);
  s.midEQ.coefficients=juce::dsp::IIR::Coefficients<float>::makePeakFilter(fs,850.f,0.85f,1.f);
  s.highEQ.coefficients=juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs,3500.f,0.707f,1.f);
  s.lowEQ.reset();s.midEQ.reset();s.highEQ.reset();
  s.inputFilter.prepare(fs);
  s.outputSmoother.prepare(fs, 12.f);
  s.outputSmoother.reset(cached.outputTrim->load(std::memory_order_relaxed));
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
 if (b.getNumChannels() == 0 || b.getNumSamples() == 0 || channels.empty()) return;
 // Process only the supported main bus channels; clear unexpected extra channels.
 for (int ch = getTotalNumInputChannels(); ch < b.getNumChannels(); ++ch)
  b.clear(ch, 0, b.getNumSamples());
 // A mono guitar input feeds both sides of a stereo effects chain.
 if(getTotalNumInputChannels()==1 && getTotalNumOutputChannels()==2 && b.getNumChannels()>=2)
  b.copyFrom(1,0,b,0,0,b.getNumSamples());
 // Stop non-finite device/host samples before they enter stateful filters.
 for(int ch=0;ch<b.getNumChannels();++ch){auto* samples=b.getWritePointer(ch);
  for(int i=0;i<b.getNumSamples();++i)if(!std::isfinite(samples[i]))samples[i]=0.f;
 }
 // Apply staged changes even while bypassed or in the serial engine.
 // Otherwise model/IR updates can remain indefinitely pending until the user
 // re-enables grid processing, and the first unbypassed block may be stale.
 updateGridRouting();
 commitPendingGridIR();
 commitPendingNeuralModels();
 if(cached.bypass->load(std::memory_order_relaxed)>0.5f)return; // Dry passthrough.
 if (cached.gridMode->load(std::memory_order_relaxed)>0.5f) {
  // No heap allocation: snapshot controls before processing this audio block.
  for(std::size_t row=0;row<4;++row) for(std::size_t col=0;col<8;++col)
   gridAudio.setSlotParameters(row,col,slotControls[row][col].load());
 }
 const float outputTrim=cached.outputTrim->load(std::memory_order_relaxed);
 const bool useStereo=mhDrain>0||mhEnabled->load()>.5f||mhMix>1.e-7f||stereoFlags[0]->load(std::memory_order_relaxed)>.5f||stereoDelay.active()||reverbEnabled->load(std::memory_order_relaxed)>.5f||stereoReverb.active();
 if (cached.gridMode->load(std::memory_order_relaxed)>0.5f && gridAudioReady) {
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
     b.setSample(ch,offset+i,juce::jlimit(-1.f,1.f,interleaved[static_cast<size_t>(i*count+ch)]*(useStereo?1.f:channels[(size_t)ch].outputSmoother.process(outputTrim))));
  }
  if(useStereo)processStereoDelay(b);
  return;
 }
 const float gateThreshold=juce::Decibels::decibelsToGain(cached.gate->load(std::memory_order_relaxed));
 const float drive=cached.drive->load(std::memory_order_relaxed),tone=cached.tone->load(std::memory_order_relaxed),level=cached.level->load(std::memory_order_relaxed),cabMix=cached.cab->load(std::memory_order_relaxed);
 const float bias=cached.ampBias->load(std::memory_order_relaxed),tremDepth=cached.tremDepth->load(std::memory_order_relaxed),tremRate=cached.tremRate->load(std::memory_order_relaxed);
 const float feedback=cached.delayFeedback->load(std::memory_order_relaxed),delayMix=cached.delayMix->load(std::memory_order_relaxed),reverbMix=cached.reverbMix->load(std::memory_order_relaxed);
 const bool gateOn=cached.gateOn->load(std::memory_order_relaxed)>0.5f,ampOn=cached.ampOn->load(std::memory_order_relaxed)>0.5f,cabOn=cached.cabOn->load(std::memory_order_relaxed)>0.5f;
 const bool eqOn=cached.eqOn->load(std::memory_order_relaxed)>0.5f,irOn=cached.irOn->load(std::memory_order_relaxed)>0.5f;
 const bool delayOn=cached.delayOn->load(std::memory_order_relaxed)>0.5f,reverbOn=cached.reverbOn->load(std::memory_order_relaxed)>0.5f,tremOn=cached.tremOn->load(std::memory_order_relaxed)>0.5f;
 const bool useIR = cabOn && irOn && cabinetIRLoaded.load(std::memory_order_acquire) && cabinetConvolution.getCurrentIRSize() > 0;
 const float gain=1.f+drive*38.f;
 const float toneCoeff=std::exp(-2.f*juce::MathConstants<float>::pi*(400.f+tone*10500.f)/(float)fs);
 const float cabLowCoeff=std::exp(-2.f*juce::MathConstants<float>::pi*cached.cabCut->load(std::memory_order_relaxed)/(float)fs);
 const float cabHighCoeff=std::exp(-2.f*juce::MathConstants<float>::pi*95.f/(float)fs);
 const float inputCutoff=cached.inputHP->load(std::memory_order_relaxed);
 const float gateAttack=std::exp(-1.f/(0.002f*(float)fs));
 const float gateRelease=std::exp(-1.f/(0.075f*(float)fs));
 const size_t delaySamples=(size_t)juce::jlimit(1,(int)channels.front().delay.size()-1,(int)(fs*cached.delayTime->load(std::memory_order_relaxed)*0.001));
 const float eqLow=cached.eqLow->load(std::memory_order_relaxed),eqMid=cached.eqMid->load(std::memory_order_relaxed),eqHigh=cached.eqHigh->load(std::memory_order_relaxed);
 // PRE-IR: gate -> amp -> cabinet filter (if no IR). EQ is applied post-IR.
 for(int ch=0;ch<b.getNumChannels();++ch){
  if((size_t)ch>=channels.size()){b.clear(ch,0,b.getNumSamples());continue;}
  auto& s=channels[(size_t)ch];auto* data=b.getWritePointer(ch);
  if (s.lastInputCutoff != inputCutoff) { s.inputFilter.setCutoff(inputCutoff); s.lastInputCutoff = inputCutoff; }
  if (s.lastEqLow != eqLow) { *s.lowEQ.coefficients=juce::dsp::IIR::ArrayCoefficients<float>::makeLowShelf(fs,140.f,0.707f,juce::Decibels::decibelsToGain(eqLow)); s.lastEqLow=eqLow; }
  if (s.lastEqMid != eqMid) { *s.midEQ.coefficients=juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(fs,850.f,0.85f,juce::Decibels::decibelsToGain(eqMid)); s.lastEqMid=eqMid; }
  if (s.lastEqHigh != eqHigh) { *s.highEQ.coefficients=juce::dsp::IIR::ArrayCoefficients<float>::makeHighShelf(fs,3500.f,0.707f,juce::Decibels::decibelsToGain(eqHigh)); s.lastEqHigh=eqHigh; }
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
   data[i]=std::isfinite(x)?juce::jlimit(-1.f,1.f,x*(useStereo?1.f:s.outputSmoother.process(outputTrim))):0.f;
  }
 }
 if(useStereo)processStereoDelay(b);
}
double VirtualCortexProcessor::getTailLengthSeconds() const {
 if(mhEnabled&&mhEnabled->load()>.5f)return 900.;
 const double reverbTail=reverbEnabled&&reverbEnabled->load()>.5f?juce::jlimit(3.,90.,(double)reverbParameters[0]->load()*juce::jmax(1.f,reverbParameters[6]->load())+1.):3.;
 if(!stereoFlags[0]||stereoFlags[0]->load()<=.5f)return reverbTail;
 double ms=juce::jmax(stereoParameters[0]->load(),stereoParameters[1]->load());
 if(stereoFlags[3]->load()>.5f)ms=60000./juce::jlimit(20.,300.,stereoTempo.load())*juce::jmax(stereoParameters[15]->load(),stereoParameters[16]->load());
 if(!std::isfinite(ms)||ms<0)ms=480.;
 if(stereoParameters[18]->load()>0)ms+=1000.; // conservative allowance for feedback allpass tails
 const double rawFeedback=stereoParameters[2]->load();
 const double feedback=std::isfinite(rawFeedback)?juce::jlimit(0.,.95,rawFeedback):0.;
 return juce::jlimit(3.,3600.,reverbTail+ms*.001*(feedback>1.e-6?1.+std::log(.001)/std::log(feedback):1.));
}
void VirtualCortexProcessor::processStereoDelay(juce::AudioBuffer<float>& b){
 auto value=[this](std::size_t i){return stereoParameters[i]->load(std::memory_order_relaxed);};
 vc::StereoDelayParameters p;p.timeL=value(0);p.timeR=value(1);p.feedback=value(2);p.cross=value(3);p.lowCut=value(4);p.highCut=value(5);p.mix=value(6);p.modRate=value(7);p.modDepth=value(8);p.ducking=value(9);p.duckRelease=value(10);p.width=value(11);p.wetLevel=value(12);p.glide=value(13);p.filterMix=value(17);p.diffusion=value(18);
 p.enabled=stereoFlags[0]->load(std::memory_order_relaxed)>.5f;p.pingPong=stereoFlags[1]->load(std::memory_order_relaxed)>.5f;p.tape=stereoFlags[2]->load(std::memory_order_relaxed)>.5f;
 if(stereoFlags[3]->load(std::memory_order_relaxed)>.5f){
  double tempo=value(14);if(auto* playhead=getPlayHead())if(auto position=playhead->getPosition())if(auto bpm=position->getBpm())if(std::isfinite(*bpm)&&*bpm>0)tempo=*bpm;
  tempo=std::isfinite(tempo)?juce::jlimit(20.,300.,tempo):120.;stereoTempo.store(tempo,std::memory_order_relaxed);p.timeL=(float)(60000./tempo*value(15));p.timeR=(float)(60000./tempo*value(16));
 }
 auto rv=[this](std::size_t i){return reverbParameters[i]->load(std::memory_order_relaxed);};
 vc::StereoReverbParameters rp;rp.decay=rv(0);rp.size=rv(1);rp.preDelay=rv(2);rp.diffusion=rv(3);rp.early=rv(4);rp.mix=rv(5);rp.lowDecay=rv(6);rp.highDecay=rv(7);rp.lowFreq=rv(8);rp.highFreq=rv(9);rp.modRate=rv(10);rp.modDepth=rv(11);rp.width=rv(12);rp.ducking=rv(13);rp.release=rv(14);rp.wetLevel=rv(15);rp.inputHP=rv(16);rp.inputLP=rv(17);rp.enabled=reverbEnabled->load(std::memory_order_relaxed)>.5f;rp.preGlide=reverbPreGlide->load(std::memory_order_relaxed)>.5f;stereoReverb.configure(rp);
 std::array<vc::MultiHeadEcho::Head,4> heads;for(std::size_t i=0;i<4;++i)heads[i]={mhParameters[i*4]->load(),mhParameters[i*4+1]->load(),mhParameters[i*4+2]->load(),mhParameters[i*4+3]->load()};const bool mhOn=mhEnabled->load()>.5f;if(!mhOn)for(auto& h:heads)h.feedback=0;multiHead.configure(heads);const float mixTarget=mhOn?mhParameters[16]->load():0.f;const float mixCoefficient=(float)std::exp(-1./(fs*.01));
 stereoDelay.configure(p);const int count=juce::jmin(2,b.getNumChannels());const float trim=cached.outputTrim->load(std::memory_order_relaxed);
 for(int i=0;i<b.getNumSamples();++i){const float l=b.getSample(0,i),r=count>1?b.getSample(1,i):l;const auto delayed=stereoDelay.process(l,r);auto frame=stereoReverb.process(delayed[0],delayed[1]);if(mhOn)mhDrain=(std::size_t)(fs*4.1);else if(mhDrain)--mhDrain;const auto echo=multiHead.process(mhOn?(frame[0]+frame[1])*.5f:0);mhMix=mixTarget+mixCoefficient*(mhMix-mixTarget);if(std::abs(mhMix-mixTarget)<1.e-7f)mhMix=mixTarget;for(int ch=0;ch<2;++ch)frame[ch]=frame[ch]*(1-mhMix)+echo[ch]*mhMix;
  for(int ch=0;ch<count;++ch){const float x=count==1?(frame[0]+frame[1])*.5f:frame[(std::size_t)ch];b.setSample(ch,i,juce::jlimit(-1.f,1.f,x*channels[(std::size_t)ch].outputSmoother.process(trim)));}
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
 if(!slot || slot->type!=vc::BlockType::Amp) return false;
 vc::NeuralAmpModel model;
 try { model=vc::loadNeuralAmpModel(file.getFullPathName().toStdString()); }
 catch(const std::exception&) { return false; }
 std::lock_guard<std::mutex> guard(assetMutex);
 if(!pendingNeuralModels.submit(row,col,&model)) return false;
 gridNeuralPaths[row][col]=file.getFullPathName();
 return true;
}
juce::String VirtualCortexProcessor::getGridNeuralModelName(std::size_t row,std::size_t col) const {
 if(!vc::SignalGrid::valid(row,col)) return "Invalid slot";
 std::lock_guard<std::mutex> guard(assetMutex);
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
                        static_cast<unsigned>(assetSampleRate.load(std::memory_order_acquire)), vc::PortableAudioEngine::maxIrTaps);
 } catch (const std::exception&) { return false; }
 if (decoded.taps.empty()) return false;
 std::lock_guard<std::mutex> guard(assetMutex);
 if (!pendingGridIRs.submit(row,col,decoded.taps.data(),decoded.taps.size())) return false;
 gridIRPaths[row][col]=file.getFullPathName();
 return true;
}
juce::String VirtualCortexProcessor::getGridCabinetIRName(std::size_t row,
                                                           std::size_t col) const {
 if (!vc::SignalGrid::valid(row,col)) return "Invalid slot";
 std::lock_guard<std::mutex> guard(assetMutex);
 const auto& path=gridIRPaths[row][col];
 return path.isEmpty() ? "No grid IR" : juce::File(path).getFileName();
}
bool VirtualCortexProcessor::loadCabinetIR(const juce::File& file){
 if(!file.existsAsFile() || !file.hasFileExtension("wav;aiff;aif")) return false;
 juce::AudioFormatManager formats;formats.registerBasicFormats();
 std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
 if(!reader || reader->lengthInSamples<=0)return false;
 std::lock_guard<std::mutex> guard(assetMutex);
 cabinetConvolution.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
                                      juce::dsp::Convolution::Trim::yes, 0,
                                      juce::dsp::Convolution::Normalise::yes);
 cabinetIRName=file.getFileName();
 cabinetIRPath=file.getFullPathName();
 cabinetIRLoaded.store(true,std::memory_order_release);
 return true;
}
juce::String VirtualCortexProcessor::getCabinetIRName() const {
 std::lock_guard<std::mutex> guard(assetMutex);return cabinetIRName;
}
void VirtualCortexProcessor::getStateInformation(juce::MemoryBlock& dest){
 auto state=parameters.copyState();
 std::lock_guard<std::mutex> guard(assetMutex);
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
  state.setProperty(key+"repeatFx",p.repeatFx,nullptr);
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
   p.repeatFx=static_cast<bool>(restored.getProperty(key+"repeatFx",false));
   setGridSlotParameters(row,col,p);
   const auto neuralPath=restored.getProperty(key+"neuralPath").toString();
   if(neuralPath.isEmpty() || !loadGridNeuralModel(juce::File(neuralPath),row,col)){
    std::lock_guard<std::mutex> guard(assetMutex);
    pendingNeuralModels.submit(row,col,nullptr);gridNeuralPaths[row][col].clear();
   }
   const auto gridPath=restored.getProperty(key+"irPath").toString();
   if(gridPath.isEmpty() || !loadGridCabinetIR(juce::File(gridPath),row,col)){
    std::lock_guard<std::mutex> guard(assetMutex);
    pendingGridIRs.submit(row,col,nullptr,0);gridIRPaths[row][col].clear();
   }
  }
  const auto current=parameters.copyState();
  auto addDefault=[&](const char* id){if(restored.getChildWithProperty("id",id).isValid())return;auto node=current.getChildWithProperty("id",id).createCopy();if(node.isValid())if(auto* parameter=parameters.getParameter(id)){node.setProperty("value",parameter->convertFrom0to1(parameter->getDefaultValue()),nullptr);restored.addChild(node,-1,nullptr);}};
  const char* mhIDs[]={"mh1Time","mh1Level","mh1Pan","mh1Feedback","mh2Time","mh2Level","mh2Pan","mh2Feedback","mh3Time","mh3Level","mh3Pan","mh3Feedback","mh4Time","mh4Level","mh4Pan","mh4Feedback","mhMix","mhOn"};for(auto id:mhIDs)addDefault(id);
  for(const auto* id:vc::stereoDelayIDs)addDefault(id);for(const auto* id:vc::stereoDelayFlagIDs)addDefault(id);
  for(const auto* id:vc::stereoReverbIDs)addDefault(id);addDefault("stereoReverbOn");addDefault("srPreGlide");
  parameters.replaceState(restored);
  if(path.isEmpty() || !loadCabinetIR(juce::File(path))){
   std::lock_guard<std::mutex> guard(assetMutex);
   cabinetIRPath.clear();cabinetIRName="No IR loaded";
   cabinetIRLoaded.store(false,std::memory_order_release);
  }
 }
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new VirtualCortexProcessor();}
