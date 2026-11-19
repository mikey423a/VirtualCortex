# Moving-delay detail verification

Test-only follow-up to 6.46. Evaluates the original Chorus and Flanger moving delay against a sinusoid evaluated at the same fractional position, so the reference includes the intended pitch sweep. Tests cover 1 kHz and 6.5 kHz at 44.1, 48 and 96 kHz, fully wet with feedback off. Cubic output must reduce squared error and bring RMS closer to the reference. Delay history and modulation phase must match both read modes exactly.

Local GCC passed all 12 cases. At 48 kHz and 6.5 kHz, Chorus RMS error against the reference was -0.523 dB with linear and -0.053 dB with Detail Read. Flanger measured -0.520 and -0.053 dB. This measures level and interpolation error for specific signals and settings. Broader listening and sideband isolation remain open. 

[Windows CI 44476944159](https://github.com/mikey423a/VirtualCortex/actions/runs/44476944159) built a7b551ea028c7254347f3f20fff3c0cfc8b6613c. All 48 regressions, pluginval strictness 5, standalone startup, install/uninstall, both archive digests and four package hash checks passed. This adds coverage to 6.46 with no new package release.

```text
MOD_SWEEP rate=44100 flanger=0 hz=1000 error_ratio=0.000322606 linear_db=-0.0148928 detail_db=-4.63772e-05
MOD_SWEEP rate=44100 flanger=0 hz=6500 error_ratio=0.0324346 linear_db=-0.62864 detail_db=-0.075348
MOD_SWEEP rate=44100 flanger=1 hz=1000 error_ratio=0.000319085 linear_db=-0.0150043 detail_db=-4.62195e-05
MOD_SWEEP rate=44100 flanger=1 hz=6500 error_ratio=0.0323485 linear_db=-0.633833 detail_db=-0.07622
MOD_SWEEP rate=48000 flanger=0 hz=1000 error_ratio=0.000281524 linear_db=-0.0123678 detail_db=-3.27214e-05
MOD_SWEEP rate=48000 flanger=0 hz=6500 error_ratio=0.0254002 linear_db=-0.522612 detail_db=-0.0530605
MOD_SWEEP rate=48000 flanger=1 hz=1000 error_ratio=0.000285427 linear_db=-0.0123248 detail_db=-3.15977e-05
MOD_SWEEP rate=48000 flanger=1 hz=6500 error_ratio=0.0255455 linear_db=-0.520156 detail_db=-0.0526111
MOD_SWEEP rate=96000 flanger=0 hz=1000 error_ratio=6.92909e-05 linear_db=-0.00309046 detail_db=-1.99982e-06
MOD_SWEEP rate=96000 flanger=0 hz=6500 error_ratio=0.00383024 linear_db=-0.130588 detail_db=-0.00347306
MOD_SWEEP rate=96000 flanger=1 hz=1000 error_ratio=6.83564e-05 linear_db=-0.0031183 detail_db=-2.00578e-06
MOD_SWEEP rate=96000 flanger=1 hz=6500 error_ratio=0.00382964 linear_db=-0.131457 detail_db=-0.00350432
```

Next: characterize live toggles and quantify artifacts under modulation before further DSP changes. Listening and Anubis testing remain user-side.
