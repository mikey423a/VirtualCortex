# 6.31 inactive-grid display — candidate

Based on verified 6.30 at c372f45ef0a80d76419184ae77ff7e3f139ddbb0; no active or queued CI before editing. Grid blocks now dim when the grid engine is off, with explicit grid-engine-off tooltips. Custom OUT highlights require the grid and row to be enabled. Controls remain editable; this does not bypass devices or alter saved row outputs.

Native UI regressions assert inactive/restored block and OUT states, editability, and unchanged saved configuration. Added compact disabled-grid capture. Windows CI and actual native screenshot review are pending. Do not promote or deliver as verified until compilation, regressions, pluginval, startup, installer/hash checks and screenshots pass. Main/delivered build remains 6.30. DSP, preset format, four rounded-square rows/full-width 2560x1440 preserved.

Next: verify this candidate and deliver if successful; then individual connection selection and actual-effect stateful transition tests. No profiling or looper additions. Anubis/listening remain user-side.
