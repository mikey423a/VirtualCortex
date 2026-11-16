# 6.31 inactive-grid display — verified

Code 1691ee9213839982ca7cf99e48795f18e9ec4a4d passed Windows CI run 43870960390: actual EXE/VST3 compilation, all 45 regressions, pluginval strictness 5, 10-second standalone startup, installer install/uninstall including deployed VST3 hash check. Downloaded archives and four packaged hashes matched. Native disabled-grid screenshots inspected at 2560x1440 and 822x600.

Grid blocks dim with the grid off; tooltips identify grid engine off. OUT highlights require active grid and row. Native tests cover off/restored visual state, editability and unchanged saved configuration. DSP, presets, full-width layout and four rounded-square rows preserved.

Known polish issue: inactive blocks still use generic BYPASSED captions (or SKIPPED for routing prefixes); tooltip is more specific. Add a distinct GRID OFF caption next. Then individual wire selection and actual-effect stateful transition coverage. No topology crossfade, multiple injection points, hardware buses or scene routing completion is claimed. No profiling/looper development. Installer unsigned; Anubis, Windows 11 interactive and listening tests remain user-side.

Archive SHA256: diagnostics ddc88f348f311863b1f0d78276179322f0979abff87d691770399ed6de2af927; Windows b6d700d26a39a1b73c6ee06d2005ad7bab12e7f10066235fc111de98c92e2c12. Refresh heads/active work and use guarded updates before continuing.
