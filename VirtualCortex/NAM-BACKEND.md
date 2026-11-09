# NAM backend and offline renderer

The core is pinned at v0.5.4 commit 1f42f88535884450104b8711d7595019afa0495b. The 6.24 development candidate connects it to live Amp grid slots; see LIVE-NAM-PROGRESS.md for current verification status and limitations. The separate renderer remains available:

```
virtualcortex_nam_render.exe model.nam input.wav output.wav
virtualcortex_nam_render.exe --probe model.nam
```

Input: mono/stereo PCM16, PCM24 or float32 RIFF/WAVE, up to 256 MiB. Each stereo channel has an independent mono NAM instance. Only mono-in/mono-out models are accepted. The input sample rate must match the model when the model declares one; no resampling is provided. Models without a declared rate use the input WAV rate. Output is float32 WAV and is neither normalized nor clipped, so check levels before playback. No commercial amp models, factory captures or IRs are bundled.

The probe renders a small synthetic signal and reports its peak plus total loading/prewarming/rendering time. That total is not a real-time CPU benchmark or a sound-quality score. Use trusted model files; this developer tool is not an untrusted-file sandbox. A malformed model can fail or exhaust resources in the upstream loader.

Tests compare the rendered samples of an original two-tap Linear fixture against an independent FIR oracle, cover stereo isolation and partial chunks, reject mismatched sample rates and malformed JSON, and protect the input file from accidental overwrite. Additional smoke tests load and render the upstream LSTM and WaveNet example files. These smoke tests establish execution, not sonic equivalence or real-time safety. The existing offline WAV-format tests cover the shared reader/writer refactor.



## Upstream and licenses

- https://github.com/sdatkinson/NeuralAmpModelerCore — Steven Atkinson, MIT (license included).
- Eigen, pinned by NAM's submodule at bc3b39870ecb690a623a3f49149a358b95c5781d — MPL 2.0 and applicable BSD notices (included). Source: https://gitlab.com/libeigen/eigen/-/tree/bc3b39870ecb690a623a3f49149a358b95c5781d
- nlohmann/json — MIT, complete notices retained in the bundled json.hpp.

NAM is an independent project and is not Neural DSP's Neural Capture format. Model-file licenses must be checked separately from the engine license.
