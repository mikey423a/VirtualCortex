# Virtual Cortex 6.45 verification

Smooth routing modes (5 ms) is optional, per preset, and off for older presets. On a switch between Parallel, Series and Custom, the grid emits its last sample and fades the difference between it and the new mode's first sample over 5 ms. The new graph runs once. This correction acts at the grid output, before post-chain effects. Dual Mono switches immediately. This does not run or preserve a separate old graph.

Local GCC regressions passed sustained stereo input through grid delay and reverb with Parallel to Series followed by Custom midway through the fade. The 48 kHz raw first-sample step peaked at 0.0650867, and the smoothed grid output had zero step on either switch frame. Steady samples after the second fade matched the immediate reference exactly, including tails. Tests also passed at 44.1 and 96 kHz. 

[Windows CI 44456190467](https://github.com/mikey423a/VirtualCortex/actions/runs/44456190467) built a98df0eadb25ee62b2b9a25d89b94084c4346d8a. All 48 regressions, pluginval strictness 5, ten-second startup, install/uninstall, archive digests and four package hash checks passed. The native preset compatibility check passed, and the control was reviewed at 2560 × 1440 and 822 × 600.

```text
Virtual Cortex.exe 1B33ECCFE0F0149D61E7669CBB5587787851A3E115CFAF3131676D5ED7A330AC
VirtualCortex-6.45.0-VST3-x64.zip F6A95A6A4689885279AD81516CF2B64A3D35D1B9E6CD8DE2E3433328F0A424F5
VirtualCortex-6.45.0-Windows-x64-Setup.exe AEAC204F0FA036A04EA522727070F14D18141D4D7044C67D8E86AAD56ECFCA62
VirtualCortex-NAM-Backend-Experimental-Windows-x64.zip 49533F1A903416194E8C69F788E31930B27039422E14235B5DFB7E2CCE6AE8FE
MODE_EFFECT rate=44100 immediate_peak_switch_step=0.0791698 smooth_switch_step=0 tail_abs_sum=944.478
MODE_EFFECT rate=48000 immediate_peak_switch_step=0.0650867 smooth_switch_step=0 tail_abs_sum=885.871
MODE_EFFECT rate=96000 immediate_peak_switch_step=0.0341485 smooth_switch_step=0 tail_abs_sum=708.714
```

Next measure the complete post-effects signal on mode changes and evaluate optional effect quality improvements. Anubis/Windows 11 listening remains user-side. Installer unsigned.
