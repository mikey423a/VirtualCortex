# Step Crush pitch sequence - 6.19

PITCH SEQUENCE on the PITCH / STEPS page overrides the Rate/Filter/Bits sequence
target. The six bipolar steps map to +/-12 semitones at full sequence depth.
The base sample rate, bit depth, low-pass and stutter controls remain active.
Existing scMode range and host order are preserved; scPitch appends as parameter
137 and defaults off for older presets. Octave steps is a new starting variant.

Original stereo phase-vocoder implementation: 2048-point FFT, 512-sample hop,
Hann analysis/synthesis and independent channel phase state. Fixed buffers;
no allocation in processing. Pitch processing introduces window/buffering delay
and can smear transients. The dry path is not delayed, and this creative wet-path
latency is not reported as whole-plugin latency compensation. It is not a
transparent harmonizer or verified Meris emulation. Unison in pitch mode still
passes through the spectral engine. Pitch disabled preserves the original path
once the mode transition settles.

Portable tests check octave frequency, stereo isolation, finite output, bypass,
sequencing and cleared restart state at 44.1/48/96/192 kHz. Native processor tests
check octave output, allocations and parameter recall. Native UI tests capture
the pitch switch at 1440p and minimum size. Windows CI must pass before binaries
are released. Anubis and listening verification remain outstanding.

Still incomplete: transport-position locking, MIDI clock, triggered/random/hold
modes, pattern slots, and full branded-device parity. Catalog count remains 28.
