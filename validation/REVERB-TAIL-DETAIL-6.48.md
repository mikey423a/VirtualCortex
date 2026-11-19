# Virtual Cortex 6.48 candidate

Stereo Reverb in the post chain adds optional Tail Detail. The late wet projection uses cubic interpolation while tank feedback keeps the existing linear read. Early reflections and pre-delay use their existing paths. The output tap blends over 5 ms when toggled. The new host parameter is appended and defaults off in older presets.

Local GCC reverb regressions passed at 44.1, 48 and 96 kHz. Live toggles matched the blend of two continuously rendered reference engines and returned exactly to the original output after the fade, including while the tail decayed. These tests check projection and state continuity. Listening and spectral characterization remain open. Windows verification pending.
