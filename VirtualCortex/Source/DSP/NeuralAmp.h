#pragma once
#include <array>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace vc {
// Original, small temporal MLP inference format, NOT Neural Amp Modeler (.nam).
// Eight input samples (current plus seven past), eight tanh hidden units, linear output.
struct NeuralAmpModel {
 static constexpr int inputs=8, hidden=8;
 std::array<std::array<float,inputs>,hidden> weights{};
 std::array<float,hidden> bias{}, outputWeights{};
 float outputBias=0.f;
 bool valid() const noexcept {
  for (const auto& row:weights) for(float x:row) if(!std::isfinite(x)) return false;
  for(float x:bias) if(!std::isfinite(x)) return false;
  for(float x:outputWeights) if(!std::isfinite(x)) return false;
  return std::isfinite(outputBias);
 }
 float infer(const std::array<float,inputs>& history,std::size_t newest) const noexcept {
  float y=outputBias;
  for(int j=0;j<hidden;++j) {
   float a=bias[j];
   for(int i=0;i<inputs;++i) a+=weights[j][i]*history[(newest+inputs-i)%inputs];
   y+=outputWeights[j]*std::tanh(a);
  }
  return std::isfinite(y)?y:0.f;
 }
};
// Text file: VCMLP1, then 64 input weights, 8 biases, 8 output weights, output bias.
inline NeuralAmpModel loadNeuralAmpModel(const std::string& path) {
 std::ifstream in(std::filesystem::u8path(path)); if(!in) throw std::runtime_error("Cannot open neural model: "+path);
 std::string magic; if(!(in>>magic)||magic!="VCMLP1") throw std::runtime_error("Unsupported neural model format (expected VCMLP1)");
 NeuralAmpModel model;
 for(auto& row:model.weights) for(auto& w:row) if(!(in>>w)) throw std::runtime_error("Incomplete neural weights");
 for(auto& b:model.bias) if(!(in>>b)) throw std::runtime_error("Incomplete neural biases");
 for(auto& w:model.outputWeights) if(!(in>>w)) throw std::runtime_error("Incomplete neural output weights");
 if(!(in>>model.outputBias)||!model.valid()) throw std::runtime_error("Invalid neural model values");
 std::string extra; if(in>>extra) throw std::runtime_error("Unexpected trailing neural model data");
 return model;
}
} // namespace vc
