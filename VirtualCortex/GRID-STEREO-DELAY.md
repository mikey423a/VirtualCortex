# Grid Stereo Delay development

Choose Grid Stereo Delay in the device dropdown, then Place stereo delay in
selected slot. Each slot owns its delay buffers. Drag swaps buffers, controls
and scene coordinates. Presets store slot parameters, not echo histories.

Six controls: left/right time (1–4000 ms), feedback (0–95%), mix,
cross-feedback and feedback diffusion. Other StereoDelay settings use their
DSP defaults except filter mix is zero. Full tempo/modulation/ducking/filter
controls and catalog integration remain pending. Global Stereo Delay is unchanged.

Dual mono folds each device output to mono inside its own chain. Bypass pauses
the slot rather than draining its tail. Device insertion allocates buffers while
the audio callback uses the existing move guard; a brief mute can occur.
Windows verification is required before release.
