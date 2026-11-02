// Benchmarks the *experimental VCMLP1* model, not NAM or Quad Cortex DSP.
// Runs repeated deterministic blocks, prints time per sample and realtime headroom.
#include "DSP/NeuralAmp.h"
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
int main(int argc, char** argv) {
 try {
  if(argc < 2 || argc > 4) {
   std::cerr << "Usage: virtualcortex_neural_bench model.vcmlp [seconds=10] [instances=1]\n";
   return 2;
  }
  const auto model = vc::loadNeuralAmpModel(argv[1]);
  const int seconds = argc > 2 ? std::stoi(argv[2]) : 10;
  const int instances = argc > 3 ? std::stoi(argv[3]) : 1;
  if(seconds < 1 || seconds > 120 || instances < 1 || instances > 32)
   throw std::runtime_error("seconds must be 1..120 and instances must be 1..32");
  constexpr int sampleRate = 48000;
  constexpr int blockSize = 256;
  std::array<float, vc::NeuralAmpModel::inputs> history{};
  volatile double sink = 0.0;
  const auto start = std::chrono::steady_clock::now();
  for(int frame = 0; frame < sampleRate * seconds; frame += blockSize) {
   for(int n = 0; n < blockSize && frame+n < sampleRate*seconds; ++n) {
    const auto index = static_cast<std::size_t>(frame+n) % history.size();
    history[index] = 0.2f * std::sin(0.013f * static_cast<float>(frame+n));
    for(int instance = 0; instance < instances; ++instance)
     sink += model.infer(history, index);
   }
  }
  const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
  std::cout << std::fixed << std::setprecision(3)
            << "model=VCMLP1 sample_rate=48000 instances=" << instances
            << " simulated_audio_seconds=" << seconds << " elapsed_seconds=" << elapsed
            << " realtime_factor=" << (seconds/elapsed)
            << " microseconds_per_inference=" << (elapsed*1e6/(seconds*sampleRate*instances))
            << " checksum=" << sink << '\n';
  return 0;
 } catch(const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
