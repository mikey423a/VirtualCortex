# Virtual Cortex 6.0

Windows x64 standalone guitar amplifier/effects application and VST3, built with JUCE 8.0.10 and ASIO enabled.

## Verified build

[Successful Windows build and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/41371067494)

- Standalone EXE and VST3 compiled using MSVC on the Windows Server 2022 runner.
- All 20 CTest regression tests passed.
- VST3 passed pluginval 1.0.4 at strictness level 5.
- Standalone remained running for the 10-second startup test.
- Inno Setup installer compiled; silent installation deployed the EXE and VST3, and uninstallation removed them.
- Installer is unsigned. Windows 11 interactive behavior and Merging Anubis ASIO input/output still require user testing.

Download the VirtualCortex-Windows-x64 artifact from that run. It includes the Setup EXE, standalone EXE, VST3 bundle ZIP, SHA256 hashes and installer test logs. The installer installs both standalone and VST3.

Source is in VirtualCortex/. The workflow is .github/workflows/virtualcortex-windows.yml.
The setup notes in the nested source README predate the successful build; this page records the current verification status.
