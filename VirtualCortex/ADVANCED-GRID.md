# Advanced grid effects — 6.23

All seven existing advanced processors now have independently owned grid variants:
30 Stereo Delay, 31 Stereo Reverb, 32 Four-head Echo, 33 Chase Pitch Delay,
34 Layer Loom, 35 Prism Synth, 36 Ottocrush. These reuse the original DSP; they
are not proprietary models or additional proprietary devices.

Select a Grid device in the directory and place it in the selected slot. Enable
GRID ENGINE. Select a block to edit up to eight parameters per page with NEXT /
PREVIOUS. All existing numeric controls and switches are available; the block
power switch supplies enable/bypass. Layer Loom has six transport/layer commands.
Grid instances are independent of the legacy global effects, which remain for
preset compatibility; enabling both processes both. Grid parameters are saved in
presets and scenes, but are not new host automation parameters. Existing 137
host parameters still address the legacy global processors.

Moves exchange complete DSP objects (delay tails, loop audio, pitch state),
settings, queued loop commands and stored scene settings. Topology is shared by
scenes. Presets save settings, not live audio/tails; Layer Loom audio remains
session-only. Replacing/removing a grid advanced device destroys its live audio
on the control thread. Bypassed/disabled lanes pause DSP, so tails freeze.

BPM sync follows the host when available, otherwise the per-instance tempo.
This is BPM-following, not transport phase lock. Prism tracks the signal arriving
at its slot's left channel (dual-mono routing uses that lane's mono input).
MIDI outputs use channels 2–16, shown in the editor, cycling by slot position;
shared channel/note ownership prevents one slot ending another's held note.
The legacy global synth retains channel 1. Standalone external MIDI routing is
not implemented. Moving a sounding synth may retrigger MIDI on its new channel.

Effect allocation/replacement is outside the audio callback under the existing
move guard; callbacks that cannot acquire it output silence rather than wait.
Moves are not guaranteed gapless. Each instance consumes its own buffers; many
16-second stereo delays/loopers at high sample rates can use substantial memory.

Verification added: all seven types retain sample-identical DSP state after a
move; every GUI control is traversed and bound; native preset/scene/independence,
legacy 6.22 migration, MIDI bypass note-off, loop move/undo/redo, and callback
allocation checks. Windows build and validation results must be checked before
shipping this version. Anubis hardware and listening tests remain user-side.
