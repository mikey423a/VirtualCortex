# Virtual Cortex 6.44 verification

Custom Routing adds per-preset Smooth row entry (5 ms), default off. Affected slots ramp between dry and processed signals. Each slot runs once per sample when the wet fraction is nonzero. Fully skipped delay and reverb slots pause their histories and fade back in when reentered. This changes the affected slots only and keeps destination entry editing and source-tap settings separate.

Local GCC engine check compared both channels against a clock-matched independent render through removal, an idle gap, and reentry at 44.1, 48 and 96 kHz. At 48 kHz the first left sample step was .263963 immediate vs .00657018 blended for delay and .30231 vs .000389196 for reverb. Both retained tails produced nonzero output after reentry, with zero input and a silent skipped interval.

The fixture exposed a fractional reverb read wrapping to exactly buffer size due to floating-point rounding. AddressSanitizer confirmed a one-past-end read. The corrected read maps this position to zero. Local reverb regressions and sanitizer checks passed after the correction, including the original version of the row-entry removal fixture. Native toggle and preset migration checks are included for Windows CI. 

[Windows CI run 44449584087](https://github.com/mikey423a/VirtualCortex/actions/runs/44449584087) built 554cec1500f916a8b845df0c3842adb2d01e51c5. All 48 regressions, pluginval at strictness 5, ten-second standalone startup, installer installation and uninstallation passed. The six Windows transition results reproduced the local renders. Downloaded archive digests and all four package hashes matched. Native Custom Routing screenshots reviewed at 2560 × 1440 and 822 × 600.

Setup SHA-256: 7B0CB2B4C4E9568D880FD9AC1B14EEECE941DD27CD66787BF9511322F3EBF4B6
VST3 ZIP SHA-256: DF378816E3A73EF917C799AF4F38EEE48CF6790538CA382F249C917C48A5335D

Next: measure mode changes separately and test rapid entry reversals and multi-slot fade behavior. Anubis playback and listening remain user-side. Installer unsigned.
