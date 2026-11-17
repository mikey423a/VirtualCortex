# 6.35 — selected-wire disconnection — verified software milestone

Right-click a row-to-row routing wire in Custom row mixer mode to open a menu naming its source and destination. Choose Disconnect this connection to remove only that source bit. Other input/row sources, taps, entry positions and device settings are preserved. Save the preset to retain the edit. This uses the existing routing engine, including its existing tail and topology-transition behavior; no seamless-transition claim.

The asynchronous menu captures both routing and full-state recall revisions. The processor validates them under the existing state-move/routing locks before editing; stale routing, stale recall, invalid row indices, self/backward routes, absent connections and non-custom mode are rejected. A failed edit reports that routing changed. No file-dialog, DSP algorithm, parameter IDs or preset schema changes. Version metadata is6.35.0.

Native tests invoke the shared menu action at2560x1440 and822x600, verify rejected stale edits, one-bit removal, retained direct-input/other-row sources, cleared focus and state roundtrip. Actual OS right-click/menu injection remains unverified. Windows verification is complete; delivered application milestone is 6.35.

Remaining: richer selected-connection editing, preset-file rejection feedback, physical mouse/menu interaction tests, routing audio transition behavior and effects-quality improvements. Preserve four rounded-square rows/full-width layout. No amp profiling/looper additions or proprietary DSP/catalog equivalence claims. Anubis/Windows11 hardware and subjective listening remain user-side.


## Verified build — November 17, 2026

Code: a74129e652e3df9884b4c0080c2a058171aeb716. Windows CI: https://github.com/mikey423a/VirtualCortex/actions/runs/44082073363 (job 134034916053).

Actual Windows EXE/VST3 compilation, all 48 regressions, pluginval strictness 5, standalone 10-second startup and installer install/uninstall passed. Downloaded package and log archive SHA-256 digests matched GitHub artifact digests. All four package manifest hashes were independently checked. Native disconnected-wire PNGs at 2560x1440 and 822x600 were inspected: removed row 1-to-row 2 wire, retained row 1-to-row 3 and downstream routes, rounded-square four-row layout and inactive controls are visible. The screenshot status is the expected rejected repeated-disconnect test message; it is not a popup-menu capture.

Installer SHA-256: FBBF7903D1ABFBCD19804A9DBDA1C4B3F8DEC38774701A851C6EB64D54737CB5
VST3 ZIP SHA-256: D6B1D44EF22586C313C0271D5E053C05F01D35AA89D1C1995441FF025D1DB391

Physical OS right-click/menu selection and popup placement remain unverified: tests exercise the shared native action. No seamless audio topology-transition or subjective sound-quality claim. Installer unsigned; Anubis and Windows 11 hardware testing remain user-side. DSP algorithms and legacy preset sound remain unchanged.
