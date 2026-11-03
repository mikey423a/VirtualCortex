# Interval Echo and Layer Loom (6.16)

Two original post-chain processors, inspired by published Chase Bliss workflows.
No proprietary DSP, artwork or model data is included. They are not Thermae or
Blooper emulations, and sonic equivalence has not been established.

Interval Echo: stereo filtered feedback delay, two intervals (-12 to +12 semitones)
and a free-running three-step sequence (unshifted, A, B), adjustable step time,
base delay and mix. Two overlapping read windows shift pitch. The shifter adds
up to 60 ms of read-window delay. This is digital pitch shifting, not BBD clock
modeling. No MIDI clock, tap or manual step yet. Version 6.18 adds optional host-BPM
following with manual tempo fallback; see SEQUENCER-TEMPO.md.

Layer Loom: stereo 16-second capture, eight independent layers, record/end-record,
one-pass overdubs, stop/play, undo/redo/clear, reverse, 0.5-2x playback, low-pass
and loop level. Dry signal stays at unity. Initial recording ends on button press
or capacity; an overdub automatically completes one pass at normal forward speed.
Subsequent playback can change speed/direction. Endpoint fades reduce loop clicks.

IMPORTANT: loop audio is session-only, is cleared on audio-device preparation,
and is not stored in presets or DAW state. Presets retain controls only. The UI
states this explicitly. There is no file export/import, additive modifier printing,
random stability, sampler mode, or Blooper-compatible saved-loop format yet.

Controls are in the category selector and playable catalog. Record/play enables
the looper. Clear discards all recorded layers. Stop preserves the loop.

References checked November 3, 2026:
https://www.chasebliss.eu/thermae
https://blooper.chasebliss.com/resources/docs/manual.pdf

Portable tests verify octave frequency, stereo isolation, loop capture, overdub,
undo/redo and clearing at 44.1/48/96/192 kHz. Native tests cover actual audio output,
controls, state and callback allocations. Windows CI must pass before release.
