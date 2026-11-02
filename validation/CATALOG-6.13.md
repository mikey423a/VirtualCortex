# Catalog browser verification — November 2, 2026

Source commit: 62f637c327cb0584d0417ae219dae8bbfab588dd.
[Successful Windows build](https://github.com/mikey423a/VirtualCortex/actions/runs/41528022183).

- MSVC Windows Server 2022 build produced the 6.13.0 standalone EXE and VST3 using JUCE 8.0.10 with ASIO enabled.
- All 32 CTest checks passed, including catalog integrity, native catalog selection/search/filter behavior, insertion guards, and the existing DSP/NAM/processor/UI checks.
- Native UI checks exercise case-insensitive search, gear queries without trademark marks, 23 playable originals, 632 released reference entries, 57 announced entries, 83 guitar-amp rows, no-match handling, original/post-chain callback routing, and Esc closing the panel.
- Native screenshots reviewed at 2560 × 1440 and 822 × 600 show readable fields, scrolling lists and the reference-only status. Existing four-row/fullscreen tests also passed.
- VST3 passed pluginval 1.0.4 strictness 5; standalone stayed open for the 10-second startup test.
- Inno Setup packaging, installation, installed-binary hash comparison and uninstall checks passed. Downloaded artifact and packaged-file hashes matched.
- Snapshot: 689 rows / 23 source sections. Original HTML SHA256: 1967bf6224010065eb3ffa9faf17ac961a1bcbc8848a2f98c4989481c3bf1ed5.
- Deterministic generation is checked against data/device-catalog.json. Same-named plugin entries are distinguished by their required plugin.

Completeness is limited to the factual source inventory. None of the 689 reference entries is a new playable model. The available audio processors remain the 23 Virtual Cortex originals, without a claim of Neural DSP equivalence. Live NAM GUI loading and the full playable device catalog remain unfinished. Windows 11 interactive use, Anubis hardware audio and listening evaluations still require testing. The installer is unsigned.
