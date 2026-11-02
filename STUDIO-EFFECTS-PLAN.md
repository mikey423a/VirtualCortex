# Studio effects: quality-first implementation plan

Status: researched design and acceptance requirements, not implemented studio engines.
This supersedes catalog-count growth as the immediate priority.
Current 6.7 remains a functional milestone: its 25 passing software checks do not certify sonic quality.

## User requirements

Studio-quality reverb/delay is the target. Expose meaningful deep controls rather than just many names. Keep the Cortex-style grid, browser, dark panels, category accents and rotary controls. New features should look native to that design.
The delay must regenerate repeats and support a separately editable repeat-feedback path. The current Delay already regenerates repeats, but it does not expose a feedback-effects chain.

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

These are targets, not controls available in 6.7.

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
