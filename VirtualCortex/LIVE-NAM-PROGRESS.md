# Live NAM — 6.24 development candidate

Live NAM playback is connected to Amp grid slots when built with VC_NAM_SOURCE_DIR. This candidate still requires Windows build and runtime verification; 6.23 remains the verified release until CI passes.

Select an Amp block, load a mono-in/mono-out .nam model, and adjust NAM input/output gain (±24 dB). Each channel has independent model state. Models and trims follow grid moves and preset recall. Remove AMP MODEL returns to the basic amp. VCMLP1 loading remains supported.

The audio device/host rate must match the model's declared rate; no resampling is performed. Missing models and rate mismatches mute the affected block with a visible status. Presets save paths, not model files. NAM files and commercial captures are not bundled. NAM is unrelated to Neural DSP capture formats.

The implementation processes one sample per channel without the experimental block adapter, so it introduces no adapter delay. Model loading/prewarming is synchronous on the calling control thread; it may pause the UI. Model replacement is guarded against concurrent audio access; the callback emits silence when the guard is busy. This is not gapless switching. Host state restoration may itself be called on a host-controlled thread.

Tests cover actual Linear-model output against an independent FIR oracle, channel isolation, moves, gain/preset recall, missing files, sample-rate changes, and native editor layouts. Upstream LSTM/WaveNet benchmarks check one stereo instance against real time on the CI machine, not the user's hardware or a full grid. C++ allocation checks do not intercept all upstream malloc calls. Anubis and listening tests remain user-side.

Looper development is out of scope. Existing loop behavior is retained for preset compatibility.
