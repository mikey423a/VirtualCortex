#pragma once
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
namespace vc {
struct WirePoint { float x=0,y=0; };
using WirePolyline=std::array<WirePoint,5>;
// Coordinates and tolerance must share the same space. Degenerate segments
// are intentional at row boundaries and remain selectable as points.
inline float wireDistanceSquared(WirePoint p,const WirePolyline& wire) noexcept {
 float best=std::numeric_limits<float>::infinity();
 for(std::size_t i=1;i<wire.size();++i){
  const auto a=wire[i-1],b=wire[i];const float dx=b.x-a.x,dy=b.y-a.y;
  const float length=dx*dx+dy*dy;
  float t=length>0?((p.x-a.x)*dx+(p.y-a.y)*dy)/length:0;
  t=t<0?0:t>1?1:t;
  const float x=p.x-a.x-t*dx,y=p.y-a.y-t*dy;
  const float d=x*x+y*y;if(d<best)best=d;
 }
 return best;
}
// Nearest wire wins; exact ties retain the first index for stable selection.
template<std::size_t N>
int hitRoutingWire(WirePoint p,const std::array<WirePolyline,N>& wires,
                   std::size_t count,float tolerance) noexcept {
 if(!std::isfinite(p.x)||!std::isfinite(p.y)||!std::isfinite(tolerance)||tolerance<0||count>N)return -1;
 float best=tolerance*tolerance;int selected=-1;
 for(std::size_t i=0;i<count;++i){const auto d=wireDistanceSquared(p,wires[i]);
  if(d<=best&&(selected<0||d<best)){best=d;selected=static_cast<int>(i);}
 }
 return selected;
}
}
