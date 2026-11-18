# 6.38 destination-entry menu — candidate

Right-click a Custom row mixer wire and choose Destination entry — ALL row N inputs, then Enter at A–H. Submenu repeats that it affects ALL inputs to the destination row. Source masks, source taps and row mix settings are preserved. Save preset to persist. Same entry is a no-op; stale routing/recall, invalid indices/ranges, absent connections and non-custom mode are rejected under existing locks.

Native tests cover all eight entries with multiple incoming sources, unrelated settings, rejection cases, no-op revisions, focus clearing and state persistence at 2560x1440/822x600. Entry-H screenshots added. Physical OS popup/menu interaction remains unverified. No DSP algorithms, parameter IDs, preset schema or default preset sound changed. Audio transition/tail behavior is unchanged; no click-free claim.

Version 6.38.0. Windows compilation, regression/pluginval/startup/installer/hash checks and screenshot inspection pending. Do not promote before verification; latest delivered release is 6.37.

Next: compatible effects-quality improvements, audio topology transitions and native OS interaction. No profiling or additional looper work. Windows11/Anubis hardware and subjective listening remain user-side.
