# 6.52 native grid update

Grid blocks now use larger symbols and outlines, with the selected device name beneath the expanded tile and a working corner remove button. Empty slots reveal their insertion target on hover or selection. Native Windows captures cover the expanded grid and parameter-editor layout at 2560 × 1440 and 822 × 600.

The remove control clears the selected slot using the existing insert-empty operation. The test checks the badge hit target, selection changes, and actual removal. Blank grid slots still accept drag/drop. The UI retains routing-wire selection, row focus and inactive-state captions. DSP and preset serialization are unchanged.

Windows run [44548617609](https://github.com/mikey423a/VirtualCortex/actions/runs/44548617609), job 135645915624, built 0664ea9028189d449caf99569cfd3484dafa9e69. All 48 regressions passed, including native editor review. Both downloaded Actions ZIP digests and all four package hashes matched. Verified ASIO-enabled standalone and VST3, strictness 5 pluginval, ten-second startup, installer installation and removal. Setup SHA-256 B2454DA4F875F57DCC78EAD86E2C5ED70392BDE6AF638CE9D25480B781D91592. VST3 ZIP SHA-256 4C992799C5C6D63E4FD4CC2D161324AEE49A75AAE24CD1100E66DF97DFBAFDEF.

Inspected gate-grid-expanded and gate-grid-editor at both sizes. Current parameter and gig panels follow the Virtual Cortex desktop layout. The grid Gate still has fixed settings. Physical mouse testing, Anubis playback and Windows 11 listening remain user-side. Installer unsigned. Future visual work includes the editor-open row proportions and device parameter layout.
