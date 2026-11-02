# Device coverage and completion criteria

Reference: https://neuraldsp.com/device-list (reviewed 2026-11-02).

## Current status

Virtual Cortex is an original, limited effects application. It does not include the complete Quad Cortex library and has not been validated for sonic equivalence to any listed device. A successful binary build verifies the software checks, not device-list parity.

| Reference area | Existing project capability | Remaining work |
| --- | --- | --- |
| Guitar and bass amps | Generic amp and custom VCMLP1 loader | Individual models, controls, reference measurements and validation; no dedicated bass library |
| Guitar and bass cabinets | Basic filter and user IR loading | Measured cabinet/microphone library and multi-mic controls |
| Guitar and bass overdrive | Original soft-clipping overdrive and hard-clipping distortion in Grid mode (6.5) | Individual pedal models, dedicated bass variants and reference validation |
| Delay | Grid Delay with filtered repeats; 6.9 post-chain Stereo Delay with cross-feedback, modulation, ducking and tempo | Further algorithms, insertable loop chain and listening validation |
| Reverb | Legacy basic/room blocks plus 6.10 post-chain stereo eight-line FDN with diffusion, decay shelves, early reflections, modulation and ducking | Further algorithms, band-specific decay characterization, musical listening and spatial validation |
| Compressor | Original feed-forward peak compressor in Grid mode (6.5) | Additional topologies, metering, sidechain/stereo linking and reference validation |
| Pitch | Absent | Pitch tracking/shifting and latency handling |
| Modulation | Original tremolo, chorus, flanger, six-stage phaser and vibrato | Additional algorithms, stereo modulation designs and reference validation |
| Morph | Original bit-crusher with quantization/sample hold and a ring modulator | Other morph algorithms and reference validation |
| Filter and wah | Original envelope/manual-position wah, dedicated cut-filter block and serial high-pass/cab low-pass | Other filter/wah designs, pedal/MIDI mapping and reference validation |
| EQ | Serial three-band EQ, legacy fixed grid filter and new three-band Tone EQ with adjustable mid frequency/Q | Additional EQ types and reference validation |
| IR loader | User cabinet IR support | Additional channel/dual-loader modes and restart handling |
| Looper | Absent | Recording, playback, transport and memory handling |
| Synth | Absent | Synthesis and tracking |
| Utility | Gate, global bypass, output trim and per-slot gain/polarity | Additional utility processing and routing |
| Neural Captures V1/V2 | Not supported | No compatible capture engine or supplied model assets; VCMLP1 is a different format |
| Plugin devices | Not included or hosted | Official page requires corresponding plugin licenses; no bundled licensed plugin DSP |

## Four-row routing

All four rows are visible in 6.4. In Grid mode, each enabled row receives the same channel input; enabled outputs are averaged. Disabled rows contribute nothing. With every row disabled, dry input passes through. This is fixed parallel routing, not arbitrary split/merge graphs or independent hardware bus routing. Serial mode remains available. Row enable and Grid mode are separate controls.

## Required evidence before claiming a device is included

A selectable device needs its own documented processing implementation or supported supplied model, meaningful parameters, saved state, deterministic audio tests, and a validated Windows binary. Sound-matching claims additionally require reference recordings/measurements and comparisons. Labels, generic presets and GUI similarity do not count as device implementations.

The public reference provides names and device/version metadata; this project has not received the corresponding Neural DSP algorithms, capture assets or licensed plugin implementation. Original equivalents can be developed, but exact parity cannot be certified from the catalog alone.

## 6.6 implementation evidence

The verified Windows build is https://github.com/mikey423a/VirtualCortex/actions/runs/41426495067. All 24 CTests, pluginval level 5, standalone startup and installer installation/uninstallation checks passed. Native tests cover saved-state recall and C++ allocation checks for the new effects. Portable tests verify delay timing, phaser magnitude, wah bandpass response and bit-crusher quantization/sample hold at several sample rates.

The application now has 15 processing block types, not 15 branded device models. The five new blocks are original chorus, flanger, phaser, wah and bit-crusher. The modulation paths currently process each channel independently with synchronized LFO phase; they are not stereo-spread emulations. The wah supports envelope-driven sweep and manual position; expression/MIDI control is not implemented. No full Quad Cortex catalog or sound-match claim is made.

## 6.7 implementation evidence

Verified build: https://github.com/mikey423a/VirtualCortex/actions/runs/41430856615. All 25 CTests, pluginval level 5, standalone startup and installer install/uninstall checks passed. The catalog now has 21 processing block types. New blocks are vibrato, ring modulation, per-slot gain/polarity, high/low-cut filters, Tone EQ and Room Reverb. These are original algorithms, not factory models or certified studio-quality effects.

The current Delay already feeds delayed samples back into its buffer through its feedback control. A separately editable effects chain inside that repeat path is not implemented. See STUDIO-EFFECTS-PLAN.md for the quality-first stereo redesign and Cortex-style feedback editor requirement.

## 6.8 repeat-path milestone

Verified build: https://github.com/mikey423a/VirtualCortex/actions/runs/41434583472. All 26 CTests, pluginval level 5, standalone startup and installer checks passed. Repeat FX adds high-pass, low-pass and filter-blend controls inside Grid Delay regeneration while preserving the dry input and first repeat. Presets without the new flag use the legacy delay path. An arbitrary insert chain and the researched stereo studio-effects redesign remain unimplemented. Software validation does not establish studio-quality sound.

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
