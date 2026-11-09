#include "DSP/ParallelGrid.h"
#include <iostream>
#include <stdexcept>
static void check(bool b){if(!b)throw std::runtime_error("Routing mismatch");}
int main(){try{
 vc::SignalGrid grid;for(int r=0;r<4;++r)grid.set(r,r,{vc::BlockType::Gain,true});
 vc::ParallelGrid engine;check(engine.configure(grid,{true,true,true,true}));
 auto gain=[](std::size_t channel,std::size_t row,std::size_t col,vc::BlockType,float x) noexcept {return col==row?x*float(row+1):float(channel+999);};
 auto p=engine.processFrame({1,2},vc::ParallelGrid::Mode::Parallel,gain);check(p[0]==2.5f&&p[1]==5);
 auto s=engine.processFrame({1,2},vc::ParallelGrid::Mode::Series,gain);check(s[0]==24&&s[1]==48);
 auto d=engine.processFrame({1,2},vc::ParallelGrid::Mode::DualMono,gain);check(d[0]==2&&d[1]==24);
 d=engine.processFrame({1,0},vc::ParallelGrid::Mode::DualMono,gain);check(d[0]==2&&d[1]==0);
 check(engine.configure(grid,{false,false,true,true}));d=engine.processFrame({1,2},vc::ParallelGrid::Mode::DualMono,gain);check(d[0]==1&&d[1]==24);
 check(engine.configure(grid,{false,false,false,false}));for(auto mode:{vc::ParallelGrid::Mode::Parallel,vc::ParallelGrid::Mode::Series,vc::ParallelGrid::Mode::DualMono}){auto y=engine.processFrame({1,2},mode,gain);check(y[0]==1&&y[1]==2);}
 std::cout<<"PASS: series, parallel, dual isolation, disabled lanes and physical slot identity\n";
}catch(const std::exception& e){std::cerr<<e.what();return 1;}}
