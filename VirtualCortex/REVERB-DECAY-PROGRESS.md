# Reverb decay characterization

Local GCC -O2 run against verified 6.33 source: 12 configurations at 44.1/96 kHz, requested decay 0.5/2/6 seconds, size 0.5/2, low/high decay ratios 1, modulation off, predelay/early off. Stereo energy integrated backwards; -5 to -25 dB crossing interval extrapolated to 60 dB. Measured/requested ratios 0.871745–0.962279; shortest measured 0.4605 s, longest 5.46803 s. Full existing reverb regression executable also passed.

This reveals nominal decay underestimation in these settings; it is not a certified RT60 measurement across bands/control ranges or proof of listening quality. Linear tank interpolation and input/diffusion filtering affect spectral energy and therefore broadband estimates. Preserve existing preset sound pending band measurements and an explicit compatibility decision.

Added sweep to existing Windows reverb regression, with broad 0.8–1.1 requested-decay guard. Windows verification pending. No application DSP or version change. Next: octave-band decay measurement, damping ratios, modulation on/off, reference renders and listening. Delivered release remains 6.33 (code 7ecd8a2bedbae92e29b469309956894335b9405d, CI 43993250604). No branded equivalence claim.
