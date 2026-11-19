# Virtual Cortex 6.46 candidate

Chorus, Flanger and Vibrato have an optional per-slot Detail Read. The audible fractional delay uses cubic interpolation when four historical samples are available. The feedback write continues using the original linear read. Integer positions and reads near the buffer limits keep the original path. The setting defaults off for older presets.

Local GCC tests at 44.1, 48 and 96 kHz measured squared sine interpolation error at .03686 of the previous read for a half-sample position and frequency fs/6. The exact feedback buffers matched in the high-feedback test. This is a specific interpolation accuracy result. Listening and broader modulation characterization remain open.

Native editor and preset checks are in the Windows candidate. Release pending Windows verification. The delivered app remains 6.45.
