# 6.35 — selected-wire disconnection candidate

Right-click a row-to-row routing wire in Custom row mixer mode to open a menu naming its source and destination. Choose Disconnect this connection to remove only that source bit. Other input/row sources, taps, entry positions and device settings are preserved. Save the preset to retain the edit. This uses the existing routing engine, including its existing tail and topology-transition behavior; no seamless-transition claim.

The asynchronous menu captures both routing and full-state recall revisions. The processor validates them under the existing state-move/routing locks before editing; stale routing, stale recall, invalid row indices, self/backward routes, absent connections and non-custom mode are rejected. A failed edit reports that routing changed. No file-dialog, DSP algorithm, parameter IDs or preset schema changes. Version metadata is6.35.0.

Native tests invoke the shared menu action at2560x1440 and822x600, verify rejected stale edits, one-bit removal, retained direct-input/other-row sources, cleared focus and state roundtrip. Actual OS right-click/menu injection remains unverified. Native disconnected-grid screenshots and Windows build/pluginval/startup/installer/hash verification are pending. Do not promote until those gates pass; delivered app remains6.34.

Remaining: richer selected-connection editing, preset-file rejection feedback, physical mouse/menu interaction tests, routing audio transition behavior and effects-quality improvements. Preserve four rounded-square rows/full-width layout. No amp profiling/looper additions or proprietary DSP/catalog equivalence claims. Anubis/Windows11 hardware and subjective listening remain user-side.
