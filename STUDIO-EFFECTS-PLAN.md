# Studio effects: quality-first implementation plan

Status: researched design and acceptance requirements; stereo delay implemented in 6.9 and the first stereo FDN reverb in 6.10.
This supersedes catalog-count growth as the immediate priority.
Current 6.11 passes 28 software checks; these do not certify sonic quality.

## User requirements

Studio-quality reverb/delay is the target. Expose meaningful deep controls rather than just many names. Keep the Cortex-style grid, browser, dark panels, category accents and rotary controls. New features should look native to that design.
The delay must regenerate repeats and support a separately editable repeat-feedback path. Grid Delay now exposes an optional filtered repeat return in 6.8. The 6.9 post-chain stereo delay includes a cross-feedback matrix; a freely insertable feedback-effects chain remains unimplemented.

Interpretation pending further user detail: implement a dedicated repeat-processing path first, leaving dry guitar untouched. Do not silently build arbitrary feedback connections between the four main rows. Arbitrary insertable loop graphs are a separate routing feature.

## Research and architecture decision

1. Reverb: evaluate an 8/16-line stereo feedback-delay network with input diffusion, separate early reflections and frequency-dependent attenuation. Use an energy-preserving feedback matrix as the starting point; evaluate modulation and density instead of assuming extra delay lines guarantee a good sound. Signalsmith's author describes this approach, including mixing, diffusion and modulation: https://signalsmith-audio.co.uk/writing/2021/lets-write-a-reverb/ .
2. Decay: fit and measure decay by frequency band. Prawda, Valimaki and Schlecht describe attenuation-filter design for controlled FDN reverberation times; use the method as a research basis, not an untested quality claim: https://www.dafx.de/paper-archive/2019/DAFx2019_paper_46.pdf .
3. Room IR mode: evaluate partitioned convolution as a separate mode. JUCE provides stereo partitioned convolution, configurable latency and a non-uniform option for long IRs: https://docs.juce.com/master/classjuce_1_1dsp_1_1Convolution.html . A true-stereo four-IR matrix requires explicit LL/LR/RL/RR routing; a two-channel convolver alone must not be labeled true stereo. Impulse-response assets need an appropriate redistribution license.
4. Delay: benchmark fractional-delay interpolators, then select live/render quality options based on measured error, latency and CPU. JUCE documents delay-time smoothing and several interpolation choices: https://docs.juce.com/master/classjuce_1_1dsp_1_1DelayLine.html . Signalsmith supplies Lagrange/polyphase/Kaiser-sinc primitives and output-quality tests: https://signalsmith-audio.co.uk/code/dsp/ .
5. Plate-style work: Dattorro's reverberator/filter paper is another design reference, not evidence that a prototype matches a named hardware plate: https://aes2.org/publications/elibrary-page/?id=10160 .
6. Feature benchmarks: use commercial manuals to understand useful controls, not to claim their DSP has been reproduced. Timeless documents independent timing, tempo sync, cross-feedback, freeze, ducking and time-change behaviors: https://www.fabfilter.com/help/timeless/using/delaycontrols . Pro-R documents frequency-dependent decay editing: https://www.fabfilter.com/help/pro-r/using/decayrateEQ .

Signalsmith's main DSP library is MIT-licensed according to its author. Retain notices and pin any incorporated revision. The separate example-code repository does not automatically inherit that license; inspect its licensing before copying. No third-party implementation has been incorporated for this plan.
CloudSeed was also investigated, but it is not selected as a dependency; the current reusable core and each dependency need their own revision/license review.

## Proposed delay control groups

These are extended targets; only subsets are available in 6.9/6.10.

| Page | Controls |
| --- | --- |
| Main | Independent L/R delay in ms; host/manual tempo; beat division; dotted/triplet values; tap tempo; time offset; wet/dry and independent output levels |
| Routing | Stereo, dual and ping-pong behavior; per-channel feedback; cross-feedback; feedback polarity; tap levels and pan |
| Repeat FX | Loop input/return; high/low-cut filters and slopes; feedback EQ; drive/tone; optional diffusion and modulation; wet-loop amount |
| Modulation | Rate, depth and phase; per-channel offsets; wow/flutter-style variation; envelope destinations |
| Dynamics | Repeat ducking threshold, amount, attack and release; optional input/feedback envelope choice |
| Advanced | Pitch-glide versus crossfaded time changes; freeze; spillover/kill tails; quality/latency; headroom and meters; lock wet mix |

Dry input bypasses the Repeat FX path. Read the delayed signal, process the repeat path, apply the feedback matrix/amount, then return it to the delay input with new input. The exact placement of wet output relative to Repeat FX must be explicit so the first repeat's sound is predictable.
Freeze and intentional self-oscillation need separate guarded behavior; ordinary feedback must remain stable across the allowed filter and drive ranges. Never rely on hard clipping as a substitute for stability design.

Start with filtered stereo feedback and measured interpolation, then add nonlinear coloration only with oversampling and aliasing evaluation. Do not label clean modulation as tape/BBD circuit modeling.

