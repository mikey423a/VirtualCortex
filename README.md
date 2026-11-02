# Virtual Cortex 6.13

Windows x64 standalone guitar amplifier/effects application and VST3, built with JUCE 8.0.10 and ASIO enabled.

## Verified build

[6.13 Windows build and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/41528022183)

- EXE and VST3 compiled using MSVC on Windows Server 2022.
- All 32 CTest checks passed, including the real JUCE processor, editor renders, four-row routing and the new effects.
- VST3 passed pluginval 1.0.4 at strictness level 5.
- Standalone stayed running for the 10-second startup test.
- Inno Setup installer passed install/uninstall and installed-binary checks.
- Windows 11 interactive use and Anubis ASIO input/output still need user testing. Installer is unsigned.

Download the VirtualCortex-Windows-x64 artifact for Setup EXE, standalone EXE, VST3 ZIP, hashes and installer logs. Native screenshots and test logs are in VirtualCortex-build-logs.

## Catalog inventory in 6.13

CATALOG opens the complete factual inventory from the official [device list](https://neuraldsp.com/device-list), retrieved November 2, 2026: **689 source rows in 23 sections**, including 632 released-list rows and 57 announced rows. Search supports device names, based-on gear, aliases and required plugins; trademark marks are optional in search and words may be entered in any order. Section and availability filters combine.

**This completes the reference inventory, not the playable Neural DSP device catalog.** All 689 official entries are reference-only, with insertion disabled. The app's playable catalog remains 23 original processors: 21 Grid block types plus the Stereo Delay and Stereo Reverb post-chain processors. Factory models, captures, IRs and proprietary algorithms are not bundled. Capture-family rows are not counts of individual capture variations.

The browser retains based-on gear, CorOS versions, previous names, replacement information and required plugins. Same-named plugin devices retain separate identities. Working originals have an explicit insert/open action. The existing preset directory, four-row GUI, 1440p/fullscreen support and DSP remain available. [Validation scope](validation/CATALOG-6.13.md).

## Experimental NAM backend

An optional offline NAM renderer now compiles on Windows with NeuralAmpModelerCore v0.5.4. [The build](https://github.com/mikey423a/VirtualCortex/actions/runs/41522572979) passed all 31 CTest checks, including a numerical Linear-model oracle and upstream LSTM/WaveNet rendering, plus pluginval level 5, standalone startup and installer checks.

This is a separate command-line prototype, not live NAM playback in the GUI. The app remains 6.12.0 with its existing VCMLP1 loader. No Neural DSP algorithms, factory captures or new named device models are included. See [research findings](REUSE-RESEARCH.md), [tool usage](VirtualCortex/NAM-BACKEND.md) and [verification scope](validation/NAM-BACKEND.md).

## New in 6.12: fullscreen and larger displays

The standalone header has a FULL SCREEN button. F11 toggles fullscreen and Esc exits; the previous window position and size are restored. The VST3 leaves window management to the DAW. The old 1776 × 1296 editor limit has been raised to 7680 × 4320, and windows can use different aspect ratios. The existing canvas scales proportionally and remains centered, with black side margins on widescreen displays.

Native editor checks cover 2560 × 1440 pixels and 1707 × 960 logical pixels (approximately a 1440p display at 150% Windows scaling), both with the editors open and hidden. They also exercise repeated fullscreen entry/exit, the fullscreen button, F11/Esc handlers and windowed bounds restoration in a native JUCE window. Mixed-DPI monitor transitions and Windows 11 interactive hardware testing remain pending.

The 6.11.1 reverb coefficient cache is retained: unchanged coefficient inputs reuse their calculated values, with preparation invalidating the cache. Audio output matched the previous implementation exactly in the tested 512,000-frame automated render; this is an optimization, not a new reverb model.

## Four-row interface

All four signal rows are visible together. The bottom parameter editor and Gig view start hidden; SHOW EDITORS opens them while retaining a compact four-row view. The design canvas scales proportionally.

In Grid mode, each enabled row receives the same input and enabled outputs are averaged. Row buttons enable/disable individual lanes. Empty enabled rows contribute dry input; when all rows are disabled, dry input passes through. This is fixed parallel routing, not an arbitrary split/merge graph or independent hardware buses. Enable GRID ENGINE to hear the grid; the existing serial engine remains available.

## Sound-quality priority

The current effects are original functional implementations. Passing the software checks does not establish studio-quality sound or parity with commercial effects. Development now prioritizes stereo reverb/delay quality, deeper controls and audio evaluation ahead of catalog count. See [STUDIO-EFFECTS-PLAN.md](STUDIO-EFFECTS-PLAN.md) for the researched architecture, repeat-feedback path and Cortex-style advanced editor requirements. The stereo delay arrived in 6.9; an original eight-line stereo FDN reverb is included in 6.10. Further quality gates and convolution remain planned.

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

## 6.10 stereo FDN reverb milestone

Verified Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41447139700. Source 5cb1525a24e40bd73b6547367e86d1aaf03cda56. The EXE/VST3 compiled on Windows Server 2022; all 28 CTests, pluginval strictness 5, standalone startup and installer install/uninstall checks passed.

Stereo Reverb is a new optional post-chain master module, following Stereo Delay and preceding global output trim. It is not a Grid insert and does not replace the old reverb blocks. Select Stereo Reverb from the category dropdown, choose a starting preset and use SHOW EDITORS. Space / Tone-Mod / Output-Dynamics pages expose 18 parameters: nominal decay (0.2–20 s), size (0.5–2), predelay (0–500 ms), diffusion, early/late balance, mix, low decay ratio (0.25–2), high decay ratio (0.1–1), low/high crossover frequencies, modulation rate/depth, width, wet ducking, duck release, wet level and input high/low cuts. Duck attack is fixed at 5 ms. Size/predelay automation uses smoothed moving reads, so changes can shift pitch. Power fades the wet mix down rather than providing a spillover/freeze mode.

The original algorithm uses eight unequal feedback delays, normalized orthogonal Hadamard mixing, four input allpasses per channel, separate early taps, linear fractional reads and two frequency-dependent feedback shelves. Linear interpolation colors the high-frequency decay; the nominal decay and ratios are design targets rather than exact measured band RT60 guarantees. Filters have bounded gain at fixed settings; buffer guards remain for malformed/extreme inputs. This is not a factory model or a copied proprietary algorithm.

There are now 69 host parameters: all previous 50 IDs and their order are retained, with 18 reverb controls and one enable appended. Old presets default the new master off. Portable tests verify broadband decay, stereo decorrelation, zero wet width, early timing, damping, ducking, extreme automation, malformed controls, bypass fade and prepare/reset. A nominal 1.2 s unmodulated broadband tail measured approximately 1.10–1.13 s at 44.1/48/96/192 kHz using a -5 to -25 dB Schroeder slope. These measurements cover the tested configuration only. Native tests verify state migration, allocation-free tested automation, output mute and all editor pages.

Further work includes octave-band characterization across the control range, aliasing/discontinuity/CPU measurements, level-matched musical listening, convolution/hybrid modes and additional genuine device implementations. This release does not certify studio-quality sound, full Neural DSP catalog parity, Windows 11 audio behavior or Anubis hardware I/O. Installer unsigned.

## 6.10.1 grid appearance fix

Rounded-square grid blocks now retain their shape in both compact and expanded views. Corner radius and outline thickness scale with the block dimensions instead of keeping a fixed radius that became circular at small sizes. DSP, parameter IDs and preset format are unchanged. Verified Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41465770775. All 28 CTests, VST3 pluginval level 5, standalone startup and installer install/uninstall checks passed; native compact and expanded screenshots were inspected. Windows 11 audio behavior and Anubis hardware I/O still require testing. Installer unsigned.

## 6.11 pre-delay transition and mix precision

Verified Windows build: https://github.com/mikey423a/VirtualCortex/actions/runs/41478258299. Source 345b067db573a24b07e781835f0e07def11b7fbb. All 28 CTests, pluginval level 5, standalone startup and installer install/uninstall checks passed.

Stereo Reverb adds PRE GLIDE in the existing editor. Off selects a 20 ms linear crossfade between two fixed pre-delay read positions, including the early-reflection taps and the feed into the late tank. Rapid changes queue the latest position after the current fade; switching to glide during a fade completes that fade first. This avoids sweeping the pre-delay read position in clean mode, but a transition can still cause temporary phase cancellation/comb filtering. Size and tank modulation continue to use moving reads. On retains the prior pitch-changing pre-delay glide.

Existing presets without srPreGlide default it on, preserving their pre-delay behavior. Newly chosen reverb browser presets select clean mode. There are now 70 host parameters; the previous 69 IDs and their order are unchanged, with the new mode appended. Tested mode changes and pre-delay automation allocate no C++ objects in the audio callback. Native tests cover UI binding, saved mode and migration defaults. Portable tests compare real output to the expected two-fixed-head blend at 44.1/48/96/192 kHz, including a mid-fade mode change.

Stereo Delay smoothing now converges exactly to its endpoints: a fully wet long delay does not retain a trace of current dry input, and zero mix reaches exact dry output. Both endpoints are tested at all four rates. Rounded-square grid blocks from 6.10.1 remain. These implementation and software checks do not certify studio-quality listening results, full device-catalog parity, Windows 11 audio behavior or Anubis hardware I/O. Installer unsigned.
