# 6.30 selected-row routing focus — verified

Selecting any grid slot highlights incoming and outgoing custom routing connections for its physical row with brighter, thicker wires and arrows. Other active routes stay faintly visible. This is row-based focus, not individual wire selection or drag-to-connect. DSP, presets, four rounded-square rows and full-width 2560x1440 layout are unchanged.

Verified code a658a8122aa1bf26b1f664206ee332d1f5a20669, Windows CI run 43854110543: EXE/VST3 compiled, all 45 regression tests passed (including stateful tail routing oracle), pluginval strictness 5 passed, standalone remained running 10 seconds, installer install/uninstall including installed VST3 hash comparison passed. Downloaded archive hashes and all four packaged binary hashes verified. Native row-2 and row-4 focus screenshots inspected at 2560x1440, and row-4 compact at 822x600. Incoming merge and outgoing branch emphasis match selected rows.

Archive hashes: diagnostics 3741c239f71a746a2369a649b951056760e0f69c768dff1027816aa9422c1b66; Windows d32087f912d3fc538d4888c5e8370e5bc85c08afc0cff282c91fd1c4549cc765.

Next: clearer inactive controls; individual connection selection/hit testing; actual-effect transition regression coverage before any topology crossfade; multiple injection points, hardware buses and scene routing remain incomplete. Keep preset semantics for skipped state unless deliberately migrated. No profiling/looper additions or proprietary DSP equivalence claim. Installer unsigned. CI uses Windows Server 2022; Windows 11 interactive/Anubis/listening tests remain user-side. Refresh remote heads and active runs, use guarded updates, and validate before promoting subsequent milestones.
