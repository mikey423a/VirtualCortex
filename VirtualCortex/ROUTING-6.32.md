# 6.32 grid-off captions — candidate, verification pending

Grid cells now distinguish GRID OFF from individual BYPASSED and routing-prefix SKIPPED captions. Grid-off takes precedence while disabled; the original skipped state returns when enabled. Native UI checks exercise this transition and capture both 2560x1440 and 822x600. Existing dimming, row connection highlighting, DSP, presets and layout are unchanged.

Windows EXE/VST3 build, regressions, pluginval, startup, installer/hash verification and native screenshot inspection are pending. Do not promote or deliver until verified. Last verified milestone remains 6.31.

Next: individual connection selection/hit testing and actual-effect stateful routing-transition tests. No profiling or looper development. Anubis testing remains user-side.
