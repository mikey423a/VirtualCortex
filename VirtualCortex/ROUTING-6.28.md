# Virtual Cortex 6.28 routing — verified

Source-tap dots and destination arrows are now included in this verified milestone. Connection horizontals run in the gutter above the destination row. Compact tap selectors use <A for Before A and A> through H> for After A through H; tooltips identify the source row and full boundary. Routing row enable changes immediately refresh the grid. Added native UI assertions for tap state/label/tooltip synchronization and screenshots with editors both visible and hidden.

DSP and preset formats are unchanged from 6.27. The four-row rounded-square grid and full-width 2560x1440 layout are preserved. No profiling or additional looper work; no proprietary DSP equivalence or full playable device-catalog claim.

Verified code: a8c8423a52e5c893bd034abe50659a00f6277983. [Windows CI run 43804654764](https://github.com/mikey423a/VirtualCortex/actions/runs/43804654764) passed on November 16, 2026 using Windows Server 2022, MSVC, JUCE 8.0.10 and ASIO enabled. Standalone EXE and VST3 compiled; all 45 regression tests passed; pluginval strictness 5 passed; standalone stayed running for 10 seconds; installer deployment and full uninstall passed. Native 2560x1440 expanded/editor-visible grids and 822x600 routing/editor-visible screenshots were inspected. Compact tap labels are readable and destination horizontals clear block badges/outlines. Local routing regression passed AddressSanitizer/UBSan (leak detection disabled for ptrace environment). Both downloaded artifact archives and all four packaged binary hashes were verified.

Artifact SHA256: Windows archive 8561f08bc6b2c301248b58921de516d776045924cc43567b99d8f927a81c168c; diagnostics 95e578d53727764e507eedaaf7a2fb99c06bba90465f5e7fc400b006932b9654. Installer is unsigned. Windows 11 interactive operation, Anubis hardware and subjective listening remain user-side.

## Next work and known limits

- Overlapping long vertical wires can still appear to share a bus. Separate their tracks and make selected connections identifiable; row captions and the routing panel remain authoritative.
- Show source/output activity and inactive routes more clearly, including when the grid engine is off.
- Structural route changes remain immediate; investigate audio-safe transitions with stateful effect tests before changing DSP execution.
- Multiple entry points in one row, drag-to-connect, hardware multichannel buses, scene routing and DAW automation remain incomplete.
- Skipped processors do not advance their tails. Connections remain attached to physical rows/columns when devices move. Preserve these preset semantics unless explicitly migrated.

Continue from the current remote branch head, inspect active CI/development before editing, and use expected-SHA guarded ref updates. The previous candidate a5ad9c6b8c80e6fa3bd3c633e2ebe573221fb575 was also verified through run 43797689385 before this change. Its source-tap wiring was retained and visually refined here.
