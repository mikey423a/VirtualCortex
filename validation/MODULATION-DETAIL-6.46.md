# Virtual Cortex 6.46 verification

Chorus, Flanger and Vibrato have an optional per-slot Detail Read. The audible fractional delay uses cubic interpolation when four historical samples are available. The feedback write continues using the original linear read. Integer positions and reads near the buffer limits keep the original path. The setting defaults off for older presets.

Local GCC tests at 44.1, 48 and 96 kHz measured squared sine interpolation error at .03686 of the previous read for a half-sample position and frequency fs/6. The exact feedback buffers matched in the high-feedback test. This is a specific interpolation accuracy result. Listening and broader modulation characterization remain open.

[Windows CI 44470157780](https://github.com/mikey423a/VirtualCortex/actions/runs/44470157780) built 1549df169d47b9aa6818adb17c04727da1cc32c2. All 48 regressions, pluginval strictness 5, ten-second startup, install/uninstall, both archive digests and all four package hashes passed. Native preset binding and migration passed. The Chorus detail control was reviewed at 2560 × 1440 and 822 × 600.

```text
Virtual Cortex.exe 08D55314DA6F3ECB35EF7A501EB391C4886A23750F899952960D95D0AE3F44DD
VirtualCortex-6.46.0-VST3-x64.zip D6E7FA69C48220B133F3D5879F64F7B3B0965377A81CF13A7D922FAF0AAEDDB8
VirtualCortex-6.46.0-Windows-x64-Setup.exe 64285D31D5DE2F92C6EEE210C59B7F56DEF03703F0A59C97C52AA062BD5B3503
VirtualCortex-NAM-Backend-Experimental-Windows-x64.zip 78D518D6634D6301328E0E551CDB4B7E4C2BC933DCACD33DB79E5373FF35F23D
MOD_DETAIL rate=44100 squared_error_ratio=0.0368591
MOD_DETAIL rate=48000 squared_error_ratio=0.0368591
MOD_DETAIL rate=96000 squared_error_ratio=0.0368591
```

Next characterize audible sidebands and level matching under moving-delay settings and listen on Anubis. Installer unsigned.
