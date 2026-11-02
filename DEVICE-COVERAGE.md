# Device coverage and completion criteria

Reference: https://neuraldsp.com/device-list (reviewed 2026-11-02).

## Current status

Virtual Cortex is an original, limited effects application. It does not include the complete Quad Cortex library and has not been validated for sonic equivalence to any listed device. A successful binary build verifies the software checks, not device-list parity.

| Reference area | Existing project capability | Remaining work |
| --- | --- | --- |
| Guitar and bass amps | Generic amp and custom VCMLP1 loader | Individual models, controls, reference measurements and validation; no dedicated bass library |
| Guitar and bass cabinets | Basic filter and user IR loading | Measured cabinet/microphone library and multi-mic controls |
| Guitar and bass overdrive | No dedicated pedal blocks | Original pedal DSP and validation |
| Delay | Basic variable delay | Distinct algorithms and stereo behavior |
| Reverb | Basic reverb | Distinct algorithms and spatial validation |
| Compressor | Absent | Dynamics algorithms and metering |
| Pitch | Absent | Pitch tracking/shifting and latency handling |
| Modulation | Tremolo only | Additional algorithms |
| Morph | Absent | Additional algorithms |
| Filter and wah | Basic serial high-pass/cab low-pass only | Dedicated filter/wah blocks and control mapping |
| EQ | Serial three-band EQ; fixed grid filter | Configurable grid EQ and additional EQ types |
| IR loader | User cabinet IR support | Additional channel/dual-loader modes and restart handling |
| Looper | Absent | Recording, playback, transport and memory handling |
| Synth | Absent | Synthesis and tracking |
| Utility | Gate, global bypass and output trim | Additional utility processing and routing |
| Neural Captures V1/V2 | Not supported | No compatible capture engine or supplied model assets; VCMLP1 is a different format |
| Plugin devices | Not included or hosted | Official page requires corresponding plugin licenses; no bundled licensed plugin DSP |

## Four-row routing

All four rows are visible in 6.4. In Grid mode, each enabled row receives the same channel input; enabled outputs are averaged. Disabled rows contribute nothing. With every row disabled, dry input passes through. This is fixed parallel routing, not arbitrary split/merge graphs or independent hardware bus routing. Serial mode remains available. Row enable and Grid mode are separate controls.

## Required evidence before claiming a device is included

A selectable device needs its own documented processing implementation or supported supplied model, meaningful parameters, saved state, deterministic audio tests, and a validated Windows binary. Sound-matching claims additionally require reference recordings/measurements and comparisons. Labels, generic presets and GUI similarity do not count as device implementations.

The public reference provides names and device/version metadata; this project has not received the corresponding Neural DSP algorithms, capture assets or licensed plugin implementation. Original equivalents can be developed, but exact parity cannot be certified from the catalog alone.
