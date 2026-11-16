# Stateful routing verification progress

Base: verified 6.29, 8c75fc36972629929fa6a0850ef4c265298ea1de. Added a deterministic stereo one-sample-delay oracle to the existing custom routing regression. It checks that disconnected sources stop feeding input but still clock an existing tail, skipped prefixes do not advance the processor, and restoring the entry preserves the retained tail at the same physical slot. This documents current behavior, not gapless transition support or a production delay-quality test.

Local g++ AddressSanitizer/UBSan run passed including the existing custom-routing assertions (leak detection disabled for the ptrace environment). Windows CI is pending for this test-only change. No application/DSP/preset/UI changes or version bump; delivered 6.29 remains the verified app. Do not claim a new installer milestone from this change alone.

Next: inspect this commit's Windows CI; then add connection highlighting and inactive-control clarity with native screenshots, and expand transition tests to actual stateful effect instances before implementing topology crossfades. Preserve no-input tail processing and explicitly document any future change to skipped-state behavior. Anubis testing remains user-side.
