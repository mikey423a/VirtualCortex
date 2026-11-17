# Connection selection — verified status

Individual wire selection shipped in **6.33.0**, compiled code `7ecd8a2bedbae92e29b469309956894335b9405d`, Windows CI [43993250604](https://github.com/mikey423a/VirtualCortex/actions/runs/43993250604). See [release verification](ROUTING-6.33.md). Earlier candidate warnings are superseded by that verification.

Parent-background clicks use the same five-point polyline geometry as painting, with six-pixel screen-space tolerance. Nearest wire wins; exact-distance ties retain the first match. The selected active connection is yellow; inactive selection is dim gray. Mint row focus, source taps, destination arrows, rounded-square blocks and full-width four-row layout remain intact. Child block controls retain mouse handling.

Native tests exercise the shared mouse-selection handler at 2560x1440 and 822x600, assert that the visible editor owns the chosen gutter point, and check miss/slot-selection clearing. Native selected-wire screenshots were inspected. Topology tests on `e157fa9287cd31bbd8c50b140d86c169c617b4cd`, Windows CI [44045963507](https://github.com/mikey423a/VirtualCortex/actions/runs/44045963507), additionally verify clearing after connection removal, source-tap change, destination-entry change and leaving custom routing. See [test notes](ROUTING-FOCUS-TESTS.md).

Remaining: physical OS mouse injection, interactions near child bounds/overlapping rails beyond portable geometry tests, and an explicit policy/test for identical-topology preset recall focus. Current tests call the handler and refresh path; they must not be described as end-to-end physical mouse testing. No DSP or preset changes arise from selection.
