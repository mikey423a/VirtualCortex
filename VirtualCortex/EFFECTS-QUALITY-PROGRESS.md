# Effects quality — current status

Delivered application **6.49.0**. [Windows CI 44503146399](https://github.com/mikey423a/VirtualCortex/actions/runs/44503146399) built `5e7f98c14dece1f979ed700ccbe1439ae5b0e7f6`. Grid and post-chain reverb offer independent optional Tail Detail. See [6.49 verification and tone characterization](../validation/GRID-REVERB-DETAIL-6.49.md).

The [6.41 reverb control optimization](REVERB-CONTROLS-6.41.md) preserved frozen 6.40 renders across tested automation and restarts. One paired Windows microbenchmark measured 1.88% lower steady time and 2.03% lower automated time for those control changes.

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


## Verified 6.35 routing milestone

Delivered application 6.35, code a74129e652e3df9884b4c0080c2a058171aeb716, CI 44082073363: all 48 regressions, Windows EXE/VST3, pluginval5, startup, installer install/uninstall and downloaded hashes passed. Native disconnected-grid screenshots reviewed at 2560x1440/822x600. Right-click row-wire disconnection removes only the chosen source and rejects stale routing/recall actions. No DSP change or new effects-quality claim. See [6.35 verification and remaining work](ROUTING-6.35.md). Physical menu interaction, selected-wire tap editing, preset rejection feedback and audio topology-transition improvements remain next steps.


## 6.40 IR transition follow-up verification

Test code 74d512c8b371d05e008da0178902b2c945f5782f passed Windows CI [44355281102](https://github.com/mikey423a/VirtualCortex/actions/runs/44355281102): all 48 regressions and existing binary/pluginval/startup/installer/hash gates. Full 4096-tap histories now tested through portable slot movement, queued removal and buffer partitioning at three rates/four routing modes, with ordinary operator-new guards during processing. See [exact scope and hashes](IR-HISTORY-TESTS.md). Application code and delivered 6.40 installer are unchanged. Next implementation should advance compatible optional effects quality or topology-transition handling; do not repeat completed IR checks.
