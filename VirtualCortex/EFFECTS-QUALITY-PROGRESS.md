# Effects quality — current status

Delivered application: **6.34.0**, code `4852ef977fb2a1ba8d2a8304b2d60aca17163aac`, Windows CI [44072683824](https://github.com/mikey423a/VirtualCortex/actions/runs/44072683824). See [release verification](ROUTING-6.34.md). The 6.34 change concerns wire focus on state recall; DSP remains unchanged from 6.33.

## Verified application behavior

StereoDelay caches unchanged high/low-cut coefficients at the original 16-sample update cadence; prepare invalidates the cache. Frozen legacy-renderer comparisons passed exactly under cutoff/time/mode/bypass automation and sample-rate restarts. One paired Windows microbenchmark reported 56.8601 ms cached versus 58.9395 ms legacy for 480000 stereo frames at 48kHz (six alternating passes, medians), about 3.53% less elapsed time. This is not an app-wide CPU guarantee or sonic improvement.

Native wire selection and visible-editor hit interception passed at 2560x1440 and 822x600. The earlier child-control assertion was preserved; making the editor visible and choosing its routing gutter resolved the test setup. Misses, slot changes, connection removal, tap/entry changes and leaving custom routing clear focus. See [connection selection](CONNECTION-SELECTION.md) and [topology tests](ROUTING-FOCUS-TESTS.md). Physical OS mouse injection remains unverified.

## Reverb measurements

[Reverb decay progress](REVERB-DECAY-PROGRESS.md) records exact commits, runs, test stimuli, band-analysis limitations and results. Broadbands alone are insufficient to calibrate the feedback network: linear tank interpolation colors high-frequency tails, and a single broad analysis filter admits neighboring decay components. Cascaded-band and mixed-tone diagnostics help distinguish these effects; they are not a certified octave filter bank.

Preserve existing preset sound. Any new interpolation/decay mode needs explicit compatibility handling, numerical/stability tests across rates and controls, allocation checks, CPU measurements and listening review before release. Official JUCE documentation describes [third-order Lagrange](https://docs.juce.com/master/structjuce_1_1dsp_1_1DelayLineInterpolationTypes_1_1Lagrange3rd.html) as reducing interpolation low-pass coloration at increased cost, and warns that stateful [Thiran interpolation](https://docs.juce.com/master/structjuce_1_1dsp_1_1DelayLineInterpolationTypes_1_1Thiran.html) is unsuitable for fast delay modulation. These are candidate-design considerations, not evidence that either is a drop-in improvement in this feedback tank.

## Remaining quality work

- Isolate modulation sidebands and transition residuals across frequencies/rates; distinguish intentional pitch movement from unwanted clicks and aliasing.
- Evaluate optional interpolation/decay modes with legacy behavior retained; do not globally retune old presets.
- Extend automated delay CPU characterization; keep microbenchmarks distinct from full application real-time load.
- Level-matched musical listening and Windows 11/Anubis hardware evaluation remain user-side.

Strymon/Eventide/Neural DSP are quality aspirations. No proprietary equivalence, full playable catalog or studio-quality certification is claimed. No profiling or additional looper work.
