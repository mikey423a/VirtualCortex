# 6.34 — verified preset-state wire focus

Code `4852ef977fb2a1ba8d2a8304b2d60aca17163aac` fixes stale connection selection after successful preset-file or host-state restoration. A nonserialized processor revision is published only after valid state loading completes; editors observe it during their normal refresh. No GUI calls are made by the state-loading thread. Identical routing still clears old focus. Invalid state data and gain-only routing edits retain focus. A new selection made after a completed recall consumes the revision before selection, so a later refresh does not erase it.

Native tests cover repeated identical-state restores, rejected binary data, gain-only edits, unchanged restored routing, immediate post-recall selection, and before/after screenshots at 2560x1440 and 822x600. Four rounded-square rows, full-width layout, row/wire highlights and inactive dimming are retained. DSP, serialized presets and parameter IDs are unchanged. Version metadata is 6.34.0.

## Verification

Windows CI [44072683824](https://github.com/mikey423a/VirtualCortex/actions/runs/44072683824), job134002601013, passed actual EXE/VST3 compilation, all48 regressions, pluginval strictness5, standalone10-second startup and installer install/uninstall/deployed-hash gates. Both artifact archives matched their GitHub SHA256 digests and all four packaged hashes were independently checked. Native selected-wire and recalled-wire-cleared screenshots were inspected at2560x1440 and compact822x600 sizes: the recalled connection returns to mint row highlighting, with no stale yellow selection. Artifact IDs: Windows14322314651, logs14323094585.

## Remaining work

- Add deliberate selected-connection editing/disconnection through the existing routing UI, with stale-selection protection and native interaction tests.
- Physical OS mouse-event injection remains unverified; current native tests invoke the handler/refresh path.
- Preset-file dialog currently reports load success after calling the host state API even if it rejects the blob. Use the recall revision to make its success/failure feedback accurate in a subsequent change.
- Scene and factory quick-preset focus policies are unchanged. This release's new policy applies only to full preset-file/host-state restoration.
- Routing topology audio transitions, broader modulation/aliasing characterization, compatible optional effects improvements and listening evaluation remain unfinished.

Installer unsigned. CI Windows Server 2022 verification is not Windows 11/Anubis hardware testing; those remain user-side. No proprietary DSP equivalence, full playable catalog, amp profiling or additional looper development is claimed.
