# 6.36 preset file feedback — verified software milestone

The file chooser now uses a shared bounded loader and displays distinct missing/read/oversize/invalid messages. Rejected state no longer reports success; successful restore refreshes the editor and clears old wire focus. File reads are bounded to 1 MiB plus one sentinel byte. Cancellation does nothing. The processor exposes an explicit boolean from its existing restore path; host setStateInformation delegates to that same path. No DSP algorithm, parameter IDs, preset format or legacy sound changes.

Native tests at 2560x1440 and 822x600 cover missing, empty, malformed and oversized files, unchanged serialized state/recall identity/wire focus on rejection, successful file recall and focus clearing. Screenshots added for rejected/loaded status. Physical OS file-dialog selection and unreadable-file permission simulation remain unverified. Existing accepted-state fallback rules for missing IR/models are unchanged; acceptance does not guarantee external assets loaded.

Version 6.36.0 passed Windows verification; see the exact build record below.

Next: physical wire-menu interaction, selected-wire tap/entry controls, audio topology-transition handling and compatible optional effects-quality improvements. No profiling or additional looper work. Anubis/Windows11 hardware and subjective listening remain user-side.


Initial candidate 318d17ccea14cb4473edf44c3c6f5a6e43b8ed58 compiled EXE/VST3 in Windows CI 44135106081, but native_editor_layout failed the expected status assertion (47/48 tests passed). The empty-file fixture used JUCE File::replaceWithData with zero bytes, which deletes the file. Corrected to File::create with explicit existence/zero-size assertions; all original rejection/state/focus checks retained, with expected/actual status diagnostics. Pluginval/startup/installer were skipped by the failed test gate. The corrected rerun passed; see below.


## Verified build — November 17, 2026

Compiled code: d005a6132d629b6f862b914506b0632eb0a5bcd8. Windows CI: https://github.com/mikey423a/VirtualCortex/actions/runs/44182466428 (job 134390116004).

Actual Windows EXE/VST3 compilation, all 48 regressions, pluginval strictness 5, standalone 10-second startup, installer install/uninstall and package hashes passed. Native preset-rejected and preset-loaded PNGs were inspected at 2560x1440 and 822x600: rejected files retain yellow wire focus, successful recall clears it, and four rounded-square rows/full-width layout are preserved.

Both downloaded GitHub artifact archive SHA-256 hashes and all four packaged manifest hashes were independently checked. Installer SHA-256: 9E9BC8457C0973F5158E6ABD0FD3CE47EBBFE34440D3973E1BBEA85FD0944B53. VST3 ZIP SHA-256: 8BFCE1CB384EBC0A247D04679383B334843D090B6ED95DDA0F60033850116ADE.

Next routing UI work: sourceTaps[from] is per incoming connection, while entryColumn is shared by every source feeding a destination row. Any wire menu that edits entry must label its whole-row impact. Preserve expected routing/recall guards and unrelated source bits. Audio topology transitions still use existing behavior; do not claim click-free/spillover handling. Physical OS menu/file-dialog interaction, unreadable-file simulation and optional effects improvements remain unfinished.

Installer unsigned. Anubis/Windows 11 hardware and subjective listening remain user-side. No DSP-equivalence, studio-quality or completed-app claim.
