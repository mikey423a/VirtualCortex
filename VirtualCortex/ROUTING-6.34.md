# 6.34 — state-recall wire focus candidate

Successful preset-file/host state restoration now publishes a nonserialized recall revision. The editor clears old wire selection on its next normal refresh, including when source/tap/entry and routing mode are unchanged. Invalid state data and ordinary row gain edits retain focus. Selecting a wire after a completed recall consumes the revision first, preventing a later timer refresh from clearing the new selection. No GUI calls are made from state-loading threads.

The marker does not change DSP, serialized preset fields or routing. Native review covers repeated identical-state loads, rejected binary data, gain-only edits, immediate post-recall selection and screenshots at2560x1440 and822x600. Existing row highlighting, four rounded-square rows and full-width layout remain intact. Version metadata bumped to6.34.0. Windows compilation and verification pending; delivered app remains6.33 until all gates pass and native screenshots are inspected.

This addresses preset-file/host state loading, not scene or factory quick-preset selection policy. Physical OS mouse injection, routing topology audio transitions, broader effects-quality work and hardware/listening tests remain unfinished. No profiling/looper work, proprietary DSP equivalence or full playable catalog claim.
