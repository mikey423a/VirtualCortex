# Virtual Cortex 6.9

Windows x64 standalone guitar amplifier/effects application and VST3, built with JUCE 8.0.10 and ASIO enabled.

## Verified build

[Successful 6.9 build and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/41438893229)

- EXE and VST3 compiled using MSVC on Windows Server 2022.
- All 27 CTest checks passed, including the real JUCE processor, editor renders, four-row routing and the new effects.
- VST3 passed pluginval 1.0.4 at strictness level 5.
- Standalone stayed running for the 10-second startup test.
- Inno Setup installer passed install/uninstall and installed-binary checks.
- Windows 11 interactive use and Anubis ASIO input/output still need user testing. Installer is unsigned.

Download the VirtualCortex-Windows-x64 artifact for Setup EXE, standalone EXE, VST3 ZIP, hashes and installer logs. Native screenshots and test logs are in VirtualCortex-build-logs.

## Four-row interface

All four signal rows are visible together. The bottom parameter editor and Gig view start hidden; SHOW EDITORS opens them while retaining a compact four-row view. The design canvas scales proportionally.

In Grid mode, each enabled row receives the same input and enabled outputs are averaged. Row buttons enable/disable individual lanes. Empty enabled rows contribute dry input; when all rows are disabled, dry input passes through. This is fixed parallel routing, not an arbitrary split/merge graph or independent hardware buses. Enable GRID ENGINE to hear the grid; the existing serial engine remains available.

## Sound-quality priority

The current effects are original functional implementations. Passing the software checks does not establish studio-quality sound or parity with commercial effects. Development now prioritizes stereo reverb/delay quality, deeper controls and audio evaluation ahead of catalog count. See [STUDIO-EFFECTS-PLAN.md](STUDIO-EFFECTS-PLAN.md) for the researched architecture, repeat-feedback path and Cortex-style advanced editor requirements. The full stereo/FDN redesign remains planned. The first repeat-filter step is included in 6.8.

## New in 6.8: editable repeat return

Select a Grid Delay block, show its editor, and enable REPEAT FX.

- TIME, FEEDBACK and MIX remain available.
- LOOP HIGH PASS, LOOP LOW PASS and LOOP FILTER MIX control processing inside the regeneration path.
- Dry input and the first repeat remain unfiltered; subsequent repeats pass through the filters each time they regenerate.
- The optional path uses third-order Lagrange reading with smoothed time, cutoff, feedback and mix changes.
- Repeat FX is saved per slot. A preset without the new property loads with Repeat FX disabled, preserving the legacy delay path.
- Controls use the existing Cortex-style rotary tiles and toggle; there is no separate plugin skin.

This is a filtered feedback path, not an arbitrary effect-block insert chain, stereo cross-feedback matrix, or tape/BBD emulation. It applies to Grid Delay; serial Delay is unchanged. The existing Grid time range remains 20–500 ms. Repeat FX is not a completed studio-quality redesign.

Tests verify first-repeat behavior, filtered regeneration, loop blend, dry input, timing/cutoff automation, several sample rates, state recall, legacy state and tested allocation-free callbacks. Native UI tests check the checkbox and loop controls.

## Additions from 6.7

Six original blocks bring the catalog to 21 processing block types:

- Vibrato: fully wet fractional-delay modulation, with rate, depth, base delay and mix.
- Ring modulator: a 10–2000 Hz sine carrier with depth and mix.
- Gain: per-slot gain from -60 to +12 dB, smoothed transitions and polarity inversion.
- Low / High Cut: separate second-order high-pass and low-pass filters with mix.
- Tone EQ: low/high shelves and an adjustable mid peak, with gain, mid frequency, mid Q and mix.
- Room Reverb: a parallel-comb/allpass delay network with nominal decay, damping, pre-delay, diffusion and mix.

The browser now has a category dropdown for all 21 block types plus global Output. Existing sidebar shortcuts remain. All four routing rows are retained, and the bottom editors start hidden.

The preset reader supports both legacy grids and the expanded V2 encoding. Grids containing only older types retain their legacy encoding when saved. Presets using the six new types require 6.7 or later. The existing preset container keys, plugin IDs and all 28 host parameter IDs remain unchanged.

