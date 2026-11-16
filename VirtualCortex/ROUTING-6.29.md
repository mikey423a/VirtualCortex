# 6.29 routing candidate — not yet verified

Base d1864123b50848ecc0aac54cf68d0052d8a35806 passed Windows CI run 43834660579, including compilation/regressions, pluginval/standalone and installer verification with the new installed VST3 SHA256 comparison.

This candidate offsets vertical connection tracks by source row to reduce apparent shared buses at identical tap columns. Routing wires and expanded source captions dim when the grid engine is off. Added a disabled-grid native screenshot scenario. DSP, presets, four rounded-square rows and full-width layout are unchanged.

Pending: inspect this commit's Windows CI, regression/pluginval/startup/install/uninstall logs and actual native screenshots at 2560x1440 and compact sizes, particularly rail crossings and disabled-grid state. Do not promote or distribute as verified until these pass. Main remains verified 6.28. Next work: connection selection, transition behavior with stateful tests, multiple injection points, hardware buses and scene routing. No profiling or looper additions. Anubis remains user-side.