## Proposed reverb control groups

| Page | Controls |
| --- | --- |
| Space | Algorithm family; size; pre-delay and tempo sync; nominal decay; distance; early/late balance |
| Early reflections | Pattern, level, spread and filtering |
| Tail | Diffusion, density, damping; low/mid/high decay ratios and crossovers; modulation depth/rate |
| Tone | Input EQ, decay shaping and output EQ as separate functions |
| Stereo | Width, channel coupling, balance and mono preview |
| Dynamics | Ducking controls; gate; hold/release; freeze; spillover |
| IR/quality | IR selection and trim; convolution routing; latency/CPU mode; wet mix lock |

Algorithm names must represent actual processing differences. Parameter presets must be identified as presets. Shimmer/pitched tails wait for a separately validated pitch module.

## Engine and parameter changes required before integration

- Process stereo frames with explicit channel coupling. The current independent per-channel callback is insufficient for genuine ping-pong routing or a shared stereo reverb tank.
- Replace the six-normalized-control ceiling with typed, versioned per-device parameter descriptions. Preserve existing IDs and preset behavior; add stable new automation mappings rather than changing parameter count dynamically while hosted.
- Keep preset migration explicit. Existing basic blocks stay available for old sessions; new studio blocks use separate identifiers.
- Build/allocate convolution and long-delay resources away from the audio callback. Publish prepared instances at safe boundaries and reclaim retired instances off the audio thread.
- Avoid all-slot maximum-size preallocation for long stereo IRs. Budget memory per active instance and test preset switching.
- Smooth continuous parameters. Use controlled time-change transitions and tail crossfades where direct coefficient changes would click.
- Host tempo, transport, reported latency and dry/wet compensation must be designed together.
- Live, studio and offline quality modes need measured CPU/latency tradeoffs. More CPU or more parameters alone does not prove higher sound quality.

## Cortex-style UI contract

The four-row grid stays the main view. Select the delay/reverb block to open its existing-style parameter editor. Add Main / Tone / Feedback / Modulation / Advanced pages, using the same rotary/value tiles, typography and accent colors. Editors can still be hidden.
The Feedback page shows the repeat return path and its internal processing. Do not introduce an unrelated plugin skin or floating rack. Extra controls must be accessible; six generic knobs are not the final interface.

## Quality acceptance gates

Before a studio-quality claim:
- Record impulse/step/swept-sine responses across supported rates and extreme valid settings.
- Measure octave-band decay, buildup/echo density, coloration and modulation artifacts. Set documented limits for a declared algorithm/range; do not treat one universal number as a definition of quality.
- Measure interpolation amplitude/phase error and aliasing against a higher-quality offline reference.
- Verify stereo coupling, channel isolation where expected, width and mono fold-down behavior.
- Test long tails, near-maximum feedback, freeze, bypass/spillover, preset changes and sample-rate changes.
- Check sample/block-boundary automation, zipper noise and discontinuities, not only finite output.
- Require no allocation/locking in the realtime path; measure worst-case CPU and deadline margin on a declared reference machine at guitar-friendly ASIO buffer sizes.
- Export level-matched guitar DI, chords/transients and other musical material for blind listening comparisons. Different creative effects have no single objectively best output. Do not present a preference test as formal codec-quality certification.
- Repeat Windows EXE/VST3, host validation and installer checks. Anubis hardware input/output still needs a real PC test.

## Development order

1. Stereo-frame processing, extensible parameters, benchmark renders and automation tests.
2. Stereo digital delay with an explicit repeat-feedback path and Cortex-style editor pages.
3. Diffused stereo FDN reverb with measured band-dependent decay and modulation.
4. Convolution/hybrid IR mode.
5. Additional character models and modulation only after the clean cores meet their acceptance tests.

No studio-quality certification, commercial-plugin parity, or full factory-catalog completion is claimed by this document.

## Completed first step: 6.8

REPEAT FX is an opt-in, saved per-slot Grid Delay setting. It exposes loop high-pass, low-pass and filter-blend controls in the existing Cortex-style block editor. Regenerated repeats traverse those filters; dry input and the first repeat do not. Time and cutoff changes are smoothed, and the optional path uses third-order Lagrange delay reading. This is not the full proposed studio delay. The legacy path remains when Repeat FX is disabled, and old presets default to that path. Next priorities remain true stereo-frame routing, deeper typed parameters, long-delay modes and the quality/listening gates above.

## 6.9 stereo delay milestone

Verified Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41438893229 (source c9632c4f16addf3507606bc251df3d6c663626be). All 27 CTests, pluginval level 5, standalone startup and installer install/uninstall checks passed. Editor screenshots were inspected across all three pages and the four-row view.

