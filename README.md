# Virtual Cortex 6.5

Windows x64 standalone guitar amplifier/effects application and VST3, built with JUCE 8.0.10 and ASIO enabled.

## Verified build

[Successful 6.5 build and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/41419026978)

- EXE and VST3 compiled using MSVC on Windows Server 2022.
- All 23 CTest checks passed, including the real JUCE processor, editor renders, four-row routing and the new effects.
- VST3 passed pluginval 1.0.4 at strictness level 5.
- Standalone stayed running for the 10-second startup test.
- Inno Setup installer passed install/uninstall and installed-binary checks.
- Windows 11 interactive use and Anubis ASIO input/output still need user testing. Installer is unsigned.

Download the VirtualCortex-Windows-x64 artifact for Setup EXE, standalone EXE, VST3 ZIP, hashes and installer logs. Native screenshots and test logs are in VirtualCortex-build-logs.

## Four-row interface

All four signal rows are visible together. The bottom parameter editor and Gig view start hidden; SHOW EDITORS opens them while retaining a compact four-row view. The design canvas scales proportionally.

In Grid mode, each enabled row receives the same input and enabled outputs are averaged. Row buttons enable/disable individual lanes. Empty enabled rows contribute dry input; when all rows are disabled, dry input passes through. This is fixed parallel routing, not an arbitrary split/merge graph or independent hardware buses. Enable GRID ENGINE to hear the grid; the existing serial engine remains available.

## New original effects in 6.5

- Compressor: feed-forward peak detection with threshold, ratio, attack, release, makeup and parallel mix.
- Overdrive: soft clipping with drive, tone, level and mix.
- Distortion: hard clipping with the same controls.
- Drive blocks use 2x interpolation, tone filtering and DC removal; this does not imply alias-free processing or a measured analog circuit model.
- Choose a new category in the left rail and a browser entry to insert the effect. Selecting a new effect from serial mode enables Grid mode. Parameters are stored per slot and recalled with presets.
- Tests cover compressor response, drive algorithm distinction, DC removal, dry mix, rates, preset recall and zero C++ allocations during tested audio callback automation.

These are original algorithms, not Neural DSP device models. The full website device list is not implemented. See [DEVICE-COVERAGE.md](DEVICE-COVERAGE.md) for the remaining gaps.

## Existing functionality

Original amp, gate, cabinet IR loading, EQ, delay, reverb, tremolo and VCMLP1 model loading remain. Plugin IDs and all 28 existing host parameter IDs are unchanged. Old presets remain readable; presets using new block types require 6.5 or later.

The top A-H buttons select slots. Click a grid slot to select it, then choose a browser entry to insert a device. With editors shown, Gig tiles toggle block bypass; numeric readouts can be edited by double-clicking. The parameter header menu removes or bypasses the selected block. The cabinet and amp browsers expose IR and VCMLP1 loading. Grid gate/EQ still have fixed settings.

The project does not bundle the Quad Cortex factory models, Neural Captures, licensed Neural DSP plugin devices, or NAM support. Visual similarity is not evidence of sonic equivalence.

Source: VirtualCortex/. Workflow: .github/workflows/virtualcortex-windows.yml.
