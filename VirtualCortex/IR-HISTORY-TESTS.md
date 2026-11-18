# Cabinet IR history and movement regression

Follow-up to delivered 6.40. Test-only change; no app version/DSP/preset/GUI change. Extends cabinet_ir_crossfade with 4096-tap sparse FIRs including the last tap, 4200-sample history warmup, distinct stereo input, three sample rates and all four routing modes. Queued and moved block-rendered output must equal a stationary single-frame reference that receives replacements at fade boundaries. Moving A to H mid-fade must carry FIRs, pending target, fade counter and history. Queued removal completes without restart. Ordinary operator-new allocations are forbidden during the checked processing calls.

Local GCC -O2 passed. Windows full build/regression/pluginval/startup/installer/hash verification pending. Latest delivered installer remains 6.40 from code 4c0545b2c94e665db1718b125e2cb075da9f3215, CI 44292050163. No new installer release from this test-only follow-up.

Next: compatible optional effects improvements and topology transition policy; physical OS menu/file-dialog, Anubis/Windows11 hardware and listening remain unverified.
