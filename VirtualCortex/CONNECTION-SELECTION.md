# Connection selection — verified status

Individual wire selection shipped in **6.33.0**, compiled code `7ecd8a2bedbae92e29b469309956894335b9405d`, Windows CI [43993250604](https://github.com/mikey423a/VirtualCortex/actions/runs/43993250604). See [release verification](ROUTING-6.33.md). Earlier candidate warnings are superseded by that verification.

Parent-background clicks use the same five-point polyline geometry as painting, with six-pixel screen-space tolerance. Nearest wire wins; exact-distance ties retain the first match. The selected active connection is yellow; inactive selection is dim gray. Mint row focus, source taps, destination arrows, rounded-square blocks and full-width four-row layout remain intact. Child block controls retain mouse handling.

Native tests exercise the shared mouse-selection handler at 2560x1440 and 822x600, assert that the visible editor owns the chosen gutter point, and check miss/slot-selection clearing. Native selected-wire screenshots were inspected. Topology tests on `e157fa9287cd31bbd8c50b140d86c169c617b4cd`, Windows CI [44045963507](https://github.com/mikey423a/VirtualCortex/actions/runs/44045963507), additionally verify clearing after connection removal, source-tap change, destination-entry change and leaving custom routing. See [test notes](ROUTING-FOCUS-TESTS.md).

6.34 now clears old wire focus on full preset-file/host state recall even with identical topology; rejected state blobs and gain-only edits retain it. Immediate selection after a completed recall survives the later refresh. Native regression and screenshots passed Windows CI 44072683824; see [6.34 notes](ROUTING-6.34.md).

Remaining: physical OS mouse injection and interactions near child bounds/overlapping rails beyond portable geometry tests. Scene/factory quick-preset focus policies are unchanged. Current tests call the handler and refresh path; they must not be described as end-to-end physical mouse testing. No DSP or preset changes arise from selection.


## Verified 6.35 routing milestone

Delivered application 6.35, code a74129e652e3df9884b4c0080c2a058171aeb716, CI 44082073363: all 48 regressions, Windows EXE/VST3, pluginval5, startup, installer install/uninstall and downloaded hashes passed. Native disconnected-grid screenshots reviewed at 2560x1440/822x600. Right-click row-wire disconnection removes only the chosen source and rejects stale routing/recall actions. No DSP change or new effects-quality claim. See [6.35 verification and remaining work](ROUTING-6.35.md). Physical menu interaction, selected-wire tap editing, preset rejection feedback and audio topology-transition improvements remain next steps.
