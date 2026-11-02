#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "OfflineGridConfig.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdio>
int main(){
 const char* path="vc_test_grid_config.txt";
 {std::ofstream f(path); f<<"lane 0 on\nslot 0 1 Amp on\nparam 0 1 drive 0.8\nslot 0 2 Cabinet off\n";}
 auto c=vc::loadOfflineGridConfig(path);
 assert(c.lanes[0] && !c.lanes[1]);
 assert(c.grid.get(0,1)->type==vc::BlockType::Amp);
 assert(!c.grid.get(0,2)->enabled);
 assert(c.params[0][1].drive==0.8f);
 {std::ofstream f(path); f<<"lane 4 on\n";}
 bool threw=false;try{(void)vc::loadOfflineGridConfig(path);}catch(const std::exception&){threw=true;}assert(threw);
 {std::ofstream f(path); f<<"param 0 0 drive nan\n";}
 threw=false;try{(void)vc::loadOfflineGridConfig(path);}catch(const std::exception&){threw=true;}assert(threw);
 {std::ofstream f(path); f<<"slot 0 0 Unknown on\n";}
 threw=false;try{(void)vc::loadOfflineGridConfig(path);}catch(const std::exception&){threw=true;}assert(threw);
 std::remove(path);std::cout<<"grid config tests passed\n";
}
