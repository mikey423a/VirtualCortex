# Virtual Cortex 6.48.0 verification

Stereo Reverb in the post chain adds optional Tail Detail. The late wet projection uses cubic interpolation while tank feedback keeps the existing linear read. Early reflections and pre-delay use their existing paths. The output tap blends over 5 ms when toggled. The new host parameter is appended at index 137 and defaults off in older presets.

The initial Windows run [44493899387](https://github.com/mikey423a/VirtualCortex/actions/runs/44493899387) compiled both products and passed 47 of 48 regressions. The host parameter contract still expected 137 parameters. Its correction requires a total of 138 and explicitly checks the new parameter's index.

The retry compiled commit `1a3d9a62fa97b8b365fea062715dd15d33d6af0b` in [Windows run 44495608391](https://github.com/mikey423a/VirtualCortex/actions/runs/44495608391). The Windows x64 standalone and VST3 compiled. All 48 regressions, pluginval strictness 5, a ten-second standalone startup check, installer install/uninstall, and package hash checks passed. Downloaded artifact archives and the four packages matched their reported hashes. The native Tail Detail screenshots at 2560x1440 and 822x600 were reviewed, including the checkbox, four-row grid and effect control placement.

Local GCC and Windows regressions checked 44.1, 48 and 96 kHz. Live toggles matched the blend of two continuously rendered references and returned exactly to the original output after the fade, including during tail decay. Native UI binding, new and legacy preset behavior and callback allocations were checked.

```
Virtual Cortex.exe 7F090C2692B812D3FF7517EAAD61D1A9A7A13F1668CB8CDA59ED6B7F81F1E4BB
VirtualCortex-6.48.0-VST3-x64.zip ECF9186D6B0310B5DE388C387FA95C80F27D4FFBA460C667D9E77F867D95F4D8
VirtualCortex-6.48.0-Windows-x64-Setup.exe BECCCB073EB9E30C33644C2EB012DCF24C99553074C2EAEBA61F32BBB7F5B03C
VirtualCortex-NAM-Backend-Experimental-Windows-x64.zip F63FDA75B58B7BA658B94F96DC5718C53540719DFD92BA86BCB5432B25D80EE0
REVERB_DETAIL rate=44100 absolute_sample_difference_sum=1.17564 retained_tail_energy=33.4139
REVERB_DETAIL rate=48000 absolute_sample_difference_sum=1.2279 retained_tail_energy=38.3344
REVERB_DETAIL rate=96000 absolute_sample_difference_sum=1.48017 retained_tail_energy=56.2153
```

Follow-up: characterize spectral differences and listen to sustained guitar and modulated tails with Tail Detail enabled. The regression verifies output and state continuity. Listening, Windows 11 hardware and Anubis ASIO checks remain open. The installer is unsigned.
