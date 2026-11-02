# Virtual Cortex 6.2

Windows x64 standalone guitar amplifier/effects application and VST3, built with JUCE 8.0.10 and ASIO enabled.

## Current verified build

[Successful 6.2 build and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/41376751062)

The editor follows the supplied desktop reference's 1184x864 layout: narrow icon rail, device list, two visible grid rows, parameter tiles and a two-row Gig view. Panel, title strip and selection colours were sampled from the reference. The whole canvas scales with its original proportions to fit smaller displays.

The interface retains Virtual Cortex branding and names for its actual supported effects. The DSP algorithms, plugin IDs and all 28 parameter IDs are retained. This update does not add Quad Cortex's proprietary amp library, capture workflow, hardware phantom power or ground lift.

- EXE and VST3 compiled with MSVC on the Windows Server 2022 runner.
- All 21 CTest checks passed, including native editor renders, initial value formatting, editable readout binding, block insertion, bypass and the second grid bank.
- VST3 passed pluginval 1.0.4 at strictness level 5.
- Standalone passed the 10-second startup test.
- Inno Setup installer passed install/uninstall checks.
- Anubis ASIO input/output and Windows 11 interactive audio still require user testing. Installer is unsigned.

Download the VirtualCortex-Windows-x64 artifact for Setup EXE, standalone EXE, VST3 ZIP, hashes and installer logs. Native PNG renders are included in VirtualCortex-build-logs.

## Controls

- I / II select the two pairs of grid rows; the Gig view row dropdown selects any of the four rows.
- The icon rail selects a device category. In Grid mode, a device-list entry inserts that effect into the selected slot. Entries also provide original parameter presets, IR loading or VCMLP1 model loading as applicable.
- The top A-H buttons select slots; the Gig view tiles toggle the selected row's block bypass states.
- STOMP or the header pedal button enables the Grid engine. SERIAL / GRID status is shown at the bottom. The default serial engine is preserved.
- Double-click a numeric readout to edit it. The ellipsis in the parameter header opens block removal/bypass options.
- The cabinet and amp panels expose the existing IR and VCMLP1 loading controls. Some grid blocks have fixed settings in the existing engine; output trim applies to the serial engine.

Source: VirtualCortex/. Workflow: .github/workflows/virtualcortex-windows.yml. This page supersedes the historical nested setup README.
