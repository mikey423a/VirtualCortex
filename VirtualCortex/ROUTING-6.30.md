# 6.30 selected-row routing focus — candidate

Base 3a7f7450b638ad51bcd026382f8efe365723622d passed Windows CI run 43844024515 including regression compilation, pluginval/standalone and installer checks. Its stateful routing oracle is now Windows-tested.

Selecting a grid slot highlights incoming/outgoing connections for its physical row using brighter, thicker wires and arrows; unrelated active routes remain faintly visible. Selection explicitly repaints the parent. Inactive routes remain dim. This is row-based focus, not individual wire hit-testing or drag-to-connect. Added native screenshot scenarios for row 2 and row 4 at 2560x1440 and compact row 4 at 822x600.

Windows compilation/tests and native screenshot inspection are pending for this candidate. Do not distribute or promote as verified until those pass. Main and delivered installer remain 6.29. DSP/preset formats, four rows, rounded-square blocks and full-width layout are unchanged.

Next: inspect candidate CI and screenshots, particularly incoming merge/outgoing branch focus and disabled-grid appearance; fix issues, validate installer hashes, then deliver. Follow with clearer inactive controls and actual-effect transition regression coverage before topology crossfades. No profiling/looper work; Anubis and listening remain user-side.
