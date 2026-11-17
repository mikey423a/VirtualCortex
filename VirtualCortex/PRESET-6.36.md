# 6.36 preset file feedback — candidate

The file chooser now uses a shared bounded loader and displays distinct missing/read/oversize/invalid messages. Rejected state no longer reports success; successful restore refreshes the editor and clears old wire focus. File reads are bounded to 1 MiB plus one sentinel byte. Cancellation does nothing. The processor exposes an explicit boolean from its existing restore path; host setStateInformation delegates to that same path. No DSP algorithm, parameter IDs, preset format or legacy sound changes.

Native tests at 2560x1440 and 822x600 cover missing, empty, malformed and oversized files, unchanged serialized state/recall identity/wire focus on rejection, successful file recall and focus clearing. Screenshots added for rejected/loaded status. Physical OS file-dialog selection and unreadable-file permission simulation remain unverified. Existing accepted-state fallback rules for missing IR/models are unchanged; acceptance does not guarantee external assets loaded.

Version bumped to 6.36.0. Windows compilation, 48 regressions, pluginval5, startup, installer install/uninstall, hashes and native screenshot review pending. Delivered release remains verified 6.35. Do not promote this candidate before checks pass.

Next: physical wire-menu interaction, selected-wire tap/entry controls, audio topology-transition handling and compatible optional effects-quality improvements. No profiling or additional looper work. Anubis/Windows11 hardware and subjective listening remain user-side.


Initial candidate 318d17ccea14cb4473edf44c3c6f5a6e43b8ed58 compiled EXE/VST3 in Windows CI 44135106081, but native_editor_layout failed the expected status assertion (47/48 tests passed). The empty-file fixture used JUCE File::replaceWithData with zero bytes, which deletes the file. Corrected to File::create with explicit existence/zero-size assertions; all original rejection/state/focus checks retained, with expected/actual status diagnostics. Pluginval/startup/installer were skipped by the failed test gate. New Windows verification pending; no promotion.
