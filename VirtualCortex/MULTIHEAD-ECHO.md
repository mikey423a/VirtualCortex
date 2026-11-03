# Four-head echo in 6.15

Original post-chain effect after Stereo Reverb, using mono recording (average of
stereo inputs) and stereo playback. Each head has time, level, pan and feedback;
all heads share a wet/dry mix. Feedback is independent of playback level and pan.
Aggregate sends are limited to .95. Times crossfade over 20 ms; gain/send changes
smooth over 10 ms. Bypass fades to dry and drains the recording buffer.

Select Four-head Echo from the category menu or playable catalog. Head buttons
open each set of controls. Four-head spread and Rhythmic cascade are presets.
No tape/drum coloration or branded algorithm is claimed. Tempo synchronization,
per-head filtering and listening comparisons remain future work.

Portable tests cover 44.1/48/96/192 kHz timing, pan, independent feedback,
maximum sends and restart. Native tests exercise UI, state and callback allocation.
Windows build results must be checked before distributing this version.
