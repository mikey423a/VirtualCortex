// Offline NAM compatibility and rendering tool. Not called by the audio callback.
#include <NAM/get_dsp.h>
#include "WavFile.h"
#include <array>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>

static std::unique_ptr<nam::DSP> load(const std::filesystem::path& path,double rate){
 if(std::filesystem::file_size(path)>64*1024*1024)throw std::runtime_error("Model exceeds 64 MiB");
 nam::DspLoadOptions options;options.prewarm=false;
 auto model=nam::get_dsp(path,options);
 if(!model||model->NumInputChannels()!=1||model->NumOutputChannels()!=1)
  throw std::runtime_error("Only mono-in/mono-out NAM models supported");
 const double expected=model->GetExpectedSampleRate();
 if(expected>0.&&std::abs(expected-rate)>.5)
  throw std::runtime_error("WAV and model sample rates differ; resampling is not implemented");
 model->Reset(rate,128); // allocates and prewarms outside any real-time callback
 return model;
}
static void render(vcwav::Wav& wav,const std::filesystem::path& path){
 std::array<std::unique_ptr<nam::DSP>,2> models;
 for(int ch=0;ch<wav.channels;++ch)models[ch]=load(path,wav.rate);
 std::array<float,128> input{},output{};
 const auto frames=wav.samples.size()/wav.channels;
 for(std::size_t offset=0;offset<frames;offset+=128){
  const auto n=static_cast<int>(std::min<std::size_t>(128,frames-offset));
  for(int ch=0;ch<wav.channels;++ch){
   for(int i=0;i<n;++i)input[i]=wav.samples[(offset+i)*wav.channels+ch];
   auto* in=input.data();auto* out=output.data();models[ch]->process(&in,&out,n);
   for(int i=0;i<n;++i){
    if(!std::isfinite(output[i]))throw std::runtime_error("Model produced non-finite audio");
    wav.samples[(offset+i)*wav.channels+ch]=output[i];
   }
  }
 }
}
static void writeFloatWav(const std::filesystem::path& path,const vcwav::Wav& wav){
 if(wav.samples.size()>(0xffffffffULL-36)/4)throw std::runtime_error("Output too large");
 std::ofstream f(path,std::ios::binary);if(!f)throw std::runtime_error("Cannot create output");
 const auto bytes=static_cast<std::uint32_t>(wav.samples.size()*4);
 f.write("RIFF",4);vcwav::w32(f,36+bytes);f.write("WAVEfmt ",8);vcwav::w32(f,16);
 vcwav::w16(f,3);vcwav::w16(f,wav.channels);vcwav::w32(f,wav.rate);
 vcwav::w32(f,wav.rate*wav.channels*4);vcwav::w16(f,wav.channels*4);vcwav::w16(f,32);
 f.write("data",4);vcwav::w32(f,bytes);
 for(float sample:wav.samples){std::uint32_t bits;std::memcpy(&bits,&sample,4);vcwav::w32(f,bits);}
 if(!f)throw std::runtime_error("Failed writing output WAV");
}
int main(int argc,char** argv){
 try{
  if(argc==3&&std::string(argv[1])=="--probe"){
   nam::DspLoadOptions options;options.prewarm=false;
   auto model=nam::get_dsp(std::filesystem::u8path(argv[2]),options);
   const auto rate=model->GetExpectedSampleRate()>0?model->GetExpectedSampleRate():48000.;
   vcwav::Wav wav;wav.rate=static_cast<std::uint32_t>(rate);wav.channels=1;wav.samples.resize(4096);
   for(std::size_t i=0;i<wav.samples.size();++i)wav.samples[i]=.01f*std::sin(i*6.283185307179586*220./rate);
   const auto start=std::chrono::steady_clock::now();render(wav,std::filesystem::u8path(argv[2]));
   float peak=0;for(float v:wav.samples)peak=std::max(peak,std::abs(v));
   std::cout<<"PASS: loaded and rendered 4096 frames at "<<rate<<" Hz, peak="<<peak
    <<", total load/prewarm/render ms="<<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()<<"\n";
   return 0;
  }
  if(argc!=4){std::cerr<<"Offline tool (not GUI playback):\n  virtualcortex_nam_render model.nam input.wav output.wav\n  virtualcortex_nam_render --probe model.nam\n";return 2;}
  const auto source=std::filesystem::u8path(argv[2]),destination=std::filesystem::u8path(argv[3]);
  if(std::filesystem::weakly_canonical(source)==std::filesystem::weakly_canonical(destination))
   throw std::runtime_error("Input and output must be different files");
  if(std::filesystem::file_size(source)>256*1024*1024)throw std::runtime_error("WAV exceeds 256 MiB");
  auto wav=vcwav::readWav(source);
  for(float v:wav.samples)if(!std::isfinite(v))throw std::runtime_error("Non-finite input audio");
  render(wav,std::filesystem::u8path(argv[1]));writeFloatWav(destination,wav);
  std::cout<<"Rendered "<<wav.samples.size()/wav.channels<<" frames; float32 output is not normalized or clipped.\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"NAM render failed: "<<e.what()<<"\n";return 1;}
}
