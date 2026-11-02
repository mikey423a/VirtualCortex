# NAM backend verification — November 2, 2026

- Source commit: 4672df3d89e09b0903c0cd9350f99eceea768472.
- Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41522572979.
- MSVC on Windows Server 2022, JUCE 8.0.10, ASIO enabled.
- All 31 CTest checks passed. The three added NAM checks cover an independent FIR numerical oracle, mono/stereo state isolation, partial chunks, sample-rate mismatch, malformed models, input-file preservation, and loading/rendering the pinned upstream LSTM and WaveNet example files.
- Existing native processor/UI tests, including four-row/1440p/fullscreen checks, passed.
- VST3 passed pluginval 1.0.4 at strictness level 5. Standalone startup and installer deployment/hash/uninstall checks passed.
- Experimental ZIP contains the Windows x64 NAM rendering executable, usage notes, NAM MIT license, Eigen MPL/BSD notices and nlohmann single-header source with its notices. No third-party model files are bundled.

Scope: this is an offline command-line engine prototype. No live NAM GUI loading, real-time callback allocation/deadline guarantee, capture equivalence, listening test or Anubis hardware test is established. The GUI app remains version 6.12.0; its 21 Grid block types and separate stereo delay/reverb remain unchanged by this checkpoint.
