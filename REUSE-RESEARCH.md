# Reusable engine and Neural DSP interoperability research

Checked November 2, 2026. These are repository authors' documented capabilities, not hardware tests performed by Virtual Cortex.

| Project | What is available | Application to Virtual Cortex |
|---|---|---|
| [qc-mcp](https://github.com/lexasoft123/qc-mcp) | MIT source for reverse-engineered Cortex Control USB-HID/protobuf communication, hardware preset/scene/block editing | Candidate for future hardware control/interoperability. It requires a Cortex and supplies no amp/effect DSP engine. |
| [cortex](https://github.com/pacharanero/cortex) | AGPL-3.0 cross-platform device editor/CLI/Rust transport; pre-alpha status and platform limitations documented | Protocol/UI research. Not a replacement for the hardware's audio engine; no code copied into this build. |
| [pyquadcortex](https://github.com/stokes-audio/pyquadcortex) | USB hardware client and preset/control operations; authors document measured firmware/platform coverage | Future hardware interop, not computer-side amp emulation. License/integration review still needed before reuse. |
| [dsp-decompiled](https://github.com/anmolsekhon590/dsp-decompiled) | Structural study of the Plini X installer and high-level binary observations | README explicitly excludes DSP source and model weights. Not a usable Plini amp/effects implementation. No code or assets imported. |
| [ndsp_preset_tool](https://github.com/headdirt/ndsp_preset_tool) | GPL-3.0-or-later parser/editor for Neural DSP preset serializations | Useful format research; preset values do not reproduce the underlying algorithms. No code imported. |
| [toneparse](https://github.com/vian21/toneparse) | Neural DSP preset parsing/analysis | Another format reference; license and compatibility review needed. No code imported. |
| [NeuralAmpModelerCore](https://github.com/sdatkinson/NeuralAmpModelerCore) | MIT C++ model inference engine and example models | Selected for an experimental offline rendering backend. Real NAM models, not Neural DSP factory captures. |

No complete, reusable Quad Cortex audio engine or Neural DSP amp/effects DSP implementation was verified in this search. This is a search result, not proof that none exists anywhere. A public device catalog, preset parser or USB control protocol does not supply the device algorithms or their weights.

## Build work

The source now optionally builds virtualcortex_nam_render when VC_NAM_SOURCE_DIR points at the pinned upstream checkout with its Eigen submodule. The Windows workflow checks out that source, compiles the offline backend and runs numerical and upstream-model smoke tests alongside the existing app tests. The GUI and installer remain version 6.12.0, because this checkpoint does not add live NAM playback to them. The standalone experimental ZIP is packaged separately with license notices.

See [NAM-BACKEND.md](VirtualCortex/NAM-BACKEND.md) for supported inputs and the remaining live-integration gates. Build results must be checked before describing the new executable as compiled or validated.
