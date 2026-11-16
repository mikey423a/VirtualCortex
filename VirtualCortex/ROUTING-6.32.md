# 6.32 grid-off captions — verified

Code 1cc105744a41741042de96da2a850a44f7a5d936 passed Windows CI run 43894463366: actual Windows EXE/VST3 compilation, all 45 regressions, pluginval strictness 5, 10-second standalone startup, installer install/uninstall and deployed EXE/VST3 hash checks. Both downloaded archive hashes and all four packaged binary hashes matched.

Native disabled-grid screenshots at 2560x1440 and 822x600 were inspected: GRID OFF captions are visible, all four rounded-square rows and full-width layout remain intact. Native assertions verify grid-off precedence and restored SKIPPED state, editability and unchanged stored configuration. Existing row-based connection highlighting and dimming remain. DSP and preset formats are unchanged.

Archive SHA256: Windows 5a96fbc0c84ccdefa4f3bf965e408ef6eabf5e785fc5e3b44207517a9d85bb46; diagnostics 14f76541c0c0f4acfe6dfd2b4404f15bbe1e0b272c190c0ae1b63a6d78222350.

Next: individual wire selection/hit testing; actual-effect stateful transition tests. A local draft using real StereoDelay and StereoReverb passed ASan/UBSan at 44.1/48/96 kHz (leak detection disabled). It compares disconnected tails, skipped-state retention and reconnection to a separately clocked reference; Windows integration remains pending.

No topology crossfade, multiple injection points, hardware buses or scene-routing completion is claimed. No proprietary DSP equivalence or full playable catalog claim. No profiling or additional looper work. Installer unsigned; Windows Server 2022 CI does not certify Windows 11 interactive audio, Anubis hardware or listening quality. Refresh heads/active CI before work and use guarded ref updates.
