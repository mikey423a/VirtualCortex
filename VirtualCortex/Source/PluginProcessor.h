#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include "DSP/GridPlan.h"
#include <vector>
#include <mutex>
#include "DSP/CoreDSP.h"
#include "DSP/PortableAudioEngine.h"
#include "DSP/StereoDelay.h"
#include "DSP/MultiHeadEcho.h"
#include "DSP/CreativeEffects.h"
#include "DSP/TrackingEffects.h"
#include "DSP/SequenceTempo.h"
#include "DSP/StereoReverb.h"
#include "DSP/PendingIRQueue.h"
#include "DSP/PendingNeuralQueue.h"
class VirtualCortexProcessor final : public juce::AudioProcessor {
public:
 VirtualCortexProcessor();
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override {return true;}
 const juce::String getName() const override {return "Virtual Cortex";}
 bool acceptsMidi() const override {return false;}
 bool producesMidi() const override {return true;}
 bool isMidiEffect() const override {return false;}
 double getTailLengthSeconds() const override;
 int getNumPrograms() override {return 1;}
 int getCurrentProgram() override {return 0;}
 void setCurrentProgram(int) override {}
 const juce::String getProgramName(int) override {return {};}
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState parameters;
 vc::SignalGrid getEditableGrid() const noexcept;
 bool setGridBlock(std::size_t row,std::size_t col,vc::BlockSlot slot) noexcept;
 bool setGridLaneEnabled(std::size_t row,bool enabled) noexcept;
 bool isGridLaneEnabled(std::size_t row) const noexcept;
 bool setGridSlotParameters(std::size_t row,std::size_t col,const vc::SlotParameters& params) noexcept;
 vc::SlotParameters getGridSlotParameters(std::size_t row,std::size_t col) const noexcept;
 bool loadCabinetIR(const juce::File& wavFile);
 // Loads and stages a WAV IR for the selected grid slot; audio thread commits it.
 bool loadGridCabinetIR(const juce::File& wavFile, std::size_t row, std::size_t col);
 bool loadGridNeuralModel(const juce::File& modelFile,std::size_t row,std::size_t col);
 juce::String getGridNeuralModelName(std::size_t row,std::size_t col) const;
 juce::String getGridCabinetIRName(std::size_t row, std::size_t col) const;
 juce::String getCabinetIRName() const;
 void loopCommand(unsigned command)noexcept{loopCommands.fetch_or(command);}
 unsigned loopStatus()const noexcept{return loopState.load();}
 // Message/control-thread operations; never call from processBlock.
 bool storeScene(int index);
 bool recallScene(int index);
 bool hasScene(int index) const;
 int selectedScene() const noexcept {return activeScene.load();}
private:
 mutable std::mutex sceneMutex;
 std::array<juce::ValueTree,8> scenes{};
 std::atomic<int> activeScene{-1};
 void processTracking(juce::AudioBuffer<float>&,juce::MidiBuffer&,bool bypass);
 std::array<std::atomic<float>*,6> sequenceTempo{};
 float sequencerStep(float manual,std::size_t offset) const;
 vc::TrackingSynth trackingSynth;vc::SequenceCrusher sequenceCrusher;
 std::array<std::atomic<float>*,25> trackingParameters{};
 std::array<std::atomic<float>*,4> trackingFlags{};
 int emittedNote=-1;bool midiPanic=true;
 vc::IntervalEcho intervalEcho;vc::LayerLoom layerLoom;
 std::array<std::atomic<float>*,10> creativeParameters{};std::array<std::atomic<float>*,3> creativeFlags{};
 std::atomic<unsigned> loopCommands{0},loopState{0};
 static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
 void processStereoDelay(juce::AudioBuffer<float>&);
 std::array<std::atomic<float>*,19> stereoParameters{};
 std::array<std::atomic<float>*,4> stereoFlags{};
 vc::StereoDelay stereoDelay;
 vc::MultiHeadEcho multiHead;
 std::array<std::atomic<float>*,17> mhParameters{};
 std::atomic<float>* mhEnabled=nullptr;float mhMix=0;std::size_t mhDrain=0;
 vc::StereoReverb stereoReverb;
 std::array<std::atomic<float>*,18> reverbParameters{};
 std::atomic<float>* reverbEnabled=nullptr;
 std::atomic<float>* reverbPreGlide=nullptr;
 std::atomic<double> stereoTempo{120.0};
 // Stable APVTS parameter objects: avoid string/table lookups on the audio thread.
 struct CachedParameters {
  std::atomic<float>* gate=nullptr;
  std::atomic<float>* drive=nullptr;
  std::atomic<float>* ampBias=nullptr;
  std::atomic<float>* inputHP=nullptr;
  std::atomic<float>* tone=nullptr;
  std::atomic<float>* level=nullptr;
  std::atomic<float>* cab=nullptr;
  std::atomic<float>* cabCut=nullptr;
  std::atomic<float>* delayTime=nullptr;
  std::atomic<float>* delayFeedback=nullptr;
  std::atomic<float>* delayMix=nullptr;
  std::atomic<float>* reverbMix=nullptr;
  std::atomic<float>* tremRate=nullptr;
  std::atomic<float>* tremDepth=nullptr;
  std::atomic<float>* outputTrim=nullptr;
  std::atomic<float>* eqLow=nullptr;
  std::atomic<float>* eqMid=nullptr;
  std::atomic<float>* eqHigh=nullptr;
  std::atomic<float>* gateOn=nullptr;
  std::atomic<float>* ampOn=nullptr;
  std::atomic<float>* cabOn=nullptr;
  std::atomic<float>* delayOn=nullptr;
  std::atomic<float>* reverbOn=nullptr;
  std::atomic<float>* tremOn=nullptr;
  std::atomic<float>* eqOn=nullptr;
  std::atomic<float>* bypass=nullptr;
  std::atomic<float>* irOn=nullptr;
  std::atomic<float>* gridMode=nullptr;
 } cached;
 struct ChannelState {
  float gateEnvelope=0, cabLP=0, cabHP=0, toneLP=0, tremPhase=0;
  vc::HighPassOnePole inputFilter;
  vc::SmoothedGain outputSmoother;
  float delayLP=0, reverbA=0, reverbB=0;
  float lastInputCutoff=-1.f, lastEqLow=0.f, lastEqMid=0.f, lastEqHigh=0.f;
  juce::dsp::IIR::Filter<float> lowEQ,midEQ,highEQ;
  std::vector<float> delay, combA, combB;
  size_t delayIndex=0, indexA=0, indexB=0;
 };
 std::array<std::atomic<std::uint64_t>,4> gridRows{};
 std::atomic<std::uint8_t> gridLanes{3};
 std::array<std::uint64_t,4> appliedGridRows{};
 std::uint8_t appliedGridLanes=255;
 void updateGridRouting() noexcept;
 // Atomics are written on the UI thread and read only at audio block boundaries.
 // Per-field updates are eventually consistent, never a data race.
 struct AtomicSlotParameters {
  std::atomic<float> drive{0.5f}, mix{0.2f}, time{0.5f}, feedback{0.35f}, rate{0.5f}, depth{0.5f};
  std::atomic<bool> repeatFx{false};
  vc::SlotParameters load() const noexcept { return {drive.load(),mix.load(),time.load(),feedback.load(),rate.load(),depth.load(),repeatFx.load()}; }
  void store(const vc::SlotParameters& p) noexcept {drive.store(p.drive);mix.store(p.mix);time.store(p.time);feedback.store(p.feedback);rate.store(p.rate);depth.store(p.depth);repeatFx.store(p.repeatFx);}
 };
 std::array<std::array<AtomicSlotParameters,8>,4> slotControls{};
 vc::PendingIRQueue<vc::PortableAudioEngine::maxIrTaps> pendingGridIRs;
 vc::PendingNeuralQueue pendingNeuralModels;
 std::array<std::array<juce::String,8>,4> gridNeuralPaths{};
 std::array<std::array<juce::String,8>,4> gridIRPaths{};
 void commitPendingGridIR() noexcept;
 void commitPendingNeuralModels() noexcept;
 vc::PortableAudioEngine gridAudio;
 bool gridAudioReady = false;
 std::vector<ChannelState> channels;
 double fs=48000;
 std::atomic<double> assetSampleRate{48000.0};
 mutable std::mutex assetMutex;
 std::atomic<bool> cabinetIRLoaded{false};
 juce::dsp::Convolution cabinetConvolution;
 juce::String cabinetIRName = "No IR loaded";
 juce::String cabinetIRPath;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VirtualCortexProcessor)
};
