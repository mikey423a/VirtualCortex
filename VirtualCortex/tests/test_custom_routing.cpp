#include "DSP/ParallelGrid.h"
#include <iostream>
#include <stdexcept>
#include <limits>
static void check(bool b,const char* m){if(!b)throw std::runtime_error(m);}
static bool near(float a,float b){return std::abs(a-b)<1e-5f;}
int main(){try{
 vc::SignalGrid grid;for(int r=0;r<4;++r)grid.set(r,7-r,{vc::BlockType::Gain,true});
 vc::ParallelGrid engine;check(engine.configure(grid,{true,true,true,true}),"configure");vc::RowRouting config;
 auto pass=[](std::size_t,std::size_t,vc::BlockType,std::array<float,2>x){return x;};
 engine.setRowRouting(config);engine.prepareMixer(48000);
 auto run=[&](std::array<float,2>x){return engine.processStereoFrame(x,vc::ParallelGrid::Mode::Custom,pass);};
 check(run({.2f,.4f})==std::array<float,2>{.2f,.4f},"Parallel average changes dry unity");
 config.rows[0].output=config.rows[1].output=config.rows[2].output=false;config.rows[1].sources=config.rows[2].sources=2;config.rows[3].sources=12;engine.setRowRouting(config);
 int calls=0;auto y=engine.processStereoFrame({.1f,.2f},vc::ParallelGrid::Mode::Custom,[&](std::size_t r,std::size_t c,vc::BlockType,std::array<float,2>x){++calls;check(c==7-r,"Physical slot mismatch");for(auto& v:x)v*=float(r+1);return x;});
 check(calls==4&&near(y[0],1)&&near(y[1],2),"Split merge must process upstream once and average branches");
 config.average=false;engine.setRowRouting(config);check(near(run({.2f,.4f})[0],.4f),"Sum mode merge wrong");
 auto bad=config;bad.rows[0].sources=2;check(!engine.setRowRouting(bad),"Self feedback accepted");bad=config;bad.rows[1].sources=8;check(!engine.setRowRouting(bad),"Backward connection accepted");bad=config;bad.rows[3].gainDb=std::numeric_limits<float>::quiet_NaN();check(!engine.setRowRouting(bad),"NaN gain accepted");
 // Two independent stereo-represented mono chains: no crossfeed or 6 dB loss.
 config={};config.average=false;config.rows[0].channel=1;config.rows[0].output=false;config.rows[1].sources=2;config.rows[1].balance=-1;config.rows[2].channel=2;config.rows[2].output=false;config.rows[3].sources=8;config.rows[3].balance=1;engine.setRowRouting(config);engine.prepareMixer(48000);
 check(run({.2f,.4f})==std::array<float,2>{.2f,.4f},"Independent channels wrong");check(run({.2f,0})[1]==0,"Crossfeed between independent paths");
 // Polarity transitions pass smoothly through zero, never jump sign.
 config.rows[1].invert=true;engine.setRowRouting(config);float previous=.2f;
 for(int i=0;i<12000;++i){auto f=run({.2f,0});check(std::abs(f[0]-previous)<.001f,"Gain zipper/discontinuity");previous=f[0];}check(near(previous,-.2f),"Polarity target not reached");
 config.rows[1].output=config.rows[3].output=false;engine.setRowRouting(config);check(run({.2f,.4f})==std::array<float,2>{0,0},"No output must be silent");
 config={};engine.setRowRouting(config);engine.prepareMixer(48000);engine.configure(grid,{false,false,false,false});check(run({.2f,.4f})==std::array<float,2>{0,0},"Disabled graph should mute");
 std::cout<<"PASS: custom split/merge oracle, channel isolation, unity levels, invalid cycles, smooth polarity and mute\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
