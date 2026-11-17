# Reverb modulation probe

Offline characterization against unchanged 6.33 StereoReverb DSP (development test commit 5b710e88277b5b83ce13bd065704afa6bbdd39c3). This helper was compiled/run locally with GCC -O2; it is not a Windows CI target and is not shipped in the app.

From repository root, with C++17 and Python/NumPy available:

```sh
g++ -std=c++17 -O2 -I VirtualCortex/Source validation/reverb-modulation/probe.cpp -o reverb-probe
./reverb-probe reverb-probe-output
python validation/reverb-modulation/analyze.py reverb-probe-output
```

Writes little-endian float32 stereo on the tested x86-64 platform. The script expects that format. Input is a .1-amplitude 1kHz sine in the left channel, right silent, 48kHz. Render12s, discard first4s, analyze8s with an unwindowed FFT (integer carrier/modulator cycles). Reverb: nominal1s, low/high decay ratios1, size1, predelay/early0, full wet, 2Hz modulation, depths0/.8/2ms. Other parameters retain defaults.

Local results November17:

| Depth ms | Stereo RMS | Fraction outside carrier | Fraction outside 980–1020Hz |
|---|---:|---:|---:|
| 0 | .0222058316 | 4.84e-14 | 4.85e-14 |
| .8 | .0373767089 | .992566875 | .036648078 |
| 2 | .0327860204 | .991649267 | .452606270 |

The zero-depth residual guard checks the stationary linear reference. No guard interprets modulation energy as bad: intentional frequency modulation and changing resonances contribute to the sidebands and output level. These results are not isolated aliasing measurements, distortion ratings, loudness-matched comparisons, or listening-quality certification. Only one carrier, rate and modulation frequency are covered. No tank interpolation, presets or app DSP were changed.

Next: expand carriers/sample rates and modulation frequencies, compare reference resampling approaches with stability/CPU/legacy-compatibility gates before adding optional modes. See ../../VirtualCortex/REVERB-DECAY-PROGRESS.md for Windows regression and decay evidence.
