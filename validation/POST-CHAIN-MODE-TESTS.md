# Final-output routing transition regression

The native stereo processor test sends sustained input through an empty grid and active Stereo Delay and Stereo Reverb. It switches from Parallel into a Custom row at -12 dB. The opt-in and immediate renders match before the edit. The largest first switched-sample step across channels drops from 0.0732387 to 0.00214295 at 48 kHz, about 34×. Both renders produce tails after input ends. Their energies are close but differ because the short correction also feeds the post effects.

```text
POST_CHAIN_MODE rate=48000 immediate_step=0.0732387 smooth_step=0.00214295 immediate_tail_energy=65.4064 smooth_tail_energy=65.1658
```

[Windows CI 44462964270](https://github.com/mikey423a/VirtualCortex/actions/runs/44462964270) built 832425e46038bc7012d6899fda4969a9a786183d. All 48 regressions, pluginval strictness 5, standalone ten-second startup and installer install/uninstall passed. Both downloaded archive digests and all four package hashes matched. The build is a test-only follow-up. Version 6.45 remains the delivered app.

Next evaluate a compatible opt-in improvement to original modulation or reverb sound. Musical listening and Anubis hardware checks remain user-side.
