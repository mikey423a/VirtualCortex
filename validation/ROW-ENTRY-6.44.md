# Virtual Cortex 6.44 candidate

Custom Routing adds per-preset Smooth row entry (5 ms), default off. Affected slots ramp between dry and processed signals. Each slot runs once per sample when the wet fraction is nonzero. Fully skipped delay and reverb slots pause their histories and fade back in when reentered. This changes the affected slots only and keeps destination entry editing and source-tap settings separate.

Local GCC engine check compared both channels against a clock-matched independent render through removal, an idle gap, and reentry at 44.1, 48 and 96 kHz. At 48 kHz the first left sample step was .263963 immediate vs .00657018 blended for delay and .30231 vs .000389196 for reverb. Both retained tails were audible in the numerical render after reentry, with zero input and a silent skipped interval.

The fixture exposed a fractional reverb read wrapping to exactly buffer size due to floating-point rounding. AddressSanitizer confirmed a one-past-end read. The corrected read maps this position to zero. Local reverb regressions and sanitizer checks passed after the correction, including the original version of the row-entry removal fixture. Native toggle and preset migration checks are included for Windows CI. Pending packaging and screenshot review. The verified release remains 6.43.
