#ifdef NDEBUG
#undef NDEBUG
#endif
#include "DSP/MultiHeadEcho.h"
#include <cassert>
#include <limits>
int main(){for(double rate:{44100.,48000.,96000.,192000.}){
 vc::MultiHeadEcho d;d.prepare(rate);std::array<vc::MultiHeadEcho::Head,4> h{};
 h[0]={100,1,-1,0};h[1]={200,1,1,0};d.configure(h);
 for(int i=0;i<(int)(rate*.3);++i){auto y=d.process(i==0?.1f:0);const float l=i==(int)(rate*.1)?.1f:0,r=i==(int)(rate*.2)?.1f:0;assert(std::abs(y[0]-l)<1e-5&&std::abs(y[1]-r)<1e-5);}
 // An inaudible head still returns feedback; a separate head plays that return.
 d.prepare(rate);h={};h[0]={100,0,0,.5f};h[1]={150,1,-1,0};d.configure(h);
 for(int i=0;i<(int)(rate*.3);++i){auto y=d.process(i==0?.1f:0);if(i==(int)(rate*.15))assert(std::abs(y[0]-.1f)<1e-5);if(i==(int)(rate*.25))assert(std::abs(y[0]-.05f)<1e-5);}
 // Four maximum feedback sends normalize to a contracting aggregate.
 d.prepare(rate);for(auto& x:h)x={1,1,0,.95f};d.configure(h);
 float last=0;for(int i=0;i<(int)rate;++i){auto y=d.process(i==0?.1f:0);assert(std::isfinite(y[0])&&std::abs(y[0])<1);last=std::abs(y[0]);}assert(last<1e-6);
 d.prepare(rate);d.configure(h);for(int i=0;i<100;++i){auto y=d.process(0);assert(y[0]==0&&y[1]==0);}
 h[0].feedback=std::numeric_limits<float>::infinity();d.configure(h);assert(std::isfinite(d.process(0)[0]));
}}
