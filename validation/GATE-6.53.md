# 6.53 adjustable grid Gate

The grid Gate offers per-slot Threshold, Attack, Hold and Release. Enable **Adjustable Gate** in the parameter editor. Earlier presets recall the original gate by default. This is an original peak-triggered, per-channel gate. Threshold spans −80 to −5 dB, attack 0.2–50 ms, hold 0–500 ms and release 2–1000 ms. The gain ramps linearly between 5% and unity. The scene and preset formats retain each slot’s mode and controls. When the switch is off, audio uses the original detector and fixed attenuation.

Local portable-render tests and Windows regressions compared legacy gain sample by sample and tested threshold detection, independent stereo channels, attack midpoint, hold duration, release midpoint, attenuation and stream restart at 44.1, 48 and 96 kHz. Native editor tests covered threshold entry, preset and scene recall, and a missing mode field in an older preset. Native editor screenshots reviewed at 2560 × 1440 and 822 × 600 show all four controls.

[Windows CI 44660621412](https://github.com/mikey423a/VirtualCortex/actions/runs/44660621412), job 136037675340, compiled e3c018d345d0e8b9133427a14eb6ddc0cd1f07d1. All 48 tests, pluginval strictness 5, ten-second standalone startup and installer install/uninstall passed. Both downloaded Actions archive digests and four package hashes matched. Installer SHA-256 4B21FFBC057E167DE6F5A5F4D3408E78CD6E39537599C8399A8EEB705DE39651. VST3 ZIP SHA-256 4BDEAFDC98641ECFCCC9FF4A4F76FA6E680D72EB385372418B3FCDED01D6E802.

Next steps include physical guitar/Anubis listening for pick attack and decay, and closer editor-open grid proportions. Windows 11 hardware testing remains user-side. Installer unsigned.
