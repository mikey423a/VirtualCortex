# 6.54 Gate editor layout

Opening a grid Gate gives all four grid rows more vertical space and puts Threshold, Attack, Hold and Release across one full-width row. The Gate mode switch sits in the editor header.

The layout and routing hit test share the same row geometry. Native regressions exercise wire selection in the gutter, selected-block removal, and the order of the fourth row, editor header, and knobs at 2560 × 1440 and 822 × 600. The wider layout currently applies to the grid Gate. Other editors use their existing proportions.

[Windows CI 44722912165](https://github.com/mikey423a/VirtualCortex/actions/runs/44722912165), job 136242937947, built d8bc560f1c735a6c0b808169bfbfae50270a7303. All 48 regressions, pluginval strictness 5, ten-second standalone startup, and installer install/uninstall passed. Both downloaded archive digests and four package hashes matched. Native screenshots at both sizes were reviewed.

Setup SHA-256 67EB3E5A6E78BD264A2D2B9402764E219CDE9A24F3B3AC7D8DB1A63A25A8AFE7  
VST3 ZIP SHA-256 84399659C09C7023B529979B21374B8262E6F6996AAB59DE67393F452A79C877

Next work includes examining combined routing taps and source connections under rapid edits and improving the editor layout for other compact effects. Physical Anubis and Windows 11 tests remain user-side. Installer unsigned.
