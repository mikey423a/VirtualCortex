# Virtual Cortex 6.1

Windows x64 standalone guitar amplifier/effects application and VST3 built with JUCE 8.0.10 and ASIO enabled.

## Current verified build

[Successful 6.1 Windows build and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/41373888288)

The editor now uses a dark device browser, a four-row icon grid, selected-effect controls, and A-H slot bypass buttons. It resizes from 900x560 to 1800x1100 and chooses its initial size from the display work area. Numeric controls display readable units immediately after launch. Existing DSP algorithms and all 28 parameter IDs were retained.

- Standalone EXE and VST3 compiled with MSVC on Windows Server 2022.
- All 21 CTest checks passed, including native editor screenshots at three sizes, initial value formatting, drive binding, slot insertion, and bypass.
- VST3 passed pluginval 1.0.4 at strictness level 5.
- Standalone remained running for the 10-second startup test.
- Inno Setup installer compiled and passed silent install/uninstall checks.
- Windows 11 interactive audio and Merging Anubis ASIO input/output still require user testing. Installer is unsigned.

The VirtualCortex-Windows-x64 artifact contains the Setup EXE, standalone EXE, VST3 ZIP, hashes, and installer logs. The VirtualCortex-build-logs artifact includes the actual native editor PNGs and test logs.

## Using the editor

The default serial engine is preserved. The sidebar selects which serial effect's controls are shown. Enable GRID ENGINE to hear the editable parallel grid. Click a slot, select a device type in the browser, and use ADD TO SLOT. A-H toggle block bypass in the selected row. The input's four enabled rows share the same audio input and mix to the main output.

Load cabinet IRs or VCMLP1 models with the selected cabinet/amp controls. Some grid effects have fixed settings in the existing DSP. Output trim currently applies to the serial engine. This update does not reproduce Quad Cortex firmware, its amp library, or hardware capture/phantom-power/ground-lift controls.

Source is in VirtualCortex/. The Windows workflow is .github/workflows/virtualcortex-windows.yml. The nested README describes the earlier setup stage; this page is the current build status.
