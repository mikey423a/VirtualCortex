# Stereo effects performance baseline

Measured against the 6.11 source on a shared Linux x86-64 cloud environment exposing AMD EPYC 9V74 CPUs. Compiled with g++ -O3 -DNDEBUG -std=c++17. This is an offline wall-clock benchmark, not a Windows ASIO deadline certification.

The benchmark runs Stereo Delay followed by Stereo Reverb, both enabled, with modulation and long feedback/decay settings. Each iteration includes configure() calls and one audio block. It warms 400 blocks and records 4000 blocks per configuration. Percentages divide measured wall time by bufferFrames/sampleRate. 100% consumes the entire audio block deadline. Measurements include scheduling interruptions. Constant input is used; this does not assess sonic quality. The full amp/grid engine, JUCE host wrapper, drivers, UI load and hardware I/O are excluded.

```csv
rate,buffer,mean_budget_percent,p99_budget_percent,max_budget_percent
48000,32,2.32521,13.3191,127.606
48000,64,2.24293,9.33885,25.6198
48000,128,2.34221,9.72386,68.2567
48000,256,2.27593,6.46616,138.548
96000,32,4.14105,13.821,105.289
96000,64,4.36041,17.2838,84.7571
96000,128,4.28661,11.7313,100.25
96000,256,4.39397,10.8329,19.1987
192000,32,8.54027,43.1574,182.503
192000,64,8.64691,34.0206,169.703
192000,128,8.57354,23.5783,54.3944
192000,256,8.76504,21.5027,97.7393
```

Mean cost is approximately 2.2–2.4% at 48 kHz, 4.1–4.4% at 96 kHz and 8.5–8.8% at 192 kHz on this machine. Several maxima exceed 100%. Their cause is not isolated; shared-host scheduling may contribute. Do not infer that all spikes are harmless scheduling noise or that these numbers predict the user's PC. No real-time performance or studio-quality claim is established.

Next acceptance work: profile the complete Windows processor under sustained audio and automation, measure deadline overruns at 32/64/128/256 samples with the actual Merging ASIO driver, and compare active/bypassed effects. Include listening tests and musical transients. Preserve the maximum measurements rather than reporting averages alone.

Reproduce from the repository root:

```sh
g++ -O3 -DNDEBUG -std=c++17 -I VirtualCortex/Source validation/stereo_effects_benchmark.cpp -o stereo_effects_benchmark
./stereo_effects_benchmark
```

This validation update does not change application binaries. Version 6.11 remains the latest compiled release.
