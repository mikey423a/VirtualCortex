# 6.39 duplicate model command handling — verified

Re-publishing the same original VCMLP1 weights now preserves temporal sample history and an in-flight crossfade. Repeated clear commands preserve a running fade to the basic amp instead of jumping to the new target. Value comparison includes all weights, biases and output fields, not model addresses. Genuine replacements retain the existing 128-sample fade. Invalid commands still reject before mutation. No NAM inference, amp profiling, new device, preset schema or default sound change.

Regression reproduces failure on unmodified 6.38; corrected local GCC -O2 test passes exact changed/reference sample equality at 44.1/48/96 kHz across parallel, series, dual mono and custom routing. It covers separate copies, repeated commands mid-fade and settled, temporal weights, changed fields, invalid inputs and restart. Existing constant-output replacement fade oracle retained. No click-free guarantee for arbitrary rapid changes between different models.

Version 6.39.0 verified November 18, 2026. Compiled code cc644c1643a2222bb7635a11aceaa757a4dbbff2. Windows CI https://github.com/mikey423a/VirtualCortex/actions/runs/44282299743, job 134725878019: successful EXE/VST3 compilation, all 48 regressions including expanded neural_model_crossfade, pluginval strictness 5, standalone 10-second startup, installer install/uninstall. Downloaded both archives and verified their GitHub SHA256 digests; all four package manifest hashes matched. Separate Steinberg validator is not configured.

Windows archive SHA256: 24e4d6936fab6f6ce22c5aa0b17b73eefc56de17efe61b4d79e0b6554d3d98de.
Logs archive SHA256: c0e22a545ee03c07af0d33b82f5a000d99eea1f3f3a733f21eeacbd0c3274ea9.
Installer SHA256: 9cddec3d7cb908b87d2552a75151d7e10cc7794c6f6ed34df61663d39932d59e.
VST3 ZIP SHA256: a10e8380835125c32bf1998820069d8dbcb4ee832fee8bac11965b2255532b86.

Native entry-H screenshots reviewed at 2560x1440/822x600: four rounded-square rows, full-width layout, routing captions/arrows and inactive dimming preserved. GUI unchanged except version footer. This is not physical OS menu interaction.

Local GCC -O2 and AddressSanitizer/UndefinedBehaviorSanitizer (leak detection disabled) passed. The expanded regression first failed against unmodified 6.38 at exact output equality, then passed with the fix. Controlled 48kHz constant .8 model output, fade-to-basic on silent input, duplicate clear after 31 samples: before .60625; old next sample 0 (step .60625), fixed next sample .6 (step .00624996). This is a targeted numerical discontinuity check, not subjective listening or an arbitrary-model click-free guarantee.

Identical VCMLP1 weights reloaded through model loading/state restoration now retain that model's temporal history rather than resetting it. Genuine model changes and device prepare still use their established transition/reset behavior; GUI preset-recall focus policy is unchanged. The live NAM path is not modified by this fix.

Next: optional compatible effects improvements, audio topology transition handling and physical OS menu/file-dialog validation. Existing selected-wire tap/entry controls and presets preserved. Anubis/Windows11 hardware and listening remain user-side; unsigned installer.
