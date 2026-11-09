#pragma once
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
namespace vc {
// Bridges a stereo sample-at-a-time graph to a block model. Model::process must
// accept two planar input/output arrays and must not allocate. Prepare the model
// on the control thread. This adapter owns no model and adds exactly Block frames
// of latency; callers must compensate parallel/dry paths before integration.
template<std::size_t Block=128> class ModelBlockAdapter {
 static_assert(Block>0);
 std::array<std::array<float,Block>,2> input{},output{};
 std::size_t position=0;
public:
 static constexpr std::size_t latency=Block;
 void reset() noexcept {input={};output={};position=0;}
 template<class Model> std::array<float,2> process(Model& model,float left,float right) {
  std::array<float,2> result{output[0][position],output[1][position]};
  input[0][position]=std::isfinite(left)?left:0.f;
  input[1][position]=std::isfinite(right)?right:0.f;
  if(++position==Block){
   position=0;
   float* in[]{input[0].data(),input[1].data()};
   float* out[]{output[0].data(),output[1].data()};
   // A model exception propagates to the caller's fault policy. Do not silently
   // present a failed model as valid audio.
   model.process(in,out,static_cast<int>(Block));
   for(auto& channel:output)for(auto& value:channel)if(!std::isfinite(value))value=0.f;
  }
  return result;
 }
};
}
