# 6.41 reverb control efficiency — candidate

Skip already-settled reverb smoothers and reuse unchanged duck envelope/wet-gain coefficients across configure calls. Preparation invalidates the cache. No parameter IDs, preset schema, routing, tank interpolation, decay design or GUI layout changes. Existing preset audio is intended to remain exact.

Frozen 6.40 renderer comparisons cover six preparation rates/restarts, repeated and automated controls, malformed release/wet values, bypass, sleep/wake and predelay changes; stereo samples must match bit-for-bit. Existing reverb tests retained. Local GCC -O2 passed; ASan/UBSan reached final PASS with leak detection disabled. Local paired six-pass benchmark, 480000 frames at 48kHz, configure every64: steady optimized153.066ms versus legacy164.981ms; automated158.252ms versus162.545ms. Timing is informational, not an app-wide CPU guarantee. Windows verification pending; do not promote or deliver until gates pass.

Next: optional compatible sonic improvements, audio topology-transition policy and physical OS interaction. No sonic-equivalence, click-free or whole-app completion claim. Anubis/Windows11 hardware and listening remain user-side.
