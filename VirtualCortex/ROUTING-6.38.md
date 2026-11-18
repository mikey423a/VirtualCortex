# 6.38 destination-entry menu — verified

Right-click a Custom row mixer wire and choose Destination entry — ALL row N inputs, then Enter at A–H. Submenu repeats that it affects ALL inputs to the destination row. Source masks, source taps and row mix settings are preserved. Save preset to persist. Same entry is a no-op; stale routing/recall, invalid indices/ranges, absent connections and non-custom mode are rejected under existing locks.

Native tests cover all eight entries with multiple incoming sources, unrelated settings, rejection cases, no-op revisions, focus clearing and state persistence at 2560x1440/822x600. Entry-H screenshots added. Physical OS popup/menu interaction remains unverified. No DSP algorithms, parameter IDs, preset schema or default preset sound changed. Audio transition/tail behavior is unchanged; no click-free claim.

Version 6.38.0 verified. Compiled code: `5be53d3a6b811c470fab90e2f52541cca00be168`. Windows CI [44241454309](https://github.com/mikey423a/VirtualCortex/actions/runs/44241454309), job 134590088892, completed successfully. Actual EXE/VST3 compilation, all 48 regressions, pluginval strictness 5, standalone 10-second startup, installer install/uninstall passed. The separate Steinberg validator was not configured; pluginval passed.

Downloaded Windows archive SHA256: c15446b8e529cf2c4c028565eadbf771c50db08665c8796fa71092154934d65d.
Downloaded logs archive SHA256: e1fac51552966b6d8eb90d653aa31a2d46556945febb0a4882748f6b85c5e80a.
All four package manifest hashes matched.
Installer SHA256: 659d950a8709a1c98dfbec7e7547658150d05a83037c568a9bf613a6d863d618.
VST3 ZIP SHA256: 87c120abe046072ec45d946e93352da443a5c780633ccf284efba9ef8a5bc4e5.

Native wire-entry-h PNGs inspected at 2560x1440 and 822x600: four rows retained, row 2 pre-entry blocks dimmed/skipped, arrow enters before H, direct input plus row-1 source caption retained, successful whole-row update feedback visible. These invoke the shared editor action; they do not verify physical OS popup interaction. Installer unsigned.

Next: compatible effects-quality improvements, audio topology transitions and native OS interaction. No profiling or additional looper work. Windows11/Anubis hardware and subjective listening remain user-side.
