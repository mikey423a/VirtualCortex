# Four-head echo prototype

Original mono-record/stereo-playback echo core, with four independently timed
heads. Each head has playback level, equal-power pan and feedback send. Feedback
is independent of playback level; the sum of sends is normalized to at most .95.
Linear interpolation preserves convex feedback reads. Recording is bounded.

Portable tests passed at 44.1, 48, 96 and 192 kHz for independent head timing and
pan, inaudible-head feedback, maximum-send stability, restart clearing and invalid
feedback sanitization. Compile with C++17 and Source as an include directory.

This prototype is NOT connected to the app or catalog. The current distributed
installer remains 6.14. Required before integration: parameter smoothing and
crossfaded timing, stereo input policy, bypass/tails, preset/automation/UI wiring,
allocation and performance checks, Windows build and listening validation.
It is not a Strymon Volante model; no tape/drum circuitry is modeled.
