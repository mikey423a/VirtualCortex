# 6.39 duplicate model command handling — candidate

Re-publishing the same original VCMLP1 weights now preserves temporal sample history and an in-flight crossfade. Repeated clear commands preserve a running fade to the basic amp instead of jumping to the new target. Value comparison includes all weights, biases and output fields, not model addresses. Genuine replacements retain the existing 128-sample fade. Invalid commands still reject before mutation. No NAM inference, amp profiling, new device, preset schema or default sound change.

Regression reproduces failure on unmodified 6.38; corrected local GCC -O2 test passes exact changed/reference sample equality at 44.1/48/96 kHz across parallel, series, dual mono and custom routing. It covers separate copies, repeated commands mid-fade and settled, temporal weights, changed fields, invalid inputs and restart. Existing constant-output replacement fade oracle retained. No click-free guarantee for arbitrary rapid changes between different models.

Version 6.39.0 candidate. Windows EXE/VST3 compilation, complete regression/pluginval/startup/installer/hash verification pending. Do not promote until verified. GUI unchanged except version footer. Latest delivered app remains 6.38.

Next: optional compatible effects improvements, audio topology transition handling and physical OS menu/file-dialog validation. Existing selected-wire tap/entry controls and presets preserved. Anubis/Windows11 hardware and listening remain user-side; unsigned installer.
