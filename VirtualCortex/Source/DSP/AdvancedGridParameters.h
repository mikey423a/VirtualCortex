#pragma once
#include <array>
#include <cmath>
#include <algorithm>
namespace vc {
inline constexpr int firstAdvanced=30,lastAdvanced=36,advancedCapacity=24;
inline constexpr bool isAdvanced(int type) noexcept {return type>=firstAdvanced&&type<=lastAdvanced;}
struct AdvancedControl {const char* id;const char* label;float low,high,initial;bool toggle;};
struct AdvancedDefinition {const char* name;int count;std::array<AdvancedControl,advancedCapacity> controls;};
inline constexpr std::array<AdvancedDefinition,7> advancedDefinitions{{
 {"Grid Stereo Delay",22,{{
  {"sdTimeL","LEFT TIME (ms)",1.0f,16000.0f,320.0f,false},
  {"sdTimeR","RIGHT TIME (ms)",1.0f,16000.0f,480.0f,false},
  {"sdFeedback","FEEDBACK",0.0f,0.95f,0.35f,false},
  {"sdCross","CROSS FEEDBACK",0.0f,1.0f,0.0f,false},
  {"sdLowCut","HIGH PASS (Hz)",20.0f,1000.0f,40.0f,false},
  {"sdHighCut","LOW PASS (Hz)",1000.0f,20000.0f,16000.0f,false},
  {"sdMix","MIX",0.0f,1.0f,0.2f,false},
  {"sdModRate","MOD RATE (Hz)",0.05f,8.0f,0.5f,false},
  {"sdModDepth","MOD DEPTH (ms)",0.0f,10.0f,0.0f,false},
  {"sdDucking","DUCKING",0.0f,1.0f,0.0f,false},
  {"sdDuckRelease","DUCK RELEASE (ms)",20.0f,2000.0f,250.0f,false},
  {"sdWidth","WIDTH",0.0f,1.5f,1.0f,false},
  {"sdWetLevel","WET LEVEL (dB)",-24.0f,12.0f,0.0f,false},
  {"sdGlide","GLIDE (ms)",5.0f,500.0f,50.0f,false},
  {"sdBpm","TEMPO (BPM)",20.0f,300.0f,120.0f,false},
  {"sdBeatsL","LEFT BEATS",0.125f,4.0f,1.0f,false},
  {"sdBeatsR","RIGHT BEATS",0.125f,4.0f,1.5f,false},
  {"sdFilterMix","LOOP FILTER MIX",0.0f,1.0f,1.0f,false},
  {"sdDiffusion","LOOP DIFFUSION",0.0f,1.0f,0.0f,false},
  {"sdPingPong","PING PONG",0.f,1.f,0.f,true},
  {"sdTape","PITCH GLIDE",0.f,1.f,0.f,true},
  {"sdSync","TEMPO SYNC",0.f,1.f,0.f,true},
 }}},
 {"Grid Stereo Reverb",19,{{
  {"srDecay","DECAY (s)",0.2f,20.0f,2.5f,false},
  {"srSize","SIZE",0.5f,2.0f,1.0f,false},
  {"srPreDelay","PRE DELAY (ms)",0.0f,500.0f,20.0f,false},
  {"srDiffusion","DIFFUSION",0.0f,1.0f,0.7f,false},
  {"srEarly","EARLY BALANCE",0.0f,1.0f,0.2f,false},
  {"srMix","MIX",0.0f,1.0f,0.2f,false},
  {"srLowDecay","LOW DECAY RATIO",0.25f,2.0f,1.0f,false},
  {"srHighDecay","HIGH DECAY RATIO",0.1f,1.0f,0.5f,false},
  {"srLowFreq","LOW CROSSOVER (Hz)",80.0f,1000.0f,250.0f,false},
  {"srHighFreq","HIGH CROSSOVER (Hz)",1500.0f,16000.0f,4000.0f,false},
  {"srModRate","MOD RATE (Hz)",0.05f,4.0f,0.35f,false},
  {"srModDepth","MOD DEPTH (ms)",0.0f,2.0f,0.3f,false},
  {"srWidth","WIDTH",0.0f,1.5f,1.0f,false},
  {"srDucking","DUCKING",0.0f,1.0f,0.0f,false},
  {"srRelease","DUCK RELEASE (ms)",20.0f,2000.0f,300.0f,false},
  {"srWetLevel","WET LEVEL (dB)",-24.0f,12.0f,0.0f,false},
  {"srInputHP","INPUT HIGH PASS (Hz)",20.0f,1000.0f,40.0f,false},
  {"srInputLP","INPUT LOW PASS (Hz)",1000.0f,20000.0f,16000.0f,false},
  {"srPreGlide","PRE DELAY GLIDE",0.f,1.f,1.f,true},
 }}},
 {"Grid Four-head Echo",17,{{
  {"mh1Time","HEAD 1 TIME (ms)",1.0f,4000.0f,100.0f,false},
  {"mh1Level","HEAD 1 LEVEL",0.0f,1.0f,0.5f,false},
  {"mh1Pan","HEAD 1 PAN",-1.0f,1.0f,0.0f,false},
  {"mh1Feedback","HEAD 1 FEEDBACK",0.0f,0.95f,0.15f,false},
  {"mh2Time","HEAD 2 TIME (ms)",1.0f,4000.0f,200.0f,false},
  {"mh2Level","HEAD 2 LEVEL",0.0f,1.0f,0.5f,false},
  {"mh2Pan","HEAD 2 PAN",-1.0f,1.0f,0.0f,false},
  {"mh2Feedback","HEAD 2 FEEDBACK",0.0f,0.95f,0.15f,false},
  {"mh3Time","HEAD 3 TIME (ms)",1.0f,4000.0f,300.0f,false},
  {"mh3Level","HEAD 3 LEVEL",0.0f,1.0f,0.5f,false},
  {"mh3Pan","HEAD 3 PAN",-1.0f,1.0f,0.0f,false},
  {"mh3Feedback","HEAD 3 FEEDBACK",0.0f,0.95f,0.15f,false},
  {"mh4Time","HEAD 4 TIME (ms)",1.0f,4000.0f,400.0f,false},
  {"mh4Level","HEAD 4 LEVEL",0.0f,1.0f,0.5f,false},
  {"mh4Pan","HEAD 4 PAN",-1.0f,1.0f,0.0f,false},
  {"mh4Feedback","HEAD 4 FEEDBACK",0.0f,0.95f,0.15f,false},
  {"mhMix","MIX",0.0f,1.0f,0.3f,false},
 }}},
 {"Grid Chase Pitch Delay",10,{{
  {"ieTime","TIME (ms)",1.0f,4000.0f,350.0f,false},
  {"ieFeedback","FEEDBACK",0.0f,0.9f,0.4f,false},
  {"ieA","PITCH A (st)",-12.0f,12.0f,7.0f,false},
  {"ieB","PITCH B (st)",-12.0f,12.0f,-5.0f,false},
  {"ieStep","STEP (ms)",100.0f,4000.0f,500.0f,false},
  {"ieCutoff","LOW PASS (Hz)",200.0f,18000.0f,5000.0f,false},
  {"ieMix","MIX",0.0f,1.0f,0.3f,false},
  {"ieBpm","TEMPO (BPM)",20.f,300.f,120.f,false},
  {"ieBeats","STEP BEATS",0.125f,4.f,1.f,false},
  {"ieSync","TEMPO SYNC",0.f,1.f,0.f,true},
 }}},
 {"Grid Layer Loom",4,{{
  {"llLevel","LOOP LEVEL",0.0f,1.0f,0.5f,false},
  {"llRate","PLAYBACK RATE",0.5f,2.0f,1.0f,false},
  {"llCutoff","LOW PASS (Hz)",200.0f,18000.0f,8000.0f,false},
  {"llReverse","REVERSE",0.f,1.f,0.f,true},
 }}},
 {"Grid Prism Synth",11,{{
  {"syMix","MIX",0.0f,1.0f,1.0f,false},
  {"syTranspose","TRANSPOSE (st)",-24.0f,24.0f,0.0f,false},
  {"syDetune","DETUNE (ct)",0.0f,30.0f,7.0f,false},
  {"syCutoff","LOW PASS (Hz)",60.0f,18000.0f,1800.0f,false},
  {"syResonance","RESONANCE",0.5f,8.0f,1.0f,false},
  {"syAttack","ATTACK (ms)",1.0f,500.0f,8.0f,false},
  {"syRelease","RELEASE (ms)",10.0f,2000.0f,100.0f,false},
  {"syGlide","GLIDE (ms)",1.0f,500.0f,25.0f,false},
  {"syWave","WAVE",0.0f,2.0f,1.0f,false},
  {"syGate","GATE (dB)",-70.0f,-15.0f,-45.0f,false},
  {"syMidi","MIDI OUTPUT",0.f,1.f,0.f,true},
 }}},
 {"Grid Ottocrush",19,{{
  {"scBits","BITS",1.0f,24.0f,8.0f,false},
  {"scRate","SAMPLE RATE (Hz)",48.0f,48000.0f,8000.0f,false},
  {"scCutoff","LOW PASS (Hz)",60.0f,18000.0f,6000.0f,false},
  {"scStepMs","STEP (ms)",30.0f,2000.0f,125.0f,false},
  {"scDepth","DEPTH",0.0f,1.0f,1.0f,false},
  {"scMix","MIX",0.0f,1.0f,0.5f,false},
  {"scMode","MODE: RATE/FILTER/BITS",0.0f,2.0f,0.0f,false},
  {"scStutterMs","STUTTER TIME (ms)",5.0f,250.0f,60.0f,false},
  {"scStutter","STUTTER MIX",0.0f,1.0f,0.0f,false},
  {"scStep1","STEP 1",-1.0f,1.0f,0.0f,false},
  {"scStep2","STEP 2",-1.0f,1.0f,0.5f,false},
  {"scStep3","STEP 3",-1.0f,1.0f,-0.5f,false},
  {"scStep4","STEP 4",-1.0f,1.0f,1.0f,false},
  {"scStep5","STEP 5",-1.0f,1.0f,0.0f,false},
  {"scStep6","STEP 6",-1.0f,1.0f,-1.0f,false},
  {"scBpm","TEMPO (BPM)",20.f,300.f,120.f,false},
  {"scBeats","STEP BEATS",0.125f,4.f,0.25f,false},
  {"scSync","TEMPO SYNC",0.f,1.f,0.f,true},
  {"scPitch","PITCH SEQUENCE",0.f,1.f,0.f,true},
 }}},
}};
using AdvancedValues=std::array<float,advancedCapacity>;
inline AdvancedValues advancedDefaults(int type) noexcept {
 AdvancedValues v{};if(isAdvanced(type)){const auto& d=advancedDefinitions[type-firstAdvanced];for(int i=0;i<d.count;++i)v[i]=d.controls[i].initial;}return v;
}
inline bool validAdvanced(int type,const AdvancedValues& v) noexcept {
 if(!isAdvanced(type))return false;const auto& d=advancedDefinitions[type-firstAdvanced];
 for(int i=0;i<d.count;++i)if(!std::isfinite(v[i])||v[i]<d.controls[i].low||v[i]>d.controls[i].high)return false;return true;
}
}
