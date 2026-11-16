# Routing 6.28 candidate

Base: development/grid-routing-modes a5ad9c6b8c80e6fa3bd3c633e2ebe573221fb575. Main remains verified 6.27 at 49007e1b72637f8d1485c38b88f350624785fada until this candidate passes.

The base Windows run 43797689385 passed all 45 regression tests, pluginval strictness 5, standalone 10-second startup, and installer installation/uninstallation. Its diagnostic archive SHA256 e704810f5833448ff98221c203f0ec26fb08c7c015ae1e014717f9de5b38f1bb was checked. Native 2560x1440 screenshot inspection found connection horizontals overlapping destination block outlines/badges.

Candidate changes: draw connection horizontals in the gutter above the destination row; compact source tap labels <A and A>–H> with full row/boundary descriptions in tooltips; immediately refresh the main grid when toggling routing row enable. Version is 6.28.0 to distinguish these binaries from 6.27. Native UI tests check tap label/state/tooltip synchronization and capture both expanded and editor-visible custom grids.

DSP, preset format, four rows and full-width layout are unchanged. No profiling or looper work. This candidate is not yet verified; inspect its Windows CI and native screenshots before delivery or promotion.

Next: verify candidate compilation, all regressions, pluginval, startup, installer deployment/removal and artifact hashes. Review expanded and compact native screenshots. Then improve overlapping long wires/connection selection, followed by routing transition behavior and additional routing features, each with audio tests. Multiple entry points, drag-to-connect, hardware buses and scene/automation routing remain incomplete. Anubis hardware and listening verification remain user-side.
