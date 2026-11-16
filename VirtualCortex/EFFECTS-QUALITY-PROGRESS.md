# Effects quality work — candidate

User priority: improve original effects toward professional guitar/studio quality. No measured Strymon/Eventide/Neural DSP equivalence claimed.

StereoDelay candidate caches unchanged low/high-cut filter coefficients, invalidated on prepare. Coefficients still update at the same 16-sample cadence during smoothing. Intended result: identical audio with fewer trigonometric calculations at steady settings. Numerical old/new render comparison, restart/automation coverage and CPU measurements remain required; no measured speedup or audio improvement is claimed yet.

Existing native UI CI 43960242244 failed because the selected source-rail point was intercepted by a child block. Test now uses the horizontal routing gutter. Keep the interception assertion; inspect resulting native screenshots before promotion.

Next quality milestones: benchmark steady-state and automated delay CPU; frequency/decay measurements of the FDN across rates and settings; measure modulation and time-transition artifacts; level-matched musical listening. Fractional interpolation affects frequency response (Julius O. Smith, Physical Audio Signal Processing, https://www.dsprelated.com/freebooks/pasp/Delay_Line_Signal_Interpolation.html); do not replace interpolation blindly inside a feedback loop. Preserve legacy preset sound or introduce explicit optional modes for sonic changes.

Windows CI pending for this candidate. Main and delivered 6.32 unchanged. No new installer until regression/pluginval/startup/install/uninstall/hash checks and native screenshot review pass. Anubis testing remains user-side.
