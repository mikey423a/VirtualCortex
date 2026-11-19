# Virtual Cortex 6.47 verification

Detail Read now ramps between the linear and cubic wet readings over 5 ms on a live toggle. Rapid reversals continue from the current blend. The delay, feedback, LFO and steady-state output use the existing paths. New or prepared state starts at the saved setting. The existing per-slot preset field still defaults off in older presets.

Local GCC tests compared real grid renders of Chorus, Flanger and Vibrato sample by sample with fixed-mode reference engines at 44.1, 48 and 96 kHz. The test toggles and reverses mid-fade under sustained stereo input, then waits for both exact endpoints. At 48 kHz the first-sample residual relative to the untouched linear trajectory was .0271551 immediate versus .000113159 ramped for Chorus. This measures the extra switch step above the signal's own motion. Further listening remains user-side. 

[Windows CI 44484750646](https://github.com/mikey423a/VirtualCortex/actions/runs/44484750646) built 3282abb82eb02cbe0dcb0c91ede4ca2f4c2979ae. All 48 regressions, pluginval strictness 5, ten-second startup, install/uninstall, archive digests and four package hashes passed. The native Chorus editor at 822 × 600 showed the existing control and updated version.

```text
Virtual Cortex.exe D94B1CAAE4BE4BB3598FB9FEEEC41DBC49402BFBCB5B0BFCDA430A284E0C355E
VirtualCortex-6.47.0-VST3-x64.zip 3EA18E471DB9C0C4EE55DC9373D6D03DF25AE362C2E8281194AB316DB0E52C60
VirtualCortex-6.47.0-Windows-x64-Setup.exe 251EA3CF37C9DA5C65746F1F3AD8176E5E8A026DD26426AD95DF4FDE9BA00D5A
VirtualCortex-NAM-Backend-Experimental-Windows-x64.zip 7EDA847786D593C9936060B88BED61368646792F46FEE495532B999B9743D321
MOD_TOGGLE rate=44100 block=11 immediate_residual=0.0119226 ramp_residual=5.39422e-05
MOD_TOGGLE rate=44100 block=12 immediate_residual=0.0318488 ramp_residual=0.000144124
MOD_TOGGLE rate=44100 block=16 immediate_residual=0.0157894 ramp_residual=7.14511e-05
MOD_TOGGLE rate=48000 block=11 immediate_residual=0.0271551 ramp_residual=0.000113159
MOD_TOGGLE rate=48000 block=12 immediate_residual=0.00827621 ramp_residual=3.44813e-05
MOD_TOGGLE rate=48000 block=16 immediate_residual=0.0134474 ramp_residual=5.60284e-05
MOD_TOGGLE rate=96000 block=11 immediate_residual=0.00718873 ramp_residual=1.49757e-05
MOD_TOGGLE rate=96000 block=12 immediate_residual=0.0236812 ramp_residual=4.93228e-05
MOD_TOGGLE rate=96000 block=16 immediate_residual=0.00862704 ramp_residual=1.79708e-05
```

Next evaluate compatible original reverb improvements. Anubis/Windows 11 listening remains user-side. Installer unsigned.
