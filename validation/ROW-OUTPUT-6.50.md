# Virtual Cortex 6.50 candidate

Custom Routing adds **Smooth row outputs (5 ms)**, per preset and off by default. The option ramps changes to each row's OUT switch. The row continues processing and feeding downstream taps while its direct contribution to the mix fades. In Average mode, the denominator follows the sum of the row weights, with a minimum of one so the final audible row can fade to silence.

Portable regression renders use sustained asymmetric stereo input, Grid Stereo Delay feeding Grid Stereo Reverb, mid-fade reversal, and restoration after a silent output interval. Independent delay-only and reverb-only references test summed and averaged routing at 44.1, 48 and 96 kHz. The 48 kHz averaged first-sample discontinuity dropped from 0.0561698 to 0.00177774. Settled samples were identical to the immediate switch reference, and the reverb tail resumed after both outputs were muted.

Windows verification pending. Native GUI tests include control binding and preset migration. Windows 11 listening and Anubis hardware testing remain open.
