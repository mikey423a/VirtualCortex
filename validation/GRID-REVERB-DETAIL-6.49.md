# Virtual Cortex 6.49.0 verification

**Grid Stereo Reverb → page 3 → Tail Detail** adds the same optional cubic read used in the 6.48 post-chain reverb. Each slot stores its own setting. Only the late wet output blends over 5 ms. The feedback tank retains its linear read. Presets and scenes without the new field default to off, and the post-chain setting remains independent.

[Windows CI run 44503146399](https://github.com/mikey423a/VirtualCortex/actions/runs/44503146399) compiled `5e7f98c14dece1f979ed700ccbe1439ae5b0e7f6` on Windows x64 with ASIO. All 48 regressions passed. Pluginval at strictness 5, ten-second standalone startup, installation and uninstallation passed. Downloaded archive and package SHA-256 values matched. Native screenshots at 2560 × 1440 and 822 × 600 show the toggle in the grid device editor with all four rows visible.

The actual grid device was compared with always-linear and always-cubic renders at 44.1, 48 and 96 kHz. Both live fades matched the references, and turning Tail Detail off restored exact linear output during decay. Slot movement retained sample-identical state in its dedicated fixture. Native checks included binding, isolation between slots and from the post chain, and preset round-trip and default-off migration.

Routing regressions also exercised source-tap, destination-entry and routing-mode switches on sustained stimuli with stateful effects. Values below are specific to those fixtures. Skipped effects retain state for reentry. These measurements do not establish an audible result on arbitrary input.

```text
VirtualCortex-Windows-x64 archive 3db56ea58fcec70ea4ffc696ce0159e41462203b1d08622a04baf7972d1e9eca
VirtualCortex-build-logs archive a105c344a7c32b0c0d09884536253b99413bdbdce4e7e8a414540c20be6324d8
Virtual Cortex.exe FFD5FE1D874E2CFDAC2FBA1CA97B42C8BAEBB923C0FDBB83057DBADB579BAC5C
VirtualCortex-6.49.0-VST3-x64.zip 58FA0DDE5456026F279DA54A1009C502FE4F806D4A0D70A8B6EE1801A6ED12AA
VirtualCortex-6.49.0-Windows-x64-Setup.exe 9BF03831C2E42E59136C0721F0D068214F67615DFD0A168B41B350F24A30DF6E
VirtualCortex-NAM-Backend-Experimental-Windows-x64.zip BB3B92D395CEA17B63CB69E4DE4D36B370933ACF000496A2AE9349ADD4DE37BD
GRID_REVERB_DETAIL rate=44100 abs_difference_sum=0.324612 retained_tail_energy=9.74032
GRID_REVERB_DETAIL rate=48000 abs_difference_sum=0.569882 retained_tail_energy=14.9925
GRID_REVERB_DETAIL rate=96000 abs_difference_sum=0.417226 retained_tail_energy=10.0788
TAP_EFFECT rate=48000 type=30 hard_step=0.258285 blend_step=0.000892088 tail_abs_sum=443.507 resumed_tail=0
TAP_EFFECT rate=48000 type=31 hard_step=0.303781 blend_step=0.00108166 tail_abs_sum=553.328 resumed_tail=0
ENTRY_EFFECT rate=48000 type=30 hard_step=0.263963 blend_step=0.00657018 tail_abs_sum=0 resumed_tail=413.101
ENTRY_EFFECT rate=48000 type=31 hard_step=0.30231 blend_step=0.000389189 tail_abs_sum=0 resumed_tail=536.152
MODE_EFFECT rate=48000 immediate_peak_switch_step=0.0650867 smooth_switch_step=0 tail_abs_sum=885.871
POST_CHAIN_MODE rate=48000 immediate_step=0.0732387 smooth_step=0.00214295 immediate_tail_energy=65.4064 smooth_tail_energy=65.1658
```

## Tail Detail tone characterization

The [offline probe](reverb-modulation/tail_detail_probe.cpp) was built with GCC C++17 `-O2` and the `VirtualCortex/Source` include directory. It fed a 0.1-amplitude sine into the left input, let the wet-only reverb settle for four seconds, and accumulated two seconds of stereo power. Both renders used the same tank settings with no pre-delay or early output. Columns are sample rate, tone frequency, modulation depth in milliseconds, and cubic versus linear output level in dB. These values include all modulation sidebands.

```text
48000 1000 0 0.01636
48000 1000 0.8 0.0123544
48000 4000 0 0.235029
48000 4000 0.8 0.190669
48000 8000 0 0.731829
48000 8000 0.8 0.658751
48000 12000 0 1.27999
48000 12000 0.8 1.23806
96000 1000 0 0.00333914
96000 1000 0.8 0.0030716
96000 4000 0 0.057464
96000 4000 0.8 0.0486629
96000 8000 0 0.245192
96000 8000 0.8 0.189722
96000 12000 0 0.554656
96000 12000 0.8 0.404394
```

At 48 kHz with 0.8 ms modulation, the measured level rose by 0.012 dB at 1 kHz and 0.659 dB at 8 kHz. The measurements characterize this setting's high-frequency output in a fixed fixture. A separate five-pass local kernel microbenchmark on the same machine took 158.8 ms with linear reads and 183.3 ms with cubic output reads for 480,000 frames at 48 kHz. App-wide CPU, aliasing and audibility were outside these checks.

Next evaluate level-matched sustained guitar and decaying tails, then use transition measurements to choose the next routing change. Windows 11, Anubis ASIO and listening remain user-side. The installer is unsigned.
