# 6.37 source-tap menu — verified software milestone

Right-click an existing row-to-row connection in Custom row mixer mode and choose Source tap — row N, then Before A or After A–H. Current tap is checked. Only this connection's tap changes; other connections, source masks, destination entry and row mix settings remain unchanged. Save preset to persist. Existing disconnect action remains.

Processor action validates routing and recall revisions under existing locks, rejects invalid indices/taps, absent connections and non-custom mode. Same tap is an accepted no-op without a revision increment. Changed topology clears old selection through existing refresh policy. No DSP algorithms, parameter IDs, preset schema or default preset sound changes. Existing audio topology/tail behavior remains; no click-free claim.

Native regression calls shared menu action at 2560x1440/822x600: all nine taps, unrelated settings, no-op, stale routing/recall, absent connection/mode/range rejection, focus clearing, preset persistence and Before-A screenshots. Physical OS popup/submenu interaction remains unverified. Version 6.37.0 passed the software verification below.

Next: native OS menu interaction, whole-row entry editing with explicit scope labels, audio transitions and optional compatible effects-quality improvements. No profiling or additional looper work. Anubis/Windows11 hardware and listening remain user-side.


## Verified November 17, 2026

Compiled code b38534c1fcebbd8ba23e23b46fbf0cb360e4b821; Windows CI https://github.com/mikey423a/VirtualCortex/actions/runs/44186016423 (job 134402624883).

Actual Windows EXE/VST3 compilation, all 48 regressions, pluginval strictness 5, standalone 10-second startup, installer install/uninstall and downloaded archive/package hashes passed. Native Before-A source-tap PNGs were inspected at 2560x1440 and 822x600: source dot, destination arrow, routing caption, inactive dimming and four rounded-square rows remain visible. These are editor snapshots after invoking the shared action, not physical OS popup interaction.

Both artifact archive digests and all four packaged manifest SHA-256 hashes were independently checked.

Installer SHA-256: 6B2FD9FEC7EDF21A2BF519477215C8CB8EB18E5738D0E09F65953D3981BB35D4
VST3 ZIP SHA-256: 821C7F2E93D258F714CDBCE5CDD14AA4CBB83E4AF558BAD2F1D32B03DAE4432D

Installer unsigned. Physical submenu operation, Windows11/Anubis hardware and subjective listening remain unverified. Audio transition behaviour is unchanged. Next: compatible effects-quality improvements and whole-row entry editing with clear scope labels; entryColumn affects every input to that row, unlike sourceTaps[from].
