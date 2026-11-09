#include "DSP/ModelBlockAdapter.h"
#include <iostream>
#include <limits>
#include <stdexcept>
static void check(bool b){if(!b)throw std::runtime_error("Model block adapter mismatch");}
struct Model {
 int calls=0;
 void process(float** in,float** out,int n){++calls;for(int i=0;i<n;++i){out[0][i]=in[0][i]*2;out[1][i]=in[1][i]*-3;}}
};
template<std::size_t N> void test(){
 vc::ModelBlockAdapter<N> a;Model m;
 for(std::size_t i=0;i<1000;++i){auto y=a.process(m,float(i+1),float(i+2));check(y[0]==(i<N?0:2.f*(i-N+1)));check(y[1]==(i<N?0:-3.f*(i-N+2)));}
 check(m.calls==1000/N);a.reset();
 for(std::size_t i=0;i<N;++i){auto y=a.process(m,1.f,2.f);check(y[0]==0&&y[1]==0);}
 auto y=a.process(m,std::numeric_limits<float>::quiet_NaN(),1.f);check(y[0]==2&&y[1]==-6);
 for(std::size_t i=0;i<N;++i)y=a.process(m,0,0);
 check(y[0]==0&&y[1]==-3);
}
int main(){try{test<1>();test<32>();test<128>();test<512>();std::cout<<"PASS: exact latency, stereo isolation, reset and finite input\n";}catch(const std::exception& e){std::cerr<<e.what();return 1;}}
