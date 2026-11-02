# Virtual Cortex 6.6

Windows x64 standalone guitar amplifier/effects application and VST3, built with JUCE 8.0.10 and ASIO enabled.

## Verified build

[Successful 6.6 build and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/41426495067)

- EXE and VST3 compiled using MSVC on Windows Server 2022.
- All 24 CTest checks passed, including the real JUCE processor, editor renders, four-row routing and the new effects.
- VST3 passed pluginval 1.0.4 at strictness level 5.
- Standalone stayed running for the 10-second startup test.
- Inno Setup installer passed install/uninstall and installed-binary checks.
- Windows 11 interactive use and Anubis ASIO input/output still need user testing. Installer is unsigned.

Download the VirtualCortex-Windows-x64 artifact for Setup EXE, standalone EXE, VST3 ZIP, hashes and installer logs. Native screenshots and test logs are in VirtualCortex-build-logs.

## Four-row interface

All four signal rows are visible together. The bottom parameter editor and Gig view start hidden; SHOW EDITORS opens them while retaining a compact four-row view. The design canvas scales proportionally.

In Grid mode, each enabled row receives the same input and enabled outputs are averaged. Row buttons enable/disable individual lanes. Empty enabled rows contribute dry input; when all rows are disabled, dry input passes through. This is fixed parallel routing, not an arbitrary split/merge graph or independent hardware buses. Enable GRID ENGINE to hear the grid; the existing serial engine remains available.

## New in 6.6

Five original Grid effects are now available, bringing the application to 15 processing block types:

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
