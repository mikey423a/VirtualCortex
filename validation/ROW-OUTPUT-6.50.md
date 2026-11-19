# Virtual Cortex 6.50.0 verification

Custom Routing adds **Smooth row outputs (5 ms)**. Each OUT switch ramps its row's contribution to the final grid mix while the row's effects and downstream taps continue running. With Average enabled, the divisor follows the summed output weights and has a floor of one so the final row fades to silence. The setting is stored per preset and defaults off when missing. It applies to OUT switches in Custom Routing. Disabling a whole row and changing a connection's source membership retain their existing behavior.

The fixture sends sustained asymmetric stereo input through a Grid Stereo Delay feeding a Grid Stereo Reverb. Separate output references check averaging and unity summing. The tests reverse a fade, mute both outputs, then restore only the downstream output and compare settled samples exactly against the immediate-switch reference. The tail survives the muted interval. At 48 kHz with Average enabled, the largest first switched-sample step across the two channels dropped from 0.0561698 to 0.00177773. This measurement is specific to the fixture. The local portable routing suite also passed ASan and UBSan with leak detection disabled because the environment prevents LeakSanitizer from running.

[Windows CI 44512215846](https://github.com/mikey423a/VirtualCortex/actions/runs/44512215846) compiled `bdc6fd88b98a0a7d880107bfd4891d945837bfc2` with ASIO. All 48 regressions, pluginval strictness 5, ten-second standalone startup, installation and uninstallation passed. The downloaded CI archives and four package hashes matched. Native routing screenshots at 2560 × 1440 and 822 × 600 show the four smoothing controls. The native test also verified toggle binding and legacy preset recall. The 2560 × 1440 grid capture shows all four rows and the existing wire layout after closing the routing panel.

```text
VirtualCortex-Windows-x64 archive 2b5d2baa42f03fcfdc904c67e5b80c1fc4588f3a6438b8ed51785413981c564e
VirtualCortex-build-logs archive 6ed24919ef616cbc4174a0f6320a99fcdc91e3f365da6cf5657e0368a88590a4
Virtual Cortex.exe 3AE03DC212FFBD7B7F40BF004AC0C00C0CFD0127F6009C72C2797B75B092E2E6
VirtualCortex-6.50.0-VST3-x64.zip 35E652C9F9BF31D5A381DB3D41AB92CABA0F53B905640DC405C01CC4176C42FC
VirtualCortex-6.50.0-Windows-x64-Setup.exe 51FEA654567EA163942D92B53F5415A5ED5AFA20218D1F6F664F6AA967EFC64F
VirtualCortex-NAM-Backend-Experimental-Windows-x64.zip A06E3FACE3C40FB8A7C3CDBBF0F9725A7D7324538AFA21417E834E8850F28BAE
GRID_REVERB_DETAIL rate=44100 abs_difference_sum=0.324612 retained_tail_energy=9.74032
GRID_REVERB_DETAIL rate=48000 abs_difference_sum=0.569882 retained_tail_energy=14.9925
GRID_REVERB_DETAIL rate=96000 abs_difference_sum=0.417226 retained_tail_energy=10.0788
ROW_OUTPUT rate=44100 average=0 immediate_step=0.0933564 blended_step=0.00390035 restored_tail_energy=14.0028
ROW_OUTPUT rate=44100 average=1 immediate_step=0.0664246 blended_step=0.00188028 restored_tail_energy=14.0028
ROW_OUTPUT rate=48000 average=0 immediate_step=0.0968308 blended_step=0.00371747 restored_tail_energy=10.1604
ROW_OUTPUT rate=48000 average=1 immediate_step=0.0561698 blended_step=0.00177773 restored_tail_energy=10.1604
ROW_OUTPUT rate=96000 average=0 immediate_step=0.0135531 blended_step=0.00333502 restored_tail_energy=5.95679
ROW_OUTPUT rate=96000 average=1 immediate_step=0.0270148 blended_step=0.00166983 restored_tail_energy=5.95679
POST_CHAIN_MODE rate=48000 immediate_step=0.0732387 smooth_step=0.00214295 immediate_tail_energy=65.4064 smooth_tail_energy=65.1658
```

Next measure live source-connect and disconnect edits, including normalized multi-source merges with downstream delay and reverb. Keep the existing Tail Detail options and use level-matched guitar listening to assess them. The installer is unsigned. Windows 11 listening and Anubis ASIO testing remain user-side.
