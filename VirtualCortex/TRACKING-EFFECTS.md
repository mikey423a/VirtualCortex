# Prism Synth and Step Crush — 6.17

Original effects inspired by the broad instrument-synth and sequenced-crusher ideas
of Meris Enzo and Ottobit Jr. No Meris code, factory presets, circuit model or sonic
parity claim. Official references:
- https://meris.us/product/enzo/
- https://www.meris.us/wp-content/uploads/2017/03/Meris_OttobitJr_Manual_v3.pdf

## Prism Synth

Clean left-input monophonic pitch tracking runs before the existing amp/grid chain.
A decimated, bounded YIN-style difference search feeds two oscillators, with
sine/saw/pulse morph, detune, semitone transpose, glide, envelope attack/release,
resonant low-pass filter, tracking gate and dry/wet mix. Saw/pulse edges use polyBLEP.
Tracking covers approximately 55–1400 Hz; use single notes and a clean input.
Detection requires a full analysis window plus two matching estimates: expect
roughly 50–80 ms acquisition, dependent on sample rate, pitch and signal. This is
not a polyphonic tracker; chords, heavy noise and weak fundamentals may mis-track.
No pitch-bend export, MPE, arpeggiator, or Enzo poly mode in this version.

MIDI NOTES enables channel-1 note-on/off output from the VST3 to a host capable of
routing effect-plugin MIDI output. It is separate from the internal synth audio.
Transpose affects both; detune/glide affect only the internal voice. Note changes
send old-note-off before new-note-on. Silence, disabling MIDI/synth, and global
bypass release the active note. Prepare schedules all-notes-off for the next
callback. A stopped host cannot receive events until audio callbacks resume.
The stock standalone has no added external MIDI-device routing UI; its internal
synth works without MIDI routing. MIDI-out hardware/DAW compatibility needs user
verification. The host-owned JUCE MidiBuffer may allocate when it grows; core DSP
allocates only during prepare. The processor regression reserves MIDI capacity.

## Step Crush

Stereo pre-chain bit/sample-rate reducer, followed by a one-pole low-pass filter.
Six editable bipolar step values, manual step time (30–2000 ms), depth,
rate/filter/bit-depth target, and wet mix. Filter/rate steps span ±4 octaves at
full depth; bit-depth steps span ±12 bits. Base resolution 1–24 bits and sample
rate 48 Hz–48 kHz, clamped to the actual processing sample rate. Quantization and
sample-hold aliasing are intentional. Zero input remains zero.

Stutter captures a 5–250 ms stereo slice at the start of each step, then repeats it
until the next step, with adjustable mix. Step duration must exceed slice length
for repetitions to occur. This is a free-running slice repeater, not Ottobit's
complete triggered/random/hold mode system. Pitch sequencing, tap/MIDI clock, transport-position locking, random steps and
pattern save slots are not implemented. Version 6.18 adds optional host-BPM
following with manual fallback; see SEQUENCER-TEMPO.md.

## Placement and compatibility

Input → Prism Synth → Step Crush → existing serial/grid engine → existing master
delays/reverbs/looper → output. These two effects are global input processors, not
independent per-slot grid inserts. Both default off; the four-row grid is preserved.
Twenty-eight parameters append after the previous 102, preserving earlier IDs and
host order. Parameters are saved with presets/DAW state. MIDI notes and live DSP
histories are transient. Cortex-style rectangular blocks and existing fullscreen
layout are unchanged; the new controls use the existing parameter editor.

## Verification

Portable tests exercise harmonic-rich notes, noise rejection, release to silence,
finite output, all three sequence targets, stereo isolation and 44.1/48/96/192 kHz.
Native processor tests cover note events, bypass note-off, state and reserved-buffer
allocation checks. Native UI review captures both pages at 1440p and minimum size.
Windows compilation, pluginval and installer results are recorded by CI; do not
infer listening quality or Anubis verification from automated tests.
