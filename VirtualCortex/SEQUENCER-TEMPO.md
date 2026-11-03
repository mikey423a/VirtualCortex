# Sequencer tempo extension - 6.18

Step Crush and Interval Echo now offer TEMPO SYNC, MANUAL TEMPO (20-300 BPM),
and STEP BEATS (0.125-4 quarter-note beats). With sync on, a finite positive host
BPM overrides manual tempo; otherwise manual BPM is used. The calculated step
duration spans 25 ms to 12 seconds. With sync off the previous manual-millisecond
control remains in use, with its previous host parameter range and default.

This is BPM following, not transport/beat-position locking, MIDI-clock input,
tap tempo, or a sequencer restart command. Step Crush retains its fractional
free-running phase; Interval Echo retains its elapsed-step counter. Neither
resets to step one on a DAW seek or transport start. Tempo changes can move the
next step boundary. Effects continue advancing while active even if transport
is stopped, provided the host continues audio processing.

The GUI disables manual step milliseconds while sync is on. The manual BPM is
a fallback even though it remains editable when a host supplies tempo. Beat
controls are plain numeric quarter-note multipliers: 1 = quarter, 0.5 = eighth,
0.25 = sixteenth, 0.75 = dotted eighth, 1/3 approximates an eighth-note triplet.
Existing presets default sync off. Six new parameters append after the previous
130; original parameter order is preserved. Catalog count stays at 28 playable
originals: these are extensions, not new named devices.

Also fixes the master-editor header: Prism Synth and Step Crush now display
INPUT PROCESSOR, matching their actual placement before the amp/grid engine.

Portable tests cover manual passthrough, tempo extremes, invalid tempo and long
steps. Native tests compare synchronized output to equal manual milliseconds,
confirm host BPM overrides the fallback and beat changes alter the sequence.
UI tests bind beat/sync controls, check disabled manual time and capture the
1440p editor. Windows CI remains the authority for the full release checks.

Still incomplete: pitch sequencing in Step Crush, triggered/random/hold modes,
transport position locking; Interval Echo tap/manual step and BBD modeling;
Prism polyphonic tracking/arpeggiation; Layer Loom audio file persistence.
No proprietary Meris/Chase Bliss DSP is included or claimed.
