# Actual-effect routing transition coverage

Candidate: real StereoDelay and StereoReverb at 44.1, 48 and 96 kHz. A separately clocked reference verifies disconnected-source tails, input exclusion, skipped-state retention and reconnection. Nonzero energy assertions prevent a silent renderer from passing. Local ASan/UBSan passed, with leak detection disabled. Windows CI pending; no new binary delivery or promotion on this evidence alone.

App remains 6.32.0; these changes add tests only. Verified app code is 1cc105744a41741042de96da2a850a44f7a5d936, CI 43894463366; see ROUTING-6.32.md. Next: individual wire selection/hit testing, retaining row-based highlighting and inactive dimming. No change to current frozen-tail skip behavior or topology smoothing.
