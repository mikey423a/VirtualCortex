# Virtual Cortex 6.47 candidate

Detail Read now ramps between the linear and cubic wet readings over 5 ms on a live toggle. Rapid reversals continue from the current blend. The delay, feedback, LFO and steady-state output use the existing paths. New or prepared state starts at the saved setting. The existing per-slot preset field still defaults off in older presets.

Local GCC tests compared real grid renders of Chorus, Flanger and Vibrato sample by sample with fixed-mode reference engines at 44.1, 48 and 96 kHz. The test toggles and reverses mid-fade under sustained stereo input, then waits for both exact endpoints. At 48 kHz the first-sample residual relative to the untouched linear trajectory was .0271551 immediate versus .000113159 ramped for Chorus. This measures the extra switch step above the signal's own motion. Further listening remains user-side. Windows verification pending.
