#include "RoutingWireHitTest.h"
#include <stdexcept>
#include <iostream>
static void check(bool b){if(!b)throw std::runtime_error("Wire hit test failed");}
int main(){try{
 std::array<vc::WirePolyline,2> wires{{{{{0,0},{0,0},{0,20},{30,20},{30,30}}},{{{5,0},{5,0},{5,20},{30,20},{30,30}}}}};
 check(vc::hitRoutingWire({0,0},wires,2,0)==0);
 check(vc::hitRoutingWire({0,15},wires,2,2)==0);
 check(vc::hitRoutingWire({4,15},wires,2,2)==1);
 check(vc::hitRoutingWire({15,20},wires,2,0)==0);
 check(vc::hitRoutingWire({30,30},wires,2,0)==0);
 check(vc::hitRoutingWire({30,33},wires,2,2)==-1);
 check(vc::hitRoutingWire({15,10},wires,2,2)==-1);
 check(vc::hitRoutingWire({0,0},wires,0,2)==-1);
 check(vc::hitRoutingWire({0,0},wires,3,2)==-1);
 check(vc::hitRoutingWire({0,0},wires,2,-1)==-1);
 for(auto& wire:wires)for(auto& p:wire){p.x*=2;p.y*=2;}
 check(vc::hitRoutingWire({8,30},wires,2,4)==1);
 std::cout<<"PASS: wire endpoints, bends, nearest hit, ties, misses and scale\n";
}catch(const std::exception& e){std::cerr<<e.what();return 1;}}
