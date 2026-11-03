# Stored scenes — 6.20

A–H are now scene memories rather than grid-column shortcuts. Click an empty
scene to store the current sound; click a stored scene to recall it. Shift-click
to overwrite a scene. Save the preset or DAW project to retain stored scenes.
Edits are not automatically written back into the selected scene.

Each scene stores all 137 processor parameter values, four row enables, and
32 slots' bypass and parameter settings. Eight memories and the selected scene
are serialized in .vcpreset files and DAW state. Legacy presets clear the bank.

Devices, routing topology and loaded IR/model assets are shared across scenes.
A slot whose device type has changed since storing is skipped on recall.
Looper audio, playheads, sequencer phase and effect histories are not captured.
Recall uses existing parameter smoothing but is not atomic across an audio
block, sample-accurate, MIDI-triggered or guaranteed gapless. Scene operations
run on the control thread; they do not reload assets or call prepareToPlay.
The highlighted letter is the last stored/recalled scene, not a dirty indicator.

Validation exercises all eight scenes, parameter/boolean/slot/lane/bypass
recall, project round trips, legacy clearing, replacement-device protection,
and finite allocation-free audio callbacks following recalls.
