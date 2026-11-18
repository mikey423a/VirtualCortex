# 6.40 queued cabinet IR transitions — candidate

Rapid cabinet IR replacements no longer overwrite either FIR of an ongoing 128-sample crossfade. One fixed-storage pending target retains the latest request; when the current fade completes, the next fade starts from its endpoint. Returning to the active target cancels any queued replacement. Invalid inputs leave the queue untouched. The selected IR length reports the latest requested target. Device restart commits that latest selection, discarding obsolete fades/history. Slot swaps carry the entire state.

This changes only overlapping cabinet IR transitions. Ordinary single updates, steady FIR sound, preset schema and DSP parameters are unchanged. Additional fixed storage is one 4096-float FIR per slot (about 512 KiB across 32 slots, plus metadata), with bounded coefficient copies and no new process allocation. Latest target may wait up to one 128-sample fade (2.67 ms at 48 kHz) before its own 128-sample fade. No universal click-free or CPU improvement claim.

Expanded cabinet_ir_crossfade regression uses analytical one-tap output at 44.1/48/96 kHz in four routing modes. Covers rapid multiple targets, latest-wins, invalid request, queued cancellation, both channels, restart with pending target and queued removal. Fails against unmodified 6.39 and passes locally with fix. Existing add/remove fade and duplicate-update regressions retained.

Version 6.40.0 candidate. Windows build/regression/pluginval/startup/installer/hash verification pending; latest delivered app remains 6.39. GUI unchanged except version footer.

Next: optional compatible effects quality, topology transition policy and physical OS interaction. No amp profiling or additional looper development. Anubis/Windows11 hardware and subjective listening remain user-side.
