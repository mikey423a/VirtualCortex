# 6.29 routing — verified Windows milestone

Code 152ce1480cfeb38e04df615029ce4c988867bf25 passed Windows CI run 43840483327. Actual standalone EXE/VST3 compiled with ASIO; all 45 regressions passed, pluginval passed, standalone remained running for 10 seconds, installer deployment including installed VST3 hash comparison and uninstall passed. Downloaded archives and four packaged hashes matched.

Native 2560x1440 active/disabled custom-grid and 822x600 editor-visible screenshots inspected. Vertical source tracks are separated and routing wires/captions dim with grid off. DSP, presets, four rounded-square rows and full-width layout preserved. Some inactive grid controls still look enabled; this is not a complete inactive-state redesign.

Next: selectable/highlighted connections, clearer inactive controls, stateful audio tests before transition changes, multiple injection points, hardware buses and scene routing. Main should only advance through verified commits; refresh heads and check active work before edits. No profiling or looper additions. No proprietary DSP/catalog equivalence claim. Windows Server 2022 CI is not Windows 11 hardware validation; Anubis and listening tests remain user-side. Installer unsigned.