Tests cover old/new state formats, gain and polarity, filter response, EQ gain, ring-modulation sidebands, room decay, several sample rates, native category selection and state recall. New reverb buffers are allocated during preparation while processing is stopped.

The existing algorithms remain for compatibility. Room Reverb is not a replacement for the old Reverb block. All new algorithms are original and are not certified matches to Neural DSP or branded devices. Modulation and room processing have independent per-channel state, without a stereo-spread design.

## Additions from 6.6

Five original Grid effects were added in 6.6:

- Chorus and flanger: fractional delay modulation with rate, depth, base delay, feedback and mix.
- Phaser: six allpass stages with rate, depth, center frequency, feedback and mix.
- Wah: envelope-following bandpass filter with sensitivity, manual position, resonance, attack, release and mix. Set sensitivity to zero for a stationary/manual sweep.
- Bit-crusher: 4–16-bit symmetric quantization, 1–64 sample hold, level and mix.
- Browser entries select starting settings; parameters remain editable and are saved per slot.
- Selecting a new Grid-only effect from serial mode enables Grid mode.
- Portable audio tests check delay timing, phaser magnitude, wah response, quantization/hold, dry mix and rate handling. Native processor tests exercise state recall and allocation-free processing during tested automation. The browser selection path is also exercised by the real editor test.

These are original basic algorithms, not measured branded-device emulations. Modulation currently uses independent channel state with synchronized LFO phase; no stereo-spread or expression-pedal/MIDI support is claimed. Individual factory amps, cabinets, Neural Captures and licensed plugin devices are not bundled. The full website catalog remains incomplete; see [DEVICE-COVERAGE.md](DEVICE-COVERAGE.md).

## Existing original effects

The compressor, soft-clipping overdrive and hard-clipping distortion added in 6.5 remain, along with amp, gate, cabinet IR loading, EQ, delay, reverb, tremolo and VCMLP1 model loading. All 28 existing host parameter IDs and plugin IDs are unchanged. Older presets remain readable. Presets using the five new block types require 6.6 or later.

## Controls and limits

Choose a row/slot, then select a category in the left rail and an entry in the browser. Row buttons enable individual parallel lanes. SHOW EDITORS opens the parameter and Gig panels while keeping all four rows visible. Gig tiles toggle block bypass; double-click numeric readouts to edit values. The parameter header menu removes or bypasses the selected block. Cabinet and amp browsers expose IR and VCMLP1 loading.

Grid gate/EQ retain fixed settings. The project has no NAM or Quad Cortex capture-format compatibility. Visual similarity does not establish sonic equivalence.

Source: VirtualCortex/. Workflow: .github/workflows/virtualcortex-windows.yml.

## 6.9 stereo delay milestone

Verified Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41438893229 (source c9632c4f16addf3507606bc251df3d6c663626be). All 27 CTests, pluginval level 5, standalone startup and installer install/uninstall checks passed. Editor screenshots were inspected across all three pages and the four-row view.

A new optional Stereo Delay runs after the main serial/grid chain and before output trim. It provides independent L/R times up to 16 seconds, feedback/cross-feedback, ping-pong, repeat-loop high/low-pass filtering and blend, modulation rate/depth, wet ducking with editable release, width, wet level, time glide, and host/manual tempo with numeric beat multipliers. Its 18 continuous controls and four switches occupy Main, Tone/Mod and Dynamics/Tempo pages in the existing editor style. Tape Glide describes pitch-changing time transitions, not a tape circuit emulation. Duck attack is fixed at 5 ms. Manual times and cross-feedback are disabled when overridden by sync and ping-pong respectively.

Original 28 host parameter IDs retain their order; 22 new parameters are appended (50 total). Older presets default the master effect off. Tests cover stereo isolation, ping-pong timing, ducking, fractional interpolation, malformed controls, automation allocation checks, tempo, recall and global output mute. This is one post-chain master module, not an insertable Grid block or arbitrary feedback-effects chain. The 21 existing Grid processing types remain. FDN reverb, convolution room modes and the full factory catalog remain unfinished. Studio-quality listening evaluation, reference-device matching, Windows 11 audio behavior and Anubis hardware I/O are not certified by these tests. Installer unsigned.