A new optional Stereo Delay runs after the main serial/grid chain and before output trim. It provides independent L/R times up to 16 seconds, feedback/cross-feedback, ping-pong, repeat-loop high/low-pass filtering and blend, modulation rate/depth, wet ducking with editable release, width, wet level, time glide, and host/manual tempo with numeric beat multipliers. Its 18 continuous controls and four switches occupy Main, Tone/Mod and Dynamics/Tempo pages in the existing editor style. Tape Glide describes pitch-changing time transitions, not a tape circuit emulation. Duck attack is fixed at 5 ms. Manual times and cross-feedback are disabled when overridden by sync and ping-pong respectively.

Original 28 host parameter IDs retain their order; 22 new parameters are appended (50 total). Older presets default the master effect off. Tests cover stereo isolation, ping-pong timing, ducking, fractional interpolation, malformed controls, automation allocation checks, tempo, recall and global output mute. This is one post-chain master module, not an insertable Grid block or arbitrary feedback-effects chain. The 21 existing Grid processing types remain. FDN reverb, convolution room modes and the full factory catalog remain unfinished. Studio-quality listening evaluation, reference-device matching, Windows 11 audio behavior and Anubis hardware I/O are not certified by these tests. Installer unsigned.

## 6.10 stereo FDN reverb milestone

Verified Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41447139700. Source 5cb1525a24e40bd73b6547367e86d1aaf03cda56. The EXE/VST3 compiled on Windows Server 2022; all 28 CTests, pluginval strictness 5, standalone startup and installer install/uninstall checks passed.

Stereo Reverb is a new optional post-chain master module, following Stereo Delay and preceding global output trim. It is not a Grid insert and does not replace the old reverb blocks. Select Stereo Reverb from the category dropdown, choose a starting preset and use SHOW EDITORS. Space / Tone-Mod / Output-Dynamics pages expose 18 parameters: nominal decay (0.2–20 s), size (0.5–2), predelay (0–500 ms), diffusion, early/late balance, mix, low decay ratio (0.25–2), high decay ratio (0.1–1), low/high crossover frequencies, modulation rate/depth, width, wet ducking, duck release, wet level and input high/low cuts. Duck attack is fixed at 5 ms. Size/predelay automation uses smoothed moving reads, so changes can shift pitch. Power fades the wet mix down rather than providing a spillover/freeze mode.

The original algorithm uses eight unequal feedback delays, normalized orthogonal Hadamard mixing, four input allpasses per channel, separate early taps, linear fractional reads and two frequency-dependent feedback shelves. Linear interpolation colors the high-frequency decay; the nominal decay and ratios are design targets rather than exact measured band RT60 guarantees. Filters have bounded gain at fixed settings; buffer guards remain for malformed/extreme inputs. This is not a factory model or a copied proprietary algorithm.

There are now 69 host parameters: all previous 50 IDs and their order are retained, with 18 reverb controls and one enable appended. Old presets default the new master off. Portable tests verify broadband decay, stereo decorrelation, zero wet width, early timing, damping, ducking, extreme automation, malformed controls, bypass fade and prepare/reset. A nominal 1.2 s unmodulated broadband tail measured approximately 1.10–1.13 s at 44.1/48/96/192 kHz using a -5 to -25 dB Schroeder slope. These measurements cover the tested configuration only. Native tests verify state migration, allocation-free tested automation, output mute and all editor pages.

Further work includes octave-band characterization across the control range, aliasing/discontinuity/CPU measurements, level-matched musical listening, convolution/hybrid modes and additional genuine device implementations. This release does not certify studio-quality sound, full Neural DSP catalog parity, Windows 11 audio behavior or Anubis hardware I/O. Installer unsigned.

## 6.11 pre-delay transition and mix precision

Verified Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41478258299. Source 345b067db573a24b07e781835f0e07def11b7fbb. All 28 CTests, pluginval level 5, standalone startup and installer install/uninstall checks passed.

Stereo Reverb adds PRE GLIDE in the existing editor. Off selects a 20 ms linear crossfade between two fixed pre-delay read positions, including the early-reflection taps and the feed into the late tank. Rapid changes queue the latest position after the current fade; switching to glide during a fade completes that fade first. This avoids sweeping the pre-delay read position in clean mode, but a transition can still cause temporary phase cancellation/comb filtering. Size and tank modulation continue to use moving reads. On retains the prior pitch-changing pre-delay glide.

Existing presets without srPreGlide default it on, preserving their pre-delay behavior. Newly chosen reverb browser presets select clean mode. There are now 70 host parameters; the previous 69 IDs and their order are unchanged, with the new mode appended. Tested mode changes and pre-delay automation allocate no C++ objects in the audio callback. Native tests cover UI binding, saved mode and migration defaults. Portable tests compare real output to the expected two-fixed-head blend at 44.1/48/96/192 kHz, including a mid-fade mode change.

Stereo Delay smoothing now converges exactly to its endpoints: a fully wet long delay does not retain a trace of current dry input, and zero mix reaches exact dry output. Both endpoints are tested at all four rates. Rounded-square grid blocks from 6.10.1 remain. These implementation and software checks do not certify studio-quality listening results, full device-catalog parity, Windows 11 audio behavior or Anubis hardware I/O. Installer unsigned.
