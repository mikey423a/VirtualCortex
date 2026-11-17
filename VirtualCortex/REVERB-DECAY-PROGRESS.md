# Reverb decay characterization

Local GCC -O2 run against verified 6.33 source: 12 configurations at 44.1/96 kHz, requested decay 0.5/2/6 seconds, size 0.5/2, low/high decay ratios 1, modulation off, predelay/early off. Stereo energy integrated backwards; -5 to -25 dB crossing interval extrapolated to 60 dB. Measured/requested ratios 0.871745–0.962279; shortest measured 0.4605 s, longest 5.46803 s. Full existing reverb regression executable also passed.

This reveals nominal decay underestimation in these settings; it is not a certified RT60 measurement across bands/control ranges or proof of listening quality. Linear tank interpolation and input/diffusion filtering affect spectral energy and therefore broadband estimates. Preserve existing preset sound pending band measurements and an explicit compatibility decision.

Added sweep to existing Windows reverb regression, with broad 0.8–1.1 requested-decay guard. Windows CI 44008241232 (job 133788680060) passed actual EXE/VST3 compilation, all 48 regressions, pluginval5, standalone startup and installer checks on ca128087719c2676c17e46c4c9d5d3799e1511e8. Job logs inspected. Local ASan/UBSan also passed with leak detection disabled. These are test-only additions; the delivered 6.33 application remains current. No application DSP or version change. Next: octave-band decay measurement, damping ratios, modulation on/off, reference renders and listening. Delivered release remains 6.33 (code 7ecd8a2bedbae92e29b469309956894335b9405d, CI 43993250604). No branded equivalence claim.


Modulation follow-up: six local GCC -O2 configurations at 44.1/96 kHz and depth 0/0.8/2 ms, requested decay 2 s, ratios1, predelay/early0. Measured broadband T60 1.82575–1.86088 s; finite nonzero output, max peak below .003 for .1 impulse. Existing regression executable passed. New modulation sweep now guards bounded output and broad decay range; Windows verification pending. This does not measure pitch sidebands, spectral decay or listening quality. DSP unchanged; next octave-band measurement and modulation artifact analysis.


Modulation sweep Windows CI 44022344230 succeeded for e13294ad7d27990e7a3ed1f56be05daffb2eea78. New local GCC -O2 band characterization at 48kHz, requested2s, centers500/2000/8000Hz, depths0/.8ms, high ratios1/.3 passed finite-energy/range guards. Ratio1 depth0 results1.98231/1.87637/1.46044s; ratio.3 results1.911/1.52694/.952s. Modulated ratio1:1.97213/1.9005/1.55375s; ratio.3:1.89738/1.55319/.970125s.

Analysis uses a single RBJ bandpass Q=sqrt(2), backward energy -5/-25dB crossings. Broad skirts leak other bands; these are exploratory band estimates, NOT IEC octave filtering or precise band RT60. Do not equate highDecay=.3 with exactly .6 seconds in these measurements. Next validate analysis filters with synthetic decays and narrower/cascaded bands before calibrating tank feedback. New Windows band tests pending. Application DSP/presets unchanged, current release6.33.


Band sweep Windows CI 44029407814 succeeded on 0c50e15a7bc2d267763960f5887bb3208ebe3721. Measurement oracle added: synthetic exponentially decaying center-frequency sinusoids, known T60 .5/2s, 500/2000/8000Hz, 44.1/96kHz. Local GCC -O2 full regression passed; raw and filtered estimates within .2% of known values in all12 cases. Guards allow2% raw/3% filtered to accommodate platform changes. This validates slope/time calculation for isolated tones only; cross-band leakage and multi-slope tails remain unvalidated. New Windows oracle CI pending. No app DSP/version change. Next measure leakage with two-tone tails and implement justified opt-in tonal/decay modes only after characterization.


Two-tone leakage oracle: equal .1 amplitudes at500Hz (T60=2s) and8000Hz (T60=.6s),48kHz. Single8000Hz bandpass estimated.640188s; two identical cascaded bandpasses estimated.6s. Local full regression passed. Confirms leakage bias in simple band estimates; does NOT validate all frequency/decay combinations or establish a standardized octave bank. New Windows check pending. Next compare real tank single/cascaded estimates before opt-in calibration. No application DSP change.
