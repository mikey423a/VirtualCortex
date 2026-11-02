# Device coverage and completion criteria

Reference: https://neuraldsp.com/device-list (reviewed 2026-11-02).

## Current status

Virtual Cortex is an original, limited effects application. It does not include the complete Quad Cortex library and has not been validated for sonic equivalence to any listed device. A successful binary build verifies the software checks, not device-list parity.

| Reference area | Existing project capability | Remaining work |
| --- | --- | --- |
| Guitar and bass amps | Generic amp and custom VCMLP1 loader | Individual models, controls, reference measurements and validation; no dedicated bass library |
| Guitar and bass cabinets | Basic filter and user IR loading | Measured cabinet/microphone library and multi-mic controls |
| Guitar and bass overdrive | Original soft-clipping overdrive and hard-clipping distortion in Grid mode (6.5) | Individual pedal models, dedicated bass variants and reference validation |
| Delay | Basic variable delay | Distinct algorithms and stereo behavior |
| Reverb | Basic reverb plus original room delay network | Distinct algorithms and spatial validation |
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
