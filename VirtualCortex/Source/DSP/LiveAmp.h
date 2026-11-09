#pragma once
#include <cstddef>
#include <memory>
#include <string>
namespace vc {
enum class LiveAmpStatus:int {None,Ready,RateMismatch,Fault,Missing};
// No JUCE or NAM dependency in the portable graph. prepare/destruction and
// ownership changes must run off the audio thread, with processing excluded.
class LiveAmp {
public:
 virtual ~LiveAmp()=default;
 virtual void prepare(double rate)noexcept=0;
 virtual void setGains(float inputDb,float outputDb)noexcept=0;
 virtual float process(std::size_t channel,float input)noexcept=0;
 virtual LiveAmpStatus status()const noexcept=0;
 virtual double expectedRate()const noexcept=0;
};
class UnavailableLiveAmp final:public LiveAmp {
public:
 void prepare(double)noexcept override{} void setGains(float,float)noexcept override{}
 float process(std::size_t,float)noexcept override{return 0;}
 LiveAmpStatus status()const noexcept override{return LiveAmpStatus::Missing;}
 double expectedRate()const noexcept override{return 0;}
};
// Implemented only when the pinned NAM core is built; UTF-8 path on all hosts.
std::unique_ptr<LiveAmp> loadLiveNamAmp(const std::string& path,double rate,bool allowRateMismatch=false);
}
