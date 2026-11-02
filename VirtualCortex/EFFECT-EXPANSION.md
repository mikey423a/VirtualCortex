# Effect expansion: 6.14

## Implemented: delay Loop Diffusion

Stereo Delay > Tone / Mod exposes Loop Diffusion (0-100%). Four fixed-delay
allpass stages per channel disperse the feedback return after the loop filters.
The first repeat is unchanged; later repeats become denser. Stereo channels have
independent diffusion times. Existing cross-feedback, modulation, ducking and
filter controls remain available. Prepare allocates the buffers; processing does
not allocate. The parameter is smoothed, appended at host index 70, and defaults
to zero when restoring older presets. This extends the existing post-chain delay;
it is not a new grid device, arbitrary feedback insert chain, or branded emulation.

Two browser presets: Diffuse cloud (modulated, filtered, diffused repeats) and Slow
bloom (longer times with ducking). These are parameter presets of one processor.

Validation: portable DSP tests at 44.1/48/96/192 kHz verify first-repeat preservation,
later-repeat dispersion, channel isolation, finite output and device restart.
Zero diffusion matched the pre-change engine exactly over 128,000 automated frames.
Native tests cover UI access, presets, state recall, legacy defaults and callback
allocations. Windows results are recorded after CI completes. Listening quality
and comparisons against physical pedals remain unverified.

## Design references and future directions

The user's approved references are Eventide, Strymon, Meris and Chase Bliss.
These public feature descriptions inform original implementations; no proprietary
algorithms or assets have been imported, and none of these products is emulated.

- Meris Polymoon: delay diffusion is the first implemented direction.
  https://meris.us/wp-content/uploads/2017/07/Meris_Polymoon_FULL_Manual_4.pdf
- Strymon Volante: independently controlled multi-head playback/feedback is a future target.
  https://www.strymon.net/faq/how-do-volantes-playback-feedback-buttons-work/
- Eventide Blackhole: large spaces and inverse-decay envelopes are future targets.
  https://www.eventideaudio.com/plug-ins/blackhole/
- Strymon Cloudburst: sustained ambient textures are a future target, not Ensemble parity.
  https://www.strymon.net/support/cloudburst/
- Chase Bliss MOOD MKII: microloop and frozen-effect workflows are future targets.
  https://www.chasebliss.com/mood-mkii

These planned effects are not yet playable and do not increase the playable catalog count.
