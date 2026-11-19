# Virtual Cortex 6.51.0 verification

Custom Routing adds **Smooth connections (5 ms)**. Each direct input and earlier-row feed ramps when connected or disconnected. A disconnect continues reading its last connected tap during the fade. Average mode tracks fractional source weights, with a minimum divisor of one so the last input fades to zero. Downstream effects continue to process, including when no inputs remain. The setting defaults off in older presets. Existing tap-position smoothing is separate. Whole-row enable switches retain their existing behavior.

The test runs a sustained asymmetric stereo signal through Grid Stereo Delay and merges its output with direct input to Grid Stereo Reverb. The test compares an independent weighted input reference, reverses a fade halfway through, disconnects both sources and restores the direct input. Both average and unity-sum modes passed at 44.1, 48 and 96 kHz. At 48 kHz in Average mode, the largest across-channel first-sample step at the initial disconnect fell from 0.0149405 to 0.00111667 (about 13.4×), and reverb still decayed after the source went silent. An independent routing test covered a simultaneous tap change during disconnect and the fade from silence when reconnecting a sole direct input. The portable effect routing test passed ASan and UBSan with leak detection disabled.

A separate local comparison rendered 48,000 stereo frames through the 6.50 and 6.51 routers with connection smoothing off. It varied lane enables, source masks, tap locations, entry slots, OUT flags and averaging, including with the previously released tap, entry and OUT smoothing enabled. The two raw float buffers matched byte for byte (SHA-256 `a9d898b2535938d8d84c3b7286096ceed8a3e7d34ad0ef594346d4779ee3da4c`).

[Windows CI 44521858503](https://github.com/mikey423a/VirtualCortex/actions/runs/44521858503) built `0268926657985534686f717b17e8e92371ce33eb` with ASIO. All 48 regressions passed, including native control binding and old-preset migration. Pluginval strictness 5, ten-second standalone startup, installer install/uninstall and downloaded artifact and package checks passed. Reviewed native routing panel screenshots at 2560 × 1440 and 822 × 600 and checked the four-row grid and wires at 2560 × 1440.

```text
VirtualCortex-build-logs archive 28bb854df87f63d63567243d441d809bb238e2c07c98e6189485e2d858080c67
VirtualCortex-Windows-x64 archive 192d6104661ed14e31c4a12158c3a849dc331c7e510fadec741d4cb6036ff9c3
Virtual Cortex.exe FE9DA995E4A45841A7089DBBBA85C1ABBF5053D52D67D0A0CA092A632C944E6D
VirtualCortex-6.51.0-VST3-x64.zip 94CF8059ABD361BE1DC097A11E19B2BDBAEB9CD1B0554339B6BC25173B6BB4BD
VirtualCortex-6.51.0-Windows-x64-Setup.exe 451F0C72293A46FABE1C578BB683850BD9139F67BB19DE02131B095C5C9AA986
VirtualCortex-NAM-Backend-Experimental-Windows-x64.zip 2809F94897A82935922F87C537017D67AC7351BA813E9D317DC2DD56EAEBE6F3
GRID_REVERB_DETAIL rate=44100 abs_difference_sum=0.324612 retained_tail_energy=9.74032
GRID_REVERB_DETAIL rate=48000 abs_difference_sum=0.569882 retained_tail_energy=14.9925
GRID_REVERB_DETAIL rate=96000 abs_difference_sum=0.417226 retained_tail_energy=10.0788
SOURCE_CONNECTION rate=44100 average=0 immediate_step=0.0152614 blended_step=0.00175551 retained_tail_energy=8.05473
SOURCE_CONNECTION rate=44100 average=1 immediate_step=0.00838288 blended_step=0.000922106 retained_tail_energy=2.0955
SOURCE_CONNECTION rate=48000 average=0 immediate_step=0.0148953 blended_step=0.00225852 retained_tail_energy=13.6912
SOURCE_CONNECTION rate=48000 average=1 immediate_step=0.0149405 blended_step=0.00111667 retained_tail_energy=3.53849
SOURCE_CONNECTION rate=96000 average=0 immediate_step=0.0149048 blended_step=0.00116248 retained_tail_energy=22.0383
SOURCE_CONNECTION rate=96000 average=1 immediate_step=0.00667025 blended_step=0.000565752 retained_tail_energy=5.53672
POST_CHAIN_MODE rate=48000 immediate_step=0.0732387 smooth_step=0.00214295 immediate_tail_energy=65.4064 smooth_tail_energy=65.1658
```

Next verify combined tap-position and connection edits under rapid changes and measure remaining discontinuities on whole-row enable edits. Evaluate the existing Tail Detail options with level-matched guitar input. The installer is unsigned. Windows 11 interactive, Anubis ASIO and guitar listening remain user-side.
