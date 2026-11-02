#include "DSP/StereoDelay.h"
#include "DSP/StereoReverb.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
int main(){volatile float sink=0;std::cout<<"rate,buffer,mean_budget_percent,p99_budget_percent,max_budget_percent\n";for(double fs:{48000.,96000.,192000.})for(int block:{32,64,128,256}){vc::StereoDelay d;vc::StereoReverb r;d.prepare(fs);r.prepare(fs);vc::StereoDelayParameters dp;dp.enabled=true;dp.modDepth=2;dp.feedback=.8f;vc::StereoReverbParameters rp;rp.enabled=true;rp.modDepth=1;rp.decay=10;rp.preGlide=false;std::vector<double> times;times.reserve(4000);for(int b=0;b<4400;++b){auto begin=std::chrono::steady_clock::now();d.configure(dp);r.configure(rp);for(int i=0;i<block;++i){auto x=d.process(.1f,.07f);auto y=r.process(x[0],x[1]);sink=y[0];}auto end=std::chrono::steady_clock::now();if(b>=400)times.push_back(std::chrono::duration<double>(end-begin).count()*fs/block*100);}double sum=0;for(double x:times)sum+=x;std::sort(times.begin(),times.end());std::cout<<fs<<","<<block<<","<<sum/times.size()<<","<<times[3960]<<","<<times.back()<<"\n";}return sink==999;}
