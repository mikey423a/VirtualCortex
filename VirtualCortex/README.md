# Virtual Cortex Windows builds

Current verified milestone **6.52.0**, code `0664ea9028189d449caf99569cfd3484dafa9e69`. See the [verification notes](../validation/GRID-6.52.md) and [Windows CI and downloads](https://github.com/mikey423a/VirtualCortex/actions/runs/44548617609).

The existing workflow builds standalone EXE/VST3 with MSVC on Windows Server 2022, JUCE 8.0.10 and ASIO enabled. Regression tests, pluginval strictness 5, standalone startup, installation and uninstallation passed. Anubis hardware and Windows 11 interactive testing remain user-side. The installer is unsigned.

Current fullscreen uses the full display width at 2560x1440; controls preserve rounded-square blocks and circular knobs. F11 toggles standalone fullscreen and Esc restores windowed mode. See current routing notes for limitations. The historical notes below describe earlier versions and do not supersede the current status.

## 6.12.0 display and fullscreen update

The standalone app has a FULL SCREEN button in the header. F11 toggles fullscreen; Esc exits it. The previous window bounds are restored on exit. Fullscreen controls are limited to the standalone application so a VST3 cannot take over a DAW window.

Window resizing now allows up to 7680 × 4320 instead of the previous 1776 × 1296 cap. The editor scales proportionally and centers its existing layout, preserving circular knobs and rounded-square grid blocks. On a widescreen display, black side margins preserve the original proportions. Four parallel rows remain visible, and bottom editors start hidden. Native UI checks cover 2560 × 1440 and 1707 × 960 logical pixels (approximately a 1440p monitor at 150% Windows scaling), plus repeated fullscreen entry/exit and bounds restoration. Actual mixed-DPI monitor transitions and Anubis hardware operation still require user testing.


## 6.13 catalog browser

CATALOG opens the full reference inventory from the official device-list snapshot retrieved November 2, 2026: 689 rows across 23 source sections, comprising 632 released-list rows and 57 announced rows. Each plugin-specific duplicate remains a separate entry, identified by its required plugin. Search covers the device name, based-on gear, previous names, category, section and required plugin. Section and availability filters can be combined.

All 689 official entries are explicitly reference-only: no Neural DSP DSP algorithm, model weight, factory capture or cabinet IR is included. Their insertion action is disabled. These are device-list rows, not a count of individual capture variations or playable Virtual Cortex models.

A separate group contains the 23 actual Virtual Cortex originals (21 Grid types plus two stereo post-chain processors). Only these can add a block or open the corresponding post-chain controls. Original processors are not certified matches for the referenced devices. Opening and searching the inventory does not change audio or presets; adding an original requires an explicit action. The existing directory, presets, four-row GUI, fullscreen controls and DSP are retained.

The factual source snapshot is in data/device-catalog.json. The update/import and deterministic header-generation scripts are in tools. The snapshot date is displayed in the panel; there is no automatic network lookup inside the app.
