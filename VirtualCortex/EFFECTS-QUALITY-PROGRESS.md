# Effects quality work — candidate

User priority: improve original effects toward professional guitar/studio quality. No measured Strymon/Eventide/Neural DSP equivalence claimed.

StereoDelay candidate caches unchanged low/high-cut filter coefficients, invalidated on prepare. Coefficients still update at the same 16-sample cadence during smoothing. Intended result: identical audio with fewer trigonometric calculations at steady settings. Numerical old/new render comparison, restart/automation coverage and CPU measurements remain required; no measured speedup or audio improvement is claimed yet.

Existing native UI CI 43960242244 failed because the selected source-rail point was intercepted by a child block. Test now uses the horizontal routing gutter. Keep the interception assertion; inspect resulting native screenshots before promotion.

Next quality milestones: benchmark steady-state and automated delay CPU; frequency/decay measurements of the FDN across rates and settings; measure modulation and time-transition artifacts; level-matched musical listening. Fractional interpolation affects frequency response (Julius O. Smith, Physical Audio Signal Processing, https://www.dsprelated.com/freebooks/pasp/Delay_Line_Signal_Interpolation.html); do not replace interpolation blindly inside a feedback loop. Preserve legacy preset sound or introduce explicit optional modes for sonic changes.

Windows CI pending for this candidate. Main and delivered 6.32 unchanged. No new installer until regression/pluginval/startup/install/uninstall/hash checks and native screenshot review pass. Anubis testing remains user-side.


CI 43971152595 compiled but failed native_editor_layout at interception assertion. Follow-up explicitly makes the editor visible for getComponentAt, reports unexpected child identity, and retains assertion. Adds frozen pre-cache delay fixture and exact output comparisons at three rates, automated filters/time/mode/bypass and prepare restarts. CI pending; CPU measurements still pending. No promotion or new installer.


Verified CI evidence (Nov 16 Toronto): commit 8aa43b78e1b29074f068b11b2eb2316a956b7dca, run 43976543322, job 133681856802 passed all 48 tests, including exact delay_cache_equivalence and native_editor_layout, pluginval strictness 5, standalone startup and installer verification. GUI interception failure resolved after explicitly making the test editor visible. Artifact IDs: Windows 14282005839, diagnostics 14281396141. Archive digests are metadata only; downloads not independently checked this run.

Executor unavailable in this run: native PNGs cannot be unpacked/viewed, so no GUI signoff, promotion or installer delivery. Existing 6.32 remains verified release. Added informational paired steady-state CPU benchmark: 48k warmup, six alternating 480k-frame passes, median times and equal checksum assertion; wall-clock speedup is not a pass/fail threshold. Benchmark results pending new CI. Still require automated CPU measurements, screenshots, topology reset assertions and release version bump.
