# Virtual Cortex 6.51 candidate

Custom Routing adds **Smooth connections (5 ms)**, off by default. Each selected input or row feed fades in or out when connected or disconnected. The last connected tap keeps supplying a disconnect fade. Average mode tracks fractional source weights with a minimum divisor of one. Downstream effects continue processing through the edit.

Portable tests at 44.1, 48 and 96 kHz compare an independently mixed input to a real downstream Grid Stereo Reverb fed from Grid Stereo Delay and direct input. The test covers mid-fade reversal, disconnecting both sources and restoring direct input. The 48 kHz averaged first-sample step dropped from 0.0149405 to 0.00111667. A separate oracle checks a concurrent tap change and fade to zero.

Windows verification pending. Anubis hardware and musical listening remain user-side.
