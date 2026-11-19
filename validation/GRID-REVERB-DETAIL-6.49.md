# Virtual Cortex 6.49 candidate

Grid Stereo Reverb now exposes **Tail Detail** on page 3, independently for each slot. It reuses the post-chain 6.48 reverb implementation. The late wet projection blends between linear and cubic interpolation over 5 ms. The feedback tank retains its linear read. The field is appended and defaults off when absent from a preset or scene.

Portable actual-grid regressions at 44.1, 48 and 96 kHz matched the continuous endpoint renders during both fades. Turning the option back off returned exactly to the linear reference while audio was still decaying. The existing slot move test also exercised the enabled option. Native tests cover the control, slot isolation, independence from the post-chain control and state recall.

Windows verification pending. Follow up with spectral characterization and musical listening. Anubis/Windows 11 hardware testing is user-side.
