#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds.
#endif
#include "OfflineGridConfig.h"
#include <filesystem>
#include <fstream>
#include <cassert>
#include <iostream>
int main(){
 namespace fs=std::filesystem;
 auto dir=fs::temp_directory_path()/"vc_neural_config_test";
 fs::create_directories(dir);
 auto config=dir/"grid.txt";
 {std::ofstream out(config);out<<"lane 0 on\nslot 0 1 Amp on\nneural 0 1 model.vcmlp\n";}
 auto parsed=vc::loadOfflineGridConfig(config.string());
 assert(parsed.neuralPaths[0][1]==(dir/"model.vcmlp").string());
 assert(parsed.neuralPaths[0][0].empty());
 {std::ofstream out(config);out<<"slot 0 1 Delay on\nneural 0 1 model.vcmlp\n";}
 bool rejected=false;
 try { (void)vc::loadOfflineGridConfig(config.string()); } catch(const std::runtime_error&) { rejected=true; }
 assert(rejected);
 {std::ofstream out(config);out<<"slot 0 1 Amp off\nneural 0 1 model.vcmlp\n";}
 rejected=false;
 try { (void)vc::loadOfflineGridConfig(config.string()); } catch(const std::runtime_error&) { rejected=true; }
 assert(rejected);
 fs::remove_all(dir);
 std::cout<<"Neural grid config checks passed\n";
}
